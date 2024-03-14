#include "FlvParser.h"


namespace FlvParser {
	static uint16_t show16(const uint8_t* data) {
		return ((uint16_t)data[0] << 8) | data[1];
	}
	static uint32_t show24(const uint8_t* data) {
		return ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | data[2];
	}
	static uint32_t show32(const uint8_t* data) {
		return ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | data[3];
	}
	static uint64_t show64(const uint8_t* data) {
		return ((uint64_t)data[0] << 56) | ((uint64_t)data[1] << 48) | ((uint64_t)data[2] << 42) | ((uint64_t)data[3] << 36) | ((uint64_t)data[4] << 24) | ((uint64_t)data[5] << 16) | ((uint64_t)data[6] << 8) | data[7];
	}
	static double show64_double(const uint8_t* data)
	{
		uint64_t ret = show64(data);
		return  *reinterpret_cast<double*>(&ret);
	}

	FlvHeader::FlvHeader() {
		memset(this, 0, sizeof(*this));
	}

	int FlvHeader::parse(const uint8_t* data, size_t size) {
		if (size < 9)return 0;
		else {
			signature_[0] = data[0];
			signature_[1] = data[1];
			signature_[2] = data[2];
			version_ = data[3];
			flags_ = data[4];
			this->size_ = show32(data + 5);
			return 9;
		}
	}

	Tag::Tag() { memset(this, 0, sizeof(*this)); }

	//@return parse的数据量

	int Tag::parse(const uint8_t* data, size_t size)
	{
		static const size_t header_size = 11;
		if (size < header_size)return 0;
		this->size_ = show24(data + 1);
		if (size < this->size_ + header_size)return 0;
		type_ = data[0];
		timestamps_ = show24(data + 4) | (data[7] << 24);
		stream_id_ = show24(data + 8);
		this->data_ = new uint8_t[this->size_];
		memcpy(this->data_, data + header_size, this->size_);
		switch (type_)
		{
		case 0x08:audio_tag_ = new AudioTag(); audio_tag_->parse(this->data_, this->size_); break;
		case 0x09:video_tag_ = new VideoTag(); video_tag_->parse(this->data_, this->size_); break;
		case 0x12:script_tag_ = new ScriptTag(); script_tag_->parse(this->data_, this->size_); break;
		default:
			cerr << "unknown type!" << endl;
		}
		return this->size_ + header_size;
	}

	Tag::~Tag()
	{
		if (data_) { delete data_; data_ = NULL; }
		if (audio_tag_) { delete audio_tag_; audio_tag_ = NULL; }
		if (video_tag_) { delete video_tag_; video_tag_ = NULL; }
		if (script_tag_) { delete script_tag_; script_tag_ = NULL; }
	}

	Tag::AudioTag::AudioTag() { memset(this, 0, sizeof(*this)); }

	void Tag::AudioTag::parse(uint8_t* p, size_t length)
	{
		static const int header_size = 1;
		static const int aac_header_size = 1;
		format_ = (p[0] & 0xf0) >> 4;
		frequency_ = (p[0] & 0x0C) >> 2;
		sample_size_ = (p[0] & 0x02) >> 1;
		audio_type_ = p[0] & 0x01;
		data_ = p + header_size;
		size_ = length - header_size;
		//aac
		if (format_ == 10) {
			aac_packet_type_ = p[1];
			data_ += aac_header_size;
			size_ -= aac_header_size;
		}
	}

	Tag::ScriptTag::ScriptTag() { memset(tag_end_, 0, sizeof(tag_end_)); }

