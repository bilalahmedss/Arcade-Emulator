#include "tetris.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <vector>
#include <iostream>
#include <ctime>
#define BOARD_WIDTH (WIDTH / TILE_SIZE)
#define BOARD_HEIGHT (HEIGHT / TILE_SIZE)

#define WIDTH 500
#define HEIGHT 800
#define TILE_SIZE (WIDTH / 20)
int dropDelay = 500;
int lastDropTime = 0;


bool running;
int score = 0;
SDL_Renderer* renderer;
SDL_Window* window;

int frameCount, timerFPS, lastFrame, fps;
bool left, right, up, down;

struct block {
 SDL_Color color;
 bool active;
};

struct shape {
 SDL_Color color;
 bool matrix[4][4];
 double x, y;
 int size;
};

shape blocks[7] = {{{255,165,0},
{{0,0,1,0} // L BLOCK
,{1,1,1,0}
,{0,0,0,0}
,{0,0,0,0}
},5,4,3}
,{{255,0,0}, // Z BLOCK
{{1,1,0,0}
,{0,1,1,0}
,{0,0,0,0}
,{0,0,0,0}
},5,4,3}
,{{224,255,255}, // I BLOCK
{{1,1,1,1}
,{0,0,0,0}
,{0,0,0,0}
,{0,0,0,0}
},5,4,4}
,{{0,0,255}, // J BLOCK
{{1,0,0,0}
,{1,1,1,0}
,{0,0,0,0}
,{0,0,0,0}
},5,4,3}
,{{255,255,0}, // O BLOCK
{{1,1,0,0}
,{1,1,0,0}
,{0,0,0,0}
,{0,0,0,0}
},5,4,2}
,{{0,0,255}, // S BLOCK
{{0,1,1,0}
,{1,1,0,0}
,{0,0,0,0}
,{0,0,0,0}
},5,4,3}
,{{128,0,128}, // T BLOCK
{{0,1,0,0}
,{1,1,1,0}
,{0,0,0,0}
,{0,0,0,0}
},5,4,3}}, cur;

