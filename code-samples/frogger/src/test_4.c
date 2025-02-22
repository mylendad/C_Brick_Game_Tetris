#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

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

void userInput(UserAction_t action, bool hold);

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
