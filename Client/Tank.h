#pragma once
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <memory>

#include "GameObject.h"
#include "Bullet.h"
#include "Explosion.h"
#include "Player.h"
#include "Bot.h"
#include "Timer.h"

class Player;
class Bot;
class Explosion;
class Timer;

class Bullet;

class Tank : public GameObject {
protected:
	double defaultTankSpeed;
	int defaultBulletDamage;
	double defaultBulletSpeed ;
	unsigned int reloadDuration;
	unsigned int changeSpeedDuration;
	unsigned int spawnInviolabilityDuration;
	unsigned int tankCost;
	int bulletDamage;
	double tankSpeed;
	int hp;
	std::unique_ptr<Timer> timer;
	std::unique_ptr<Timer> strikeTimer;
	std::unique_ptr<Timer> spawnTimer;
	SDL_Texture* waveTexture;
	SDL_Rect waveTextureRect;
public:
	Tank(SDL_Renderer* renderer, const int x, const int y, const int w, const int h, LevelData* levelData, bool spawnInviolability);
	bool moveUp();
	bool moveRight();
	bool moveLeft();
	bool moveDown();
	virtual void render();
	void strike(std::weak_ptr<IPlayer> author);
	void getDamage(const int& damage);
	void setHp(int hp);
	~Tank(){}
	void destroyObject();
	void changeSpeed(double coefficient);
	int getHp() { return hp; }
	unsigned int getTankCost() { return tankCost;  }
	bool isBrickOrEnemyForward();
	void resetTank();
};