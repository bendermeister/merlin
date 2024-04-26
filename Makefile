SOURCES := include/merlin/simd.h

tidy: ${SOURCES}
	clang-tidy --checks=altera*,android*,boost*,bugprone*,cert*,concu*,darwin*,fuchsia*,google* $^
