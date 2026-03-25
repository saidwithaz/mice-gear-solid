#include <queue>
#include <map>
#include <pathfinder.h>



std::vector<std::pair<int, int>> findNeighbors(std::pair<int, int> current, const int maze[MAZE_SIZE][MAZE_SIZE]) {
	std::vector<std::pair<int, int>> neighbors;

	if (current.second != 0 &&
		maze[current.first][current.second - 1] != Tile::WALL) {
		neighbors.push_back(std::make_pair(
			current.first, current.second - 1)); //left
	}
	if (current.second != MAZE_SIZE &&
		maze[current.first][current.second + 1] != Tile::WALL) {
		neighbors.push_back(std::make_pair(
			current.first, current.second + 1)); //right
	}
	if (current.first != 0 &&
		maze[current.first - 1][current.second] != Tile::WALL) {
		neighbors.push_back(std::make_pair(
			current.first - 1, current.second)); //up
	}
	if (current.first != MAZE_SIZE &&
		maze[current.first + 1][current.second] != Tile::WALL) {
		neighbors.push_back(std::make_pair(
			current.first + 1, current.second)); //down
	}

	return neighbors;
}



int costCalculator(std::pair<int, int> pos, std::pair<int, int> goal){
	float deltaX = abs(pos.first - goal.first);
	float deltaY = abs(pos.second - goal.second);
	int distanceToGoal = (deltaX * deltaX) + (deltaY * deltaY);

	return distanceToGoal;
}


std::vector<std::pair<int, int>> findPathToMouse(std::pair<int, int> catPos, std::pair<int, int> mousePos, const int maze[MAZE_SIZE][MAZE_SIZE]) {
	std::priority_queue<queue_item, std::vector<queue_item>, std::greater<queue_item>> frontier;
	frontier.push({ 0, catPos });
	std::map<std::pair<int, int>, std::pair<int, int>> cameFrom;

	cameFrom[catPos] = catPos;

	while (!frontier.empty()) {
		std::pair<int, int> currentCoord = frontier.top().second;
		frontier.pop();

		if (currentCoord == mousePos) {
			break;
		}

		std::vector<std::pair<int, int>> neighborCoords = findNeighbors(currentCoord, maze);


		for (auto nextCoord : neighborCoords) {
			if (!cameFrom.contains(nextCoord)) {
				int cost = costCalculator(nextCoord, mousePos);

				//TODO: Update smarter pathfinding for cat so it can go around walls better :)
				//if (maze[nextCoord.first][nextCoord.second] == Tile::WALL) {
				//	cost -= 1;
				//}

				frontier.push(std::make_pair(cost, nextCoord));
				cameFrom[nextCoord] = currentCoord;
			}
		}

	}

	std::vector<std::pair<int, int>> pathFound;

	if (cameFrom.find(mousePos) == cameFrom.end() && catPos != mousePos) {
		return pathFound;
	}

	std::pair<int, int> current = mousePos;

	while (current != catPos) {
		pathFound.push_back(current);
		current = cameFrom[current];
	}
	pathFound.push_back(catPos);

	std::reverse(pathFound.begin(), pathFound.end());

	return pathFound;
}
