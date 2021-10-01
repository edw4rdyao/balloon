#include "hash.h"

/*==========================================================
功能：	HashTable类的构造函数
返回值：	void
描述：	构造哈希表数组head和prev，申请含有2*HSIZE个元素的空间
		将head和prev数组的内存连接起来，并且赋0
==========================================================*/
HashTable::HashTable(uint16 size) {
	prev = new uint16[(int32)size * 2];
	head = prev + size;
	// 将head和prev数组元素赋0
	memset(prev, 0, (int32)size * 2 * sizeof(uint16));
}

/*==========================================================
功能：	HashTable类的析构函数
返回值：	void
描述：	delete数组prev申请的含有2*HSIZE个元素的空间
		将head和prev指针置空
==========================================================*/
HashTable::~HashTable() {
	delete[] prev;
	prev = nullptr;
	head = nullptr;
}

/*==========================================================
功能：	更新哈希表
返回值：	void
描述：	当窗口向前移动32K时（即先前缓冲区大小不足MIN_LOOKAHEAD）
		需要将head数组中存放位置大于32K的元素减去32K，将存放的位
		置小于32K的元素置0
==========================================================*/
void HashTable::Updata() {
	for (uint32 i = 0; i < HSIZE; i++) {
		// 更新head数组
		if (head[i] >= WSIZE) {
			head[i] -= WSIZE;
		}else{
			head[i] = 0;
		}
		// 更新prev数组
		if (prev[i] >= WSIZE) {
			prev[i] -= WSIZE;
		}else{
			prev[i] = 0;
		}
	}
}

/*==========================================================
功能：	获得字符串的哈希函数
返回值：	void
描述：	通过传入字符串（三字符）的第三个字符来获取本字符串的哈希
		地址，（计算字符串哈希地址需要三个字符，只传入一个字符的
		原因是前两个字符的哈希地址已经被保存在上一个字符串的哈希
		地址中，所以只需要第三个字符就可以将该串的哈希地址计算出
==========================================================*/
void HashTable::HashFunction(uint16& hashAddr, uint8 ch) {
	hashAddr = ((hashAddr << 5) ^ ch) & HASHMASK;
}

/*==========================================================
功能：	通过字符串的哈希地址将其位置插入到哈希表中
返回值：	void
描述：	每从先行缓冲区读取一个字符时，就计算其与其后两个字符组成
		的字符串的哈希地址（head数组的索引），获得对应head数组
		元素（若为0说明无匹配，即之前没有出现过该串），再将该元素
		赋给prev数组中索引为当前位置的元素，从而形成匹配链
==========================================================*/
void HashTable::Insert(uint16& matchHead, uint8 ch, uint16 pos, uint16& hashAddr) {
	// 通过哈希函数计算出当前串的哈希地址
	HashFunction(hashAddr, ch);
	// 传入匹配链头（用于查找匹配串），再将head[hashAddr]的值（离本串最近的匹配串的位置）赋给匹配链头
	matchHead = head[hashAddr];
	// 将head[hashAddr]的值（离本串最近的匹配串的位置）赋给prev数组中索引为当前位置的元素，从而形成匹配链
	prev[pos & HASHMASK] = head[hashAddr];
	// 将当前位置赋给匹配链的头
	head[hashAddr] = pos;
}
