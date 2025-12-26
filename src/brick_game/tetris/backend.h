#ifndef BACKEND_H
#define BACKEND_H

#include "../../game.h"

GameInfo_t updateCurrentState();
void initializeGame(GameInfo_t *gameInfo, Cursor_s *cursor);
void fsmLoop();
int clearLines(GameInfo_t *gameInfo);
bool checkSide(GameInfo_t *game_info, int x, int y, int cursor[4][4]);
void mergeCursor(GameInfo_t *gameInfo, Cursor_s *cursor);

UserAction_t controllerInput(int ch);
void userInput(UserAction_t action, bool hold);
InputContext_s *getInputContext();

void renderingGame();
void moveOnTimerFunc(struct timespec *current_time);
void moveDown(GameInfo_t *gameInfo, Cursor_s *cursor);

void createShape(GameInfo_t *game_info, Cursor_s *cursor, int shape_index,
                 int flag);
bool spawnShape(GameInfo_t *gameInfo, Cursor_s *cursor);
void rotateShape(GameInfo_t *gameInfo, Cursor_s *cursor);
void sziShapeRotate(GameInfo_t *gameInfo, Cursor_s *cursor);
void jltShapeRotate(GameInfo_t *gameInfo, Cursor_s *cursor);

void saveHighScore(int highScore);
int loadHighScore();

void waitStart(Cursor_s *cursor);
void waitQuit(Cursor_s *cursor);

void sleep_ms(long ms);

#endif  // BACKEND_H