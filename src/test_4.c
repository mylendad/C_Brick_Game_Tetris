#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 10
#define HEIGHT 20

#define MAX_LEVEL 10
#define SCORE_UP_LEVEL 600
#define START_SPEED 500
#define SPEED_STEP 50
#define MIN_SPEED 100

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

typedef enum State { START, SPAWN, MOVING, SHIFT, CONNECTION, GAME_OVER } FSM;

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
  char nextType;
  int cursorX, cursorY;
  int rotationPosition;
  int quit;
  bool start;
  bool gameOver;
} Cursor;

// static GameInfo_t gameInfo;
void down(GameInfo_t *gameInfo, Cursor *cursor);
void userInput(GameInfo_t *gameInfo, Cursor *cursor, UserAction_t action,
               bool hold);
void spawn(GameInfo_t *gameInfo, Cursor *cursor);
void controller(GameInfo_t *gameInfo, Cursor *cursor, int ch);
struct timespec timerFunc();
void game(GameInfo_t *gameInfo, Cursor *cursor, struct timespec *timerFunc);
void sziShapeRotate(GameInfo_t *gameInfo, Cursor *cursor);
void jltShapeRotate(GameInfo_t *gameInfo, Cursor *cursor);
void moving(Cursor *cursor);
void printStart();
void waitStart(Cursor *cursor);
void waitQuit(Cursor *cursor);
void printShapes(GameInfo_t *gameInfo, Cursor *cursor);
void clearLines();
void mergeCursor(GameInfo_t *gameInfo, Cursor *cursor);
int loadHighScore();
void generateNextShape(GameInfo_t *gameInfo, Cursor *cursor);
void initializeGame(GameInfo_t *gameInfo, Cursor *cursor);
bool checkSide(GameInfo_t *gameInfo, int x, int y, int cursor[4][4]);
void createShape(GameInfo_t *gameInfo, Cursor *cursor, int flag);
void printField(GameInfo_t *gameInfo, Cursor *cursor);
void printTablo();
void printPause();
void printGameOver();

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

void mergeCursor(GameInfo_t *gameInfo, Cursor *cursor) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j]) {
        gameInfo->field[cursor->cursorY + i][cursor->cursorX + j] =
            cursor->shape[i][j];
      }
    }
  }
}

void initializeGame(GameInfo_t *gameInfo, Cursor *cursor) {
  int createNew = 0;
  int createNext = 1;
  gameInfo->field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo->field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo->field[i][j] = 0;
    }
  }

  gameInfo->next = (int **)calloc(4, sizeof(int *));
  for (int i = 0; i < 4; i++) {
    gameInfo->next[i] = (int *)calloc(4, sizeof(int));
    for (int j = 0; j < 4; j++) {
      gameInfo->next[i][j] = 0;
    }
  }

  gameInfo->score = 0;
  gameInfo->high_score = loadHighScore();
  gameInfo->level = 1;
  gameInfo->speed = START_SPEED;
  cursor->gameOver = false;
  gameInfo->pause = 0;

  srand(time(NULL));

  createShape(gameInfo, cursor, createNew);

  createShape(gameInfo, cursor, createNext);

  createShape(gameInfo, cursor, createNew);
  cursor->cursorX = WIDTH / 2 - 2;
  cursor->cursorY = -2;
}

void printNextShape(GameInfo_t *gameInfo, Cursor *cursor) {
  mvprintw(7, WIDTH * 2 + 4, "Next:");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (gameInfo->next[i][j]) {
        attron(COLOR_PAIR(8));
        mvprintw(8 + i, WIDTH * 2 + 6 + j * 2, "[]");
        attroff(COLOR_PAIR(8));
      }
    }
  }
}

void rendering(GameInfo_t *gameInfo, Cursor *cursor) {
  clear();

  printTablo(gameInfo);

  printField(gameInfo, cursor);

  printShapes(gameInfo, cursor);
  printNextShape(gameInfo, cursor);
  // printTablo();

  // printNextShape(cursor);

  if (gameInfo->pause != 0) {
    printPause();
  }

  if (cursor->gameOver) {
    printGameOver();
  }

  refresh();
}

void moving(Cursor *cursor) { cursor->cursorY++; }

void saveHighScore(int highScore) {
  FILE *file = fopen("highscore.txt", "w");
  if (file) {
    if (highScore > loadHighScore()) {
      fprintf(file, "%d", highScore);
      fclose(file);
    }
  }
}

void createShape(GameInfo_t *gameInfo, Cursor *cursor, int flag) {
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
  int shapeIndex = rand() % 7;

  if (flag == 0) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = shapes[shapeIndex][i][j];
      }
    }
    cursor->type = types[shapeIndex];
    cursor->rotationPosition = 0;
  } else if (flag == 1) {
    cursor->nextType = types[shapeIndex];
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        gameInfo->next[i][j] = shapes[shapeIndex][i][j];
      }
    }
  }
}

