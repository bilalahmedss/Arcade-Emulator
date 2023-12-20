#include "game_over.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>

GameOver::GameOver(int windowWidth, int windowHeight, const char* fontPath, int fontSize, int displayDuration)
    : windowWidth(windowWidth), windowHeight(windowHeight), fontPath(fontPath), fontSize(fontSize), displayDuration(displayDuration) {}

void GameOver::show() {
     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        SDL_Window* window = SDL_CreateWindow("Game Over", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        TTF_Init();
        TTF_Font* font = TTF_OpenFont(fontPath, fontSize);
        if (!font) {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return;
        }

        SDL_Color textColor = {255, 255, 255, 255}; // White color
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Game Over", textColor);
        SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

        SDL_Rect messageRect;
        messageRect.x = (windowWidth - surfaceMessage->w) / 2;
        messageRect.y = (windowHeight - surfaceMessage->h) / 2;
        messageRect.w = surfaceMessage->w;
        messageRect.h = surfaceMessage->h;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, message, NULL, &messageRect);
        SDL_RenderPresent(renderer);

        SDL_Delay(displayDuration);

        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(message);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }
