#pragma once
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "GameObject.h"

class Explosion : public GameObject {
private:
	unsigned int ticks = 0;
public:
	Explosion(SDL_Renderer* renderer, const double x, const double y, const int w, const int h, LevelData* levelData) : GameObject(renderer, x, y, w, h, levelData) {

		currentImgState = 0;

		SDL_Surface* grid = IMG_Load("498_Tanks_sprites/gfx/effect/explosion.png");
		SDL_DestroyTexture(texture);
		texture = SDL_CreateTextureFromSurface(renderer, grid);
		SDL_FreeSurface(grid);
		isCollided = false;

		blit = { 64 * 3, 64 * 3, 64, 64 };
	}
	virtual void render() {
		if (currentImgState > 15) {
			destroyObject();
			return;
		}
		blit.x = 64 * (3 - currentImgState % 4);
		blit.y = 64 * (3 - (currentImgState % 16) / 4);
		currentImgState++;
		GameObject::render();
	}
};