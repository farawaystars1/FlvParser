#include<iostream>
#include<memory.h>

#ifndef __GBK__H
#define __GBK__H
using namespace std;

namespace GBK {
	int  printGBKCharSet(int index);
	//@param p:���ֵ�ַ ȡ2���ֽ�
	bool isHanzi(const uint8_t* p);
}

#endif //__GBK__H