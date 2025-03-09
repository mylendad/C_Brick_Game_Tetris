#include "frontend.h"

void initColor() {
  start_color();
  init_pair(RED, COLOR_RED, COLOR_BLACK);
  init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(FIOLET, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);
}

void printFrontend(GameInfo_t *gameInfo, Cursor_s *cursor) {
  clear();

  printTablo(gameInfo);
  printField(gameInfo);
  printShapes(cursor);
  printNext(gameInfo);
  if (gameInfo->pause != FALSE) {
    printPause();
  }
  if (cursor->game_over) {
    printGameOver();
  }

  refresh();
}

// void printField(GameInfo_t *gameInfo) {
//   for (int i = 0; i < HEIGHT + 2; i++) {
//     mvaddch(i, 0, ACS_VLINE);
//     mvaddch(i, CENTER_TOP - 1, ACS_VLINE);
//   }

//   for (int j = 0; j < CENTER_TOP; j++) {
//     mvaddch(0, j, ACS_HLINE);
//     mvaddch(HEIGHT + 1, j, ACS_HLINE);
//   }

//   mvaddch(0, 0, ACS_ULCORNER);
//   mvaddch(0, WIDTH * 2 + 1, ACS_URCORNER);
//   mvaddch(HEIGHT + 1, 0, ACS_LLCORNER);
//   mvaddch(HEIGHT + 1, CENTER_TOP - 1, ACS_LRCORNER);

//   for (int i = 0; i < HEIGHT; i++) {
//     for (int j = 0; j < WIDTH; j++) {
//       if (gameInfo->field[i][j]) {
//         attron(COLOR_PAIR(RED));
//         mvprintw(i + 1, j * 2 + 1, "[]");
//         attroff(COLOR_PAIR(RED));
//       }
//     }
//   }
// }

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
  int color = FALSE;
  switch (cursor->type) {
    case 'I':
      color = RED;
      break;
    case 'T':
      color = WHITE;
      break;
    case 'Z':
      color = BLUE;
      break;
    case 'S':
      color = GREEN;
      break;
    case 'O':
      color = CYAN;
      break;
    case 'J':
      color = FIOLET;
      break;
    case 'L':
      color = RED;
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
  attron(COLOR_PAIR(WHITE));
  mvprintw(7, WIDTH * 2 + 4, "Next:");
  attroff(COLOR_PAIR(WHITE));
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (gameInfo->next[i][j]) {
        attron(COLOR_PAIR(RED));
        mvprintw(8 + i, WIDTH * 2 + 6 + j * 2,
                 "[]");  // on the right under the scoreboard
        attroff(COLOR_PAIR(RED));
      }
    }
  }
}

void printTablo(GameInfo_t *gameInfo) {
  mvprintw(1, ON_THE_RIGHT, "Point: %d", gameInfo->score);
  mvprintw(3, ON_THE_RIGHT, "Max: %d", gameInfo->high_score);
  mvprintw(5, ON_THE_RIGHT, "Level: %d", gameInfo->level);
}

void printPause() { mvprintw(HEIGHT / 2, CENTER + 2, "PAUSED"); }

void printGameOver() { mvprintw(HEIGHT / 2, CENTER, "GAME OVER"); }
void printStart() {
  mvprintw(HEIGHT / 2, WIDTH / 2, "Press 'Enter'   |\n|      to start");
}
