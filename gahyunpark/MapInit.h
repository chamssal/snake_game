#ifndef MAPINIT_H
#define MAPINIT_H

#include <vector>
#include <utility>

namespace MapInit {
    void initializeMap(int level, int width, int height, std::vector<std::vector<int>>& map, std::pair<int, int>& portal1, std::pair<int, int>& portal2);
    void placeRandomGates(int width, int height, std::vector<std::vector<int>>& map, std::pair<int, int>& portal1, std::pair<int, int>& portal2);
}

#endif // MAPINIT_H
