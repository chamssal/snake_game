// ScoreBoard.cpp
#include "ScoreBoard.h"

ScoreBoard::ScoreBoard(int width) : width(width), appleCount(0), snakeLength(0) {}

void ScoreBoard::update(int appleCount, int snakeLength) {
    this->appleCount = appleCount;
    this->snakeLength = snakeLength;
}

void ScoreBoard::reset() {
    appleCount = 0;
    snakeLength = 0;
}

void ScoreBoard::draw() const {
    mvprintw(0, width * 2 + 2, "Scoreboard");
    mvprintw(2, width * 2 + 2, "Apples: %d", appleCount);
    mvprintw(3, width * 2 + 2, "Length: %d", snakeLength);
}
