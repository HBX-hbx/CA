#include <sched.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

using namespace std;

int test_ass = 1; // test associativity
const int INT_ARRAY_SIZE = (1 << 14); // 16K * 4B = 64KB = 2 * cache_size
const int ROUNDS = (1 << 30); // the number of rounds

int main() {
	// bind the cpu core
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(3, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        perror("sched_setaffinity");
    }
    // More code follows

	clock_t start, finish;
	int *a = new int[INT_ARRAY_SIZE];
	register int i, j = INT_ARRAY_SIZE - 1; // i: loop index, j: mod variable

	for (int k = 0; k < 8; ++k) {
		int blocks_num = test_ass * 4; // eg.32, the number of blocks the array will be divided into
		int num_of_ints_per_block = INT_ARRAY_SIZE / blocks_num; // eg.512
		int stride = num_of_ints_per_block << 1; // eg.1024, the stride for accessing the array

		start = clock();

		//======================================================
		for (i = 0; i < ROUNDS; ++i) {
			a[(i * stride + num_of_ints_per_block) & j] += i; // access the array in a store way
		}
		//======================================================

		finish = clock();

		cout << "TEST_ASSOCIATIVITY: " << test_ass << " - " << "avg time spent - " << (double)(finish - start) * 1000 / CLOCKS_PER_SEC << " ms" << endl;

		test_ass <<= 1;
	}

	
}
