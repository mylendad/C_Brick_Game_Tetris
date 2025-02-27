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
  char color;
  char nextType;
  int cursorX, cursorY;
  int rotationPosition;
} Cursor;

static GameInfo_t gameInfo;
static bool gameOver = false;  // DEL!!!!!

void printShapes(Cursor *cursor);
void clearLines();
void mergecursor(Cursor *cursor);
int loadHighScore();
void generateNextPiece(Cursor *cursor);
void initializeGame(Cursor *cursor);
bool checkSide(int x, int y, int cursor[4][4]);
void createShape(Cursor *cursor);
void printField(Cursor *cursor);
void printTablo();
void printPause();
void printGameOver();

Cursor nextCursor;

int loadHighScore() {
  FILE *file = fopen("highscore.txt", "r");
  int highScore = 0;

  if (file) {
    fscanf(file, "%d", &highScore);
    fclose(file);
  } else {
    file = fopen("highscore.txt", "w");
    if (file) {
      fprintf(file, "%d", highScore);
      fclose(file);
    }
  }

  return highScore;
}

void mergecursor(Cursor *cursor) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j]) {
        gameInfo.field[cursor->cursorY + i][cursor->cursorX + j] =
            cursor->shape[i][j];
      }
    }
  }
}

void initializeGame(Cursor *cursor) {
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

  gameInfo.score = 0;
  // gameInfo.high_score = 0;
  gameInfo.high_score = loadHighScore();
  gameInfo.level = 1;
  gameInfo.speed = 500;
  gameInfo.pause = 0;

  srand(time(NULL));

  createShape(cursor);

  generateNextPiece(cursor);
}

GameInfo_t updateCurrentPosition(Cursor *cursor) {
  if (gameInfo.pause == 0 && !gameOver) {
    if (checkSide(cursor->cursorX, cursor->cursorY + 1, cursor->shape)) {
      mergecursor(cursor);
      clearLines();

      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
          cursor->shape[i][j] = gameInfo.next[i][j];
        }
      }
      cursor->type = cursor->nextType;
      generateNextPiece(cursor);

      cursor->cursorX = WIDTH / 2 - 2;
      cursor->cursorY = 0;

      if (checkSide(cursor->cursorX, cursor->cursorY, cursor->shape)) {
        gameOver = true;
      }
    } else {
      cursor->cursorY++;
    }
  }

  return gameInfo;
}

void printNextShape(Cursor *nextCursor) {
  mvprintw(7, WIDTH * 2 + 4, "Next Shape:");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (nextCursor->shape[i][j]) {
        attron(COLOR_PAIR(1));
        mvprintw(8 + i, WIDTH * 2 + 6 + j * 2, "[]");
        attroff(COLOR_PAIR(1));
      }
    }
  }
}

// void rendering(Cursor *cursor) {
//   clear();
//   printField(cursor);
//   printShapes(cursor);
//   printNextShape(&nextCursor);
//   printTablo();

//   if (gameInfo.pause != 0) {
//     printPause();
//   }

//   if (gameOver) {
//     printGameOver();
//   }

//   refresh();
// }

void rendering(Cursor *cursor) {
  clear();
  printField(cursor);
  printShapes(cursor);

  printTablo();

  mvprintw(7, WIDTH * 2 + 4, "Next:");  // выделить
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (gameInfo.next[i][j]) {
        attron(COLOR_PAIR(1));
        mvprintw(8 + i, WIDTH * 2 + 6 + j * 2, "[]");
        attroff(COLOR_PAIR(1));
      }
    }
  }

  if (gameInfo.pause != 0) {
    printPause();
  }

  if (gameOver) {
    printGameOver();
  }

  refresh();
}

// int loadHighScore() {
//   FILE *file = fopen("highscore.txt", "r");
//   int highScore = 0;
//   if (file) {
//     fscanf(file, "%d", &highScore);
//     fclose(file);
//   }
//   return highScore;
// }

void saveHighScore(int highScore) {
  FILE *file = fopen("highscore.txt", "w");
  if (file) {
    if (highScore > loadHighScore()) {
      fprintf(file, "%d", highScore);
      fclose(file);
    }
  }
}

// void clearLines() {
//   int linesCleared = 0;

//   for (int i = HEIGHT - 1; i >= 0; i--) {
//     bool full = true;
//     for (int j = 0; j < WIDTH; j++) {
//       if (!gameInfo.field[i][j]) {
//         full = false;
//         break;
//       }
//     }
//     if (full) {
//       for (int k = i; k > 0; k--) {
//         for (int j = 0; j < WIDTH; j++) {
//           gameInfo.field[k][j] = gameInfo.field[k - 1][j];
//         }
//       }
//       for (int j = 0; j < WIDTH; j++) {
//         gameInfo.field[0][j] = 0;
//       }
//       linesCleared++;
//       i++;
//     }
//   }

