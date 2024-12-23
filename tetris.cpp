#include "tetris.h"

#include <iostream>
#include <conio.h>
#include <string>


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

const int boardWidth = 10;
const int boardHeight = 24;

bool rotateKeyPressed = false;
bool rightKeyPressed = false;
bool leftKeyPressed = false;
bool downKeyPressed = false;
int clearedRowsCount = 0;
int score = 0;
int level = 1;
int linesCleared = 0;
int dropInterval = DROP_INTERVAL;
vector<vector<int>> board(boardHeight, vector<int>(boardWidth, 0));


RGB getBlockColor(int color) {
    switch (color) {
    case 1: return { 0, 255, 255 };    // Cyan
    case 2: return { 0, 0, 255 };      // Blue
    case 3: return { 255, 165, 0 };    // Orange
    case 4: return { 255, 255, 0 };    // Yellow
    case 5: return { 0, 255, 0 };      // Green
    case 6: return { 128, 0, 128 };    // Purple
    case 7: return { 255, 0, 0 };      // Red
    default: return { 255, 255, 255 }; // White
    }
}


vector<Tetromino> tetrominos = {
    {{{ 1, 1, 1, 1 }}, 3, 0, 1},              // I
    {{{ 1, 1, 1 }, { 1, 0, 0 }}, 3, 0, 2},    // J
    {{{ 1, 1, 1 }, { 0, 0, 1 }}, 3, 0, 3},    // L
    {{{ 1, 1 }, { 1, 1 }}, 4, 0, 4},          // O
    {{{ 0, 1, 1 }, { 1, 1, 0 }}, 3, 0, 5},    // S
    {{{ 1, 1, 1 }, { 0, 1, 0 }}, 3, 0, 6},    // T
    {{{ 1, 1, 0 }, { 0, 1, 1 }}, 3, 0, 7}     // Z
};


// Initialize SDL_ttf
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    if (TTF_Init() == -1) {
        cout << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    return true;
}


bool isGameOver(const Tetromino& tetromino, const vector<vector<int>>& board) {
    return checkCollision(tetromino, board);
}

// Cleanup SDL_ttf
void close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;

    TTF_Quit();
    SDL_Quit();
}

/**
 * @brief Renders text on the screen.
 * 
 * @param message The text message to render.
 * @param x The x-coordinate of the text.
 * @param y The y-coordinate of the text.
 * @param color The color of the text.
 * @param font The TTF font.
 * @param renderer The SDL renderer.
 */
void renderText(const std::string &message, int x, int y, SDL_Color color, TTF_Font *font, SDL_Renderer *renderer) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, message.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

/**
 * @brief Displays the game board, the current Tetromino, and the next Tetromino.
 * 
 * @param board The game board.
 * @param tetromino The current Tetromino.
 * @param nextTetromino The next Tetromino.
 * @param font The TTF font.
 * 
 */
