#pragma once

//������ ��ȭâ

#include "dlg.h"

//������ ���� ��ȭ����
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
	//������ �Ҹ���
    CSelectDlg(int select_max=64, unsigned int vbuffer_size=1024);
	~CSelectDlg();

	//���� �������̵�
	void SetDlg(int x, int y, int width, int line, BOOL row=FALSE, int start=0);

	//���� ���� �޼ҵ�
	void MakeSelection(char* input_dlg);
	int Selecting();
	int Select(char* input_dlg, BOOL cancel=FALSE);
	void SetSelect(int select);
};