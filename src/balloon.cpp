#include "deflate.h"
#include "inflate.h"
#include <iostream>
#include <time.h>

int main() {
	auto start = clock();
	Deflate deflate;
	deflate.Compress("3.txt", "3.tj");
	// Inflate inflate;
	// inflate.Uncompress("3.tj", "3t.txt");
	auto end = clock();
	cout << "Time: " << (end - start) / (double)CLOCKS_PER_SEC << "s";
	return 0;
}