#ifndef BACKEND_H
#define BACKEND_H

#include "../../game.h"

GameInfo_t updateCurrentState();
void logic(GameInfo_t *gameInfo, Cursor *cursor,
           struct timespec *last_move_time, FSM_State current_state,
           struct timespec *current_time);
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
void createShape(GameInfo_t *gameInfo, Cursor *cursor, int shapeIndex,
                 int flag);

#endif  // BACKEND_H