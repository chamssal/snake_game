#ifndef MAPINIT_H
#define MAPINIT_H
using namespace std;
#include <vector>
#include <utility>

namespace MapInit {
    void initMap(int level, int width, int height, vector<vector<int>>& map, pair<int, int>& portal1, pair<int, int>& portal2);
    void randomGates(int width, int height, vector<vector<int>>& map, pair<int, int>& portal1, pair<int, int>& portal2);
}

#endif
