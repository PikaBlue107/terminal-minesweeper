#include "board.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

/**
 * Minesweeper board data, containing board size, board contents, and mine
 * count.
 */

///** Resets all color / format settings. */
//#define COLOR_NC "\033[0m"
///** Fades the foreground color when printed. */
//#define COLOR_FADE "\033[2m"
///** Underlines the foreground when printed. */
//#define COLOR_UNDERLINE "\033[4m"
///** Inverts the background and foreground colors when printed. */
//#define COLOR_INV "\033[7m"

/** Format code for clearing all formatting. */
#define FMT_NONE 0
/** Format code for dimming. */
#define FMT_DIM 2
/** Format code for underlining. */
#define FMT_UND 4
/** Format for inverting foreground and background colors. */
#define FMT_INV 7
/** Color code for red */
#define COLOR_RED 31 // To be used for 5's as "Maroon"
/** Color code for green */
#define COLOR_GREEN 33
/** Color code for blue */
#define COLOR_BLUE 34
/** Color code for magenta */
#define COLOR_MAGENTA 35
/** Color code for cyan */
#define COLOR_CYAN 36
/** Color code for light gray */
#define COLOR_L_GRAY 37
/** Color code for background red */
#define COLOR_BG_RED 41
/** Color code for dark gray */
#define COLOR_D_GRAY 90
/** Color code for light red */
#define COLOR_L_RED 91
/** Color code for light green */
#define COLOR_L_GREEN 92
/** Color code for light yellow */
#define COLOR_L_YELLOW 93
/** Color code for whilte */
#define COLOR_WHITE 97
/** Color code for background as light red */
#define COLOR_BG_L_RED 101

/**
 * Checks the submitted bounds.
 * If the position is out of bounds, returns ERR_OUT_OF_BOUNDS.
 * Otherwise, returns EXIT_SUCCESS.
 *
 * @param board the board with width and height
 * @param x the x position to check
 * @param y the y position to check
 * @return 0 if OK, else ERR_OUT_OF_BOUNDS.
 */
static int check_bounds(Board *board, short x, short y) {
  if ( x < 0 || x >= board -> width || y < 0 || y >= board -> height ) {
    return ERR_OUT_OF_BOUNDS;
  }
  return EXIT_SUCCESS;
}

/**
 * Finds the list of nearby tiles on the board within a distance of 1 (including
 * diagonals).
 * Won't return out of bounds tiles, or the same tile as passed in.
 * Tiles are placed in the provided array. Any array positions not used up to
 * index 7 are set to NULL.
 *
 * @param board the board of tiles to search through
 * @param x the x position to get nearby tiles around
 * @param y the y position to get nearby tiles around
 * @param nearby a provided array to place found tiles into
 */
static void list_nearby(Board *board, short x, short y, Tile *nearby[8]) {
  // Loop over nearby tiles
  short tile_idx = 0;
  for ( int offset_y = -1; offset_y <= 1; offset_y++ ) {
    for ( int offset_x = -1; offset_x <= 1; offset_x++ ) {
      // If we're on the bomb tile itself, skip it
      if ( offset_x == 0 && offset_y == 0 ) {
        continue;
      }

      // Save the nearby absolute position
      size_t near_y = y + offset_y;
      size_t near_x = x + offset_x;

      // If this tile is out of bounds, skip it
      if ( near_y < 0 || near_y >= board -> height ||
          near_x < 0 || near_x >= board -> width) {
        continue;
      }

      // Add this tile into the list of nearby tiles
      nearby[tile_idx++] = board -> board[near_y][near_x];
    }
  }
  // Reset any array indices we didn't use
  for ( ; tile_idx < 8; tile_idx++ ) {
    nearby[tile_idx] = NULL;
  }
  // All done!
}

/**
 * Counts the number of nearby flags to a tile.
 *
 * @param board the board to check on
 * @param x the x position of the tile to check
 * @param y the y position of the tile to check
 * @return the number of nearby flagged tiles, 0-8.
 */
static short nearby_flags(Board *board, short x, short y) {
  // Get list of nearby tiles
  Tile *nearby[9] = { NULL };
  list_nearby(board, x, y, nearby);
  // Process list of nearby tiles
  short nearby_flags = 0;
  for ( Tile **tile_ptr = nearby; *tile_ptr; tile_ptr++ ) {
    Tile *tile_near = *tile_ptr;
    // If it's flagged, count it up
    if ( tile_near -> flagged ) {
      nearby_flags++;
    }
  }
  return nearby_flags;
}


/**
 * Counts the number of nearby blanks to a tile.
 *
 * @param board the board to check on
 * @param x the x position of the tile to check
 * @param y the y position of the tile to check
 * @return the number of nearby blank tiles, 0-8.
 */
static short nearby_blanks(Board *board, short x, short y) {
  // Get list of nearby tiles
  Tile *nearby[9] = { NULL };
  list_nearby(board, x, y, nearby);
  // Process list of nearby tiles
  short nearby_blanks = 0;
  for ( Tile **tile_ptr = nearby; *tile_ptr; tile_ptr++ ) {
    Tile *tile_near = *tile_ptr;
    // If it's blank, and not flagged, count it up
    if ( !tile_near -> exposed && !tile_near -> flagged ) {
      nearby_blanks++;
    }
  }
  return nearby_blanks;
}

/**
 * Constructor for a Board. Initializes Tiles, places mines, and returns the
 * created Board.
 *
 * @param width the horizontal count of tiles across the board
 * @param height the veritcal count of tiles across the board
 * @param mineCount the number of mines that will be placed on the baord
 * @return the newly created Board
 */
Board *newBoard(short width, short height, short mineCount) {
  
  // Allocate the board struct itself
  printf("Allocating board container...\n");
  Board *board = malloc(sizeof(Board));
  // Copy over the data
  printf("Copying over board creation data...\n");
  printf("width=%d\n", width);
  printf("height=%d\n", height);
  printf("mineCount=%d\n", mineCount);
  board -> width = width;
  board -> height = height;
  board -> mineCount = mineCount;

  // Initialize the main board array

  // Create variables to keep track of the size of data needed
  int len_header = sizeof(Tile **) * height;
  int len_arr = sizeof(Tile *) * height * width;
  int len_total = len_header + len_arr;

  // Allocate full board array
  printf("Allocating the actual board, %lu bytes...\n", sizeof(Tile) * width * height);
  board -> board = (Tile ***) malloc( len_total );

  // Get pointer to first element of board array
  Tile **main_table = (Tile **)(board -> board + height);

  // for loop to point rows pointer to appropriate location in 2D array
  for(size_t y = 0; y < height; y++)
    board -> board[y] = (main_table + width * y);


  printf("Start of board is %p\n", board -> board);

  // Loop through the board, creating each tile
  printf("Initializing board tiles...\n");
  for ( size_t y = 0; y < height; y++ ) {
    // Loop over this row
    for ( size_t x = 0; x < width; x++ ) {
      // Initialize this Tile
      printf("Initializing tile...\n");
      Tile *tile = newTile(x, y, 0);
      printf("Placing tile at (%2zu,%2zu)...\n", x, y);
      board -> board[y][x] = tile;
    }
    printf("Row %zu complete.\n", y);
  }
  printf("Board initialization complete.\n");

  // Assign the mines
  printf("Assigning mines...\n");
  // Use current time as seed for random generator
  srand(time(0));
  // Keep track of how many we've assigned
  short minesAssigned = 0;
  // While there's still more mines to assign
  while ( minesAssigned < mineCount ) {

    // Pick a random spot to mine
    short rand_x = rand() % width;
    short rand_y = rand() % height;
    printf("Picking spot (%2d,%2d) for potential mine (%d of %d)...\n",
        rand_x, rand_y, minesAssigned + 1, mineCount);
    // Get the Tile at that spot
    Tile *rand_tile = board -> board[rand_y][rand_x];
    printf("Checking Tile at (%2d,%2d)...\n", rand_x, rand_y);
    
    // Check that spot to make sure it's not already a bomb
    if ( rand_tile -> bomb == BOMB_HERE ) {
      // This spot already has a mine. Re-generate.
      printf("Found spot (%2d,%2d) that already has a mine. Skipping...\n",
          rand_x, rand_y);
      continue;
    }

    // Assign a bomb to this spot
    rand_tile -> bomb = BOMB_HERE;
    printf("Bomb assigned at spot (%2d,%2d).\n", rand_x, rand_y);
    // Record this placed mine
    minesAssigned++;
    printf("Mine %d of %d assigned.\n", minesAssigned, mineCount);

    // Increment nearby tiles' bomb count
    // Find nearby tiles
    Tile *nearby[9] = { NULL };
    list_nearby(board, rand_x, rand_y, nearby);
    // Process nearby tiles - increment their bomb count
    for ( Tile **tile_ptr = nearby; *tile_ptr; tile_ptr++ ){
      Tile *tile = *tile_ptr;
      printf("Attempting to incrememt bomb count at (%2d,%2d)\n",
          tile -> x, tile -> y);
      printf("DEBUG: tile=%p\n", tile);
      // If it's a bomb, skip it
      if ( tile -> bomb == BOMB_HERE ) {
        printf("This tile already has a bomb. Skipping.\n");
        continue;
      }

      // Otherwise, increment its bomb index
      tile -> bomb++;
      printf("Tile's bomb count incremented to %d.\n", tile -> bomb);
      // Done!
    }
  }
  // Bombs assigned
  

  // Return the created board
  printf("Board initialization complete, returning.\n");
  return board;
}

/**
 * Exposes the board by setting all Tiles' status to STATUS_EXPOSED.
 *
 * @param board the board to flip
 */
void board_expose_all(Board *board) {
  // Loop through the board
  for ( size_t y = 0; y < board -> height; y++ ) {
    for ( size_t x = 0; x < board -> width; x++ ) {
      // Expose this tile
      board -> board[y][x] -> exposed = true;
    }
  }
}

/**
 * Exposes one blank tile on the board, to start off.
 * If a blank tile cannot be found after a certain number of retries,
 * exposes the first non-bomb tile it finds.
 *
 * @param board the board to expose a tile on
 */
void board_expose_safe(Board *board) {

  // Sanity check: Is the board just all bombs?
  if ( board -> mineCount == board -> width * board -> height ) {
    // There's no safe move.
    printf("ERROR: Attempted to expose safe tile, but board is all bombs.\n");
    printf("Returning without doing anything...\n");
    return; // TODO: Report error?
  }
  
  // Set some random coordinates to check
  printf("Picking a random coordinate to check...\n");
  short try_x = rand() % board -> width;
  short try_y = rand() % board -> height;

  // Limit to a set number of random checks
  const size_t MAX_ITERATIONS = 1000;

  // Strategy: Start only looking for blank spaces.
  // If we don't find any after 1000 iterations, move on to look for 1's.
  // If no 1's, then look for 2's, and so on.
  printf( "Beginning target loop...\n");
  for ( short target_bomb = 0; target_bomb <= 8; target_bomb++ ) {
    printf( "Looking for a target tile with bomb level %d\n", target_bomb );

    // Start the iteration count for this target bomb level
    size_t iteration = 0;
    do {
      printf("Checking for valid spot at (%2d,%2d)\n", try_x, try_y);
      // Check for a valid spot
      if ( board -> board[try_y][try_x] -> bomb == 0 ) {
        // If we find it, expose that spot
        printf("Found valid spot. Exposing...\n");
        board_expose_pick ( board, try_x, try_y );
        return;
      }
      // Pick another one
      printf("Spot was not a blank tile. Finding another to check...\n");
      try_x = rand() % board -> width;
      try_y = rand() % board -> height;

      // But, if we run out of iterations, move on...
    } while ( iteration < MAX_ITERATIONS );

    printf("DEBUG: Reached iteration limit for bomb level %d.\n", target_bomb);

  }

  printf("Reality has broken, or there's a bug somewhere. board.c:board_expose_safe()\n");
  // If we get here, then the whole board is almost definitely full of bombs,
  // which shouldn't be possible, because we checked for this at the start of
  // the method.
  return; // TODO: Complain about reality break
}


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
short board_expose_pick(Board *board, short x, short y) {
  printf("Beginning board_expose_pick with dimensions %2dx%2d at position (%2d,%2d)\n",
      board -> width, board -> height, x, y );
  // Check bounds
  if ( check_bounds(board, x, y) == ERR_OUT_OF_BOUNDS ) {
    printf("Position is out of bounds.\n");
    return ERR_OUT_OF_BOUNDS;
  }
  // Get the tile
  printf("Retrieving tile from board...\n");
  Tile *tile = board -> board[y][x];
  // If it's flagged, don't expose it, and return an invalid code
  if ( tile -> flagged ) {
    printf("Tile is flagged. Will not expose it.\n");
    return INVALID_FLAGGED;
  }

  // If it's already exposed
  if ( tile -> exposed ) {
    // If it's a number, and there's that many bombs nearby,
    // expose everything else
    if ( tile -> bomb > 0 && tile -> bomb < BOMB_HERE ) {
      // Check for nearby bombs
      //// Get list of nearby tiles
      //Tile *nearby[9] = { NULL };
      //list_nearby(board, x, y, nearby);
      //// Process list of nearby tiles
      //short nearby_flags = 0;
      //for ( Tile **tile_ptr = nearby; *tile_ptr; tile_ptr++ ) {
      //  Tile *tile_near = *tile_ptr;
      //  // If it's flagged, count it up
      //  if ( tile_near -> status == STATUS_FLAGGED ) {
      //    nearby_flags++;
      //  }
      //}
      // If number of nearby flags matches the number of expected bombs
      short nearby_flag_count = nearby_flags(board, x, y);
      if ( nearby_flag_count == tile -> bomb ) {
        // Expose all nearby blank, non-flagged tiles
        printf("Nearby flags matches indicated bombs of %d. Exposing nearby blanks.\n",
            tile -> bomb);
        // Get list of nearby tiles
        Tile *nearby[9] = { NULL };
        list_nearby(board, x, y, nearby);
        // Loop over list of nearby tiles to expose
        for ( int i = 0; nearby[i]; i++ ) {
          Tile *tile_near = nearby[i];
          // If it's blank and not flagged, expose it
          if ( !tile_near -> exposed && !tile_near -> flagged ) {
            short ret = board_expose_pick( board, tile_near -> x, tile_near -> y );
            // If we just exposed a flag, complain
            if ( ret == LOSE_MINE ) {
              printf("Exposed a nearby bomb. YOU LOSE!\n");
              return LOSE_MINE;
            }
          }
        }
      }
      // Number of nearby flags does not match.
      else {
        printf("Tile indicates %d bombs nearby, but %d tiles are flagged.\n",
            tile -> bomb, nearby_flag_count);
        printf("To expose all nearby tiles of this one, ensure flags = bombs.\n");
      }
    }

  }
  // If the tile isn't already exposed
  else{

    // Gonna go ahead and expose the tile
    printf("Exposing tile.\n");
    tile -> exposed = true;
    // Increment the board's exposed count
    board -> exposed++;

    // If it's a bomb, return a lose
    if ( tile -> bomb == BOMB_HERE ) {
      printf("Tile is a bomb. YOU LOSE!\n");
      return LOSE_MINE; 
    }
    // If it's a blank, then expose every other non-exposed tile around this one.
    else if ( tile -> bomb == 0 ) {
      printf("Tile is a blank. Exposing nearby tiles...\n");
      
      // Get a list of nearby cells
      Tile *nearby[9] = { NULL };
      list_nearby(board, x, y, nearby);
      printf("Got list of tiles.\n");

      // Loop across every nearby cell
      for ( Tile **tile_ptr = nearby; *tile_ptr; tile_ptr++ ) {
        Tile *tile = *tile_ptr;
        printf("Checking tile at (%2d,%2d)\n", tile -> x, tile -> y);
        
        // If it's already exposed or flagged, skip it
        if ( tile -> exposed || tile -> flagged ) {
          printf("Tile is already exposed or flagged, skipping.\n");
          continue;
        }

        // Otherwise, we've got a valid, blank cell to expose.
        printf("Recursively exposing tile at (%2d,%2d)\n", tile -> x, tile -> y);
        board_expose_pick(board, tile -> x, tile -> y);
      }

      // All nearby cells have been exposed
      printf("Done exposing nearby tiles from position (%2d,%2d)\n", x, y);
    }
    printf("Done exposing tile at (%2d,%2d).\n", x, y);
    printf("Exposed: %d of %d\n", board -> exposed,
        board -> width * board -> height - board -> mineCount);
  }

  // All necessary cells have been exposed, good to return
  return EXIT_SUCCESS;
  
}

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
short board_flag(Board *board, short x, short y) {
  // Check the bounds
  if ( check_bounds(board, x, y) == ERR_OUT_OF_BOUNDS ) {
    printf("Position is out of bounds.\n");
    return ERR_OUT_OF_BOUNDS;
  }

  // Check the tile
  Tile *tile = board -> board[y][x];
  if ( tile -> exposed ) {
    printf("Tile is already exposed; cannot flag it.\n");
    return INVALID_EXPOSED;
  }
  // Switch whether it's flagged or blank
  tile -> flagged = !tile->flagged;
  // All done!
  return EXIT_SUCCESS;
  
}


