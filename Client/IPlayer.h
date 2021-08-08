#pragma once
class Tank;
class IPlayer {
public:
	virtual void render() = 0;
	virtual std::shared_ptr<Tank> getObject() = 0;
	virtual void increaseScore(unsigned int score) = 0;
	virtual unsigned int getScore() = 0;
	virtual void increaseTanksDestroyed(unsigned int count) = 0;
	virtual unsigned int getTanksDestroyed() = 0;
};
