#include <ncurses.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "ScoreBoard.h"

using namespace std;

class SnakeGame {
public:
    SnakeGame(int width, int height, int level)
        : width(width), height(height), level(level), direction(-1), lastGateChangeTime(0), maxSnakeLength(10), appleCount(0), scoreBoard(width), itemExists(false), itemLastAppeared(0) {
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
            init_pair(4, COLOR_GREEN, COLOR_BLACK); // 아이템 색상 쌍 (초록 문자, 검은 배경)
        }

        map.resize(height, vector<int>(width, 0));
        initializeMap();
        initializeSnake();
        placeApple(); // 초기 사과 위치 설정
        srand(time(0)); // 랜덤 시드 초기화
        lastGateChangeTime = time(NULL);
        itemLastAppeared = time(NULL);
    }

    ~SnakeGame() {
        endwin();
    }

    void run() {
        while (true) {
            clear();
            drawMap();
            drawSnake();
            drawApple();
            drawItem();
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
            }

            // 20초마다 게이트 위치 변경
            if (difftime(time(NULL), lastGateChangeTime) >= 20) {
                changeGatePosition();
                lastGateChangeTime = time(NULL);
            }

            // 아이템 상태 변경
            if (itemExists && difftime(time(NULL), itemLastAppeared) >= 5) {
                itemExists = false;
                itemLastAppeared = time(NULL);
            } else if (!itemExists && difftime(time(NULL), itemLastAppeared) >= 10) {
                itemExists = true;
                placeItem();
                itemLastAppeared = time(NULL);
            }

            usleep(100000);
        }
    }

