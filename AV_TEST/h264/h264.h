#ifndef __H264__H
#define __H264__h
#include<iostream>
#include<fstream>
#include<string>
#include<string.h>
#include<vector>
using namespace std;

namespace H264 {
	struct Nal_t {
		uint8_t forbidden_zero : 1;
		uint8_t ref_idc : 2;
		uint8_t nal_type : 5;
		string data;
		int id;
		size_t offset;
		size_t length;
		const char* start_code;
		const char* nal_type_desc;
		Nal_t();
		~Nal_t();
		void dump();
	};
	class H264Parser {
	public:
		H264Parser();
		~H264Parser();
		void parse(const char* file);
		void dump()const;
	private:
		void _parseNal(const char* buf, size_t length, int id, size_t offset);
	private:
		vector<Nal_t*> nals_;

	};
}

#endif //__H264__H