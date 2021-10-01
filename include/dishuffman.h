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
	void DestroyTree(Node p);

	class Cmp{
	public:
		bool operator()(const Node& a, const Node& b) const {
			return a->frequency > b->frequency;
		}
	};

private:

	uint8* disToCode;			// distance对应的区间码(0-29)
	uint16* codeExtraBits;		// 区间码的extraBits(拓展位数)
	uint16* codeBeginDis;		// 区间码的distance范围的下限
	uint16* codeLength;			// 区间码对应的码字长度
	uint16* codeLengthCnt;		// 各个长度的区间码字出现的次数

	uint32* hfmCodeInt;		// 区间码对应的码字(整型)
	uint32* nextHfmCodeInt;	// 各个长度的区间码字的下一个码字(整型)
	BS* hfmCode;			// 区间码对应的码字(字符串型)

	Node* codeNode;			// 用于构建原始Huffman树的节点数组
	Node root;				// 根节点
	priority_queue<Node, vector<Node>,  Cmp> nodeHeap;  // 构建原始Huffman树时的小根堆
	friend class Deflate;
	friend class Inflate;
};
#endif