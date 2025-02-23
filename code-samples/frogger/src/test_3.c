#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct _win_border_struct {
  chtype ls, rs, ts, bs, tl, tr, bl, br, xx;
} WIN_BORDER;

typedef struct _WIN_struct {
  char **matrix;
  int startx, starty;
  int height, width;
  int two_right_x, three_right_x, four_right_x, two_right_y, three_right_y,
      two_left_x, three_left_x, four_left_x, two_left_y, three_left_y,
      four_left_y, four_right_y, one_right_x, one_right_y, one_left_x,
      one_left_y, one_bottom_x, two_bottom_x, three_bottom_x, four_bottom_x,
      one_bottom_y, two_bottom_y, three_bottom_y, four_bottom_y;
  WIN_BORDER border;
} WIN;

// typedef struct matrix_struct {  // переделать ровс и колумн убрать или вообще
//                                 // объеденить с WIN
//   int startx, starty;
//   int height, width;
//   char **matrix;
//   int height;
//   int width;
// } WIN;
void down(WIN *a, WIN *win);
void logic_x(WIN *a);
void logic_y(WIN *a);
bool check_collision(WIN *a);
void update_coordinates(WIN *a);
void start_x(WIN *a);
void start_y(WIN *a);
void rotate(WIN *a);
void spawn(WIN *a);
void create_shape(WIN *a, bool flag);
void init_shape_snake_x_params(WIN *a);
void init_shape_snake_y_params(WIN *a);
void create_x_snake(WIN *x_snake);
void create_y_snake(WIN *y_snake);
void init_shape_random(WIN *a);
void copy_matrix_a_to_window(WIN *win, WIN *a);

