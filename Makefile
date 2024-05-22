HEADERS := include/merlin/simd.h
CC := clang #just for now later support for other compilers will come

# actual library source code
HEADERS := $(wildcard include/merlin/*.h)
ALOCTR_SOURCE := $(wildcard src/aloctr/*.c)
UMAP_SOURCE := $(wildcard src/umap/*.c)
MERLIN_SOURCE := ${ALOCTR_SOURCE} ${UMAP_SOURCE}
MERLIN_OBJECT := $(patsubst %.c, %.o, ${MERLIN_SOURCE})

# testing sources
TEST_SIMD_SOURCE := test/simd.c
TEST_UMAP_SOURCE := test/umap.c
TEST_SOURCE := ${TEST_SIMD_SOURCE} 

# example sources
EXAMPLE_SIMD_SOURCE := $(wildcard example/simd/*.c)
EXAMPLE_ALOCTR_SOURCE := $(wildcard example/aloctr/*.c)
EXAMPLE_SOURCE := ${EXAMPLE_SIMD_SOURCE} ${EXAMPLE_ALOCTR_SOURCE}

# combination of all sources
SOURCE := ${MERLIN_SOURCE} ${BENCH_SOURCE} ${TEST_SOURCE} ${EXAMPLE_SOURCE}
OBJECT := $(patsubst %.c, %.o, ${SOURCE})

# flags
SSE_FLAGS := -msse -msse2 -msse3 -msse4.1 -msse4.2
AVX_FLAGS := -mavx -mavx2
BASE_FLAGS := -Wall -Wextra -I include ${SSE_FLAGS} ${AVX_FLAGS}
DEBUG_FLAGS := ${BASE_FLAGS} -g -fsanitize=address,undefined -DMRLN_ASSUME_TRAPS
RELEASE_FLAGS := ${BASE_FLAGS} -O3 -march=native -mtune=native -fno-omit-frame-pointer

# CFLAGS := ${RELEASE_FLAGS}
CFLAGS := ${DEBUG_FLAGS}

# targets
# testing
test/simd.out := test/simd.o
test/umap.out := test/umap.o
TEST_TARGETS := test/simd.out test/umap.out

# examples
example/simd/simple.out := example/simd/simple.o
example/aloctr/simple.out := example/aloctr/simple.o
EXAMPLE_TARGETS := example/simd/simple.out example/aloctr/simple.out

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