void display(const vector<vector<int>>& board, const Tetromino& tetromino, const Tetromino& nextTetromino, TTF_Font *font) {
    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Calculate the board rendering area
    int boardRenderWidth = boardWidth * BLOCK_SIZE;
    int boardRenderHeight = (boardHeight - HIDDEN_ROWS) * BLOCK_SIZE;
    int horizontalOffset = (SCREEN_WIDTH - boardRenderWidth) / 2;
    int verticalOffset = -HIDDEN_ROWS * BLOCK_SIZE;

    // Display the board
    for (int y = 0; y < board.size(); y++) {
        for (int x = 0; x < board[y].size(); x++) {
            if (board[y][x] != 0) {
                RGB color = getBlockColor(board[y][x]);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black outline
                SDL_Rect blockOutline = { horizontalOffset + x * BLOCK_SIZE, verticalOffset + y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
                SDL_RenderFillRect(renderer, &blockOutline);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255); // Block color
                SDL_Rect block = { horizontalOffset + x * BLOCK_SIZE + 1, verticalOffset + y * BLOCK_SIZE + 1, BLOCK_SIZE - 2, BLOCK_SIZE - 2 };
                SDL_RenderFillRect(renderer, &block);
            }
        }
    }

    // Display the tetromino
    for (int i = 0; i < tetromino.shape.size(); i++) {
        for (int j = 0; j < tetromino.shape[i].size(); j++) {
            if (tetromino.shape[i][j] != 0) {
                RGB color = getBlockColor(tetromino.color);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black outline
                SDL_Rect blockOutline = { horizontalOffset + (tetromino.x + j) * BLOCK_SIZE, verticalOffset + (tetromino.y + i) * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
                SDL_RenderFillRect(renderer, &blockOutline);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255); // Block color
                SDL_Rect block = { horizontalOffset + (tetromino.x + j) * BLOCK_SIZE + 1, verticalOffset + (tetromino.y + i) * BLOCK_SIZE + 1, BLOCK_SIZE - 2, BLOCK_SIZE - 2 };
                SDL_RenderFillRect(renderer, &block);
            }
        }
    }

    // Draw vertical lines on either side of the board
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    SDL_Rect leftLine = { horizontalOffset - 3, 0, 3, SCREEN_HEIGHT };
    SDL_Rect rightLine = { horizontalOffset + boardRenderWidth, 0, 3, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &leftLine);
    SDL_RenderFillRect(renderer, &rightLine);

    // Display the next tetromino in the box
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for the box outline
    SDL_Rect nextPieceBox = { NEXT_PIECE_BOX_X, NEXT_PIECE_BOX_Y, NEXT_PIECE_BOX_SIZE, NEXT_PIECE_BOX_SIZE };
    SDL_RenderDrawRect(renderer, &nextPieceBox);

    int nextPieceOffsetX = NEXT_PIECE_BOX_X + (NEXT_PIECE_BOX_SIZE - nextTetromino.shape[0].size() * BLOCK_SIZE) / 2;
    int nextPieceOffsetY = NEXT_PIECE_BOX_Y + (NEXT_PIECE_BOX_SIZE - nextTetromino.shape.size() * BLOCK_SIZE) / 2;

    for (int i = 0; i < nextTetromino.shape.size(); i++) {
        for (int j = 0; j < nextTetromino.shape[i].size(); j++) {
            if (nextTetromino.shape[i][j] != 0) {
                RGB color = getBlockColor(nextTetromino.color);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black outline
                SDL_Rect blockOutline = { nextPieceOffsetX + j * BLOCK_SIZE, nextPieceOffsetY + i * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE };
                SDL_RenderFillRect(renderer, &blockOutline);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255); // Block color
                SDL_Rect block = { nextPieceOffsetX + j * BLOCK_SIZE + 1, nextPieceOffsetY + i * BLOCK_SIZE + 1, BLOCK_SIZE - 2, BLOCK_SIZE - 2 };
                SDL_RenderFillRect(renderer, &block);
            }
        }
    }

    // Render the score, lines, level, and next piece in the remaining 1/3 of the screen
    // Example: Render a placeholder text for score
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect scoreRect = { horizontalOffset + boardRenderWidth + 10, 10, SCREEN_WIDTH / 3 - 20, 50 };
    SDL_RenderFillRect(renderer, &scoreRect);

    SDL_Color textColor = { 0, 0, 0, 255 }; // Black color for text
    renderText("Rows: " + std::to_string(score), horizontalOffset + boardRenderWidth + 20, 20, textColor, font, renderer);
    renderText("Level: " + std::to_string(level), horizontalOffset + boardRenderWidth + 20, 35, textColor, font, renderer);
    SDL_RenderPresent(renderer);
}

/**
 * @brief Returns a list of full rows on the board.
 * 
 * @param board The game board.
 * @return vector<int> A list of full rows.
 */
vector<int> getFullRows(const vector<vector<int>>& board) {
    vector<int> fullRows;
    for (int y = 0; y < board.size(); y++) {
        bool isFull = true;
        for (int x = 0; x < board[y].size(); x++) {
            if (board[y][x] == 0) {
                isFull = false;
                break;
            }
        }
        if (isFull) {
            fullRows.push_back(y);
        }
    }
    return fullRows;
}


/**
 * @brief Clears the full rows from the board.
 * 
 * @param board The game board.
 * @param fullRows The list of full rows to clear.
 */
