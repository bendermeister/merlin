HEADERS := include/merlin/simd.h
CC := clang #just for now later support for other compilers will come

# actual library source code
HEADERS := $(wildcard include/merlin/*.h)
STR8_SOURCE := $(wildcard src/str8/*.c)
MERLIN_SOURCE := ${STR8_SOURCE}
MERLIN_OBJECT := $(patsubst %.c, %.o, ${MERLIN_SOURCE})

# benchmarking sources
BENCH_STR8_REPLACE_SOURCE := bench/str8/replace/replace.c
BENCH_SOURCE := ${BENCH_STR8_REPLACE_SOURCE}

# testing sources
TEST_SIMD_SOURCE := test/simd.c
TEST_STR8_SOURCE := test/str8.c
TEST_SOURCE := ${TEST_SIMD_SOURCE} ${TEST_STR8_SOURCE}

# example sources
EXAMPLE_SIMD_SOURCE := $(wildcard example/simd/*.c)
EXAMPLE_STR8_SOURCE := $(wildcard example/str8/*.c)
EXAMPLE_SOURCE := ${EXAMPLE_SIMD_SOURCE} ${EXAMPLE_STR8_SOURCE}

# combination of all sources
SOURCE := ${MERLIN_SOUCE} ${BENCH_SOURCE} ${TEST_SOURCE} ${EXAMPLE_SOURCE}
OBJECT := $(patsubst %.c, %.o, ${SOURCE})

# flags
SSE_FLAGS := -msse -msse2 -msse3 -msse4.1 -msse4.2
AVX_FLAGS := -mavx -mavx2
BASE_FLAGS := -Wall -Wextra -I include ${SSE_FLAGS} ${AVX_FLAGS}
DEBUG_FLAGS := ${BASE_FLAGS} -g -fsanitize=address,leak,undefined
RELEASE_FLAGS := ${BASE_FLAGS} -O3 -march=native -mtune=native -fno-omit-framepointer

#CFLAGS := ${RELEASE_FLAGS}
CFLAGS := ${DEBUG_FLAGS}

# targets
# testing
test/str8.out := test/str8.o
test/simd.out := test/simd.o
TEST_TARGETS := test/str8.out test/simd.out

# examples
example/simd/simple.out := example/simd/simple.o
example/str8/simple.out := example/str8/simple.o
EXAMPLE_TARGETS := example/simd/simple.out example/str8/simple.out

TARGETS := ${TEST_TARGETS} merlin.a ${EXAMPLE_TARGETS}

all: ${TARGETS}

tidy: ${SOURCES} ${HEADERS}
	clang-tidy --checks=altera*,android*,boost*,bugprone*,cert*,concu*,darwin*,fuchsia*,google* $^

# compilation information
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c -o $@ $<

%.out: %.o merlin.a ${HEADERS}
	${CC} ${CFLAGS} ${$@} merlin.a -o $@

merlin.a: ${MERLIN_OBJECT}
	ar r merlin.a $^

clean:
	rm -f ${TARGETS} ${OBJECT} merlin.a
