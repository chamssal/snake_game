#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <ncurses.h>
#include <chrono>

class ScoreBoard {
public:
    ScoreBoard(int width);

    void update(int growthItemCount, int snakeLength, int poisonItemCount, int portalCount);
    void reset();
    void draw() const;
    void startPlayTime(); // 플레이타임 시작 함수

private:
    int width;
    int growthItemCount;
    int snakeLength;
    int poisonItemCount;
    int portalCount;
    bool playTimeStarted;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif // SCOREBOARD_H
