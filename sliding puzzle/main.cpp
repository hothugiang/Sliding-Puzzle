#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <time.h>
#include <SDL_mixer.h>
#include "tile.h"
#include "stopwatch.h"
#include "button.h"
using namespace std;

typedef vector <vector<Tile>> tileArray;

bool inBounds(const int row, const int col, const int maxRow, const int maxCol) {
    // Return true if valid index
    return !(row < 0 || row > maxRow || col < 0 || col > maxCol);
}

bool isEmptyTileInNeighbours(tileArray& tiles, const int row, const int col, Tile* emptyTile) {
    // Check up, right, down and left tile
    static const int deltas[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

    // For each neighbour
    for (int i = 0; i < 4; ++i) {
        // If tile is valid
        if (inBounds(row + deltas[i][0], col + deltas[i][1], tiles.size() - 1, tiles[0].size() - 1)) {
            if (emptyTile == &tiles[row + deltas[i][0]][col + deltas[i][1]]) {
                return true;
            }
        }
    }

    // Empty tile not found
    return false;
}

unsigned int playMenu(SDL_Renderer* renderer, bool* exit, const unsigned int SCREEN_WIDTH, const unsigned int SCREEN_HEIGHT) {
    // This is a menu where the user can choose the difficulty of puzzle

    // User interface properties
    const unsigned int NUMBER_OF_ROW_ELEMENTS = 1;
    const unsigned int NUMBER_OF_COL_ELEMENTS = 4;
    const unsigned int NUMBER_OF_ROW_BORDERS = NUMBER_OF_ROW_ELEMENTS + 1;
    const unsigned int NUMBER_OF_COL_BORDERS = NUMBER_OF_COL_ELEMENTS + 1;
    const unsigned int BORDER_THICKNESS = 20;

    // Menu button dimensions
    const unsigned int BUTTON_WIDTH = SCREEN_WIDTH - 2 * BORDER_THICKNESS;
    const unsigned int BUTTON_HEIGHT = (SCREEN_HEIGHT - NUMBER_OF_COL_BORDERS * BORDER_THICKNESS) / NUMBER_OF_COL_ELEMENTS;

    // Define colours
    const SDL_Color FONT_COLOUR = {0, 0, 0, 255}; // Black
    const SDL_Color BUTTON_COLOUR = {242, 201, 224, 255}; // Pastel pink
    const SDL_Color BUTTON_DOWN_COLOUR = {137, 194, 217, 255}; // Blue

    // Load font for text
    const int fontSize = BUTTON_HEIGHT - 40;
    TTF_Font* font = TTF_OpenFont("src/OlgaChubby.ttf", fontSize);
    if (font == nullptr) cout << "Failed to load font! Error: " << TTF_GetError() << endl;

    // Define button texts
    const char* buttonTexts[4] = {"3x3", "4x4", "5x5", "Exit"};

    // Place stopwatch at the top
    int startX = BORDER_THICKNESS;
    int startY = 0;

    // Create vector of buttons
    vector <Button> buttons;
    for (int row = 0; row < NUMBER_OF_COL_ELEMENTS; ++row) {
        startY += BORDER_THICKNESS;
        SDL_Rect rect = {startX, startY, (int)BUTTON_WIDTH, (int)BUTTON_HEIGHT};
        Button button(rect, BUTTON_COLOUR, font, FONT_COLOUR);
        button.loadTexture(renderer, buttonTexts[row]);
        buttons.push_back(button);
        startY += BUTTON_HEIGHT;
    }

    // Frame rate parameters
    const unsigned int FPS = 60;
    const float milliSecondsPerFrame = 1000 / FPS;
    float lastTimeRendered = SDL_GetTicks();
    float deltaTimeRendered;

    // Game loop variables
    bool stop = false;
    SDL_Event event;
    unsigned int difficulty = 0;

    // Game loop
    while (!stop) {
        // Handle events
        while (SDL_PollEvent(&event) != 0) {
            // Handle quiting
            if (event.type == SDL_QUIT) {
                *exit = true;
                stop = true;
            }
            // Handle mouse down
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                Mix_Chunk* slideSound = Mix_LoadWAV("Sound/Mouse_Click_Effect.wav");
                Mix_PlayChannel(-1, slideSound, 0);
                int x, y;
                SDL_GetMouseState(&x, &y);
                for (int i = 0; i < NUMBER_OF_COL_ELEMENTS; ++i) {
                    if (buttons[i].isMouseInside(x, y)) {
                        buttons[i].changeColourTo(BUTTON_DOWN_COLOUR);
                        // Difficulty is i + 3 since we start of with 3x3 (and we need to use for loop to get index i)
                        difficulty = i + 3;
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONUP){
                for (auto& button : buttons) {
                    button.changeColourTo(BUTTON_COLOUR);
                }
                // If a difficulty has been chosen then play puzzle at that difficulty
                if (difficulty != 0) {
                    stop = true;
                }
            }
        }

        // Couple and control frame rate with game loop (render loop)
        deltaTimeRendered = SDL_GetTicks() - lastTimeRendered;
        if (deltaTimeRendered > milliSecondsPerFrame) {
            lastTimeRendered = SDL_GetTicks();
            // Clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            for (const auto& button : buttons) {
                button.render(renderer);
            }

            // Update screen from backbuffer and clear backbuffer
            SDL_RenderPresent(renderer);

        } else {
            // Else cause a delay for the remaining amount
            SDL_Delay(milliSecondsPerFrame - deltaTimeRendered);
        }

    }

    // Free textures
    for (auto& button : buttons) {
        button.free();
    }

    if (difficulty == 6) {
        cout << "End game";
        SDL_Quit();
        return 0;
    }

    // Free font
    TTF_CloseFont(font);
    font = nullptr;

    return difficulty;
}

void playPuzzle(SDL_Renderer* renderer, bool* exit, const unsigned int DIFFICULTY, const unsigned int SCREEN_WIDTH, const unsigned int SCREEN_HEIGHT) {
    //BGM Music
    srand(time(NULL));
    int num = rand() % 3 + 1;
    string BGM_file_name = "Sound/BGM/" + to_string(num) + ".mp3";
    Mix_Music* BGM = Mix_LoadMUS(BGM_file_name.c_str());
    Mix_PlayMusic(BGM, -1);

    // User interface properties
    const unsigned int NUMBER_OF_ROW_ELEMENTS = DIFFICULTY;
    const unsigned int NUMBER_OF_COL_ELEMENTS = DIFFICULTY + 2; // including stopwatch and "new" button
    const unsigned int NUMBER_OF_ROW_BORDERS = NUMBER_OF_ROW_ELEMENTS + 1;
    const unsigned int NUMBER_OF_COL_BORDERS = NUMBER_OF_COL_ELEMENTS + 1;
    const unsigned int BORDER_THICKNESS = 6;
    const unsigned int TILE_WIDTH = (SCREEN_WIDTH - NUMBER_OF_ROW_BORDERS * BORDER_THICKNESS) / NUMBER_OF_ROW_ELEMENTS;
    const unsigned int TILE_HEIGHT = (SCREEN_HEIGHT - NUMBER_OF_COL_BORDERS * BORDER_THICKNESS) / NUMBER_OF_COL_ELEMENTS;

    SDL_Texture* winText = IMG_LoadTexture(renderer, "win.png");
    int randNum = rand() % 10;
    //cout << randNum << endl;
    string filename = "pic/pic" + to_string(randNum) + ".jpg";
    SDL_Texture* picture = IMG_LoadTexture(renderer, filename.c_str());
    int picWidth, picHeight;
    SDL_QueryTexture(picture, NULL, NULL, &picWidth, &picHeight);
    SDL_Texture* win3 = IMG_LoadTexture(renderer, "pic/win_3x3.png");
    SDL_Texture* win4 = IMG_LoadTexture(renderer, "pic/win_4x4.png");
    SDL_Texture* win5 = IMG_LoadTexture(renderer, "pic/win_5x5.png");

    // Menu button dimensions
    const unsigned int BUTTON_WIDTH = SCREEN_WIDTH - 2 * BORDER_THICKNESS;
    const unsigned int BUTTON_HEIGHT = TILE_HEIGHT;

    // Stopwatch dimensions
    const unsigned int STOPWATCH_WIDTH = SCREEN_WIDTH - 2 * BORDER_THICKNESS - BUTTON_WIDTH/DIFFICULTY;
    const unsigned int STOPWATCH_HEIGHT = TILE_HEIGHT;

    // Define colours
    const SDL_Color TILE_COLOUR = {242, 201, 224, 255}; // Pastel pink
    const SDL_Color TILE_COMPLETION_COLOUR = {137, 194, 217, 255}; // Blue
    const SDL_Color FONT_COLOUR = {0, 0, 0, 255}; // Black
    const SDL_Color STOPWATCH_COLOUR = {160, 102, 198, 255}; // Purple
    const SDL_Color BUTTON_COLOUR = {242, 141, 178, 255}; // Pink
    const SDL_Color BUTTON_DOWN_COLOUR = {137, 194, 217, 255}; // Blue
    const SDL_Color BUTTON_WIN_COLOR = {242, 141, 178, 255}; //Pink

    // Load font for text
    const int fontSize = TILE_HEIGHT - 60;
    TTF_Font* font = TTF_OpenFont("src/OlgaChubby.ttf", fontSize);
    if (font == nullptr) {
        cout << "Failed to load font! Error: " << TTF_GetError() << endl;
    }

    // Place stopwatch at the top
    int startX = BORDER_THICKNESS;
    int startY = BORDER_THICKNESS;
    SDL_Rect rect = {startX, startY, (int)STOPWATCH_WIDTH, (int)STOPWATCH_HEIGHT};
    Stopwatch stopwatch(rect, STOPWATCH_COLOUR, font, FONT_COLOUR);

    // Create vector of tiles
    tileArray tiles;
    startY += TILE_HEIGHT;
    for (int row = 0; row < DIFFICULTY; ++row) {
        vector<Tile> tileRow;
        startY += BORDER_THICKNESS;
        startX = 0;
        for (int col = 0; col < DIFFICULTY; ++col) {

            startX += BORDER_THICKNESS;
            rect = {startX, startY, (int)TILE_WIDTH, (int)TILE_HEIGHT};
            // Make final tile "empty"
            SDL_Color colour;
            if (!(row == DIFFICULTY - 1 && col == DIFFICULTY - 1))
                colour = TILE_COLOUR;

            // Calculate number to be assigned to tile
            int number = row * DIFFICULTY + col + 1;

            Tile tile(rect, colour, font, FONT_COLOUR, number);
            // Convert number to c style string
            const char* numberStr = to_string(number).c_str();
            tile.loadTexture(renderer, numberStr);
            tileRow.push_back(tile);

            startX += TILE_WIDTH;
        }
        startY += TILE_HEIGHT;
        tiles.push_back(tileRow);
    }

    // Add "new" button at bottom of screen
    startX = BORDER_THICKNESS;
    startY += BORDER_THICKNESS;
    rect = {startX, startY, (int)BUTTON_WIDTH, (int)BUTTON_HEIGHT};
    Button menuButton(rect, BUTTON_COLOUR, font, FONT_COLOUR);
    menuButton.loadTexture(renderer, "Menu");

    // Frame rate parameters
    const unsigned int FPS = 90;
    const float milliSecondsPerFrame = 1000 / FPS;
    float lastTimeRendered = SDL_GetTicks();
    float deltaTimeRendered;

    // Animation speed parameters
    const unsigned int pixelsPerSecond = 500;
    const float milliSecondsPerPixel = 1000 / pixelsPerSecond;
    float lastTimeMoved;
    float deltaTimeMoved;

    // Track the "moving" and "empty" tile
    Tile* movingTile = nullptr;
    bool doneMoving = true;
    Tile* emptyTile = &tiles[DIFFICULTY - 1][DIFFICULTY - 1];
    bool selected = false;
    int tempXPosition;
    int tempYPosition;

    // Shuffle tiles by swapping
    const unsigned int TOTAL_SWAPS = 1000;
    // Set empty tile to last tile
    int emptyTileRow = DIFFICULTY - 1;
    int emptyTileCol = DIFFICULTY - 1;
    // Keep track of row and cols for neighbours
    vector <vector<int>> neighbours;
    // Set seed for randomly picking neigbour tile
    srand(time(NULL));
    // North, East, South, and West deltas
    const int deltas[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    // Do shuffling
    for (int swap = 0; swap < TOTAL_SWAPS; ++swap) {
        for (int i = 0; i < 4; ++i) {
            const int deltaRow = deltas[i][0];
            const int deltaCol = deltas[i][1];
            if (inBounds(emptyTileRow + deltaRow, emptyTileCol + deltaCol, tiles.size() - 1, tiles[0].size() - 1)) {
                vector<int> neighbour = {emptyTileRow + deltaRow, emptyTileCol + deltaCol};
                neighbours.push_back(neighbour);
            }
        }
        // Choose random tile from neighbours
        const int randomIndex = rand() % neighbours.size();
        const int row = neighbours[randomIndex][0];
        const int col = neighbours[randomIndex][1];
        // Swap positions
        tempXPosition = emptyTile->getXPosition();
        tempYPosition = emptyTile->getYPosition();
        emptyTile->setPositionTo(tiles[row][col].getXPosition(), tiles[row][col].getYPosition());
        tiles[row][col].setPositionTo(tempXPosition, tempYPosition);
        // Swap elements in vector
        iter_swap(&tiles[row][col], emptyTile);
        emptyTile = &tiles[row][col];
        emptyTileRow = row;
        emptyTileCol = col;
        neighbours.clear();
    }

    // Game loop variables
    bool stop = false;
    SDL_Event event;
    bool checkSolved = false;
    bool solved = false;
    bool menuButtonPressed = false;

    // Begin stopwatch
    stopwatch.start();

    // Game loop
    bool winsound = false;
    while (!stop) {
        // Handle events
        while (SDL_PollEvent(&event) != 0) {
            // Handle quiting
            if (event.type == SDL_QUIT) {
                stop = true;
                *exit = true;
            }

            // Handle mouse down (only when no tiles are moving)
            if (doneMoving) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    if (!solved) {

                        for (int row = 0; row < tiles.size(); ++row) {
                            for (int col = 0; col < tiles[row].size(); ++col) {
                                if (tiles[row][col].isMouseInside(x, y)) {
                                    if (isEmptyTileInNeighbours(tiles, row, col, emptyTile)) {
                                        Mix_Chunk* slideSound = Mix_LoadWAV("Sound/SlidingSound.wav");
                                        Mix_PlayChannel(-1, slideSound, 0);
                                        movingTile = &tiles[row][col];
                                        selected = true;
                                        doneMoving = false;
                                        lastTimeMoved = SDL_GetTicks();
                                    }
                                }
                            }
                        }
                    }
                    if (menuButton.isMouseInside(x, y)) {
                        Mix_Chunk* slideSound = Mix_LoadWAV("Sound/Mouse_Click_Effect.wav");
                        Mix_PlayChannel(-1, slideSound, 0);
                        menuButton.changeColourTo(BUTTON_DOWN_COLOUR);
                        menuButtonPressed = true;
                    }
                } else if (event.type == SDL_MOUSEBUTTONUP) {
                    menuButton.changeColourTo(BUTTON_COLOUR);
                    if (menuButtonPressed) {
                        stop = true;
                    }
                }
            }
        }

        // If a tile is selected (moving) save position for new empty tile position
        if (selected && movingTile != nullptr) {
            tempXPosition = movingTile->getXPosition();
            tempYPosition = movingTile->getYPosition();
            selected = false;
        }

        // Move tile into empty tile and control animation speed (independant of frame rate)
        if (movingTile != nullptr) {
            deltaTimeMoved = SDL_GetTicks() - lastTimeMoved;
            if (deltaTimeMoved > milliSecondsPerPixel) {
                // Move n number of pixels based on delta time;
                int pixelsToMove = deltaTimeMoved / milliSecondsPerPixel;
                for (int i = 0; i < pixelsToMove; ++i) {
                    doneMoving = movingTile->moveTo(emptyTile->getXPosition(), emptyTile->getYPosition());
                    if (doneMoving) {
                        emptyTile->setPositionTo(tempXPosition, tempYPosition);
                        iter_swap(movingTile, emptyTile);
                        emptyTile = movingTile;
                        movingTile = nullptr;
                        checkSolved = true;
                        break;
                    }
                }
                lastTimeMoved = SDL_GetTicks();
            }
        }

        // Check solved
        if (checkSolved) {
            solved = true;
            for (int row = 0; row < tiles.size(); ++row) {
                for (int col = 0; col < tiles[row].size(); ++col){
                    const int number = row * DIFFICULTY + col + 1;
                    if (tiles[row][col].getNumber() != number) {
                        solved = false;
                    }
                }
            }
            checkSolved = false;
        }

        // If solved, turn tiles to green and stop stopwatch

        if (solved) {
            // Change colour of each tile to green to indicate completion
                // Don't change colour of empty tile
            if (!winsound){
                Mix_Chunk* handclapSound = Mix_LoadWAV("Sound/HandClap.wav");
                Mix_PlayChannel(-1, handclapSound, 0);
                winsound = true;
            }

            stopwatch.changeColourTo(BUTTON_WIN_COLOR);
            for (int row = 0; row < tiles.size(); ++row) {
                for (int col = 0; col < tiles[row].size(); ++col){
                    if (emptyTile != &tiles[row][col]) {
                        tiles[row][col].changeColourTo(TILE_COMPLETION_COLOUR);
                    }
                }
            }
        }
        else {
            stopwatch.calculateTime(renderer);
        }

        // Couple and control frame rate with game loop (render loop)
        deltaTimeRendered = SDL_GetTicks() - lastTimeRendered;
        if (deltaTimeRendered > milliSecondsPerFrame) {
            lastTimeRendered = SDL_GetTicks();
            // Clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Render stopwatch
            stopwatch.render(renderer);

            // Render sample picture
            SDL_Rect dest;

            dest.x = STOPWATCH_WIDTH + 2 * BORDER_THICKNESS;
            dest.y = BORDER_THICKNESS;
            dest.w = TILE_WIDTH;
            dest.h = TILE_HEIGHT;

            if (randNum == 0 && DIFFICULTY == 3) SDL_RenderCopy(renderer, win3, NULL, &dest);
            else if (randNum == 0 && DIFFICULTY == 4) SDL_RenderCopy(renderer, win4, NULL, &dest);
            else if (randNum == 0 && DIFFICULTY == 5) SDL_RenderCopy(renderer, win5, NULL, &dest);
            else SDL_RenderCopy(renderer, picture, NULL, &dest);

            // Render all tiles
                // Don't render empty tile
            for (int row = 0; row < tiles.size(); ++row) {
                for (int col = 0; col < tiles[row].size(); ++col){
                    if (emptyTile != &tiles[row][col]) {
                        tiles[row][col].render(renderer);
                    }
                }
            }

            for (int i = 0; i < DIFFICULTY; i++) {
                for (int j = 0; j < DIFFICULTY; j++) {
                    if (&tiles[i][j] != emptyTile) {
                        SDL_Rect src, dest;
                        src.x = picWidth / DIFFICULTY * ((tiles[i][j].getNumber() - 1) % DIFFICULTY);
                        src.y = picHeight / DIFFICULTY * ((tiles[i][j].getNumber() - 1) / DIFFICULTY);
                        src.w = picWidth / DIFFICULTY;
                        src.h = picHeight / DIFFICULTY;

                        dest.x = tiles[i][j].getXPosition();
                        dest.y = tiles[i][j].getYPosition();
                        dest.w = TILE_WIDTH;
                        dest.h = TILE_HEIGHT;

                    SDL_RenderCopy(renderer, picture, &src, &dest);
                    }
                }
            }

            // Render "new" button
            menuButton.render(renderer);
            //SDL_RenderPresent(renderer);

            if (solved){
                SDL_RenderCopy(renderer, winText, NULL, NULL);
            }


            // Update screen from backbuffer and clear backbuffer
            SDL_RenderPresent(renderer);

        } else {
            // Else cause a delay for the remaining amount
            SDL_Delay(milliSecondsPerFrame - deltaTimeRendered);
        }
    }

    if (solved) {
        cout << "Solved!" << endl;
    }

    // Free textures
    for (int row = 0; row < tiles.size(); ++row)
        for (int col = 0; col < tiles[row].size(); ++col)
            tiles[row][col].free();
    stopwatch.free();

    SDL_DestroyTexture(picture);

    // Free font
    TTF_CloseFont(font);
    font = nullptr;

    //tat nhac
    Mix_HaltMusic();
    Mix_HaltChannel(-1);
}

