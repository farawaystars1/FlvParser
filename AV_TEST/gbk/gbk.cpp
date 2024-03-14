#include"gbk.h"

namespace GBK {
	int  printGBKCharSet(int index)
	{
		uint8_t first_start;
		uint8_t first_end;
		uint8_t second_start;
		uint8_t second_end;
		if (index < 1 || index>5)return -1;
		switch (index) {
		case 1:first_start = '\xA1'; first_end = '\xA9'; second_start = '\xA1'; second_end = '\xF1'; break;
		case 2:first_start = '\xB0'; first_end = '\xF7'; second_start = '\xA1'; second_end = '\xF1'; break;
		case 3:first_start = '\x81'; first_end = '\xA0'; second_start = '\x40'; second_end = '\xFE'; break;//(7F³ıÍâ)
		case 4:first_start = '\xAA'; first_end = '\xFE'; second_start = '\x40'; second_end = '\xA0'; break;//(7F³ıÍâ)
		case 5:first_start = '\xA8'; first_end = '\xA9'; second_start = '\x40'; second_end = '\xA0'; break;//(7F³ıÍâ)
		default:
			return -1;
		}

		char word[3];
		memset(word, 0, sizeof(word));
		cout << "start------->" << endl;
		for (uint8_t i = first_start; i <= first_end; i++)
		{
			for (uint8_t j = second_start; j < second_end; j++)
			{
				word[0] = i;;
				word[1] = j;
				//cout << hex << i << ',' << hex << j << ": " << word << '\t';
				cout << word;
			}
		}
		cout << endl << "end------->" << endl;
		return 0;
	}

	bool isHanzi(const uint8_t* p)
	{
		if      (p[0] >= (uint8_t)'\xB0' && p[0] <= (uint8_t)'\xF7' && p[1] >= (uint8_t)'\xA1' && p[1] <= (uint8_t)'\xFE')return true;//GBK2.0 ºº×Ö
		else if (p[0] >= (uint8_t)'\x81' && p[0] <= (uint8_t)'\xA0' && p[1] >= (uint8_t)'\x40' && p[1] <= (uint8_t)'\xFE')return true;//GBK3.0 À©³äºº×Ö
		else if (p[0] >= (uint8_t)'\xAA' && p[0] <= (uint8_t)'\xFE' && p[1] >= (uint8_t)'\x40' && p[1] <= (uint8_t)'\xA0')return true;//GBK4.0 À©³äºº×Ö
		else return false;
	}
}
