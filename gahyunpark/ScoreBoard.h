#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <ncurses.h>
#include <chrono>

using namespace std;

class ScoreBoard {
public:
    ScoreBoard(int width);

    void update(int growthItemCount, int snakeLength, int poisonItemCount, int gateCount);
    void reset();
    void draw() const;
    void startPlayTime();

private:
    int width;
    int growthItemCount;
    int snakeLength;
    int poisonItemCount;
    int gateCount;
    bool playTimeStarted;
    chrono::time_point<chrono::steady_clock> startTime;
};

#endif
