#include <check.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// #include "../game.h"

// Мок-функция для userInput
// UserAction_t last_action;
// bool last_hold;

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
  CONNECTION,
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

void userInput(UserAction_t action, bool hold) {
  // UserAction_t action = action;
  bool last_hold = hold;
}

UserAction_t controller(int ch) {
  UserAction_t action = Terminate;
  bool hold = false;

  switch (ch) {
    case KEY_LEFT:
      action = Left;
      hold = true;
      break;
    case KEY_RIGHT:
      action = Right;
      hold = true;
      break;
    case KEY_DOWN:
      action = Down;
      hold = true;
      break;
    case ' ':
      action = Action;
      hold = true;
      break;
    case 'q':
      action = Terminate;
      hold = true;
      break;
    case 'p':
      action = Pause;
      hold = true;
      break;
    case '\n':
      action = Start;
      bool hold = true;
      break;
    default:
      action = ch;
      hold = false;
      break;
  }

  if (hold) userInput(action, hold);
  return action;
}

START_TEST(test_controller_left) {
  // updateCurrentState();
  UserAction_t action = controller(KEY_LEFT);
  ck_assert_int_eq(action, Left);
  // ck_assert(last_hold);
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
  UserAction_t action = controller('\n');
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
  // ck_assert(!last_hold);
}
END_TEST

Suite *controller_suite(void) {
  Suite *s = suite_create("Controller");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_controller_left);
  tcase_add_test(tc_core, test_controller_right);
  tcase_add_test(tc_core, test_controller_down);
  tcase_add_test(tc_core, test_controller_action);
  tcase_add_test(tc_core, test_controller_terminate);
  tcase_add_test(tc_core, test_controller_pause);
  tcase_add_test(tc_core, test_controller_start);
  tcase_add_test(tc_core, test_controller_default);

  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  int number_failed;
  Suite *s = controller_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}