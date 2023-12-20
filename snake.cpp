#include "snake.h"
#include <iostream>
#include <list>
#include <SDL.h>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <SDL_ttf.h>

enum class Direction { UP, DOWN, LEFT, RIGHT };

class Node { //linked list implementation of snake game
public:
    Node(int x, int y) : x(x), y(y) {}
    int x, y;
};

class Apple {
public:
    Apple(int grid_size) : grid_size(grid_size), x(0), y(0) {
        // Initialize apple in a random position
        randomizePosition();
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for the apple
        SDL_Rect rect = {x * grid_size, y * grid_size, grid_size, grid_size};
        SDL_RenderFillRect(renderer, &rect);
    }

    void randomizePosition() {
        x = rand() % (1000 / grid_size); // Assuming screen width is 1000
        y = rand() % (800 / grid_size);  // Assuming screen height is 800
    }

    int getX() const { return x; }
    int getY() const { return y; }

private:
    int grid_size;
    int x, y;
};

class Score {
public:
    // Default constructor
    Score() : value(0) {}

    // Constructor that accepts an initial score value
    Score(int initialValue) : value(initialValue) {}

    void addPoints(int points) {
        value += points;
    }

    int getValue() const {
        return value;
    }

    // Overload the + operator
    Score operator+(const int points) {
        return Score(value + points);
    }

private:
    int value;
};




class Snake {
public:
    Snake(int grid_size) : grid_size(grid_size), dir(Direction::RIGHT) {
        // Start with three segments
        // Head segment
        segments.push_back(Node(5, 5)); // Initial head position (5, 5)

        // Add two more segments behind the head
        segments.push_back(Node(4, 5)); // Second segment
        segments.push_back(Node(3, 5)); // Third segment
        segments.push_back(Node(2, 5)); // fourth segment
        segments.push_back(Node(1, 5)); // fifth segment
    }

    void changeDirection(Direction new_dir) {
        // Prevent the snake from reversing onto itself
        if ((dir == Direction::UP && new_dir != Direction::DOWN) ||
            (dir == Direction::DOWN && new_dir != Direction::UP) ||
            (dir == Direction::LEFT && new_dir != Direction::RIGHT) ||
            (dir == Direction::RIGHT && new_dir != Direction::LEFT)) {
            dir = new_dir;
        }
    }

    bool checkSelfCollision() {
        const Node& head = segments.front();
        for (auto it = std::next(segments.begin(), 2); it != segments.end(); ++it) {
            if (head.x == it->x && head.y == it->y) {
                return true;
            }
        }
        return false;
    }

    void update() {
        // Get current head position
        Node& head = segments.front();

        // Calculate new head position based on direction
        int new_x = head.x;
        int new_y = head.y;
        switch (dir) {
            case Direction::UP:    new_y--; break;
            case Direction::DOWN:  new_y++; break;
            case Direction::LEFT:  new_x--; break;
            case Direction::RIGHT: new_x++; break;
        }

        // Move head to the front of the list
        segments.push_front(Node(new_x, new_y));
        segments.pop_back(); // Remove last segment to simulate movement
        int max_x = 1000 / grid_size; // Total cells horizontally
        int max_y = 800 / grid_size; // Total cells vertically

        if (new_x < 0) new_x = max_x - 1;
        if (new_x >= max_x) new_x = 0;
        if (new_y < 0) new_y = max_y - 1;
        if (new_y >= max_y) new_y = 0;

        // Move head to the front of the list
        segments.push_front(Node(new_x, new_y));
        segments.pop_back();
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for the snake

        for (const auto& segment : segments) {
            SDL_Rect rect = { segment.x * grid_size, segment.y * grid_size, grid_size, grid_size };
            SDL_RenderFillRect(renderer, &rect);
        }
    }

        void grow() {
        // Add two segments at the tail's position
        const Node& tail = segments.back();
        segments.push_back(Node(tail.x, tail.y));
        segments.push_back(Node(tail.x, tail.y));
    }

    // Check if the snake's head has collided with the apple
    bool checkCollisionWithApple(const Apple& apple) {
        const Node& head = segments.front();
        return head.x == apple.getX() && head.y == apple.getY();
    }

private:
    std::list<Node> segments;
    Direction dir;
    int grid_size; // Size of each grid cell
};

class SnakeGame {
public:
    SnakeGame() : snake(20), apple(20), snakeSpeed(100), score(0), isRunning(true), window(nullptr), renderer(nullptr) {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 800, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        srand(static_cast<unsigned int>(time(nullptr))); // Seed the random number generator
    }

    ~SnakeGame() {
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
            SDL_Delay(snakeSpeed); // Delay in milliseconds, controlling snake speed
        }
    }

private:
    void handleEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
                isRunning = false;
                std::cout << "SDL_QUIT event captured. Exiting game loop." << std::endl;
            } else if (e.type == SDL_KEYDOWN) {
                handleKeyPress(e.key.keysym.sym);
            }
        }
    }

    void handleKeyPress(SDL_Keycode key) {
        switch (key) {
            case SDLK_w: snake.changeDirection(Direction::UP); break;
            case SDLK_s: snake.changeDirection(Direction::DOWN); break;
            case SDLK_a: snake.changeDirection(Direction::LEFT); break;
            case SDLK_d: snake.changeDirection(Direction::RIGHT); break;
        }
    }

    void update() {
        snake.update();

        if (snake.checkCollisionWithApple(apple)) {
            snake.grow();
            apple.randomizePosition();
            increaseSpeed();
            score = score + 10; // Use the overloaded operator to add score
            std::cout << "Score: " << score.getValue() << std::endl;
        }

        if (snake.checkSelfCollision()) {
            isRunning = false; // Game over on self-collision
        }
    }

    void increaseSpeed() {
        // Decrease the delay time to increase speed, but at a slower rate
        if (snakeSpeed > 20) {
            snakeSpeed -= 5; // Reduce speed increment to 5 milliseconds
        } else if (snakeSpeed > 10) {
            snakeSpeed -= 2; // Further reduce speed increment to 2 milliseconds for higher speeds
        }
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        snake.render(renderer);
        apple.render(renderer);

        SDL_RenderPresent(renderer);
    }

    Snake snake;
    Apple apple;
    int snakeSpeed;
    Score score; // Score attribute
    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
};

void runSnakeGame(){
    SnakeGame game;
    game.run();
}



