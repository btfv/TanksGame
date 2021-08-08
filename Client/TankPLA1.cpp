#include "TankPLA1.h"

TankPLA1::TankPLA1(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData, bool spawnInviolability) : Tank(renderer, x, y, w, h, levelData, spawnInviolability) {
	defaultTankSpeed = 1.5;
	defaultBulletDamage = 50;
	defaultBulletSpeed = 4;
	reloadDuration = 400;
	changeSpeedDuration = 10 * 1000;
	tankCost = 100;
	hp = 250;
	this->tankSpeed = defaultTankSpeed;
}