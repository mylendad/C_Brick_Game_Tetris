#include "backend.h"

#include "../../gui/cli/frontend.h"

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

void renderingGame(GameInfo_t *game_info, Cursor_s *cursor,
                   struct timespec *last_move_time, FSM_State_e current_state,
                   struct timespec *current_time) {
  timerFunc(game_info, cursor, last_move_time, current_time, current_state);
  printFrontend(game_info, cursor);
  usleep(SECOND * 10);
}

GameInfo_t updateCurrentState() {
  static GameInfo_t game_info;
  static Cursor_s cursor;
  static FSM_State_e current_state = START;
  static struct timespec last_move_time = {0};
  InputContext_s *ctx = getInputContext();
  ctx->game_info = &game_info;
  ctx->cursor = &cursor;
  initializeGame(&game_info, &cursor);
  clock_gettime(CLOCK_MONOTONIC, &last_move_time);
  fsm_loop(&game_info, &cursor, &last_move_time, current_state);

  return game_info;
}

void timerFunc(GameInfo_t *game_info, Cursor_s *cursor,
               struct timespec *last_move_time, struct timespec *current_time,
               FSM_State_e current_state) {
  long time_diff =
      (current_time->tv_sec - last_move_time->tv_sec) * SECOND +
      (current_time->tv_nsec - last_move_time->tv_nsec) / THOUSAND_SECONDS;

  if (time_diff >= game_info->speed && current_state == SHIFT) {
    if (!checkSide(game_info, cursor->cursor_x, cursor->cursor_y + 1,
                   cursor->shape) &&
        game_info->pause == 0 && !cursor->game_over) {
      cursor->cursor_y++;
    }
    clock_gettime(CLOCK_MONOTONIC, last_move_time);
  }
}

UserAction_t controllerInput(int ch) {
  UserAction_t action = Terminate;

  switch (ch) {
    case KEY_ENTER:
      action = Start;
      break;
    case KEY_LEFT:
      action = Left;
      break;
    case KEY_RIGHT:
      action = Right;
      break;
    case KEY_DOWN:
      action = Down;
      break;
    case ' ':
      action = Action;
      break;
    case 'q':
      action = Terminate;
      break;
    case 'p':
      action = Pause;
      break;
    default:
      action = ch;
      break;
  }

  return action;
}

void userInput(UserAction_t action, bool hold) {
  InputContext_s *ctx = getInputContext();
  GameInfo_t *game_info = ctx->game_info;
  Cursor_s *cursor = ctx->cursor;

  switch (action) {
    case Left:
      if (hold &&
          !checkSide(game_info, cursor->cursor_x - 1, cursor->cursor_y,
                     cursor->shape) &&
          game_info->pause == FALSE)
        cursor->cursor_x--;
      break;
    case Right:
      if (hold &&
          !checkSide(game_info, cursor->cursor_x + 1, cursor->cursor_y,
                     cursor->shape) &&
          game_info->pause == FALSE)
        cursor->cursor_x++;
      break;
    case Up:
      break;
    case Action:
      if (hold && game_info->pause == FALSE) rotateShape(game_info, cursor);
      break;
    case Down:
      if (hold && game_info->pause == FALSE) {
        while (!checkSide(game_info, cursor->cursor_x, cursor->cursor_y + 1,
                          cursor->shape))
          cursor->cursor_y++;
      }
      break;
    case Start:
      if (hold) initializeGame(game_info, cursor);
      break;
    case Pause:
      if (!game_info->pause)
        game_info->pause = TRUE;
      else
        game_info->pause = FALSE;
      break;
      break;
    case Terminate:
      cursor->quit = TRUE;
      break;
  }
}

InputContext_s *getInputContext() {
  static InputContext_s context;
  return &context;
}

bool spawnShape(GameInfo_t *game_info, Cursor_s *cursor) {
  bool result = true;
  cursor->cursor_x = CENTER_TOP;
  cursor->cursor_y = 0;
  if (checkSide(game_info, cursor->cursor_x, cursor->cursor_y, cursor->shape))
    result = false;
  else {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = game_info->next[i][j];
      }
    }
    cursor->type = cursor->next_type;
    int shape_index = rand() % 7;
    createShape(game_info, cursor, shape_index, NEXT_SHAPE);

    if (checkSide(game_info, cursor->cursor_x, cursor->cursor_y, cursor->shape))
      result = false;
  }
  return result;
}

void createShape(GameInfo_t *game_info, Cursor_s *cursor, int shape_index,
                 int flag) {
  int shapes[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},  // Z
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // S
      {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}},  // L
      {{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}}   // J
  };

  char types[] = {'I', 'O', 'T', 'Z', 'S', 'J', 'L'};

  if (flag == NEW_SHAPE) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = shapes[shape_index][i][j];
      }
    }
    cursor->type = types[shape_index];
    cursor->rotation_position = HORISONTAL;
  } else if (flag == NEXT_SHAPE) {
    cursor->next_type = types[shape_index];
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        game_info->next[i][j] = shapes[shape_index][i][j];
      }
    }
  }
}