//   switch (linesCleared) {
//     case 1:
//       gameInfo.score += 100;
//       break;
//     case 2:
//       gameInfo.score += 300;
//       break;
//     case 3:
//       gameInfo.score += 700;
//       break;
//     case 4:
//       gameInfo.score += 1500;
//       break;
//   }

//   if (gameInfo.score > gameInfo.high_score) {
//     gameInfo.high_score = gameInfo.score;
//     saveHighScore(gameInfo.high_score);
//   }
// }

// void initializeGame(Cursor *cursor) {
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
//   gameInfo.speed = 500;
//   gameInfo.pause = 0;

//   srand(time(NULL));

//   createShape(cursor);

//   generateNextPiece(cursor);
// }

// void initializeGame(Cursor *ursor) {
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
//   gameInfo.high_score = loadHighScore();
//   gameInfo.level = 1;
//   gameInfo.speed = 500;
//   gameInfo.pause = 0;

//   srand(time(NULL));
// }

// void createShape(Cursor *cursor) {
//   int shapes[7][4][4] = {
//       {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
//       {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
//       {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
//       {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},  // S
//       {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // Z
//       {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}},  // L
//       {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}   // J
//   };

//   char types[] = {'I', 'O', 'T', 'S', 'Z', 'L', 'J'};
//   int shapeIndex = rand() % 7;

//   for (int i = 0; i < 4; i++) {
//     for (int j = 0; j < 4; j++) {
//       cursor->shape[i][j] = shapes[shapeIndex][i][j];
//     }
//   }
//   cursor->type = types[shapeIndex];
//   cursor->rotationPosition = 0;
// }

void createShape(Cursor *cursor) {
  int shapes[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},  // S
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // Z
      {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}},  // L
      {{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}}   // J
  };

  char types[] = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};
  // int color[] = {1, 2, 3, 4, 5, 6, 7};
  int shapeIndex = rand() % 7;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      cursor->shape[i][j] = shapes[shapeIndex][i][j];
    }
  }
  cursor->type = types[shapeIndex];
  // cursor->color = color[shapeIndex];
  cursor->rotationPosition = 0;
}

void generateNextPiece(Cursor *cursor) {
  int shapes[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},  // S
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // Z
      {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}},  // L
      {{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}}   // J
  };

  char types[] = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};
  int color[] = {1, 2, 3, 4, 5, 6, 7};
  int shapeIndex = rand() % 7;

  cursor->nextType = types[shapeIndex];
  cursor->color = color[shapeIndex];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      gameInfo.next[i][j] = shapes[shapeIndex][i][j];
    }
  }
}

void rotateShape(Cursor *cursor) {
  int temp[4][4];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp[i][j] = cursor->shape[i][j];
    }
  }

  if (cursor->rotationPosition == 0) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[j][3 - i];
      }
    }
    cursor->rotationPosition = 1;
  } else {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[3 - j][i];
      }
    }
    cursor->rotationPosition = 0;
  }

  if (checkSide(cursor->cursorX, cursor->cursorY, cursor->shape)) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[i][j];
      }
    }
  }
}

bool checkSide(int x, int y, int cursor[4][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor[i][j] && (x + j < 0 || x + j >= WIDTH || y + i >= HEIGHT ||
                           gameInfo.field[y + i][x + j])) {
        return true;
      }
    }
  }
  return false;
}

void clearLines() {
  int linesCleared = 0;

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
      linesCleared++;
      i++;
    }
  }

  switch (linesCleared) {
    case 1:
      gameInfo.score += 100;
      break;
    case 2:
      gameInfo.score += 300;
      break;
    case 3:
      gameInfo.score += 700;
      break;
    case 4:
      gameInfo.score += 1500;
      break;
  }

  if (gameInfo.score > gameInfo.high_score) {
    gameInfo.high_score = gameInfo.score;
    saveHighScore(gameInfo.high_score);
  }
}

// void mergecursor(Cursor *cursor) {
//   for (int i = 0; i < 4; i++) {
//     for (int j = 0; j < 4; j++) {
//       if (cursor->shape[i][j]) {
//         gameInfo.field[cursor->cursorY + i][cursor->cursorX + j] =
//             cursor->shape[i][j];
//       }
//     }
//   }
// }

