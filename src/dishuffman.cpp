#include "dishuffman.h"

#include <bitset>

DisHuffman::DisHuffman() {
  // ��ʼ����������distance�Ĺ�ϵ
  dis_to_ic_ = new uint8[512]{
      0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,
      8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  10, 10, 10, 10,
      10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11,
      11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12,
      12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
      12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
      13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
      14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15,
      15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
      15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
      15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
      15, 15, 15, 15, 0,  0,  16, 17, 18, 18, 19, 19, 20, 20, 20, 20, 21, 21,
      21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
      24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26,
      26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
      26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
      27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
      28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29,
      29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
      29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
      29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
      29, 29, 29, 29, 29, 29, 29, 29};
  // ��ʼ��extrabits(��չλ)
  ic_extra_bits_ = new uint16[D_CODENUM]{0, 0, 0,  0,  1,  1,  2,  2,  3,  3,
                                         4, 4, 5,  5,  6,  6,  7,  7,  8,  8,
                                         9, 9, 10, 10, 11, 11, 12, 12, 13, 13};
  // ��ʼ��ic_begin_dis_,ÿ�������뿪ʼ��distanceֵ
  ic_begin_dis_ = new uint16[D_CODENUM]{
      1,    2,    3,    4,    5,    7,    9,    13,    17,    25,
      33,   49,   65,   97,   129,  193,  257,  385,   513,   769,
      1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
  // ��ʼ��ic_node_
  ic_node_ = new Node[D_CODENUM];
  for (int32 i = 0; i < D_CODENUM; i++) {
    ic_node_[i] = new TreeNode;
    ic_node_[i]->code_ = i;
  }
  // ��ʼ��ic_code_len_
  ic_code_len_ = new uint16[D_CODENUM];
  for (int32 i = 0; i < D_CODENUM; i++) ic_code_len_[i] = 0;
  // ��ʼ��ic_code_len_cnt_
  ic_code_len_cnt_ = new uint16[D_MAX_CODELENGTH];
  for (int32 i = 0; i < D_MAX_CODELENGTH; i++) ic_code_len_cnt_[i] = 0;
  // ��ʼ��ic_code_int_
  ic_code_int_ = new uint32[D_CODENUM];
  for (int32 i = 0; i < D_CODENUM; i++) ic_code_int_[i] = 0;
  // ��ʼ��ic_code_
  ic_code_ = new bitstring[D_CODENUM];
  // ��ʼ��next_len_code_int_
  next_len_code_int_ = new uint32[D_MAX_CODELENGTH];
  for (int32 i = 0; i < D_MAX_CODELENGTH; i++) next_len_code_int_[i] = 0;
  // ��ʼ�������ڵ�
  tree_root_ = NULL;
}

DisHuffman::~DisHuffman() {
  delete[] dis_to_ic_;
  delete[] ic_begin_dis_;
  delete[] ic_extra_bits_;
  delete[] ic_code_len_;
  for (int32 i = 0; i < D_CODENUM; i++) delete ic_node_[i];
  delete[] ic_node_;
  delete[] ic_code_len_cnt_;
  delete[] ic_code_;
  delete[] next_len_code_int_;
  delete[] ic_code_int_;
}

void DisHuffman::AddFrequency(uint16 d) {
  // ����distanceΪ���Ӧ������������Ƶ��
  ic_node_[(d < 256) ? dis_to_ic_[d] : dis_to_ic_[256 + (d >> 7)]]
      ->frequency_++;
  return;
}

void DisHuffman::CreatNormalTree() {
  // ���������Ӧ�Ľڵ���뵽���ȶ���
  for (int32 i = 0; i < D_CODENUM; i++) {
    if (ic_node_[i]->frequency_ > 0) node_heap_.push(ic_node_[i]);
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
    } else {
      new_node->left_ = q;
      new_node->right_ = p;
    }
    node_heap_.push(new_node);
  }

  return;
}

