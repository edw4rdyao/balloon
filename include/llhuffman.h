#ifndef LLHUFFMAN_H
#define LLHUFFMAN_H
#include "universal.h"
#include "treenode.h"
#include <algorithm>
#include <queue>

class LlHuffman {
public:
	LlHuffman();
	~LlHuffman();
	void AddFrequency(uint8 l, bool f);
	void CreatNormalTree();
	void CreatLlHfmTree();
	void GetCodeLength(Node p, uint16 l);
	void GetHfmCode();
	void ResetHfm();
	void DestroyTreeNode(Node p);

	class Cmp {
	public:
		bool operator()(const Node& a, const Node& b) const {
			return a->frequency_ > b->frequency_;
		}
	};

private:
	uint16* len_to_ic_;			// length��Ӧ��������(257-285)
	uint16* ic_extra_bits_;		// �������extraBits��չλ��
	uint16* ic_begin_ll_;		// �������ll��Χ������
	uint16* ic_code_len_;			// �������Ӧ�����ֳ���
	uint16* ic_code_len_cnt_;		// �������ȵ��������ֳ��ֵĴ���

	bitstring* ic_code_;			// �������Ӧ������(�ַ�����)
	bitstring* next_len_code_;		// �������ȵ��������ֵ���һ������(�ַ�����)

	Node* ic_node_;			// ���ڹ���ԭʼHuffman���Ľڵ�����
	Node tree_root_;				// Huffman�����ڵ�
	std::priority_queue<Node, std::vector<Node>, Cmp> node_heap_;  // ����ԭʼHuffman��ʱ��С����
	friend class Deflate;
	friend class Inflate;
};

#endif // !LLHUFFMAN_H
