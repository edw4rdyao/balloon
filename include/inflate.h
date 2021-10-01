#ifndef INFLATE_H
#define INFLATE_H
#define _CRT_SECURE_NO_WARNINGS
#include "dishuffman.h"
#include "llhuffman.h"
#include <fstream>
#include <cstdio>
#include <assert.h>

class Inflate {
public:
	Inflate();
	~Inflate();
	void Uncompress(string fileName, string newFileName);
	uint16 EncodeALl();
	uint16 EncodeADis();
	uint8 InBit();
private:
	DisHuffman* disHuffman;		// Distance重构Huffman树
	LlHuffman* llHuffman;		// literal/length重构Huffman树
	FILE* fI;					// 输入输出文件
	FILE* fO;		
	uint8* oBuff;				// 缓冲区
	uint32 oBuffCnt;
	uint8 iData = 0;			// 8bits输入信息
	uint8 iBitCnt = 0;			// 输入信息已经处理的bit位数
};


#endif // !INFLATE_H
