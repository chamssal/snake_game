// ScoreBoard.h
#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <ncurses.h>

class ScoreBoard {
public:
    ScoreBoard(int width);
    void update(int appleCount, int snakeLength);
    void reset();
    void draw() const;

private:
    int width;
    int appleCount;
    int snakeLength;
};

#endif // SCOREBOARD_H
