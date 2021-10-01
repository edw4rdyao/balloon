#include "deflate.h"
Deflate::Deflate()
	:window(new uint8[2 * WSIZE]), lzHash(new HashTable(WSIZE)), 
	disHuffman(new DisHuffman), llHuffman(new LlHuffman){
	fBuff = new uint8[F_BUFFSIZE];	fBuffCnt = 0;
	lBuff = new uint8[L_BUFFSIZE];	lBuffCnt = 0;
	dBuff = new uint16[D_BUFFSIZE];	dBuffCnt = 0;
	oBuff = new uint8[O_BUFFSIZE_D];	oBuffCnt = 0;
	fO = NULL;
	fI = NULL;
}

Deflate::~Deflate() {
	delete[] window;
	delete lzHash;
	delete disHuffman;
	delete llHuffman;
	delete[] fBuff;
	delete[] lBuff;
	delete[] dBuff;
	delete[] oBuff;
}

/*=========================================================
功能：	核心压缩函数，将文件通过Deflate算法压缩
描述：	通过滑动窗口中的先行缓冲区读取的字符（和其后两个字符构成
		的字符串）建立哈希表，同时建立匹配链，再用当前串通过哈希
		表的匹配链查找最长匹配，将<长度，距离>对替换该串（当找不
		到匹配就不做替换，继续读入下一字符）；再将每个位置对应的
		标记信息（是否被‘<长度，距离>对’替换了）写入到缓冲区...
=========================================================*/
void Deflate::Compress(string fileName, string newFileName) {
	// 打开文件
	fO = fopen(newFileName.c_str(), "wb");
	assert(fO);
	fI = fopen(fileName.c_str(), "rb");
	assert(fI);

	// 获取原始文件大小，并输出到文件
	fseek(fI, 0, SEEK_END);
	uint64 fileSize = ftell(fI);
	fwrite(&fileSize, sizeof(uint64), 1, fO);
	fseek(fI, 0, SEEK_SET);

	// 先从文件中读取64K个字节
	auto lookAhead = fread(window, 1, WSIZE * 2, fI);

	// 初始化哈希地址
	uint16 hashAdress = 0;
	lzHash->HashFunction(hashAdress, window[0]);
	lzHash->HashFunction(hashAdress, window[1]);

	uint8 fData = 0;			// 8bits标记信息
	uint8 fBitCnt = 0;			// 标记信息bit位数
	uint8 oData = 0;			// 8bits输出信息
	uint8 oBitCnt = 0;			// 输出信息bit位数
	
	uint16 strStart = 0;		// 当前匹配查找的位置
	uint16 matchHead = 0;		// 当前字符串匹配查找的头位置，若为0则没有找到匹配串
	uint16 tMatchLength = 0;	// 当前匹配长度
	uint16 tMatchDistance = 0;	// 当前匹配距离

	while (lookAhead) {
		// 插入三字符字符串中的最后一个字符以计算哈希地址
		lzHash->Insert(matchHead, window[strStart + 2], strStart, hashAdress);
		tMatchLength = 0;
		tMatchDistance = 0;
		// 当获取的匹配头不为0，则计算匹配链中的最长匹配长度
		if (matchHead) {
			tMatchLength = GetMaxMatchLength(matchHead, tMatchDistance, strStart, lookAhead);
		}
		// 验证是否找到的匹配串
		if (tMatchLength >= MIN_MATCHLENGTH) {
			// 将<长度，距离>对写入缓冲区
			lBuff[lBuffCnt++] = tMatchLength - 3;
			dBuff[dBuffCnt++] = tMatchDistance;
			// 统计distance length频率用于构建Huffman树
			disHuffman->AddFrequency(tMatchDistance - 1);
			llHuffman->AddFrequency(tMatchLength - 3, true);
			// 写入标记缓冲区
			WriteFlag(fData, fBitCnt, true, oData, oBitCnt);
			// 替换后将被替换的串中所有三字符字符串插入到哈希表中
			for (int32 i = 0; i < tMatchLength - 1; i++) {
				strStart++;
				lzHash->Insert(matchHead, window[strStart + 2], strStart, hashAdress);
			}
			strStart++;
			lookAhead -= tMatchLength;
		}
		else {
			// 说明没有找到有效匹配串，将字符原样输出到压缩数据文件中
			lBuff[lBuffCnt++] = window[strStart];
			// 统计literal频率用于构建Huffman树
			llHuffman->AddFrequency(window[strStart], false);
			strStart++;
			lookAhead--;
			// 写入标记文件
			WriteFlag(fData, fBitCnt, false, oData, oBitCnt);
		}
		// 当先行缓冲区的长度小于最小长度时，窗口滑动
		if (lookAhead <= MIN_LOOKAHEAD) {
			MoveWindow(lookAhead, strStart);
		}
	}

	// 当最后的标记位数不够8bit，直接左移写入缓冲区
	if (fBitCnt) {
		fData <<= (8 - fBitCnt);
		fBuff[fBuffCnt++] = fData;
		fData = 0;
		fBitCnt = 0;
	}
	// 写入最后一块
	OutHuffman(oData, oBitCnt);
	if (oBuffCnt) {
		fwrite(oBuff, sizeof(uint8), oBuffCnt, fO);
	}

	fclose(fO);
	return;
}

