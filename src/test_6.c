#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 10
#define HEIGHT 20

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef struct {
  int shape[4][4];
  char type;
  int rotationPosition;
} Cursor;

static GameInfo_t gameInfo;
static Cursor currentCursor;
static Cursor nextCursor;
static int cursorX, cursorY;
static bool gameOver = false;
static bool paused = false;

// void initializeGame() {
//   gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
//   for (int i = 0; i < HEIGHT; i++) {
//     gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
//     for (int j = 0; j < WIDTH; j++) {
//       gameInfo.field[i][j] = 0;
//     }
//   }

//   gameInfo.next = (int **)calloc(4, sizeof(int *));
//   for (int i = 0; i < 4; i++) {
//     gameInfo.next[i] = (int *)calloc(4, sizeof(int));
//     for (int j = 0; j < 4; j++) {
//       gameInfo.next[i][j] = 0;
//     }
//   }

//   gameInfo.score = 0;
//   gameInfo.high_score = 0;
//   gameInfo.level = 1;
//   gameInfo.speed = 500;  // ??
//   gameInfo.pause = 0;

//   srand(time(NULL));
// }

void initializeGame() {
  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }

  gameInfo.next = (int **)calloc(4, sizeof(int *));
  for (int i = 0; i < 4; i++) {
    gameInfo.next[i] = (int *)calloc(4, sizeof(int));
    for (int j = 0; j < 4; j++) {
      gameInfo.next[i][j] = 0;
    }
  }
  // bool gameOver = false;
  gameInfo.score = 0;
  gameInfo.high_score = 0;
  gameInfo.level = 1;
  gameInfo.speed = 500;
  gameInfo.pause = 0;

  srand(time(NULL));

  // Генерация первой текущей фигуры
  generatePiece(&currentCursor);

  // Генерация первой следующей фигуры и запись в **field
  generateNextPiece();
}

void generatePiece(Cursor *piece) {
  int shapes[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},  // S
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // Z
      {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}},  // L
      {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}   // J
  };

  char types[] = {'I', 'O', 'T', 'S', 'Z', 'L', 'J'};
  int shapeIndex = rand() % 7;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      piece->shape[i][j] = shapes[shapeIndex][i][j];
    }
  }
  piece->type = types[shapeIndex];
  piece->rotationPosition = 0;  // Начальное состояние вращения
}

void generateNextPiece() {
  int shapes[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},  // S
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // Z
      {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}},  // L
      {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}   // J
  };

  int shapeIndex = rand() % 7;

  // Записываем следующую фигуру в **field (последние 4 строки)
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      gameInfo.field[HEIGHT - 4 + i][j] = shapes[shapeIndex][i][j];
    }
  }
}

bool checkSide(int x, int y, int piece[4][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (piece[i][j] && (x + j < 0 || x + j >= WIDTH || y + i >= HEIGHT ||
                          gameInfo.field[y + i][x + j])) {
        return true;
      }
    }
  }
  return false;
}

void rotateShape(Cursor *piece) {
  int temp[4][4];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp[i][j] = piece->shape[i][j];
    }
  }
  if (piece->type == 'I') {
    if (piece->rotationPosition == 0) {
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          piece->shape[i][j] = temp[j][3 - i];
        }
      }
      piece->rotationPosition = 1;
    } else {
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          piece->shape[i][j] = temp[3 - j][i];
        }
      }
      piece->rotationPosition = 0;
    }
  } else if (piece->type == 'S' || piece->type == 'Z' || piece->type == 'I') {
    if (piece->rotationPosition == 0) {  // vertical
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          piece->shape[i][j] = temp[j][3 - i];
        }
      }
      piece->rotationPosition = 1;
    } else {
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          piece->shape[i][j] = temp[3 - j][i];  // horisont
        }
      }
      piece->rotationPosition = 0;
    }
  } else if (piece->type == 'T' || piece->type == 'L' || piece->type == 'J') {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        piece->shape[i][j] = temp[3 - j][i];
      }
    }
  }
  if (checkSide(cursorX, cursorY, piece->shape)) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        piece->shape[i][j] = temp[i][j];
      }
    }
  }
}

void mergePiece() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentCursor.shape[i][j]) {
        gameInfo.field[cursorY + i][cursorX + j] = currentCursor.shape[i][j];
      }
    }
  }
}

