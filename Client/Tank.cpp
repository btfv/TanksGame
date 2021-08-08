#pragma once

#include <iostream>

#include "Tank.h"
#include "Level.h"


class Player;
class Bot;

Tank::Tank(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData, bool spawnInviolability) : GameObject(renderer, x, y, w, h, levelData) {
	currentImgState = 0;
	defaultTankSpeed = 2.5;
	defaultBulletDamage = 50;
	defaultBulletSpeed = 8;
	reloadDuration = 300;
	changeSpeedDuration = 10 * 1000;
	spawnInviolabilityDuration = spawnInviolability ? 2 * 1000 : 0;
	tankCost = 100;
	hp = 150;

	SDL_Surface* grid = IMG_Load("498_Tanks_sprites/gfx/tanks/pla1.png");
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTextureFromSurface(renderer, grid);
	SDL_FreeSurface(grid);
	isCollided = true;
	isDestroyedOnContact = 0;
	tankSpeed = defaultTankSpeed;
	timer = nullptr;
	strikeTimer = nullptr;
	spawnTimer = std::unique_ptr<Timer>(new Timer(spawnInviolabilityDuration));

	SDL_Surface* waveSurface = IMG_Load("498_Tanks_sprites/gfx/effect/ecircle.png");
	waveTexture = SDL_CreateTextureFromSurface(renderer, waveSurface);
	SDL_FreeSurface(waveSurface);
}
bool Tank::moveUp() {
	currentImgState = (currentImgState + 1) % 4;
	blit.x = currentImgState * 32;
	return moveObject(position->rect.x, position->rect.y - tankSpeed, -90);
}
bool Tank::moveRight() {
	currentImgState = (currentImgState + 1) % 4;
	blit.x = (currentImgState + 1) % 4 * 32;
	return moveObject(position->rect.x + tankSpeed, position->rect.y, 0);
}
bool Tank::moveLeft() {
	currentImgState = (currentImgState + 1) % 4;
	blit.x = (currentImgState + 1) % 4 * 32;
	return moveObject(position->rect.x - tankSpeed, position->rect.y, 180);
}
bool Tank::moveDown() {
	currentImgState = (currentImgState + 1) % 4;
	blit.x = (currentImgState + 1) % 4 * 32;
	return moveObject(position->rect.x, position->rect.y + tankSpeed, 90);
}
void Tank::render() {
	SDL_Rect rect;
	rect.x = static_cast<int>(position->rect.x);
	rect.y = static_cast<int>(position->rect.y);
	rect.w = position->rect.w;
	rect.h = position->rect.h;
	if (strikeTimer != nullptr && strikeTimer->isExpired()) {
		strikeTimer.reset();
	}
	if (timer != nullptr && timer->isExpired()) {
		tankSpeed = defaultTankSpeed;
		timer.reset();
	}
	/*if (timer != nullptr) {
		SDL_RenderCopyEx(renderer, texture, &blit, &(position->rect), position->rotation, nullptr, position->flip);
	}*/
	if (timer != nullptr && SDL_GetTicks() % 8 == 0) {
		SDL_Rect rect = { static_cast<int>(position->rect.x), static_cast<int>(position->rect.y), position->rect.w, position->rect.h };
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &rect);
	}
	SDL_RenderCopyEx(renderer, texture, &blit, &(rect), position->rotation, nullptr, position->flip);
	if (!spawnTimer->isExpired()) {
		waveTextureRect = { static_cast<int>(position->rect.x) - levelData->tileWidth / 2, static_cast<int>(position->rect.y) - levelData->tileHeight / 2, levelData->tileWidth * 2, levelData->tileHeight * 2 };
		SDL_RenderCopyEx(renderer, waveTexture, NULL, &(waveTextureRect), SDL_GetTicks() % 360, NULL, SDL_FLIP_NONE);
	}
}

void Tank::getDamage(const int& damage) {
	if (!spawnTimer->isExpired())
		return;
	hp -= damage;
	if (hp <= 0) {
		destroyObject();
	}
}

