#ifndef DEFLATE_H
#define DEFLATE_H
#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <cstdio>
#include <assert.h>
#include "hash.h"
#include "dishuffman.h"
#include "llhuffman.h"

class Deflate {
public:
	Deflate();
	~Deflate();
	void Compress(const char* origin_file_path_and_name, const char* compressed_file_path_and_name, const char* origin_file_name);
	void MoveWindow(size_t& look_ahead, uint16& str_start);
	uint16 GetMaxMatchLength(uint16 match_head, uint16& t_match_dis, uint16 str_start, uint32 look_ahead);
	void WriteFlag(bool is_encoded);
	void OutHuffman();
	inline void OutBit(bool bit);
private:
	uint8* window_;					// �������ڣ���СΪ64K�����ڴ�Ż�����������
	HashTable* lz77_hash_;			// ���ڶ�Ӧ�Ĺ�ϣ�������Ŵ��ڵĻ������ϸ���
	DisHuffman* dis_hfm_tree_;		// Distance����Huffman��
	LlHuffman* ll_hfm_tree_;		// literal/length����Huffman��
	FILE* fp_in_;					// ��������ļ�
	FILE* fp_out_;
	uint8* flag_buffer_;uint32 flag_buffer_cnt_;
	uint8* ll_buffer_;	uint32 ll_buffer_cnt_;
	uint16* dis_buffer_;uint32 dis_buffer_cnt_;
	uint8* out_buffer_;	uint32 out_buffer_cnt_;
	uint8 flag_data_;	uint8 flag_bit_cnt_;
	uint8 out_data_;	uint8 out_bit_cnt_;
};

#endif