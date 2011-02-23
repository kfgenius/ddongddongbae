//파일 처리 클래스
#pragma once

#include <vector>

using namespace std;

class CFiles
{
public:
	vector<char*> filename;
	void AddFile(char* name);
	void SearchFile(char* dir, char* filename);

	//생성자 소멸자
	CFiles();
	~CFiles();
};