void Tank::strike(std::weak_ptr<IPlayer> author) {
	if (strikeTimer != nullptr)
		return;
	int xRot = 0;
	int yRot = 0;

	switch (position->rotation) {
	case 0:
		xRot = 32;
		break;
	case 90:
		yRot = 32;
		break;
	case 180:
		xRot = -32;
		break;
	case 270:
		yRot = -32;
		break;
	case -90:
		yRot = -32;
		break;
	}
	yRot += 10;
	
	levelData->level->createGameObject(std::shared_ptr<Bullet>(new Bullet(renderer, position->rect.x + xRot, position->rect.y + yRot, 32, 10, position->rotation, levelData, author, defaultBulletSpeed, defaultBulletDamage)));
	strikeTimer = std::unique_ptr<Timer>(new Timer(reloadDuration));
};
void Tank::destroyObject() {
	GameObject::destroyObject();
	levelData->level->createGameObject(std::shared_ptr<Explosion>(new Explosion(renderer, position->rect.x, position->rect.y, 32, 32, levelData)));
}

void Tank::changeSpeed(double coefficient) {
	if (tankSpeed != defaultTankSpeed) {
		return;
	}
	timer = std::unique_ptr<Timer>(new Timer(changeSpeedDuration));
	tankSpeed *= coefficient;
}

void Tank::setHp(int hp) {
	if (hp < 0) {
		return;
	}
	this->hp = hp;
}

