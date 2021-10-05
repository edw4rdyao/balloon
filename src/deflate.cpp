#include "deflate.h"
// TODO: add english commont...

Deflate::Deflate():
	window_(new uint8[2 * WSIZE]),			lz77_hash_(new HashTable(WSIZE)), 
	dis_hfm_tree_(new DisHuffman), 			ll_hfm_tree_(new LlHuffman),
	flag_buffer_(new uint8[F_BUFFSIZE]),	flag_buffer_cnt_ (0),
	ll_buffer_(new uint8[L_BUFFSIZE]), 		ll_buffer_cnt_ (0),
	dis_buffer_(new uint16[D_BUFFSIZE]),	dis_buffer_cnt_ (0),
	out_buffer_(new uint8[D_O_BUFFSIZE]), 	out_buffer_cnt_ (0),
	flag_data_(0),							flag_bit_cnt_(0),
	out_data_(0), 							out_bit_cnt_(0),
	fp_out_ (NULL), 						fp_in_( NULL){
}

Deflate::~Deflate() {
	delete[] window_;
	delete lz77_hash_;
	delete dis_hfm_tree_;
	delete ll_hfm_tree_;
	delete[] flag_buffer_;
	delete[] ll_buffer_;
	delete[] dis_buffer_;
	delete[] out_buffer_;
}

/*=========================================================

=========================================================*/
void Deflate::Compress(	const char* origin_file_path_and_name, 
						const char* compressed_file_path_and_name, 
						const char* origin_file_name) {
	// open the original file and the file to be compressed
	fp_in_ = fopen(origin_file_path_and_name, "rb");
	if(NULL == fp_in_){
		std::cout << "Open file: " << origin_file_path_and_name << " failed!\n";
		assert(0);
	}
	fp_out_ = fopen(compressed_file_path_and_name, "wb");
	if(NULL == fp_out_){
		std::cout << "Open file: " << compressed_file_path_and_name << " failed!\n";
		assert(0);
	}
	// output the original filename and the extention name into the compressfile...
	for(int32 i = 0; origin_file_name[i] != '\0'; i++){
		fputc(origin_file_name[i], fp_out_);
	}
	// output a '\0' as file_name end flag
	fputc('\0', fp_out_);

	// output the original file's size into the compressed file
	fseek(fp_in_, 0, SEEK_END);
	uint64 file_size = ftell(fp_in_);
	fseek(fp_in_, 0, SEEK_SET);
	fwrite(&file_size, sizeof(uint64), 1, fp_out_);

	// 
	auto look_ahead = fread(window_, 1, WSIZE * 2, fp_in_);

	// ��ʼ����ϣ��ַ
	uint16 hash_address = 0;
	lz77_hash_->HashFunction(hash_address, window_[0]);
	lz77_hash_->HashFunction(hash_address, window_[1]);

	uint16 str_start = 0;		// ��ǰƥ����ҵ�λ��
	uint16 match_head = 0;		// ��ǰ�ַ���ƥ����ҵ�ͷλ�ã���Ϊ0��û���ҵ�ƥ�䴮
	uint16 t_match_len = 0;	// ��ǰƥ�䳤��
	uint16 t_match_dis = 0;	// ��ǰƥ�����

	while (look_ahead) {
		// �������ַ��ַ����е����һ���ַ��Լ����ϣ��ַ
		lz77_hash_->Insert(match_head, window_[str_start + 2], str_start, hash_address);
		t_match_len = 0;
		t_match_dis = 0;
		// ����ȡ��ƥ��ͷ��Ϊ0�������ƥ�����е��ƥ�䳤��
		if (match_head) {
			t_match_len = GetMaxMatchLength(match_head, t_match_dis, str_start, look_ahead);
		}
		// ��֤�Ƿ��ҵ���ƥ�䴮
		if (t_match_len >= MIN_MATCHLENGTH) {
			// ��<���ȣ�����>��д�뻺����
			ll_buffer_[ll_buffer_cnt_++] = t_match_len - 3;
			dis_buffer_[dis_buffer_cnt_++] = t_match_dis;
			// ͳ��distance lengthƵ�����ڹ���Huffman��
			dis_hfm_tree_->AddFrequency(t_match_dis - 1);
			ll_hfm_tree_->AddFrequency(t_match_len - 3, true);
			// д���ǻ�����
			WriteFlag(true);
			// �滻�󽫱��滻�Ĵ����������ַ��ַ������뵽��ϣ����
			for (int32 i = 0; i < t_match_len - 1; i++) {
				str_start++;
				lz77_hash_->Insert(match_head, window_[str_start + 2], str_start, hash_address);
			}
			str_start++;
			look_ahead -= t_match_len;
		}
		else {
			// ˵��û���ҵ���Чƥ�䴮�����ַ�ԭ�������ѹ�������ļ���
			ll_buffer_[ll_buffer_cnt_++] = window_[str_start];
			// ͳ��literalƵ�����ڹ���Huffman��
			ll_hfm_tree_->AddFrequency(window_[str_start], false);
			str_start++;
			look_ahead--;
			// д�����ļ�
			WriteFlag(false);
		}
		// �����л������ĳ���С����С����ʱ�����ڻ���
		if (look_ahead <= MIN_LOOKAHEAD) {
			MoveWindow(look_ahead, str_start);
		}
	}

	// �����ı��λ������8bit��ֱ������д�뻺����
	if (flag_bit_cnt_) {
		flag_data_ <<= (8 - flag_bit_cnt_);
		flag_buffer_[flag_buffer_cnt_++] = flag_data_;
		flag_data_ = 0;
		flag_bit_cnt_ = 0;
	}
	// д�����һ��
	OutHuffman();
	if (out_buffer_cnt_) {
		fwrite(out_buffer_, sizeof(uint8), out_buffer_cnt_, fp_out_);
	}

	fclose(fp_out_);
	return;
}

