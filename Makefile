HEADERS := include/merlin/simd.h
TEST := test/simd.out
SOURCES :=
CC := clang #just for now later support for other compilers will come

FLAGS := -Wall -Wextra -I include
DEBUG_FLAGS := ${FLAGS} -g3 -fsanitize=address,leak,undefined,unreachable

tidy: ${SOURCES} ${HEADERS}
	clang-tidy --checks=altera*,android*,boost*,bugprone*,cert*,concu*,darwin*,fuchsia*,google* $^

test: ${TEST}

test/%.out: test/%.c
	${CC} ${DEBUG_FLAGS} $< -o $@
