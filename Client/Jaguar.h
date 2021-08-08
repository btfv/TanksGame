#pragma once
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "GameObject.h"
#include "Tank.h"
class Jaguar : public GameObject {
private:
	double coefficientSpeed = 1.4;
public:
	Jaguar(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData);
	void render();
	void collidesWith(std::shared_ptr<GameObject> secondObject);
};