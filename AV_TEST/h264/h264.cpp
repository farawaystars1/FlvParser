#include"h264.h"

namespace H264 {
	Nal_t::Nal_t() :forbidden_zero(0), ref_idc(0), nal_type(0), id(0), offset(0), length(0), start_code(NULL), nal_type_desc(NULL) { }

	Nal_t::~Nal_t() {
		if (start_code)delete start_code;
		if (nal_type_desc)delete nal_type_desc;
	}

	void Nal_t::dump() {
		cout << "forbiddent_zero:" << (int)forbidden_zero << " ";
		cout << "ref_idc:" << (int)ref_idc << " ";
		cout << "nal_type:" << (int)nal_type << " ";
		cout << "id:" << id << " ";
		cout << "off set:" << offset << " ";
		cout << "length:" << length << ' ';
		cout << "start_code:" << start_code << " ";
		cout << "nal_type_desc" << nal_type_desc << " ";
		cout << endl;
	}

	H264Parser::H264Parser() {}

	H264Parser::~H264Parser() {
		for (Nal_t* nal : nals_)
		{
			delete nal;
			nals_.clear();
		}
	}

	void H264Parser::parse(const char* file)
	{
		//clear
		static int id;
		id = 1;
		if (nals_.empty() != 0)nals_.clear();
		//提取文件内容
		char buf[1024];
		string file_str;
		std::ifstream in(file, ios::binary);
		while (in.good())
		{
			in.read(buf, sizeof(buf));
			file_str.append(buf, in.gcount());
		}
		in.close();

		size_t length = file_str.size();
		//uint8_t* p = reinterpret_cast<uint8_t*>(&file_str[0]);
		int last = 0;
		for (int i = 2; i <= length; i++)
		{
			if (i == length) {
				if (last) {
					int offset = 3;
					if (file_str[i - 3] == '\x00')offset = 4;
					_parseNal(file_str.data() + last - offset, i - last + offset, id++, last - offset);

				}
			}
			else if (file_str[i] == '\x01' && file_str[i - 1] == '\x00' && file_str[i - 2] == '\x00') {
				if (last)
				{
					size_t size = i - last - 3;
					if (file_str[i - 3])size++;
					int offset = 3;
					if (file_str[i - 3] == '\x00')offset = 4;
					_parseNal(file_str.data() + last - offset, size + offset, id++, last - offset);
				}
				last = i + 1;
			}
		}
	}

	void H264Parser::_parseNal(const char* buf, size_t length, int id, size_t offset)
	{


		const uint8_t* p = reinterpret_cast<const uint8_t*>(buf);
		Nal_t* nal = new Nal_t();
		if (p[2] == '\x01') {
			nal->length = length - 3;
			nal->start_code = strdup("000001");
			p += 3;
		}
		else {
			nal->length = length - 4;
			nal->start_code = strdup("00000001");
			p += 4;
		}
		nal->forbidden_zero = (p[0] >> 7) & 0x01;
		nal->ref_idc = (p[0] >> 5) & 0x03;
		nal->nal_type = p[0] & 0x1f;
		nal->data.assign(buf, length);
		nal->id = id;
		nal->offset = offset;

		switch (nal->nal_type)
		{
		case 7:nal->nal_type_desc = strdup("Sequence parameter set"); break;
		case 8:nal->nal_type_desc = strdup("Picture parameter set"); break;
		case 6:nal->nal_type_desc = strdup("Supplemental enhancement information"); break;
		case 5:nal->nal_type_desc = strdup("Coded slice of an IDR picture"); break;
		case 1:nal->nal_type_desc = strdup("Coded slice of a non-IDR picture"); break;
		default:
			nal->nal_type_desc = strdup("unknown nal type");
		}
		if (id < 50)
			nals_.push_back(nal);

	}

	void H264Parser::dump()const
	{
		for (Nal_t* nal : nals_)
		{
			nal->dump();
		}
	}
}
