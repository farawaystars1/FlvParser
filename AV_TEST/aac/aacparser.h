#ifndef __AACPARSER__H
#define __AACPARSER__H
#include<fstream>
#include<iostream>
#include<memory.h>
#include<vector>
using namespace std;

namespace AACParser {
	class ADTS {
	public:
		ADTS();
		~ADTS();
		//@offset p在aac文件中的偏移
		//return <0:failed 0:succeed >0:需要补充数据
		int parse(const uint8_t* p, size_t length, size_t& offset);
	private:
		//adts_fixed_header
		uint16_t syncword : 12;
		uint16_t ID : 1;
		uint16_t layer : 2;
		uint16_t protection_absent : 1;
		uint16_t profile : 2;
		uint16_t sampling_frequency_index : 4;
		uint16_t private_bit : 1;
		uint16_t channel_configuration : 3;
		uint16_t original_copy : 1;
		uint16_t home : 1;
		//variable header 
		uint32_t copyright_identification_bit : 1;
		uint32_t copyright_identification_start : 1;
		uint32_t aac_frame_length : 13;
		uint32_t adts_buffer_fullness : 11;
		uint32_t number_of_raw_data_blocks_in_frame : 2;

		uint16_t more;//if pretection_absent==0 含有此字段

		uint8_t* data_;
		uint32_t  size_;
	};
	class AACParser {
	public:
		AACParser();
		~AACParser();
		int parse(const char* aac_file);
	private:
		vector<ADTS*> adtses_;
	};
}


#endif //__AACPARSER__H