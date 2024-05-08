SIMD_FLAGS := -msse -msse2 -msse3 -msse4.1 -msse4.2 -mavx -mavx2
OPT_FLAGS := -O3 -march=native -mtune=native ${SIMD_FLAGS} -fno-exceptions -fno-omit-frame-pointer
LINK_FLAGS := -lbenchmark ../../merlin.a
INCLUDE_FLAGS := -I ../../include
CXXFLAGS = ${OPT_FLAGS} ${LINK_FLAGS} ${INCLUDE_FLAGS}
CFLAGS := ${OPT_FLAGS} ${INCLUDE_FLAGS}

all: umap.out dmap.out sdmap.out gmap.out

umap.out: umap.cpp
	clang++ $< ${CXXFLAGS} -o $@

dmap.out: dmap.cpp dmap.o
	clang++ $^ ${CXXFLAGS} -o $@

gmap.out: gmap.cpp gmap.o
	clang++ $^ ${CXXFLAGS} -o $@

sdmap.out: sdmap.cpp sdmap.o
	clang++ $^ ${CXXFLAGS} -o $@

dmap.o: dmap/dmap.c
	clang ${CFLAGS} -c -o $@ $^

sdmap.o: sdmap/sdmap.c
	clang ${CFLAGS} -c -o $@ $^

gmap.o: gmap/gmap.c
	clang ${CFLAGS} -c -o $@ $^

clean:
	rm -f *.out *.json *.csv *.o
