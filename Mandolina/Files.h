//���� ó�� Ŭ����
#pragma once

#include <vector>

using namespace std;

class CFiles
{
public:
	vector<char*> filename;
	void AddFile(char* name);
	void SearchFile(char* dir, char* filename);

	//������ �Ҹ���
	CFiles();
	~CFiles();
};