private:
    int width, height;
    int level;
    int maxSnakeLength;
    int appleCount;
    vector<vector<int>> map;
    vector<pair<int, int>> snake;  // Snake body parts positions
    int direction; // 0: up, 1: right, 2: down, 3: left, -1: 초기 상태
    pair<int, int> portal1, portal2; // 문 위치
    pair<int, int> apple; // 사과 위치
    pair<int, int> item; // 초기화 아이템 위치
    bool itemExists; // 아이템 존재 여부
    time_t lastGateChangeTime; // 마지막 게이트 위치 변경 시간
    time_t itemLastAppeared; // 마지막 아이템 등장 시간
    ScoreBoard scoreBoard; // 점수판

    void initializeMap() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                map[y][x] = 0; // 초기화
            }
        }

        // 1단계 맵
        if (level == 1) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                        map[y][x] = 2;
                    }
                }
            }
            map[0][0] = map[0][width - 1] = map[height - 1][0] = map[height - 1][width - 1] = 1;

            // 초기 문 위치 설정
            placeRandomGates();
        }

        // 2단계 맵
        if (level == 2) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                        map[y][x] = 2;
                    }
                }
            }
            map[0][0] = map[0][width - 1] = map[height - 1][0] = map[height - 1][width - 1] = 1;
            for (int y = height / 2 - 4; y <= height / 2 + 4; ++y) {
                map[y][width / 2] = 2; // 중앙 벽 길이를 9칸으로 확장
            }
            // 게이트 초기화
            placeRandomGates();
        }

        // 3단계 맵
        if (level == 3) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                        map[y][x] = 2;
                    }
                }
            }
            map[0][0] = map[0][width - 1] = map[height - 1][0] = map[height - 1][width - 1] = 1;

            // 고정된 벽 위치 설정
            map[6][6] = 2;
            map[6][15] = 2;
            for (int y = 8; y <= 13; ++y) {
                map[15][y] = 2;
            }

            // 게이트 초기화
            placeRandomGates();
        }
    }

    void initializeSnake() {
        snake.clear();
        snake.push_back({1, 3}); // Head
        snake.push_back({1, 2});
        snake.push_back({1, 1});
    }

    void drawMap() {
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

    void drawSnake() {
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

    void drawApple() {
        attron(COLOR_PAIR(3));
        mvprintw(apple.first, 2 * apple.second, "A");
        attroff(COLOR_PAIR(3));
    }

    void drawItem() {
        if (itemExists) {
            attron(COLOR_PAIR(4));
            mvprintw(item.first, 2 * item.second, "I");
            attroff(COLOR_PAIR(4));
        }
    }

    void changeDirection(int ch) {
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

    void moveSnake() {
        pair<int, int> newHead = snake.front();
        switch (direction) {
            case 0: newHead.first--; break; // Up
            case 1: newHead.second++; break; // Right
            case 2: newHead.first++; break; // Down
            case 3: newHead.second--; break; // Left
        }

        // 사과를 먹었을 때
        if (newHead == apple) {
            if (snake.size() < maxSnakeLength) {
                // 몸 길이 증가 - 사과를 먹었을 때 바로 증가
                snake.insert(snake.begin(), newHead);
            }
            placeApple(); // 새로운 사과 위치 설정
            appleCount++; // 먹은 사과의 수 증가
            scoreBoard.update(appleCount, snake.size());
        } else if (itemExists && newHead == item) {
            while (snake.size() > 3) {
                snake.pop_back(); // 몸 길이를 3으로 줄임
            }
            itemExists = false; // 아이템 제거
            itemLastAppeared = time(NULL); // 아이템이 사라진 시간을 설정
            snake.insert(snake.begin(), newHead);
        } else {
            snake.pop_back(); // 몸 길이 증가하지 않으면 마지막 부분 제거
            snake.insert(snake.begin(), newHead);
        }

        // 텔레포트 후에도 안전한지 검증
        if (map[newHead.first][newHead.second] == 2 || map[newHead.first][newHead.second] == 1) {
            mvprintw(height / 2, (width - 10) / 2, "Game Over");
            refresh();
            sleep(3);
            resetGame();
            return;
        }

        // 문에 도달했을 때 텔레포트 구현
        if (newHead == portal1) {
            teleport(newHead, portal2);
        } else if (newHead == portal2) {
            teleport(newHead, portal1);
        }

        scoreBoard.update(appleCount, snake.size());
    }

    void teleport(pair<int, int>& head, const pair<int, int>& target) {
        // 이전 위치와 방향을 저장
        pair<int, int> prevHead = head;
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

    bool isEdgePortal(const pair<int, int>& pos) {
        return pos.first == 0 || pos.first == height - 1 || pos.second == 0 || pos.second == width - 1;
    }

    bool checkCollision() {
        const pair<int, int>& head = snake.front();
        if (map[head.first][head.second] == 2 || map[head.first][head.second] == 1) {
            return true;
        }
        // 뱀이 자기 몸과 충돌하는지 체크
        for (size_t i = 1; i < snake.size(); ++i) {
            if (snake[i] == head) return true;
        }
        return false;
    }

    void resetGame() {
        // 키 입력 버퍼 비우기
        flushinp();

        direction = -1;
        initializeSnake();
        clear();
        initializeMap();
        placeApple(); // 새로운 사과 위치 설정
        appleCount = 0; // 사과 카운트 초기화
        scoreBoard.reset(); // 점수판 초기화
        drawMap();
        drawSnake();
        drawApple();
        scoreBoard.draw();
        refresh();
    }

    void placeApple() {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while (map[y][x] != 0 || isSnakePosition(y, x)); // 벽이나 뱀의 몸체와 겹치지 않는 위치 선택

        apple = {y, x};
    }

    void placeItem() {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while (map[y][x] != 0 || isSnakePosition(y, x) || apple == make_pair(y, x)); // 벽, 뱀의 몸체, 사과와 겹치지 않는 위치 선택

        item = {y, x};
    }

    bool isSnakePosition(int y, int x) {
        for (const auto& part : snake) {
            if (part.first == y && part.second == x) {
                return true;
            }
        }
        return false;
    }

    void placeRandomGates() {
        // 기존 게이트 위치 지우기
        if (map[portal1.first][portal1.second] == 3) map[portal1.first][portal1.second] = 2; // 벽 복원
        if (map[portal2.first][portal2.second] == 3) map[portal2.first][portal2.second] = 2; // 벽 복원

        vector<pair<int, int>> potentialGates;

        // 벽 위치 중에서 랜덤으로 선택
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (map[y][x] == 2) {
                    potentialGates.push_back({y, x});
                }
            }
        }

        // 포탈 위치 랜덤 선택
        do {
            int index = rand() % potentialGates.size();
            portal1 = potentialGates[index];
        } while (map[portal1.first][portal1.second] != 2);

        do {
            int index = rand() % potentialGates.size();
            portal2 = potentialGates[index];
        } while (map[portal2.first][portal2.second] != 2 || portal2 == portal1);

        // 새로운 게이트 위치 설정
        map[portal1.first][portal1.second] = 3;
        map[portal2.first][portal2.second] = 3;
    }

    void changeGatePosition() {
        placeRandomGates();
    }
};

int main() {
    int level;
    printf("Choose a level (1, 2, 3): ");
    scanf("%d", &level);

    SnakeGame game(21, 21, level);
    game.run();
    return 0;
}
