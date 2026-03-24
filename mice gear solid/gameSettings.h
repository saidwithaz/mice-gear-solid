#pragma once

/* Defines global settings and constants for the game. 
*  The following shouldn't really be changed...
*/
constexpr int OBJECT_SIZE = 64;
constexpr int WINDOW_SIZE = 640;

/* Tinker speed here (cat has lower numbers for higher speed).
*  TODO: rename cat speed??
*/
constexpr int CAT_CALC_SPEED = 500;
constexpr int CAT_SPEED = 15;
constexpr float MOUSE_SPEED = 0.25f;

constexpr uint32_t DELAY = 500;


constexpr int MAZE_SIZE = 10;
constexpr int MAZE[MAZE_SIZE][MAZE_SIZE] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 5, 1, 1, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 2, 1, 1, 1},
    {1, 1, 0, 0, 4, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 2, 1, 1},
    {1, 1, 1, 1, 3, 0, 1, 1, 1, 1}
};

constexpr const char* gameWinText = "You win!";
constexpr const char* gameOverText = "You lose... ";

constexpr float scale = 4.0f;


enum  Tile
{
    FLOOR,
    WALL,
    ENTRANCE,
    MOUSE,
    CAT,
    CHEESE,
};