void clearLines() {
  for (int i = HEIGHT - 1; i >= 0; i--) {
    bool full = true;
    for (int j = 0; j < WIDTH; j++) {
      if (!gameInfo.field[i][j]) {
        full = false;
        break;
      }
    }
    if (full) {
      for (int k = i; k > 0; k--) {
        for (int j = 0; j < WIDTH; j++) {
          gameInfo.field[k][j] = gameInfo.field[k - 1][j];
        }
      }
      for (int j = 0; j < WIDTH; j++) {
        gameInfo.field[0][j] = 0;
      }
      gameInfo.score += 100;
      if (gameInfo.score > gameInfo.high_score) {
        gameInfo.high_score = gameInfo.score;
      }
    }
  }
}

void userInput(UserAction_t action, bool hold) {
  if (gameOver) return;

  switch (action) {
    case Start:
      if (gameOver) {
        initializeGame();
        gameOver = false;
      }
      break;
    case Pause:
      paused = !paused;
      break;
    case Terminate:
      gameOver = true;
      break;
    case Left:
      if (!checkSide(cursorX - 1, cursorY, currentCursor.shape)) {
        cursorX--;
      }
      break;
    case Right:
      if (!checkSide(cursorX + 1, cursorY, currentCursor.shape)) {
        cursorX++;
      }
      break;
    case Down:
      if (!checkSide(cursorX, cursorY + 1, currentCursor.shape)) {
        cursorY++;
      }
      break;
    case Up:
      rotateShape(&currentCursor);
      break;
    case Action:
      // Fast fall
      break;
  }
}

// GameInfo_t updateCurrentPosition() {
//   if (!paused && !gameOver) {
//     if (checkSide(cursorX, cursorY + 1, currentCursor.shape)) {
//       mergePiece();
//       clearLines();
//       generatePiece(&currentCursor);
//       cursorX = WIDTH / 2 - 2;
//       cursorY = 0;
//       if (checkSide(cursorX, cursorY, currentCursor.shape)) {
//         gameOver = true;
//       }
//     } else {
//       cursorY++;
//     }
//   }

//   return gameInfo;
// }

GameInfo_t updateCurrentPosition() {
  if (!paused && !gameOver) {
    if (checkSide(cursorX, cursorY + 1, currentCursor.shape)) {
      mergePiece();
      clearLines();

      // Текущая фигура берется из **field (последние 4 строки)
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          currentCursor.shape[i][j] = gameInfo.field[HEIGHT - 4 + i][j];
        }
      }

      // Генерация новой следующей фигуры
      generateNextPiece();

      cursorX = WIDTH / 2 - 2;
      cursorY = 0;

      if (checkSide(cursorX, cursorY, currentCursor.shape)) {
        gameOver = true;
      }
    } else {
      cursorY++;
    }
  }

  return gameInfo;
}

void drawGame() {
  clear();

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
      if (gameInfo.field[i][j]) {
        attron(COLOR_PAIR(1));
        mvprintw(i + 1, j * 2 + 1, "[]");
        attroff(COLOR_PAIR(1));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentCursor.shape[i][j] && currentCursor.type == 'I') {
        attron(COLOR_PAIR(1));
        mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
        attron(COLOR_PAIR(1));
      }
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentCursor.shape[i][j] && currentCursor.type == 'T') {
        attron(COLOR_PAIR(2));
        mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(2));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentCursor.shape[i][j] && currentCursor.type == 'Z') {
        attron(COLOR_PAIR(2));
        mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(2));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentCursor.shape[i][j] && currentCursor.type == 'S') {
        attron(COLOR_PAIR(4));
        mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(4));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentCursor.shape[i][j] && currentCursor.type == 'O') {
        attron(COLOR_PAIR(5));
        mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(5));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentCursor.shape[i][j] && currentCursor.type == 'J') {
        attron(COLOR_PAIR(6));
        mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(6));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentCursor.shape[i][j] && currentCursor.type == 'L') {
        attron(COLOR_PAIR(7));
        mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(7));
      }
    }
  }

  mvprintw(1, WIDTH * 2 + 4, "Score: %d", gameInfo.score);
  mvprintw(3, WIDTH * 2 + 4, "High Score: %d", gameInfo.high_score);
  mvprintw(5, WIDTH * 2 + 4, "Level: %d", gameInfo.level);

  if (paused) {
    mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "PAUSED");
  }

  if (gameOver) {
    mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "GAME OVER");
  }

  refresh();
}

// void drawGame() {
//   clear();

//   for (int i = 0; i < HEIGHT + 2; i++) {
//     mvaddch(i, 0, ACS_VLINE);
//     mvaddch(i, WIDTH * 2 + 1, ACS_VLINE);
//   }

//   for (int j = 0; j < WIDTH * 2 + 2; j++) {
//     mvaddch(0, j, ACS_HLINE);
//     mvaddch(HEIGHT + 1, j, ACS_HLINE);
//   }

