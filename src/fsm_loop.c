#include "gui/cli/frontend.h"
#include "brick_game/tetris/backend.h"

void fsmLoop() {
  InputContext_s *ctx = getInputContext();
  GameInfo_t *game_info = ctx->game_info;
  Cursor_s *cursor = ctx->cursor;
  FSM_State_e *current_state = ctx->state;
  initializeGame(game_info, cursor);
  UserAction_t action = Terminate;
  bool hold = false;
  while (!cursor->quit) {
    int ch = getch();
    action = controllerInput(ch);
    if (action >= 0 || action < 8) {
      hold = true;
      userInput(action, hold);
    }
    flushinp();  //  discards (flushes) any characters in the input buffer
                 //  associated with the current screen
    if (!game_info->pause && !cursor->game_over) {
      switch (*current_state) {
        case START:
          printField(game_info);  // del
          waitStart(cursor);      // del
          *current_state = SHIFT;
          break;
        case SHIFT:
          if (checkSide(game_info, cursor->cursor_x, cursor->cursor_y + 1,
                        cursor->shape))
            *current_state = ATTACHING;
          break;
        case ATTACHING:
          mergeCursor(game_info, cursor);
          clearLines(game_info);
          *current_state = SPAWN;
          break;
        case SPAWN:
          if (spawnShape(game_info, cursor))
            *current_state = SHIFT;
          else
            *current_state = GAME_OVER;
          break;
        case GAME_OVER:
          cursor->game_over = true;
          break;
      }
    }
    if (SHIFT) {
      updateCurrentState();
      renderingGame();
    }
  }
}
