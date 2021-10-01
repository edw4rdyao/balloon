#ifndef DEFLATE_H
#define DEFLATE_H
#define _CRT_SECURE_NO_WARNINGS
#include "hash.h"
#include "dishuffman.h"
#include "llhuffman.h"
#include <fstream>
#include <cstdio>
#include <assert.h>

class Deflate {
public:
	Deflate();
	~Deflate();
	void Compress(string fileName, string newFileName);
	void MoveWindow(size_t& lookAhead, uint16& strStart);
	uint16 GetMaxMatchLength(uint16 matchHead, uint16& tMatchDistance, uint16 strStart, uint32 lookAhead);
	void WriteFlag(uint8& fData, uint8& fBitCnt, bool hasEncode, uint8& oData, uint8& oBitCnt);
	void OutHuffman(uint8& oData, uint8& oBitCnt);
	inline void OutBit(uint8& oData, uint8& oBitCnt, bool bit);
private:
	uint8* window;				// 滑动窗口，大小为64K，用于存放缓冲区的数据
	HashTable* lzHash;			// 窗口对应的哈希表，随着窗口的滑动不断更新
	DisHuffman* disHuffman;		// Distance构建Huffman树
	LlHuffman* llHuffman;		// literal/length构建Huffman树
	FILE* fI;					// 输入输出文件
	FILE* fO;
	uint8* fBuff;				// 缓冲区
	uint32 fBuffCnt;
	uint8* lBuff;
	uint32 lBuffCnt;
	uint16* dBuff;
	uint32 dBuffCnt;
	uint8* oBuff;
	uint32 oBuffCnt;
};

#endif