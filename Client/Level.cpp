#include "Level.h"
#include "TankV1.h"
#include "TankV2.h"
#include "TankV3.h"
#include "TankPLA1.h"
#include "TankPLA2.h"
#include "Brick.h"
class Tank;
class TankV1;
class TankV2;
class TankV3;
class TankPLA1;
class TankPLA2;
class Brick;

Level::Level(const int windowWidth, const int windowHeight, SDL_Renderer* renderer, std::string& ratingFilePath, std::string playerName) {
	this->levelData.level = this;
	this->levelData.windowHeight = windowHeight;
	this->levelData.windowWidth = windowWidth;
	this->renderer = renderer;
	this->levelData.tileHeight = 32;
	this->levelData.tileWidth = 32;

	this->returnToMenuTime = 2000;
	this->returnToMenuTimer = nullptr;

	this->generator = std::unique_ptr<AStar::Generator>(new AStar::Generator());
	this->generator->setWorldSize({ levelData.windowWidth / levelData.tileWidth, levelData.windowHeight / levelData.tileHeight });
	this->generator->setHeuristic(AStar::Heuristic::euclidean);
	this->generator->setDiagonalMovement(false);

	std::vector<std::shared_ptr<Player>> x;
	this->levelData.players = x;

	this->randomEventsTimer = nullptr;
	this->font = TTF_OpenFont("Pixeboy-z8XGD.ttf", 72);
	this->endTimer = nullptr;
	this->playerLivesLeft = 2;

	this->levelState = LevelState::Live;

	blockRect = { 0, 0, levelData.tileWidth, levelData.tileHeight };
	SDL_Surface* surface = SDL_LoadBMP("498_Tanks_sprites/gfx/block/world01/block_01.bmp");
	blockTexture = SDL_CreateTextureFromSurface(renderer, surface);

	this->filePath = ratingFilePath;
	this->playerName = playerName;
	setup();
}
void Level::setup() {
	this->levelData.colliderTileMap = std::vector<std::vector<bool>>(this->levelData.windowHeight / this->levelData.tileHeight + 1, std::vector<bool>(this->levelData.windowWidth / this->levelData.tileWidth + 1, false));
	this->botsOnLevel = 2;
	this->botsSpawnTime = 3 * 1000;

	SDL_Surface* backgroundSurface = SDL_LoadBMP("498_Tanks_sprites/gfx/ground/ground02.bmp");
	if (!backgroundSurface) {
		std::cout << SDL_GetError();
		exit(EXIT_FAILURE);
	}
	background = new GameObject(renderer, backgroundSurface, 0, 0, this->levelData.windowWidth, this->levelData.windowHeight, &levelData);
	SDL_FreeSurface(backgroundSurface);

	spawnPoints = { {18 * levelData.tileWidth, 3 * levelData.tileHeight}, {18 * levelData.tileWidth, 6 * levelData.tileHeight}, {18 * levelData.tileWidth, 9 * levelData.tileHeight} };

	for (int i = 0; i <= 2; i++) {
		this->levelData.colliderTileMap[4][i] = true;
		this->generator->addCollision({ i, 4 });
	}
	for (int i = 0; i <= 2; i++) {
		this->levelData.colliderTileMap[i][5] = true;
		this->generator->addCollision({ 5, i });
	}
	for (int i = 0; i <= 2; i++) {
		this->levelData.colliderTileMap[i][5] = true;
		this->generator->addCollision({ 5, i });
	}

	player = std::shared_ptr<Player>(new Player(std::shared_ptr<Tank>(new TankPLA1(renderer, 32, 32, 32, 32, &levelData, true)), this->playerName));
	this->levelData.players.push_back(player);
	
	for (int i = 0; i < 20; i++) {
		gameObjects.push_back(std::shared_ptr<Brick>(new Brick(renderer, 10 * levelData.tileWidth, i * levelData.tileHeight, 32, 32, &levelData)));
		gameObjects.push_back(std::shared_ptr<Brick>(new Brick(renderer, 15 * levelData.tileWidth, i * levelData.tileHeight, 32, 32, &levelData)));
	}

}
void Level::render() {
	if (levelState == LevelState::Exit) {
		return;
	}

	drawGrid();
	background->render();

	if (checkIfGameIsOver() && endTimer == nullptr) {
		endTimer = std::unique_ptr<Timer>(new Timer(3000));
	}
	if (checkIfGameIsOver() && endTimer != nullptr && endTimer->isExpired()) {
		levelState = LevelState::GameEnded;
		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Game over", { 255, 0, 0, 255 });
		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_Rect Message_rect = { levelData.windowWidth / 2 - 75,  levelData.windowHeight / 2 - 35, 150, 70 };
		SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(Message);

		char str[20] = "Your score: ";
		char score[10];
		_itoa_s(player->getScore(), score, 10);
		strcat_s(str, score);

		surfaceMessage = TTF_RenderText_Solid(font, str, { 0, 0, 0, 255 });
		Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		Message_rect = { levelData.windowWidth / 2 - 75,  levelData.windowHeight / 2 + 35, 150, 70 };
		SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(Message);

		player->writeScoreToFile(filePath);
		if(returnToMenuTimer == nullptr)
			returnToMenuTimer = std::unique_ptr<Timer>(new Timer(returnToMenuTime));
	}
	if (returnToMenuTimer != nullptr && returnToMenuTimer->isExpired()) {
		levelState = LevelState::Exit;
	}
	if (levelState == LevelState::GameEnded || levelState == LevelState::Exit) {
		return;
	}

	for (int i = 0; i < levelData.windowHeight / levelData.tileHeight; i++) {
		for (int j = 0; j < levelData.windowHeight / levelData.tileHeight; j++) {
			if (levelData.colliderTileMap[i][j]) {
				blockRect.x = j * levelData.tileWidth;
				blockRect.y = i * levelData.tileHeight;
				SDL_RenderCopy(renderer, blockTexture, NULL, &blockRect);
			}
		}
	}

	if (player && bots.size() < botsOnLevel && (botSpawnTimer == nullptr || botSpawnTimer->isExpired())) {
		std::pair<int, int> spawnPoint = spawnPoints[rand() % spawnPoints.size()];
		if (player->getTanksDestroyed() < 4) {
			bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV1(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
		}
		else if (player->getTanksDestroyed() < 8) {
			if(rand() % 2 == 0)
				bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV1(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
			else
				bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV2(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
		}
		else if (player->getTanksDestroyed() < 12) {
			this->botsOnLevel = 4;
			int r = rand() % 3;
			if (r == 0)
				bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV1(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
			else if(r == 1)
				bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV2(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
			else
				bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV3(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
		}
		else if (player->getTanksDestroyed() < 14) {
			if (rand() % 2 == 0)
				bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV2(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
			else
				bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV3(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
		}
		else if (player->getTanksDestroyed() < 16) {
			this->botsOnLevel = 5;
			if (rand() % 2 == 0)
				bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV2(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
			else
				bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV3(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
		}
		else if (player->getTanksDestroyed() < 18) {
			bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV3(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
		}
		else {
			this->botsOnLevel = 6;
			bots.push_back(std::shared_ptr<Bot>(new Bot(std::shared_ptr<Tank>(new TankV3(renderer, spawnPoint.first, spawnPoint.second, 32, 32, &levelData, true)))));
			this->botsSpawnTime = 2 * 1000;
		}
		botSpawnTimer.reset(new Timer(botsSpawnTime));
	}

	if(player != nullptr)
		player->render();

	for (int i = 0; i < bots.size(); i++) {
		bots[i]->render();
	}

	clearObjects();

	for (int i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->render();
	}

	if (randomEventsTimer == nullptr) {
		createRandomEvents();
		randomEventsTimer = std::unique_ptr<Timer>(new Timer(5000));
	}
	else if(randomEventsTimer->isExpired()) {
		randomEventsTimer.reset();
	}

	if (player != nullptr)
		player->renderGui();
	renderGui();
}
Level::LevelState Level::getState() {
	return levelState;
}
void Level::createRandomEvents() {
	int spawnX = 32 * (rand() % (levelData.windowHeight /levelData.tileHeight));
	int spawnY = 32 * (rand() % (levelData.windowWidth / levelData.tileWidth));
	if (this->levelData.colliderTileMap[(spawnX) / 32][(spawnY) / 32]) {
		Level::createRandomEvents();
		return;
	}
	if(rand() % 4 == 0)
		gameObjects.push_back(std::shared_ptr<Jaguar>(new Jaguar(renderer, spawnX, spawnY, 32, 32, &levelData)));
}

Level::~Level() {
	SDL_DestroyTexture(blockTexture);
}
void Level::createGameObject(std::shared_ptr<GameObject> gameObject) {
	gameObjects.push_back(gameObject);
}

void Level::clearObjects() {

	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->isToBeDestructed()) {	
			gameObjects.erase(gameObjects.begin() + i);
		}
	}
	for (int i = 0; i < bots.size(); i++) {
		if (bots[i]->getObject()->isToBeDestructed()) {
			bots.erase(bots.begin() + i);
		}
	}
	if (!player->getObject() || player && player->getObject()->isToBeDestructed()) {
		playerLivesLeft = std::max(0, playerLivesLeft - 1);
		std::shared_ptr<Tank> playerTank = player->getObject();
		for (int i = 0; i < levelData.players.size(); i++) {
			if (levelData.players[i] == player) {
				levelData.players.erase(levelData.players.begin() + i);
				break;
			}
		}
		if (playerLivesLeft) {
			if (std::dynamic_pointer_cast<TankPLA1>(playerTank)) {
				playerTank = std::shared_ptr<Tank>(new TankPLA1(renderer, 32, 32, 32, 32, &levelData, true));
				player->changeTankObject(std::shared_ptr<Tank>(playerTank));
				levelData.players.push_back(player);
			}
			else if (std::dynamic_pointer_cast<TankPLA2>(playerTank)) {
				playerTank = std::shared_ptr<Tank>(new TankPLA2(renderer, 32, 32, 32, 32, &levelData, true));
				player->changeTankObject(std::shared_ptr<Tank>(playerTank));
				levelData.players.push_back(player);
			}
		}
		else {
			player->destroyTankObject();
		}
		playerTank.reset();
	}
}

bool Level::checkIfGameIsOver() {
	return !playerLivesLeft && (bots.size() == 0 || !player->getObject() || player->getObject()->isToBeDestructed());
}

bool Level::checkForCollision(const double& leftX, const double& leftY, const double& rightX, const double& rightY, std::shared_ptr<GameObject> author) {
	std::shared_ptr<GameObject::Position> pos2;
	if (!checkForStaticCollision(leftX, leftY, rightX, rightY)) {
		return false;
	}
	for (int i = 0; i < gameObjects.size(); i++) {
		if (author == gameObjects[i])
			continue;
		pos2 = gameObjects[i]->getPosition();
		if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
			||
			(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)
			||
			(pos2->rect.x >= leftX && pos2->rect.x <= rightX)
			||
			(pos2->rect.x + pos2->rect.w >= leftX && pos2->rect.x + pos2->rect.w <= rightX)) {
			if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
				||
				(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)
				||
				(pos2->rect.y >= leftY && pos2->rect.y <= rightY)
				||
				(pos2->rect.y + pos2->rect.h >= leftY && pos2->rect.y + pos2->rect.h <= rightY)
				) {
				gameObjects[i]->collidesWith(author);
				if (gameObjects[i]->getIsCollided()) {
					author->collidesWith(gameObjects[i]);
					return false;
				}
			}
		}
	}
	for (int j = 0; j < bots.size(); j++) {
		if (std::dynamic_pointer_cast<GameObject>(bots[j]->getObject()) == author) 
			continue;
		pos2 = bots[j]->getObject()->getPosition();

		if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
			||
			(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)) {
			if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
				||
				(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)) {
				bots[j]->getObject()->collidesWith(author);
				author->collidesWith(bots[j]->getObject());
				return false;
			}
		}
	}
	if (player && player->getObject() && std::dynamic_pointer_cast<GameObject>(player->getObject()) != author) {
		pos2 = player->getObject()->getPosition();

		if ((leftX >= pos2->rect.x && leftX <= pos2->rect.x + pos2->rect.w)
			||
			(rightX >= pos2->rect.x && rightX <= pos2->rect.x + pos2->rect.w)
			||
			(pos2->rect.x >= leftX && pos2->rect.x <= rightX)
			||
			(pos2->rect.x + pos2->rect.w >= leftX && pos2->rect.x + pos2->rect.w <= rightX)) {
			if ((leftY >= pos2->rect.y && leftY <= pos2->rect.y + pos2->rect.h)
				||
				(rightY >= pos2->rect.y && rightY <= pos2->rect.y + pos2->rect.h)
				||
				(pos2->rect.y >= leftY && pos2->rect.y <= rightY)
				||
				(pos2->rect.y + pos2->rect.h >= leftY && pos2->rect.y + pos2->rect.h <= rightY)
				) {
				player->getObject()->collidesWith(author);
				author->collidesWith(player->getObject());
				return false;
			}
		}
	}
	return true;
}

void Level::renderGui() {
	char str[20] = "Lives left: ";
	char lives[10] = "\0";
	_itoa_s(playerLivesLeft, lives, 10);
	strcat_s(str, lives);

	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, str, { 0, 0, 0, 255 });
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_Rect Message_rect = { levelData.windowWidth / 2 - 75, 0, 150, 40 };
	SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

bool Level::checkForStaticCollision(const double& leftX, const double& leftY, const double& rightX, const double& rightY) {
	if (rightX < 0 || rightX > levelData.windowWidth || rightY < 0 || rightY > levelData.windowHeight) {
		return false;
	}
	if (leftX < 0 || leftX > levelData.windowWidth || leftY < 0 || leftY > levelData.windowHeight) {
		return false;
	}
	if (levelData.colliderTileMap[leftY / levelData.tileHeight][leftX / levelData.tileWidth]) {
		return false;
	}
	if (levelData.colliderTileMap[rightY / levelData.tileHeight][(leftX) / levelData.tileWidth]) {
		return false;
	}
	if (levelData.colliderTileMap[rightY / levelData.tileHeight][rightX / levelData.tileWidth]) {
		return false;
	}
	if (levelData.colliderTileMap[(leftY) / levelData.tileHeight][rightX / levelData.tileWidth]) {

		return false;
	}
	return true;
}

void Level::drawGrid() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	for (int i = 0; i <= levelData.windowWidth / levelData.tileWidth; i++) {
		SDL_RenderDrawLine(renderer, i * levelData.tileWidth, 0, i * levelData.tileWidth, levelData.windowHeight);
	}
	for (int i = 0; i <= levelData.windowHeight / levelData.tileHeight; i++) {
		SDL_RenderDrawLine(renderer, 0, i * levelData.tileHeight, levelData.windowWidth, i * levelData.tileHeight);
	}
	SDL_Rect rect;
	for (int i = 0; i < levelData.windowWidth / levelData.tileWidth; i++) {
		for (int j = 0; j < levelData.windowHeight / levelData.tileHeight; j++) {
			if (levelData.colliderTileMap[i][j]) {
				rect = { j * levelData.tileWidth, i * levelData.tileHeight, levelData.tileWidth, levelData.tileHeight };
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}

}

AStar::CoordinateList Level::calculateWayBetweenPoints(std::shared_ptr<GameObject::Position> pos1, std::shared_ptr<GameObject::Position> pos2) {
	AStar::CoordinateList path = generator->findPath({ static_cast<int>(pos1->rect.x) / levelData.tileWidth, static_cast<int>(pos1->rect.y) / levelData.tileHeight }, { static_cast<int>(pos2->rect.x) / levelData.tileWidth, static_cast<int>(pos2->rect.y) / levelData.tileHeight });
	
	if (showAStarPaths) {
		SDL_Rect pointRect;
		SDL_SetRenderDrawColor(renderer, rand() % 255, rand() % 255, rand() % 255, 255);
		for (int i = 0; i < path.size(); i++) {
			pointRect = { path[i].x * levelData.tileWidth + levelData.tileWidth / 2, path[i].y * levelData.tileHeight + levelData.tileHeight / 2 , 5, 5 };
			SDL_RenderFillRect(renderer, &pointRect);
		}
	}
	return path;
}

std::vector <std::shared_ptr<Brick>> Level::getBricks(){
	std::vector <std::shared_ptr<Brick>> bricks;
	for (int i = 0; i < gameObjects.size(); i++) {
		if (std::dynamic_pointer_cast<Brick>(gameObjects[i])) {
			bricks.push_back(std::dynamic_pointer_cast<Brick>(gameObjects[i]));
		}
	}
	return bricks;
}