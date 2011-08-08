//�Է��� ���� ��ȭ����(�ܹ� ������ �Է��ϱ� ���� ���� ���۸� ������ ����)
#pragma once

#include "dlg.h"
#include "Han.h"

#define FLAG_TEXT	0x01
#define FLAG_NUMBER	0x02
#define FLAG_MARK	0x04

class CInputDlg:public CDlg
{
private:
	CHan m_han;

	int old_len;
	int text_length;
	int str_max;
	char is_eng_buffer[81];
	BOOL flag;

public:
	//������
	CInputDlg(int vx, int vy, int max);
	CInputDlg();

	void SetDlg(int vx, int vy, int max);
	void SetFlag(BOOL vflag);

	char eng_buffer[81];
	char input_text[161];

	void Clear();
	char* Input();
	int Inputing();
};