shape reverseCols(shape s) {
    shape tmp = s;
    for(int i=0; i<s.size; i++) {
        for(int j=0; j<s.size/2; j++) {
            bool t = s.matrix[i][j];
            tmp.matrix[i][j]=s.matrix[i][s.size-j-1];
            tmp.matrix[i][s.size-j-1]=t;
        }
    }
    return tmp;
}
shape transpose(shape s) {
    shape tmp = s;
    for(int i=0; i<s.size; i++) {
        for(int j=0; j<s.size; j++) {
            tmp.matrix[i][j]=s.matrix[j][i];
        }
    }
    return tmp;
}
SDL_Rect rect;
block board[BOARD_WIDTH][BOARD_HEIGHT] = {};
void drawBoard() {
    for (int x = 0; x < BOARD_WIDTH; ++x) {
        for (int y = 0; y < BOARD_HEIGHT; ++y) {
            if (board[x][y].active) {
                rect.x = x * TILE_SIZE;
                rect.y = y * TILE_SIZE;
                SDL_SetRenderDrawColor(renderer, board[x][y].color.r, board[x][y].color.g, board[x][y].color.b, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 219, 219, 219, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }
}


bool checkCollision(shape& s) {
    for (int i = 0; i < s.size; i++) {
        for (int j = 0; j < s.size; j++) {
            if (s.matrix[i][j]) {
                int boardX = static_cast<int>(s.x) + i;
                int boardY = static_cast<int>(s.y) + j;
                if (boardX < 0 || boardX >= BOARD_WIDTH || boardY >= BOARD_HEIGHT || board[boardX][boardY].active) {
                    return true; // Collision detected
                }
            }
        }
    }
    return false; // No collision
}

void rotate() {
    shape tmp = cur;
    tmp = reverseCols(transpose(tmp)); // Rotate the temporary shape

    if (!checkCollision(tmp)) {
        cur = tmp; // Only rotate if no collision
    }
}



void draw(shape s) {
    for(int i=0; i<s.size; i++) {
        for(int j=0; j<s.size; j++) {
            if(s.matrix[i][j]) {
                rect.x=(s.x+i)*TILE_SIZE; rect.y=(s.y+j)*TILE_SIZE;
                SDL_SetRenderDrawColor(renderer, s.color.r, s.color.g, s.color.b, 255);
                SDL_RenderFillRect(renderer, &rect);
                SDL_SetRenderDrawColor(renderer, 219, 219, 219, 255);
                SDL_RenderDrawRect(renderer, &rect);
            }
        }
    }
}

class CheckMove {
public:
    CheckMove(block board[][BOARD_HEIGHT], shape* currentShape) : board(board), currentShape(currentShape) {}

    // Declare friend functions
    friend bool canMoveDown(const CheckMove& cm);
    friend bool canMoveLeft(const CheckMove& cm);
    friend bool canMoveRight(const CheckMove& cm);

private:
    block (*board)[BOARD_HEIGHT];
    shape* currentShape;
};

// Friend function implementations
bool canMoveDown(const CheckMove& cm) {
    for (int i = 0; i < cm.currentShape->size; ++i) {
        for (int j = 0; j < cm.currentShape->size; ++j) {
            if (cm.currentShape->matrix[i][j]) {
                int boardX = static_cast<int>(cm.currentShape->x) + i;
                int boardY = static_cast<int>(cm.currentShape->y) + j + 1;
                if (boardY >= BOARD_HEIGHT || cm.board[boardX][boardY].active)
                    return false;
            }
        }
    }
    return true;
}

bool canMoveLeft(const CheckMove& cm) {
    for (int i = 0; i < cm.currentShape->size; ++i) {
        for (int j = 0; j < cm.currentShape->size; ++j) {
            if (cm.currentShape->matrix[i][j]) {
                int boardX = static_cast<int>(cm.currentShape->x) + i - 1;
                int boardY = static_cast<int>(cm.currentShape->y) + j;
                if (boardX < 0 || cm.board[boardX][boardY].active)
                    return false;
            }
        }
    }
    return true;
}

bool canMoveRight(const CheckMove& cm) {
    for (int i = 0; i < cm.currentShape->size; ++i) {
        for (int j = 0; j < cm.currentShape->size; ++j) {
            if (cm.currentShape->matrix[i][j]) {
                int boardX = static_cast<int>(cm.currentShape->x) + i + 1;
                int boardY = static_cast<int>(cm.currentShape->y) + j;
                if (boardX >= BOARD_WIDTH || cm.board[boardX][boardY].active)
                    return false;
            }
        }
    }
    return true;
}


class LineFull {
public:
    // Constructor
    LineFull(block board[][BOARD_HEIGHT]);

    // Public methods
    void checkLines();

private:
    // Private member variables
    block (*board)[BOARD_HEIGHT];

    // Private methods
    void checkLines(int line);
    void moveLinesDown(int clearedLine);
};

// Constructor Implementation
LineFull::LineFull(block board[][BOARD_HEIGHT]) : board(board) {}

// Public Methods Implementation
void LineFull::checkLines() {
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool lineFull = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!board[x][y].active) {
                lineFull = false;
                break;
            }
        }

        if (lineFull) {
            checkLines(y);
            moveLinesDown(y);
            y++; // Check this line again after moving lines down
            score += 10;
            std::cout<<"score : "<<score<<std::endl;
        }
    }
}

// Private Methods Implementation
void LineFull::checkLines(int line) {  // function overloading
    for (int x = 0; x < BOARD_WIDTH; x++) {
        board[x][line].active = false;
    }
}

void LineFull::moveLinesDown(int clearedLine) {
    for (int y = clearedLine; y > 0; y--) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            board[x][y] = board[x][y - 1];
        }
    }
}



class ShapePlacer {
public:
    ShapePlacer(block b[][BOARD_HEIGHT], shape* c, shape bks[]) 
    : board(b), cur(c), blocks(bks) {}

    void placeShapeOnBoard();

private:
    block (*board)[BOARD_HEIGHT];
    shape* cur;
    shape* blocks;

