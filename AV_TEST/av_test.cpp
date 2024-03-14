// module.cpp: 定义应用程序的入口点。
//

#include "av_test.h"
#include"flv/FlvParser.h"
#include"aac/aacparser.h"
#include"h264/h264.h"
//#include"fdk-aac/aacdecoder_lib.h"
using namespace std;


int main()
{
	H264::H264Parser parser;
	parser.parse("/mnt/c/users/administrator/desktop/child.h264");
	parser.dump();
	return 0;
}
