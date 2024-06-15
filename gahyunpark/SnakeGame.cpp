#include "SnakeGame.h"
#include "MapInit.h"
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <ncurses.h>

SnakeGame::SnakeGame(int width, int height, int level)
    : width(width), height(height), level(level), direction(-1), lastGateChangeTime(0), maxSnakeLength(13), growthItemCount(0), poisonItemCount(0), portalCount(0), scoreBoard(width), itemExists(false), itemLastAppeared(0), reverseControlItemExists(false), reverseControlItemLastAppeared(0), reverseControlActive(false), reverseControlActivatedAt(0), appleLastAppeared(0) {
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(100);

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK); // 기본 색상 쌍 (흰색 문자, 검은 배경)
        init_pair(2, COLOR_BLUE, COLOR_BLACK);  // 게이트 색상 쌍 (파란 문자, 검은 배경)
        init_pair(3, COLOR_RED, COLOR_BLACK);   // 사과 색상 쌍 (빨간 문자, 검은 배경)
        init_pair(4, COLOR_GREEN, COLOR_BLACK); // 초기화 아이템 색상 쌍 (초록 문자, 검은 배경)
        init_pair(5, COLOR_YELLOW, COLOR_BLACK); // 방향 바꾸기 아이템 색상 쌍 (노란 문자, 검은 배경)
    }

    map.resize(height, std::vector<int>(width, 0));
    initializeMap(level, width, height, map);
    initializeSnake();
    placeApple(); // 초기 사과 위치 설정
    srand(time(0)); // 랜덤 시드 초기화
    lastGateChangeTime = time(NULL);
    itemLastAppeared = time(NULL);
    reverseControlItemLastAppeared = time(NULL);
    appleLastAppeared = time(NULL);
}

SnakeGame::~SnakeGame() {
    endwin();
}

void SnakeGame::run() {
    while (true) {
        clear();
        drawMap();
        drawSnake();
        drawApple();
        drawItem();
        drawReverseControlItem();
        scoreBoard.draw();
        refresh();

        int ch = getch();
        if (ch == 'q') break;

        if (ch != ERR) {
            changeDirection(ch);
        }

        if (direction != -1) {
            moveSnake();
        }

        if (checkCollision()) {
            mvprintw(height / 2, (width - 10) / 2, "Game Over");
            refresh();
            sleep(3);
            resetGame();
            continue;
        }

        // 20초마다 게이트 위치 변경
        if (difftime(time(NULL), lastGateChangeTime) >= 20) {
            changeGatePosition();
            lastGateChangeTime = time(NULL);
        }

        // 5초마다 사과 위치 변경
        if (difftime(time(NULL), appleLastAppeared) >= 5) {
            placeApple();
            appleLastAppeared = time(NULL);
        }

        // 초기화 아이템 상태 변경
        if (itemExists && difftime(time(NULL), itemLastAppeared) >= 5) {
            itemExists = false;
            itemLastAppeared = time(NULL);
        } else if (!itemExists && difftime(time(NULL), itemLastAppeared) >= 10) {
            itemExists = true;
            placeItem();
            itemLastAppeared = time(NULL);
        }

        // 방향 바꾸기 아이템 상태 변경
        if (reverseControlItemExists && difftime(time(NULL), reverseControlItemLastAppeared) >= 5) {
            reverseControlItemExists = false;
            reverseControlItemLastAppeared = time(NULL);
        } else if (!reverseControlItemExists && difftime(time(NULL), reverseControlItemLastAppeared) >= 15) {
            reverseControlItemExists = true;
            placeReverseControlItem();
            reverseControlItemLastAppeared = time(NULL);
        }

        // 방향 바꾸기 효과 상태 변경
        if (reverseControlActive && difftime(time(NULL), reverseControlActivatedAt) >= 5) {
            reverseControlActive = false;
        }

        usleep(100000);
    }
}

void SnakeGame::initializeSnake() {
    snake.clear();
    snake.push_back({1, 3}); // Head
    snake.push_back({1, 2});
    snake.push_back({1, 1});
}

void SnakeGame::drawMap() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int displayX = 2 * x; // x 좌표를 두 배로 해서 비율 맞춤
            if (map[y][x] == 2) {
                attron(COLOR_PAIR(1));
                mvprintw(y, displayX, "#");
                attroff(COLOR_PAIR(1));
            } else if (map[y][x] == 1) {
                attron(COLOR_PAIR(1));
                mvprintw(y, displayX, "+");
                attroff(COLOR_PAIR(1));
            } else if (map[y][x] == 3) {
                attron(COLOR_PAIR(2));
                mvprintw(y, displayX, "P"); // 문 표시
                attroff(COLOR_PAIR(2));
            }
        }
    }
}

