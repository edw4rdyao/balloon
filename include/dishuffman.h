#ifndef DISHUFFMAN_H
#define DISHUFFMAN_H
#include "universal.h"
#include "treenode.h"
#include <algorithm>
#include <queue>

class DisHuffman{
public:
	DisHuffman();
	~DisHuffman();
	void AddFrequency(uint16 d);
	void CreatNormalTree();
	void CreatDisHfmTree();
	void GetCodeLength(Node p, uint16 l);
	void GetHfmCode();
	void ResetHfm();
	void DestroyTreeNode(Node p);
	class Cmp{
	public:
		bool operator()(const Node& a, const Node& b) const {
			return a->frequency_ > b->frequency_;
		}
	};
private:

	uint8* dis_to_ic_;			// distance��Ӧ��������(0-29)
	uint16* ic_extra_bits_;		// �������extraBits(��չλ��)
	uint16* ic_begin_dis_;		// �������distance��Χ������
	uint16* ic_code_len_;			// �������Ӧ�����ֳ���
	uint16* ic_code_len_cnt_;		// �������ȵ��������ֳ��ֵĴ���

	uint32* ic_code_int_;		// �������Ӧ������(����)
	uint32* next_len_code_int_;	// �������ȵ��������ֵ���һ������(����)
	bitstring* ic_code_;			// �������Ӧ������(�ַ�����)

	Node* ic_node_;			// ���ڹ���ԭʼHuffman���Ľڵ�����
	Node tree_root_;				// ���ڵ�
	std::priority_queue<Node, std::vector<Node>,  Cmp> node_heap_;  // ����ԭʼHuffman��ʱ��С����
	friend class Deflate;
	friend class Inflate;
};
#endif