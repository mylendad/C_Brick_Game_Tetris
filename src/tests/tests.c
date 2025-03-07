#include <check.h>

#include "../game.h"

#define TEST_HIGHSCORE_FILE "highscore.txt"

START_TEST(test_load_highscore) {
  FILE *f = fopen(TEST_HIGHSCORE_FILE, "w");
  fprintf(f, "1500");
  fclose(f);

  int result = loadHighScore();
  ck_assert_int_eq(result, 1500);
  remove(TEST_HIGHSCORE_FILE);
}
END_TEST

START_TEST(test_save_highscore) {
  saveHighScore(2000);
  int result = loadHighScore();
  ck_assert_int_eq(result, 2000);
  remove(TEST_HIGHSCORE_FILE);
}
END_TEST

START_TEST(test_initialize_game) {
  GameInfo_t gi;
  Cursor c;
  initializeGame(&gi, &c);

  ck_assert_int_eq(gi.score, 0);
  ck_assert_int_eq(gi.level, 1);
  ck_assert_int_eq(gi.pause, 0);
  ck_assert_ptr_nonnull(gi.field);
  ck_assert_ptr_nonnull(gi.next);

  for (int i = 0; i < HEIGHT; i++) free(gi.field[i]);
  free(gi.field);
  for (int i = 0; i < 4; i++) free(gi.next[i]);
  free(gi.next);
}
END_TEST

START_TEST(test_merge_cursor) {
  GameInfo_t gi = {0};
  Cursor c = {0};

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
  }

  c.cursorX = 3;
  c.cursorY = 0;
  int shape[4][4] = {{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}};
  memcpy(c.shape, shape, sizeof(shape));

  mergeCursor(&gi, &c);

  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(gi.field[i][3], 1);
  }

  for (int i = 0; i < HEIGHT; i++) free(gi.field[i]);
  free(gi.field);
}
END_TEST

START_TEST(test_check_side_collisions) {
  GameInfo_t gi = {0};
  Cursor c = {0};

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
  }
  gi.field[5][5] = 1;

  int shape[4][4] = {{0}};
  shape[3][3] = 1;

  c.cursorX = WIDTH - 1;
  c.cursorY = 0;
  ck_assert(checkSide(&gi, c.cursorX, c.cursorY, shape));

  c.cursorX = 5;
  c.cursorY = HEIGHT - 1;
  ck_assert(checkSide(&gi, c.cursorX, c.cursorY, shape));

  for (int i = 0; i < HEIGHT; i++) free(gi.field[i]);
  free(gi.field);
}
END_TEST

START_TEST(test_clear_lines) {
  GameInfo_t gi = {0};
  gi.field = calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = calloc(WIDTH, sizeof(int));
    if (i == HEIGHT - 1) memset(gi.field[i], 1, WIDTH * sizeof(int));
  }

  int cleared = clearLines(&gi);
  ck_assert_int_eq(cleared, 1);
  ck_assert_int_eq(gi.score, 100);

  for (int i = 0; i < HEIGHT; i++) free(gi.field[i]);
  free(gi.field);
}
END_TEST

