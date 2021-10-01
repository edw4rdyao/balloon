#ifndef UNIVERSAL_H
#define UNIVERSAL_H
#include<iostream>
#include<string.h>
using namespace std;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef string BS;

const uint16 WSIZE = 32 * 1024; 						// 32*1024=32K
const uint16 MIN_MATCHLENGTH = 3;						// ��̵�ƥ�䳤��Ϊ3
const uint16 MAX_MATCHLENGTH = 258;						// ���ƥ�䳤��255+3=258
const uint16 MIN_LOOKAHEAD = MIN_MATCHLENGTH + MAX_MATCHLENGTH + 1;		// ���л���������С���ȣ���С�ڴ˳���ʱ����ʹ���ڻ���32K
const uint16 MAX_MATCH_TIMES = 16;

const uint16 HSIZE = uint16(32 * 1024);					// ��ϣ������Ĵ�С
const uint16 HASHMASK = uint16(HSIZE - 1);				// ��ֹ��ϣ���������

const uint16 D_CODENUM = 30;
const uint16 D_MAX_CODELENGTH = 30;

const uint16 LL_CODENUM = 286;
const uint16 LL_MAX_CODELENGTH = 286;

const uint32 L_BUFFSIZE = 64 * 1024;					// 64k
const uint32 D_BUFFSIZE = 64 * 1024;
const uint32 F_BUFFSIZE = 8  * 1024;
const uint32 O_BUFFSIZE_D = 64 * 1024;
const uint32 O_BUFFSIZE_I = 64 * 1024;
const uint32 I_BUFFSIZE = 64 * 1024;

BS IntToBS(uint32 i, uint16 l, bool b);
void BSShiftLeft(BS& b, uint16 l);
BS BSAdd(const BS& a, const BS& b);

#endif