int s21_create_matrix(int height, int width, int startx, int starty,
                      WIN *result) {
  int res = OK;
  if (height < 1 || width < 1 || result == NULL) {
    res = 1;
  } else {
    result->startx = startx;
    result->starty = starty;
    result->height = height;
    result->width = width;
    result->matrix = (char **)calloc(height, sizeof(char *));
    for (int i = 0; i < height; i++) {
      result->matrix[i] = (char *)calloc(width, sizeof(char));
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
        mvprintw(5, 5, "%c\n", A.matrix[i][j]);
      else
        mvprintw(5, 5, "%c", A.matrix[i][j]);
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
  noecho();
  // curs_set(0);
  nodelay(stdscr, TRUE);
  init_win_params(&win);
  // void print_win_params(WIN * p_win);
  init_shape_snake_x_params(&a);
  start_x(&a);
  logic_x(&a);
  // attron(COLOR_PAIR(1));
  // printw("");sss
  refresh();
  // attroff(COLOR_PAIR(1));
  create_box(&win, TRUE);
  create_shape(&a, TRUE);
  // start(&a);
  int timer = 0;
  while (1) {
    // create_box(&win, TRUE);
    // create_shape(&a, TRUE);
    // down(&a, &win);
    // usleep(500000);

    int control = getch();
    // while ((ch = getch()) != KEY_F(1)) {
    // switch (ch) {
    if (control != ERR) {
      if (control == KEY_UP) {
        create_shape(&a, FALSE);
        rotate(&a);

        // if (check_collision(&win, &a)) {
        //   rotate(&a);
        //   rotate(&a);
        // }
        create_shape(&a, TRUE);
      }
      // break;
      if (control == KEY_LEFT) {
        if (a.border.xx == 'b' && (a.startx > (win.startx + 2)) &&
            mvinch(a.one_left_y, a.one_left_x - 1) != ']' &&
            mvinch(a.two_left_y, a.two_left_x - 1) != ']' &&
            mvinch(a.three_left_y, a.three_left_x - 1) != ']' &&
            mvinch(a.four_left_y, a.four_left_x - 1) != ']') {
          create_shape(&a, FALSE);
          --a.startx;
          --a.startx;

          --a.one_left_x;
          --a.one_left_x;
          --a.two_left_x;
          --a.two_left_x;
          --a.three_left_x;
          --a.three_left_x;
          --a.four_left_x;
          --a.four_left_x;

          --a.one_right_x;
          --a.one_right_x;
          --a.two_right_x;
          --a.two_right_x;
          --a.three_right_x;
          --a.three_right_x;
          --a.four_right_x;
          --a.four_right_x;

          --a.one_bottom_x;
          --a.one_bottom_x;
          create_shape(&a, TRUE);
        } else if (a.border.xx == 'a' && (a.startx > (win.startx + 2)) &&
                   mvinch(a.one_left_y, a.one_left_x - 1) != ']') {
          create_shape(&a, FALSE);
          --a.startx;
          --a.startx;

          --a.one_left_x;
          --a.one_left_x;

          --a.one_right_x;
          --a.one_right_x;

          --a.one_bottom_x;
          --a.one_bottom_x;
          --a.two_bottom_x;
          --a.two_bottom_x;
          --a.three_bottom_x;
          --a.three_bottom_x;
          --a.four_bottom_x;
          --a.four_bottom_x;
          // void print_win_params(WIN * p_swin);
          create_shape(&a, TRUE);
        }
      }
      // break
      if (control == KEY_RIGHT) {
        if (a.border.xx == 'b' &&
            a.startx < (win.startx + (win.width - a.width)) &&
            mvinch(a.one_right_y, a.one_right_x + 1) != '[' &&
            mvinch(a.two_right_y, a.two_right_x + 1) != '[' &&
            mvinch(a.three_right_y, a.three_right_x + 1) != '[' &&
            mvinch(a.four_right_y, a.four_right_x + 1) != '[') {
          create_shape(&a, FALSE);

          ++a.startx;
          ++a.startx;

          ++a.one_right_x;
          ++a.one_right_x;
          ++a.two_right_x;
          ++a.two_right_x;
          ++a.three_right_x;
          ++a.three_right_x;
          ++a.four_right_x;
          ++a.four_right_x;

          ++a.one_left_x;
          ++a.one_left_x;
          ++a.two_left_x;
          ++a.two_left_x;
          ++a.three_left_x;
          ++a.three_left_x;
          ++a.four_left_x;
          ++a.four_left_x;

          ++a.one_bottom_x;
          ++a.one_bottom_x;

          create_shape(&a, TRUE);
        } else if (a.border.xx == 'a' &&
                   a.startx < (win.startx + (win.width - a.width)) &&
                   mvinch(a.one_right_y, a.one_right_x + 1) != '[') {
          create_shape(&a, FALSE);
          ++a.startx;
          ++a.startx;

          ++a.one_right_x;
          ++a.one_right_x;

          ++a.one_left_x;
          ++a.one_left_x;

          ++a.one_bottom_x;
          ++a.one_bottom_x;
          ++a.two_bottom_x;
          ++a.two_bottom_x;
          ++a.three_bottom_x;
          ++a.three_bottom_x;
          ++a.four_bottom_x;
          ++a.four_bottom_x;
          create_shape(&a, TRUE);
        }
      }
      // break;

      if (control == KEY_DOWN) down(&a, &win);
    }
    if (timer % 10 == 0) down(&a, &win);
    refresh();
    usleep(50000);
    timer++;
  }
  endwin();
  return 0;
}

void down(WIN *a, WIN *win) {
  if (a->border.xx == 'a' && a->starty < (win->height - a->height) &&
      mvinch(a->one_bottom_y + 1, a->one_bottom_x) != '[' &&
      mvinch(a->two_bottom_y + 1, a->two_bottom_x) != '[' &&
      mvinch(a->three_bottom_y + 1, a->three_bottom_x) != '[' &&
      mvinch(a->four_bottom_y + 1, a->four_bottom_x) != '[') {
    create_shape(a, FALSE);
    ++a->starty;
    ++a->one_bottom_y;
    ++a->two_bottom_y;
    ++a->three_bottom_y;
    ++a->four_bottom_y;

    ++a->one_right_y;

    ++a->one_left_y;
    create_shape(a, TRUE);
  } else if (a->border.xx == 'b' && a->starty < (win->height - a->height) &&
             mvinch(a->one_bottom_y + 1, a->one_bottom_x) != '[') {
    create_shape(a, FALSE);
    ++a->starty;
    ++a->one_bottom_y;

    ++a->one_right_y;
    ++a->two_right_y;
    ++a->three_right_y;
    ++a->four_right_y;

    ++a->one_left_y;
    ++a->two_left_y;
    ++a->three_left_y;
    ++a->four_left_y;
    create_shape(a, TRUE);
  } else {
    spawn(a);
  }
}

void init_win_params(WIN *p_win) {
  p_win->height = 20;
  p_win->width = 21;
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

void start_x(WIN *a) {
  a->starty = 0 + 1;
  a->startx = ((COLS - a->width) / 2);  // вынести отдельно
}

void start_y(WIN *a) {
  a->starty = 0 + 1;
  a->startx = ((COLS - a->width) / 2) + 1;  // вынести отдельно
}

void init_shape_snake_x_params(WIN *a) {
  s21_create_matrix(1, 8, a->starty, a->startx, a);  // вынести из функции
  // s21_create_matrix(4, 8, 0, 0, a);
  a->border.xx = 'a';
  a->height = 1;
  a->width = 8;
}

void logic_x(WIN *a) {
  a->one_bottom_x = a->startx;
  a->one_bottom_y = a->starty;
  a->two_bottom_x = a->startx + 2;
  a->two_bottom_y = a->starty;
  a->three_bottom_x = a->startx + 4;
  a->three_bottom_y = a->starty;
  a->four_bottom_x = a->startx + 6;
  a->four_bottom_y = a->starty;

  a->one_left_x = a->startx;
  a->one_left_y = a->starty;

  a->one_right_x = a->startx + 6;
  a->one_right_y = a->starty;

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
  s21_create_matrix(4, 2, a->starty, a->startx, a);  // вынести из функции
  // s21_create_matrix(4, 8, 0, 0, a);

  a->border.xx = 'b';
  a->height = 4;
  a->width = 2;
}

void logic_y(WIN *a) {
  int current_x = a->startx;
  int current_y = a->starty;

  a->one_left_x = current_x;
  a->one_left_y = a->starty;
  a->two_left_x = current_x;
  a->two_left_y = a->starty + 1;
  a->three_left_x = current_x;
  a->three_left_y = a->starty + 2;
  a->four_left_x = current_x;
  a->four_left_y = a->starty + 3;

  a->one_right_x = current_x + 1;
  a->one_right_y = a->starty;
  a->two_right_x = current_x + 1;
  a->two_right_y = a->starty + 1;
  a->three_right_x = current_x + 1;
  a->three_right_y = a->starty + 2;
  a->four_right_x = current_x + 1;
  a->four_right_y = a->starty + 3;
  a->one_bottom_x = current_x;
  a->one_bottom_y = a->starty + 3;
  a->matrix[0][0] = '[';
  a->matrix[0][1] = ']';
  a->matrix[1][0] = '[';
  a->matrix[1][1] = ']';
  a->matrix[2][0] = '[';
  a->matrix[2][1] = ']';
  a->matrix[3][0] = '[';
  a->matrix[3][1] = ']';
  a->startx = current_x;
  a->starty = current_y;
}

// void rotate(WIN *a) {
//   if (a->border.xx == 'a') {
//     init_shape_snake_y_params(a);
//     update_update_coordinates(a);
//     logic_y(a);

//   } else {
//     init_shape_snake_x_params(a);
//     update_update_coordinates(a);
//     logic_x(a);
//   }
// }

void rotate(WIN *a) {
  int old_startx = a->startx;
  int old_starty = a->starty;

  // create_shape(a, FALSE);

  if (a->border.xx == 'a') {
    init_shape_snake_y_params(a);
    start_y(a);
    logic_y(a);
  } else {
    init_shape_snake_x_params(a);
    start_x(a);
    logic_x(a);
  }

  a->startx = old_startx;
  a->starty = old_starty;

  update_coordinates(a);

  if (check_collision(a)) {
    create_shape(a, FALSE);
    if (a->border.xx == 'a') {
      init_shape_snake_y_params(a);
      start_y(a);
      logic_y(a);
    } else {
      init_shape_snake_x_params(a);
      start_x(a);
      logic_x(a);
    }
    a->startx = old_startx;
    a->starty = old_starty;
    update_coordinates(a);
  }

  // create_shape(a, TRUE);
}

// void update_update_coordinates(WIN *a) {
//   if (a->border.xx == 'a') {
//     a->startx += 2;
//     a->starty += 2;
//   } else {
//     a->startx -= 2;
//     a->starty -= 2;
//   }
// }

void update_coordinates(WIN *a) {
  if (a->border.xx == 'a') {
    a->one_bottom_x = a->startx;
    a->one_bottom_y = a->starty;
    a->two_bottom_x = a->startx + 2;
    a->three_bottom_x = a->startx + 4;
    a->four_bottom_x = a->startx + 6;
  } else {
    a->one_left_x = a->startx;
    a->one_left_y = a->starty;
    a->two_left_y = a->starty + 1;
    a->three_left_y = a->starty + 2;
    a->four_left_y = a->starty + 3;
  }
}

bool check_collision(WIN *a) {
  for (int i = 0; i < a->height; i++) {
    for (int j = 0; j < a->width; j++) {
      int y = a->starty + i;
      int x = a->startx + j;
      chtype ch = mvinch(y, x);
      if (ch != ' ' && ch != a->matrix[i][j]) {
        return true;
      }
    }
  }
  return false;
}

void spawn(WIN *a) {
  char shape_list[7] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
  char chaa = shape_list[rand() % 8];
  switch (chaa) {
    case 'a':
      init_shape_snake_x_params(a);

      start_x(a);
      logic_x(a);
      break;
    case 'b':
      init_shape_snake_y_params(a);
      start_y(a);
      logic_y(a);
      break;
    case 'c':
      init_shape_snake_x_params(a);
      start_x(a);
      logic_x(a);
      break;
    case 'd':
      init_shape_snake_x_params(a);
      start_x(a);
      logic_x(a);
      break;
    case 'e':
      init_shape_snake_y_params(a);
      start_y(a);
      logic_y(a);
      break;
    case 'f':
      init_shape_snake_x_params(a);
      start_x(a);
      logic_x(a);
      break;
    case 'g':
      init_shape_snake_y_params(a);
      start_y(a);
      logic_y(a);
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