#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */


#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <gameSettings.h>
#include <pathfinder.h>


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


static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;

Object objects[MAZE_SIZE*MAZE_SIZE];        
Object* cat, * mouse;
std::vector<std::pair<int, int>> catPath;
double rotateAngle = 0;

bool inWalls = false, gameOver = false, gameWon = false;
char inputDir = 'u';


/* Loads a given texture (sprite).
* Inputs(s):	const char* textureName  = The file name for the sprite.
*				SDL_Texture** texturePtr = A pointer that points to where the texture is saved.
*
* Returns:		Success if loaded correctly.
*/
SDL_AppResult textureLoader(const char* textureName, SDL_Texture** texturePtr) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, textureName);
    if (!texture) {
        SDL_Log("Couldn't load the texture: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    *texturePtr = texture;

    return SDL_APP_CONTINUE;
}


/* Detect collisions between the mouse and other objects.
* Inputs(s):	SDL_FRect character  = Contains the coordinate data for the mouse's collision box.
*				SDL_FRect objs = Contains the coordinate data for the object's collision box.
*
* Returns:		Boolean if mouse has collided with object.
*/
static bool collisionDetector(SDL_FRect character, SDL_FRect obj) {
    int leftChar = character.x;
    int rightChar = character.x + character.w;
    int topChar = character.y;
    int bottomChar = character.y + character.h;

    int leftObj = obj.x;
    int rightObj = obj.x + obj.w;
    int topObj = obj.y;
    int bottomObj = obj.y + obj.h;

    if (bottomChar <= topObj)
    {
        return false;
    }

    if (topChar >= bottomObj)
    {
        return false;
    }

    if (rightChar <= leftObj)
    {
        return false;
    }

    if (leftChar >= rightObj)
    {
        return false;
    }

    return true;
}


/* Draws a simple text message to fill the screen when the game is won or lost collisions.
* Inputs(s):	const char* message = The message to be displayed.
*
* Returns:		None.
*/
static void drawMessage(const char* message) {
    int w = 0, h = 0;
    float x, y;

    //TODO: fix centering on scaled message?
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, x, y, message);
    SDL_RenderPresent(renderer);
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    // create window
    if (!SDL_CreateWindowAndRenderer("Get the cheese!", WINDOW_SIZE, WINDOW_SIZE, NULL, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    int currX = 0, currY = 0;
    SDL_Texture* textures[6];
    SDL_Texture* texture = NULL;

    textureLoader("floor.png", &texture);
    textures[FLOOR] = texture;
    textureLoader("brick walls.png", &texture);
    textures[WALL] = texture;
    textureLoader("enter walls.png", &texture);
    textures[ENTRANCE] = texture;
    textureLoader("mouse guy.png", &texture);
    textures[MOUSE] = texture;
    textureLoader("cat creature.png", &texture);
    textures[CAT] = texture;
    textureLoader("cheese reward.png", &texture);
    textures[CHEESE] = texture;

    int mazeVal = 0;
    int objectIndex = 0;


    for (int i = 0; i < MAZE_SIZE; i++) {
        currX = 0;
        for (int j = 0; j < MAZE_SIZE; j++) {
            mazeVal = MAZE[i][j];
            SDL_FRect dstRect = { currX, currY, OBJECT_SIZE, OBJECT_SIZE };

            Object object = Object(textures[mazeVal], dstRect, mazeVal);
            if (mazeVal == Tile::MOUSE) {

                mouse = new Object(textures[mazeVal], dstRect, mazeVal);

                object = Object(textures[FLOOR], dstRect, 0);

                SDL_RenderTexture(renderer, textures[FLOOR], NULL, &dstRect);
            }
            if (mazeVal == Tile::CAT) {
                cat = new Object(textures[mazeVal], dstRect, mazeVal);

                SDL_Texture* floorTexture = textures[FLOOR];
                object = Object(floorTexture, dstRect, 0);

                SDL_RenderTexture(renderer, floorTexture, NULL, &dstRect);
            }

            objects[objectIndex] = object;

            SDL_RenderTexture(renderer, textures[mazeVal], NULL, &dstRect);

            currX += OBJECT_SIZE;
            objectIndex++;
        }
        currY += OBJECT_SIZE;
    }

    catPath = findPathToMouse({ cat->colliderRect.x, cat->colliderRect.y }, { mouse->colliderRect.x, mouse->colliderRect.y }, MAZE);


    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}


/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }

    return SDL_APP_CONTINUE;
}


