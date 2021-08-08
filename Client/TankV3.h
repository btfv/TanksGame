#pragma once
#include "Tank.h"

class Tank;

class TankV3 : public Tank {
public:
	TankV3(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData, bool spawnInviolability);
};