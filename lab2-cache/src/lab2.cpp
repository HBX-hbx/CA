#include <sched.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

using namespace std;

int stride = 4; // the stride for accessing the array
const int INT_ARRAY_SIZE = (1 << 15); // 32K * 4B = 128KB
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
	register int i, j = INT_ARRAY_SIZE - 1;

	for (int k = 0; k < 8; ++k) {
		// put the data into cache
		for (i = 0; i < INT_ARRAY_SIZE; ++i) {
			a[i] = i; // access the array in a store way
		}

		start = clock();

		//======================================================
		for (i = 0; i < ROUNDS; ++i) {
			a[(i * stride / 4) & j] = i; // access the array in a store way
		}
		//======================================================

		finish = clock();

		cout << "STRIDE: " << stride << "B - " << "avg time spent - " << (double)(finish - start) * 1000 / CLOCKS_PER_SEC << " ms" << endl;
		stride <<= 1;
	}

	
}
