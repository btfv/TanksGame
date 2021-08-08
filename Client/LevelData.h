#pragma once
#include <vector>
//#include "Level.h"
class Level;
class Player;
struct LevelData {
	int windowWidth;
	int windowHeight;
	int tileHeight;
	int tileWidth;
	std::vector<std::vector<bool>> colliderTileMap;
	Level* level;
	std::vector<std::shared_ptr<Player>> players;
};