#include <stdlib.h>
#include <stdio.h>
#include "tile.h"
#include <stdbool.h>


/**
 * Tile display status:
 *  - BLANK: User has not flagged or exposed this tile.
 *  - FLAGGED: User has flagged this tile as a possible bomb.
 *  - EXPOSED: User has exposed this tile
 */

/**
 * Tile data structure, holding its position, whether it has a bomb,
 * and its display status.
 */


/**
 * Allocates and initializes a new Tile with the given data.
 *
 * @param x the x position (from the left) of the new Tile.
 * @param y the y position (from the top) of the new Tile.
 * @param bomb BOMB_HERE (9) if this tile is a bomb, else 0-8 indicating nearby
 *  bombs
 * @return a Tile pointer to a created Tile struct
 */
Tile *newTile(short x, short y, short bomb) {
  Tile *tile = malloc(sizeof(Tile));
  tile -> x = x;
  tile -> y = y;
  tile -> bomb = bomb;
  tile -> flagged = false;
  tile -> exposed = false;
  return tile;
}

/**
 * Returns a string representation of this Tile's data.
 * If it is blank or flagged, hides bomb data.
 *
 * @param tile the tile to print data for
 * @return a String of the form ( x, y, display)
 */
char *tile_toString(Tile *tile) {
  char status[8];
  if (tile -> exposed) {
    if (tile -> bomb == BOMB_HERE) {
      sprintf(status, "BOMB");
    } else {
      sprintf(status, "%d", tile -> bomb);
    }
  } else {
    if (tile -> flagged) {
      sprintf(status, "flagged");
    } else {
      sprintf(status, "blank");
    }
  }
  char *string = malloc(sizeof(char) * 16);
  sprintf(string, "(%2d,%2d,%s)", tile->x, tile->y, status);
  return string;
}

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
char tile_toChar(Tile *tile) {
  // If the tile is already exposed
  if ( tile -> exposed ) {
    // If flagged and NOT a bomb, show as X
    if ( tile -> flagged && tile -> bomb != BOMB_HERE ) {
      return 'X';
    }
    // If bomb, show as '*'
    if ( tile -> bomb == BOMB_HERE ) {
      return '*';
    }
    // If empty, show as ' '
    else if ( tile -> bomb == 0 ) {
      return ' ';
    }
    // Otherwise, show the number itself
    else {
      return '0' + tile -> bomb;
    }
  }
  // Otherwise, look at whether or not it's flagged
  else {
    // If flagged, show as '!'
    if ( tile -> flagged ) {
      return '!';
    }
    // If not touched, show as '.'
    return '.';
  }
}