void DisHuffman::CreatDisHfmTree() {
  // ��ȡ�������ȵı�������
  for (int32 i = 0; i < D_CODENUM; i++) {
    ic_code_len_cnt_[ic_code_len_[i]]++;
  }
  // ��ȡ�����������Huffman����
  GetHfmCode();
  // �½����ڵ�
  tree_root_ = new TreeNode;
  // ����Huffman��
  for (uint32 i = 0; i < D_CODENUM; i++) {
    if (ic_code_len_[i]) {
      Node p = tree_root_;
      bitstring code = ic_code_[i];
      for (auto it = code.begin(); it != code.end(); it++) {
        if (*it == '0') {
          if (it + 1 == code.end())
            p->left_ = ic_node_[i];
          else if (!p->left_)
            p->left_ = new TreeNode;
          p = p->left_;
        } else {
          if (it + 1 == code.end())
            p->right_ = ic_node_[i];
          else if (!p->right_)
            p->right_ = new TreeNode;
          p = p->right_;
        }
      }
    }
  }
  return;
}

void DisHuffman::GetCodeLength(Node p, uint16 l) {
  if (p) {
    if (p->code_ >= 0) {
      ic_code_len_[p->code_] = l;
      ic_code_len_cnt_[l]++;
    }
    GetCodeLength(p->left_, l + 1);
    GetCodeLength(p->right_, l + 1);
  }
  return;
}

void DisHuffman::GetHfmCode() {
  // ����ÿ�����ȵ���������������ߵı���
  int32 code = 0;
  for (int32 i = 1; i < D_MAX_CODELENGTH; i++) {
    code = (code + ic_code_len_cnt_[i - 1]) << 1;
    next_len_code_int_[i] = code;
  }
  // �õ�ÿ�������������
  for (int32 i = 0; i < D_CODENUM; i++) {
    if (ic_code_len_[i] > 0) {
      ic_code_int_[i] = next_len_code_int_[ic_code_len_[i]];
      next_len_code_int_[ic_code_len_[i]]++;
    }
  }
  // �õ�ÿ����������ַ���������
  for (int32 i = 0; i < D_CODENUM; i++) {
    ic_code_[i] =
        BitString::IntToBitstring(ic_code_int_[i], ic_code_len_[i], false);
  }

  // debug
  /*cout << "Code Frequency: " << endl;
  for (int32 i = 0; i < D_CODENUM; i++) {
          cout << i << "-" << ic_node_[i]->frequency_ << endl;
  }
  cout << "Code HfmCodeInt: " << endl;
  for (int32 i = 0; i < D_CODENUM; i++) {
          cout << i << "-" << ic_code_len_[i] << "-" << bitset<sizeof(uint32) *
  8>(ic_code_int_[i]) << endl;
  }*/
  /*cout << "Code HfmCodeBitstring: " << endl;
  for (int32 i = 0; i < D_CODENUM; i++) {
          cout << i << "-" << ic_code_len_[i] << "-" << ic_code_[i] << endl;
  }*/
  return;
}

void DisHuffman::ResetHfm() {
  // �������������
  memset(ic_code_len_, 0, sizeof(uint16) * D_CODENUM);
  memset(ic_code_len_cnt_, 0, sizeof(uint16) * D_MAX_CODELENGTH);
  // ���Huffman��������
  for (int32 i = 0; i < D_CODENUM; i++) ic_code_[i] = "";
  memset(ic_code_int_, 0, sizeof(uint32) * D_CODENUM);
  memset(next_len_code_int_, 0, sizeof(uint32) * D_MAX_CODELENGTH);
  while (!node_heap_.empty()) node_heap_.pop();
  // ���Huffman��
  DestroyTreeNode(tree_root_);
  tree_root_ = NULL;
  for (int32 i = 0; i < D_CODENUM; i++) {
    ic_node_[i]->frequency_ = 0;
    ic_node_[i]->code_ = i;
    ic_node_[i]->left_ = ic_node_[i]->right_ = NULL;
  }
  return;
}

void DisHuffman::DestroyTreeNode(Node p) {
  if (p) {
    DestroyTreeNode(p->left_);
    DestroyTreeNode(p->right_);
    if (p->code_ < 0) delete p;
  }
  return;
}
