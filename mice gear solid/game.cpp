#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

#include <SDL3/SDL_main.h>
#include <pathfinder.h>


/* Loads a given texture (sprite).
* Inputs(s):	const char* textureName  = The file name for the sprite.
*				SDL_Texture** texturePtr = A pointer that points to where the texture is saved.
*
* Returns:		Success if loaded correctly.
*/
SDL_AppResult textureLoader(SDL_Renderer* renderer, const char* textureName, SDL_Texture** texturePtr) {

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
static void drawMessage(SDL_Renderer* renderer, const char* message) {
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
    //store "global" game state data in this struct
    static AppState state;
    *appstate = &state;

    if (!SDL_CreateWindowAndRenderer("Get the cheese!", WINDOW_SIZE, WINDOW_SIZE, NULL, &state.window, &state.renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    int currX = 0, currY = 0;
    SDL_Texture* textures[TEXTURES];
    SDL_Texture* texture = NULL;

    textureLoader(state.renderer, "floor.png", &texture);
    textures[FLOOR] = texture;
    textureLoader(state.renderer, "brick walls.png", &texture);
    textures[WALL] = texture;
    textureLoader(state.renderer, "enter walls.png", &texture);
    textures[ENTRANCE] = texture;
    textureLoader(state.renderer, "mouse guy.png", &texture);
    textures[MOUSE] = texture;
    textureLoader(state.renderer, "cat creature.png", &texture);
    textures[CAT] = texture;
    textureLoader(state.renderer, "cheese reward.png", &texture);
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

                state.mouse = Object(textures[mazeVal], dstRect, mazeVal);

                object = Object(textures[FLOOR], dstRect, 0);

                SDL_RenderTexture(state.renderer, textures[FLOOR], NULL, &dstRect);
            }
            if (mazeVal == Tile::CAT) {
                state.cat = Object(textures[mazeVal], dstRect, mazeVal);

                SDL_Texture* floorTexture = textures[FLOOR];
                object = Object(floorTexture, dstRect, 0);

                SDL_RenderTexture(state.renderer, floorTexture, NULL, &dstRect);
            }

            state.objects[objectIndex] = object;

            SDL_RenderTexture(state.renderer, textures[mazeVal], NULL, &dstRect);

            currX += OBJECT_SIZE;
            objectIndex++;
        }
        currY += OBJECT_SIZE;
    }

    state.catPath = findPathToMouse({ state.cat.colliderRect.x, state.cat.colliderRect.y },
        { state.mouse.colliderRect.x, state.mouse.colliderRect.y }, MAZE);


    SDL_RenderPresent(state.renderer);

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
    AppState* state = (AppState*)appstate;

    Uint64 timePassed = SDL_GetTicks();
    const bool* keys = SDL_GetKeyboardState(NULL);

    int oldX = state->mouse.colliderRect.x;
    int oldY = state->mouse.colliderRect.y;
    int oldCatX = state->cat.colliderRect.x;
    int oldCatY = state->cat.colliderRect.y;

    if (state->gameOver) {
        drawMessage(state->renderer, gameOverText);
    }
    else if (state->gameWon) {
        drawMessage(state->renderer, gameWinText);
    }
    else {
        if (keys[SDL_SCANCODE_LEFT]) {
            state->inputDir = 'l';
            state->rotateAngle = 270;
            state->mouse.colliderRect.x -= MOUSE_SPEED;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            state->inputDir = 'r';
            state->rotateAngle = 90;
            state->mouse.colliderRect.x += MOUSE_SPEED;
        }
        else if (keys[SDL_SCANCODE_UP]) {
            state->inputDir = 'u';
            state->rotateAngle = 0;
            state->mouse.colliderRect.y -= MOUSE_SPEED;
        }
        else if (keys[SDL_SCANCODE_DOWN]) {
            state->inputDir = 'd';
            state->rotateAngle = 180;
            state->mouse.colliderRect.y += MOUSE_SPEED;
        }

        //check out of bounds
        if ((state->mouse.colliderRect.x < 0) || (state->mouse.colliderRect.y < 0) ||
            (state->mouse.colliderRect.x + OBJECT_SIZE > WINDOW_SIZE) || (state->mouse.colliderRect.y + OBJECT_SIZE > WINDOW_SIZE)) {
            state->mouse.colliderRect.x = oldX;
            state->mouse.colliderRect.y = oldY;
        }

        //check collisions for mouse
        for (int i = 0; i < MAZE_SIZE*MAZE_SIZE; i++) {
            if (collisionDetector(state->mouse.colliderRect, state->objects[i].colliderRect)) {
                switch (state->objects[i].value) {
                case FLOOR:
                    if (state->inWalls) {
                        state->mouse.colliderRect.x = oldX;
                        state->mouse.colliderRect.y = oldY;
                    }
                    break;
                case WALL:
                    if (!state->inWalls) {
                        state->mouse.colliderRect.x = oldX;
                        state->mouse.colliderRect.y = oldY;
                    }
                    break;
                case ENTRANCE:
                    if (state->inputDir == 'l') {
                        state->mouse.colliderRect.x -= OBJECT_SIZE * 2;
                        state->mouse.colliderRect.y = state->objects[i].colliderRect.y;
                        state->inWalls = !state->inWalls;
                        SDL_Delay(DELAY);
                    }
                    if (state->inputDir == 'r') {
                        state->mouse.colliderRect.x += OBJECT_SIZE * 2;
                        state->mouse.colliderRect.y = state->objects[i].colliderRect.y;
                        state->inWalls = !state->inWalls;
                        SDL_Delay(DELAY);
                    }

                    break;
                case CHEESE:
                    state->gameWon = true;
                    break;

                }
            }
        }

        //calculate path to mouse for cat
        //TODO: change to deltaTime
        if (timePassed % CAT_CALC_SPEED == 0) {
            state->catPath = findPathToMouse({ state->cat.colliderRect.x, state->cat.colliderRect.y },
                { state->mouse.colliderRect.x, state->mouse.colliderRect.y }, MAZE);
        }


        //move cat
        if (timePassed % CAT_SPEED == 0 && ! state->inWalls && !state->catPath.empty()) {
            state->cat.colliderRect.x = state->catPath.front().first;
            state->cat.colliderRect.y = state->catPath.front().second;

            state->catPath.erase(state->catPath.begin());
        }


        SDL_RenderClear(state->renderer);

        for (int i = 0; i < MAZE_SIZE*MAZE_SIZE; i++) {
            SDL_RenderTexture(state->renderer, state->objects[i].texture, NULL, &state->objects[i].colliderRect);
        }

        //cat collisions
        for (int i = 0; i < MAZE_SIZE*MAZE_SIZE; i++) {
            if (collisionDetector(state->cat.colliderRect, state->objects[i].colliderRect)) {
                switch (state->objects[i].value) {
                    case WALL:
                    case ENTRANCE:
                        state->cat.colliderRect.x = oldCatX;
                        state->cat.colliderRect.y = oldCatY;
                        break;
                }
            }
        }

        if (state->catPath.empty()) {
            state->catPath = findPathToMouse({ state->cat.colliderRect.x, state->cat.colliderRect.y }, 
                { state->mouse.colliderRect.x, state->mouse.colliderRect.y }, MAZE);
        }

        if (collisionDetector(state->cat.colliderRect, state->mouse.colliderRect)) {
            state->gameOver = true;
        }


        SDL_RenderTextureRotated(state->renderer, state->mouse.texture, NULL, 
            &state->mouse.colliderRect, state->rotateAngle, NULL, SDL_FLIP_NONE);
        SDL_RenderTexture(state->renderer, state->cat.texture, NULL, &state->cat.colliderRect);
        SDL_RenderPresent(state->renderer);

    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{

}
