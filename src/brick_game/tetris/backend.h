#ifndef BACKEND_H
#define BACKEND_H

#include "../../game.h"

GameInfo_t updateCurrentState();
void initializeGame(GameInfo_t *gameInfo, Cursor_s *cursor);
static Cursor_s *getCurrentCursor();
static void setCurrentContext(GameInfo_t *gameInfo, Cursor_s *cursor);
void fsm_loop(GameInfo_t *gameInfo, Cursor_s *cursor,
              struct timespec *last_move_time, FSM_State_e current_state);
int clearLines(GameInfo_t *gameInfo);
bool checkSide(GameInfo_t *gameInfo, int x, int y, int cursor[4][4]);
void mergeCursor(GameInfo_t *gameInfo, Cursor_s *cursor);

UserAction_t controllerInput(int ch);
void userInput(UserAction_t action, bool hold);
static InputContext_s *getInputContext();

void renderingGame(GameInfo_t *gameInfo, Cursor_s *cursor,
                   struct timespec *last_move_time, FSM_State_e current_state,
                   struct timespec *current_time);
void timerFunc(GameInfo_t *gameInfo, Cursor_s *cursor,
               struct timespec *last_move_time, struct timespec *current_time,
               FSM_State_e current_state);
void moveDown(GameInfo_t *gameInfo, Cursor_s *cursor);

void createShape(GameInfo_t *gameInfo, Cursor_s *cursor, int shapeIndex,
                 int flag);
bool spawnShape(GameInfo_t *gameInfo, Cursor_s *cursor);
void rotateShape(GameInfo_t *gameInfo, Cursor_s *cursor);
void sziShapeRotate(GameInfo_t *gameInfo, Cursor_s *cursor);
void jltShapeRotate(GameInfo_t *gameInfo, Cursor_s *cursor);

void saveHighScore(int highScore);
int loadHighScore();

void waitStart(Cursor_s *cursor);
void waitQuit(Cursor_s *cursor);

#endif  // BACKEND_H