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

static GameInfo_t gameInfo;
static int currentPiece[4][4];
static int nextPiece[4][4];
static int pieceX, pieceY;
static bool gameOver = false;
static bool paused = false;

const int wallKickI[4][5][2] = {{{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},
                                {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},
                                {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},
                                {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}};

const int wallKickOther[4][5][2] = {
    {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}};

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

void generatePiece(int piece[4][4]) {
  int shapes[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},  // S
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // Z
      {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}},  // L
      {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}   // J
  };

  int shape = rand() % 7;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      piece[i][j] = shapes[shape][i][j];
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

void rotatePiece(int piece[4][4]) {
  int temp[4][4];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp[i][j] = piece[i][j];
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      piece[i][j] = temp[3 - j][i];
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      piece[i][j] = temp[i][j];
    }
  }
}

void mergeCursor() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentPiece[i][j]) {
        gameInfo.field[pieceY + i][pieceX + j] = currentPiece[i][j];
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
      if (!checkSide(pieceX - 1, pieceY, currentPiece)) {
        pieceX--;
      }
      break;
    case Right:
      if (!checkSide(pieceX + 1, pieceY, currentPiece)) {
        pieceX++;
      }
      break;
    case Down:
      if (!checkSide(pieceX, pieceY + 1, currentPiece)) {
        pieceY++;
      }
      break;
    case Up:
      rotatePiece(currentPiece);
      break;
    case Action:
      break;
  }
}

GameInfo_t updateCurrentState() {
  if (!paused && !gameOver) {
    if (checkSide(pieceX, pieceY + 1, currentPiece)) {
      mergeCursor();
      clearLines();
      generatePiece(currentPiece);
      pieceX = WIDTH / 2 - 2;
      pieceY = 0;
      if (checkSide(pieceX, pieceY, currentPiece)) {
        gameOver = true;
      }
    } else {
      pieceY++;
    }
  }

  return gameInfo;
}

void drawGame() {
  clear();

  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (gameInfo.field[i][j]) {
        mvprintw(i + 1, j * 2 + 1, "[]");
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (currentPiece[i][j]) {
        mvprintw(pieceY + i + 1, (pieceX + j) * 2 + 1, "[]");
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

int main() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(gameInfo.speed);

  start_color();

  initializeGame();
  generatePiece(currentPiece);
  pieceX = WIDTH / 2 - 2;
  pieceY = 0;

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

    flushinp();

    GameInfo_t info = updateCurrentState();
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
