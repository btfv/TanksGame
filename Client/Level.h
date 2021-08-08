#pragma once
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <vector>
#include <SDL/SDL_ttf.h>
#include <utility>

#include "Jaguar.h"
#include "Player.h"
#include "Bot.h"
#include "LevelData.h"
#include "Timer.h"
#include "AStar.h"
#include "Tank.h"
#include "Brick.h"

struct LevelData;
class Player;
class Bot;
class Timer;
class Tank;
class TankV1;

class Level {
public:
	enum class LevelState { Live = 1, GameEnded = 2, Exit = 3 };
private:
	LevelData levelData;
	SDL_Renderer* renderer;
	SDL_Texture* blockTexture;
	SDL_Rect blockRect;
	std::vector <std::shared_ptr<GameObject>> gameObjects;
	std::shared_ptr<Player> player;
	GameObject* background;
	std::vector <std::shared_ptr<Bot>> bots;
	void drawGrid();
	std::unique_ptr<Timer> randomEventsTimer;
	std::unique_ptr<Timer> endTimer;
	std::unique_ptr<Timer> botSpawnTimer;
	std::unique_ptr<Timer> returnToMenuTimer;
	std::vector <std::pair<int, int>> spawnPoints;
	TTF_Font* font;
	std::unique_ptr<AStar::Generator> generator;
	unsigned int botsOnLevel;
	unsigned int botsSpawnTime;
	unsigned int returnToMenuTime;
	const bool showAStarPaths = 0;
	int playerLivesLeft;
	std::string filePath;
	LevelState levelState;
	std::string playerName;
public:
	Level(const int windowWidth, const int windowHeight, SDL_Renderer* renderer, std::string& ratingFilePath, std::string playerName);
	void setup();
	void render();
	bool checkForCollision(const double&, const double&, const double&, const double&, std::shared_ptr<GameObject>);
	~Level();
	void renderGui();
	SDL_Renderer* getRenderer() { return renderer; };
	TTF_Font* getFont() { return font; };
	void createGameObject(std::shared_ptr<GameObject> gameObject);
	void clearObjects();
	void createRandomEvents();
	Level::LevelState getState();
	bool checkIfGameIsOver();
	std::vector <std::shared_ptr<Brick>> getBricks();
	std::shared_ptr<Player> getPlayer() { return player; };
	bool checkForStaticCollision(const double&, const double&, const double&, const double&);
	AStar::CoordinateList calculateWayBetweenPoints(std::shared_ptr<GameObject::Position> pos1, std::shared_ptr<GameObject::Position> pos2);
};

