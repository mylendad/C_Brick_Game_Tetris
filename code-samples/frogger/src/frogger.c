#include "../inc/frogger.h"
#include <stdlib.h>


typedef struct matrix_struct {
  char **matrix;
  int rows;
  int columns;
} matrix_t;

int s21_create_matrix(int rows, int columns, matrix_t *result) {
  int res = OK;
  if (rows < 1 || columns < 1 || result == NULL) {
    res = 1;
  } else {
    result->rows = rows;
    result->columns = columns;
    result->matrix = (char**)calloc(rows, sizeof(char*));
    for (int i = 0; i < rows; i++) {
      result->matrix[i] = (char*)calloc(columns, sizeof(char));
    }
  }
  return res;
}

void s21_print_matrix(matrix_t A) { // change to mvprintw or wprint?
  if (A.rows < 1 || A.columns < 1) {
    return;
  }

  for (int i = 0; i < A.rows; i++) {
    for (int j = 0; j < A.columns; j++) {
      if (j == A.columns - 1)
        printf("%c\n", A.matrix[i][j]);
      else
        printf("%c", A.matrix[i][j]);
    }
  }
}


WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

int main(void)
{

    matrix_t a;
s21_create_matrix(4, 8, &a);

a.matrix[0][0] = 0;
  a.matrix[0][1] = 0;
  a.matrix[0][2] = 0;
  a.matrix[0][3] = 0;
  a.matrix[0][4] = 0;
  a.matrix[0][5] = 0;
  a.matrix[0][6] = 0;
  a.matrix[0][7] = 0;

   a.matrix[1][0] = '[';
  a.matrix[1][1] = ']';
  a.matrix[1][2] = '[';
  a.matrix[1][3] = ']';
  a.matrix[1][4] = '[';
  a.matrix[1][5] = ']';
  a.matrix[1][6] = '[';
  a.matrix[1][7] = ']';

  a.matrix[2][0] = 0;
  a.matrix[2][1] = 0;
  a.matrix[2][2] = 0;
  a.matrix[2][3] = 0;
  a.matrix[2][4] = 0;
  a.matrix[2][5] = 0;
  a.matrix[2][6] = 0;
  a.matrix[2][7] = 0;

  a.matrix[3][0] = 0;
  a.matrix[3][1] = 0;
  a.matrix[3][2] = 0;
  a.matrix[3][3] = 0;
  a.matrix[3][4] = 0;
  a.matrix[3][5] = 0;
  a.matrix[3][6] = 0;
  a.matrix[3][7] = 0;
 s21_print_matrix(a);

initscr();			/* Start curses mode 		*/
	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/


    // WIN_INIT(50);
    // setlocale(LC_ALL, "");
    // print_overlay();
    // print_rectangle(0, ROWS_MAP + 1, 0, COLS_MAP + 1);
    // print_rectangle(0, BOARD_N + 1, 0, BOARD_M + 1);

    create_newwin(10, 20, 5, 5);

    game_loop();
    endwin();

    return SUCCESS;
}

void game_loop()
{
    board_t map;
    game_stats_t stats;
    player_pos frog;

    bool break_flag = TRUE;
    int signal = 0;
    frog_state state = START;

    stats_init(&stats);

    while (break_flag)
    {   
        if (state == GAMEOVER || state == EXIT_STATE || state == FILE_ERROR_STATE)
            break_flag = FALSE;

        sigact(get_signal(signal), &state, &stats, &map, &frog);

        if (state == MOVING || state == START)
            signal = GET_USER_INPUT;
    }

    if (state == FILE_ERROR_STATE)
    {
        print_levelerror();
        nodelay(stdscr, FALSE);
        getch();
    }
}