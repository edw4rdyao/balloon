#include "llhuffman.h"

LlHuffman::LlHuffman(){
	// ��ʼ��len_to_ic_
	len_to_ic_ = new uint16[256]{
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 12,
		13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
		17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,
		19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
		23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
		25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
		26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
		26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
		27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
	};
	// ��ʼ��ic_node_
	ic_node_ = new Node[LL_CODENUM];
	for (int32 i = 0; i < LL_CODENUM; i++) {
		ic_node_[i] = new TreeNode;
		ic_node_[i]->code_ = i;
	}
	// ��ʼ��ic_extra_bits_
	ic_extra_bits_ = new uint16[LL_CODENUM - 257]{
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
	// ��ʼ��codeBeginLl
	ic_begin_ll_ = new uint16[LL_CODENUM - 257]{ 
		0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56,
		64, 80, 96, 112, 128, 160, 192, 224, 255 };
	// ��ʼ��ic_code_len_
	ic_code_len_ = new uint16[LL_CODENUM];
	for (int32 i = 0; i < LL_CODENUM; i++) ic_code_len_[i] = 0;
	// ��ʼ��ic_code_len_cnt_
	ic_code_len_cnt_ = new uint16[LL_MAX_CODELENGTH];
	for (int32 i = 0; i < LL_MAX_CODELENGTH; i ++)	ic_code_len_cnt_[i] = 0;
	// ��ʼ��ic_code_
	ic_code_ = new bitstring[LL_CODENUM];
	// ��ʼ��next_len_code_
	next_len_code_ = new bitstring[LL_MAX_CODELENGTH];
	// ��ʼ�������ڵ�
	tree_root_ = NULL;
}

LlHuffman::~LlHuffman(){
	delete[] len_to_ic_;
	delete[] ic_begin_ll_;
	delete[] ic_extra_bits_;
	for (int32 i = 0; i < LL_CODENUM; i++) delete ic_node_[i];
	delete[] ic_node_;
	delete[] ic_code_len_;
	delete[] ic_code_len_cnt_;
	delete[] ic_code_;
	delete[] next_len_code_;
}

void LlHuffman::AddFrequency(uint8 l, bool f){
	// len��literal
	if (f)	ic_node_[257 + len_to_ic_[l]]->frequency_++;
	else ic_node_[l]->frequency_++;
	return ;
}

void LlHuffman::CreatNormalTree(){
	// ���������Ӧ�Ľڵ���뵽���ȶ���
	for (int32 i = 0; i < LL_CODENUM; i++) {
		if (ic_node_[i]->frequency_ > 0)
			node_heap_.push(ic_node_[i]);
	}
	// ����ԭʼ��huffman��
	while (!node_heap_.empty()) {
		Node p = node_heap_.top();
		node_heap_.pop();
		if (node_heap_.empty()) {
			tree_root_ = p;
			break;
		}
		Node q = node_heap_.top();
		node_heap_.pop();
		Node new_node = new TreeNode;
		new_node->frequency_ = p->frequency_ + q->frequency_;
		if (p->frequency_ <= q->frequency_) {
			new_node->left_ = p;
			new_node->right_ = q;
		}
		else {
			new_node->left_ = q;
			new_node->right_ = p;
		}
		node_heap_.push(new_node);
	}
	return ;
}

void LlHuffman::CreatLlHfmTree(){
	// ��ȡ�������ȵı�������
	for (int32 i = 0; i < LL_CODENUM; i++) {
		ic_code_len_cnt_[ic_code_len_[i]]++;
	}
	// ��ȡ�����������Huffman����
	GetHfmCode();
	// �½����ڵ�
	tree_root_ = new TreeNode;
	// ����Huffman��
	for (uint32 i = 0; i < LL_CODENUM; i++) {
		if (ic_code_len_[i]) {
			Node p = tree_root_;
			bitstring code = ic_code_[i];
			for (auto it = code.begin(); it != code.end(); it++) {
				if (*it == '0') {
					if (it + 1 == code.end()) p->left_ = ic_node_[i];
					else if (!p->left_)	p->left_ = new TreeNode;
					p = p->left_;
				}
				else {
					if (it + 1 == code.end()) p->right_ = ic_node_[i];
					else if (!p->right_)	p->right_ = new TreeNode;
					p = p->right_;
				}
			}
			p->code_ = i;
		}
	}
	return;
}

void LlHuffman::GetCodeLength(Node p, uint16 l){
	if (p) {
		if (p->code_ >= 0) {
			ic_code_len_[p->code_] = l;
			ic_code_len_cnt_[l] ++;
		}
		GetCodeLength(p->left_, l + 1);
		GetCodeLength(p->right_, l + 1);
	}
	return;
}

void LlHuffman::GetHfmCode(){
	// ����ÿ�����ȵ���������������ߵı���
	bitstring code = "0";
	next_len_code_[1] = "0";
	for (int32 i = 2; i < LL_MAX_CODELENGTH; i++) {
		code = BitString::BitstringAdd(code, BitString::IntToBitstring(ic_code_len_cnt_[i - 1], 0, true));
		BitString::BitstringShiftLeft(code, 1);
		next_len_code_[i] = code;
	}
	// �õ�ÿ�������������
	for (int32 i = 0; i < LL_CODENUM; i++) {
		if (ic_code_len_[i] > 0) {
			ic_code_[i] = next_len_code_[ic_code_len_[i]];
			next_len_code_[ic_code_len_[i]]  = BitString::BitstringAdd(next_len_code_[ic_code_len_[i]], BitString::IntToBitstring(1, 0, true));
		}
	}
	// debug
	/*cout << "Code Frequency: " << endl;
	for (int32 i = 0; i < LL_CODENUM; i++) {
		cout << i << "-" << ic_node_[i]->frequency_ << endl;
	}*/
	/*cout << "Code HfmCodeBitstring: " << endl;
	for (int32 i = 0; i < LL_CODENUM; i++) {
		cout << i << "-" << ic_code_len_[i] << "-" << ic_code_[i] << endl;
	}*/
	return;
}

void LlHuffman::ResetHfm(){
	// �������������
	memset(ic_code_len_, 0, sizeof(uint16) * LL_CODENUM);
	memset(ic_code_len_cnt_, 0, sizeof(uint16) * LL_MAX_CODELENGTH);
	// ���Huffman��������
	for (int32 i = 0; i < LL_CODENUM; i++) ic_code_[i] = "";
	for(int32 i = 0; i < LL_MAX_CODELENGTH; i ++) next_len_code_[i] = "";
	while (!node_heap_.empty()) node_heap_.pop();
	// ���Huffman��
	DestroyTreeNode(tree_root_);
	tree_root_ = NULL;
	for (int32 i = 0; i < LL_CODENUM; i++) {
		ic_node_[i]->code_ = i;
		ic_node_[i]->frequency_ = 0;
		ic_node_[i]->left_ = ic_node_[i]->right_ = NULL;
	}
	return;
}

void LlHuffman::DestroyTreeNode(Node p){
	if (p) {
		DestroyTreeNode(p->left_);
		DestroyTreeNode(p->right_);
		if(p->code_ < 0) delete p;
	}
	return;
}
