CXX=mpicxx

all:
	mpicxx -O3 -fno-asynchronous-unwind-tables -mfma -funsafe-math-optimizations -floop-parallelize-all -ftree-parallelize-loops=4 -fprefetch-loop-arrays -fno-tree-vectorize -fprofile-use -w main.cc matmul.cc -o matmul

.PHONY:
	clean

clean:
	rm matmul
