#include <ncurses.h>
#include <stdlib.h>

typedef struct _win_border_struct {
  chtype ls, rs, ts, bs, tl, tr, bl, br;
} WIN_BORDER;

typedef struct _WIN_struct {
  char **matrix;
  int startx, starty;
  int height, width;
  WIN_BORDER border;
} WIN;

// typedef struct matrix_struct {  // переделать ровс и колумн убрать или вообще
//                                 // объеденить с WIN
//   int startx, starty;
//   int height, width;
//   char **matrix;
//   int height;
//   int columns;
// } WIN;

void spawn(WIN *a);
void create_shape(WIN *a, bool flag);
void init_shape_snake_x_params(WIN *a);
void init_shape_snake_y_params(WIN *a);
void create_x_snake(WIN *x_snake);
void create_y_snake(WIN *y_snake);
void init_shape_random(WIN *a);
void copy_matrix_a_to_window(WIN *win, WIN *a);

int s21_create_matrix(int height, int columns, int startx, int starty,
                      WIN *result) {
  int res = OK;
  if (height < 1 || columns < 1 || result == NULL) {
    res = 1;
  } else {
    result->startx = startx;
    result->starty = starty;
    result->height = height;
    result->width = columns;
    result->matrix = (char **)calloc(height, sizeof(char *));
    for (int i = 0; i < height; i++) {
      result->matrix[i] = (char *)calloc(columns, sizeof(char));
    }
  }
  return res;
}

void s21_print_matrix(WIN A) {  // change to mvprintw or wprint?
  if (A.height < 1 || A.width < 1) {
    return;
  }

  for (int i = 0; i < A.height; i++) {
    for (int j = 0; j < A.width; j++) {
      if (j == A.width - 1)
        printw("%c\n", A.matrix[i][j]);
      else
        printw("%c", A.matrix[i][j]);
    }
  }
}

void init_win_params(WIN *p_win);
void print_win_params(WIN *p_win);
void create_box(WIN *win, bool flag);

int main() {
  WIN a;
  // WIN win;
  // s21_create_matrix(20, 20,  0,0, &win);
  WIN win;

  int ch;
  initscr();
  cbreak();

  keypad(stdscr, TRUE);
  // noecho();
  init_win_params(&win);
  void print_win_params(WIN * p_win);
  init_shape_snake_x_params(&a);
  // attron(COLOR_PAIR(1));
  // printw("");
  refresh();
  // attroff(COLOR_PAIR(1));
  create_box(&win, TRUE);
  create_shape(&a, TRUE);
  while ((ch = getch()) != KEY_F(1)) {
    create_box(&win, TRUE);
    switch (ch) {
      case KEY_LEFT:
        if (a.startx > (win.startx + 2)) {
          create_shape(&a, FALSE);
          --a.startx;
          --a.startx;
          create_shape(&a, TRUE);
        }

        break;
      case KEY_RIGHT:
        if (a.startx < (win.startx + (win.width - a.width))) {
          create_shape(&a, FALSE);
          ++a.startx;
          ++a.startx;
          create_shape(&a, TRUE);
        }
        break;

      case KEY_DOWN:
        if (a.starty < (win.height - a.height) &&
            (win.matrix[a.starty][a.startx] != ']' &&
             win.matrix[a.starty][a.startx] !=
                 '[')) {  // sega if +2) выход за массив?
                          // if (a.starty < (win.height - a.height)) {
          // if (win.matrix[a.starty + 3][a.startx] <= 90) {  // sega
          create_shape(&a, FALSE);
          ++a.starty;
          // void print_win_params(WIN * p_swin);
          create_shape(&a, TRUE);
        } else {
          copy_matrix_a_to_window(&win, &a);  // change
          // s21_print_matrix(win);
          spawn(&a);
        }
        break;

        // else {s
        //   copy_matrix_a_to_window(&win, &a);
        //   s21_print_matrix(win);
        //   spawn(&a);
        // }
    }
  }
  endwin();
  return 0;
}

void copy_matrix_a_to_window(WIN *win, WIN *a) {
  for (int i = 0; i < a->height; i++) {
    for (int j = 0; j < a->width; j++) {
      int screen_y = a->starty + i;
      int screen_x = a->startx + j;

      chtype ch = mvinch(screen_y, screen_x);

      if (screen_y >= 0 && screen_y < win->height && screen_x >= 0 &&
          screen_x < win->width) {
        win->matrix[screen_y][screen_x] = (char)ch;
      }
    }
  }
}

void init_win_params(WIN *p_win) {
  p_win->height = 20;
  p_win->width = 20;
  s21_create_matrix(p_win->height, p_win->width, 0, 0, p_win);
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

void init_shape_snake_x_params(WIN *a) {
  s21_create_matrix(4, 8, 0, 0, a);

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

void init_shape_snake_y_params(WIN *a) {
  s21_create_matrix(4, 8, 0, 0, a);

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

void spawn(WIN *a) {
  char shape_list[7] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
  char chaa = shape_list[rand() % 8];
  switch (chaa) {
    case 'a':
      init_shape_snake_x_params(a);
      break;
    case 'b':
      init_shape_snake_y_params(a);
      break;
    case 'c':
      init_shape_snake_x_params(a);
      break;
    case 'd':
      init_shape_snake_y_params(a);
      break;
    case 'e':
      init_shape_snake_y_params(a);
      break;
    case 'f':
      init_shape_snake_x_params(a);
      break;
    case 'g':
      init_shape_snake_y_params(a);
      break;
    default:
      break;
  }
}

void create_shape(WIN *a, bool flag) {
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