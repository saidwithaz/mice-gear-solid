#pragma once

#include <utility>
#include <gameSettings.h>


typedef std::pair<int, std::pair<int, int>> queue_item;


/* Finds traversable neighboring tiles for a given tile.
*  Traversable tiles are floor tiles. The cat uses this function only, so no need to consider wall tiles for the mouse.
*  There is no diagonal movement, so those tiles aren't checked.
* Inputs(s):	std::pair<int, int> current  = The current tile's 2D coordinates.
*				const int maze[10][10] = A simple representation of the maze.
*
* Returns:		A list of neighboring tile coorindates.
*/
std::vector<std::pair<int, int>> findNeighbors(std::pair<int, int> current, const int maze[MAZE_SIZE][MAZE_SIZE]);


/* Finds the "cost" for travelling between 2 tiles.
*  This is a really simple heuristic using absolute distances, not factoring Manhattan movement.
*  Can be upgraded to account for Manhattan movement if enhancing algorith to A*?
* Inputs(s):	std::pair<int, int> pos  = A chosen tile's 2D coordinates.
*				std::pair<int, int> goal  = The goal tile's 2D coordinates.
*
* Returns:		The shortest distance between the two coordinates (squared for effiicent calculation).
*/
int costCalculator(std::pair<int, int> pos, std::pair<int, int> goal);

/* Calculates a path from the cat to the mouse using a simple greedy search algorithm.
* Inputs(s):	std::pair<int, int> catPos  = The cat's current 2D coordinates.
*				std::pair<int, int> mousePos = The mouse's current 2D coordinates.
*				const int maze[10][10] = A simple representation of the maze.
*
* Returns:		A list of coordinates that provides the path from cat to mouse.
*/
std::vector<std::pair<int, int>> findPathToMouse(std::pair<int, int> catPos, std::pair<int, int> mousePos, const int maze[MAZE_SIZE][MAZE_SIZE]);