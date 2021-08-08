#include "TankPLA2.h"

TankPLA2::TankPLA2(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData, bool spawnInviolability) : Tank(renderer, x, y, w, h, levelData, spawnInviolability) {
	SDL_Surface* grid = IMG_Load("498_Tanks_sprites/gfx/tanks/pla2.png");
	SDL_DestroyTexture(texture);
	texture = SDL_CreateTextureFromSurface(renderer, grid);
	SDL_FreeSurface(grid);

	defaultTankSpeed = 2;
	defaultBulletDamage = 100;
	defaultBulletSpeed = 8;
	reloadDuration = 450;
	changeSpeedDuration = 10 * 1000;
	tankCost = 300;
	hp = 500;
	this->tankSpeed = defaultTankSpeed;
}