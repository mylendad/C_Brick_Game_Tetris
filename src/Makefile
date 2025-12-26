CC = gcc
CFLAGS = -std=c11 -g -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lncurses
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
CLANG_FORMAT = clang-format-18

SRC_FILES = ./brick_game/tetris/backend.c ./gui/cli/*.c ./fsm_loop.c
SRC_TEST = ./tests/*.c
OBJ_FILES = ./*.o

# Настройка библиотек Check для Linux/Mac
ifeq ($(shell uname), Linux)
    CHECK_LIBS = -lcheck -lm -lsubunit
else
    CHECK_LIBS = -lcheck -lm
    CLANG_FORMAT = clang-format
endif

.PHONY: all build/libtetris.a install uninstall clean test gcov_report run style clear

all: clean install test gcov_report

build/libtetris.a: $(SRC_FILES)
	mkdir -p build
	$(CC) $(CFLAGS) -c $(SRC_FILES)
	ar rcs build/libtetris.a $(OBJ_FILES)
	rm -rf $(OBJ_FILES)

install: build/libtetris.a
	$(CC) $(CFLAGS) ./game.c build/libtetris.a $(LDFLAGS) -o build/tetris

uninstall:
	rm -rf build

clean:
	rm -rf gcov report build
	rm -rf *gcda *gcno result.info result_filtered.info tests/brick_game_test tests/brick_game_test.dSYM
	rm -rf *.out highscore.txt ./tests/tests_log.log
	find dvi ! -name 'documentation.tex' -type f -exec rm -f {} +
	rm -f tetris.tar.gz
	rm -rf dvi/documentation

test: build/libtetris.a
	echo 0 > tests/highscore.txt
	$(CC) $(CFLAGS) $(SRC_TEST) build/libtetris.a $(CHECK_LIBS) -lncurses -o tests/brick_game_test
	./tests/brick_game_test

gcov_report: build/libtetris.a
	echo 0 > highscore.txt
	$(CC) $(CFLAGS) $(SRC_TEST) $(SRC_FILES) $(GCOV_FLAGS) $(CHECK_LIBS) -lncurses -o build/brick_game_test
	./build/brick_game_test
	lcov --capture --directory . --output-file coverage.info --rc geninfo_unexecuted_blocks=1
	lcov --extract coverage.info '*brick_game/tetris/backend.c' -o coverage_filtered.info
	genhtml -o report/ coverage_filtered.info
	rm -rf *.gcno *.gcov *.gcda *.info *.a tests/*.o
	open ./report/index.html || xdg-open ./report/index.html

run:
	./build/tetris

style:
	cp ../materials/linters/.clang-format ./
	cp ../materials/linters/.clang-format ./tests/
	cp ../materials/linters/.clang-format ./brick_game/tetris/
	cp ../materials/linters/.clang-format ./gui/cli
	$(CLANG_FORMAT) -i ./tests/*.c ./brick_game/tetris/*.c ./gui/cli/*.c ./brick_game/tetris/*.h ./gui/cli/*.h
	rm .clang-format ./tests/.clang-format ./brick_game/tetris/.clang-format ./gui/cli/.clang-format

clear:
	clear
