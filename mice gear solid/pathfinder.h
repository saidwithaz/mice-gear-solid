#pragma once


#include <utility>
#include <vector>


std::vector<std::pair<int, int>> findPathToMouse(std::pair<int, int> catPos, std::pair<int, int> mousePos, int maze[10][10]);
std::vector<std::pair<int, int>>  findNeighbors(std::pair<int, int> current, int mazeBoundary);
int costCalculator(std::pair<int, int> pos, std::pair<int, int> goal);