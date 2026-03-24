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

std::vector<Object*> objects;        
Object* cat, * mouse;
std::vector<std::pair<int, int>> catPath;
double rotateAngle = 0;

bool inWalls, gameOver, gameWon = false;
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
bool collisionDectector(SDL_FRect character, SDL_FRect obj) {
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

void drawMessage(const char* message) {
    int w = 0, h = 0;
    float x, y;

    //TODO: fix centering on scaled message?
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    //draw
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
    std::vector<SDL_Texture*> textures;
    SDL_Texture** texturePtr = new SDL_Texture*;

    textureLoader("floor.png", texturePtr);
    textures.push_back(*texturePtr);
    textureLoader("brick walls.png", texturePtr);
    textures.push_back(*texturePtr);
    textureLoader("enter walls.png", texturePtr);
    textures.push_back(*texturePtr);
    textureLoader("mouse guy.png", texturePtr);
    textures.push_back(*texturePtr);
    textureLoader("cat creature.png", texturePtr);
    textures.push_back(*texturePtr);
    textureLoader("cheese reward.png", texturePtr);
    textures.push_back(*texturePtr);

    delete texturePtr;
    int mazeVal = 0;


    for (int i = 0; i < MAZE_SIZE; i++) {
        currX = 0;
        for (int j = 0; j < MAZE_SIZE; j++) {
            mazeVal = MAZE[i][j];
            SDL_Texture* objTexture = textures.at(mazeVal);
            SDL_FRect dstRect = { currX, currY, OBJECT_SIZE, OBJECT_SIZE };

            Object* object = new Object(objTexture, dstRect, mazeVal);
            if (mazeVal > 2) {
                objects.insert(objects.begin(), object);
                if (mazeVal == 3) {
                    mouse = object;
                }
                if (mazeVal == 4) {
                    cat = object;
                }
                

                SDL_Texture* floorTexture = textures.at(0);
                Object* object = new Object(floorTexture, dstRect, 0);
                objects.push_back(object);

                SDL_RenderTexture(renderer, floorTexture, NULL, &dstRect);

            }
            else {
                //TODO: floor tiles are currently added as objects also, part of bg problem

                objects.push_back(object);
            }


            SDL_RenderTexture(renderer, objTexture, NULL, &dstRect);

            currX += OBJECT_SIZE;
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
        for (int i = 1; i < objects.size(); i++) {
            if (collisionDectector(mouse->colliderRect, objects.at(i)->colliderRect)) {
                switch (objects.at(i)->value) {
                case 0:
                    if (inWalls) {
                        mouse->colliderRect.x = oldX;
                        mouse->colliderRect.y = oldY;
                    }
                    break;
                case 1:
                    if (!inWalls) {
                        mouse->colliderRect.x = oldX;
                        mouse->colliderRect.y = oldY;
                    }
                    break;
                case 2:

                    if (inputDir == 'l') {
                        mouse->colliderRect.x -= OBJECT_SIZE * 2;
                        mouse->colliderRect.y = objects.at(i)->colliderRect.y;
                        inWalls = !inWalls;
                        SDL_Delay(DELAY);
                    }
                    if (inputDir == 'r') {
                        mouse->colliderRect.x += OBJECT_SIZE * 2;
                        mouse->colliderRect.y = objects.at(i)->colliderRect.y;
                        inWalls = !inWalls;
                        SDL_Delay(DELAY);
                    }

                    break;
                case 4:
                    gameOver = true;
                    break;
                case 5:
                    gameWon = true;
                    break;

                }
            }
        }

        //calculate path to mouse for cat
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

        for (int i = 2; i < objects.size(); i++) {
            SDL_RenderTexture(renderer, objects.at(i)->texture, NULL, &objects.at(i)->colliderRect);
        }

        //cat collisions
        for (int i = 2; i < objects.size(); i++) {
            if (collisionDectector(cat->colliderRect, objects.at(i)->colliderRect)) {
                switch (objects.at(i)->value) {
                case 1:
                    cat->colliderRect.x = oldCatX;
                    cat->colliderRect.y = oldCatY;
                    break;
                case 2:
                    cat->colliderRect.x = oldCatX;
                    cat->colliderRect.y = oldCatY;
                    break;
                case 4:
                    printf("game over :(");
                    break;
                }
            }
        }

        if (catPath.empty()) {
            gameOver = true;
        }



        SDL_RenderTextureRotated(renderer, mouse->texture, NULL, &mouse->colliderRect, rotateAngle, NULL, SDL_FLIP_NONE);


        SDL_RenderTexture(renderer, cat->texture, NULL, &cat->colliderRect);
        SDL_RenderTexture(renderer, objects.at(2)->texture, NULL, &objects.at(2)->colliderRect);


        SDL_RenderPresent(renderer);

    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    for (Object* object : objects) {
        delete object;
    }
}
