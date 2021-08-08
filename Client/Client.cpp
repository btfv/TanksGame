#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <Windows.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <string>
#include "Level.h"
#include "Menu.h"

std::string rating_file_path("rating.dat");

const int WIDTH = 640, HEIGHT = 640;

enum class GameMode { Menu = 1, Level_1 = 2, Level_2 = 3 };

int main(int argc, char* argv[])
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    std::srand(std::time(nullptr));
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    SDL_Window* window = SDL_CreateWindow("tanks", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, 0);
    if (!renderer) {
        std::cout << SDL_GetError();
        return EXIT_FAILURE;
    }
    std::unique_ptr<Menu> menu = std::unique_ptr<Menu>(new Menu(renderer, WIDTH, HEIGHT, rating_file_path));
    std::unique_ptr<Level> level = nullptr;

    GameMode mode = GameMode::Menu;

    std::string playerName;
    SDL_bool done = SDL_FALSE;
    while (!done) {
        SDL_Event event;
        SDL_Delay(10);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        switch (mode) {
        case GameMode::Menu:
            menu->render();
            break;
        case GameMode::Level_1:
            level->render();
            break;
        }

        if(menu)
            switch (menu->getState()) {
            case Menu::MenuState::Play:
                if (!level) {
                    mode = GameMode::Level_1;
                    playerName = menu->getName();
                    level = std::unique_ptr<Level>(new Level(WIDTH, HEIGHT, renderer, rating_file_path, playerName));
                    menu.reset();
                }
                break;
            case Menu::MenuState::Exit:
                done = SDL_TRUE;
                break;
            }
        if (level && level->getState() == Level::LevelState::Exit) {
            level.reset();
            mode = GameMode::Menu;
            menu = std::unique_ptr<Menu>(new Menu(renderer, WIDTH, HEIGHT, rating_file_path, playerName));
        }

        SDL_RenderPresent(renderer);
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                done = SDL_TRUE;
                break;
            }
        }
    }
    level.reset();
    menu.reset();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return EXIT_SUCCESS;
}