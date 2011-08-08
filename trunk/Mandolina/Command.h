//명령 처리 클래스
#pragma once

#include "SelectDlg.h"

#define COMMAND_BUFFER_SIZE		SELECT_BUFFER_SIZE

class CCommand
{
private:
	char** source;
	char commands[COMMAND_BUFFER_SIZE];

	int command_max;
	int count;
	int* com_id_array;

	int GetComID(int no);

public:
	CSelectDlg com_dlg;

	void Init(char* source[], int max=64);
	void AddComs(int n, ...);
	void AddCom(int id);
	void AddComStr(int id, char* str);
	int CommandSelect();
	int CommandSelecting();
	BOOL IsFull();
	BOOL IsEmpty();

	int GetCount();

	//생성자
	CCommand(char* source[], int x, int y, int width, int line, BOOL row=FALSE, int max=64);
	~CCommand();
};