#pragma once
#include "Tank.h"
#include "IPlayer.h"
#include <string>
class Tank;

constexpr auto STRING_SIZE = 30;

class Player : public IPlayer, public std::enable_shared_from_this<Player> {
public:
	struct PlayerData {
		char name[STRING_SIZE];
		unsigned int score;
	} playerData;
private:
	std::shared_ptr<Tank> tankObject;
	unsigned int score;
	unsigned int tanksDestroyed;
	unsigned int tanksDestroyedRequired = 4;
public:
	Player(std::shared_ptr<Tank> tankObject, std::string& playerName);

	void render();
	void renderGui();
	unsigned int getScore();
	void increaseScore(unsigned int score);
	void increaseTanksDestroyed(unsigned int count);
	unsigned int getTanksDestroyed();
	std::shared_ptr<Tank> getObject();

	void destroyTankObject();
	void changeTankObject(std::shared_ptr<Tank>);
	
	void writeScoreToFile(std::string& path);

	~Player();
};
