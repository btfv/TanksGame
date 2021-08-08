#define _CRT_SECURE_NO_WARNINGS
#include "Menu.h"
#include <iostream>
#include <algorithm>
#include <fstream>

Menu::Menu(SDL_Renderer* renderer, const int width, const int height, std::string& ratingFilePath) {
	this->renderer = renderer;

	SDL_Surface* surface = IMG_Load("498_Tanks_sprites/gfx/gui/i_background.jpg");
	if (!surface) {
		std::cout << IMG_GetError();
		exit(EXIT_FAILURE);
	}
	background = SDL_CreateTextureFromSurface(renderer, surface);
	backgroundRect = { 0, 0, width, height };
	SDL_FreeSurface(surface);
	font = TTF_OpenFont("Pixeboy-z8XGD.ttf", 72);
	fontOutline = TTF_OpenFont("Pixeboy-z8XGD.ttf", 72);
	TTF_SetFontOutline(fontOutline, 1);

	playTextState = 0;
	this->windowHeight = height;
	this->windowWidth = width;

	this->getRating(ratingFilePath);
	menuState = MenuState::NameInput;
	countOfMenuPositions = 2;
	currMenuPosition = 0;
}

Menu::Menu(SDL_Renderer* renderer, const int width, const int height, std::string& ratingFilePath, std::string playerName) {
	this->renderer = renderer;

	SDL_Surface* surface = IMG_Load("498_Tanks_sprites/gfx/gui/i_background.jpg");
	if (!surface) {
		std::cout << IMG_GetError();
		exit(EXIT_FAILURE);
	}
	background = SDL_CreateTextureFromSurface(renderer, surface);
	backgroundRect = { 0, 0, width, height };
	SDL_FreeSurface(surface);
	font = TTF_OpenFont("Pixeboy-z8XGD.ttf", 72);
	fontOutline = TTF_OpenFont("Pixeboy-z8XGD.ttf", 72);
	TTF_SetFontOutline(fontOutline, 1);

	playTextState = 0;
	this->windowHeight = height;
	this->windowWidth = width;

	this->getRating(ratingFilePath);
	if (playerName.empty()) {
		menuState = MenuState::NameInput;
		SDL_StartTextInput();
	}
	else {
		menuState = MenuState::Main;
		this->playerName = playerName;
	}
	countOfMenuPositions = 2;
	currMenuPosition = 0;
}
void Menu::render() {
	SDL_RenderCopy(renderer, background, nullptr, &backgroundRect);
	switch (menuState) {
	case MenuState::Main:
		if (playTextState == 0) {
			SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Play!", { static_cast<Uint8>((currMenuPosition == 0) ? 255 : 0), 0, 0, 255 });
			SDL_Texture* playText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
			SDL_Rect playTextRect = { windowWidth / 2 - 75, windowHeight / 2 - 75, 150, 75 };

			SDL_RenderCopy(renderer, playText, nullptr, &playTextRect);

			SDL_FreeSurface(surfaceMessage);
			SDL_DestroyTexture(playText);
		}
		else {
			SDL_Surface* surfaceMessage = TTF_RenderText_Solid(fontOutline, "Play!", { static_cast<Uint8>((currMenuPosition == 0) ? 255 : 0), 0, 0, 255 });
			SDL_Texture* playText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
			SDL_Rect playTextRect = { windowWidth / 2 - 75, windowHeight / 2 - 75, 150, 75 };

			SDL_RenderCopy(renderer, playText, nullptr, &playTextRect);

			SDL_FreeSurface(surfaceMessage);
			SDL_DestroyTexture(playText);
		}
		if (ratingTextState == 0) {
			SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Rating", { static_cast<Uint8>((currMenuPosition == 1) ? 255 : 0), 0, 0, 255 });
			SDL_Texture* playText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
			SDL_Rect playTextRect = { windowWidth / 2 - 75, windowHeight / 2 + 50, 150, 75 };

			SDL_RenderCopy(renderer, playText, nullptr, &playTextRect);

			SDL_FreeSurface(surfaceMessage);
			SDL_DestroyTexture(playText);
		}
		else {
			SDL_Surface* surfaceMessage = TTF_RenderText_Solid(fontOutline, "Rating", { static_cast<Uint8>((currMenuPosition == 1) ? 255 : 0), 0, 0, 255 });
			SDL_Texture* playText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
			SDL_Rect playTextRect = { windowWidth / 2 - 75, windowHeight / 2 + 50, 150, 75 };

			SDL_RenderCopy(renderer, playText, nullptr, &playTextRect);

			SDL_FreeSurface(surfaceMessage);
			SDL_DestroyTexture(playText);
		}
		break;
	case MenuState::Rating:
		showRating();
		break;
	case MenuState::NameInput:
		inputName();
		break;
	}
	
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Control:   W - up, S - down, D - right, A - left, Space - strike", { 0, 0, 0, 255 });
	SDL_Texture* playText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_Rect playTextRect = { 25, windowHeight - 50, 600, 50 };

	SDL_RenderCopy(renderer, playText, nullptr, &playTextRect);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(playText);

	SDL_Event event;

	if (menuState == MenuState::NameInput) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				exit(EXIT_SUCCESS);
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
					if (playerName.length())
						playerName.pop_back();
				}
				break;
			case SDL_KEYUP:
				if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
					if (playerName.length()) {
						SDL_StopTextInput();
						menuState = MenuState::Main;
					}
				}
				break;
			case SDL_TEXTINPUT:
				if(playerName.length() < 30)
					playerName.push_back(event.text.text[0]);
				break;
			}
		}
	}
	else {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				exit(EXIT_SUCCESS);
				break;
			case SDL_KEYUP:
				if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
					switch (currMenuPosition) {
					case 0:
						playTextState = 0;
						menuState = MenuState::Play;
						break;
					case 1:
						menuState = MenuState::Rating;
						break;
					}
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_S) {
					currMenuPosition = std::min(currMenuPosition + 1, static_cast<int>(countOfMenuPositions) - 1);
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_W) {
					currMenuPosition = std::max(currMenuPosition - 1, static_cast<int>(0));
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					if (menuState != MenuState::Main)
						menuState = MenuState::Main;
					else
						menuState = MenuState::Exit;
				}
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_RETURN && currMenuPosition == 0) {
					playTextState = 1;
				}
				break;
			}
		}
	}
}

