#include"aacparser.h"

namespace AACParser {
	ADTS::ADTS() {
		memset(this, 0, sizeof(*this));
	}

	ADTS::~ADTS() {
		if (data_) { delete data_; data_ = NULL; }
	}

	//@offset p在aac文件中的偏移
	//return <0:failed 0:succeed >0:需要补充数据

	int ADTS::parse(const uint8_t* p, size_t length, size_t& offset)
	{
		static bool header_parsed = false;
		static size_t header_size = -1;
		if (header_parsed == false)
		{
			if (length < 9)return 1;
			//adts_fixed_header
			syncword = ((uint16_t)p[0] << 4) | p[1] >> 4;
			ID = (p[1] >> 3) & 0x01;
			layer = (p[1] >> 1) & 0x03;
			protection_absent = p[1] & 0x01;
			profile = (p[2] >> 6) & 0x03;
			sampling_frequency_index = (p[2] >> 2) & 0x0f;
			private_bit = (p[2] >> 1) & 0x01;
			channel_configuration = ((p[2] & 0x01) << 2) | (p[3] >> 6);
			original_copy = (p[3] >> 5) & 0x01;
			home = (p[3] >> 4) & 0x01;
			//variable header 
			copyright_identification_bit = (p[3] >> 3) & 0x01;
			copyright_identification_start = (p[3] >> 2) & 0x01;
			aac_frame_length = (((uint16_t)p[3] & 0x03) << 11) | ((uint16_t)p[4] << 3) | ((p[5] >> 5) & 0x07);
			adts_buffer_fullness = ((p[5] & 0x1f) << 6) | (p[6] >> 2) & 0x3f;
			number_of_raw_data_blocks_in_frame = p[6] & 0x03;
			header_size = 7;

			offset += 7;
			length -= 7;
			p += 7;
			//增加的2字节
			if (protection_absent == 0)
			{
				more = ((uint16_t)(p[0]) << 8) | p[1];

				offset += 2;
				length -= 2;
				p += 2;
				header_size += 2;
			}
			header_parsed = true;
			parse(p, length, offset);
		}
		else {
			if (length < aac_frame_length - header_size)return 1;
			size_ = aac_frame_length - header_size;
			data_ = new uint8_t[size_];
			memcpy(data_, p, size_);
			header_parsed = false;
			header_size = -1;

			offset += size_;
			p += size_;
			length -= size_;
		}
		return 0;
	}

	AACParser::AACParser() {}

	AACParser::~AACParser() {
		for (ADTS* adts : adtses_)
		{
			delete adts;
			adts = NULL;
			adtses_.clear();
		}
	}

	int AACParser::parse(const char* aac_file)
	{
		//读取aac文件数据到内存中
		std::ifstream in(aac_file, ios::binary);
		char buf[1024];
		string aac_content;
		while (in.good())
		{
			in.read(buf, sizeof(buf));
			aac_content.append(buf, in.gcount());
		}
		in.close();
		const uint8_t* p = reinterpret_cast<const uint8_t*>(aac_content.data());
		size_t offset = 0;
		while (true)
		{
			ADTS* adts = new(std::nothrow) ADTS;
			if (adts == NULL) {
				cerr << "alloc ADTS error" << endl;
				return -1;
			}
			int ret = adts->parse(p + offset, aac_content.size() - offset, offset);
			if (ret > 0) {
				cout << "need more data!" << endl;
				return 1;
			}
			else if (ret == 0)
			{
				adtses_.push_back(adts);
			}
			else {
				cerr << "adts parse error!" << endl;
				return -1;
			}
		}
	}
}
