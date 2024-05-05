
FLAGS := -O3 -I ../../include -march=native -mtune=native -fno-exceptions -fno-omit-frame-pointer ../../merlin.a -lbenchmark

all: umap.out

%.out: %.cpp
	clang++ $< ${FLAGS} -o $@

clean:
	rm -f *.out *.json *.csv
