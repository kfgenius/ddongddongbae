#pragma once

//선택지 대화창

#include "dlg.h"

#define SELECT_CANCEL		-999
#define SELECT_NONE			-1

#define SELECT_MAX			64
#define SELECT_BUFFER_SIZE	TEXT_BUFFER_SIZE

//선택을 위한 대화상자
class CSelectDlg:public CDlg
{
private:
	int select;
	int gap;
	int sx, sy;
	char* dlg_buffer;
	char** select_point;
	BOOL is_row;

public:
	//생성자 소멸자
    CSelectDlg(int select_max = SELECT_MAX, unsigned int vbuffer_size = SELECT_BUFFER_SIZE);
	~CSelectDlg();

	//설정 오버라이드
	void SetDlg(int x, int y, int width, int line, BOOL is_row = FALSE, int start = 0);

	//선택 관련 메소드
	void MakeSelection(char* input_dlg);
	int Selecting();
	int Select(char* input_dlg, BOOL cancel = FALSE);
	void SetSelect(int select);
};