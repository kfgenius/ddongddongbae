//����� ���� ��ȭ����
#include "Global.h"
#include "TextDlg.h"

#define SNR_START 0

CTextDlg::CTextDlg(unsigned int vbuffer_size)
{
	buffer_size = vbuffer_size;
	dlg_buffer = new char[buffer_size];		//��ȭ ���� ����
	text_buffer = new char[buffer_size];	//������� ����� ������ ����

	lock = FALSE;
	text_auto = FALSE;
}

CTextDlg::~CTextDlg()
{
	//���� �����
	if(dlg_buffer != NULL)
	{
		delete[] dlg_buffer;
		dlg_buffer = NULL;
	}

	if(text_buffer != NULL)
	{
		delete[] text_buffer;
		text_buffer = NULL;
	}
}

void CTextDlg::SetDlg(int vx, int vy, int vwidth, int vline)
{
	x = vx;
	y = vy;
	width = Max(30, vwidth);
	width /= 10;
	line=Max(1, vline);

	setting = TRUE;
}

//��ȭ �����
void CTextDlg::MakeText(char* content)
{
	//�ʱ�ȭ�� �� �Ǿ� ������ ����
	if(!setting)
	{
		printf("Warning : ��ȭâ�� �ʱ�ȭ �Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	if(content == NULL)
	{
		printf("Warning : ������ �����ϴ�.");
		return;
	}

	//��ȭ���� ����
	length = (int)strlen(content);
	strncpy(dlg_buffer, content, buffer_size);
	if(strlen(content) >= buffer_size)
	{
		dlg_buffer[buffer_size - 1] = NULL;
	}
	scroll_end = FALSE;

	//��ȭ�� ����ϱ� �˸°� ó��
	sp = SNR_START;	//ó���ǰ� �ִ� ����Ʈ
	int space = -1;	//�������� ó���� ����Ʈ, space�� �ִ� ��
	int senlen = 0;	//������ ����
	
	//��ȭâ�� �°� ������ �� �ٲ�
	if(text_font == &global_font)
	while(sp<length)
	{
		BOOL length_check = TRUE;	//���� üũ�� ���� �� ���� �˻�
		int sp_add = 1;				//�̵��� ĭ ��, �ѱ��� ��츸 2ĭ.

		//�� �ٲ�
		if(dlg_buffer[sp] == '\\')
		{
			dlg_buffer[sp] = '\n';
			space = -1;
			senlen = 0;
			length_check = FALSE;
		}
		//�ѱ�
		else if(dlg_buffer[sp]&0x80)
		{
			senlen+=20;
			sp_add=2;
		}
		//��Ÿ
		else
		{
			if(dlg_buffer[sp]==' ')space=sp;
			senlen+=GetCharLength(dlg_buffer[sp]);
		}
		
		//���̰� ��ȭâ�� �Ѿ����� �˻�
		if(length_check && senlen > width*10)
		{
			if(space < 0)	//�����̽��� �ϳ��� ������ ���� �� ��ȯ
			{
				//���̿� CR�� �־���
				char* tmp_text;
				tmp_text = new char[length + 2];

				//���ο� ���ڿ�(���� �ͺ��� ���̰� 1 ū ��)�� ���� ����
				for(int i = 0; i < sp; ++i)tmp_text[i] = dlg_buffer[i];
				tmp_text[sp] = '\n';
				for(int i = sp + 1; i < length + 1; ++i)tmp_text[i] = dlg_buffer[i - 1];
				tmp_text[length + 1] = NULL;
				strcpy(dlg_buffer, tmp_text);
				delete [] tmp_text;

				//���� ���ڿ� ���̴� ���� ������ ����
				if(dlg_buffer[sp + 1] & 0x80)
				{
					senlen = 20;
				}
				else
				{
					senlen=GetCharLength(dlg_buffer[sp + 1]);
				}

				++length;
				++sp;
			}
			else
			{
				dlg_buffer[space] = '\n';
				senlen = 0;
				for(int i = space + 1; i <= sp; ++i)
				{
					if(dlg_buffer[i]&0x80)
					{
						senlen += 20;
						++i;
					}
					//��Ÿ
					else senlen += GetCharLength(dlg_buffer[i]);
				}
				space = -1;
			}
		}

		//�ѱ��� �� 2ĭ �ٰ� �������� 1ĭ
		sp += sp_add;
	}

	sp = SNR_START, tp = 0;;	//�����ġ �ʱ�ȭ
	n_of_e = 0;					//�ٹٲ� Ƚ��

	setting2 = TRUE;
}

//��ȭ ���(������ ���¿���)
void CTextDlg::Print(char* content)
{
	//�ʱ�ȭ�� �� �Ǿ� ������ ����
	if(!setting)
	{
		printf("Warning : ��ȭâ�� �ʱ�ȭ �Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	MakeText(content);

	//�޹�� ��� ����(���� �ִ� ��ȭâ�� �� ���� ���鼭 ������ �������� ���� ����
	jdd->DrawPicture("_CommonBlank", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//������ �޼��� ó��

		jdd->DrawPicture(backbuffer, "_CommonBlank", 0, 0, NULL);
		if(Printing() == TEXT_DLG_END)break;	//������ �� ����ϸ� �ߴ�
		jdd->Render();

		//���� �ѱ��
		if(keyboard_control && GetKey(vkey_enter, 0) || mouse_control && LeftDown());
			else Sleep(10);
	}
}

//��ȭ���(�ǽð�����)
int CTextDlg::Printing()
{
	//�ʱ�ȭ�� �� �Ǿ� ������ ����
	if(!setting || !setting2)
	{
		printf("Warning : ��ȭâ�� �ʱ�ȭ �Ǿ� ���� �ʽ��ϴ�.");
		return TEXT_DLG_ERROR;
	}

	if(!scroll_end)
	{
		if(sp < length && n_of_e < line)
		{
			if(dlg_buffer[sp] == '\n')++n_of_e;
			text_buffer[tp] = dlg_buffer[sp];
			
			//�ѱ��� ��� 2ĭ ����
			if(dlg_buffer[sp] & 0x80)
			{
				++sp;
				++tp;
				text_buffer[tp] = dlg_buffer[sp];
			}
			
			text_buffer[tp+1] = NULL;
			
			//������ ����
			++sp;
			++tp;
		}
		else
		{
			scroll_end = TRUE;
		}
	}
	else
	{
		//��������
		if((keyboard_control && GetKey(vkey_enter)) || (mouse_control && LeftDown()) || text_auto)
		{
			text_buffer[0] = NULL;	//�ؽ�Ʈ ���� �����

			if(sp < length)
			{
				scroll_end = FALSE;
				n_of_e = 0;
				tp = 0;
			}
			else
			{
				if(!lock)
				{
					return TEXT_DLG_END;
				}
			}
		}
	}

	ShowText();

	return 0;
}

//��ȭ ���
void CTextDlg::ShowText()
{
	//�ʱ�ȭ�� �� �Ǿ� ������ ����
	if(!setting || !setting2)
	{
		printf("Warning : ��ȭâ�� �ʱ�ȭ �Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	//��ȭâ ���
	DrawDlg();

	RECT text_rect, shadow_rect;
	
	//�׸���
	if(b_shadow)
	{
		SetRect(&shadow_rect, x+1, y+1, SCREEN_X, SCREEN_Y);
		jdd->DrawText(backbuffer, text_buffer, *text_font, &shadow_rect, JColor(0,0,0));
	}
	
	//����
	SetRect(&text_rect, x, y, SCREEN_X, SCREEN_Y);
	jdd->DrawText(backbuffer, text_buffer, *text_font, &text_rect, text_color);
}

//��ȭâ ��/���
void CTextDlg::Lock()
{
	lock=TRUE;
}

void CTextDlg::UnLock()
{
	lock=FALSE;
}

void CTextDlg::SetTextAuto(BOOL on_off)
{
	text_auto = on_off;
}

BOOL CTextDlg::GetTextAuto()
{
	return text_auto;
}