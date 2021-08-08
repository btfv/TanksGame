#include "Jaguar.h"

Jaguar::Jaguar(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData) : GameObject(renderer, x, y, w, h, levelData) {
	currentImgState = 0;

	SDL_Surface* grid = IMG_Load("498_Tanks_sprites/soda.png");
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTextureFromSurface(renderer, grid);
	SDL_FreeSurface(grid);
	isCollided = false;

	blit = { 0, 0, 700, 932 };
}
void Jaguar::render() {
	if (tick % 150 == 0) {
		position->rotation = (position->rotation == 0) ? 30 : 0;
	}
	GameObject::render();
	tick = (tick + 1) % 1000;
}
void Jaguar::collidesWith(std::shared_ptr<GameObject> secondObject) {
	if (std::dynamic_pointer_cast<Tank>(secondObject) != nullptr) {
		std::dynamic_pointer_cast<Tank>(secondObject).get()->changeSpeed(coefficientSpeed);
	}
	destroyObject();
};