#include "ScoreBoard.h"

ScoreBoard::ScoreBoard(int width) 
    : width(width), growthItemCount(0), snakeLength(0), poisonItemCount(0), portalCount(0), playTimeStarted(false) {}

void ScoreBoard::update(int growthItemCount, int snakeLength, int poisonItemCount, int portalCount) {
    this->growthItemCount = growthItemCount;
    this->snakeLength = snakeLength;
    this->poisonItemCount = poisonItemCount;
    this->portalCount = portalCount;
}

void ScoreBoard::reset() {
    growthItemCount = 0;
    snakeLength = 0;
    poisonItemCount = 0;
    portalCount = 0;
    playTimeStarted = false;
}

void ScoreBoard::draw() const {
    mvprintw(0, width * 2 + 2, "Mission");
    mvprintw(2, width * 2 + 2, "Length: %d/13", (snakeLength>13)?13:snakeLength);
    mvprintw(3, width * 2 + 2, "Growth Item: %d/8", (growthItemCount>8)?8:growthItemCount);
    mvprintw(4, width * 2 + 2, "Poison Item: %d/3", (poisonItemCount>3)?3:poisonItemCount);
    mvprintw(5, width * 2 + 2, "Portals: %d/5", (portalCount>5)?5:portalCount);
    
    if (playTimeStarted) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startTime).count();
        mvprintw(6, width * 2 + 2, "Playtime: %lds", elapsed);
    } else {
        mvprintw(6, width * 2 + 2, "Playtime: 0s");
    }
}

void ScoreBoard::startPlayTime() {
    if (!playTimeStarted) {
        startTime = std::chrono::steady_clock::now();
        playTimeStarted = true;
    }
}