// void clearLines() {
//   for (int i = HEIGHT - 1; i >= 0; i--) {
//     bool full = true;
//     for (int j = 0; j < WIDTH; j++) {
//       if (!gameInfo.field[i][j]) {
//         full = false;
//         break;
//       }
//     }
//     if (full) {
//       for (int k = i; k > 0; k--) {
//         for (int j = 0; j < WIDTH; j++) {
//           gameInfo.field[k][j] = gameInfo.field[k - 1][j];
//         }
//       }
//       for (int j = 0; j < WIDTH; j++) {
//         gameInfo.field[0][j] = 0;
//       }
//       gameInfo.score += 100;
//       if (gameInfo.score > gameInfo.high_score) {
//         gameInfo.high_score = gameInfo.score;
//       }
//     }
//   }
// }

void down(Cursor *cursor) {
  if (!checkSide(cursor->cursorX, cursor->cursorY + 1, cursor->shape)) {
    cursor->cursorY++;
  }
}

void userInput(Cursor *cursor, UserAction_t action, bool hold) {
  if (gameOver) return;

  switch (action) {
    case Start:
      if (gameOver) {
        initializeGame(cursor);
        gameOver = false;
      }
      break;
    case Pause:
      if (!gameInfo.pause)
        gameInfo.pause = 1;
      else
        gameInfo.pause = 0;
      break;
    case Terminate:
      gameOver = true;
      break;
    case Left:
      if (!checkSide(cursor->cursorX - 1, cursor->cursorY, cursor->shape)) {
        cursor->cursorX--;
      }
      break;
    case Right:
      if (!checkSide(cursor->cursorX + 1, cursor->cursorY, cursor->shape)) {
        cursor->cursorX++;
      }
      break;
    case Down:
      down(cursor);
      down(cursor);
      down(cursor);
      down(cursor);
      down(cursor);
      break;
    case Up:
      rotateShape(cursor);
      break;
  }
}

// GameInfo_t updateCurrentPosition(Cursor *cursor) {
//   if (gameInfo.pause == 0 && !gameOver) {
//     if (checkSide(cursor->cursorX, cursor->cursorY + 1, cursor->shape)) {
//       mergecursor(cursor);
//       clearLines();

//       for (int i = 0; i < 4; i++) {
//         for (int j = 0; j < 4; j++) {
//           cursor->shape[i][j] = gameInfo.next[i][j];
//         }
//       }
//       cursor->type = cursor->nextType;

//       generateNextPiece(cursor);

//       cursor->cursorX = WIDTH / 2 - 2;
//       cursor->cursorY = 0;

//       if (checkSide(cursor->cursorX, cursor->cursorY, cursor->shape)) {
//         gameOver = true;
//       }
//     } else {
//       cursor->cursorY++;
//     }
//   }

//   return gameInfo;
// }

void printField(Cursor *cursor) {
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
        attroff(COLOR_PAIR(1));  // цвет внизу
      }
    }
  }
}

void printShapes(Cursor *cursor) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j] && cursor->type == 'I') {
        attron(COLOR_PAIR(cursor->color));
        mvprintw(cursor->cursorY + i + 1, (cursor->cursorX + j) * 2 + 1, "[]");
        attron(COLOR_PAIR(cursor->color));
      }
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j] && cursor->type == 'T') {
        attron(COLOR_PAIR(cursor->color));
        mvprintw(cursor->cursorY + i + 1, (cursor->cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(cursor->color));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j] && cursor->type == 'Z') {
        attron(COLOR_PAIR(cursor->color));
        mvprintw(cursor->cursorY + i + 1, (cursor->cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(cursor->color));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j] && cursor->type == 'S') {
        attron(COLOR_PAIR(cursor->color));
        mvprintw(cursor->cursorY + i + 1, (cursor->cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(cursor->color));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j] && cursor->type == 'O') {
        attron(COLOR_PAIR(cursor->color));
        mvprintw(cursor->cursorY + i + 1, (cursor->cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(cursor->color));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j] && cursor->type == 'J') {
        attron(COLOR_PAIR(cursor->color));
        mvprintw(cursor->cursorY + i + 1, (cursor->cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(cursor->color));
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j] && cursor->type == 'L') {
        attron(COLOR_PAIR(cursor->color));
        mvprintw(cursor->cursorY + i + 1, (cursor->cursorX + j) * 2 + 1, "[]");
        attroff(COLOR_PAIR(cursor->color));
      }
    }
  }
}

