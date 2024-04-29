HEADERS := include/merlin/simd.h
TEST := test/simd.out test/str8.out
SOURCES :=
CC := clang #just for now later support for other compilers will come

FLAGS := -Wall -Wextra -I include -msse -msse2 -msse3 -msse4.1 -msse4.2 -mavx -mavx2 -I src/include
DEBUG_FLAGS := ${FLAGS} -g -fsanitize=address,leak,undefined

tidy: ${SOURCES} ${HEADERS}
	clang-tidy --checks=altera*,android*,boost*,bugprone*,cert*,concu*,darwin*,fuchsia*,google* $^

test: ${TEST}

test/%.out: test/%.c
	${CC} ${DEBUG_FLAGS} $< -o $@

test/str8.out: test/str8.c src/str8.c include/merlin/str8.h
	${CC} ${DEBUG_FLAGS} src/str8.c $< -o $@

BENCH := bench/str8/replace.out

BENCH_FLAGS := ${FLAGS} -O3 -march=native -mtune=native
#BENCH_FLAGS := ${DEBUG_FLAGS}

bench: ${BENCH}

bench/str8/%.out: bench/str8/%.c src/str8.c
	${CC} ${BENCH_FLAGS} $^ -o $@

clean:
	rm -f ${TEST} ${BENCH}
