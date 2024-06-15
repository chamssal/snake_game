#ifndef MAPINITIALIZER_H
#define MAPINITIALIZER_H

#include <vector>

void initializeMap(int level, int width, int height, std::vector<std::vector<int>>& map);
void placeRandomGates(int width, int height, std::vector<std::vector<int>>& map, std::pair<int, int>& portal1, std::pair<int, int>& portal2);

#endif // MAPINITIALIZER_H
