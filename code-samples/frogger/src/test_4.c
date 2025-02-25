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
  int x, y;
} Cursor;

static GameInfo_t gameInfo;
static Cursor current;
static bool gameOver = false;
static bool paused = false;

void createShape(Cursor *cursor) {
  int shapes[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},  // S
      {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // Z
      {{0, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},  // L
      {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}   // J
  };

  int idx = rand() % 7;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) cursor->shape[i][j] = shapes[idx][i][j];
}

void initializeGame() {
  gameInfo.field = (int **)malloc(HEIGHT * sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)malloc(WIDTH * sizeof(int));
    for (int j = 0; j < WIDTH; j++) gameInfo.field[i][j] = 0;
  }

  gameInfo.speed = 500;
  srand(time(NULL));
  createShape(&current);
  current.x = WIDTH / 2 - 2;
  current.y = 0;
}

bool checkCollision(int dx, int dy) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (current.shape[i][j]) {
        int newX = current.x + j + dx;
        int newY = current.y + i + dy;
        if (newX < 0 || newY >= HEIGHT) return true;
        if (newY >= 0 && gameInfo.field[newY][newX]) return true;
      }
    }
  }
  return false;
}

void drawField() {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (gameInfo.field[i][j]) {
        mvprintw(i + 1, j * 2 + 1, "[]");
      }
    }
  }
}

void drawCurrent() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (current.shape[i][j]) {
        mvprintw(current.y + i + 1, (current.x + j) * 2 + 1, "[]");
      }
    }
  }
}

void userInput(UserAction_t action) {
  if (gameOver) return;

  switch (action) {
    case Left:
      if (!checkCollision(-1, 0)) current.x--;
      break;
    case Right:
      if (!checkCollision(1, 0)) current.x++;
      break;
    case Down:
      if (!checkCollision(0, 1)) current.y++;
      break;
    default:
      break;
  }
}

int main() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(1);
  initializeGame();

  int frame = 0;
  while (!gameOver) {
    clear();
    for (int i = 0; i <= HEIGHT; i++) {
      mvaddch(i, 0, ACS_VLINE);
      mvaddch(i, WIDTH * 2 + 1, ACS_VLINE);
    }
    mvaddch(HEIGHT, 0, ACS_LLCORNER);
    mvaddch(HEIGHT, WIDTH * 2 + 1, ACS_LRCORNER);
    int ch = getch();
    switch (ch) {
      case KEY_LEFT:
        userInput(Left);
        break;
      case KEY_RIGHT:
        userInput(Right);
        break;
      case KEY_DOWN:
        userInput(Down);
        break;
      case 'q':
        gameOver = true;
        break;
    }

    if (frame++ % 10 == 0) {
      if (!checkCollision(0, 1))
        current.y++;
      else {
        for (int i = 0; i < 4; i++)
          for (int j = 0; j < 4; j++)
            if (current.shape[i][j])
              gameInfo.field[current.y + i][current.x + j] = 1;
        createShape(&current);
        current.x = WIDTH / 2 - 2;
        current.y = 0;
        if (checkCollision(0, 0)) gameOver = true;
      }
    }

    drawField();
    drawCurrent();
    refresh();
    usleep(50000);
  }

  endwin();
  return 0;
}