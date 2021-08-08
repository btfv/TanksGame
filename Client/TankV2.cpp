#include "TankV2.h"

TankV2::TankV2(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData, bool spawnInviolability) : Tank(renderer, x, y, w, h, levelData, spawnInviolability) {
	SDL_Surface* grid = IMG_Load("498_Tanks_sprites/gfx/tanks/v2.png");
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTextureFromSurface(renderer, grid);
	SDL_FreeSurface(grid);

	defaultTankSpeed = 1.6;
	defaultBulletDamage = 100;
	defaultBulletSpeed = 3;
	reloadDuration = 1000;
	changeSpeedDuration = 10 * 1000;
	tankCost = 300;
	this->tankSpeed = defaultTankSpeed;
	hp = 200;
}