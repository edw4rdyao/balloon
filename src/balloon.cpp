#include <iostream>
#include <time.h>
#include "deflate.h"
#include "inflate.h"

int main(int argc,char *argv[]) {
	auto start = clock();
	Deflate deflate;
	deflate.Compress("./test/1.txt", "./test/1.tj");
	Inflate inflate;
	inflate.Uncompress("./test/1.tj", "./test/1t.txt");
	auto end = clock();
	std::cout << "Time: " << (end - start) / (double)CLOCKS_PER_SEC << "s";
	system("pause");
	return 0;
}