int main( int argc, char* args[] ) {
    // Screen dimensions
    const unsigned int SCREEN_WIDTH = 600;
    const unsigned int SCREEN_HEIGHT = 800;

    // Initialise SDL video subsystem
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "SDL could not initialise! Error: " << SDL_GetError() << endl;
        return -1;
    }

    // Intialise SDL_ttf
    if (TTF_Init() == -1) {
        cout << "SDL_ttf could not initialise! Error: " << TTF_GetError() << endl;
        return -1;
    }

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    // Create window
    SDL_Window* window = SDL_CreateWindow("Sliding Puzzle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        cout << "SDL could not create window! Error: " << SDL_GetError() << endl;
        return -1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr) {
        cout << "SDL could not create renderer! Error: " << SDL_GetError() << endl;
        return -1;
    }

    // Open game
    SDL_Texture *tex = IMG_LoadTexture(renderer, "open.png");
    SDL_RenderCopy(renderer, tex, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);
    SDL_RenderClear(renderer);


    // Exit variable
    bool exit = false;

    // Number of tiles = difficulty * difficulty
    unsigned int difficulty;

    // Break when exit variable is set to true
    while (1) {
        // Play menu and return difficulty chosen
        difficulty = playMenu(renderer, &exit, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (exit) {
            break;
        }

        // Play puzzle
        playPuzzle(renderer, &exit, difficulty, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (exit) {
            break;
        }
    }

    // Destroy renderer and window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = nullptr;
    window = nullptr;

    // Quit SDL and TTF
    SDL_Quit();
    TTF_Quit();

    // Exit program
    cout << "Exiting program..." << endl;
    return 0;
}
