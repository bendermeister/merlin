HEADERS := include/merlin/simd.h
TEST := test/simd.out
SOURCES :=
CC := clang #just for now later support for other compilers will come

FLAGS := -Wall -Wextra -I include -msse -msse2 -msse3 -msse4.1 -msse4.2 -mavx -mavx2 -ffp-exception-behavior=maytrap
DEBUG_FLAGS := ${FLAGS} -g -fsanitize=address,leak,undefined

tidy: ${SOURCES} ${HEADERS}
	clang-tidy --checks=altera*,android*,boost*,bugprone*,cert*,concu*,darwin*,fuchsia*,google* $^

test: ${TEST}

test/%.out: test/%.c
	${CC} ${DEBUG_FLAGS} $< -o $@

clean:
	rm -f ${TEST}