START_TEST(test_shape_rotationL_collision) {
  GameInfo_t gi;
  Cursor c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursorX = 9;
  c.cursorY = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};
  c.type = 'L';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  rotateShape(&gi, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_shape_rotationL) {
  GameInfo_t gi;
  Cursor c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursorX = WIDTH / 2 - 2;
  c.cursorY = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};
  c.type = 'L';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  rotateShape(&gi, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_shape_rotationJ) {
  GameInfo_t gi;
  Cursor c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursorX = WIDTH / 2 - 2;
  c.cursorY = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'J';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  rotateShape(&gi, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_shape_rotationI_collision) {
  GameInfo_t gi;
  Cursor c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursorX = WIDTH / 2 - 2;
  c.cursorY = 5;
  c.rotationPosition == 0;
  int shapeL[4][4] = {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  c.type = 'I';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  rotateShape(&gi, &c);
  rotateShape(&gi, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_shape_rotationI) {
  GameInfo_t gi;
  Cursor c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursorX = WIDTH / 2 - 2;
  c.cursorY = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}};
  c.type = 'I';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  rotateShape(&gi, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_shape_rotationO) {
  GameInfo_t gi;
  Cursor c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursorX = WIDTH / 2 - 2;
  c.cursorY = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}};
  c.type = 'O';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  rotateShape(&gi, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_shape_rotationT) {
  GameInfo_t gi;
  Cursor c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursorX = WIDTH / 2 - 2;
  c.cursorY = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}};
  c.type = 'T';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  rotateShape(&gi, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_shape_rotationS_collision) {
  GameInfo_t gi;
  Cursor c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursorX = 9;
  c.cursorY = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'S';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  rotateShape(&gi, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_shape_rotationS) {
  GameInfo_t gi;
  Cursor c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursorX = WIDTH / 2 - 2;
  c.cursorY = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'S';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  rotateShape(&gi, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_shape_rotationZ) {
  GameInfo_t gi;
  Cursor c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursorX = WIDTH / 2 - 2;
  c.cursorY = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}};
  c.type = 'Z';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }

  rotateShape(&gi, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 0, 0, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_create_shape) {
  GameInfo_t gi;
  Cursor c;
  initializeGame(&gi, &c);
  createShape(&gi, &c, 3, 1);
  int has_blocks = 0;

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      if (gi.next[i][j]) has_blocks++;

  ck_assert_int_gt(has_blocks, 0);

  for (int i = 0; i < HEIGHT; i++) free(gi.field[i]);
  free(gi.field);
  for (int i = 0; i < 4; i++) free(gi.next[i]);
  free(gi.next);
}
END_TEST

START_TEST(test_print_shapesT) {
  Cursor cursor = {0};
  cursor.type = 'T';
  cursor.cursorX = 3;
  cursor.cursorY = 4;
  cursor.shape[1][1] = 1;

  printShapes(&cursor);

  ck_assert_int_eq(cursor.shape[1][1], 1);
}
END_TEST

START_TEST(test_print_shapesI) {
  Cursor cursor = {0};
  cursor.type = 'I';
  cursor.cursorX = 3;
  cursor.cursorY = 4;
  cursor.shape[1][1] = 1;

  printShapes(&cursor);

  ck_assert_int_eq(cursor.shape[1][1], 1);
}
END_TEST

START_TEST(test_print_shapesO) {
  Cursor cursor = {0};
  cursor.type = 'O';
  cursor.cursorX = 3;
  cursor.cursorY = 4;
  cursor.shape[1][1] = 1;
  printShapes(&cursor);

  ck_assert_int_eq(cursor.shape[1][1], 1);
}
END_TEST

START_TEST(test_print_shapesS) {
  Cursor cursor = {0};
  cursor.type = 'S';
  cursor.cursorX = 3;
  cursor.cursorY = 4;
  cursor.shape[1][1] = 1;

  printShapes(&cursor);

  ck_assert_int_eq(cursor.shape[1][1], 1);
}
END_TEST

START_TEST(test_print_shapesZ) {
  Cursor cursor = {0};
  cursor.type = 'Z';
  cursor.cursorX = 3;
  cursor.cursorY = 4;
  cursor.shape[1][1] = 1;

  printShapes(&cursor);

  ck_assert_int_eq(cursor.shape[1][1], 1);
}
END_TEST

START_TEST(test_print_shapesL) {
  Cursor cursor = {0};
  cursor.type = 'L';
  cursor.cursorX = 3;
  cursor.cursorY = 4;
  cursor.shape[1][1] = 1;

  printShapes(&cursor);

  ck_assert_int_eq(cursor.shape[1][1], 1);
}
END_TEST

