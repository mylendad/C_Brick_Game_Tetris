#include <ncurses.h>
#include <stdlib.h>

typedef struct _win_border_struct {
  chtype ls, rs, ts, bs, tl, tr, bl, br;
} WIN_BORDER;

typedef struct _WIN_struct {
  int startx, starty;
  int height, width;
  WIN_BORDER border;
} WIN;

typedef struct matrix_struct {
  int startx, starty;
  int height, width;
  char **matrix;
  int rows;
  int columns;
} matrix_t;

void create_shape(matrix_t *a, bool flag);
void init_shape_snake_x_params(matrix_t *a);
void init_shape_snake_y_params(matrix_t *a);

int s21_create_matrix(int rows, int columns, matrix_t *result) {
  int res = OK;
  if (rows < 1 || columns < 1 || result == NULL) {
    res = 1;
  } else {
    result->rows = rows;
    result->columns = columns;
    result->matrix = (char **)calloc(rows, sizeof(char *));
    for (int i = 0; i < rows; i++) {
      result->matrix[i] = (char *)calloc(columns, sizeof(char));
    }
  }
  return res;
}

int s21_transpose(matrix_t *A, matrix_t *result) {
  if (A == NULL || result == NULL || A->matrix == NULL || A->rows < 1 ||
      A->columns < 1)
    return 1;

  s21_create_matrix(A->columns, A->rows, result);
  for (int i = 0; i < A->columns; i++) {
    for (int j = 0; j < A->rows; j++) {
      result->matrix[i][j] = A->matrix[j][i];
    }
  }
  return OK;
}

void s21_print_matrix(matrix_t A,
                      WINDOW *wind) {  // change to mvprintw or wprint?
  if (A.rows < 1 || A.columns < 1) {
    return;
  }

  for (int i = 0; i < A.rows; i++) {
    for (int j = 0; j < A.columns; j++) {
      if (j == A.columns - 1)
        wprintw(wind, "%c\n", A.matrix[i][j]);
      else
        wprintw(wind, "%c", A.matrix[i][j]);
    }
  }
}

void init_win_params(WIN *p_win);
void print_win_params(WIN *p_win);
void create_box(WIN *win, bool flag);

int main(int argc, char *argv[]) {
  matrix_t a;
  // s21_create_matrix(4, 8, &a);

  // a.matrix[0][0] = 0;
  // a.matrix[0][1] = 0;
  // a.matrix[0][2] = 0;
  // a.matrix[0][3] = 0;
  // a.matrix[0][4] = 0;
  // a.matrix[0][5] = 0;
  // a.matrix[0][6] = 0;
  // a.matrix[0][7] = 0;

  // a.matrix[1][0] = '[';
  // a.matrix[1][1] = ']';
  // a.matrix[1][2] = '[';
  // a.matrix[1][3] = ']';
  // a.matrix[1][4] = '[';
  // a.matrix[1][5] = ']';
  // a.matrix[1][6] = '[';
  // a.matrix[1][7] = ']';

  // a.matrix[2][0] = 0;
  // a.matrix[2][1] = 0;
  // a.matrix[2][2] = 0;
  // a.matrix[2][3] = 0;
  // a.matrix[2][4] = 0;
  // a.matrix[2][5] = 0;
  // a.matrix[2][6] = 0;
  // a.matrix[2][7] = 0;

  // a.matrix[3][0] = 0;
  // a.matrix[3][1] = 0;
  // a.matrix[3][2] = 0;
  // a.matrix[3][3] = 0;
  // a.matrix[3][4] = 0;
  // a.matrix[3][5] = 0;
  // a.matrix[3][6] = 0;
  // a.matrix[3][7] = 0;

  WIN win;
  // WINDOW *loc_win;
  int ch;
  initscr();
  // start_color();
  cbreak();

  keypad(stdscr, TRUE);
  noecho();
  // init_pair(1, COLOR_CYAN, COLOR_BLACK);

  init_win_params(&win);
  init_shape_snake_x_params(&a);
  // init_shape_snake_y_params(&a);
  attron(COLOR_PAIR(1));
  printw("Press F1 to exit");
  refresh();
  attroff(COLOR_PAIR(1));
  create_box(&win, TRUE);
  create_shape(&a, TRUE);
  // s21_print_matrix(a, loc_win);
  while ((ch = getch()) != KEY_F(1)) {
    // s21_print_matrix(a, loc_win);
    create_box(&win, TRUE);
    // create_shape(&a, TRUE);
    switch (ch) {
      case KEY_LEFT:
        create_shape(&a, FALSE);
        --a.startx;
        create_shape(&a, TRUE);
        break;
      case KEY_RIGHT:
        create_shape(&a, FALSE);
        ++a.startx;
        create_shape(&a, TRUE);
        break;

      case KEY_DOWN:
        create_shape(&a, FALSE);
        ++a.starty;
        create_shape(&a, TRUE);
        break;
    }
  }

  endwin();
  return 0;
}

void init_win_params(WIN *p_win) {
  p_win->height = 20;
  p_win->width = 20;
  p_win->starty = 0;
  p_win->startx = (COLS - p_win->width) / 2;
  p_win->border.ls = '|';
  p_win->border.rs = '|';
  p_win->border.ts = '-';
  p_win->border.bs = '-';
  p_win->border.tl = ACS_ULCORNER;
  p_win->border.tr = ACS_URCORNER;
  p_win->border.bl = ACS_LLCORNER;
  p_win->border.br = ACS_LRCORNER;
}

