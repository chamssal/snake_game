#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <ncurses.h>
#include <vector>
#include <utility>
#include <ctime>
#include "ScoreBoard.h"

class SnakeGame {
public:
    SnakeGame(int width, int height, int level);
    ~SnakeGame();
    void run();

private:
    int width, height;
    int level;
    int maxSnakeLength;
    int growthItemCount;
    int poisonItemCount;
    int portalCount;
    std::vector<std::vector<int>> map;
    std::vector<std::pair<int, int>> snake;  // Snake body parts positions
    int direction; // 0: up, 1: right, 2: down, 3: left, -1: 초기 상태
    std::pair<int, int> portal1, portal2; // 문 위치
    std::pair<int, int> apple; // 사과 위치
    std::pair<int, int> item; // 초기화 아이템 위치
    bool itemExists; // 초기화 아이템 존재 여부
    time_t lastGateChangeTime; // 마지막 게이트 위치 변경 시간
    time_t itemLastAppeared; // 마지막 초기화 아이템 등장 시간
    time_t appleLastAppeared; // 마지막 사과 등장 시간
    std::pair<int, int> reverseControlItem; // 방향 바꾸기 아이템 위치
    bool reverseControlItemExists; // 방향 바꾸기 아이템 존재 여부
    time_t reverseControlItemLastAppeared; // 마지막 방향 바꾸기 아이템 등장 시간
    bool reverseControlActive; // 방향 바꾸기 활성화 여부
    time_t reverseControlActivatedAt; // 방향 바꾸기 활성화 시간
    ScoreBoard scoreBoard; // 점수판

    void initializeMap();
    void initializeSnake();
    void drawMap();
    void drawSnake();
    void drawApple();
    void drawItem();
    void drawReverseControlItem();
    void changeDirection(int ch);
    void moveSnake();
    void teleport(std::pair<int, int>& head, const std::pair<int, int>& target);
    bool isEdgePortal(const std::pair<int, int>& pos);
    bool checkCollision();
    void resetGame();
    void placeApple();
    void placeItem();
    void placeReverseControlItem();
    bool isSnakePosition(int y, int x);
    void placeRandomGates();
    void changeGatePosition();
};

#endif // SNAKEGAME_H
