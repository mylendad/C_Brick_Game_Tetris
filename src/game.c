#include "game.h"
#include "fsm_loop.h"
#include "./gui/cli/frontend.h"
#include "./brick_game/tetris/backend.h"

int main() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(0);
  initColor();
  static GameInfo_t game_info;
  static Cursor_s cursor;
  static FSM_State_e current_state = START;
  static struct timespec last_move_time = {0};
  clock_gettime(CLOCK_MONOTONIC, &last_move_time);
  InputContext_s *ctx = getInputContext();
  ctx->game_info = &game_info;
  ctx->cursor = &cursor;
  ctx->state = &current_state;
  ctx->time = &last_move_time;
  fsmLoop();
  usleep(1000);
  endwin();
  return 0;
}
