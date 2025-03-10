#include <check.h>

#include "../brick_game/tetris/backend.h"
#include "../gui/cli/frontend.h"

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

START_TEST(test_timerFunc) {
  GameInfo_t gi;
  Cursor_s c;
  struct timespec last_move_time;
  struct timespec current_time;

  gi.speed = 100;
  gi.pause = 0;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = 0;
  c.cursor_y = 0;
  c.game_over = false;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'J';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 0;

  clock_gettime(CLOCK_MONOTONIC, &last_move_time);

  current_time.tv_sec = last_move_time.tv_sec;
  current_time.tv_nsec = last_move_time.tv_nsec + 101 * 1000000;

  timerFunc(&gi, &c, &last_move_time, &current_time, SHIFT);

  ck_assert_int_eq(c.cursor_y, 1);
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
  Cursor_s c;
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
  Cursor_s c = {0};

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
  }

  c.cursor_x = 3;
  c.cursor_y = 0;
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
  Cursor_s c = {0};

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
  }
  gi.field[5][5] = 1;

  int shape[4][4] = {{0}};
  shape[3][3] = 1;

  c.cursor_x = WIDTH - 1;
  c.cursor_y = 0;
  ck_assert(checkSide(&gi, c.cursor_x, c.cursor_y, shape));

  c.cursor_x = 5;
  c.cursor_y = HEIGHT - 1;
  ck_assert(checkSide(&gi, c.cursor_x, c.cursor_y, shape));

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
  Cursor_s c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = 9;
  c.cursor_y = 5;
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
  Cursor_s c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 5;
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
  Cursor_s c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 5;
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
  Cursor_s c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 5;
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
  Cursor_s c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 5;
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
  Cursor_s c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 5;
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
  Cursor_s c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 5;
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
  Cursor_s c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = 9;
  c.cursor_y = 5;
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
  Cursor_s c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 5;
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
  Cursor_s c;
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 5;
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
  Cursor_s c;
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

START_TEST(test_spawn_shape_success) {
  GameInfo_t gi;
  Cursor_s c;
  gi.next = (int **)calloc(4, sizeof(int *));
  for (int i = 0; i < 4; i++) {
    gi.next[i] = (int *)calloc(4, sizeof(int));
    for (int j = 0; j < 4; j++) {
      gi.next[i][j] = 0;
    }
  }
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }
  for (int i = 0; i < 4; i++) {
    gi.field[0][WIDTH / 2 - 2 + i] = 1;
  }
  bool result = spawnShape(&gi, &c);
  for (int i = 0; i < 4; i++) {
    free(gi.field[i]);
  }
  free(gi.field);
  for (int i = 0; i < 4; i++) {
    free(gi.next[i]);
  }
  free(gi.next);
  ck_assert_int_eq(result, 0);
  ck_assert_int_eq(c.cursor_x, WIDTH / 2 - 2);
  ck_assert_int_eq(c.cursor_y, 0);
}
END_TEST

START_TEST(test_spawn_shape_fail) {
  GameInfo_t gi;
  Cursor_s c;
  gi.next = (int **)calloc(4, sizeof(int *));
  for (int i = 0; i < 4; i++) {
    gi.next[i] = (int *)calloc(4, sizeof(int));
    for (int j = 0; j < 4; j++) {
      gi.next[i][j] = 0;
    }
  }
  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }
  bool result = spawnShape(&gi, &c);
  for (int i = 0; i < 4; i++) {
    free(gi.field[i]);
  }
  free(gi.field);
  for (int i = 0; i < 4; i++) {
    free(gi.next[i]);
  }
  free(gi.next);
  ck_assert_int_eq(result, 1);
  ck_assert_int_eq(c.cursor_x, WIDTH / 2 - 2);
  ck_assert_int_eq(c.cursor_y, 0);
}
END_TEST

START_TEST(test_controller_left) {
  UserAction_t action = controllerInput(KEY_LEFT);
  ck_assert_int_eq(action, Left);
}
END_TEST

START_TEST(test_controller_right) {
  UserAction_t action = controllerInput(KEY_RIGHT);
  ck_assert_int_eq(action, Right);
}
END_TEST

