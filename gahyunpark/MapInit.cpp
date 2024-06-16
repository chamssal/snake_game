#include "MapInit.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>
using namespace std;
namespace MapInit {

void initMap(int level, int width, int height, vector<vector<int>>& map, pair<int, int>& portal1, pair<int, int>& portal2) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) 
            map[y][x] = 0; // 초기화
    }

    // 1단계 맵
    if (level == 1) {
        for (int y = 0; y < height; ++y) 
            for (int x = 0; x < width; ++x) 
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) 
                    map[y][x] = 2;
        map[0][0] = map[0][width - 1] = map[height - 1][0] = map[height - 1][width - 1] = 1;

        // 초기 문 위치 설정
        randomGates(width, height, map, portal1, portal2);
    }

    // 2단계 맵
    if (level == 2) {
        for (int y = 0; y < height; ++y) 
            for (int x = 0; x < width; ++x) 
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) 
                    map[y][x] = 2;
        map[0][0] = map[0][width - 1] = map[height - 1][0] = map[height - 1][width - 1] = 1;
        for (int y = height / 2 - 4; y <= height / 2 + 4; ++y) 
            map[y][width / 2] = 2;
        randomGates(width, height, map, portal1, portal2);
    }

    // 3단계 맵
    if (level == 3) {
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) 
                    map[y][x] = 2;
        map[0][0] = map[0][width - 1] = map[height - 1][0] = map[height - 1][width - 1] = 1;

        map[6][6] = 2;
        map[6][15] = 2;
        for (int y = 8; y <= 13; ++y) 
            map[15][y] = 2;

        randomGates(width, height, map, portal1, portal2);
    }

    // 4단계 맵
    if (level == 4) {
        for (int y = 0; y < height; ++y) 
            for (int x = 0; x < width; ++x) 
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) 
                    map[y][x] = 2;
        
        map[0][0] = map[0][width - 1] = map[height - 1][0] = map[height - 1][width - 1] = 1;

        for (int y = 2; y <= 19; ++y) 
            map[y][11] = 2;

        map[10][5] = 2;
        map[10][16] = 2;

        // 게이트 초기화
        randomGates(width, height, map, portal1, portal2);
    }
}

void placeRandomGates(int width, int height, vector<vector<int>>& map, pair<int, int>& portal1, pair<int, int>& portal2) {
    // 기존 게이트 위치 지우기
    if (map[portal1.first][portal1.second] == 3) 
        map[portal1.first][portal1.second] = 2; // 벽 복원
    if (map[portal2.first][portal2.second] == 3) 
        map[portal2.first][portal2.second] = 2; // 벽 복원

    vector<pair<int, int>> potentialGates;

    // 벽 위치 중에서 랜덤으로 선택
    for (int y = 0; y < height; ++y) 
        for (int x = 0; x < width; ++x) 
            if (map[y][x] == 2) 
                potentialGates.push_back({y, x});

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

}
