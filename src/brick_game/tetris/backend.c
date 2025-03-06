#include "../../game.h"

void fsm_loop(GameInfo_t *gameInfo, Cursor *cursor,
              struct timespec *last_move_time, FSM_State current_state) {
  while (!cursor->quit) {
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    int ch = getch();
    controller(ch);
    flushinp();
    if (!gameInfo->pause && !cursor->gameOver) {
      switch (current_state) {
        case START:
          printField(gameInfo);
          waitStart(cursor);
          current_state = SHIFT;
          break;
        case SPAWN:
          if (spawn(gameInfo, cursor))
            current_state = SHIFT;
          else
            current_state = GAME_OVER;
          break;
        case SHIFT: {
          bool collision = checkSide(gameInfo, cursor->cursorX,
                                     cursor->cursorY + 1, cursor->shape);
          if (collision) current_state = CONNECTION;
          break;
        }
        case CONNECTION:
          mergeCursor(gameInfo, cursor);
          clearLines(gameInfo);
          current_state = SPAWN;
          break;
        case GAME_OVER:
          cursor->gameOver = true;
          if (ch == '\n') {
            initializeGame(gameInfo, cursor);
            current_state = START;
          }
          break;
      }
    }
    timer(gameInfo, cursor, last_move_time, &current_time, current_state);
    rendering(gameInfo, cursor);
    usleep(10000);
  }
}

GameInfo_t updateCurrentState() {
  static GameInfo_t gameInfo;
  static Cursor cursor;
  static FSM_State current_state = START;
  static struct timespec last_move_time = {0};
  InputContext *ctx = getInputContext();
  ctx->gameInfo = &gameInfo;
  ctx->cursor = &cursor;
  initializeGame(&gameInfo, &cursor);
  clock_gettime(CLOCK_MONOTONIC, &last_move_time);
  fsm_loop(&gameInfo, &cursor, &last_move_time, current_state);

  return gameInfo;
}

// static GameInfo_t *getCurrentGameInfo() {
//   static GameInfo_t *currentGameInfo = NULL;
//   return currentGameInfo;
// }

void timer(GameInfo_t *gameInfo, Cursor *cursor,
           struct timespec *last_move_time, struct timespec *current_time,
           FSM_State current_state) {
  long time_diff = (current_time->tv_sec - last_move_time->tv_sec) * 1000 +
                   (current_time->tv_nsec - last_move_time->tv_nsec) / 1000000;

  if (time_diff >= gameInfo->speed && current_state == SHIFT) {
    if (!checkSide(gameInfo, cursor->cursorX, cursor->cursorY + 1,
                   cursor->shape) &&
        gameInfo->pause == 0 && !cursor->gameOver) {
      moving(cursor);
    }
    clock_gettime(CLOCK_MONOTONIC, last_move_time);
  }
}

UserAction_t controller(int ch) {
  UserAction_t action = Terminate;
  bool hold = false;

  switch (ch) {
    case KEY_LEFT:
      action = Left;
      hold = true;
      break;
    case KEY_RIGHT:
      action = Right;
      hold = true;
      break;
    case KEY_DOWN:
      action = Down;
      hold = true;
      break;
    case ' ':
      action = Action;
      hold = true;
      break;
    case 'q':
      action = Terminate;
      hold = true;
      break;
    case 'p':
      action = Pause;
      hold = true;
      break;
    case '\n':
      action = Start;
      bool hold = true;
      break;
    default:
      action = ch;
      hold = false;
      break;
  }

  if (hold) userInput(action, hold);  // вынести
  return action;
}

