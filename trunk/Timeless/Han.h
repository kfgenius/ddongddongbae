//한글 입력 클래스
#pragma once

#define STR_MAX	80

class CHan
{
private:
	int mark[20];
	char han[STR_MAX];

	unsigned int FindMark(char eng);

public:
	char* EngToHan(char* text, char* han_area=NULL);

	CHan();
};