	void Tag::ScriptTag::parse(uint8_t* p, size_t length)
	{
		AMF_TYPE AMF_type = (AMF_TYPE)-1;
		string key;
		size_t key_length = -1;
		string str_value;
		double double_value = -1;
		uint8_t bool_value = false;
		size_t value_length = -1;
		uint32_t AMF_array_length = -1;
		//onMetaData
		assert(p[0] == 0x02);
		key_length = show16(p + 1);
		key.assign(reinterpret_cast<char*>(p + 1 + 2), key_length);
		assert(p[1 + 2 + key_length] == 0x08);
		AMF_array_length = show32(p + 1 + 2 + key_length + 1);
		str_value = to_string(AMF_array_length);
		meta_datas_.push_back(make_pair(key, str_value));
		p += 1 + 2 + key_length + 1 + 4;//1:AMF_TYPE 3:3字节 2:key_length
		while (AMF_array_length--)
		{
			key_length = show16(p);
			key.assign(reinterpret_cast<char*>(p + 2), key_length);
			p += 2 + key_length;
			switch (p[0])
			{
			case String:
			case Long_string:
			case Date:
				value_length = show16(p + 1);
				str_value.assign(reinterpret_cast<char*>(p + 1 + 2), value_length);
				p += 1 + 2 + value_length;
				meta_datas_.push_back(make_pair(key, str_value));
				break;
			case Number:
				str_value = to_string(show64_double(p + 1));
				p += 1 + 8;
				meta_datas_.push_back(make_pair(key, str_value));
				break;
			case Boolean:
				str_value = to_string(*(p + 1));
				p += 1 + 1;
				meta_datas_.push_back(make_pair(key, str_value));
				break;
			case Object:
				cout << "object no parse" << endl; return;
				break;
			case MovieClip:cout << "MovieClip no parse" << endl; return;
				break;
			case Null: meta_datas_.push_back(make_pair(key, "Null")); p += 1;
				break;
			case Undefined: meta_datas_.push_back(make_pair(key, "Undefined")); p += 1;
				break;
			case Reference:
				str_value = to_string(show16(p + 1));
				p += 1;
				meta_datas_.push_back(make_pair(key, str_value));
				break;
			case Object_end_marker:
				cout << "Object_end_marker no parse" << endl; return;
				break;
			default:
				//meta_datas.push_back(make_pair(key, ""));
				cerr << "unsupported amf_type" << endl;
				return;
			}
		}
		tag_end_[0] = p[0];
		tag_end_[1] = p[1];
		tag_end_[2] = p[2];
	}

	Tag::VideoTag::VideoTag() { memset(this, 0, sizeof(*this)); }

	void Tag::VideoTag::parse(uint8_t* p, size_t length)
	{
		static const int header_size = 1;
		static const int h264_header_size = 4;
		frame_type_ = (p[0] & 0xf0) >> 4;
		encode_id_ = p[0] & 0x0f;
		data_ = p + header_size;
		size_ = length - header_size;
		if (encode_id_ == 0x07)
		{
			avc_packet_type_ = p[1];
			composition_time_ = show24(p + 2);
			data_ += h264_header_size;
			size_ -= h264_header_size;
		}

	}

	FlvParser::FlvParser() :header_(NULL) {}

	FlvParser::~FlvParser() {
		if (header_)delete header_;
		for (Tag* tag : tags_)
		{
			delete tag;
			tags_.clear();
		}
	}

	int FlvParser::parse(const char* file_name)
	{
		std::ifstream in(file_name, std::ios::binary);
		char buf[1024];
		string file_content;
		size_t offset = 0;
		while (in.good())
		{
			in.read(buf, sizeof(buf));
			file_content.append(buf, in.gcount());
		}
		in.close();//读取文件结束
		const uint8_t* p = reinterpret_cast<const uint8_t*>(file_content.data());
		size_t size = file_content.size();
		header_ = new FlvHeader();
		int ret = header_->parse(p, size);
		if (ret == 0) {
			delete header_;
			header_ = NULL;
			return 0;
		}
		else offset += ret;

		if (offset + 4 <= size)
		{
			size_t pre_tag_size = show32(p + offset);
			all_prev_tag_size_.push_back(pre_tag_size);
			offset += 4;
		}

		while (true)
		{
			Tag* tag = new Tag();
			ret = tag->parse(p + offset, size - offset);
			if (ret == 0) {
				delete tag;
				tag = NULL;
				break;
			}
			else {
				offset += ret;
				tags_.push_back(tag);
			}
			if (offset + 4 <= size)
			{
				size_t pre_tag_size = show32(p + offset);
				all_prev_tag_size_.push_back(pre_tag_size);
				offset += 4;
			}
		}
		return static_cast<int>(offset);
	}
}
