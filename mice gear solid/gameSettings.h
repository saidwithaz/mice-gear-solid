#pragma once

#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include <vector>

/* Defines global settings and constants for the game.
*  These don't change once program is compiled.
*/
constexpr int OBJECT_SIZE = 64;
constexpr int WINDOW_SIZE = 640;

/* Tinker speed here (cat has lower numbers for higher speed).
*  TODO: rename cat speed??
*/
constexpr int CAT_CALC_SPEED = 500;
constexpr int CAT_SPEED = 25;
constexpr float MOUSE_SPEED = 0.175f;

constexpr uint32_t DELAY = 500;

constexpr int TEXTURES = 6;
constexpr int MAZE_SIZE = 10;
constexpr int OBJECTS = MAZE_SIZE * MAZE_SIZE;

constexpr int MAZE[MAZE_SIZE][MAZE_SIZE] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 5, 1, 1, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 2, 1, 1, 1},
    {1, 1, 0, 0, 4, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 1, 1, 1, 1, 0, 0, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 1},
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

struct Object {
    SDL_Texture* texture;
    SDL_FRect colliderRect;
    int value;

    Object(SDL_Texture* t, SDL_FRect c, int v) {
        texture = t;
        colliderRect = c;
        value = v;
    }

    Object() {
    }
};

/* Malleable game variables are held here.
*  They are populated when the game is initialized and can be updated in the main loop.
*/
struct AppState {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;

    Object objects[OBJECTS];
    Object cat, mouse;
    std::vector<std::pair<int, int>> catPath;
    double rotateAngle = 0;

    bool inWalls = false, gameOver = false, gameWon = false;
    char inputDir = 'u';
};

