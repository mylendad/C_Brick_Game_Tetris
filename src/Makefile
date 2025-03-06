CC = gcc
CFLAGS = -std=c11  -g -lncurses
LDFLAGS = -lcheck -lsubunit -lm
CHECK = -lcheck
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
CLANG_FORMAT = clang-format-18
SRC_FILES = ./brick_game/tetris/*.c ./gui/cli/*.c
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

.PHONY: install uninstall clean dvi dist test gcov_report

all: install test clean

build/libtetris.a: $(SRC_FILES)
	mkdir -p build
	$(CC) $(CFLAGS) -c $(SRC_FILES)
	ar rcs build/libtetris.a $(OBJ_FILES)
	rm -rf $(OBJ_FILES)

install: clean build/libtetris.a
	echo 0 > build/highscore.txt
	$(CC) $(CFLAGS) game.c build/libtetris.a -o build/tetris

uninstall:
	rm -rf build

run: clean install
	./build/tetris

test: clean build/libtetris.a
	echo 0 > tests/highscore.txt
	$(CC) $(CFLAGS) $(SRC_TEST) build/libtetris.a $(CHECK) -o tests/brick_game_test 
	./tests/brick_game_test 


valgrind: 
	valgrind --tool=memcheck --leak-check=yes ./s21_test

gcov_report:
	echo 0 > highscore.txt
	$(CC) $(CFLAGS) $(SRC_TEST) $(SRC_FILES) $(CHECK) $(GCOV_FLAGS) -o brick_game_test 
	./brick_game_test 
	lcov --capture --directory . --output-file coverage.info --rc geninfo_unexecuted_blocks=1
	genhtml -o report/ coverage.info
	rm -rf *.gcno *.gcov *.gcda *.info *.a tests/*.o
	open ./report/index.html || xdg-open ./report/index.html
	
gcov_report_common:
	mkdir -p tests/build
	$(CC) --coverage -fPIC -O0 $(SRC_FILES) $(TEST_DIR)/*.c -o $(TBD)/test_cov_runner $(LDFLAGS)
	./$(TBD)/test_cov_runner

gcov_report_console: gcov_report_common
	gcov -H $(TBD)/test_cov_runner-s21*.gcno

clean:
	rm -rf gcov
	rm -rf report
	rm -rf *gcda *gcno result.info result_filtered.info tests/brick_game_test
	rm -rf result.info result_filtered.info tests/brick_game_test
	rm -rf *.out
	rm -rf rm ./tests/tests_log.log
	rm -rf test

dvi:
	xelatex -output-directory=dvi dvi/documentation.tex

dist: install
	tar -czvf tetris.tar.gz Makefile brick_game gui testы game.c game.h Diagram.pdf

style:
	cp ../materials/linters/.clang-format ./
	cp ../materials/linters/.clang-format ./tests/
	$(CLANG_FORMAT) -n ./tests/*.c
	$(CLANG_FORMAT) -i ./tests/*.c
	$(CLANG_FORMAT) -n ./tests/*.c
	$(CLANG_FORMAT) -n ./*.c
	$(CLANG_FORMAT) -i ./*.c
	$(CLANG_FORMAT) -n ./*.c
	$(CLANG_FORMAT) -n ./*.h
	$(CLANG_FORMAT) -i ./*.h
	$(CLANG_FORMAT) -n ./*.h
	rm .clang-format
	rm ./tests/.clang-format

clear:
	clear
