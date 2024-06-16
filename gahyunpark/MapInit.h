#ifndef MAPINIT_H
#define MAPINIT_H

#include <vector>
#include <utility>

using namespace std;

namespace MapInit {
    void initMap(int level, int width, int height, vector<vector<int>>& map, pair<int, int>& portal1, pair<int, int>& portal2);
    void placeRandomGates(int width, int height, vector<vector<int>>& map, pair<int, int>& portal1, pair<int, int>& portal2);
}

#endif // MAPINIT_H
