#pragma once
#include "Player.h"
#include <memory>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <vector>

class Player;


class Menu {
public:
	enum class MenuState { Play = -1, Exit = 0, Main = 1, Rating = 2, NameInput = 3 };
private:
	SDL_Renderer* renderer;
	SDL_Texture* background;
	TTF_Font* font;
	TTF_Font* fontOutline;
	SDL_Rect backgroundRect;

	bool playTextState;
	bool ratingTextState;

	std::vector<Player::PlayerData> ratingBoard;
	MenuState menuState;
	int windowWidth;
	int windowHeight;
	int currMenuPosition;
	unsigned int countOfMenuPositions;
	std::string playerName;
	const int ratingBoardSize = 6;
public:
	Menu(SDL_Renderer* renderer, const int width, const int height, std::string& ratingFilePath);
	Menu(SDL_Renderer* renderer, const int width, const int height, std::string& ratingFilePath, std::string playerName);
	void render();
	Menu::MenuState getState();
	void showRating();
	void getRating(std::string& filePath);
	~Menu();
	std::string getName();
	void inputName();
};