#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 10
#define HEIGHT 20

#define MAX_LEVEL 10
#define SCORE_UP_LEVEL 600
#define START_SPEED 500
#define SPEED_STEP 50
#define MIN_SPEED 100

#define SECOND 1000
#define THOUSAND_SECONDS 1000000

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

typedef enum State {
  START,
  SPAWN,
  MOVING,
  ATTACHING,
  SHIFT,
  GAME_OVER
} FSM_State;

typedef struct {
  int shape[4][4];
  char type;
  char nextType;
  int cursorX, cursorY;
  int rotationPosition;
  int quit;
  bool start;
  bool gameOver;
} Cursor;

typedef struct {
  GameInfo_t *gameInfo;
  Cursor *cursor;
} InputContext;

#include "./brick_game/tetris/backend.h"
#include "./gui/cli/frontend.h"

#endif  // GAME_H