    void checkGameOver();
    void generateNewShape();
};

void ShapePlacer::placeShapeOnBoard() {
    checkGameOver();

    // Set the board cells to active and update the color
    for (int i = 0; i < cur->size; ++i) {
        for (int j = 0; j < cur->size; ++j) {
            if (cur->matrix[i][j]) {
                int boardX = static_cast<int>(cur->x) + i;
                int boardY = static_cast<int>(cur->y) + j;
                board[boardX][boardY].active = true;
                board[boardX][boardY].color = cur->color;
            }
        }
    }

    LineFull lineFull(board);
    lineFull.checkLines();

    generateNewShape();
}

void ShapePlacer::checkGameOver() {
    for (int i = 0; i < cur->size; ++i) {
        for (int j = 0; j < cur->size; ++j) {
            if (cur->matrix[i][j] && static_cast<int>(cur->y) + j <= 0) {
                // Game over condition
                std::cout << "Game Over!" << std::endl;
                exit(0); // or use a global running variable to stop the game loop
            }
        }
    }
}

void ShapePlacer::generateNewShape() {
    // Generate a new shape
    *cur = blocks[rand() % 7];
    cur->x = BOARD_WIDTH / 2 - cur->size / 2;
    cur->y = 0;
}


class TetrisGame{
public:
void update() {
        CheckMove checkMove(board, &cur); // Create an instance of CheckMove

        int currentTime = SDL_GetTicks();

        // Auto-move the piece down every dropDelay milliseconds
        if (currentTime - lastDropTime > dropDelay) {
            if (canMoveDown(checkMove)) {
                cur.y++;
            } else {
                // Example usage
                ShapePlacer shapePlacer(board, &cur, blocks);
                shapePlacer.placeShapeOnBoard();

            }
            lastDropTime = currentTime;
        }

        // User input to move the shape down faster
        if (down) {
            if (canMoveDown(checkMove)) {
                cur.y++;
                lastDropTime = currentTime; // Reset the timer after manual move down
            } else {
                // Example usage
                    ShapePlacer shapePlacer(board, &cur, blocks);
                    shapePlacer.placeShapeOnBoard();

            }
        }

        // Left and right movement
        if (left && canMoveLeft(checkMove)) cur.x--;
        if (right && canMoveRight(checkMove)) cur.x++;

        // Rotate the piece on user input
        if (up) rotate();
    }

    void input() {
        up = down = left = right = 0;
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if( e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) running=false;
            switch(e.type) {
                case SDL_KEYDOWN: // Use KEYDOWN for immediate response
                    switch(e.key.keysym.sym) {
                        case SDLK_LEFT:
                            left = 1;
                            break;
                        case SDLK_RIGHT:
                            right = 1;
                            break;
                        case SDLK_UP:
                            up = 1;
                            break;
                        case SDLK_DOWN:
                            down = 1;
                            break;
                        case SDLK_ESCAPE:
                            running=false;
                            break;
                    }
                    break;
            }
        }
    }



    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw all the active blocks on the board first.
        drawBoard();

        // Draw the current moving shape.
        draw(cur);

        SDL_RenderPresent(renderer);
    }



    void run() {
        srand(time(NULL));
        cur=blocks[rand() % 7];
        rect.w=rect.h=TILE_SIZE;
        running=1;
        static int lastTime=0;
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed at SDL_Init()" << std::endl;
        if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "Failed at SDL_CreateWindowAndRenderer()" << std::endl;
        SDL_SetWindowTitle(window, "Tetris");
        SDL_Event e;

        while(running) {
            lastFrame=SDL_GetTicks();
            if(lastFrame>=(lastTime+1000)) {
                lastTime=lastFrame;
                fps=frameCount;
                frameCount=0;
            }
            update();
            input();
            render();
        }
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

void runTetrisGame() {
    TetrisGame game;
    game.run();
}
/* compilation 
g++ -std=c++11 -IC:\mingw_dev_lib\include\SDL2 -LC:\mingw_dev_lib\lib -o Tetris tetris.cpp -lmingw32 -lSDL2main -lSDL2
./tetris.exe
*/