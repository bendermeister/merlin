SOURCES := include/merlin/simd.h

tidy: ${SOURCES}
	clang-tidy --checks=altera*,android*,boost*,bugprone*,cert*,concu*,cpp*,darwin*,fuchsia*,google*,hicpp* $^