void clearFullRows(vector<vector<int>>& board, const vector<int>& fullRows) {
    cout << "Clearing rows: ";
    for (int row : fullRows) {
        cout << row << " ";
    }
    cout << endl;

    int lines = fullRows.size();
    clearedRowsCount += lines; // Increment the counter by the number of cleared rows
    linesCleared += lines;
    score += lines * lines * 100; // Increase the score based on the number of cleared lines

    if (clearedRowsCount >= level * 10) {
        level++;
        linesCleared -= 10;
        dropInterval -= max(100, dropInterval - 50); // Decrease the drop interval by 50 milliseconds
    }

    for (int row : fullRows) {
        // Shift rows above down
        for (int y = row; y > 0; y--) {
            board[y] = board[y - 1];
        }
        // Clear the top row
        board[0] = vector<int>(board[0].size(), 0);
    }

    // Print the board state after clearing rows
    cout << "Board state after clearing rows:" << endl;
    for (const auto& row : board) {
        for (int cell : row) {
            cout << cell << " ";
        }
        cout << endl;
    }
}


/**
 * @brief Flashes the full rows before clearing them.
 * 
 * @param board The game board.
 * @param fullRows The list of full rows to flash.
 * @param renderer The SDL renderer.
 * @param font The TTF font.
 * @param nextTetromino The next Tetromino to display.
 * 
 */
void flashRows(vector<vector<int>>& board, const vector<int>& fullRows, SDL_Renderer* renderer, TTF_Font* font, const Tetromino& nextTetromino) {
    for (int i = 0; i < FLASH_COUNT; i++) {
        for (int row : fullRows) {
            for (int x = 0; x < board[row].size(); x++) {
                board[row][x] = (board[row][x] == 0) ? 1 : 0; // Toggle visibility
            }
        }
        display(board, Tetromino(), nextTetromino, font); // Update the display
        SDL_RenderPresent(renderer);
        SDL_Delay(FLASH_INTERVAL);
    }
}

/**
 * @brief Checks for collision between the Tetromino and the game board.
 * 
 * @param tetromino The Tetromino to check for collision.
 * @param board The game board.
 * @return true if there is a collision, false otherwise.
 */