START_TEST(test_controller_down) {
  UserAction_t action = controllerInput(KEY_DOWN);
  ck_assert_int_eq(action, Down);
}
END_TEST

START_TEST(test_controller_action) {
  UserAction_t action = controllerInput(' ');
  ck_assert_int_eq(action, Action);
}
END_TEST

START_TEST(test_controller_terminate) {
  UserAction_t action = controllerInput('q');
  ck_assert_int_eq(action, Terminate);
}
END_TEST

START_TEST(test_controller_pause) {
  UserAction_t action = controllerInput('p');
  ck_assert_int_eq(action, Pause);
}
END_TEST

START_TEST(test_controller_start) {
  UserAction_t action = controllerInput(KEY_ENTER);
  ck_assert_int_eq(action, Start);
}
END_TEST

START_TEST(test_controller_default) {
  UserAction_t action = controllerInput('g');
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
  Cursor_s c;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }
  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 5;
  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }

  jltShapeRotate(&gameInfo, &c);
  int expectedShapeAfterRotation[4][4] =

      {{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ck_assert_int_eq(expectedShapeAfterRotation[i][j], c.shape[i][j]);
    }
  }
}
END_TEST

START_TEST(test_checkSide_no_collision) {
  GameInfo_t gameInfo;
  Cursor_s c;

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
      c.shape[i][j] = cursora[i][j];
    }
  }

  ck_assert_int_eq(checkSide(&gameInfo, 3, 2, c.shape), false);
}
END_TEST

START_TEST(test_checkSide_collision_with_field) {
  GameInfo_t gameInfo;
  Cursor_s c;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }
  c.cursor_x = WIDTH - 1;
  c.cursor_y = 5;
  int cursora[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = cursora[i][j];
    }
  }
  ck_assert_int_eq(checkSide(&gameInfo, c.cursor_x, c.cursor_y, c.shape), true);
}
END_TEST

START_TEST(test_checkSide_right) {
  GameInfo_t gameInfo;
  Cursor_s c;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }
  c.cursor_x = 10;
  c.cursor_y = 5;
  int cursora[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = cursora[i][j];
    }
  }
  ck_assert_int_eq(checkSide(&gameInfo, c.cursor_x, c.cursor_y, c.shape), true);
}
END_TEST

START_TEST(test_checkSide_bottom) {
  GameInfo_t gameInfo;
  Cursor_s c;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }
  c.cursor_x = WIDTH - 1;
  c.cursor_y = 19;
  int cursora[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = cursora[i][j];
    }
  }

  ck_assert_int_eq(checkSide(&gameInfo, c.cursor_x, c.cursor_y, c.shape), true);
}
END_TEST

START_TEST(test_checkSide_no_collision_2) {
  GameInfo_t gameInfo;
  Cursor_s c;

  gameInfo.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gameInfo.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gameInfo.field[i][j] = 0;
    }
  }
  c.cursor_x = WIDTH - 1;
  c.cursor_y = 18;
  int cursora[4][4] = {{0, 0, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 0}};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = cursora[i][j];
    }
  }

  ck_assert_int_eq(checkSide(&gameInfo, c.cursor_x, c.cursor_y, c.shape), true);
}
END_TEST

START_TEST(test_move_left) {
  static GameInfo_t gi;
  static Cursor_s c;
  InputContext_s *ctx = getInputContext();
  ctx->game_info = &gi;
  ctx->cursor = &c;

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'J';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 0;

  int initial_x = c.cursor_x;
  userInput(Left, true);
  ck_assert_int_eq(c.cursor_x, initial_x - 1);
}
END_TEST

// Тест на перемещение вправо
START_TEST(test_move_right) {
  static GameInfo_t gi;
  static Cursor_s c;
  InputContext_s *ctx = getInputContext();
  ctx->game_info = &gi;
  ctx->cursor = &c;

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'J';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 0;

  int initial_x = c.cursor_x;

  userInput(Right, true);
  ck_assert_int_eq(c.cursor_x, initial_x + 1);
}
END_TEST