bool Tank::isBrickOrEnemyForward() {
	std::vector <std::shared_ptr<Brick>> bricks = levelData->level->getBricks();
	std::shared_ptr <Player> player = levelData->level->getPlayer();
	switch (position->rotation) {
	case 0:
		for (int i = position->rect.x; i < levelData->windowWidth; i += levelData->tileWidth) {
			for (int j = 0; j < bricks.size(); j++) {
				std::shared_ptr<GameObject::Position> pos2 = bricks[j]->getPosition();
				double leftX = i;
				double rightX = leftX + position->rect.w;
				double leftY = position->rect.y;
				double rightY = leftY + position->rect.h;
				if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
					||
					(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)
					||
					(pos2->rect.x >= leftX && pos2->rect.x <= rightX)
					||
					(pos2->rect.x + pos2->rect.w >= leftX && pos2->rect.x + pos2->rect.w <= rightX)) {
					if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
						||
						(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)
						||
						(pos2->rect.y >= leftY && pos2->rect.y <= rightY)
						||
						(pos2->rect.y + pos2->rect.h >= leftY && pos2->rect.y + pos2->rect.h <= rightY)
						) {
						return true;
					}
				}
				if (player && player->getObject()) {
					pos2 = player->getObject()->getPosition();
					if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
						||
						(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)
						||
						(pos2->rect.x >= leftX && pos2->rect.x <= rightX)
						||
						(pos2->rect.x + pos2->rect.w >= leftX && pos2->rect.x + pos2->rect.w <= rightX)) {
						if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
							||
							(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)
							||
							(pos2->rect.y >= leftY && pos2->rect.y <= rightY)
							||
							(pos2->rect.y + pos2->rect.h >= leftY && pos2->rect.y + pos2->rect.h <= rightY)
							) {
							return true;
						}
					}
				}
			}
		}
		break;
	case 90:
		for (int i = position->rect.y; i < levelData->windowHeight; i += levelData->tileHeight) {
			for (int j = 0; j < bricks.size(); j++) {
				std::shared_ptr<GameObject::Position> pos2 = bricks[j]->getPosition();
				double leftX = position->rect.x;
				double rightX = leftX + position->rect.w;
				double leftY = i;
				double rightY = leftY + position->rect.h;
				if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
					||
					(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)
					||
					(pos2->rect.x >= leftX && pos2->rect.x <= rightX)
					||
					(pos2->rect.x + pos2->rect.w >= leftX && pos2->rect.x + pos2->rect.w <= rightX)) {
					if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
						||
						(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)
						||
						(pos2->rect.y >= leftY && pos2->rect.y <= rightY)
						||
						(pos2->rect.y + pos2->rect.h >= leftY && pos2->rect.y + pos2->rect.h <= rightY)
						) {
						return true;
					}
				}
				if (player && player->getObject()) {
					pos2 = player->getObject()->getPosition();
					if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
						||
						(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)
						||
						(pos2->rect.x >= leftX && pos2->rect.x <= rightX)
						||
						(pos2->rect.x + pos2->rect.w >= leftX && pos2->rect.x + pos2->rect.w <= rightX)) {
						if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
							||
							(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)
							||
							(pos2->rect.y >= leftY && pos2->rect.y <= rightY)
							||
							(pos2->rect.y + pos2->rect.h >= leftY && pos2->rect.y + pos2->rect.h <= rightY)
							) {
							return true;
						}
					}
				}
			}
		}
		break;
	case 180:
		for (int i = position->rect.x; i > 0; i -= levelData->tileWidth) {
			for (int j = 0; j < bricks.size(); j++) {
				std::shared_ptr<GameObject::Position> pos2 = bricks[j]->getPosition();
				double leftX = i;
				double rightX = leftX + position->rect.w;
				double leftY = position->rect.y;
				double rightY = leftY + position->rect.h;
				if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
					||
					(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)
					||
					(pos2->rect.x >= leftX && pos2->rect.x <= rightX)
					||
					(pos2->rect.x + pos2->rect.w >= leftX && pos2->rect.x + pos2->rect.w <= rightX)) {
					if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
						||
						(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)
						||
						(pos2->rect.y >= leftY && pos2->rect.y <= rightY)
						||
						(pos2->rect.y + pos2->rect.h >= leftY && pos2->rect.y + pos2->rect.h <= rightY)
						) {
						return true;
					}
				}
				if (player && player->getObject()) {
					pos2 = player->getObject()->getPosition();
					if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
						||
						(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)
						||
						(pos2->rect.x >= leftX && pos2->rect.x <= rightX)
						||
						(pos2->rect.x + pos2->rect.w >= leftX && pos2->rect.x + pos2->rect.w <= rightX)) {
						if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
							||
							(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)
							||
							(pos2->rect.y >= leftY && pos2->rect.y <= rightY)
							||
							(pos2->rect.y + pos2->rect.h >= leftY && pos2->rect.y + pos2->rect.h <= rightY)
							) {
							return true;
						}
					}
				}
			}
		}
		break;
	case -90:
	case 270:
		for (int i = position->rect.y; i > 0; i -= levelData->tileWidth) {
			for (int j = 0; j < bricks.size(); j++) {
				std::shared_ptr<GameObject::Position> pos2 = bricks[j]->getPosition();
				double leftX = position->rect.x;
				double rightX = leftX + position->rect.w;
				double leftY = i;
				double rightY = leftY + position->rect.h;
				if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
					||
					(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)
					||
					(pos2->rect.x >= leftX && pos2->rect.x <= rightX)
					||
					(pos2->rect.x + pos2->rect.w >= leftX && pos2->rect.x + pos2->rect.w <= rightX)) {
					if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
						||
						(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)
						||
						(pos2->rect.y >= leftY && pos2->rect.y <= rightY)
						||
						(pos2->rect.y + pos2->rect.h >= leftY && pos2->rect.y + pos2->rect.h <= rightY)
						) {
						return true;
					}
				}
				if (player && player->getObject()) {
					pos2 = player->getObject()->getPosition();
					if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
						||
						(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)
						||
						(pos2->rect.x >= leftX && pos2->rect.x <= rightX)
						||
						(pos2->rect.x + pos2->rect.w >= leftX && pos2->rect.x + pos2->rect.w <= rightX)) {
						if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
							||
							(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)
							||
							(pos2->rect.y >= leftY && pos2->rect.y <= rightY)
							||
							(pos2->rect.y + pos2->rect.h >= leftY && pos2->rect.y + pos2->rect.h <= rightY)
							) {
							return true;
						}
					}
				}
			}
		}
		break;
	}
	return false;
}