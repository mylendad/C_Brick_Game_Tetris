#include "../../game.h"

void initColor() {
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_YELLOW, COLOR_BLACK);
  init_pair(5, COLOR_CYAN, COLOR_BLACK);
  init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(7, COLOR_RED, COLOR_BLACK);
  init_pair(8, COLOR_WHITE, COLOR_BLACK);
}

void printFrontend(GameInfo_t *gameInfo, Cursor_s *cursor) {
  clear();

  printTablo(gameInfo);
  printField(gameInfo);
  printShapes(cursor);
  printNext(gameInfo);
  if (gameInfo->pause != 0) {
    printPause();
  }
  if (cursor->game_over) {
    printGameOver();
  }

  refresh();
}

void printField(GameInfo_t *gameInfo) {
  for (int i = 0; i < HEIGHT + 2; i++) {
    mvaddch(i, 0, ACS_VLINE);
    mvaddch(i, WIDTH * 2 + 1, ACS_VLINE);
  }

  for (int j = 0; j < WIDTH * 2 + 2; j++) {
    mvaddch(0, j, ACS_HLINE);
    mvaddch(HEIGHT + 1, j, ACS_HLINE);
  }

  mvaddch(0, 0, ACS_ULCORNER);
  mvaddch(0, WIDTH * 2 + 1, ACS_URCORNER);
  mvaddch(HEIGHT + 1, 0, ACS_LLCORNER);
  mvaddch(HEIGHT + 1, WIDTH * 2 + 1, ACS_LRCORNER);

  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (gameInfo->field[i][j]) {
        attron(COLOR_PAIR(1));
        mvprintw(i + 1, j * 2 + 1, "[]");
        attroff(COLOR_PAIR(1));
      }
    }
  }
}

void printShapes(Cursor_s *cursor) {
  int color = 0;
  switch (cursor->type) {
    case 'I':
      color = 8;
      break;
    case 'T':
      color = 2;
      break;
    case 'Z':
      color = 3;
      break;
    case 'S':
      color = 4;
      break;
    case 'O':
      color = 5;
      break;
    case 'J':
      color = 6;
      break;
    case 'L':
      color = 7;
      break;
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j]) {
        attron(COLOR_PAIR(color));
        mvprintw(cursor->cursor_y + i + 1, (cursor->cursor_x + j) * 2 + 1,
                 "[]");
        attron(COLOR_PAIR(color));
      }
    }
  }
}

void printNext(GameInfo_t *gameInfo) {
  attron(COLOR_PAIR(8));
  mvprintw(7, WIDTH * 2 + 4, "Next:");
  attroff(COLOR_PAIR(8));
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (gameInfo->next[i][j]) {
        attron(COLOR_PAIR(1));
        mvprintw(8 + i, WIDTH * 2 + 6 + j * 2, "[]");
        attroff(COLOR_PAIR(1));
      }
    }
  }
}

void printTablo(GameInfo_t *gameInfo) {
  mvprintw(1, WIDTH * 2 + 4, "Point: %d", gameInfo->score);
  mvprintw(3, WIDTH * 2 + 4, "Max: %d", gameInfo->high_score);
  mvprintw(5, WIDTH * 2 + 4, "Level: %d", gameInfo->level);
}

void printPause() { mvprintw(HEIGHT / 2, WIDTH / 2 + 4, "PAUSED"); }

void printGameOver() { mvprintw(HEIGHT / 2, WIDTH / 2 + 2, "GAME OVER"); }
void printStart() {
  mvprintw(HEIGHT / 2, WIDTH / 2, "Press 'Enter'   |\n|      to start");
}