// // Тест на падение вниз
START_TEST(test_move_down) {
  static GameInfo_t gi;
  static Cursor_s c;
  InputContext_s *ctx = getInputContext();
  ctx->game_info = &gi;
  ctx->cursor = &c;

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'J';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 0;

  int initial_y = c.cursor_y;

  userInput(Down, true);
  ck_assert(c.cursor_y > initial_y);
}
END_TEST

// // Тест на вращение фигуры
START_TEST(test_rotate) {
  static GameInfo_t gi;
  static Cursor_s c;
  InputContext_s *ctx = getInputContext();
  ctx->game_info = &gi;
  ctx->cursor = &c;

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'J';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 0;

  char initial_type = c.type;

  userInput(Action, true);
  ck_assert_int_eq(c.type, initial_type);
}
END_TEST

// // Тест на старт игры
START_TEST(test_start_game) {
  static GameInfo_t gi;
  static Cursor_s c;
  InputContext_s *ctx = getInputContext();
  ctx->game_info = &gi;
  ctx->cursor = &c;

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'J';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 0;

  c.cursor_x = 5;
  c.cursor_y = 5;

  userInput(Start, true);
  ck_assert_int_eq(c.cursor_x, WIDTH / 2 - 2);
  ck_assert_int_eq(c.cursor_y, 0);
}
END_TEST

// // Тест на паузу
START_TEST(test_pause_game) {
  static GameInfo_t gi;
  static Cursor_s c;
  InputContext_s *ctx = getInputContext();
  ctx->game_info = &gi;
  ctx->cursor = &c;

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'J';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 0;

  gi.pause = 0;

  userInput(Pause, true);
  ck_assert_int_eq(gi.pause, 1);

  userInput(Pause, true);
  ck_assert_int_eq(gi.pause, 0);
}
END_TEST

// // Тест на завершение игры
START_TEST(test_terminate_game) {
  static GameInfo_t gi;
  static Cursor_s c;
  InputContext_s *ctx = getInputContext();
  ctx->game_info = &gi;
  ctx->cursor = &c;

  gi.field = (int **)calloc(HEIGHT, sizeof(int *));
  for (int i = 0; i < HEIGHT; i++) {
    gi.field[i] = (int *)calloc(WIDTH, sizeof(int));
    for (int j = 0; j < WIDTH; j++) {
      gi.field[i][j] = 0;
    }
  }

  int shapeL[4][4] = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
  c.type = 'J';
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      c.shape[i][j] = shapeL[i][j];
    }
  }
  c.cursor_x = WIDTH / 2 - 2;
  c.cursor_y = 0;

  c.quit = 0;

  userInput(Terminate, true);
  ck_assert_int_eq(c.quit, 1);
}
END_TEST

START_TEST(test_waitStart_not_started) {
  Cursor_s cursor;
  cursor.start = false;
  int mock_getch = '\n';

  int ch = waitStart(&cursor, mock_getch);
  ck_assert_int_eq(ch, '\n');
}
END_TEST

Suite *tetris_suite(void) {
  Suite *s;
  TCase *tc_core, *tc_logic, *tc_input;

  s = suite_create("Tetris");

  tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_load_highscore);
  tcase_add_test(tc_core, test_timerFunc);
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
  tcase_add_test(tc_core, test_spawn_shape_success);
  tcase_add_test(tc_core, test_spawn_shape_fail);
  suite_add_tcase(s, tc_logic);

  tc_input = tcase_create("Input Handling");
  tcase_add_test(tc_core, test_controller_left);
  tcase_add_test(tc_core, test_controller_right);
  tcase_add_test(tc_core, test_controller_down);
  tcase_add_test(tc_core, test_controller_action);
  tcase_add_test(tc_core, test_controller_terminate);
  tcase_add_test(tc_core, test_controller_pause);
  tcase_add_test(tc_core, test_controller_start);
  tcase_add_test(tc_core, test_controller_default);
  tcase_add_test(tc_core, test_move_left);
  tcase_add_test(tc_core, test_move_right);
  tcase_add_test(tc_core, test_move_down);
  tcase_add_test(tc_core, test_rotate);
  tcase_add_test(tc_core, test_start_game);
  tcase_add_test(tc_core, test_pause_game);
  tcase_add_test(tc_core, test_terminate_game);
  tcase_add_test(tc_core, test_waitStart_not_started);
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
