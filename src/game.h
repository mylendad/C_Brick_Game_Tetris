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
#define CENTER WIDTH / 2 + 2
#define CENTER_TOP WIDTH / 2 - 2
#define RIGHT_WALL WIDTH * 2 + 2
#define ON_THE_RIGHT WIDTH * 2 + 4

#define HORISONTAL 0
#define VERTICAL 1

#define NEW_SHAPE 0
#define NEXT_SHAPE 1

#define TRUE 1
#define FALSE 0

#define MAX_LEVEL 10
#define SCORE_UP_LEVEL 600
#define START_SPEED 500
#define SPEED_STEP 50
#define MIN_SPEED 100

#define SECOND 1000

#define QUANTITY_OF_SHAPES 7

#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
#define CYAN 5
#define FIOLET 6
#define WHITE 7

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

typedef enum State { START, SHIFT, ATTACHING, SPAWN, GAME_OVER } FSM_State_e;

typedef struct {
  int shape[4][4];
  char type;
  char next_type;
  int cursor_x, cursor_y;
  int rotation_position;
  int quit;
  bool start;
  bool game_over;
} Cursor_s;

typedef struct {
  GameInfo_t *game_info;
  Cursor_s *cursor;
  FSM_State_e *state;
  struct timespec *time;
} InputContext_s;

#endif  // GAME_H