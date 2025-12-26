CC = gcc
CFLAGS = -std=c11 -g -lncurses
LDFLAGS = -lcheck -lsubunit -s
CHECK = -lcheck
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
CLANG_FORMAT = clang-format-18
SRC_FILES = ./brick_game/tetris/backend.c ./gui/cli/*.c ./fsm_loop.c
SRC_TEST = ./tests/*.c
TEST_DIR = tests
OBJ_FILES = ./*.o
TBD = tests/build

ifeq ($(shell uname), Linux)
	CHECK += $(LDFLAGS)
else
	CHECK = -lcheck
	CLANG_FORMAT = clang-format
endif

.PHONY: build/libtetris.a install uninstall clean dvi dist test gcov_report valgrind run style clear

all: clean install test gcov_report

build/libtetris.a: $(SRC_FILES)
	mkdir -p build
	$(CC) $(CFLAGS) -c $(SRC_FILES)
	ar rcs build/libtetris.a $(OBJ_FILES)
	rm -rf $(OBJ_FILES)

install: clean build/libtetris.a
	$(CC) $(CFLAGS) ./game.c build/libtetris.a -o build/tetris

uninstall:
	rm -rf build
	
clean:
	rm -rf gcov
	rm -rf report
	rm -rf build
	rm -rf *gcda *gcno result.info result_filtered.info tests/brick_game_test tests/brick_game_test.dSYM
	rm -rf result.info result_filtered.info tests/brick_game_test
	rm -rf *.out
	rm -rf highscore.txt
	rm -rf ./tests/tests_log.log
	rm -f tests/brick_game_test tests/highscore.txt
	find dvi ! -name 'documentation.tex' -type f -exec rm -f {} +
	rm -f tetris.tar.gz
	rm -rf dvi/documentation

dvi:
	latex2html dvi/documentation.tex
	open dvi/documentation/index.html

dist: install
	tar -czvf tetris.tar.gz Makefile brick_game gui tests game.c game.h Diagram.png

test: build/libtetris.a
	echo 0 > tests/highscore.txt
	$(CC) $(CFLAGS) $(SRC_TEST) build/libtetris.a $(CHECK) -o tests/brick_game_test 
	./tests/brick_game_test

gcov_report:
	echo 0 > highscore.txt
	$(CC) $(CFLAGS) $(SRC_TEST) $(SRC_FILES) $(CHECK) $(GCOV_FLAGS) -o build/brick_game_test 
	./build/brick_game_test 
	lcov --capture --directory . --output-file coverage.info --rc geninfo_unexecuted_blocks=1 
	lcov --extract coverage.info '*brick_game/tetris/backend.c' -o coverage_filtered.info 
	genhtml -o report/ coverage_filtered.info
	rm -rf *.gcno *.gcov *.gcda *.info *.a tests/*.o
	open ./report/index.html || xdg-open ./report/index.html

valgrind: 
	valgrind --tool=memcheck --leak-check=yes ./tests/brick_game_test 

run:
	./build/tetris

style:
	cp ../materials/linters/.clang-format ./
	cp ../materials/linters/.clang-format ./tests/
	cp ../materials/linters/.clang-format ./brick_game/tetris/ 
	cp ../materials/linters/.clang-format ./gui/cli
	$(CLANG_FORMAT) -n ./tests/*.c
	$(CLANG_FORMAT) -i ./tests/*.c
	$(CLANG_FORMAT) -n ./tests/*.c
	$(CLANG_FORMAT) -n ./brick_game/tetris/*.c
	$(CLANG_FORMAT) -i ./brick_game/tetris/*.c
	$(CLANG_FORMAT) -n ./brick_game/tetris/*.c
	$(CLANG_FORMAT) -n ./brick_game/tetris/*.h
	$(CLANG_FORMAT) -i ./brick_game/tetris/*.h
	$(CLANG_FORMAT) -n ./brick_game/tetris/*.h
	$(CLANG_FORMAT) -n ./gui/cli/*.c
	$(CLANG_FORMAT) -i ./gui/cli/*.c
	$(CLANG_FORMAT) -n ./gui/cli/*.c
	$(CLANG_FORMAT) -n ./gui/cli/*.h
	$(CLANG_FORMAT) -i ./gui/cli/*.h
	$(CLANG_FORMAT) -n ./gui/cli/*.h
	rm .clang-format
	rm ./tests/.clang-format
	rm ./brick_game/tetris/.clang-format
	rm ./gui/cli/.clang-format

clear:
	clear
