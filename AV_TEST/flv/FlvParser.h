#ifndef __FLVPARSER__H
#define __FLVPARSER__H

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<memory.h>
#include<assert.h>
using namespace std;

namespace FlvParser {

	class FlvHeader {
	public:
		FlvHeader();
		int parse(const uint8_t* data, size_t size);
	private:
		char signature_[3];
		uint8_t version_;
		uint8_t flags_;
		size_t size_;
	};

	class Tag {
	private:
		class AudioTag {
		public:
			AudioTag();
			void parse(uint8_t* p, size_t length);
		private:
			uint8_t format_ : 4;//0 :linear pcm ,platform endian 1:adpcm 2:mp3 ...10:aac ... 15:device-specific sound
			uint8_t frequency_ : 2;
			uint8_t sample_size_ : 1;//压缩过的音频都为16bit
			uint8_t audio_type_ : 1;//0:mono 1:stereo
			uint8_t* data_;
			size_t size_;

			//aac专有可变头
			uint8_t aac_packet_type_;//0:aac sequence header 1:aac raw //此时 if aac_packet_type==0: audios pecifig config  else if aac_packet_type==1:raw aac frame data
		};
		class ScriptTag {
			enum AMF_TYPE :uint8_t {
				Number,//double
				Boolean,//uint8_t
				String,//script data string
				Object,//script data object
				MovieClip,//reserved,not supported
				Null,
				Undefined,
				Reference,//uint16_t
				ECMA_array,//script data script array  uint32_t
				Object_end_marker,
				Date,//script data date
				Long_string,//script data long string
			};
		public:
			ScriptTag();
			void parse(uint8_t* p, size_t length);
		private:
			vector<pair<string, string>>  meta_datas_;
			char tag_end_[3];
		};
		class VideoTag {
		public:
			VideoTag();
			void parse(uint8_t* p, size_t length);
		private:
			uint8_t frame_type_ : 4; //1:jpeg 2:sorenson h.263 3: screen video 4: on2 vp6 5: on2 vp6 with alpha channet 7:AVC
			uint8_t encode_id_ : 4;
			uint8_t* data_;
			size_t size_;

			//h264专有可变头
			uint8_t avc_packet_type_;
			uint32_t composition_time_;
		};

	public:
		Tag();
		//@return parse的数据量
		int parse(const uint8_t* data, size_t size);
		void dump()
		{
			cout <<"tag_type:" <<(int)this->type_ << endl;
		}
		~Tag();
	private:
		uint8_t type_;//tag类型 0x08:audio 0x09:video 0x12:script 
		size_t size_;//data大小 3字节
		size_t timestamps_; // 时间戳 3字节+1扩展字节
		uint32_t stream_id_;//3个字节总是0
		uint8_t* data_;
		AudioTag* audio_tag_;
		VideoTag* video_tag_;
		ScriptTag* script_tag_;
	};
	class FlvParser {
	public:
		FlvParser();
		~FlvParser();
		int parse(const char* file_name);
		void dump()
		{
			for (Tag* tag : tags_)
			{
				tag->dump();
			}
		}
	private:
		FlvHeader* header_;
		vector<Tag*> tags_;
		vector<size_t> all_prev_tag_size_;
	};

}

#endif //__FLVPARSER__H