/*=========================================================

=========================================================*/
uint16 Deflate::GetMaxMatchLength(uint16 match_head, uint16& t_match_dis, uint16 str_start, uint32 look_ahead) {
	uint16 t_match_len = 0;				// ÿ��ƥ��ĳ���
	uint16 max_match_start = 0;					// ��ǰƥ���ڲ��һ������е���ʼλ��
	uint8 match_times = MAX_MATCH_TIMES;	// ����ƥ�����
	uint16 max_match_len = 0;				// ���ƥ�䳤��

	while (match_head > 0 && match_times--) {
		uint8* t_p_start = window_ + str_start;	
		uint8* t_p_end = t_p_start + ((look_ahead <= MAX_MATCHLENGTH) ? (look_ahead - 1) : MAX_MATCHLENGTH);
		uint8* t_p_match_start = window_ + match_head;
		t_match_len = 0;
		// ̫Զ��ƥ�����
		if (str_start - match_head > WSIZE - MIN_LOOKAHEAD) {
			break;
		}
		// ���ַ�����ƥ��
		while (t_p_start < t_p_end && *t_p_start == *t_p_match_start) {
			t_p_start++;
			t_p_match_start++;
			t_match_len++;
		}
		// һ��ƥ������ˣ��жϲ���¼�ƥ��
		if (t_match_len > max_match_len) {
			max_match_len = t_match_len;
			max_match_start = match_head;
		}
		// ƥ��������һ��ƥ��λ��
		match_head = lz77_hash_->prev_[match_head & HASHMASK];
	}

	// ��������ƥ���Ӧ�ľ���
	t_match_dis = str_start - max_match_start;
	return max_match_len;
}