void printNext() {
  mvprintw(7, WIDTH * 2 + 4, "Next:");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (gameInfo.next[i][j]) {
        attron(COLOR_PAIR(1));
        mvprintw(8 + i, WIDTH * 2 + 6 + j * 2, "[]");
        attroff(COLOR_PAIR(1));
      }
    }
  }
}

// void rendering(Cursor *cursor) {
//   clear();
//   printField(cursor);
//   printShapes(cursor);

//   printTablo();

//   printNext();

//   if (gameInfo.pause != 0) {
//     printPause();
//   }

//   if (gameOver) {
//     printGameOver();
//   }

//   refresh();
// }

// void printTablo() {
//   mvprintw(1, WIDTH * 2 + 4, "Score: %d", gameInfo.score);
//   mvprintw(3, WIDTH * 2 + 4, "High Score: %d", gameInfo.high_score);
//   mvprintw(5, WIDTH * 2 + 4, "Level: %d", gameInfo.level);
// }

void printTablo() {
  mvprintw(1, WIDTH * 2 + 4, "Score: %d", gameInfo.score);
  // mvprintw(3, WIDTH * 2 + 4, "High Score: %d", gameInfo.high_score);
  mvprintw(3, WIDTH * 2 + 4, "High Score: %d", gameInfo.high_score);
  mvprintw(5, WIDTH * 2 + 4, "Level: %d", gameInfo.level);
}

void printPause() { mvprintw(HEIGHT / 2, WIDTH / 2 + 4, "PAUSED"); }

void printGameOver() { mvprintw(HEIGHT / 2, WIDTH / 2 + 2, "GAME OVER"); }

int printMenu() {
  mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 10, "Press 'q' to quit");
  refresh();
  while (getch() != 'q');
}

// int main() {
//   Cursor cursor;
//   initscr();
//   cbreak();
//   noecho();
//   keypad(stdscr, TRUE);
//   curs_set(0);
//   timeout(gameInfo.speed);

//   start_color();
//   init_pair(1, COLOR_RED, COLOR_BLACK);
//   init_pair(2, COLOR_GREEN, COLOR_BLACK);
//   init_pair(3, COLOR_BLUE, COLOR_BLACK);
//   init_pair(4, COLOR_YELLOW, COLOR_BLACK);
//   init_pair(5, COLOR_CYAN, COLOR_BLACK);
//   init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
//   init_pair(7, COLOR_RED, COLOR_BLACK);

//   initializeGame(&cursor);
//   createShape(&cursor);
//   cursor.cursorX = WIDTH / 2 - 2;
//   cursor.cursorY = 0;

//   while (!gameOver) {
//     int ch = getch();
//     switch (ch) {
//       case 'q':
//         userInput(&cursor, Terminate, false);
//         break;
//       case 'p':
//         userInput(&cursor, Pause, false);
//         break;
//       case KEY_LEFT:
//         userInput(&cursor, Left, false);
//         break;
//       case KEY_RIGHT:
//         userInput(&cursor, Right, false);
//         break;
//       case KEY_DOWN:
//         userInput(&cursor, Down, false);
//         break;
//       case KEY_UP:  // поменять на Action
//         userInput(&cursor, Up, false);
//         break;
//       case ' ':
//         userInput(&cursor, Action, false);
//         break;
//     }

//     flushinp();  // buf clean

//     GameInfo_t info = updateCurrentPosition(&cursor);
//     rendering(&cursor);

//     if (gameOver) {
//       printMenu();
//     }

//     usleep(gameInfo.speed * 1000);
//   }

//   endwin();

//   return 0;
// }

int main() {
  Cursor cursor;
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

  initializeGame(&cursor);
  createShape(&cursor);
  cursor.cursorX = WIDTH / 2 - 2;
  cursor.cursorY = 0;

  while (!gameOver) {
    int ch = getch();
    switch (ch) {
      case 'q':
        userInput(&cursor, Terminate, false);
        break;
      case 'p':
        userInput(&cursor, Pause, false);
        break;
      case KEY_LEFT:
        userInput(&cursor, Left, false);
        break;
      case KEY_RIGHT:
        userInput(&cursor, Right, false);
        break;
      case KEY_DOWN:
        userInput(&cursor, Down, false);
        break;
      case KEY_UP:
        userInput(&cursor, Up, false);
        break;
      case ' ':
        userInput(&cursor, Action, false);
        break;
    }

    flushinp();

    GameInfo_t info = updateCurrentPosition(&cursor);
    rendering(&cursor);

    if (gameOver) {
      saveHighScore(gameInfo.high_score);
      printMenu();
    }

    usleep(gameInfo.speed * 1000);
  }

  endwin();

  return 0;
}