//   mvaddch(0, 0, ACS_ULCORNER);
//   mvaddch(0, WIDTH * 2 + 1, ACS_URCORNER);
//   mvaddch(HEIGHT + 1, 0, ACS_LLCORNER);
//   mvaddch(HEIGHT + 1, WIDTH * 2 + 1, ACS_LRCORNER);

//   for (int i = 0; i < HEIGHT; i++) {
//     for (int j = 0; j < WIDTH; j++) {
//       if (gameInfo.field[i][j]) {
//         attron(COLOR_PAIR(1));
//         mvprintw(i + 1, j * 2 + 1, "[]");
//         attroff(COLOR_PAIR(1));
//       }
//     }
//   }

//   for (int i = 0; i < 4; i++) {
//     for (int j = 0; j < 4; j++) {
//       if (currentCursor.shape[i][j] && currentCursor.type == 'I') {
//         attron(COLOR_PAIR(1));
//         mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
//         attron(COLOR_PAIR(1));
//       }
//     }
//   }
//   for (int i = 0; i < 4; i++) {
//     for (int j = 0; j < 4; j++) {
//       if (currentCursor.shape[i][j] && currentCursor.type == 'T') {
//         attron(COLOR_PAIR(2));
//         mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
//         attroff(COLOR_PAIR(2));
//       }
//     }
//   }

//   for (int i = 0; i < 4; i++) {
//     for (int j = 0; j < 4; j++) {
//       if (currentCursor.shape[i][j] && currentCursor.type == 'Z') {
//         attron(COLOR_PAIR(2));
//         mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
//         attroff(COLOR_PAIR(2));
//       }
//     }
//   }

//   for (int i = 0; i < 4; i++) {
//     for (int j = 0; j < 4; j++) {
//       if (currentCursor.shape[i][j] && currentCursor.type == 'S') {
//         attron(COLOR_PAIR(4));
//         mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
//         attroff(COLOR_PAIR(4));
//       }
//     }
//   }

//   for (int i = 0; i < 4; i++) {
//     for (int j = 0; j < 4; j++) {
//       if (currentCursor.shape[i][j] && currentCursor.type == 'O') {
//         attron(COLOR_PAIR(5));
//         mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
//         attroff(COLOR_PAIR(5));
//       }
//     }
//   }

//   for (int i = 0; i < 4; i++) {
//     for (int j = 0; j < 4; j++) {
//       if (currentCursor.shape[i][j] && currentCursor.type == 'J') {
//         attron(COLOR_PAIR(6));
//         mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
//         attroff(COLOR_PAIR(6));
//       }
//     }
//   }

//   for (int i = 0; i < 4; i++) {
//     for (int j = 0; j < 4; j++) {
//       if (currentCursor.shape[i][j] && currentCursor.type == 'L') {
//         attron(COLOR_PAIR(7));
//         mvprintw(cursorY + i + 1, (cursorX + j) * 2 + 1, "[]");
//         attroff(COLOR_PAIR(7));
//       }
//     }
//   }

//   mvprintw(1, WIDTH * 2 + 4, "Score: %d", gameInfo.score);
//   mvprintw(3, WIDTH * 2 + 4, "High Score: %d", gameInfo.high_score);
//   mvprintw(5, WIDTH * 2 + 4, "Level: %d", gameInfo.level);

//   if (paused) {
//     mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "PAUSED");
//   }

//   if (gameOver) {
//     mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "GAME OVER");
//   }

//   refresh();
// }

int main() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(gameInfo.speed);

  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_YELLOW, COLOR_BLACK);
  init_pair(5, COLOR_CYAN, COLOR_BLACK);
  init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(7, COLOR_RED, COLOR_BLACK);

  initializeGame();
  bool gameOver = false;
  generatePiece(&currentCursor);
  cursorX = WIDTH / 2 - 2;
  cursorY = 0;

  while (!gameOver) {
    int ch = getch();
    switch (ch) {
      case 'q':
        userInput(Terminate, false);
        break;
      case 'p':
        userInput(Pause, false);
        break;
      case KEY_LEFT:
        userInput(Left, false);
        break;
      case KEY_RIGHT:
        userInput(Right, false);
        break;
      case KEY_DOWN:
        userInput(Down, false);
        break;
      case KEY_UP:
        userInput(Up, false);
        break;
      case ' ':
        userInput(Action, false);
        break;
    }

    flushinp();  // buf clean

    GameInfo_t info = updateCurrentPosition();
    drawGame();

    if (gameOver) {
      mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 10, "Press 'q' to quit");
      refresh();
      while (getch() != 'q');
    }

    usleep(gameInfo.speed * 1000);
  }

  endwin();

  return 0;
}
