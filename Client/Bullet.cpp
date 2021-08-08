#include "Bullet.h"
#include "Brick.h"

Bullet::Bullet(SDL_Renderer* renderer, const double x, const double y, const int w, const int h, const int rotation, LevelData* levelData, std::weak_ptr<IPlayer> author, double speed, int damage) : GameObject(renderer, x, y, w, h, levelData) {
	isDestroyedOnContact = 1;
	currentImgState = 0;
	SDL_Surface* grid = IMG_Load("498_Tanks_sprites/gfx/bullet/bullet02.png");
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTextureFromSurface(renderer, grid);
	SDL_FreeSurface(grid);
	isCollided = true;
	this->author = author;
	position->rotation = rotation;
	this->speed = speed;
	this->damage = damage;
}

void Bullet::moveForward() {
	currentImgState = (currentImgState + 1) % 8;
	blit.x = currentImgState * 32;
	int moveX = 0;
	int moveY = 0;
	switch (position->rotation) {
	case 0:
		moveX = speed;
		moveY = 0;
		break;
	case 90:
		moveX = 0;
		moveY = speed;
		break;
	case 180:
		moveX = -speed;
		moveY = 0;
		break;
	case -90:
	case 270:
		moveX = 0;
		moveY = -speed;
		break;
	}
	moveObject(position->rect.x + moveX, position->rect.y + moveY, position->rotation);
}

void Bullet::render() {
	moveForward();
	GameObject::render();
}

void Bullet::collidesWith(std::shared_ptr<GameObject> t) {
	if (!author.expired() && t == std::dynamic_pointer_cast<GameObject>(author.lock().get()->getObject()))
		return;
	if (dynamic_cast <Tank*> (t.get()) != nullptr) {
		std::cout << "Tank collider\n";
		Tank* collidedWith = dynamic_cast <Tank*> (t.get());
		collidedWith->getDamage(damage);
		if (!author.expired() && collidedWith->isToBeDestructed()) {
			author.lock().get()->increaseScore(collidedWith->getTankCost());
			author.lock().get()->increaseTanksDestroyed(1);
		}
	}
	else if (dynamic_cast <Brick*> (t.get()) != nullptr) {
		std::cout << "Brick collider\n";
		Brick* collidedWith = dynamic_cast <Brick*> (t.get());
		collidedWith->getDamage(damage);
	}

	GameObject::collidesWith(t);
}