#ifndef SNAKEGAME_H
#define SNAKEGAME_H
using namespace std;
#include <ncurses.h>
#include <vector>
#include <utility>
#include <ctime>
#include "ScoreBoard.h"
#include "MapInit.h"

class SnakeGame {
public:
    SnakeGame(int width, int height, int level);
    ~SnakeGame();
    void run();

private:
    int width, height;
    int level;
    int maxlength;
    int growthItemCount;
    int poisonItemCount;
    int gateCount;
    vector<vector<int>> map;
    vector<pair<int, int>> snake;  // Snake body parts positions
    int direction; // 0: up, 1: right, 2: down, 3: left, -1: 초기 상태
    pair<int, int> gate1, gate2; // 문 위치
    pair<int, int> growthItem; // growth 위치
    pair<int, int> poisonItem; // poison 위치
    bool isPoison; // poison 존재 여부
    time_t lastGateChangeTime; // 마지막 게이트 위치 변경 시간
    time_t poisonLastAppeared; // 마지막 poison 등장 시간
    time_t growthLastAppeared; // 마지막 growth 등장 시간
    pair<int, int> reverseItem; // 방향 바꾸기 아이템 위치
    bool reverseItemExists; // 방향 바꾸기 아이템 존재 여부
    time_t reverseItemLastAppeared; // 마지막 방향 바꾸기 아이템 등장 시간
    bool reverseActive; // 방향 바꾸기 활성화 여부
    time_t reverseActivatedAt; // 방향 바꾸기 활성화 시간
    ScoreBoard scoreBoard; // 점수판

    void initSnake();
    void drawMap();
    void drawSnake();
    void drawGrowth();
    void drawPoison();
    void drawReverseItem();
    void changeDirection(int ch);
    void moveSnake();
    void teleport(pair<int, int>& head, const pair<int, int>& target);
    bool isEdgeGate(const pair<int, int>& pos);
    bool checkCollision();
    void resetGame();
    void placeGrowth();
    void placePoison();
    void placeReverseItem();
    bool isSnakePosition(int y, int x);
    void changeGatePosition();
    bool checkClearCondition();
    void nextLevel();
    void gameOver(); // 게임 오버 함수 선언
};

#endif
