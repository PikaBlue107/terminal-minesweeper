// Stored in Tile -> bomb indicating that a bomb is at this tile
#define BOMB_HERE 9


/**
 * Tile data structure, holding its position, whether it has a bomb,
 * and its display status.
 */
typedef struct Tile {
  short x;
  short y;
  short bomb; // BOMB_HERE (9) for bomb, otherwise 0-8 for nearby
  _Bool exposed;
  _Bool flagged;
} Tile;


/**
 * Allocates and initializes a new Tile with the given data.
 *
 * @param x the x position (from the left) of the new Tile.
 * @param y the y position (from the top) of the new Tile.
 * @param bomb BOMB_HERE (9) if this tile is a bomb, else 0-8 indicating nearby
 *  bombs
 * @return a Tile pointer to a created Tile struct
 */
struct Tile *newTile(short x, short y, short bomb);


/**
 * Prints out a Tile's data. If it is blank or flagged, hides bomb data.
 *
 * @param tile the tile to print data for
 * @return a String of the form ( x, y, display)
 */
char *tile_toString(struct Tile *tile);

/**
 * Returns a character representation of this Tile's data.
 * If it's blank, returns '.'
 * It it's flagged, returns '!'
 * If it's exposed, returns:
 *  '*' for bomb
 *  ' ' for empty
 *  1-9 for nearby
 * 
 * @returns the character representing this space on the minesweeper board.
 */
char tile_toChar(struct Tile *tile);

