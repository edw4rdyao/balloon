#include "hash.h"

/*==========================================================
���ܣ�	HashTable��Ĺ��캯��
����ֵ��	void
������	�����ϣ������head_��prev_�����뺬��2*HSIZE��Ԫ�صĿռ�
		��head_��prev_������ڴ��������������Ҹ�0
==========================================================*/
HashTable::HashTable(uint16 size) {
	prev_ = new uint16[(int32)size * 2];
	head_ = prev_ + size;
	// ��head_��prev_����Ԫ�ظ�0
	memset(prev_, 0, (int32)size * 2 * sizeof(uint16));
}

/*==========================================================
���ܣ�	HashTable�����������
����ֵ��	void
������	delete����prev_����ĺ���2*HSIZE��Ԫ�صĿռ�
		��head_��prevָ���ÿ�
==========================================================*/
HashTable::~HashTable() {
	delete[] prev_;
	prev_ = nullptr;
	head_ = nullptr;
}

/*==========================================================
���ܣ�	���¹�ϣ��
����ֵ��	void
������	��������ǰ�ƶ�32Kʱ������ǰ��������С����MIN_LOOKAHEAD��
		��Ҫ��head_�����д��λ�ô���32K��Ԫ�ؼ�ȥ32K������ŵ�λ
		��С��32K��Ԫ����0
==========================================================*/
void HashTable::Updata() {
	for (uint32 i = 0; i < HSIZE; i++) {
		// ����head_����
		head_[i] = (head_[i] >= WSIZE) ? (head_[i] - WSIZE) : 0;
		// ����prev_����
		prev_[i] = (prev_[i] >= WSIZE) ? (prev_[i] - WSIZE) : 0;
	}
}

/*==========================================================
���ܣ�	����ַ����Ĺ�ϣ����
����ֵ��	void
������	ͨ�������ַ��������ַ����ĵ������ַ�����ȡ���ַ����Ĺ�ϣ
		��ַ���������ַ�����ϣ��ַ��Ҫ�����ַ���ֻ����һ���ַ���
		ԭ����ǰ�����ַ��Ĺ�ϣ��ַ�Ѿ�����������һ���ַ����Ĺ�ϣ
		��ַ�У�����ֻ��Ҫ�������ַ��Ϳ��Խ��ô��Ĺ�ϣ��ַ�����
==========================================================*/
void HashTable::HashFunction(uint16& hash_address, uint8 ch) {
	hash_address = ((hash_address << 5) ^ ch) & HASHMASK;
}

/*==========================================================
���ܣ�	ͨ���ַ����Ĺ�ϣ��ַ����λ�ò��뵽��ϣ����
����ֵ��	void
������	ÿ�����л�������ȡһ���ַ�ʱ���ͼ���������������ַ����
		���ַ����Ĺ�ϣ��ַ��head_���������������ö�Ӧhead����
		Ԫ�أ���Ϊ0˵����ƥ�䣬��֮ǰû�г��ֹ��ô������ٽ���Ԫ��
		����prev_����������Ϊ��ǰλ�õ�Ԫ�أ��Ӷ��γ�ƥ����
==========================================================*/
void HashTable::Insert(uint16& match_head, uint8 ch, uint16 pos, uint16& hash_address) {
	// ͨ����ϣ�����������ǰ���Ĺ�ϣ��ַ
	HashFunction(hash_address, ch);
	// ����ƥ����ͷ�����ڲ���ƥ�䴮�����ٽ�head_[hash_address]��ֵ���뱾�������ƥ�䴮��λ�ã�����ƥ����ͷ
	match_head = head_[hash_address];
	// ��head_[hash_address]��ֵ���뱾�������ƥ�䴮��λ�ã�����prev_����������Ϊ��ǰλ�õ�Ԫ�أ��Ӷ��γ�ƥ����
	prev_[pos & HASHMASK] = head_[hash_address];
	// ����ǰλ�ø���ƥ������ͷ
	head_[hash_address] = pos;
}