Menu::MenuState Menu::getState() {
	return menuState;
}

Menu::~Menu() {
	TTF_CloseFont(font);
	TTF_CloseFont(fontOutline);
	SDL_DestroyTexture(background);
}

void Menu::showRating() {
	SDL_Rect mainRect = { windowWidth / 2 - 100, windowHeight / 2 - 250,  200, 50};
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Rating board", { 0, 0, 0, 255 });
	SDL_Texture* ratingBoardText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_RenderCopy(renderer, ratingBoardText, NULL, &mainRect);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(ratingBoardText);

	for (int i = 0; i < ratingBoard.size(); i++) {
		mainRect = { windowWidth / 2 - 125, windowHeight / 2 - 200 + (i + 1) * 50,  50, 25 };
		surfaceMessage = TTF_RenderText_Solid(font, ratingBoard[i].name, { 0, 0, 0, 255 });
		ratingBoardText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_RenderCopy(renderer, ratingBoardText, NULL, &mainRect);
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(ratingBoardText);

		mainRect = { windowWidth / 2 + 100, windowHeight / 2 - 200 + (i + 1) * 50,  25, 25 };
		char temp[11];
		_itoa_s(ratingBoard[i].score, temp, 10);
		surfaceMessage = TTF_RenderText_Solid(font, temp, { 0, 0, 0, 255 });
		ratingBoardText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_RenderCopy(renderer, ratingBoardText, NULL, &mainRect);
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(ratingBoardText);
	}
	if (!ratingBoard.size()) {
		mainRect = { windowWidth / 2 - 200, windowHeight / 2 - 200 + 30, 400, 30 };
		surfaceMessage = TTF_RenderText_Solid(font, "Rating is empty. You can be the first!", { 255, 0, 0, 255 });
		ratingBoardText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_RenderCopy(renderer, ratingBoardText, NULL, &mainRect);
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(ratingBoardText);
	}
}

void Menu::getRating(std::string& filePath) {
	ratingBoard.clear();
	Player::PlayerData temp;

	std::ifstream ifs(filePath, std::ifstream::binary);
	if (!ifs.is_open()) {
		return;
	}
	while (!ifs.eof()) {
		ifs.read(reinterpret_cast<char*>(&temp), sizeof(temp));
		if (!ifs.eof()) {
			ratingBoard.push_back(temp);
		}
	}
	ifs.close();

	std::sort(ratingBoard.begin(), ratingBoard.end(), [](Player::PlayerData& left, Player::PlayerData& right) {
		return left.score > right.score;
		});
	if(ratingBoard.size() > ratingBoardSize)
		ratingBoard.erase(ratingBoard.begin() + ratingBoardSize, ratingBoard.end());
}

std::string Menu::getName() {
	return playerName;
}

void Menu::inputName() {
	SDL_Rect mainRect = { windowWidth / 2 - 150, windowHeight / 2 - 250, 300, 50 };
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Enter your name", { 0, 0, 0, 255 });
	SDL_Texture* ratingBoardText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_RenderCopy(renderer, ratingBoardText, NULL, &mainRect);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(ratingBoardText);

	mainRect = { windowWidth / 2 - 50, windowHeight / 2 - 150, static_cast<int>(playerName.length() * 20), 20 };
	surfaceMessage = TTF_RenderText_Solid(font, playerName.c_str(), { 0, 0, 0, 255 });
	ratingBoardText = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_RenderCopy(renderer, ratingBoardText, NULL, &mainRect);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(ratingBoardText);
}