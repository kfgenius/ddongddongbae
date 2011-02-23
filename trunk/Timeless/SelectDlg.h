#pragma once

//선택지 대화창

#include "dlg.h"

//선택을 위한 대화상자
class CSelectDlg:public CDlg
{
private:
	int select;
	int gap;
	int sx, sy;
	char* dlg_buffer;
	char** select_point;
	BOOL row;

public:
	//생성자 소멸자
    CSelectDlg(int select_max=64, unsigned int vbuffer_size=1024);
	~CSelectDlg();

	//설정 오버라이드
	void SetDlg(int x, int y, int width, int line, BOOL row=FALSE, int start=0);

	//선택 관련 메소드
	void MakeSelection(char* input_dlg);
	int Selecting();
	int Select(char* input_dlg, BOOL cancel=FALSE);
	void SetSelect(int select);
};