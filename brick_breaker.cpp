//#include "brick_breaker.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

// GameObject Base Class
class GameObject {
protected:
    int x, y;
    SDL_Renderer* renderer;

public:
    GameObject(int x, int y, SDL_Renderer* renderer) : x(x), y(y), renderer(renderer) {}

    virtual void draw() = 0;
    virtual void update() {}
};

// Brick Class
class Brick : public GameObject {
private:
    int width, height;
    SDL_Color color;
    bool visible;

public:
    Brick(int x, int y, int width, int height, SDL_Color color, SDL_Renderer* renderer)
        : GameObject(x, y, renderer), width(width), height(height), color(color), visible(true) {}

    void draw() override {
        if (visible) {
            SDL_Rect rect = { x, y, width, height };
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    bool isVisible() const { return visible; }
    void setVisible(bool v) { visible = v; }

    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

// Paddle Class
class Paddle : public GameObject {
private:
    int width, height;
    int maxSpeed;  // Maximum speed
    int screenWidth;
    SDL_Scancode leftKey, rightKey;
    int velocity;

public:
    Paddle(int x, int y, int width, int height, int maxSpeed, int screenWidth, SDL_Renderer* renderer, SDL_Scancode leftKey, SDL_Scancode rightKey)
        : GameObject(x, y, renderer), width(width), height(height), maxSpeed(maxSpeed), screenWidth(screenWidth), leftKey(leftKey), rightKey(rightKey), velocity(0) {}

    int getX() const { return x; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void handleInput(const Uint8* keystate) {
        if (keystate[leftKey]) {
            velocity = std::max(velocity - 1, -maxSpeed); // Smaller decrement for finer control
        } else if (keystate[rightKey]) {
            velocity = std::min(velocity + 1, maxSpeed); // Smaller increment for finer control
        } else {
            // Gradually reduce velocity to zero when no key is pressed
            if (velocity > 0) velocity--;
            else if (velocity < 0) velocity++;
        }
    }

    void update() {
        x += velocity;
        if (x < 0) x = 0;
        if (x + width > screenWidth) x = screenWidth - width;
    }

    void reset() {
        x = (screenWidth - width) / 2;
    }

    SDL_Rect getRect() const {
        return SDL_Rect{ x, y, width, height };
    }

    int getY() const {
        return y;
    }

    void draw() override {
        SDL_Rect rect = getRect();
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};

// Ball Class
class Ball : public GameObject {
private:
    int radius;
    static const int BALL_SPEED_X = 1; // Consistent and slower speed
    static const int BALL_SPEED_Y = 1;  // Cons
    int speedX, speedY;
    int screenWidth, screenHeight;

public:
    Ball(int x, int y, int radius, int speedX, int speedY, int screenWidth, int screenHeight, SDL_Renderer* renderer)
        : GameObject(x, y, renderer), radius(radius), speedX(BALL_SPEED_X), speedY(BALL_SPEED_Y), screenWidth(screenWidth), screenHeight(screenHeight) {}
    int getRadius() const { return radius; }
    void update(Paddle& paddle){
        x += speedX;
        y += speedY;

        if (x - radius <= 0 || x + radius >= screenWidth) {
            speedX = -speedX;
        }
        if (y - radius <= 0) {
            speedY = -speedY;
        }

        if (collidesWith(paddle)) {
            speedY = -abs(speedY);
        }
    }

    bool isCollisionHorizontal(const Brick& brick) {
        int centerY = y;
        return centerY > brick.getY() && centerY < brick.getY() + brick.getHeight();
    }

    void reverseXDirection() {
        speedX = -speedX;
    }


    bool collidesWith(const Paddle& paddle) {
        SDL_Rect paddleRect = paddle.getRect();
        return x + radius > paddleRect.x && x - radius < paddleRect.x + paddleRect.w &&
               y + radius > paddleRect.y && y - radius < paddleRect.y + paddleRect.h;
    }
    bool collidesWith(const Brick& brick) {
        if (!brick.isVisible()) return false;

        int centerX = x;
        int centerY = y;
        int closestX = (centerX < brick.getX()) ? brick.getX() : (centerX > brick.getX() + brick.getWidth()) ? brick.getX() + brick.getWidth() : centerX;
        int closestY = (centerY < brick.getY()) ? brick.getY() : (centerY > brick.getY() + brick.getHeight()) ? brick.getY() + brick.getHeight() : centerY;

        int distanceX = centerX - closestX;
        int distanceY = centerY - closestY;

        if ((distanceX * distanceX + distanceY * distanceY) < (radius * radius)) {
            reverseYDirection();
            return true;
        }
        return false;
    }


    void draw() override {
        SDL_Rect rect = { x - radius, y - radius, 2 * radius, 2 * radius };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    void reverseYDirection() {
        speedY = -speedY;
    }

    bool isOutOfBounds(int screenHeight) {
        return y - radius > screenHeight;
    }

    void reset(int newX, int newY) {
        x = newX;
        y = newY;
        speedX = BALL_SPEED_X;
        speedY = BALL_SPEED_Y;
    }
};

// Score Class
class Score {
private:
    int score;

public:
    Score() : score(0) {}

    void addPoints(int points) {
        score += points;
    }

    int getScore() const {
        return score;
    }
};

// Game Class
class Game {
private:
    static const int BALL_SPEED_X = -1;
    static const int BALL_SPEED_Y = 1;
    int screenWidth, screenHeight;
    std::vector<GameObject*> gameObjects; // Container for all game objects
    SDL_Window* window;
    SDL_Renderer* renderer;
    Paddle* paddle;  // Paddle now a pointer
    Ball* ball;      // Ball now a pointer
    int lives;
    TTF_Font* font;
    Score gameScore;
    bool quit;

public:
    Game()
        : screenWidth(1000), screenHeight(600), window(nullptr), renderer(nullptr),
          lives(3), quit(false) {

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        window = SDL_CreateWindow("Brick Breaker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return;
        }

        if (TTF_Init() == -1) {
            std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
            return;
        }

        font = TTF_OpenFont("font.ttf", 24); // Replace with your font file path
        if (!font) {
            std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
            return;
        }

        // Initialize paddle and ball
        paddle = new Paddle(350, 550, 150, 20, 5, screenWidth, renderer, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT);
        gameObjects.push_back(paddle);

        ball = new Ball(screenWidth / 2, screenHeight - 50, 10, BALL_SPEED_X, BALL_SPEED_Y, screenWidth, screenHeight, renderer);
        gameObjects.push_back(ball);

        initializeBricks();
    }

    ~Game() {
        for (auto obj : gameObjects) {
            delete obj;
        }
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void run() {
        while (!quit) {
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
                    quit = true;
                }
            }

            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            paddle->handleInput(keystate);

            updateGame();

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            for (auto obj : gameObjects) {
                obj->update();
                obj->draw();
            }

            SDL_RenderPresent(renderer);
        }
    }
private:
    void initializeBricks() {
        int rows = 5;
        int cols = screenWidth / (80 + 20);  // Brick size and padding
        int brickWidth = 80;
        int brickHeight = 30;
        int brickPadding = 20;
        int offsetX = (screenWidth - cols * (brickWidth + brickPadding)) / 2;
        int offsetY = 50;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int x = offsetX + c * (brickWidth + brickPadding);
                int y = offsetY + r * (brickHeight + brickPadding);
                SDL_Color color = getRandomColor();
                Brick* brick = new Brick(x, y, brickWidth, brickHeight, color, renderer);
                gameObjects.push_back(brick);
            }
        }
    }

    void updateGame() {
        ball->update(*paddle);

        for (auto obj : gameObjects) {
            // Cast object to Brick to check for collision
            Brick* brick = dynamic_cast<Brick*>(obj);
            if (brick && brick->isVisible() && ball->collidesWith(*brick)) {
                brick->setVisible(false);
                // Determine if the collision is horizontal or vertical
                if (ball->isCollisionHorizontal(*brick)) {
                    ball->reverseXDirection();
                } else {
                    ball->reverseYDirection();
                }
                gameScore.addPoints(10);
                std::cout << "Score: " << gameScore.getScore() << std::endl;
            }
        }

        if (ball->isOutOfBounds(screenHeight)) {
            lives--;
            std::cout << "Lives left: " << lives << std::endl;
            if (lives <= 0) {
                quit = true;
                std::cout << "Game Over!" << std::endl;
            } else {
                resetBallAndPaddle();
            }
        }
    }

    void resetBallAndPaddle() {
        paddle->reset();
        ball->reset(paddle->getX() + paddle->getWidth() / 2 - ball->getRadius(), paddle->getY() - ball->getRadius() * 2);
    }

    SDL_Color getRandomColor() {
        const std::vector<SDL_Color> colors = {
            {255, 165, 0, 255}, // Orange
            {255, 0, 0, 255},   // Red
            {224, 255, 255, 255}, // Light Blue
            {0, 0, 255, 255},   // Blue
            {255, 255, 0, 255}, // Yellow
            {128, 0, 128, 255}  // Purple
        };
        int randomIndex = rand() % colors.size();
        return colors[randomIndex];
    }
};


// Main function
void runBrickGame() {
    Game game;
    game.run();
}



// Ensure to include SDL and TTF libraries during compilation.

//g++ -std=c++11 -o brick brick_breaker.cpp -IC:\mingw_dev_lib\include\SDL2 -LC:\mingw_dev_lib\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf