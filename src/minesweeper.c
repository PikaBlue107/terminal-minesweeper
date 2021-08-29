#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
//#include <ncurses.h>

typedef enum action_enum {
  FLAG,
  EXPOSE
} Action;

typedef struct move_struct {
  short x;
  short y;
  Action action;
} Move;

//void get_action(Board *board, char *action) {
//  //cbreak(); // Line buffering disabled
//  char tmp = getch();
//  //nocbreak(); // Enable line buffering
//  printf("Char: [%c] (%d)\n", tmp, tmp);
//}

/**
 * Gets a valid position on the board from the user.
 * Columns are spreadsheet style (a/A, b/B, c, ..., z, aa, ab, ..., az, ba, ...)
 * Rows are direct numbers
 *
 * @param board the board containing the width and height, for error checking
 * @param move a move struct for us to place the user's move into
 */
void get_move(Board *board, Move *move) {

  // SETUP

  // Check for how many column characters we need
  size_t column_chars;
  if ( board -> width <= 26 ) {
    column_chars = 1;
  } else if ( board -> width <= 26 + 26 * 26 ) {
    column_chars = 2;
  } else {
    // Assume that nobody is going to make a board more than 18278 columns wide
    column_chars = 3;
  }
  // Check for how many row characters we need
  size_t row_chars = 0;
  int tmp_height = board -> height + 1;
  do {
    // Divide by 10 to chop off a digit
    tmp_height /= 10;
    // Add a character for the digit
    row_chars++;
  } while ( tmp_height != 0 );
  printf("column_chars=%zu, row_chars=%zu\n", column_chars, row_chars);

  // Set up buffers to read in positions
  char column[column_chars + 1];
  char row[row_chars + 1];
  char line[32] = { 0 };
  char extra[32] = { 0 };
  char action_char = '\0';
  // Create the format string
  char format[32];
  snprintf(format, sizeof(format), "%%c%%%zu[a-zA-Z]%%%zu[0-9]%%[^\n]",
      column_chars, row_chars);
  printf("Print format: %s\n", format);

  // PERFORM OPERATION

  bool first_time = true;
  move -> x = -1;
  move -> y = -1;
  move -> action = -1;

  do {

    if ( !first_time ) {
      printf("Please enter a position in the format [EF][a-zA-Z]+[0-9]+.\n");
      int tmp_width = board -> width;
      char last_column[column_chars + 1];
      memset ( last_column, 0, column_chars + 1 );
      for ( short i = column_chars - 1; i >= 0; i-- ) {
        last_column[i] = 'A' + ( tmp_width % 26 ) - 1;
        tmp_width /= 26;
      }
      printf("  Moves: [E]xpose, [F]lag. Column in A-%s, row in 0-%d.\n",
          last_column, board -> height - 1 );
    }
    first_time = false;


    memset(column, 0, column_chars + 1);
    memset(row, 0, row_chars + 1);
    memset(extra, 0, row_chars + 1);

    // Read in one line
    printf("Reading in one line. Make it nice!\n");
    if ( fgets(line, sizeof(line), stdin) != line ) {
      printf("Problem reading in line data, exiting...\n");
      exit(EXIT_FAILURE);
    }
    // Check that it's less than the size limit
    if ( line[strlen(line) - 1] != '\n' ) {
      printf("strlen(line): %zu, line[strlen(line) - 1]: [%c]\n",
          strlen(line), line[strlen(line)] );
      printf("Problem: You entered too much data.\n");
      continue; // TODO: Probably have to consume that data until line is empty
    }
    printf("You entered: %s\n", line);

    // Use scanf to read in characters for position
    short scanned = sscanf( line, format, &action_char, column, row, extra );
    printf("Row: [%s] Column: [%s]\n", row, column);
    // Check for didn't get all values
    if ( scanned < 3 ) {
      printf("Problem pulling row and column out (only got %d).\n", scanned);
      continue;
    }
    // Check for illegal action
    action_char = tolower(action_char);
    if ( action_char == 'e' ) {
      move -> action = EXPOSE;
    } else if ( action_char == 'f' ) {
      move -> action = FLAG;
    } else {
      printf("Error: Unknown action '%c'. Allowed actions: [E]xpose, [F]lag.\n",
          action_char);
      continue;
    }
    // Check for extra data at end
    printf("Extra data: [%s]\n", extra);
    if ( strlen(extra) > 0 ) {
      printf("Problem, extra data at end of input (got %s)\n", extra);
      continue;
    }

    // Parse out the x column number
    move -> x = 0;
    for ( size_t column_char_idx = 0; column_char_idx < column_chars;
        column_char_idx++ ) {
      // Scale up any previous digits
      move -> x *= 26;
      // Add in a new digit
      move -> x += tolower( column[column_char_idx] ) - 'a';
      printf("char: %c, tolower: %d, 'a': %d\n", column[column_char_idx],
          tolower( column[column_char_idx] ), 'a');
    }

    // Parse out the y row from digits directly
    move -> y = atoi(row) - 1;

    // Confirm what we have
    printf("X: %d Y: %d\n", move -> x, move -> y);

  } while ( move -> x < 0 || move -> y < 0 || 
      move -> x >= board -> width || move -> y >= board -> height );

  // Valid position. Yay!

}

int main(int argc, char *argv[]) {

  //initscr();
  //clear();
  //noecho();

  printf("Creating board!\n");
  struct Board *board = newBoard( 9, 10, 15 );
  printf("Board created, printing it out...\n");
  board_print(board);
  printf("Done printing out the board.\n");

  printf("Exposing a starter block...\n");
  board_expose_safe(board);
  Move *move = malloc(sizeof(Move));
  
  //printf("TESTING getch\n");
  //char action = '\0';
  //get_action(board, &action);

  while ( true ) {
    // Print out board
    printf("Printing out the board again...\n");
    board_print(board);

    // Request position to reveal
    printf("Pick a position to expose.\n");
    get_move(board, move);

    // Parse response
    printf("Move: %s (%2d, %2d)\n",
        (move -> action == EXPOSE) ? "Expose" : "Flag",
        move -> x, move -> y);

    // Check the action
    if ( move -> action == EXPOSE ) {
      // Reveal that position
      int result = board_expose_pick( board, move -> x, move -> y );
      // Check for end conditions
      // If player just exposed a mine
      if ( result == LOSE_MINE ) {
        // Player lost. Expose the board
        board_expose_all(board);
        // Print out a defeat message
        printf("You lost!\n");
        // Print out the board
        board_print(board);
        // Exit the loop
        break;
      }
      // If the player just exposed the last non-mine
      else if ( board -> exposed == (board -> width * board -> height
            - board -> mineCount ) ) {
        // Player won! Expose board
        board_expose_all(board);
        // Print out a victory message
        printf("You win!\n");
        // Print out the board
        board_print(board);
        // Exit the loop
        break;
      }
    } else {
      // Flag that position
      board_flag( board, move -> x, move -> y );
    }
  }

  free(move);

  return EXIT_SUCCESS;
}
