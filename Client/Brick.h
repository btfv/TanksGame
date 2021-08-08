#pragma once
#include <SDL/SDL_render.h>
#include <SDL/SDL_image.h>
#include "GameObject.h"

class Brick : public GameObject {
protected:
	int blockHp = 100;
public:
	Brick(SDL_Renderer* renderer, const double x, const double y, const int w, const int h, LevelData* levelData) : GameObject(renderer, x, y, w, h, levelData){
		this->isCollided = true;
		this->texture = texture;
		this->isDestroyedOnContact = false;
		SDL_Surface* surface = IMG_Load("498_Tanks_sprites/gfx/block/world03/block_05.png");
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
	}
	void getDamage(const int& damage) {
		blockHp -= damage;
		if (blockHp <= 50) {
			blit.x += 32;
		}
		if (blockHp <= 0) {
			this->destroyObject();
		}
	}
};