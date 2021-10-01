#ifndef HASH_H
#define HASH_H
#include "universal.h"
class HashTable {
public:
	HashTable(uint16 size);
	~HashTable();
	void Updata();
	void HashFunction(uint16& hash_address, uint8 ch);
	void Insert(uint16& match_head, uint8 ch, uint16 pos, uint16& hash_address);

private:
	uint16* head_;		// ��ϣ����head���飬���Դ��ĳ�����ַ����ַ�����ƥ��ͷ������Ϊ�����ַ����ַ�����Ӧ�Ĺ�ϣ��ַ��ͨ����ϣ������ȡ��
	uint16* prev_;		// ��ϣ����prev���飬���Դ����һ���ַ�����λ�ã����ڽ����ϣ��ͻ���ַ�����ƥ����*/
	friend class Deflate;
};

#endif