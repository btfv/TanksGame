#include "Bot.h"

Bot::Bot(std::shared_ptr<Tank> tankObject) {
	this->tankObject = std::move(tankObject);
	mode = (rand() % 3 == 0) ? BotMode::PlayerMovement : BotMode::PlayerMovement;
	this->score = 0;
	this->ticks = 0;
	this->isAutoGuidanced = false;
	this->changeDirTimer = nullptr;
	this->tanksDestroyed = 0;
	this->direction = 0;
}

void Bot::render() {
	tankObject->render();
	switch (mode) {
	case BotMode::RandomMovement : 
		Bot::randomMovement();
		break;
	case BotMode::PlayerMovement:
		Bot::playerMovement();
		break;
	}
	ticks = (ticks + 1) % 1000;
}

void Bot::changeMode() {
	switch (mode) {
	case BotMode::RandomMovement:
		mode = BotMode::PlayerMovement;
		break;
	case BotMode::PlayerMovement:
		mode = BotMode::RandomMovement;
		break;
	}
}

void Bot::randomMovement() {
	if (rand() % 40 == 0 && (!changeDirTimer || changeDirTimer->isExpired())) {
		changeDirTimer.reset(new Timer(2500));
		direction = abs(direction - 1) % 4;
	}
	if (tankObject->isBrickOrEnemyForward()) {
		tankObject->strike(shared_from_this());
	}
	switch (direction) {
	case 0:
		if(!tankObject->moveDown())
			direction = (direction + 2) % 4;
		break;
	case 1:
		if(!tankObject->moveRight())
			direction = (direction + 2) % 4;
		break;
	case 2:
		if(!tankObject->moveUp())
			direction = (direction + 2) % 4;
		break;
	case 3:
		if (!tankObject->moveLeft()) {
			direction = (direction + 2) % 4;
		}
		break;
	}
	if (rand() % 10000 == 0) {
		changeMode();
	}
}

void Bot::increaseScore(unsigned int score) {
	this->score += score;
}
void Bot::increaseTanksDestroyed(unsigned int count) {
	this->tanksDestroyed += count;
}

unsigned int Bot::getScore() {
	return score;
}
unsigned int Bot::getTanksDestroyed() {
	return tanksDestroyed;
}

void Bot::playerMovement() {
	checkBestWayToPlayer();
	moveByWay();
}

void Bot::checkBestWayToPlayer() {
	LevelData* levelData = tankObject->getLevelData();
	std::vector<std::shared_ptr<Player>> players = levelData->players;
	AStar::CoordinateList shortestWay;
	AStar::CoordinateList currentWay;
	for (int i = 0; i < players.size(); i++) {
		currentWay = levelData->level->calculateWayBetweenPoints(players[i]->getObject()->getPosition(), tankObject->getPosition());
		if (shortestWay.size() == 0 || currentWay.size() <= shortestWay.size()) {
			shortestWay = currentWay;
		}
	}
	if (way.size() && shortestWay.size() && (way[0].x != shortestWay[0].x || way[0].y != shortestWay[0].y))
		isAutoGuidanced = false;
	way = shortestWay;
}

void Bot::moveByWay() {
	LevelData* levelData = tankObject->getLevelData();
	/*if ((tankObject->getPosition()->rect.x / levelData->tileWidth == way.back().x && abs(tankObject->getPosition()->rotation) % 180 != 0)
		||
		(tankObject->getPosition()->rect.y / levelData->tileHeight == way.back().y && abs(tankObject->getPosition()->rotation) % 180 == 0)) {
		tankObject->strike(shared_from_this());
	}*/
	if (tankObject->isBrickOrEnemyForward()) {
		tankObject->strike(shared_from_this());
	}
	if (way.size() < 2)
		return;
	int currentCellX = static_cast<int>(tankObject->getPosition()->rect.x) / levelData->tileWidth;
	int currentCellY = static_cast<int>(tankObject->getPosition()->rect.y) / levelData->tileHeight;
	if (!isAutoGuidanced) {
		if (way[0].x * static_cast<double>(levelData->tileWidth) > tankObject->getPosition()->rect.x) {
			tankObject->moveRight();
		}
		else if (way[0].x * static_cast<double>(levelData->tileWidth) < tankObject->getPosition()->rect.x) {
			tankObject->moveLeft();
		}
		else if (way[0].y * static_cast<double>(levelData->tileHeight) > tankObject->getPosition()->rect.y) {
			tankObject->moveDown();
		}
		else if (way[0].y * static_cast<double>(levelData->tileHeight) < tankObject->getPosition()->rect.y) {
			tankObject->moveUp();
		}

		if (abs(tankObject->getPosition()->rect.x - way[0].x * static_cast<double>(levelData->tileWidth)) < 2) {
			tankObject->moveObject(way[0].x * levelData->tileWidth, tankObject->getPosition()->rect.y, tankObject->getPosition()->rotation);
		}
		if (abs(tankObject->getPosition()->rect.y - way[0].y * static_cast<double>(levelData->tileHeight)) < 2) {
			tankObject->moveObject(tankObject->getPosition()->rect.x, way[0].y * levelData->tileHeight, tankObject->getPosition()->rotation);
		}
		isAutoGuidanced = static_cast<int>(tankObject->getPosition()->rect.x) == way[0].x * levelData->tileWidth && static_cast<int>(tankObject->getPosition()->rect.y) == way[0].y * levelData->tileHeight;
		}
	else {
		if (way[1].x > currentCellX) {
			tankObject->moveRight();
		}
		else if (way[1].x < currentCellX) {
			tankObject->moveLeft();
		}
		else if (way[1].y > currentCellY) {
			tankObject->moveDown();
		}
		else if (way[1].y < currentCellY) {
			tankObject->moveUp();
		}
	}
}

std::shared_ptr<Tank> Bot::getObject() {
	return tankObject;
}

Bot::~Bot() {

	tankObject->destroyObject();
}