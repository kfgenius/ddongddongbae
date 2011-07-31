#pragma once

//������ ��ȭâ

#include "dlg.h"

#define SELECT_CANCEL		-999
#define SELECT_NONE			-1

#define SELECT_MAX			64
#define SELECT_BUFFER_SIZE	TEXT_BUFFER_SIZE

//������ ���� ��ȭ����
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
	//������ �Ҹ���
    CSelectDlg(int select_max = SELECT_MAX, unsigned int vbuffer_size = SELECT_BUFFER_SIZE);
	~CSelectDlg();

	//���� �������̵�
	void SetDlg(int x, int y, int width, int line, BOOL is_row = FALSE, int start = 0);

	//���� ���� �޼ҵ�
	void MakeSelection(char* input_dlg);
	int Selecting();
	int Select(char* input_dlg, BOOL cancel = FALSE);
	void SetSelect(int select);
};