void SnakeGame::drawSnake() {
    for (size_t i = 0; i < snake.size(); ++i) {
        int y = snake[i].first;
        int x = 2 * snake[i].second; // x 좌표를 두 배로 해서 비율 맞춤
        if (i == 0) {
            mvprintw(y, x, "O");  // Head
        } else {
            mvprintw(y, x, "o");  // Body
        }
    }
}

void SnakeGame::drawApple() {
    attron(COLOR_PAIR(3));
    mvprintw(apple.first, 2 * apple.second, "A");
    attroff(COLOR_PAIR(3));
}

void SnakeGame::drawItem() {
    if (itemExists) {
        attron(COLOR_PAIR(4));
        mvprintw(item.first, 2 * item.second, "I");
        attroff(COLOR_PAIR(4));
    }
}

void SnakeGame::drawReverseControlItem() {
    if (reverseControlItemExists) {
        attron(COLOR_PAIR(5));
        mvprintw(reverseControlItem.first, 2 * reverseControlItem.second, "R");
        attroff(COLOR_PAIR(5));
    }
}

void SnakeGame::changeDirection(int ch) {
    if (reverseControlActive) {
        switch (ch) {
            case KEY_UP:
            case 'w':
                if (direction != 2) direction = 2;
                break;
            case KEY_RIGHT:
            case 'd':
                if (direction != 3) direction = 3;
                break;
            case KEY_DOWN:
            case 's':
                if (direction != 0) direction = 0;
                break;
            case KEY_LEFT:
            case 'a':
                if (direction != 1) direction = 1;
                break;
        }
    } else {
        switch (ch) {
            case KEY_UP:
            case 'w':
                if (direction != 2) direction = 0;
                break;
            case KEY_RIGHT:
            case 'd':
                if (direction != 3) direction = 1;
                break;
            case KEY_DOWN:
            case 's':
                if (direction != 0) direction = 2;
                break;
            case KEY_LEFT:
            case 'a':
                if (direction != 1) direction = 3;
                break;
        }
    }
}

void SnakeGame::moveSnake() {
    std::pair<int, int> newHead = snake.front();
    switch (direction) {
        case 0: newHead.first--; break; // Up
        case 1: newHead.second++; break; // Right
        case 2: newHead.first++; break; // Down
        case 3: newHead.second--; break; // Left
    }

    if (newHead != std::make_pair(1, 3)) {
        scoreBoard.startPlayTime();
    }

    // 텔레포트 후에도 안전한지 검증
    if (newHead == portal1) {
        teleport(newHead, portal2);
        portalCount++; // 포탈 통과 카운트 증가
    } else if (newHead == portal2) {
        teleport(newHead, portal1);
        portalCount++; // 포탈 통과 카운트 증가
    }

    // 텔레포트 후에 다시 위치 검증
    if (map[newHead.first][newHead.second] == 2 || map[newHead.first][newHead.second] == 1) {
        mvprintw(height / 2, (width - 10) / 2, "Game Over");
        refresh();
        sleep(3);
        resetGame();
        return;
    }

    // 사과를 먹었을 때
    if (newHead == apple) {
        if (snake.size() < maxSnakeLength) {
            snake.insert(snake.begin(), newHead); // 몸 길이 1 증가
        }
        placeApple(); // 새로운 사과 위치 설정
        growthItemCount++; // 먹은 사과의 수 증가
        appleLastAppeared = time(NULL); // 사과가 등장한 시간을 갱신
        scoreBoard.update(growthItemCount, snake.size(), poisonItemCount, portalCount);
    } else if (itemExists && newHead == item) {
        if (snake.size() > 3) {
            snake.pop_back(); // 몸 길이 1 감소
            drawSnake(); // 현재 뱀의 길이를 출력
            refresh();
        } else {
            snake.pop_back(); // 길이가 2로 줄어들게 함
            drawSnake(); // 현재 뱀의 길이를 출력
            refresh();
            scoreBoard.update(growthItemCount, snake.size(), poisonItemCount, portalCount);
            mvprintw(height / 2, (width - 10) / 2, "Game Over");
            refresh();
            sleep(3);
            resetGame();
            return;
        }
        itemExists = false; // 아이템 제거
        itemLastAppeared = time(NULL); // 아이템이 사라진 시간을 설정
        poisonItemCount++; // 초기화 아이템 카운트 증가
    } else if (reverseControlItemExists && newHead == reverseControlItem) {
        reverseControlActive = true; // 방향 바꾸기 활성화
        reverseControlActivatedAt = time(NULL); // 방향 바꾸기가 활성화된 시간을 설정
        reverseControlItemExists = false; // 아이템 제거
        reverseControlItemLastAppeared = time(NULL); // 아이템이 사라진 시간을 설정
    } else {
        snake.pop_back(); // 몸 길이 증가하지 않으면 마지막 부분 제거
        snake.insert(snake.begin(), newHead);
    }

    scoreBoard.update(growthItemCount, snake.size(), poisonItemCount, portalCount);
}

