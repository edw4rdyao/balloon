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
	void DestroyTree(Node p);

	class Cmp {
	public:
		bool operator()(const Node& a, const Node& b) const {
			return a->frequency > b->frequency;
		}
	};

private:
	uint16* lToCode;			// length对应的区间码(257-285)
	uint16* codeExtraBits;		// 区间码的extraBits拓展位数
	uint16* codeBeginLl;		// 区间码的ll范围的下限
	uint16* codeLength;			// 区间码对应的码字长度
	uint16* codeLengthCnt;		// 各个长度的区间码字出现的次数

	BS* hfmCode;			// 区间码对应的码字(字符串型)
	BS* nextHfmCode;		// 各个长度的区间码字的下一个码字(字符串型)

	Node* codeNode;			// 用于构建原始Huffman树的节点数组
	Node root;				// Huffman树根节点
	priority_queue<Node, vector<Node>, Cmp> nodeHeap;  // 构建原始Huffman树时的小根堆
	friend class Deflate;
	friend class Inflate;
};

#endif // !LLHUFFMAN_H
