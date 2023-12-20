#include "pong.h"
#include <SDL.h>
#include <iostream>
#include <vector>
#include <ctime> // for time()
#include <cstdlib> // for srand(), rand()
#include <string>
#include <SDL_ttf.h>

class Pong_Paddle {
public:
    Pong_Paddle(int x, int y, int w, int h, SDL_Scancode upKey, SDL_Scancode downKey)
        : x(x), y(y), width(w), height(h), velocity(0), upKey(upKey), downKey(downKey) {}

    void handleInput(const Uint8* keystate) {
        if (keystate[upKey]) {
            velocity = -5;
        } else if (keystate[downKey]) {
            velocity = 5;
        } else {
            velocity = 0;
        }
    }

    void update() {
        y += velocity;
        if (y < 0) y = 0;
        if (y + height > 800) y = 800 - height; // Assuming screen height is 800
    }

    void render(SDL_Renderer* renderer) {
        SDL_Rect rect = {x, y, width, height};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
        SDL_RenderFillRect(renderer, &rect);
    }

    // Getter methods for the paddle's properties
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    int x, y;
    int width, height;
    int velocity;
    SDL_Scancode upKey, downKey;
};


class Score {
public:
    Score() : scoreA(0), scoreB(0) {}

    // Increment functions (existing)
    void incrementScoreA() {
        scoreA += 10;
    }
    void incrementScoreB() {
        scoreB += 10;
    }

    // Overload the += operator for Score
    Score& operator+=(const Score& other) {
        scoreA += other.scoreA;
        scoreB += other.scoreB;
        return *this;
    }

    // Getters (existing)
    int getScoreA() const {
        return scoreA;
    }
    int getScoreB() const {
        return scoreB;
    }

private:
    int scoreA;
    int scoreB;
};


class Pong_Ball {
public:
    Pong_Ball(int x, int y, int size, int velocityX, int velocityY)
        : x(x), y(y), size(size), velocityX(velocityX), velocityY(velocityY) {}

    void update() {
        x += velocityX;
        y += velocityY;

        // Bounce off the top and bottom edges
        if (y < 0 || y + size > 800) {
            velocityY = -velocityY;
        }
    }

    void render(SDL_Renderer* renderer) {
        SDL_Rect rect = {x, y, size, size};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for the ball
        SDL_RenderFillRect(renderer, &rect);
    }

    bool collidesWith(const Pong_Paddle& paddle) {
        // Simple AABB collision detection
        return x < paddle.getX() + paddle.getWidth() &&
               x + size > paddle.getX() &&
               y < paddle.getY() + paddle.getHeight() &&
               y + size > paddle.getY();
    }

    void reverseX() { velocityX = -velocityX; }
    void reverseY() { velocityY = -velocityY; }

    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;
    }

    void setVelocity(int newVelocityX, int newVelocityY) {
        velocityX = newVelocityX;
        velocityY = newVelocityY;
    }

    int getX() const { return x; }
    int getY() const { return y; }
    int getSize() const { return size; }

private:
    int x, y;
    int size;
    int velocityX, velocityY;
};

class PongGame {
public:
    PongGame()
        : isRunning(true),
          isStarted(false),
          window(nullptr),
          renderer(nullptr),
          score() {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Pong Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 800, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        paddleA = new Pong_Paddle(30, 350, 20, 150, SDL_SCANCODE_W, SDL_SCANCODE_S);
        paddleB = new Pong_Paddle(940, 350, 20, 150, SDL_SCANCODE_I, SDL_SCANCODE_K);
        ball = new Pong_Ball(495, 395, 20, 0, 0); // Positioned in center with no initial movement
    }

    ~PongGame() {
        delete paddleA;
        delete paddleB;
        delete ball;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void run() {
        while (isRunning) {
            handleEvents();
            if (!isRunning) break;
            update();
            render();
        }
    }

private:
    void handleEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
                std::cout << "SDL_QUIT event captured. Exiting game loop." << std::endl;
                isRunning = false;
            }

            if (!isStarted) {
                const Uint8* keystate = SDL_GetKeyboardState(NULL);
                if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_S] ||
                    keystate[SDL_SCANCODE_I] || keystate[SDL_SCANCODE_K]) {
                    isStarted = true;
                    ball->setVelocity(1, 1); // Set initial velocity when the game starts
                }
            }
        }
    }

    void update() {
        if (!isStarted) {
            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            if (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_S] ||
                keystate[SDL_SCANCODE_I] || keystate[SDL_SCANCODE_K]) {
                isStarted = true;
                ball->setVelocity(1, 1); // Set initial velocity when the game restarts
            }
            return; // Don't update the game until it has started or restarted
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        paddleA->handleInput(keystate);
        paddleB->handleInput(keystate);
        paddleA->update();
        paddleB->update();
        ball->update();

        if (ball->collidesWith(*paddleA) || ball->collidesWith(*paddleB)) {
            ball->reverseX();
        }

        if (ball->getX() < 0) {
            Score point; // Temporary score object
            point.incrementScoreB(); // Increment paddle B's score
            score += point; // Add to the main score using overloaded operator
            resetBall();
            printScores();
        } else if (ball->getX() > 1000 - ball->getSize()) {
            Score point;
            point.incrementScoreA(); // Increment paddle A's score
            score += point;
            resetBall();
            printScores();
        }
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, 500, 0, 500, 800);

        paddleA->render(renderer);
        paddleB->render(renderer);
        ball->render(renderer);

        SDL_RenderPresent(renderer);
    }

    void resetBall() {
        ball->setPosition(495, 395); // Reset to center
        ball->setVelocity(0, 0); // Set velocity to zero
        isStarted = false; // Reset the start condition
    }

    void printScores() {
        std::cout << "Paddle A Score: " << score.getScoreA() << std::endl;
        std::cout << "Paddle B Score: " << score.getScoreB()-200 << std::endl;
    }

private:
    bool isRunning;
    bool isStarted;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Pong_Paddle* paddleA;
    Pong_Paddle* paddleB;
    Pong_Ball* ball;
    Score score;
};

void runPongGame() {
    PongGame game;
    game.run();
}


