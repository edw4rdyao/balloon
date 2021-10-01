#include "inflate.h"

Inflate::Inflate()
	:disHuffman(new DisHuffman), llHuffman(new LlHuffman){
	iData = 0;	iBitCnt = 0;
	oBuff = new uint8[O_BUFFSIZE_I];	oBuffCnt = 0;
	fO = NULL;
	fI = NULL;
}

Inflate::~Inflate(){
	delete disHuffman;
	delete llHuffman;
	delete[] oBuff;
}

void Inflate::Uncompress(string fileName, string newFileName){
	// 打开输入输出文件
	fI = fopen(fileName.c_str(), "rb");
	assert(fI);
	fO = fopen(newFileName.c_str(), "wb");
	assert(fO);

	uint64 fileSize = 0;	// 原文件大小
	uint64 inflateCnt = 0;	// 已解压文件大小

	// 获取原文件大小
	fread(&fileSize, sizeof(uint64), 1, fI);

	while (inflateCnt < fileSize) {
		// 块的开始，获取重构Huffman树所需要的信息
		fread(llHuffman->codeLength, sizeof(uint16), LL_CODENUM, fI);
		fread(disHuffman->codeLength, sizeof(uint16), D_CODENUM, fI);
		fread(&iData, sizeof(uint8), 1, fI);
		iBitCnt = 0;
		// 重构Huffman树
		disHuffman->CreatDisHfmTree();
		llHuffman->CreatLlHfmTree();
		while (true) {
			// 解码出length/literal区间码
			uint16 encodeLl = EncodeALl();	
			if (encodeLl >= 0 && encodeLl <= 255) {
				// 为literal
				uint8 literal = encodeLl;
				oBuff[oBuffCnt++] = literal;
				if (oBuffCnt == O_BUFFSIZE_I) {
					fwrite(oBuff, sizeof(uint8), O_BUFFSIZE_I / 2, fO);
					memcpy(oBuff, oBuff + O_BUFFSIZE_I / 2, O_BUFFSIZE_I / 2 * sizeof(uint8));
					oBuffCnt -= O_BUFFSIZE_I / 2;
				}
				// 更新已解码大小
				inflateCnt++;
			}
			else if (encodeLl >= 257 && encodeLl <= 285) {
				// 为length，则需要添加extra
				uint16 length = llHuffman->codeBeginLl[encodeLl - 257], extra = 0;
				for (int32 i = 0; i < llHuffman->codeExtraBits[encodeLl - 257]; i++) {
					if (InBit()) extra |= (1 << i);
				}
				length += extra;
				length += 3;
				// 解码出distance
				uint16 distance = EncodeADis();
				// 更新已解码大小
				inflateCnt += length;
				// 判断是否可以写入缓冲区
				if (oBuffCnt + length >= O_BUFFSIZE_I) {
					fwrite(oBuff, sizeof(uint8), O_BUFFSIZE_I / 2, fO);
					memcpy(oBuff, oBuff + O_BUFFSIZE_I / 2, O_BUFFSIZE_I / 2 * sizeof(uint8));
					oBuffCnt -= O_BUFFSIZE_I / 2;
				}
				uint32 oBuffCntPre = oBuffCnt;
				if (distance < length) {
					uint32 restLength = length;
					while (restLength >= distance) {
						memcpy(oBuff + oBuffCnt, oBuff + oBuffCnt - distance, distance * sizeof(uint8));
						oBuffCnt += distance;
						restLength -= distance;
					}
					memcpy(oBuff + oBuffCnt, oBuff + oBuffCntPre - distance, restLength * sizeof(uint8));
					oBuffCnt += restLength;
				}
				else {
					memcpy(oBuff + oBuffCnt, oBuff + oBuffCnt - distance, length * sizeof(uint8));
					oBuffCnt += length;
				}
			}
			else if (encodeLl == 256) {
				// 块的结束
				break;
			}
		}
		// 新的一个块的开始，重置Huffman树
		disHuffman->ResetHfm();
		llHuffman->ResetHfm();
	}
	if (oBuffCnt) {
		fwrite(oBuff, sizeof(uint8), oBuffCnt, fO);
	}
	fclose(fO);
	return;
}

uint16 Inflate::EncodeALl(){
	Node p = llHuffman->root;
	while (p->code < 0) {
		if (InBit()) p = p->right;
		else p = p->left;
	}
	return uint16(p->code);
}

uint16 Inflate::EncodeADis(){
	Node p = disHuffman->root;
	while (p->code < 0) {
		if (InBit()) p = p->right;
		else p = p->left;
	}
	uint16 encode = p->code;
	uint16 distance = disHuffman->codeBeginDis[encode], extra = 0;
	for (int32 i = 0; i < disHuffman->codeExtraBits[encode]; i++) {
		if(InBit()) extra |= (1 << i);
	}
	distance += extra;
	return distance;
}

uint8 Inflate::InBit(){
	uint8 bit = 0;
	if (iBitCnt == 8) {
		fread(&iData, sizeof(uint8), 1, fI);
		iBitCnt = 0;
	}
	if (iData & 0x80) bit = 1;
	iData <<= 1;
	iBitCnt++;
	return bit;
}

