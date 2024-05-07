SIMD_FLAGS :=  -mavx2 -mavx -msse4.2 -msse3 -msse2 -msse
FAST_FLAGS := -O3 -march=native -mtune=native -fno-omit-frame-pointer -fno-exceptions ${SIMD_FLAGS}
INCLUDE_FLAGS := -I ../../include
LINK_FLAGS := -lbenchmark ../../merlin.a

all: smap.out packed.out

smap.out: smap.cpp
	clang++ $< ${FAST_FLAGS} ${INCLUDE_FLAGS} ${LINK_FLAGS} -o $@

packed.out: packed.cpp packed.o
	clang++ ${SIMD_FLAGS} ${FAST_FLAGS} -o $@ $^ ${LINK_FLAGS} ${INCLUDE_FLAGS}

packed.o: packed/packed.c
	clang ${FAST_FLAGS} ${INCLUDE_FLAGS} -c -o $@ $^

test:
	clang -I ../../include -Wall -Wextra -g -fsanitize=address,undefined packed/packed.c packed/test.c ../../merlin.a

clean:
	rm -f *.out *.json *.csv *.o