/**
 * Prints out the necessary escape codes to style subsequent text with the
 * provided style code, according to bash coloring / formatting.
 *
 * @param code the styling code to apply
 */
static void style(short code) {
  printf("\033[%dm", code);
}


static void print_border(size_t width) {
  printf("+-");
  for ( size_t x = 0; x < width; x++ ) {
    printf("--");
  }
  printf("+\n");
}


/**
 * Prints a single row of the board to the screen.
 * Includes a border around the edge, of the form:
 *
 * +---+
 * |   |
 * |   |
 * +---+
 * ( for 3x2 )
 *
 * @param row_num the number of the row to print, from the top
 * @param board the board to print data from
 */
static void print_row(short row_num, Board *board) {
  // If -1, print column labels and top border
  if ( row_num == -1 ) {
    // Skip to the right spot. 2 for row labels, 1 for left border
    printf("    ");
    // Loop across the board width
    for ( size_t x = 0; x < board -> width; x++ ) {
      // Print column headers as letters
      printf("%c ", 'A' + (char) x);
    }
    // Done with column labels, print newline
    printf("\n  ");
    print_border(board -> width);
  }
  // If height, print bottom border
  else if ( row_num == board -> height ) {
    printf("  ");
    print_border(board -> width);
  }
  // Otherwise, print row contents
  else {
    printf("%2d| ", row_num + 1);
    for ( size_t x = 0; x < board -> width; x++ ) {

      // Get the tile to print
      Tile *tile = board -> board[row_num][x];
      // Get the character to print
      char to_print = tile_toChar(tile);

      // Determine tile formatting
      // If this tile is selected
      if ( row_num == board -> cur_y && x == board -> cur_x ) {
        // Print this tile, inverted
        style(FMT_INV);
      }
      // If this tile is flagged INCORRECTLY, and exposed
      else if ( tile -> flagged && tile -> exposed &&
          tile -> bomb != BOMB_HERE ) {
        // Print with background as red
        style(COLOR_BG_L_RED);
      }
      // If this tile is flagged or an exposed bomb
      else if ( tile -> flagged 
          || (tile -> exposed && tile -> bomb == BOMB_HERE)) {
        // Print with background as red
        style(COLOR_BG_RED);
      }
      // If this tile is an exposed number
      else if ( tile -> exposed
            && tile -> bomb > 0 && tile -> bomb < BOMB_HERE ) {

        // COLORS

        // Check what number it is
        if ( tile -> bomb == 1 ) {
          style(COLOR_BLUE);
        } else if ( tile -> bomb == 2 ) {
          style(COLOR_L_GREEN);
        } else if ( tile -> bomb == 3 ) {
          style(COLOR_L_RED);
        } else if ( tile -> bomb == 4 ) {
          style(COLOR_MAGENTA);
        } else if ( tile -> bomb == 5 ) {
          style(COLOR_RED);
        } else if ( tile -> bomb == 6 ) {
          style(COLOR_CYAN);
        } else if ( tile -> bomb == 7 ) {
          style(COLOR_D_GRAY);
        } else if ( tile -> bomb == 8 ) {
          style(COLOR_L_GRAY);
        }

        // FORMATS
        
        // If it's satisfied, but there's still nearby blanks
        if ( tile -> bomb == nearby_flags(board, x, row_num)
          && nearby_blanks(board, x, row_num) > 0 ) {
          // Print with underline
          //style(FMT_UND);
          // This is a bit cheat-y
        }
        // Otherwise, if there are no nearby blanks
        else if ( nearby_blanks(board, x, row_num ) == 0 ) {
          // Print this tile, faded
          style(FMT_DIM);
        }
      }
      // Print this tile normally
      printf("%c", to_print);
      // Clear any formatting
      style(FMT_NONE);
      // Print the space after this tile
      printf(" ");
    }
    printf("|\n");
  }
}

/**
 * Prints the provided board to stdout. Includes a border around the edge.
 *
 * @param board the game board to print
 */
void board_print(Board *board) {
  
  // Loop through the rows, print one at a time
  for ( int y = -1; y <= board -> height; y++ ) {
    print_row(y, board);
  }

  printf("Board printed!\n");

}