void SnakeGame::teleport(std::pair<int, int>& head, const std::pair<int, int>& target) {
    // 이전 위치와 방향을 저장
    int prevDirection = direction;

    head = target; // 새로운 위치로 이동

    // 1. 나오는 포탈이 21*21 맵의 가장자리에 위치한 포탈이라면 방향과 상관없이 사각형 안쪽 방향, 벽과 수직으로 나오게 한다.
    if (isEdgePortal(head)) {
        if (head.first == 0) {
            direction = 2; head.first++;
        } else if (head.first == height - 1) {
            direction = 0; head.first--;
        } else if (head.second == 0) {
            direction = 1; head.second++;
        } else if (head.second == width - 1) {
            direction = 3; head.second--;
        }
    }
    // 2. 나오는 포탈이 가장자리에 위치하지 않으면, 들어간 방향으로 나온다.
    else {
        direction = prevDirection;

        // 3. 나오는 방향 바로 앞에 벽이 있으면, 들어간 방향에서 90도 시계방향 회전해서 나온다.
        switch (direction) {
            case 0: // Up
                if (map[head.first - 1][head.second] == 2 || map[head.first - 1][head.second] == 1) direction = 1; // Right
                break;
            case 1: // Right
                if (map[head.first][head.second + 1] == 2 || map[head.first][head.second + 1] == 1) direction = 2; // Down
                break;
            case 2: // Down
                if (map[head.first + 1][head.second] == 2 || map[head.first + 1][head.second] == 1) direction = 3; // Left
                break;
            case 3: // Left
                if (map[head.first][head.second - 1] == 2 || map[head.first][head.second - 1] == 1) direction = 0; // Up
                break;
        }
    }
}

bool SnakeGame::isEdgePortal(const std::pair<int, int>& pos) {
    return pos.first == 0 || pos.first == height - 1 || pos.second == 0 || pos.second == width - 1;
}

bool SnakeGame::checkCollision() {
    const std::pair<int, int>& head = snake.front();
    if (map[head.first][head.second] == 2 || map[head.first][head.second] == 1) {
        return true;
    }
    // 뱀이 자기 몸과 충돌하는지 체크
    for (size_t i = 1; i < snake.size(); ++i) {
        if (snake[i] == head) return true;
    }
    return false;
}

void SnakeGame::resetGame() {
    // 키 입력 버퍼 비우기
    flushinp();

    direction = -1;
    initializeSnake();
    clear();
    initializeMap(level, width, height, map);
    placeApple(); // 새로운 사과 위치 설정
    growthItemCount = 0; // 사과 카운트 초기화
    poisonItemCount = 0; // 초기화 아이템 카운트 초기화
    portalCount = 0; // 포탈 통과 카운트 초기화
    scoreBoard.reset(); // 점수판 초기화
    drawMap();
    drawSnake();
    drawApple();
    scoreBoard.draw();
    refresh();
}

void SnakeGame::placeApple() {
    int x, y;
    do {
        x = rand() % width;
        y = rand() % height;
    } while (map[y][x] != 0 || isSnakePosition(y, x)); // 벽이나 뱀의 몸체와 겹치지 않는 위치 선택

    apple = {y, x};
}

void SnakeGame::placeItem() {
    int x, y;
    do {
        x = rand() % width;
        y = rand() % height;
    } while (map[y][x] != 0 || isSnakePosition(y, x) || apple == std::make_pair(y, x)); // 벽, 뱀의 몸체, 사과와 겹치지 않는 위치 선택

    item = {y, x};
}

void SnakeGame::placeReverseControlItem() {
    int x, y;
    do {
        x = rand() % width;
        y = rand() % height;
    } while (map[y][x] != 0 || isSnakePosition(y, x) || apple == std::make_pair(y, x)); // 벽, 뱀의 몸체, 사과와 겹치지 않는 위치 선택

    reverseControlItem = {y, x};
}

bool SnakeGame::isSnakePosition(int y, int x) {
    for (const auto& part : snake) {
        if (part.first == y && part.second == x) {
            return true;
        }
    }
    return false;
}

void SnakeGame::placeRandomGates() {
    ::placeRandomGates(width, height, map, portal1, portal2);
}

void SnakeGame::changeGatePosition() {
    placeRandomGates();
}