void jltShapeRotate(GameInfo_t *gameInfo, Cursor *cursor) {
  int temp[4][4];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp[i][j] = cursor->shape[i][j];
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int x = j - 2;
      int y = i - 1;
      int newX = -y;
      int newY = x;
      cursor->shape[newY + 1][newX + 2] = temp[i][j];
    }
  }
  if (checkSide(gameInfo, cursor->cursorX, cursor->cursorY, cursor->shape)) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[i][j];
      }
    }
  }
}

void sziShapeRotate(GameInfo_t *gameInfo, Cursor *cursor) {
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
  if (checkSide(gameInfo, cursor->cursorX, cursor->cursorY, cursor->shape)) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[i][j];
      }
    }
  }
}

void rotateShape(GameInfo_t *gameInfo, Cursor *cursor) {
  switch (cursor->type) {
    case 'I':
      sziShapeRotate(gameInfo, cursor);
      break;

    case 'J':
      jltShapeRotate(gameInfo, cursor);
      break;

    case 'L':
      jltShapeRotate(gameInfo, cursor);

      break;
    case 'O':
      break;

    case 'S':
      sziShapeRotate(gameInfo, cursor);
      break;

    case 'Z':
      sziShapeRotate(gameInfo, cursor);
      break;

    case 'T':
      jltShapeRotate(gameInfo, cursor);
      break;
  }
}

bool checkSide(GameInfo_t *gameInfo, int x, int y, int cursor[4][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor[i][j] && (x + j < 0 || x + j >= WIDTH || y + i >= HEIGHT ||
                           gameInfo->field[y + i][x + j])) {
        return true;
      }
    }
  }
  return false;
}

void clearLines(GameInfo_t *gameInfo) {
  int linesCleared = 0;

  for (int i = HEIGHT - 1; i >= 0; i--) {
    bool full = true;
    for (int j = 0; j < WIDTH; j++) {
      if (!gameInfo->field[i][j]) {
        full = false;
        break;
      }
    }
    if (full) {
      for (int k = i; k > 0; k--) {
        for (int j = 0; j < WIDTH; j++) {
          gameInfo->field[k][j] = gameInfo->field[k - 1][j];
        }
      }
      for (int j = 0; j < WIDTH; j++) {
        gameInfo->field[0][j] = 0;
      }
      linesCleared++;
      i++;
    }
  }

  switch (linesCleared) {
    case 1:
      gameInfo->score += 100;
      break;
    case 2:
      gameInfo->score += 300;
      break;
    case 3:
      gameInfo->score += 700;
      break;
    case 4:
      gameInfo->score += 1500;
      break;
  }

  int new_level = gameInfo->score / SCORE_UP_LEVEL + 1;
  if (new_level > gameInfo->level && new_level <= MAX_LEVEL) {
    gameInfo->level = new_level;
    gameInfo->speed = START_SPEED - (gameInfo->level - 1) * SPEED_STEP;
    if (gameInfo->speed < MIN_SPEED) gameInfo->speed = MIN_SPEED;
  }

  if (gameInfo->score > gameInfo->high_score) {
    gameInfo->high_score = gameInfo->score;
    saveHighScore(gameInfo->high_score);
  }
}

void down(GameInfo_t *gameInfo, Cursor *cursor) {
  if (!checkSide(gameInfo, cursor->cursorX, cursor->cursorY + 1,
                 cursor->shape)) {
    cursor->cursorY++;
  }
}

void userInput(GameInfo_t *gameInfo, Cursor *cursor, UserAction_t action,
               bool hold) {
  // if (cursor->gameOver) return;
  switch (action) {
    case Down:
      if (gameInfo->pause == 0) {
        down(gameInfo, cursor);
      }
      break;
    case Start:
      // if (cursor->gameOver) {
      // initializeGame(cursor);
      // cursor->gameOver = false;
      // cursor->start = true;
      initializeGame(gameInfo, cursor);
      struct timespec lastTime = timerFunc(gameInfo);

      game(gameInfo, cursor, &lastTime);
      // }
      break;
    case Pause:
      if (!gameInfo->pause)
        gameInfo->pause = 1;
      else
        gameInfo->pause = 0;
      break;
    case Terminate:
      if (!cursor->gameOver) {
        cursor->gameOver = true;
        // cursor->start = false;
        // endwin();
      } else
        cursor->quit = 1;
      // cursor->gameOver = false;
      break;
    case Left:
      if (!checkSide(gameInfo, cursor->cursorX - 1, cursor->cursorY,
                     cursor->shape) &&
          gameInfo->pause == 0) {
        cursor->cursorX--;
      }
      break;
    case Right:
      if (!checkSide(gameInfo, cursor->cursorX + 1, cursor->cursorY,
                     cursor->shape) &&
          gameInfo->pause == 0) {
        cursor->cursorX++;
      }
      break;
    case Action:
      if (gameInfo->pause == 0) {
        int dropDistance = 0;

        while (!checkSide(gameInfo, cursor->cursorX, cursor->cursorY + 1,
                          cursor->shape)) {
          cursor->cursorY++;
          dropDistance++;
        }
      }
      break;
    case Up:
      if (gameInfo->pause == 0) {
        rotateShape(gameInfo, cursor);
      }
      break;
  }
}