void userInput(UserAction_t action, bool hold) {
  InputContext *ctx = getInputContext();
  GameInfo_t *gameInfo = ctx->gameInfo;
  Cursor *cursor = ctx->cursor;

  // if (!gameInfo || !cursor) return; // change

  switch (action) {
    case Left:
      if (hold &&
          !checkSide(gameInfo, cursor->cursorX - 1, cursor->cursorY,
                     cursor->shape) &&
          gameInfo->pause == 0)
        cursor->cursorX--;
      break;
    case Right:
      if (hold &&
          !checkSide(gameInfo, cursor->cursorX + 1, cursor->cursorY,
                     cursor->shape) &&
          gameInfo->pause == 0)
        cursor->cursorX++;
      break;
    case Action:
      if (hold && gameInfo->pause == 0) rotateShape(gameInfo, cursor);
      break;
    case Down:
      if (hold && gameInfo->pause == 0) {
        while (!checkSide(gameInfo, cursor->cursorX, cursor->cursorY + 1,
                          cursor->shape))
          cursor->cursorY++;
      }
      break;
    case Start:
      if (hold) initializeGame(gameInfo, cursor);
      break;
    case Pause:
      if (!gameInfo->pause)
        gameInfo->pause = 1;
      else
        gameInfo->pause = 0;
      break;
      break;
    case Terminate:
      cursor->quit = 1;
      break;
  }
}

static InputContext *getInputContext() {
  static InputContext context;
  return &context;
}

bool spawn(GameInfo_t *gameInfo, Cursor *cursor) {
  bool result = true;
  cursor->cursorX = WIDTH / 2 - 2;
  cursor->cursorY = 0;
  if (checkSide(gameInfo, cursor->cursorX, cursor->cursorY, cursor->shape))
    result = false;
  else {
    char createNext = 1;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = gameInfo->next[i][j];
      }
    }
    cursor->type = cursor->nextType;

    createShape(gameInfo, cursor, createNext);

    if (checkSide(gameInfo, cursor->cursorX, cursor->cursorY, cursor->shape))
      result = false;
  }
  return result;
}

void createShape(GameInfo_t *gameInfo, Cursor *cursor, int flag) {
  int shapes[7][4][4] = {
      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
      {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}},  // T
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}},  // S
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // Z
      {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}},  // L
      {{0, 0, 0, 0}, {0, 0, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 0}}   // J
  };

  char types[] = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};
  int shapeIndex = rand() % 7;

  if (flag == 0) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = shapes[shapeIndex][i][j];
      }
    }
    cursor->type = types[shapeIndex];
    cursor->rotationPosition = 0;
  } else if (flag == 1) {
    cursor->nextType = types[shapeIndex];
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        gameInfo->next[i][j] = shapes[shapeIndex][i][j];
      }
    }
  }
}

void rotateShape(GameInfo_t *gameInfo, Cursor *cursor) {
  switch (cursor->type) {
    case 'I':
      sziShapeRotate(gameInfo, cursor);
      break;

    case 'J':
      jltShapeRotate(gameInfo, cursor);
      break;

    case 'L':
      jltShapeRotate(gameInfo, cursor);

      break;
    case 'O':
      break;

    case 'S':
      sziShapeRotate(gameInfo, cursor);
      break;

    case 'Z':
      sziShapeRotate(gameInfo, cursor);
      break;

    case 'T':
      jltShapeRotate(gameInfo, cursor);
      break;
  }
}

void jltShapeRotate(GameInfo_t *gameInfo, Cursor *cursor) {
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
      int newX = -y;
      int newY = x;
      cursor->shape[newY + 1][newX + 2] = temp[i][j];
    }
  }
  if (checkSide(gameInfo, cursor->cursorX, cursor->cursorY, cursor->shape)) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[i][j];
      }
    }
  }
}

void sziShapeRotate(GameInfo_t *gameInfo, Cursor *cursor) {
  int temp[4][4];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp[i][j] = cursor->shape[i][j];
    }
  }
  if (cursor->rotationPosition == 0) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[j][3 - i];
      }
    }
    cursor->rotationPosition = 1;
  } else {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[3 - j][i];
      }
    }
    cursor->rotationPosition = 0;
  }
  if (checkSide(gameInfo, cursor->cursorX, cursor->cursorY, cursor->shape)) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        cursor->shape[i][j] = temp[i][j];
      }
    }
  }
}

