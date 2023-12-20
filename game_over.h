#ifndef GAME_OVER_H
#define GAME_OVER_H

class GameOver {
public:
    GameOver(int windowWidth, int windowHeight, const char* fontPath, int fontSize, int displayDuration);
    void show();

private:
    int windowWidth;
    int windowHeight;
    const char* fontPath;
    int fontSize;
    int displayDuration;
};

#endif
