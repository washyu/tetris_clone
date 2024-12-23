#ifndef TETRIS_H
#define TETRIS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

using namespace std;

// Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BLOCK_SIZE = 24;
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;
const int HIDDEN_ROWS = 4;
const int DROP_INTERVAL = 500;
const int FLASH_COUNT = 5;
const int FLASH_INTERVAL = 150; // milliseconds
const int NEXT_PIECE_BOX_SIZE = 120;
const int NEXT_PIECE_BOX_X = 10;
const int NEXT_PIECE_BOX_Y = 10;

// Type definitions
struct RGB {
    int r, g, b;
};

struct Tetromino {
    vector<vector<int>> shape;
    int x, y;
    int color;
};

// Function declarations
bool init();
void close();
void renderText(const std::string &message, int x, int y, SDL_Color color, TTF_Font *font, SDL_Renderer *renderer);
void display(const vector<vector<int>>& board, const Tetromino& currentTetromino, const Tetromino& nextTetromino, TTF_Font *font);
vector<int> getFullRows(const vector<vector<int>>& board);
void clearFullRows(vector<vector<int>>& board, const vector<int>& fullRows);
void flashRows(vector<vector<int>>& board, const vector<int>& fullRows, SDL_Renderer* renderer, TTF_Font* font, const Tetromino& nextTetromino);
bool handleCollision(Tetromino& tetromino, vector<vector<int>>& board);
void rotateTetromino(Tetromino& tetromino, const vector<vector<int>>& board);
bool checkCollision(const Tetromino& tetromino, const vector<vector<int>>& board);
bool isGameOver(const Tetromino& tetromino, const vector<vector<int>>& board);
RGB getBlockColor(int color);


#endif // TETRIS_H