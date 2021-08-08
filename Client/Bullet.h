#pragma once
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "Player.h"
#include "LevelData.h"
struct LevelData;
class GameObject;
class Level;
class Player;
class Tank;
class Bot;
class IPlayer;

class Bullet : public GameObject {
protected:
	std::weak_ptr<IPlayer> author;
	int damage;
	double speed;
public:
	Bullet(SDL_Renderer* renderer, const double x, const double y, const int w, const int h, const int rotation, LevelData* levelData, std::weak_ptr<IPlayer> author, double speed, int damage);
	void moveForward();
	void render();
	void collidesWith(std::shared_ptr<GameObject> t);
};