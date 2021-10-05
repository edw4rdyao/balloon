#include "inflate.h"

Inflate::Inflate()
	:dis_hfm_tree_(new DisHuffman), 		ll_hfm_tree_(new LlHuffman),
	in_data_(0),							in_bit_cnt_ (0),
	out_buffer_(new uint8[I_O_BUFFSIZE]),	out_buffer_cnt_ (0),
	fp_out_ (NULL),							fp_in_ (NULL){
}

Inflate::~Inflate(){
	delete dis_hfm_tree_;
	delete ll_hfm_tree_;
	delete[] out_buffer_;
}

void Inflate::Uncompress(	const char* compressed_file_path_and_name, 
							char* uncompressed_file_path){
	// open the compressed file
	fp_in_ = fopen(compressed_file_path_and_name, "rb");
	if(NULL == fp_in_){
		std::cout << "Open file: " << compressed_file_path_and_name << " failed!\n";
		assert(0);
	}
	// read the original filename int the input file
	char original_file_name[_MAX_FNAME];
	char ch;	int32 i = 0;
	do{
		ch = getc(fp_in_);
		original_file_name[i++] = ch;
	}while(ch != '\0');// when read the '\0', the flag of end of the original filename
	strcat(uncompressed_file_path, original_file_name);
	// open the uncompressed file
	fp_out_ = fopen(uncompressed_file_path, "wb");
	if(NULL == fp_out_){
		std::cout << "Open file: " << uncompressed_file_path << " failed!\n";
		assert(0);
	}

	uint64 file_size = 0;	// ԭ�ļ���С
	uint64 inflate_cnt = 0;	// �ѽ�ѹ�ļ���С

	// read the original file size
	fread(&file_size, sizeof(uint64), 1, fp_in_);

	while (inflate_cnt < file_size) {
		// ��Ŀ�ʼ����ȡ�ع�Huffman������Ҫ����Ϣ
		fread(ll_hfm_tree_->ic_code_len_, sizeof(uint16), LL_CODENUM, fp_in_);
		fread(dis_hfm_tree_->ic_code_len_, sizeof(uint16), D_CODENUM, fp_in_);
		fread(&in_data_, sizeof(uint8), 1, fp_in_);
		in_bit_cnt_ = 0;
		// �ع�Huffman��
		dis_hfm_tree_->CreatDisHfmTree();
		ll_hfm_tree_->CreatLlHfmTree();
		while (true) {
			// �����length/literal������
			uint16 encode_ll = EncodeALl();	
			if (encode_ll >= 0 && encode_ll <= 255) {
				// Ϊliteral
				uint8 literal = encode_ll;
				out_buffer_[out_buffer_cnt_++] = literal;
				if (out_buffer_cnt_ == I_O_BUFFSIZE) {
					fwrite(out_buffer_, sizeof(uint8), I_O_BUFFSIZE / 2, fp_out_);
					memcpy(out_buffer_, out_buffer_ + I_O_BUFFSIZE / 2, I_O_BUFFSIZE / 2 * sizeof(uint8));
					out_buffer_cnt_ -= I_O_BUFFSIZE / 2;
				}
				// �����ѽ����С
				inflate_cnt++;
			}
			else if (encode_ll >= 257 && encode_ll <= 285) {
				// Ϊlength������Ҫ����extra
				uint16 len = ll_hfm_tree_->ic_begin_ll_[encode_ll - 257], extra = 0;
				for (int32 i = 0; i < ll_hfm_tree_->ic_extra_bits_[encode_ll - 257]; i++) {
					if (InBit()) extra |= (1 << i);
				}
				len += extra;
				len += 3;
				// �����distance
				uint16 dis = EncodeADis();
				// �����ѽ����С
				inflate_cnt += len;
				// �ж��Ƿ����д�뻺����
				if (out_buffer_cnt_ + len >= I_O_BUFFSIZE) {
					fwrite(out_buffer_, sizeof(uint8), I_O_BUFFSIZE / 2, fp_out_);
					memcpy(out_buffer_, out_buffer_ + I_O_BUFFSIZE / 2, I_O_BUFFSIZE / 2 * sizeof(uint8));
					out_buffer_cnt_ -= I_O_BUFFSIZE / 2;
				}
				uint32 out_buffer_cnt_pre = out_buffer_cnt_;
				if (dis < len) {
					uint32 rest_len = len;
					while (rest_len >= dis) {
						memcpy(out_buffer_ + out_buffer_cnt_, out_buffer_ + out_buffer_cnt_ - dis, dis * sizeof(uint8));
						out_buffer_cnt_ += dis;
						rest_len -= dis;
					}
					memcpy(out_buffer_ + out_buffer_cnt_, out_buffer_ + out_buffer_cnt_pre - dis, rest_len * sizeof(uint8));
					out_buffer_cnt_ += rest_len;
				}
				else {
					memcpy(out_buffer_ + out_buffer_cnt_, out_buffer_ + out_buffer_cnt_ - dis, len * sizeof(uint8));
					out_buffer_cnt_ += len;
				}
			}
			else if (encode_ll == 256) {
				// ��Ľ���
				break;
			}
		}
		// �µ�һ����Ŀ�ʼ������Huffman��
		dis_hfm_tree_->ResetHfm();
		ll_hfm_tree_->ResetHfm();
	}
	if (out_buffer_cnt_) {
		fwrite(out_buffer_, sizeof(uint8), out_buffer_cnt_, fp_out_);
	}
	fclose(fp_in_);
	fclose(fp_out_);
	return;
}

uint16 Inflate::EncodeALl(){
	Node p = ll_hfm_tree_->tree_root_;
	while (p->code_ < 0) {
		if (InBit()) p = p->right_;
		else p = p->left_;
	}
	return uint16(p->code_);
}

uint16 Inflate::EncodeADis(){
	Node p = dis_hfm_tree_->tree_root_;
	while (p->code_ < 0) {
		if (InBit()) p = p->right_;
		else p = p->left_;
	}
	uint16 encode_ic = p->code_;
	uint16 dis = dis_hfm_tree_->ic_begin_dis_[encode_ic], extra = 0;
	for (int32 i = 0; i < dis_hfm_tree_->ic_extra_bits_[encode_ic]; i++) {
		if(InBit()) extra |= (1 << i);
	}
	dis += extra;
	return dis;
}

uint8 Inflate::InBit(){
	uint8 bit = 0;
	if (in_bit_cnt_ == 8) {
		fread(&in_data_, sizeof(uint8), 1, fp_in_);
		in_bit_cnt_ = 0;
	}
	if (in_data_ & 0x80) bit = 1;
	in_data_ <<= 1;
	in_bit_cnt_++;
	return bit;
}

