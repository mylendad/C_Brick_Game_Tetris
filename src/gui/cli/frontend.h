#ifndef FRONTEND_H
#define FRONTEND_H

#include "../../brick_game/tetris/game.h"

void initColor();
void printFrontend(GameInfo_t *gameInfo, Cursor_s *cursor);
void printStart();
void printShapes(Cursor_s *cursor);
void printNext(GameInfo_t *gameInfo);
void printField(GameInfo_t *gameInfo);
void printTablo();
void printPause();
void printGameOver();

#endif  // FRONTEND_H