/*=========================================================
���ܣ�	��������
����ֵ��	void
������	��������������ƶ�32K
=========================================================*/
void Deflate::MoveWindow(size_t& look_ahead, uint16& str_start) {
	// ���ж��Ƿ񵽴����ļ�ĩ��
	if (str_start >= WSIZE) {
		// ���Ҵ��е������������Ҵ���0
		memcpy(window_, window_ + WSIZE, WSIZE);
		memset(window_ + WSIZE, 0, WSIZE);
		str_start -= WSIZE;
		// ���¹�ϣ��
		lz77_hash_->Updata();
		// ���Ҵ�����ǰ����������������
		if (!feof(fp_in_)) {
			look_ahead += fread(window_ + WSIZE, 1, WSIZE, fp_in_);
		}
	}
}


/*=========================================================

=========================================================*/
void Deflate::WriteFlag(bool is_encoded) {
	// ������һλ����Ϊlength�����¼1
	flag_data_ <<= 1;
	if (is_encoded)	flag_data_ |= 1;
	flag_bit_cnt_++;
	// �������8bitʱ����fDataд�뻺����
	if (flag_bit_cnt_ == 8) {
		flag_buffer_[flag_buffer_cnt_++] = flag_data_;
		flag_data_ = 0;
		flag_bit_cnt_ = 0;
	}
	// ����ÿ�������flag_buffer_��С�ﵽ8k����ll_buffer_��С�ﵽ64k��
	if (flag_buffer_cnt_ == F_BUFFSIZE) {
		OutHuffman();
	}
	return ;
}

