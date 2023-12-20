#include "pong.h"
#include "brick_breaker.h"
#include "tetris.h"
#include "snake.h"
#include "game_over.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <SDL_mixer.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 50;

class Emulator {
public:
    Emulator();
    ~Emulator();
    void run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgroundTexture;
    TTF_Font* font;
    GameOver gameOverScreen;
    SDL_Rect button1, button2, button3, button4;
    bool quit;
    Mix_Music* backgroundMusic;
    bool init();
    void handleEvents();
    void render();
    bool isInside(int x, int y, SDL_Rect rect);
    void renderText(const char* text, SDL_Color color, int x, int y);
};

Emulator::Emulator() : window(nullptr), renderer(nullptr), backgroundTexture(nullptr), font(nullptr), gameOverScreen(500, 500, "font.ttf", 60, 1000), backgroundMusic(nullptr) {
    button1 = {100, 100, BUTTON_WIDTH, BUTTON_HEIGHT};
    button2 = {100, 200, BUTTON_WIDTH, BUTTON_HEIGHT};
    button3 = {100, 300, BUTTON_WIDTH, BUTTON_HEIGHT};
    button4 = {100, 400, BUTTON_WIDTH, BUTTON_HEIGHT};
}

Emulator::~Emulator() {
    TTF_CloseFont(font);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

bool Emulator::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || IMG_Init(IMG_INIT_PNG) < 0 || TTF_Init() < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Arcade Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    backgroundTexture = IMG_LoadTexture(renderer, "background.png");
    font = TTF_OpenFont("font.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;//music implemented
            return false;
        }

        backgroundMusic = Mix_LoadMUS("game.mp3");
        if (!backgroundMusic) {
            std::cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
            return false;
        }

        if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
            std::cerr << "SDL_mixer could not play music! SDL_mixer Error: " << Mix_GetError() << std::endl;
            return false;
        }

        return true;
    }

void Emulator::run() {
    if (!init()) {
        std::cerr << "Failed to initialize!" << std::endl;
        return;
    }

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        handleEvents();
        render();
    }
}

void Emulator::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            quit = true;
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            SDL_GetMouseState(&x, &y);

            if (isInside(x, y, button1)) {
                runTetrisGame();
                gameOverScreen.show();
                std::cout << "Game Over" << std::endl;
            } else if (isInside(x, y, button2)) {
                runPongGame();
                std::cout << "Pong" << std::endl;
            } else if (isInside(x, y, button3)) {
                runBrickGame();
                gameOverScreen.show();
                std::cout << "Game Over" << std::endl;
            } else if (isInside(x, y, button4)) {
                runSnakeGame();
                gameOverScreen.show();
                std::cout << "Game Over" << std::endl;
            }
        }
    }
}

void Emulator::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    if (backgroundTexture != NULL) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Button color
    SDL_RenderFillRect(renderer, &button1);
    SDL_RenderFillRect(renderer, &button2);
    SDL_RenderFillRect(renderer, &button3);
    SDL_RenderFillRect(renderer, &button4);

    SDL_Color textColor = {255, 255, 255, 255}; // Text color

    renderText("Tetris", textColor, button1.x + (BUTTON_WIDTH - strlen("Tetris") * 12) / 2, button1.y + (BUTTON_HEIGHT - 24) / 2);
    renderText("Pong", textColor, button2.x + (BUTTON_WIDTH - strlen("Pong") * 12) / 2, button2.y + (BUTTON_HEIGHT - 24) / 2);
    renderText("Brick breaker", textColor, button3.x + (BUTTON_WIDTH - strlen("Brick breaker") * 12) / 2, button3.y + (BUTTON_HEIGHT - 24) / 2);
    renderText("Snake", textColor, button4.x + (BUTTON_WIDTH - strlen("Snake") * 12) / 2, button4.y + (BUTTON_HEIGHT - 24) / 2);

    SDL_RenderPresent(renderer);
}

bool Emulator::isInside(int x, int y, SDL_Rect rect) {
    return (x > rect.x) && (x < rect.x + rect.w) && (y > rect.y) && (y < rect.y + rect.h);
}

void Emulator::renderText(const char* text, SDL_Color color, int x, int y) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[]) {
    Emulator emulator;
    emulator.run();
    return 0;
}
//g++ -std=c++11 -o Emulator emulator.cpp tetris.cpp brick_breaker.cpp pong.cpp snake.cpp game_over.cpp -IC:\mingw_dev_lib\include\SDL2 -LC:\mingw_dev_lib\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer