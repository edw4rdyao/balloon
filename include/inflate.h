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
	void Uncompress(const char* compressed_file_path_and_name, const char* uncompressed_file_path);
	uint16 EncodeALl();
	uint16 EncodeADis();
	inline uint8 InBit();
private:
	DisHuffman* dis_hfm_tree_;		// Distance�ع�Huffman��
	LlHuffman* ll_hfm_tree_;		// literal/len�ع�Huffman��
	FILE* fp_in_;					// ��������ļ�
	FILE* fp_out_;		
	uint8* out_buffer_;				// ������
	uint32 out_buffer_cnt_;
	uint8 in_data_ = 0;				// 8bits������Ϣ
	uint8 in_bit_cnt_ = 0;			// ������Ϣ�Ѿ�������bitλ��
};

#endif // !INFLATE_H
