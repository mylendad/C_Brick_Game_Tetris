#include "game.h"

int main() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(0);
  initColor();
  updateCurrentState();
  usleep(1000);
  endwin();
  return 0;
}
