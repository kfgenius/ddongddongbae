//������ ���� ��ȭ����

#include "Global.h"
#include "SelectDlg.h"

CSelectDlg::CSelectDlg(int select_max, unsigned int vbuffer_size)
{
	buffer_size = vbuffer_size;
	dlg_buffer = new char[buffer_size];
	select_point = new char*[select_max];

	setting2 = FALSE;
}

CSelectDlg::~CSelectDlg()
{
	//���� �����
	if(dlg_buffer != NULL)
	{
		delete [] dlg_buffer;
		dlg_buffer = NULL;
	}

	if(select_point != NULL)
	{
		delete [] select_point;
		select_point = NULL;
	}
}

void CSelectDlg::SetDlg(int x, int y, int width, int line, BOOL row, int start)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->line = line;
	select = start;

	//���� ũ�⸦ 10���� �������� ����
	width = Max(width, 30);
	this->width = width / 10;

	//���μ��������� ���� ���������� Ȯ��, ���� ����
	if(line <= 0)line = 1;
	this->row = row;

	setting = TRUE;
}

void CSelectDlg::MakeSelection(char* input_dlg)
{
	if(!setting || input_dlg==NULL)return;

	//���� ũ�⸦ �Ѿ�� ���� ����
	strncpy(dlg_buffer, input_dlg, buffer_size);
	if(strlen(input_dlg) >= buffer_size)dlg_buffer[buffer_size-1] = NULL;

	int length = (int)strlen(dlg_buffer);

	//��ȭ�� ����ϱ� �˸°� ó��
	n_of_e = 0;
	int bp = 1;
	select_point[0] = dlg_buffer;
	for(int i = 0; i < length; ++i)
	{
		if(dlg_buffer[i] == '\\')
		{
			dlg_buffer[i] = NULL;
			select_point[bp] = dlg_buffer + i + 1;
			++n_of_e;
			++bp;
			continue;
		}

		if(dlg_buffer[i] == 0x0d)
		{
			continue;
		}
	}

	//��ũ��Ʈ���� �о� �� ������ ���� ��ǥ(0x0d)�� ����� �� ����
	if(dlg_buffer[length - 1] == 13)
	{
		dlg_buffer[length - 1] = NULL;
	}//@_@

	//��ȭâ ǥ�ÿ� �ʿ��� �͵� ���
	if(row)
	{
		gap = (width*10)/line;

		sx=x+select%line*gap;
		sy=y+(select/line)*20;
	}
	else
	{
		gap = (width*10)/(n_of_e/line+1);

		sx=x+select/line*gap;
		sy=y+(select%line)*20;
	}

	setting2 = TRUE;
}

int CSelectDlg::Select(char* input_dlg, BOOL cancel)
{
	//�ʱ�ȭ�� �� �Ǿ� ������ ����
	if(!setting || !setting2)
	{
		printf("Warning : ��ȭâ�� �ʱ�ȭ �Ǿ� ���� �ʽ��ϴ�.");
		return -999;
	}

	MakeSelection(input_dlg);

	//�޹�� ��� ����(���� �ִ� ��ȭâ�� �� ���� ���鼭 ������ �������� ���� ����
	jdd->DrawPicture("_CommonBlank", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//������ �޼��� ó��

		jdd->DrawPicture(backbuffer, "_CommonBlank", 0, 0, NULL);
		int selected=Selecting();
		if(selected>=0 || (cancel && selected==-999))break;	//�����ϰų� ����ϸ� �ߴ�

		jdd->Render();
	}

	return select;
}

int CSelectDlg::Selecting()
{
	//�ʱ�ȭ�� �� �Ǿ� ������ ����
	if(!setting2)
	{
		printf("Warning : ��ȭâ�� �ʱ�ȭ �Ǿ� ���� �ʽ��ϴ�.");
		return -999;
	}

	//��ȭ����
	if(keyboard_control)
	{
		if(row)
		{
			//�� �Ʒ� �̵�
			if(GetKey(vkey_right,10))select=Min(n_of_e,select+1);
				else if(GetKey(vkey_left,10))select=Max(0,select-1);
			//�¿� �̵�
			if(GetKey(vkey_down,10))
			{
				if(select+line<=n_of_e)select+=line;
			}
			else if(GetKey(vkey_up,10))
			{
				if(select-line>=0)select-=line;
			}

			sx=x+select%line*gap;
			sy=y+(select/line)*20;
		}
		else
		{
			//�� �Ʒ� �̵�
			if(GetKey(vkey_down,10))select=Min(n_of_e,select+1);
				else if(GetKey(vkey_up,10))select=Max(0,select-1);

			//�¿� �̵�
			if(GetKey(vkey_right,10))
			{
				if(select+line<=n_of_e)select+=line;
			}
			else if(GetKey(vkey_left,10))
			{
				if(select-line>=0)select-=line;
			}

			sx=x+select/line*gap;
			sy=y+(select%line)*20;
		}
	}

	//���콺�� ����
	BOOL mouse_select=FALSE;
	if(mouse_control && MouseInBox())
	{
		for(int i=0; i<=n_of_e; ++i)
		{
			//������ ���� ���
			int tsx, tsy;
			if(row)
			{
				tsx=x+(i%line)*gap;
				tsy=y+(i/line)*20;
			}
			else
			{
				tsx=x+(i/line)*gap;
				tsy=y+(i%line)*20;
			}

			if(MouseX>=tsx && MouseX<tsx+gap && MouseY>=tsy && MouseY<tsy+20)
			{
				//�̵��� ���� ���� �˻�
				if(mouse_move)
				{
					select=i;
					sx=tsx;
					sy=tsy;
					mouse_move=FALSE;
				}

				mouse_select=TRUE;
				break;
			}
		}
	}

	DrawDlg();
	//���� Ŀ��
	RECT src_rect;
	SetRect(&src_rect,0,0,gap/2,20);
	jdd->DrawPicture(backbuffer,"CommonSelect",sx,sy,&src_rect);
	SetRect(&src_rect,320-gap/2,0,320,20);
	jdd->DrawPicture(backbuffer,"CommonSelect",sx+gap/2,sy,&src_rect);

	//��������
	for(int i=0; i<=n_of_e; ++i)
	{
		int tx,ty;
		if(row)
		{
			tx=(i%line)*gap+x;
			ty=(i/line)*20+y;
		}
		else
		{
			tx=(i/line)*gap+x;
			ty=(i%line)*20+y;
		}

		if(b_shadow)jdd->DrawText(backbuffer,select_point[i],*text_font,tx+1,ty+1,JColor(0,0,0));
		jdd->DrawText(backbuffer, select_point[i], *text_font, tx, ty, text_color);
	}

	//���콺 ����
	if(LeftDown() && mouse_select)return select;
	if(RightDown() && mouse_control)return -999;
	//Ű���� ����
	if(keyboard_control && GetKey(vkey_enter))return select;
	if(keyboard_control && GetKey(vkey_esc))return -999;
	
	return -1;
}

void CSelectDlg::SetSelect(int select)
{
	this->select = select;
}