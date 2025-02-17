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

void spawn(matrix_t *a);
void create_shape(matrix_t *a, bool flag);
void init_shape_snake_x_params(matrix_t *a);
void init_shape_snake_y_params(matrix_t *a);
void create_x_snake(matrix_t *x_snake);
void create_y_snake(matrix_t *y_snake);
void init_shape_random(matrix_t *a);

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

void s21_print_matrix(matrix_t A) {  // change to mvprintw or wprint?
  if (A.rows < 1 || A.columns < 1) {
    return;
  }

  for (int i = 0; i < A.rows; i++) {
    for (int j = 0; j < A.columns; j++) {
      if (j == A.columns - 1)
        printw("%c\n", A.matrix[i][j]);
      else
        printw("%c", A.matrix[i][j]);
    }
  }
}

void init_win_params(WIN *p_win);
void print_win_params(WIN *p_win);
void create_box(WIN *win, bool flag);

int main(int argc, char *argv[]) {
  // matrix_t x_snake;
  matrix_t a;
  // matrix_t y_snake;
  // matrix_t shape_list[2] = {x_snake, y_snake};
  // matrix_t *mt = &shape_list[rand() % 3];
  WIN win;
  // WINDOW *loc_win;
  int ch;
  initscr();
  // start_color();
  cbreak();

  keypad(stdscr, TRUE);
  noecho();
  // init_pair(1, COLOR_CYAN, COLOR_BLACK);
  // create_x_snake(&x_snake);
  // s21_print_matrix(x_snake);
  // create_y_snake(&y_snake);
  init_win_params(&win);
  void print_win_params(WIN * p_win);
  init_shape_snake_x_params(&a);
  // init_shape_snake_y_params(&a);

  // init_shape_random(&x_snake);
  // init_shape_random(&y_snake);
  attron(COLOR_PAIR(1));
  printw("");
  refresh();
  attroff(COLOR_PAIR(1));
  create_box(&win, TRUE);
  create_shape(&a, TRUE);
  // spawn();
  // s21_print_matrix(a, loc_win);s
  while ((ch = getch()) != KEY_F(1)) {
    // matrix_t shape_list[2] = {x_snake, y_snake};
    // matrix_t a = shape_list[rand() % 3];
    // s21_print_matrix(a, loc_win);
    create_box(&win, TRUE);

    // create_shape(&x_snake, TRUE);
    switch (ch) {
      case KEY_LEFT:
        if (a.startx > (win.border.ls - (win.width - a.width))) {
          create_shape(&a, FALSE);
          --a.startx;
          --a.startx;
          create_shape(&a, TRUE);
        }

        break;
      case KEY_RIGHT:
        if (a.startx < (win.border.rs - 1)) {
          create_shape(&a, FALSE);
          ++a.startx;
          ++a.startx;
          create_shape(&a, TRUE);
        }
        break;

      case KEY_DOWN:
        if (a.starty < (win.height - 1)) {
          create_shape(&a, FALSE);
          ++a.starty;
          create_shape(&a, TRUE);
          break;

        } else {
          spawn(&a);
        }
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

// void create_next_shape() {

//   char shape_list[7] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
//   char ch = shape_list[rand()%8];
//   switch (ch)
//   {
//   case 'a':
//     /* code */
//     break;
//   case 'b':
//     /* code */
//     break;
//     case 'c':
//     /* code */
//     break;
//     case 'd':
//     /* code */
//     break;
//     case 'e':
//     /* code */
//     break;
//     case 'f':
//     /* code */
//     break;
//     case 'g':
//     /* code */
//     break;
//   default:
//     break;
//   }
// }

void init_shape_snake_x_params(matrix_t *a) {
  s21_create_matrix(4, 8, a);

  a->height = 1;
  a->width = 8;
  a->starty = 0 + 1;
  a->startx = ((COLS - a->width) / 2) + 1;

  a->matrix[0][0] = '[';
  a->matrix[0][1] = ']';
  a->matrix[0][2] = '[';
  a->matrix[0][3] = ']';
  a->matrix[0][4] = '[';
  a->matrix[0][5] = ']';
  a->matrix[0][6] = '[';
  a->matrix[0][7] = ']';
}

// void init_shape_snake_y_params(matrix_t *a) {
//   s21_create_matrix(4, 8, a);

//   a->height = 4;
//   a->width = 2;
//   a->starty = 0 + 2;
//   a->startx = ((COLS - a->width) / 2) + 1;

//   a->matrix[0][0] = '[';
//   a->matrix[0][1] = ']';
//   a->matrix[1][0] = '[';
//   a->matrix[1][1] = ']';
//   a->matrix[2][0] = '[';
//   a->matrix[2][1] = ']';
//   a->matrix[3][0] = '[';
//   a->matrix[3][1] = ']';
// }

void create_y_snake(matrix_t *y_snake) {
  s21_create_matrix(2, 4, y_snake);
  y_snake->matrix[0][0] = '[';
  y_snake->matrix[0][1] = ']';
  y_snake->matrix[1][0] = '[';
  y_snake->matrix[1][1] = ']';
  y_snake->matrix[2][0] = '[';
  y_snake->matrix[2][1] = ']';
  y_snake->matrix[3][0] = '[';
  y_snake->matrix[3][1] = ']';
  // y_snake->height = 1;
  // y_snake->width = 8;

  // return *y_snake;
}

void create_x_snake(matrix_t *x_snake) {
  s21_create_matrix(1, 8, x_snake);
  x_snake->matrix[0][0] = '[';
  x_snake->matrix[0][1] = ']';
  x_snake->matrix[0][2] = '[';
  x_snake->matrix[0][3] = ']';
  x_snake->matrix[0][4] = '[';
  x_snake->matrix[0][5] = ']';
  x_snake->matrix[0][6] = '[';
  x_snake->matrix[0][7] = ']';
  // return *x_snake;
}

// void init_shape_snake_x_params(matrix_t *a) {
//   s21_create_matrix(4, 8, a);

//   a->height = 1;
//   a->width = 8;
//   a->starty = 0 + 1;
//   a->startx = ((COLS - a->width) / 2) + 1;

//   // a->matrix[0][0] = 0;
//   // a->matrix[0][1] = 0;
//   // a->matrix[0][2] = 0;
//   // a->matrix[0][3] = 0;
//   // a->matrix[0][4] = 0;
//   // a->matrix[0][5] = 0;
//   // a->matrix[0][6] = 0;
//   // a->matrix[0][7] = 0;

//   a->matrix[0][0] = '[';
//   a->matrix[0][1] = ']';
//   a->matrix[0][2] = '[';
//   a->matrix[0][3] = ']';
//   a->matrix[0][4] = '[';
//   a->matrix[0][5] = ']';
//   a->matrix[0][6] = '[';
//   a->matrix[0][7] = ']';
// }

void init_shape_random(matrix_t *a) {
  a->starty = 0 + 1;
  a->startx = ((COLS - a->width) / 2) + 1;
  a->height = a->rows;
  a->width = a->columns;

  // a->matrix[0][0] = '[';
  // a->matrix[0][1] = ']';
  // a->matrix[1][0] = '[';
  // a->matrix[1][1] = ']';
  // a->matrix[2][0] = '[';
  // a->matrix[2][1] = ']';
  // a->matrix[3][0] = '[';
  // a->matrix[3][1] = ']';
}

void spawn(matrix_t *a) {
  char shape_list[7] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
  char chaa = shape_list[rand() % 8];
  switch (chaa) {
    case 'a':
      init_shape_snake_x_params(a);
      break;
    case 'b':
      init_shape_snake_x_params(a);
      break;
    case 'c':
      init_shape_snake_x_params(a);
      break;
    case 'd':
      init_shape_snake_x_params(a);
      break;
    case 'e':
      init_shape_snake_x_params(a);
      break;
    case 'f':
      init_shape_snake_x_params(a);
      break;
    case 'g':
      init_shape_snake_x_params(a);
      break;
    default:
      break;
  }
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

void print_win_params(WIN *p_win) {
#ifdef _DEBUG
  mvprintw(25, 0, "%d %d %d %d %d", p_win->startx, p_win->starty, p_win->width,
           p_win->height);
  refresh();
#endif
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