void rotateShape(GameInfo_t *game_info, Cursor_s *cursor) {
  switch (cursor->type) {
    case 'I':
      sziShapeRotate(game_info, cursor);
      break;

    case 'J':
      jltShapeRotate(game_info, cursor);
      break;

    case 'L':
      jltShapeRotate(game_info, cursor);

      break;
    case 'O':
      break;

    case 'S':
      sziShapeRotate(game_info, cursor);
      break;

    case 'Z':
      sziShapeRotate(game_info, cursor);
      break;

    case 'T':
      jltShapeRotate(game_info, cursor);
      break;
  }
}

void jltShapeRotate(GameInfo_t *game_info, Cursor_s *cursor) {
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
      int new_x = -y;
      int new_y = x;
      cursor->shape[new_y + 1][new_x + 2] = temp[i][j];
    }
  }
  if (checkSide(game_info, cursor->cursor_x, cursor->cursor_y, cursor->shape)) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[i][j];
      }
    }
  }
}

void sziShapeRotate(GameInfo_t *game_info, Cursor_s *cursor) {
  int temp[4][4];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp[i][j] = cursor->shape[i][j];
    }
  }
  if (cursor->rotation_position == 0) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[j][3 - i];
      }
    }
    cursor->rotation_position = 1;
  } else {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[3 - j][i];
      }
    }
    cursor->rotation_position = 0;
  }
  if (checkSide(game_info, cursor->cursor_x, cursor->cursor_y, cursor->shape)) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[i][j];
      }
    }
  }
}

bool checkSide(GameInfo_t *game_info, int x, int y, int cursor[4][4]) {
  bool result = false;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor[i][j] && (x + j < 0 || x + j >= WIDTH || y + i >= HEIGHT ||
                           game_info->field[y + i][x + j])) {
        result = true;
      }
    }
  }
  return result;
}

int clearLines(GameInfo_t *game_info) {
  int lines_cleared = 0;

  for (int i = HEIGHT - 1; i >= 0; i--) {
    bool full = true;
    for (int j = 0; j < WIDTH; j++) {
      if (!game_info->field[i][j]) {
        full = false;
        break;
      }
    }
    if (full) {
      for (int k = i; k > 0; k--) {
        memcpy(game_info->field[k], game_info->field[k - 1],
               WIDTH * sizeof(int));
      }
      memset(game_info->field[0], 0, WIDTH * sizeof(int));
      lines_cleared++;
      i++;
    }
  }

  switch (lines_cleared) {
    case 1:
      game_info->score += 100;  // points for 1 line
      break;
    case 2:
      game_info->score += 300;  // points for 2 lines
      break;
    case 3:
      game_info->score += 700;  // points for 3 lines
      break;
    case 4:
      game_info->score += 1500;  // points for 4 lines
      break;
  }

  int new_level = game_info->score / SCORE_UP_LEVEL + 1;
  if (new_level > game_info->level && new_level <= MAX_LEVEL) {
    game_info->level = new_level;
    game_info->speed = START_SPEED - (game_info->level - 1) * SPEED_STEP;
    if (game_info->speed < MIN_SPEED) game_info->speed = MIN_SPEED;
  }

  if (game_info->score > game_info->high_score) {
    game_info->high_score = game_info->score;
    saveHighScore(game_info->high_score);
  }
  return lines_cleared;
}

void saveHighScore(int high_score) {
  FILE *file = fopen("highscore.txt", "w");
  if (file) {
    if (high_score > loadHighScore()) {
      fprintf(file, "%d", high_score);
      fclose(file);
    }
  }
}

void initializeGame(GameInfo_t *game_info, Cursor_s *cursor) {
  game_info->field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    game_info->field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      game_info->field[i][j] = 0;
    }
  }

  game_info->next = (int **)calloc(4, sizeof(int *));
  for (int i = 0; i < 4; i++) {
    game_info->next[i] = (int *)calloc(4, sizeof(int));
    for (int j = 0; j < 4; j++) {
      game_info->next[i][j] = 0;
    }
  }

  game_info->score = 0;
  game_info->high_score = loadHighScore();
  game_info->level = 1;
  game_info->speed = START_SPEED;
  cursor->game_over = false;
  game_info->pause = 0;

  srand(time(NULL));
  int shape_index = rand() % 7;
  createShape(game_info, cursor, shape_index, NEW_SHAPE);
  createShape(game_info, cursor, shape_index, NEXT_SHAPE);
  createShape(game_info, cursor, shape_index, NEW_SHAPE);

  cursor->cursor_x = CENTER_TOP;
  cursor->cursor_y = 0;
}

void mergeCursor(GameInfo_t *game_info, Cursor_s *cursor) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j]) {
        game_info->field[cursor->cursor_y + i][cursor->cursor_x + j] =
            cursor->shape[i][j];
      }
    }
  }
}

int loadHighScore() {
  FILE *file = fopen("highscore.txt", "r");
  int high_score = 0;

  if (file) {
    fscanf(file, "%d", &high_score);
    fclose(file);
  } else {
    file = fopen("highscore.txt", "w");
    if (file) {
      fprintf(file, "%d", high_score);
      fclose(file);
    }
  }
  return high_score;
}

void waitStart(Cursor_s *cursor) {
  if (!cursor->start) {
    refresh();
    while (getch() != '\n') printStart();
  }
}