bool checkSide(GameInfo_t *gameInfo, int x, int y, int cursor[4][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor[i][j] && (x + j < 0 || x + j >= WIDTH || y + i >= HEIGHT ||
                           gameInfo->field[y + i][x + j])) {
        return true;
      }
    }
  }
  return false;
}

int clearLines(GameInfo_t *gameInfo) {
  int linesCleared = 0;

  for (int i = HEIGHT - 1; i >= 0; i--) {
    bool full = true;
    for (int j = 0; j < WIDTH; j++) {
      if (!gameInfo->field[i][j]) {
        full = false;
        break;
      }
    }
    if (full) {
      for (int k = i; k > 0; k--) {
        memcpy(gameInfo->field[k], gameInfo->field[k - 1], WIDTH * sizeof(int));
      }
      memset(gameInfo->field[0], 0, WIDTH * sizeof(int));
      linesCleared++;
      i++;
    }
  }

  switch (linesCleared) {
    case 1:
      gameInfo->score += 100;
      break;
    case 2:
      gameInfo->score += 300;
      break;
    case 3:
      gameInfo->score += 700;
      break;
    case 4:
      gameInfo->score += 1500;
      break;
  }

  int new_level = gameInfo->score / SCORE_UP_LEVEL + 1;
  if (new_level > gameInfo->level && new_level <= MAX_LEVEL) {
    gameInfo->level = new_level;
    gameInfo->speed = START_SPEED - (gameInfo->level - 1) * SPEED_STEP;
    if (gameInfo->speed < MIN_SPEED) gameInfo->speed = MIN_SPEED;
  }

  if (gameInfo->score > gameInfo->high_score) {
    gameInfo->high_score = gameInfo->score;
    saveHighScore(gameInfo->high_score);
  }
  return linesCleared;
}

void moving(Cursor *cursor) { cursor->cursorY++; }

void saveHighScore(int highScore) {
  FILE *file = fopen("highscore.txt", "w");
  if (file) {
    if (highScore > loadHighScore()) {
      fprintf(file, "%d", highScore);
      fclose(file);
    }
  }
}

void initializeGame(GameInfo_t *gameInfo, Cursor *cursor) {
  int createNew = 0;
  int createNext = 1;
  gameInfo->field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo->field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo->field[i][j] = 0;
    }
  }

  gameInfo->next = (int **)calloc(4, sizeof(int *));
  for (int i = 0; i < 4; i++) {
    gameInfo->next[i] = (int *)calloc(4, sizeof(int));
    for (int j = 0; j < 4; j++) {
      gameInfo->next[i][j] = 0;
    }
  }

  gameInfo->score = 0;
  gameInfo->high_score = loadHighScore();
  gameInfo->level = 1;
  gameInfo->speed = START_SPEED;
  cursor->gameOver = false;
  gameInfo->pause = 0;

  srand(time(NULL));
  createShape(gameInfo, cursor, createNew);  // выделить для тестов
  createShape(gameInfo, cursor, createNext);
  createShape(gameInfo, cursor, createNew);
  cursor->cursorX = WIDTH / 2 - 2;
  cursor->cursorY = 0;
}

// void initShape(GameInfo_t *gameInfo, Cursor *cursor, int flag) {}

void mergeCursor(GameInfo_t *gameInfo, Cursor *cursor) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (cursor->shape[i][j]) {
        gameInfo->field[cursor->cursorY + i][cursor->cursorX + j] =
            cursor->shape[i][j];
      }
    }
  }
}

int loadHighScore() {
  FILE *file = fopen("highscore.txt", "r");
  int highScore = 0;

  if (file) {
    fscanf(file, "%d", &highScore);
    fclose(file);
  } else {
    file = fopen("highscore.txt", "w");
    if (file) {
      fprintf(file, "%d", highScore);
      fclose(file);
    }
  }

  return highScore;
}

void waitStart(Cursor *cursor) {
  if (!cursor->start) {
    refresh();
    while (getch() != '\n') printStart();
  }
}