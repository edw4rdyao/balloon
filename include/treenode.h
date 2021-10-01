#ifndef TREENODE_H
#define TREENODE_H
#include "universal.h"
class DisHuffman;
class LlHuffman;
class TreeNode{
public:
	TreeNode();
	~TreeNode();

private:
	int32 code_;
	uint64 frequency_;
	TreeNode* left_;
	TreeNode* right_;
	friend class DisHuffman;
	friend class LlHuffman;
	friend class Deflate;
	friend class Inflate;
};
typedef TreeNode* Node;

#endif // !TREENODE_H

