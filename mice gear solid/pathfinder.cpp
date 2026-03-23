#include <pathfinder.h>

#include <queue>
#include <map>

typedef std::pair<int, std::pair<int, int>> queue_item;

std::vector<std::pair<int, int>>  findNeighbors(std::pair<int, int> current, int maze[10][10]) {
	std::vector<std::pair<int, int>> neighbors;

	if (current.second != 0 &&
		maze[current.first][current.second - 1] != 1) {
		neighbors.push_back(std::make_pair(
			current.first, current.second - 1)); //left
	}
	if (current.second != 10 &&
		maze[current.first][current.second + 1] != 1) {
		neighbors.push_back(std::make_pair(
			current.first, current.second + 1)); //right
	}
	if (current.first != 0 &&
		maze[current.first - 1][current.second] != 1) {
		neighbors.push_back(std::make_pair(
			current.first - 1, current.second)); //up
	}
	if (current.first != 10 &&
		maze[current.first + 1][current.second] != 1) {
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

//idk if we actually change the maze lol...
//this uses a greedy search
std::vector<std::pair<int, int>> findPathToMouse(std::pair<int, int> catPos, std::pair<int, int> mousePos, int maze[10][10]) {


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
				frontier.push(std::make_pair(cost, nextCoord));
				cameFrom[nextCoord] = currentCoord;
			}
		}

	}

	std::vector<std::pair<int, int>> path;

	if (cameFrom.find(mousePos) == cameFrom.end() && catPos != mousePos) {
		return path;
	}

	std::pair<int, int> current = mousePos;

	while (current != catPos) {
		path.push_back(current);
		current = cameFrom[current];
	}
	path.push_back(catPos);

	std::reverse(path.begin(), path.end());

	return path;
}
