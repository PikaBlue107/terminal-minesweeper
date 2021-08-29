#include "tile.h"

// Errors are failures in user input
#define ERR_OUT_OF_BOUNDS 1101
// Invalid are game-state problems
#define INVALID_FLAGGED 1111
#define INVALID_EXPOSED 1112
// Lose conditions set by the game
#define LOSE_MINE 1121

/**
 * Minesweeper board data, containing board size, board contents, and mine
 * count.
 */
typedef struct Board {
  // Array size
  short width;
  short height;
  // Board
  struct Tile ***board;
  // Number of mines on board
  short mineCount;
  // Cursor position on the board
  short cur_x;
  short cur_y;
  // Count of exposed tiles
  int exposed;
} Board;


/**
 * Constructor for a Board. Initializes Tiles, places mines, and returns the
 * created Board.
 *
 * @param width the horizontal count of tiles across the board
 * @param height the veritcal count of tiles across the board
 * @param mineCount the number of mines that will be placed on the baord
 * @return the newly created Board
 */
Board *newBoard(short width, short height, short mineCount);

/**
 * Prints the provided board to stdout. Includes a border around the edge.
 *
 * @param board the game board to print
 */
void board_print(Board *board);

/**
 * Exposes the board by setting all Tiles' status to STATUS_EXPOSED.
 *
 * @param board the board to flip
 */
void board_expose_all(Board *board);

/**
 * Exposes one blank tile on the board, to start off.
 * If a blank tile cannot be found after a certain number of retries,
 * exposes the first non-bomb tile it finds.
 *
 * @param board the board to expose a tile on
 */
void board_expose_safe(Board *board);

/**
 * Exposes one tile on the board.
 * If the position is out of bounds, returns ERR_OUT_OF_BOUNDS.
 * If it's flagged, returns with code INVALID_FLAGGED.
 * If it's blank, exposes all nearby tiles, too, and returns with EXIT_SUCCESS.
 * If it's a number, exposes just the one, and returns with EXIT_SUCCESS.
 * If it's a bomb, exposes the bomb and returns with code LOSE_MINE.
 *
 * @param board the board to expose a tile on
 * @param x the x position of the tile to expose
 * @param y the y position of the tile to expose
 * @return 0 if successful, else LOSE_MINE, INVALID_FLAGGED, or
 * ERR_OUT_OF_BOUNDS.
 */
short board_expose_pick(Board *board, short x, short y);

/**
 * Flags one tile on the board.
 * If the position is out of bounds, returns ERR_OUT_OF_BOUNDS.
 * If it's already exposed, returns INVALID_EXPOSED.
 * If it's a blank tile, flags it, and returns EXIT_SUCCESS.
 * If it's a flagged tile, unflags it, and returns EXIT_SUCCESS.
 *
 * @param baord the board to flag a tile on
 * @param x the x position of the tile to flag
 * @param y the y position of the tile to flag
 * @return 0 if successful, else ERR_OUT_OF_BOUNDS or INVALID_EXPOSED.
 */
short board_flag(Board *board, short x, short y);
