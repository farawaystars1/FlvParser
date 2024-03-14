#include<iostream>
#include<memory.h>

#ifndef __GBK__H
#define __GBK__H
using namespace std;

namespace GBK {
	int  printGBKCharSet(int index);
	//@param p:汉字地址 取2个字节
	bool isHanzi(const uint8_t* p);
}

#endif //__GBK__H