CC=gcc
BUILDFLAGS=-std=c11 -Wall -Wextra -Werror
CFLAGS=$(BUILDFLAGS) -c

TESTFLAGS := -lcheck -lpthread -pthread

TESTFLAGS+=
ifeq ($(shell uname -s),Linux)
	TESTFLAGS += -lm -lrt -lsubunit
endif

GCOVFLAGS = -fprofile-arcs -ftest-coverage

HEADERS=s21_decimal.h s21_decimal_functions/s21_decimal_helpers.h
SOURCES=$(addprefix s21_decimal_functions/,s21_decimal_comparison.c s21_decimal_converters.c s21_decimal_helpers.c s21_decimal_arithmetic.c s21_decimal_another_functions.c)
OBJECTS=$(SOURCES:.c=.o)
TESTS_SOURCES=$(addprefix tests/, main_test.c)
TESTS_FUNCTIONS=$(addprefix tests/, ivarstet_test.c lorenttr_test.c tabathae_test.c wilmerno_test.c)

FILES_TO_CHECK=$(shell find . -name '*.c' && find . -name '*.h' )

MAINFILE=${MAIN}

.PHONY: test rebuild clean stylecheck fmt
.SECONDARY: $(OBJECTS)


all: s21_decimal.a

%.out: %.o $(OBJECTS)
	$(CC) $^ -o $@
	
%.o: %.c
	$(CC) $(CFLAGS) -g $^ -o $@ 

s21_decimal.a:  $(OBJECTS)
	ar rcs s21_decimal.a $^
	ranlib s21_decimal.a

gcov_report: test
	lcov -t test.out -o test.info -c -d . --no-external
	genhtml -o report test.info
	open report/index.html

gcovr_report: test
	mkdir gcovr/
	gcovr --html-details gcovr/report.details.html
	open gcovr/report.details.html

report_nohtml: test
	gcov *gcno > coverage.txt
	open coverage.txt

test: clean test_s21_decimal.a
	$(CC) -g $(TESTS_SOURCES) $(GCOVFLAGS) $(TESTFLAGS) -L. s21_decimal.a -o test.out
	./test.out

test_s21_decimal.a: all_functions
	ar rcs s21_decimal.a *.o
	ranlib s21_decimal.a

all_functions: $(SOURCES)
	$(CC) -g $(CFLAGS) $^ $(GCOVFLAGS)

rebuild: clean all

main: clean s21_decimal.a
	@[[ ! -z "$(MAINFILE)" ]] || (echo 'define MAIN env variable!' && exit 1)
	$(CC) -o main.out -g $(BUILDFLAGS) $(MAINFILE).c s21_decimal.a

clean: 
	@rm -rf  $(OBJECTS)
	@rm -rf report/ gcovr/ s21_decimal.a *.gcda *.gcno *.gcov *.out *.info *.o coverage.txt *.dSYM

.clang-format:
	@cp ../materials/linters/.clang-format .
	
stylecheck: .clang-format
	@clang-format -n $(FILES_TO_CHECK)
	
fmt: .clang-format
	@clang-format -i $(FILES_TO_CHECK)

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem $(FILES_TO_CHECK)
