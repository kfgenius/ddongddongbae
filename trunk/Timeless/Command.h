//명령 처리 클래스
#pragma once

#include "SelectDlg.h"

class CCommand
{
	char** source;
	char commands[1024];

	int command_max;
	int count;
	int com_id[64];

	int GetComID(int no);

public:
	CSelectDlg comdlg;

	void Init(char* source[], int max=64);
	void AddComs(int n, ...);
	void AddCom(int id);
	void AddComStr(int id, char* str);
	int CommandSelect();
	int CommandSelecting();
	BOOL IsFull();
	BOOL Empty();

	int GetCount();

	//생성자
	CCommand(char* source[], int x, int y, int width, int line, BOOL row=FALSE, int max=64);
};