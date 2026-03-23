#pragma once

constexpr int OBJECT_SIZE = 64;
constexpr int WINDOW_SIZE = 640;
constexpr int CAT_CALC_SPEED = 500;
constexpr int CAT_SPEED = 25;
constexpr float MOUSE_SPEED = 0.25f;

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

enum  Tile
{
    FLOOR,
    WALL,
    ENTRANCE,
    MOUSE,
    CAT,
    CHEESE,
};