void init_shape_snake_x_params(matrix_t *a) {
  s21_create_matrix(4, 8, a);

  a->height = 1;
  a->width = 8;
  a->starty = 0 + 2;
  a->startx = ((COLS - a->width) / 2) + 1;

  // a->matrix[0][0] = 0;
  // a->matrix[0][1] = 0;
  // a->matrix[0][2] = 0;
  // a->matrix[0][3] = 0;
  // a->matrix[0][4] = 0;
  // a->matrix[0][5] = 0;
  // a->matrix[0][6] = 0;
  // a->matrix[0][7] = 0;

  a->matrix[0][0] = '[';
  a->matrix[0][1] = ']';
  a->matrix[0][2] = '[';
  a->matrix[0][3] = ']';
  a->matrix[0][4] = '[';
  a->matrix[0][5] = ']';
  a->matrix[0][6] = '[';
  a->matrix[0][7] = ']';
}

void init_shape_snake_y_params(matrix_t *a) {
  s21_create_matrix(4, 8, a);

  a->height = 4;
  a->width = 2;
  a->starty = 0 + 2;
  a->startx = ((COLS - a->width) / 2) + 1;

  a->matrix[0][0] = '[';
  a->matrix[0][1] = ']';
  a->matrix[1][0] = '[';
  a->matrix[1][1] = ']';
  a->matrix[2][0] = '[';
  a->matrix[2][1] = ']';
  a->matrix[3][0] = '[';
  a->matrix[3][1] = ']';
}

void create_shape(matrix_t *a, bool flag) {
  int i, j;
  int x, y, w, h;
  x = a->startx;
  y = a->starty;
  w = a->width;
  h = a->height;

  if (flag == TRUE) {
    for (i = 0; i < a->height; ++i) {
      for (j = 0; j < a->width; ++j) {
        mvaddch(y + i, x + j, a->matrix[i][j]);
        // mvaddch(y + 1, x + 1, a->matrix[0][1]);
        // mvaddch(y + 1, x + 2, a->matrix[0][2]);
        // mvaddch(y + 1, x + 3, a->matrix[0][3]);
        // mvaddch(y + 1, x + 4, a->matrix[0][4]);
        // mvaddch(y + 1, x + 5, a->matrix[0][5]);
        // mvaddch(y + 1, x + 6, a->matrix[0][6]);
        // mvaddch(y + 1, x + 7, a->matrix[0][7]);

        // mvaddch(y + 1, x, a->matrix[1][0]);
        // mvaddch(y + 1, x + 1, a->matrix[1][1]);
        // mvaddch(y + 1, x + 2, a->matrix[1][2]);
        // mvaddch(y + 1, x + 3, a->matrix[1][3]);
        // mvaddch(y + 1, x + 4, a->matrix[1][4]);
        // mvaddch(y + 1, x + 5, a->matrix[1][5]);
        // mvaddch(y + 1, x + 6, a->matrix[1][6]);
        // mvaddch(y + 1, x + 7, a->matrix[1][7]);

        // mvaddch(y + 1, x, a->matrix[2][0]);
        // mvaddch(y + 1, x + 1, a->matrix[2][1]);
        // mvaddch(y + 1, x + 2, a->matrix[3][2]);
        // mvaddch(y + 1, x + 3, a->matrix[0][3]);
        // mvaddch(y + 1, x + 4, a->matrix[0][4]);
        // mvaddch(y + 1, x + 5, a->matrix[0][5]);
        // mvaddch(y + 1, x + 6, a->matrix[0][6]);
        // mvaddch(y + 1, x + 7, a->matrix[0][7]);

        // mvaddch(y + 0, x + 7, a->matrix[0][7]);
        // mvaddch(y + 0, x + 8, a->matrix[0][8]);
        // mvaddch(y + 1, x + 7, a->matrix[1][7]);
        // mvaddch(y + 1, x + 8, a->matrix[1][8]);
        // mvaddch(y + 2, x + 7, a->matrix[2][7]);
        // mvaddch(y + 2, x + 8, a->matrix[2][8]);
        // mvaddch(y + 3, x + 7, a->matrix[3][7]);
        // mvaddch(y + 3, x + 8, a->matrix[3][8]);
      }
    }
  } else {
    for (j = y; j <= y + h; ++j) {
      for (i = x; i <= x + w; ++i) {
        mvaddch(j, i, ' ');
      }
    }
    refresh();
  }
}

void create_box(WIN *p_win, bool flag) {
  int i, j;
  int x, y, w, h;
  x = p_win->startx;
  y = p_win->starty;
  w = p_win->width;
  h = p_win->height;

  if (flag == TRUE) {
    mvaddch(y, x, p_win->border.tl);
    mvaddch(y, x + w, p_win->border.tr);
    mvaddch(y + h, x, p_win->border.bl);
    mvaddch(y + h, x + w, p_win->border.br);
    mvhline(y, x + 1, p_win->border.ts, w - 1);
    mvhline(y + h, x + 1, p_win->border.bs, w - 1);
    mvvline(y + 1, x, p_win->border.ls, h - 1);
    mvvline(y + 1, x + w, p_win->border.rs, h - 1);
  } else {
    for (j = y; j <= y + h; ++j) {
      for (i = x; i <= x + w; ++i) {
        mvaddch(j, i, ' ');
      }
    }
    refresh();
  }
}