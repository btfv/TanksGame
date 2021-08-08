#pragma once
#include "Tank.h"

class Tank;

class TankV1 : public Tank {
public:
	TankV1(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData, bool spawnInviolability);
};