/* This function runs once per frame. */
SDL_AppResult SDL_AppIterate(void* appstate) {
    Uint64 timePassed = SDL_GetTicks();
    const bool* keys = SDL_GetKeyboardState(NULL);

    int oldX = mouse->colliderRect.x;
    int oldY = mouse->colliderRect.y;
    int oldCatX = cat->colliderRect.x;
    int oldCatY = cat->colliderRect.y;

    if (gameOver) {
        drawMessage(gameOverText);
    }
    else if (gameWon) {
        drawMessage(gameWinText);
    }
    else {
        if (keys[SDL_SCANCODE_LEFT]) {
            inputDir = 'l';
            rotateAngle = 270;
            mouse->colliderRect.x -= MOUSE_SPEED;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            inputDir = 'r';
            rotateAngle = 90;
            mouse->colliderRect.x += MOUSE_SPEED;
        }
        else if (keys[SDL_SCANCODE_UP]) {
            inputDir = 'u';
            rotateAngle = 0;
            mouse->colliderRect.y -= MOUSE_SPEED;
        }
        else if (keys[SDL_SCANCODE_DOWN]) {
            inputDir = 'd';
            rotateAngle = 180;
            mouse->colliderRect.y += MOUSE_SPEED;
        }

        //check out of bounds
        if ((mouse->colliderRect.x < 0) || (mouse->colliderRect.y < 0) ||
            (mouse->colliderRect.x + OBJECT_SIZE > WINDOW_SIZE) || (mouse->colliderRect.y + OBJECT_SIZE > WINDOW_SIZE)) {
            mouse->colliderRect.x = oldX;
            mouse->colliderRect.y = oldY;
        }

        //check collisions for mouse
        for (int i = 0; i < MAZE_SIZE*MAZE_SIZE; i++) {
            if (collisionDetector(mouse->colliderRect, objects[i].colliderRect)) {
                switch (objects[i].value) {
                case FLOOR:
                    if (inWalls) {
                        mouse->colliderRect.x = oldX;
                        mouse->colliderRect.y = oldY;
                    }
                    break;
                case WALL:
                    if (!inWalls) {
                        mouse->colliderRect.x = oldX;
                        mouse->colliderRect.y = oldY;
                    }
                    break;
                case ENTRANCE:
                    if (inputDir == 'l') {
                        mouse->colliderRect.x -= OBJECT_SIZE * 2;
                        mouse->colliderRect.y = objects[i].colliderRect.y;
                        inWalls = !inWalls;
                        SDL_Delay(DELAY);
                    }
                    if (inputDir == 'r') {
                        mouse->colliderRect.x += OBJECT_SIZE * 2;
                        mouse->colliderRect.y = objects[i].colliderRect.y;
                        inWalls = !inWalls;
                        SDL_Delay(DELAY);
                    }

                    break;
                case CHEESE:
                    gameWon = true;
                    break;

                }
            }
        }

        //calculate path to mouse for cat
        //TODO: change to deltaTime
        if (timePassed % CAT_CALC_SPEED == 0) {
            catPath = findPathToMouse({ cat->colliderRect.x, cat->colliderRect.y }, { mouse->colliderRect.x, mouse->colliderRect.y }, MAZE);
        }


        //move cat
        if (timePassed % CAT_SPEED == 0 && !inWalls) {
            cat->colliderRect.x = catPath.front().first;
            cat->colliderRect.y = catPath.front().second;

            catPath.erase(catPath.begin());
        }


        SDL_RenderClear(renderer);

        for (int i = 0; i < MAZE_SIZE*MAZE_SIZE; i++) {
            SDL_RenderTexture(renderer, objects[i].texture, NULL, &objects[i].colliderRect);
        }

        //cat collisions
        for (int i = 0; i < MAZE_SIZE*MAZE_SIZE; i++) {
            if (collisionDetector(cat->colliderRect, objects[i].colliderRect)) {
                switch (objects[i].value) {
                    case WALL:
                    case ENTRANCE:
                        cat->colliderRect.x = oldCatX;
                        cat->colliderRect.y = oldCatY;
                        break;
                }
            }
        }

        if (catPath.empty()) {
            catPath = findPathToMouse({ cat->colliderRect.x, cat->colliderRect.y }, { mouse->colliderRect.x, mouse->colliderRect.y }, MAZE);
        }

        if (collisionDetector(cat->colliderRect, mouse->colliderRect)) {
            gameOver = true;
        }


        SDL_RenderTextureRotated(renderer, mouse->texture, NULL, &mouse->colliderRect, rotateAngle, NULL, SDL_FLIP_NONE);
        SDL_RenderTexture(renderer, cat->texture, NULL, &cat->colliderRect);
        SDL_RenderPresent(renderer);

    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    delete mouse;
    delete cat;
}
