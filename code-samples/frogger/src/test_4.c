#include <ncurses.h>
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
  int cursorX, cursorY;
  int rotationPosition;
} Cursor;

static GameInfo_t gameInfo;
static Cursor currentCursor;
static Cursor nextCursor;
static int cursorX, cursorY;
static bool gameOver = false;  // videli`tb
static bool paused = false;

void userInput(UserAction_t action, bool hold);

void createShape(Cursor *cursor) {
  int shapes[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},  // S
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // Z
      {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}},  // L
      {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}   // J
  };
}

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

  gameInfo.score = 0;
  gameInfo.high_score = 0;
  gameInfo.level = 1;
  gameInfo.speed = 500;
  gameInfo.pause = 0;

  srand(time(NULL));
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

void printShapes(Cursor *cursor) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j] && cursor->type == 'I') {
        attron(COLOR_PAIR(1));
        mvprintw(cursor->cursorY + i + 1, (cursor->cursorX + j) * 2 + 1, "[]");
        attron(COLOR_PAIR(1));
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
      break;
    case Action:
      break;
  }
}

void rendering(Cursor *cursor) {
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
  printShapes(cursor);
  refresh();
}

int main() {
  Cursor cursor;
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  initializeGame();
  createShape(&cursor);
  cursorX = WIDTH / 2 - 2;
  cursorY = 0;

  while (1) {
    int ch = getch();
    switch (ch) {
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

        break;
    }
    rendering(&cursor);
    usleep(5000);
  }

  endwin();

  return 0;
}
