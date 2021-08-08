#pragma once
#include "Tank.h"

class Tank;

class TankPLA1 : public Tank {
public:
	TankPLA1(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData, bool spawnInviolability);
};