#pragma once
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include <vector>
#include "GameObject.h"
#include "Level.h"

GameObject::GameObject(SDL_Renderer* renderer, const double x, const double y, const int w, const int h, LevelData* levelData) {
	currentImgState = 0;
	isCollided = false;
	this->texture = nullptr;
	this->renderer = renderer;
	this->position = std::shared_ptr<GameObject::Position>(new GameObject::Position{});
	position->rect.x = x;
	position->rect.y = y;
	position->rect.w = w;
	position->rect.h = h;
	blit = { 0, 0, w, h };

	this->levelData = levelData;
}
GameObject::GameObject(SDL_Renderer* renderer, SDL_Surface* surface, const double x, const double y, const int w, const int h, LevelData* levelData) : GameObject(renderer, x, y, w, h, levelData) {
	this->texture = SDL_CreateTextureFromSurface(renderer, surface);
}
GameObject::GameObject(SDL_Renderer* renderer, SDL_Texture* texture, const double x, const double y, const int w, const int h, LevelData* levelData) : GameObject(renderer, x, y, w, h, levelData) {
	this->texture = texture;
}
void GameObject::render() {
	SDL_Rect rect;
	rect.x = static_cast<int>(position->rect.x);
	rect.y = static_cast<int>(position->rect.y);
	rect.w = position->rect.w;
	rect.h = position->rect.h;
	SDL_RenderCopyEx(renderer, texture, &blit, &(rect), position->rotation, nullptr, position->flip);
}
bool GameObject::moveObject(const double x, const double y, const int rotation) {
	if (isCollided && !levelData->level->checkForCollision(x + 2, y + 2, x + position->rect.w - 2, y + position->rect.h - 2, shared_from_this())) {
		if (isDestroyedOnContact) {
			destroyObject();
		}
		return false;
	}
	position->rect.x = x;
	position->rect.y = y;
	position->rotation = rotation;
	return true;
}
std::shared_ptr<GameObject::Position> GameObject::getPosition() {
	return position;
}
GameObject::~GameObject() {
	SDL_DestroyTexture(texture);
}

void GameObject::destroyObject() { toBeDestructed = 1; }
