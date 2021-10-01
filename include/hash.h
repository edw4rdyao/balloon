#ifndef HASH_H
#define HASH_H
#include "universal.h"
class HashTable {
public:
	HashTable(uint16 size);
	~HashTable();
	void Updata();
	void HashFunction(uint16& hashAddr, uint8 ch);
	void Insert(uint16& matchHead, uint8 ch, uint16 pos, uint16& hashAddr);

private:
	uint16* head;		// 哈希表的head数组，用以存放某（三字符）字符串的匹配头，索引为（三字符）字符串对应的哈希地址（通过哈希函数获取）
	uint16* prev;		// 哈希表的prev数组，用以存放上一个字符串的位置，用于解决哈希冲突将字符连成匹配链*/
	friend class Deflate;
};

#endif