START_TEST(test_print_shapesJ) {
  Cursor cursor = {0};
  cursor.type = 'J';
  cursor.cursorX = 3;
  cursor.cursorY = 4;
  cursor.shape[1][1] = 1;

  printShapes(&cursor);

  ck_assert_int_eq(cursor.shape[1][1], 1);
}
END_TEST

START_TEST(test_print_tablo) {
  GameInfo_t gameInfo = {0};
  gameInfo.score = 1500;
  gameInfo.high_score = 5000;
  gameInfo.level = 3;

  printTablo(&gameInfo);

  ck_assert_int_eq(gameInfo.score, 1500);
  ck_assert_int_eq(gameInfo.high_score, 5000);
  ck_assert_int_eq(gameInfo.level, 3);
}
END_TEST

START_TEST(test_print_pause) {
  GameInfo_t gameInfo = {0};
  gameInfo.pause = 1;

  printPause();

  ck_assert_int_eq(gameInfo.pause, 1);
}
END_TEST

START_TEST(test_controller_left) {
  UserAction_t action = controller(KEY_LEFT);
  ck_assert_int_eq(action, Left);
}
END_TEST

START_TEST(test_controller_right) {
  UserAction_t action = controller(KEY_RIGHT);
  ck_assert_int_eq(action, Right);
}
END_TEST

START_TEST(test_controller_down) {
  UserAction_t action = controller(KEY_DOWN);
  ck_assert_int_eq(action, Down);
}
END_TEST

START_TEST(test_controller_action) {
  UserAction_t action = controller(' ');
  ck_assert_int_eq(action, Action);
}
END_TEST

START_TEST(test_controller_terminate) {
  UserAction_t action = controller('q');
  ck_assert_int_eq(action, Terminate);
}
END_TEST

START_TEST(test_controller_pause) {
  UserAction_t action = controller('p');
  ck_assert_int_eq(action, Pause);
}
END_TEST

START_TEST(test_controller_start) {
  UserAction_t action = controller(KEY_ENTER);
  ck_assert_int_eq(action, Start);
}
END_TEST

START_TEST(test_controller_default) {
  UserAction_t action = controller('g');
  ck_assert_int_ne(action, Left);
  ck_assert_int_ne(action, Right);
  ck_assert_int_ne(action, Down);
  ck_assert_int_ne(action, Action);
  ck_assert_int_ne(action, Terminate);
  ck_assert_int_ne(action, Pause);
  ck_assert_int_ne(action, Start);
}
END_TEST

START_TEST(test_jltShapeRotate) {
  GameInfo_t gameInfo;
  Cursor cursor;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }
  cursor.cursorX = WIDTH / 2 - 2;
  cursor.cursorY = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      cursor.shape[i][j] = shapeL[i][j];
    }
  }

  jltShapeRotate(&gameInfo, &cursor);
  int expectedShapeAfterRotation[4][4] =

      {{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], cursor.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_checkSide_no_collision) {
  GameInfo_t gameInfo;
  Cursor cursor;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }

  int cursora[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      cursor.shape[i][j] = cursora[i][j];
    }
  }

  ck_assert_int_eq(checkSide(&gameInfo, 3, 2, cursor.shape), false);
}

START_TEST(test_checkSide_collision_with_field) {
  GameInfo_t gameInfo;
  Cursor cursor;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }
  cursor.cursorX = WIDTH - 1;
  cursor.cursorY = 5;
  int cursora[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      cursor.shape[i][j] = cursora[i][j];
    }
  }
  ck_assert_int_eq(
      checkSide(&gameInfo, cursor.cursorX, cursor.cursorY, cursor.shape), true);
}

START_TEST(test_checkSide_right) {
  GameInfo_t gameInfo;
  Cursor cursor;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }
  cursor.cursorX = 10;
  cursor.cursorY = 5;
  int cursora[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      cursor.shape[i][j] = cursora[i][j];
    }
  }
  ck_assert_int_eq(
      checkSide(&gameInfo, cursor.cursorX, cursor.cursorY, cursor.shape), true);
}

