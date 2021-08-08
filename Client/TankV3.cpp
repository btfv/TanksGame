#include "TankV3.h"

TankV3::TankV3(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData, bool spawnInviolability) : Tank(renderer, x, y, w, h, levelData, spawnInviolability) {
	SDL_Surface* grid = IMG_Load("498_Tanks_sprites/gfx/tanks/v3.png");
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTextureFromSurface(renderer, grid);
	SDL_FreeSurface(grid);

	defaultTankSpeed = 1.8;
	defaultBulletDamage = 300;
	defaultBulletSpeed = 5;
	reloadDuration = 2000;
	changeSpeedDuration = 10 * 1000;
	tankCost = 600;
	this->tankSpeed = defaultTankSpeed;
	hp = 250;
}