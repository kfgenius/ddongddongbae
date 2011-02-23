//��ȭâ �⺻ Ŭ����
#pragma once

#include "JDirectDraw.h"

class CDlg
{
protected:
	//�ʱ�ȭ �Ǿ������� Ȯ��
	BOOL setting, setting2;

	//��ȭâ ����
	unsigned int buffer_size;
	int n_of_e;						//�� �ٲ� Ƚ��
	int x, y, width, line;			//X,Y ��ġ, ���� ũ��, ���� �� ��
	BOOL b_shadow;					//�׸��ڸ� �ִ��� ����
	char* dlgbox;					//��ȭâ �׸�
	BOOL show_dlgbox;				//��ȭâ�� ǥ���ϴ��� ����
	JColor text_color;				//���ڻ�
	int frame_x, frame_y;			//��ȭâ Ʋ�� ����,���� ũ��(�⺻ 5, ������ ��ȭâ�� ���� ����)
	float opacity;					//â ����

	//��� ����� ���� ����
	int length;

	//��� ��Ʈ
	JFont* text_font;

	void DrawDlg();					//��ȭâ ���̱�
	BOOL MouseInBox();				//���콺�� ��ȭ���ڿ� �ִ°��� �˻�

public:
	//������
	CDlg();
	~CDlg();

	void SetDlg();											//��ȭâ ����
	void Move(int mov_x, int mov_y);						//��ȭâ �̵�
	void ShowDlg();											//�ۻ��� ���̱�
	void HideDlg();											//�ۻ��� �����
	void ShowShadow();										//�׸��� ���
	void HideShadow();										//�׸��� �����
	
	int GetX(){return x;}									//X��ǥ ��������
	int GetY(){return y;}									//Y��ǥ ��������
	int GetWidth(){return width*10;}						//���� ũ�� ��������
	int GetLine(){return line;}								//���� ũ��(�� ��) ��������
	float GetOpacity(){return opacity;}						//������ ��������

	void SetX(int vx){x = vx;}								//X��ǥ ����
	void SetY(int vy){y = vy;}								//Y��ǥ ����
	void SetFont(JFont* font);								//��Ʈ ����
	void SetTextColor(BYTE r, BYTE g, BYTE b);				//���ڻ� ����
	void SetOpacity(float opacity);							//��ȭâ ������ ����
	void SetFrameSize(int vx, int vy);						//��ȭâ Ʋ ũ�� ����

	void SetDlgBox(char* dlgbox);							//�ۻ��� ����
	void MakeDlgBox(char* dlgbox);							//�ۻ��� �����
};