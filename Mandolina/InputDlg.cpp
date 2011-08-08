//�Է��� ���� ��ȭ����
#include "InputDlg.h"
#include "Global.h"

static int is_eng = 0;

#define CURSOR_BLINK_DELAY	40

CInputDlg::CInputDlg(int vx, int vy, int max)
{
	flag = FLAG_TEXT	| FLAG_NUMBER | FLAG_MARK;

	SetDlg(vx, vy, max);
}

CInputDlg::CInputDlg()
{
	flag = FLAG_TEXT | FLAG_NUMBER | FLAG_MARK;

	SetDlg(250, 230, 140);	//�⺻��
}

void CInputDlg::SetDlg(int vx, int vy, int max)
{
	Clear();
	str_max = max;

	x = vx;
	y = vy;
	width = Max(DLG_MINIMUM_WIDTH, max);
	width /= DLG_WIDTH_ONE;
	line = DLG_MINIMUM_LINE;

	str_max *= DLG_WIDTH_ONE;

	is_set_dlg = TRUE;
	is_set_content = TRUE;
}

void CInputDlg::Clear()
{
	strcpy(input_text, "");
	strcpy(eng_buffer, "");
	old_len = text_length = 0;
}

//�ѱ��Է�
char* CInputDlg::Input()
{
	//�޹�� ��� ����(���� �ִ� ��ȭâ�� �� ���� ���鼭 ������ �������� ���� ����
	jdd->DrawPicture("_CommonBlank", backbuffer, 0, 0, NULL);

	while(TRUE)
	{
		if(!ProcessMessage())break;		//������ �޼��� ó��

		jdd->DrawPicture(backbuffer, "_CommonBlank", 0, 0, NULL);
		if(Inputing())	//�Է��� ������ �ߴ�
		{
			break;
		}

		jdd->Render();
	}

	return input_text;
}

//�Է°����� Ư�����ڵ�
char num_shift_char[] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
char special_char[] = {';', '=', ',', '-', '.', '/', '`', '[', '\\', ']','\''};	// `�� �Ÿ������� ���� �� ��
char special_shift_char[] = {':', '+', '<', '_', '>', '?', '~', '{', '|', '}','"'};

int CInputDlg::Inputing()
{
	//���
	DrawDlg();
	if(is_shadow)
	{
		jdd->DrawText(backbuffer, input_text, *text_font, x + 1, y + 1, BLACK);
	}

	//�����̴� Ŀ��
	char show_text[101];
	strcpy(show_text, input_text);
	static int blink_count = 0;
	blink_count++;
	if(blink_count >= CURSOR_BLINK_DELAY)
	{
		blink_count = 0;
	}
	if(blink_count < CURSOR_BLINK_DELAY / 2 && str_max > text_length + DLG_WIDTH_ONE)
	{
		strcat(show_text, "_");
	}

	//�Էµ� ���� ǥ��
	if(is_shadow)
	{
		jdd->DrawText(backbuffer, show_text, *text_font, x + 1, y + 1, BLACK);
	}
	jdd->DrawText(backbuffer, show_text, *text_font, x, y, text_color);

	int eng_len = (int)strlen(eng_buffer);
	
	//�Է�
	if(eng_len < 80)
	{
		BOOL done = FALSE;
		//����Ű
		if(flag & FLAG_TEXT)
		{
			for(int i = vkey_a; i <= vkey_z; i++)
			{
				if(GetKey((ValidKey)i))
				{
					size_t buffer_length = strlen(eng_buffer);
					if(_GetKeyState(VK_SHIFT))
					{
						eng_buffer[buffer_length] = 'A' + (char)(i-vkey_a);
					}
					else
					{
						eng_buffer[buffer_length] = 'a' + (char)(i-vkey_a);
					}
					eng_buffer[buffer_length+1] = NULL;

					done = TRUE;
					break;
				}
			}
		}
		//����Ű
		if(flag & FLAG_NUMBER)
		{
			if(done == FALSE)
			{
				for(int i = vkey_0; i <= vkey_9; i++)
				{
					if(GetKey((ValidKey)i))
					{
						size_t buffer_length = strlen(eng_buffer);
						if(_GetKeyState(VK_SHIFT))
						{
							eng_buffer[buffer_length] = num_shift_char[i - vkey_0];
						}
						else
						{
							eng_buffer[buffer_length] = '0' + (char)(i - vkey_0);
						}
						eng_buffer[buffer_length+1] = NULL;

						done = TRUE;
						break;
					}
				}
			}
		}
		//��Ÿ ��ȣ
		if(flag & FLAG_MARK)
		{
			if(done == FALSE)
			{
				for(int i=vkey_colon; i<=vkey_quotation; i++)
				{
					if(GetKey((ValidKey)i))
					{
						size_t buffer_length = strlen(eng_buffer);
						if(_GetKeyState(VK_SHIFT))
						{
							eng_buffer[buffer_length] = special_shift_char[i - vkey_colon];
						}
						else
						{
							eng_buffer[buffer_length] = special_char[i - vkey_colon];
						}
						eng_buffer[buffer_length+1] = NULL;

						done = TRUE;
						break;
					}
				}
			}
		}

		//�����̽�
		if(done == FALSE && GetKey(vkey_space))
		{
			strcat(eng_buffer, " ");
		}

		if(done && strlen(eng_buffer) > 0)
		{
			is_eng_buffer[strlen(eng_buffer) - 1] = is_eng;
		}
	}

	//Ư�� Ű
	if(GetKey(vkey_enter))	//����
	{
		return 1;
	}
	else if(GetKey(vkey_back, 10))	//�齺���̽�
	{
		if(eng_len > 0)
		{
			eng_buffer[strlen(eng_buffer) - 1] = NULL;
		}
	}
	else if(GetKey(vkey_alt))
	{
		is_eng = 1 - is_eng;
	}

	//�Է� ���� ����
	if(old_len != eng_len)
	{
		BOOL able_text = FALSE;
		while(able_text == FALSE)
		{
			//���� ��ü ���� ���
			strcpy(input_text, m_han.EngToHan(eng_buffer, is_eng_buffer));
			text_length = 0;
			for(size_t i = 0; i < strlen(input_text) ; i++)
			{
				text_length += GetCharLength(input_text[i]);
			}

			//���� ��ȭâ �̻��� ���̶�� �ڸ���
			if(str_max / DLG_WIDTH_ONE < text_length && eng_len > 0)
			{
				eng_buffer[strlen(eng_buffer) - 1] = NULL;
			}
			else
			{
				able_text = TRUE;
			}
		}
		old_len = eng_len;

		//�����̽��� �׸���
		if(is_shadow)
		{
			jdd->DrawText(backbuffer, input_text, *text_font, x + 1, y + 1, BLACK);
		}
		
		//�����̴� Ŀ��
		char show_text[101];
		strcpy(show_text, input_text);
		static int blink_count = 0;
		blink_count++;
		if(blink_count >= CURSOR_BLINK_DELAY)
		{
			blink_count = 0;
		}
		if(blink_count < CURSOR_BLINK_DELAY / 2 && str_max>text_length + DLG_WIDTH_ONE)
		{
			strcat(show_text, "_");
		}
		
		//�Էµ� ���� ǥ��
		jdd->DrawText(backbuffer, show_text, *text_font, x, y, text_color);
	}

	return 0;
}

void CInputDlg::SetFlag(BOOL vflag)
{
	flag = vflag;
}