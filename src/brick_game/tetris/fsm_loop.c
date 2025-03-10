#include "../../gui/cli/frontend.h"
#include "backend.h"

void fsm_loop(GameInfo_t *game_info, Cursor_s *cursor,
              struct timespec *last_move_time, FSM_State_e current_state) {
  UserAction_t action = Terminate;
  bool hold = false;
  while (!cursor->quit) {
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    int ch = getch();
    action = controllerInput(ch);
    if (action >= 0 || action < 8) {
      hold = true;
      userInput(action, hold);
    }
    flushinp();
    if (!game_info->pause && !cursor->game_over) {
      switch (current_state) {
        case START:
          printField(game_info);
          waitStart(cursor);
          current_state = SHIFT;
          break;
        case SPAWN:
          if (spawnShape(game_info, cursor))
            current_state = SHIFT;
          else
            current_state = GAME_OVER;
          break;
        case SHIFT: {
          bool collision = checkSide(game_info, cursor->cursor_x,
                                     cursor->cursor_y + 1, cursor->shape);
          if (collision) current_state = ATTACHING;
          break;
        }
        case ATTACHING:
          mergeCursor(game_info, cursor);
          clearLines(game_info);
          current_state = SPAWN;
          break;
        case GAME_OVER:
          cursor->game_over = true;
          break;
      }
    }
    if (SHIFT)
      renderingGame(game_info, cursor, last_move_time, current_state,
                    &current_time);
  }
}