void printField(GameInfo_t *gameInfo, Cursor *cursor) {
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

void printShapes(GameInfo_t *gameInfo, Cursor *cursor) {
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
        mvprintw(cursor->cursorY + i + 1, (cursor->cursorX + j) * 2 + 1, "[]");
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
  // mvprintw(7, WIDTH * 2 + 4, "Speed: %d ms", gameInfo.speed);
}

void printPause() { mvprintw(HEIGHT / 2, WIDTH / 2 + 4, "PAUSED"); }

void printGameOver() { mvprintw(HEIGHT / 2, WIDTH / 2 + 2, "GAME OVER"); }
void printStart() {
  mvprintw(HEIGHT / 2, WIDTH / 2, "Press 'Enter'   |\n|      to start");
}

struct timespec timerFunc(GameInfo_t *gameInfo) {
  struct timespec lastMoveDown = {0};
  struct timespec lastMoveSide = {0};
  int downMoveInterval = gameInfo->speed * 10;
  int sideMoveInterval = gameInfo->speed * 10;

  struct timespec lastTime;

  clock_gettime(CLOCK_MONOTONIC, &lastTime);
  return lastTime;
}

void controller(GameInfo_t *gameInfo, Cursor *cursor, int ch) {
  switch (ch) {  // moving
    case KEY_LEFT:
      userInput(gameInfo, cursor, Left, false);
      break;
    case KEY_RIGHT:
      userInput(gameInfo, cursor, Right, false);
      break;
    case KEY_DOWN:
      userInput(gameInfo, cursor, Down, false);
      break;
    case KEY_UP:
      userInput(gameInfo, cursor, Up, false);
      break;
    case ' ':
      userInput(gameInfo, cursor, Action, false);
      break;
    case 'q':
      userInput(gameInfo, cursor, Terminate, false);
      break;
    case '\n':
      userInput(gameInfo, cursor, Start, false);
      break;
    case 'p':
      userInput(gameInfo, cursor, Pause, false);
      break;
    default:
      break;
  }
}

void spawn(GameInfo_t *gameInfo, Cursor *cursor) {
  char createNext = 1;
  for (int i = 0; i < 4; i++) {  // spawn?
    for (int j = 0; j < 4; j++) {
      cursor->shape[i][j] = gameInfo->next[i][j];
    }
  }
  cursor->type = cursor->nextType;

  // createShape(cursor, createNext);
  createShape(gameInfo, cursor, createNext);

  cursor->cursorX = WIDTH / 2 - 2;
  cursor->cursorY = 0;

  if (checkSide(gameInfo, cursor->cursorX, cursor->cursorY,
                cursor->shape)) {  // gameover
    cursor->gameOver = true;
  }
}

void game(GameInfo_t *gameInfo, Cursor *cursor, struct timespec *timerFunc) {
  // if (!cursor->start) {
  printField(gameInfo, cursor);
  waitStart(cursor);  // start
  //   // waitQuit(cursor);   // game over
  //   usleep(100000);
  // }
  cursor->quit = 0;
  while (!cursor->quit) {  // moving
    struct timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);

    long elapsed = (currentTime.tv_sec - timerFunc->tv_sec) * 1000 +
                   (currentTime.tv_nsec - timerFunc->tv_nsec) / 1000000;

    int ch = getch();
    controller(gameInfo, cursor, ch);

    flushinp();  //

    if (elapsed >= gameInfo->speed) {
      // updateCurrentPosition(cursor);  // движение тут

      if (gameInfo->pause == 0 && !cursor->gameOver) {
        if (!checkSide(gameInfo, cursor->cursorX, cursor->cursorY + 1,
                       cursor->shape)) {
          moving(cursor);  // shift
        } else {
          mergeCursor(gameInfo, cursor);  // connection
          clearLines(gameInfo);

          spawn(gameInfo, cursor);
        }
      }

      *timerFunc = currentTime;
    }

    rendering(gameInfo, cursor);
    waitQuit(cursor);  // game over
    usleep(100000);
  }
}

void waitQuit(Cursor *cursor) {
  if (cursor->gameOver) {
    refresh();
    while (getch() != 'q');
  }
}

void waitStart(Cursor *cursor) {
  if (!cursor->start) {
    // printStart();
    refresh();
    while (getch() != '\n') printStart();
    // ;
  }
}

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

void startGame() {}

int main() {
  Cursor cursor;
  static GameInfo_t gameInfo;
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(0);

  initColor();

  initializeGame(&gameInfo, &cursor);
  struct timespec lastTime = timerFunc(&gameInfo);

  game(&gameInfo, &cursor, &lastTime);

  usleep(1000);

  endwin();

  return 0;
}