START_TEST(test_checkSide_bottom) {
  GameInfo_t gameInfo;
  Cursor cursor;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }
  cursor.cursorX = WIDTH - 1;
  cursor.cursorY = 19;
  int cursora[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      cursor.shape[i][j] = cursora[i][j];
    }
  }

  ck_assert_int_eq(
      checkSide(&gameInfo, cursor.cursorX, cursor.cursorY, cursor.shape), true);
}

START_TEST(test_checkSide_no_collision_2) {
  GameInfo_t gameInfo;
  Cursor cursor;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }
  cursor.cursorX = WIDTH - 1;
  cursor.cursorY = 18;
  int cursora[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      cursor.shape[i][j] = cursora[i][j];
    }
  }

  ck_assert_int_eq(
      checkSide(&gameInfo, cursor.cursorX, cursor.cursorY, cursor.shape), true);
}

Suite *tetris_suite(void) {
  Suite *s;
  TCase *tc_core, *tc_logic, *tc_input, *tc_front;

  s = suite_create("Tetris");

  tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_load_highscore);
  tcase_add_test(tc_core, test_save_highscore);
  tcase_add_test(tc_core, test_initialize_game);
  suite_add_tcase(s, tc_core);

  tc_logic = tcase_create("Game Logic");
  tcase_add_test(tc_logic, test_merge_cursor);
  tcase_add_test(tc_logic, test_check_side_collisions);
  tcase_add_test(tc_logic, test_clear_lines);
  tcase_add_test(tc_logic, test_shape_rotationT);
  tcase_add_test(tc_logic, test_shape_rotationS_collision);
  tcase_add_test(tc_logic, test_shape_rotationI);
  tcase_add_test(tc_logic, test_shape_rotationI_collision);
  tcase_add_test(tc_logic, test_shape_rotationO);
  tcase_add_test(tc_logic, test_shape_rotationJ);
  tcase_add_test(tc_logic, test_shape_rotationL);
  tcase_add_test(tc_logic, test_shape_rotationL_collision);
  tcase_add_test(tc_logic, test_shape_rotationS);
  tcase_add_test(tc_logic, test_shape_rotationZ);
  tcase_add_test(tc_core, test_jltShapeRotate);
  tcase_add_test(tc_core, test_checkSide_no_collision);
  tcase_add_test(tc_core, test_checkSide_collision_with_field);
  tcase_add_test(tc_core, test_checkSide_right);
  tcase_add_test(tc_core, test_checkSide_bottom);
  tcase_add_test(tc_core, test_checkSide_no_collision_2);
  tcase_add_test(tc_logic, test_create_shape);
  suite_add_tcase(s, tc_logic);

  tc_front = tcase_create("Gui Logic");
  tcase_add_test(tc_front, test_print_shapesT);
  tcase_add_test(tc_front, test_print_shapesI);
  tcase_add_test(tc_front, test_print_shapesO);
  tcase_add_test(tc_front, test_print_shapesL);
  tcase_add_test(tc_front, test_print_shapesJ);
  tcase_add_test(tc_front, test_print_shapesS);
  tcase_add_test(tc_front, test_print_shapesZ);
  tcase_add_test(tc_front, test_print_tablo);
  tcase_add_test(tc_front, test_print_pause);
  suite_add_tcase(s, tc_front);

  tc_input = tcase_create("Input Handling");
  tcase_add_test(tc_core, test_controller_left);
  tcase_add_test(tc_core, test_controller_right);
  tcase_add_test(tc_core, test_controller_down);
  tcase_add_test(tc_core, test_controller_action);
  tcase_add_test(tc_core, test_controller_terminate);
  tcase_add_test(tc_core, test_controller_pause);
  tcase_add_test(tc_core, test_controller_start);
  tcase_add_test(tc_core, test_controller_default);
  suite_add_tcase(s, tc_input);
  ;

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = tetris_suite();
  sr = srunner_create(s);

  srunner_set_log(sr, "tests/tests_log.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
