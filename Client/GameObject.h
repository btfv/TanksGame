#pragma once
class Level;
#include <memory>
#include "LevelData.h"

class GameObject : public std::enable_shared_from_this<GameObject> {
public:
	struct Position {
		struct Rect {
			double x;
			double y;
			int w;
			int h;
		} rect;
		int rotation;
		SDL_RendererFlip flip = SDL_FLIP_NONE;
	};
protected:
	unsigned long long tick;
	bool isDestroyedOnContact = 0;
	bool toBeDestructed = 0;
	struct LevelData* levelData;
	bool isCollided;
	SDL_Renderer* renderer;
	std::shared_ptr<Position> position;
	SDL_Rect blit;
	SDL_Texture* texture;
	int currentImgState;
public:
	GameObject(SDL_Renderer* renderer, const double x, const double y, const int w, const int h, LevelData* levelData);
	GameObject(SDL_Renderer* renderer, SDL_Surface* surface, const double x, const double y, const int w, const int h, LevelData* levelData);
	GameObject(SDL_Renderer* renderer, SDL_Texture* texture, const double x, const double y, const int w, const int h, LevelData* levelData);
	virtual void render();
	bool moveObject(const double x, const double y, const int rotation);
	std::shared_ptr<Position> getPosition();
	~GameObject();
	virtual void collidesWith(std::shared_ptr<GameObject> secondObject) {
		if (isDestroyedOnContact)
			this->destroyObject();
	};
	bool isToBeDestructed() { return toBeDestructed; }
	virtual void destroyObject();
	LevelData* getLevelData() { return levelData; }
	bool getIsCollided() { return isCollided; };
};