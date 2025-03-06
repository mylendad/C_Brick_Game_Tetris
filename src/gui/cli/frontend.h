#ifndef FRONTEND_H
#define FRONTEND_H

#include "../../game.h"

void initColor();
void rendering(GameInfo_t *gameInfo, Cursor *cursor);
void printStart();
void printShapes(Cursor *cursor);
void printNext(GameInfo_t *gameInfo);
void printField(GameInfo_t *gameInfo);
void printTablo();
void printPause();
void printGameOver();

#endif  // FRONTEND_H