/*=========================================================

=========================================================*/
void Deflate::OutHuffman(){
	// ��256��������
	ll_hfm_tree_->ic_node_[256]->frequency_++;
	// �õ�distance����lenght/literal����Huffman����
	dis_hfm_tree_->CreatNormalTree();
	dis_hfm_tree_->GetCodeLength(dis_hfm_tree_->tree_root_, 0);
	dis_hfm_tree_->GetHfmCode();
	ll_hfm_tree_->CreatNormalTree();
	ll_hfm_tree_->GetCodeLength(ll_hfm_tree_->tree_root_, 0);
	ll_hfm_tree_->GetHfmCode();

	/*double avg = 0, fre = 0;
	for (int32 i = 0; i < D_CODENUM; i++) {
		avg +=((double)dis_hfm_tree_->ic_code_len_[i] + (double)dis_hfm_tree_->ic_extra_bits_[i]) * dis_hfm_tree_->ic_node_[i]->frequency_;
		fre += (double)dis_hfm_tree_->ic_node_[i]->frequency_;
	}
	avg /= fre;
	cout <<  "dis tree: " << avg << endl;*/

	/*avg = 0, fre = 0;
	for (int32 i = 0; i < LL_CODENUM; i++) {
		if (i >= 257) {
			avg += ((double)ll_hfm_tree_->ic_code_len_[i] + (double)ll_hfm_tree_->ic_extra_bits_[i - 257]) * ll_hfm_tree_->ic_node_[i]->frequency_;
		}
		else {
			avg += (double)ll_hfm_tree_->ic_code_len_[i]  * ll_hfm_tree_->ic_node_[i]->frequency_;
		}
		fre += (double)ll_hfm_tree_->ic_node_[i]->frequency_;
	}
	avg /= fre;
	cout << "l tree: " << avg << endl;*/

	// д�뽨��Huffman������Ҫ����Ϣ
	fwrite(ll_hfm_tree_->ic_code_len_, sizeof(uint16), LL_CODENUM, fp_out_);
	fwrite(dis_hfm_tree_->ic_code_len_, sizeof(uint16), D_CODENUM, fp_out_);
	// ѭ��flag��ÿһλ����Huffman����д�������������
	uint32 ll = 0, d = 0, cnt = 0;
	for (uint32 i = 0; i < flag_buffer_cnt_; i++) {
		uint8 f = flag_buffer_[i];
		for (uint32 j = 0; j < 8; j++) {
			if (f & 0x80) {
				// ��length�ı���д��
				uint8 len = ll_buffer_[ll++];
				uint16 len_ic = ll_hfm_tree_->len_to_ic_[len];
				bitstring len_ic_code = ll_hfm_tree_->ic_code_[257 + len_ic];
				for (auto it = len_ic_code.begin(); it != len_ic_code.end(); it++) {
					if (*it == '1') OutBit(1);
					else OutBit(0);
				}
				uint32 extra = len - ll_hfm_tree_->ic_begin_ll_[len_ic];
				for (int32 k = 0; k < ll_hfm_tree_->ic_extra_bits_[len_ic]; k++) {
					if (extra & 1) OutBit(1);
					else OutBit(0);
					extra >>= 1;
				}
				// ������д��distance�ı���
				uint16 dis = dis_buffer_[d++];
				uint16 dis_ic = (dis - 1 < 256) ? dis_hfm_tree_->dis_to_ic_[dis - 1] : dis_hfm_tree_->dis_to_ic_[256 + ((dis - 1) >> 7)];
				bitstring dis_ic_code = dis_hfm_tree_->ic_code_[dis_ic];
				for (auto it = dis_ic_code.begin(); it != dis_ic_code.end(); it++) {
					if (*it == '1') OutBit(1);
					else OutBit(0);
				}
				extra = dis - dis_hfm_tree_->ic_begin_dis_[dis_ic];
				for (int32 k = 0; k < dis_hfm_tree_->ic_extra_bits_[dis_ic]; k++) {
					if (extra & 1) OutBit(1);
					else OutBit(0);
					extra >>= 1;
				}
			}
			else {
				// ��literal����д��
				uint8 lit = ll_buffer_[ll ++];
				bitstring lit_ic_code = ll_hfm_tree_->ic_code_[lit];
				for (auto it = lit_ic_code.begin(); it != lit_ic_code.end(); it++) {
					if (*it == '1') OutBit(1);
					else OutBit(0);
				}
			}
			// ��ֹ���һ���flag_buffer_�����һ���ֽ�δ��8�ֽھ����룬������Ҫ��lBuffΪ׼����
			cnt++;
			if (cnt == ll_buffer_cnt_) break;
			f <<= 1;
		}
	}
	// д��256��ʾ�ÿ�Ľ���
	bitstring end_ic_code = ll_hfm_tree_->ic_code_[256];
	for (auto it = end_ic_code.begin(); it != end_ic_code.end(); it++) {
		if (*it == '1') OutBit(1);
		else OutBit(0);
	}
	// ��������8λ��Ҳֱ��д��
	if (out_bit_cnt_) {
		out_data_ <<= (8 - out_bit_cnt_);
		out_buffer_[out_buffer_cnt_++] = out_data_;
		out_data_ = 0;
		out_bit_cnt_ = 0;
	}
	if (out_buffer_cnt_) {
		fwrite(out_buffer_, sizeof(uint8), out_buffer_cnt_, fp_out_);
		out_buffer_cnt_ = 0;
	}
	// ����Huffman��
	dis_hfm_tree_->ResetHfm();
	ll_hfm_tree_->ResetHfm();
	// ���û�����
	flag_buffer_cnt_ = 0;
	dis_buffer_cnt_ = 0;
	ll_buffer_cnt_ = 0;
	return;
}

/*=========================================================

=========================================================*/
void Deflate::OutBit(bool bit){
	out_data_ <<= 1;
	if (bit) out_data_ |= 1;
	out_bit_cnt_++;
	if (out_bit_cnt_ == 8) {
		out_buffer_[out_buffer_cnt_++] = out_data_;
		if (out_buffer_cnt_ == D_O_BUFFSIZE) {
			fwrite(out_buffer_, sizeof(uint8), D_O_BUFFSIZE, fp_out_);
			out_buffer_cnt_ = 0;
		}
		out_data_ = 0;
		out_bit_cnt_ = 0;
	}
	return;
}
