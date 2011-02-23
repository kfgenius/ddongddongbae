#pragma once

#include "dlg.h"

#define TEXT_DLG_END	999
#define TEXT_DLG_ERROR	-1

//����� ���� ��ȭ����
class CTextDlg:public CDlg
{
private:
	char* dlg_buffer;
	char* text_buffer;
	BOOL lock;
	int tp, sp;			//�ѱ��ھ� ����ϱ� ���� ������, tp�� ���� ��µǴ� ��ȭâ�� ������, sp�� ��ü ������ ������
	BOOL scroll_end;	//���ڰ� ��µǴ� �ִϸ��̼��� �������� ����

public:
	//������ �Ҹ���
	CTextDlg(unsigned int vbuffer_size=1024);
	~CTextDlg();

	void SetDlg(int vx, int vy, int vwidth, int vline);
	void MakeText(char* content);
	void ShowText();
	void Print(char* content);
	int Printing();

	void Lock();		//â�� �� �ݰ� ��
	void UnLock();		//â�� ���� �� �ְ� ��
};