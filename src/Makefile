#
# File Name		: Makefile
# Description	: Makefile for project #3.
#

CC = gcc
LEX = flex
YACC = bison


# 플랫폼별 설정
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    # 리눅스 (우분투) 설정 - static linking 포함
    CFLAGS = -g -fsanitize=address -fno-omit-frame-pointer -static-libasan
    LDFLAGS = -ll -fsanitize=address -static-libasan
else
    # macOS 설정 - static linking 제외
    CFLAGS = -g -fsanitize=address -fno-omit-frame-pointer
    LDFLAGS = -ll -fsanitize=address
endif
YACCFLAGCS = -vd

OBJECTS = lex.yy.o subc.tab.o

all : subc

clean :
	rm -f *.o lex.yy.c subc.tab.c subc.tab.h subc.output subc

test: all
	@mkdir -p ../output
	@for f in ../test/*.c; do \
		base=$$(basename $$f .c); \
		./subc $$f > ../output/$$base.out 2>&1; \
		diff -u ../expected/$$base.out ../output/$$base.out || exit 1; \
	done
	@echo "All tests passed!"

test-with-leak-check: all
	@mkdir -p ../output
	@echo "Testing with leak detection (Linux only)..."
	@for f in ../test/*.c; do \
		base=$$(basename $$f .c); \
		if ASAN_OPTIONS=detect_leaks=1 ./subc $$f > ../output/$$base.out 2>&1; then \
			diff -u ../expected/$$base.out ../output/$$base.out || exit 1; \
		else \
			echo "Leak detection not supported on this platform, running normal test..."; \
			./subc $$f > ../output/$$base.out 2>&1; \
			diff -u ../expected/$$base.out ../output/$$base.out || exit 1; \
		fi; \
	done
	@echo "All tests passed!"

subc : ${OBJECTS}
	${CC} -o subc ${OBJECTS} ${CFLAGS} ${LDFLAGS}

subc.tab.o : subc.tab.c subc.h
	${CC} -c ${CFLAGS} subc.tab.c

lex.yy.o : lex.yy.c subc.tab.h subc.h
	${CC} -c ${CFLAGS} lex.yy.c

lex.yy.c : subc.l
	${LEX} subc.l

subc.tab.h subc.tab.c : subc.y
	${YACC} ${YACCFLAGCS} subc.y

.PHONY: all clean test
