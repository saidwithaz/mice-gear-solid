#pragma once

#include <queue>
#include <utility>

void findPathToMouse(std::pair<int, int> catPos, std::pair<int, int> mousePos, int maze[10][10]);
void findNeighborTiles();
int costCalculator();