/*=========================================================
功能：	获得最长的匹配串
返回值：	最长的匹配长度
描述：	通过匹配链不断查找得到最长匹配长度的匹配串，但是要通过限
		制匹配次数以防止陷入死循环
=========================================================*/
uint16 Deflate::GetMaxMatchLength(uint16 matchHead, uint16& tMatchDistance, uint16 strStart, uint32 lookAhead) {
	uint16 nowMatchLength = 0;				// 每次匹配的长度
	uint16 tMatchStart = 0;					// 当前匹配在查找缓冲区中的起始位置
	uint8 maxMatchTimes = MAX_MATCH_TIMES;	// 最大的匹配次数
	uint16 maxMatchLenght = 0;				// 最长的匹配长度

	while (matchHead > 0 && maxMatchTimes--) {
		uint8* pStart = window + strStart;	
		uint8* pEnd = pStart + ((lookAhead <= MAX_MATCHLENGTH) ? (lookAhead - 1) : MAX_MATCHLENGTH);
		uint8* pMatchStart = window + matchHead;
		nowMatchLength = 0;
		// 太远的匹配放弃
		if (strStart - matchHead > WSIZE - MIN_LOOKAHEAD) {
			break;
		}
		// 逐字符进行匹配
		while (pStart < pEnd && *pStart == *pMatchStart) {
			pStart++;
			pMatchStart++;
			nowMatchLength++;
		}
		// 一次匹配结束了，判断并记录最长匹配
		if (nowMatchLength > maxMatchLenght) {
			maxMatchLenght = nowMatchLength;
			tMatchStart = matchHead;
		}
		// 匹配链的下一个匹配位置
		matchHead = lzHash->prev[matchHead & HASHMASK];
	}

	// 获得最终最长匹配对应的距离
	tMatchDistance = strStart - tMatchStart;
	return maxMatchLenght;
}

/*=========================================================
功能：	滑动窗口
返回值：	void
描述：	将滑动窗口向后移动32K
=========================================================*/
void Deflate::MoveWindow(size_t& lookAhead, uint16& strStart) {
	// 先判断是否到达了文件末端
	if (strStart >= WSIZE) {
		// 将右窗中的数据移至左窗右窗置0
		memcpy(window, window + WSIZE, WSIZE);
		memset(window + WSIZE, 0, WSIZE);
		strStart -= WSIZE;
		// 更新哈希表
		lzHash->Updata();
		// 向右窗（先前缓冲区）补充数据
		if (!feof(fI)) {
			lookAhead += fread(window + WSIZE, 1, WSIZE, fI);
		}
	}
}


/*=========================================================
功能：	写入标记信息
返回值：	void
描述：	通过判断字符是否被<长度，距离>对替换，来向缓冲区fBuff写入标
		记信息，用0表示未被替换，用1表示已被替换
=========================================================*/
void Deflate::WriteFlag(uint8& fData, uint8& fBitCnt, bool hasEncode, uint8& oData, uint8& oBitCnt) {
	// 先左移一位，若为length，则记录1
	fData <<= 1;
	if (hasEncode)	fData |= 1;
	fBitCnt++;
	// 当标记满8bit时，将fData写入缓冲区
	if (fBitCnt == 8) {
		fBuff[fBuffCnt++] = fData;
		fData = 0;
		fBitCnt = 0;
	}
	// 如果该块已满（fBuff大小达到8k，即lBuff大小达到64k）
	if (fBuffCnt == F_BUFFSIZE) {
		OutHuffman(oData, oBitCnt);
	}
	return ;
}

/*=========================================================
功能：	输出Huffman编码文件到缓冲区
返回值：	void
描述：	将该块的信息写入缓冲区，各块的Huffman编码独立
=========================================================*/
void Deflate::OutHuffman(uint8& oData, uint8& oBitCnt){
	// 将256加入树中
	llHuffman->codeNode[256]->frequency++;
	// 得到distance树和lenght/literal树的Huffman编码
	disHuffman->CreatNormalTree();
	disHuffman->GetCodeLength(disHuffman->root, 0);
	disHuffman->GetHfmCode();
	llHuffman->CreatNormalTree();
	llHuffman->GetCodeLength(llHuffman->root, 0);
	llHuffman->GetHfmCode();

	/*double avg = 0, fre = 0;
	for (int32 i = 0; i < D_CODENUM; i++) {
		avg +=((double)disHuffman->codeLength[i] + (double)disHuffman->codeExtraBits[i]) * disHuffman->codeNode[i]->frequency;
		fre += (double)disHuffman->codeNode[i]->frequency;
	}
	avg /= fre;
	cout <<  "distance tree: " << avg << endl;*/

	/*avg = 0, fre = 0;
	for (int32 i = 0; i < LL_CODENUM; i++) {
		if (i >= 257) {
			avg += ((double)llHuffman->codeLength[i] + (double)llHuffman->codeExtraBits[i - 257]) * llHuffman->codeNode[i]->frequency;
		}
		else {
			avg += (double)llHuffman->codeLength[i]  * llHuffman->codeNode[i]->frequency;
		}
		fre += (double)llHuffman->codeNode[i]->frequency;
	}
	avg /= fre;
	cout << "l tree: " << avg << endl;*/

	// 写入建立Huffman树所需要的信息
	fwrite(llHuffman->codeLength, sizeof(uint16), LL_CODENUM, fO);
	fwrite(disHuffman->codeLength, sizeof(uint16), D_CODENUM, fO);
	// 循环flag的每一位，将Huffman编码写入输出到缓冲区
	uint32 ll = 0, d = 0, cnt = 0;
	for (uint32 i = 0; i < fBuffCnt; i++) {
		uint8 f = fBuff[i];
		for (uint32 j = 0; j < 8; j++) {
			if (f & 0x80) {
				// 将length的编码写入
				uint8 length = lBuff[ll++];
				uint16 lengthToCode = llHuffman->lToCode[length];
				BS lCode = llHuffman->hfmCode[257 + lengthToCode];
				for (auto it = lCode.begin(); it != lCode.end(); it++) {
					if (*it == '1') OutBit(oData, oBitCnt, 1);
					else OutBit(oData, oBitCnt, 0);
				}
				uint32 extra = length - llHuffman->codeBeginLl[lengthToCode];
				for (int32 k = 0; k < llHuffman->codeExtraBits[lengthToCode]; k++) {
					if (extra & 1) OutBit(oData, oBitCnt, 1);
					else OutBit(oData, oBitCnt, 0);
					extra >>= 1;
				}
				// 紧接着写入distance的编码
				uint16 distance = dBuff[d++];
				uint16 distanceToCode = (distance - 1 < 256) ? disHuffman->disToCode[distance - 1] : disHuffman->disToCode[256 + ((distance - 1) >> 7)];
				BS dCode = disHuffman->hfmCode[distanceToCode];
				for (auto it = dCode.begin(); it != dCode.end(); it++) {
					if (*it == '1') OutBit(oData, oBitCnt, 1);
					else OutBit(oData, oBitCnt, 0);
				}
				extra = distance - disHuffman->codeBeginDis[distanceToCode];
				for (int32 k = 0; k < disHuffman->codeExtraBits[distanceToCode]; k++) {
					if (extra & 1) OutBit(oData, oBitCnt, 1);
					else OutBit(oData, oBitCnt, 0);
					extra >>= 1;
				}
			}
			else {
				// 将literal编码写入
				uint8 literal = lBuff[ll ++];
				BS lCode = llHuffman->hfmCode[literal];
				for (auto it = lCode.begin(); it != lCode.end(); it++) {
					if (*it == '1') OutBit(oData, oBitCnt, 1);
					else OutBit(oData, oBitCnt, 0);
				}
			}
			// 防止最后一块的fBuff中最后一个字节未满8字节就填入，所以需要以lBuff为准计数
			cnt++;
			if (cnt == lBuffCnt) break;
			f <<= 1;
		}
	}
	// 写入256表示该块的结束
	BS endCode = llHuffman->hfmCode[256];
	for (auto it = endCode.begin(); it != endCode.end(); it++) {
		if (*it == '1') OutBit(oData, oBitCnt, 1);
		else OutBit(oData, oBitCnt, 0);
	}
	// 如果最后不足8位，也直接写入
	if (oBitCnt) {
		oData <<= (8 - oBitCnt);
		oBuff[oBuffCnt++] = oData;
		oData = 0;
		oBitCnt = 0;
	}
	if (oBuffCnt) {
		fwrite(oBuff, sizeof(uint8), oBuffCnt, fO);
		oBuffCnt = 0;
	}
	// 重置Huffman树
	disHuffman->ResetHfm();
	llHuffman->ResetHfm();
	// 重置缓冲区
	fBuffCnt = 0;
	dBuffCnt = 0;
	lBuffCnt = 0;
	return;
}

/*=========================================================
功能：	输出一个bit到缓冲区
返回值：	void
描述：	输出一个bit位到输出缓冲区，当满8位时输出一个字节
=========================================================*/
void Deflate::OutBit(uint8& oData, uint8& oBitCnt, bool bit){
	oData <<= 1;
	if (bit) oData |= 1;
	oBitCnt++;
	if (oBitCnt == 8) {
		oBuff[oBuffCnt++] = oData;
		if (oBuffCnt == O_BUFFSIZE_D) {
			fwrite(oBuff, sizeof(uint8), O_BUFFSIZE_D, fO);
			oBuffCnt = 0;
		}
		oData = 0;
		oBitCnt = 0;
	}
	return;
}