bool checkCollision(const Tetromino& tetromino, const vector<vector<int>>& board) {
    for (int i = 0; i < tetromino.shape.size(); ++i) {
        for (int j = 0; j < tetromino.shape[i].size(); ++j) {
            if (tetromino.shape[i][j] != 0) {
                int boardX = tetromino.x + j;
                int boardY = tetromino.y + i;

                // Check if the Tetromino is out of bounds or collides with existing blocks
                if (boardX < 0 || boardX >= board[0].size() || boardY < 0 || boardY >= board.size() || board[boardY][boardX] != 0) {
                    cout << "Collision detected at (" << boardX << ", " << boardY << ")" << endl;
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief Handles collision detection and places the Tetromino on the board.
 * 
 * @param tetromino The Tetromino to handle.
 * @param board The game board.
 */
bool handleCollision(Tetromino& tetromino, vector<vector<int>>& board) {
    if (checkCollision(tetromino, board)) {
        cout << "Collision detected at position (" << tetromino.x << ", " << tetromino.y << ")" << endl;
        tetromino.y -= 1;
        // Place the tetromino on the board and spawn a new one
        for (int i = 0; i < tetromino.shape.size(); i++) {
            for (int j = 0; j < tetromino.shape[i].size(); j++) {
                if (tetromino.shape[i][j] != 0) {
                    board[tetromino.y + i][tetromino.x + j] = tetromino.color;
                }
            }
        }
        cout << "Tetromino placed on the board." << endl;
        return true;
    }
    return false;
}


/**
 * @brief Rotates the Tetromino if possible.
 * 
 * @param tetromino The Tetromino to rotate.
 * @param board The game board.
 */
void rotateTetromino(Tetromino& tetromino, const vector<vector<int>>& board) {
    int rows = tetromino.shape.size();
    int cols = tetromino.shape[0].size();
    vector<vector<int>> rotatedShape(cols, vector<int>(rows, 0));

    // Transpose the matrix
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            rotatedShape[j][i] = tetromino.shape[i][j];
        }
    }

    // Reverse the order of the columns
    for (int i = 0; i < cols; i++) {
        reverse(rotatedShape[i].begin(), rotatedShape[i].end());
    }

    // Save the original shape
    vector<vector<int>> originalShape = tetromino.shape;
    tetromino.shape = rotatedShape;

    // Check for collisions or out of bounds
    if (checkCollision(tetromino, board)) {
        // Revert to the original shape if rotation is not possible
        tetromino.shape = originalShape;
    }
}


/**
 * @brief The main function initializes the game, handles the game loop, and cleans up resources.
 * 
 * @param argc The number of command-line arguments.
 * @param args The array of command-line arguments.
 * @return int Returns 0 on successful execution, -1 on failure.
 * 
 * The main function performs the following tasks:
 * - Seeds the random number generator.
 * - Initializes the game.
 * - Loads the font.
 * - Enters the main game loop where it handles events, updates game state, and renders the game.
 * - Handles game over state and displays "Game Over" message.
 * - Cleans up resources before exiting.
 */
int main(int argc, char* args[]) {
    srand(static_cast<unsigned>(time(0)));

    if (!init()) {
        cout << "Failed to initialize!" << endl;
        return -1;
    }

    TTF_Font *font = TTF_OpenFont("fonts/ARIAL.TTF", 16);
    if (font == NULL) {
        cout << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
        return -1;
    }

    bool quit = false;
    bool gameOver = false;
    SDL_Event e;

    Tetromino currentTetromino = tetrominos[rand() % tetrominos.size()];
    Tetromino nextTetromino = tetrominos[rand() % tetrominos.size()];

    Uint32 lastDropTime = SDL_GetTicks();

    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (!gameOver && e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_w:
                        if (!rotateKeyPressed) {
                            rotateTetromino(currentTetromino, board);
                            rotateKeyPressed = true;
                        }
                        break;
                    case SDLK_d:
                        if (!rightKeyPressed) {
                            currentTetromino.x += 1;
                            if (checkCollision(currentTetromino, board)) {
                                currentTetromino.x -= 1;
                            }
                            rightKeyPressed = true;
                        }
                        break;
                    case SDLK_a:
                        if (!leftKeyPressed) {
                            currentTetromino.x -= 1;
                            if (checkCollision(currentTetromino, board)) {
                                currentTetromino.x += 1;
                            }
                            leftKeyPressed = true;
                        }
                        break;
                    case SDLK_s:
                        if (!downKeyPressed) {
                            currentTetromino.y += 1;
                            if (handleCollision(currentTetromino, board)) {
                                vector<int> fullRows = getFullRows(board);
                                if (!fullRows.empty()) {
                                    cout << "before flashRows" << endl;
                                    for (const auto& row : board) {
                                        for (int cell : row) {
                                            cout << cell << " ";
                                        }
                                        cout << endl;
                                    }
                                    flashRows(board, fullRows, renderer, font, nextTetromino);
                                    clearFullRows(board, fullRows);
                                }
                                currentTetromino = nextTetromino;
                                nextTetromino = tetrominos[rand() % tetrominos.size()];
                                if (isGameOver(currentTetromino, board)) {
                                    gameOver = true;
                                }
                            }
                            downKeyPressed = true;
                        }
                        break;
                }
            } else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_w:
                        rotateKeyPressed = false;
                        break;
                    case SDLK_d:
                        rightKeyPressed = false;
                        break;
                    case SDLK_a:
                        leftKeyPressed = false;
                        break;
                    case SDLK_s:
                        downKeyPressed = false;
                        break;
                }
            }
        }

        if (!gameOver) {
            // Drop the tetromino at the defined interval
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - lastDropTime >= DROP_INTERVAL) {
                currentTetromino.y += 1;
                if (handleCollision(currentTetromino, board)) {
                    vector<int> fullRows = getFullRows(board);
                    if (!fullRows.empty()) {
                        for (const auto& row : board) {
                            for (int cell : row) {
                                cout << cell << " ";
                            }
                            cout << endl;
                        }
                        flashRows(board, fullRows, renderer, font, nextTetromino);
                        clearFullRows(board, fullRows);
                    }
                    currentTetromino = nextTetromino;
                    nextTetromino = tetrominos[rand() % tetrominos.size()];
                    if (isGameOver(currentTetromino, board)) {
                        gameOver = true;
                    }
                }
                lastDropTime = currentTime;
            }
        }

        display(board, currentTetromino, nextTetromino, font);

        if (gameOver) {
            SDL_Color textColor = { 255, 0, 0, 255 }; // Red color
            renderText("Game Over", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, textColor, font, renderer);
        }

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    TTF_CloseFont(font);
    close();
    return 0;
}