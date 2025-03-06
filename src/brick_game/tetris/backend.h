#ifndef BACKEND_H
#define BACKEND_H

// #include <ncurses.h>
// #include <stdbool.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
// #include <unistd.h>

// void initColor();
// GameInfo_t updateCurrentState();

#include "../../game.h"

// typedef enum {
//   Start,
//   Pause,
//   Terminate,
//   Left,
//   Right,
//   Up,
//   Down,
//   Action
// } UserAction_t;

// typedef struct {
//   int **field;
//   int **next;
//   int score;
//   int high_score;
//   int level;
//   int speed;
//   int pause;
// } GameInfo_t;

// typedef enum State {
//   START,
//   SPAWN,
//   MOVING,
//   CONNECTION,
//   SHIFT,
//   GAME_OVER
// } FSM_State;

// typedef struct {
//   int shape[4][4];
//   char type;
//   char nextType;
//   int cursorX, cursorY;
//   int rotationPosition;
//   int quit;
//   bool start;
//   bool gameOver;
// } Cursor;

// typedef struct {
//   GameInfo_t *gameInfo;
//   Cursor *cursor;
// } InputContext;
// #include "../../gui/cli/frontend.h"

// #define WIDTH s
// #define HEIGHT 20

// #define MAX_LEVEL 10
// #define SCORE_UP_LEVEL 600
// #define START_SPEED 500
// #define SPEED_STEP 50
// #define MIN_SPEED 100

// typedef enum {
//   Start,
//   Pause,
//   Terminate,
//   Left,
//   Right,
//   Up,
//   Down,
//   Action
// } UserAction_t;

// typedef struct {
//   int **field;
//   int **next;
//   int score;
//   int high_score;
//   int level;
//   int speed;
//   int pause;
// } GameInfo_t;

// typedef enum State {
//   START,
//   SPAWN,
//   MOVING,
//   CONNECTION,
//   SHIFT,
//   GAME_OVER
// } FSM_State;

// typedef struct {
//   int shape[4][4];
//   char type;
//   char nextType;
//   int cursorX, cursorY;
//   int rotationPosition;
//   int quit;
//   bool start;
//   bool gameOver;
// } Cursor;

// typedef struct {
//   GameInfo_t *gameInfo;
//   Cursor *cursor;
// } InputContext;s

GameInfo_t updateCurrentState();
void rotateShape(GameInfo_t *gameInfo, Cursor *cursor);
static InputContext *getInputContext();
void fsm_loop(GameInfo_t *gameInfo, Cursor *cursor,
              struct timespec *last_move_time, FSM_State current_state);
void timer(GameInfo_t *gameInfo, Cursor *cursor,
           struct timespec *last_move_time, struct timespec *current_time,
           FSM_State current_state);
void shiftFieldDown(GameInfo_t *gameInfo);
void saveHighScore(int highScore);
void userInput(UserAction_t action, bool hold);
UserAction_t controller(int ch);
static void setCurrentContext(GameInfo_t *gameInfo, Cursor *cursor);
// static GameInfo_t *getCurrentGameInfo();
static Cursor *getCurrentCursor();
void down(GameInfo_t *gameInfo, Cursor *cursor);
bool spawn(GameInfo_t *gameInfo, Cursor *cursor);
struct timespec timerFunc();
void game();
void sziShapeRotate(GameInfo_t *gameInfo, Cursor *cursor);
void jltShapeRotate(GameInfo_t *gameInfo, Cursor *cursor);
void moving(Cursor *cursor);
void waitStart(Cursor *cursor);
void waitQuit(Cursor *cursor);
int clearLines(GameInfo_t *gameInfo);
void mergeCursor(GameInfo_t *gameInfo, Cursor *cursor);
int loadHighScore();
void generateNextShape(GameInfo_t *gameInfo, Cursor *cursor);
void initializeGame(GameInfo_t *gameInfo, Cursor *cursor);
bool checkSide(GameInfo_t *gameInfo, int x, int y, int cursor[4][4]);
void createShape(GameInfo_t *gameInfo, Cursor *cursor, int flag);

#endif  // BACKEND_H