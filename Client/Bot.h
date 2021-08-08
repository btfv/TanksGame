#pragma once
#include <memory>
#include <utility>
#include "Tank.h"
#include "Level.h"
#include "Player.h"
#include "Timer.h"
class Timer;
class Level;
class Player;
#include "IPlayer.h"
#include "LevelData.h"
#include "AStar.h"

enum class BotMode { RandomMovement = 1, PlayerMovement = 2 };

class Tank;
class Bot : public IPlayer, public std::enable_shared_from_this<Bot> {
private:
	//std::unique_ptr<Timer> actionsDelay;
	BotMode mode;
	int direction;
	int ticks;
	std::shared_ptr<Tank> tankObject;
	unsigned int score;
	AStar::CoordinateList way;
	bool isAutoGuidanced;
	std::unique_ptr<Timer> changeDirTimer;
	unsigned int tanksDestroyed;
public:
	Bot(std::shared_ptr<Tank> tankObject);

	void render();
	void randomMovement();
	void playerMovement();
	std::shared_ptr<Tank> getObject();
	void increaseScore(unsigned int score);
	unsigned int getScore();
	~Bot();
	void moveByWay();
	void changeMode();
	void checkBestWayToPlayer();
	void increaseTanksDestroyed(unsigned int count);
	unsigned int getTanksDestroyed();
};
