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
  int rotationPosition;
} Cursor;

static GameInfo_t gameInfo;
static Cursor currentCursor;
static Cursor nextCursor;
static int cursorX, cursorY;
static bool gameOver = false;
static bool paused = false;

void userInput(UserAction_t action, bool hold);

void userInput(UserAction_t action, bool hold) {
  if (gameOver) return;

  //   switch (action) {
  //     case Start:
  //       if (gameOver) {
  //         gameOver = false;
  //       }
  //       break;
  //     case Pause:
  //       break;
  //     case Terminate:
  //       break;
  //     case Left:
  //         cursorX--;
  //       }
  //       break;
  //     case Right:
  //         cursorX++;
  //       }
  //       break;
  //     case Down:
  //         cursorY++;
  //       }
  //       break;
  //     case Up:
  //       break;
  //     case Action:
  //       break;
  //   }
}

void rendering() {
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

  refresh();
}

int main() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);

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

    usleep(5000);
  }

  endwin();

  return 0;
}
