//�Է��� ���� ��ȭ����
#include "InputDlg.h"
#include "Global.h"

static int is_eng = 0;

CInputDlg::CInputDlg(int vx, int vy, int max)
{
	flag= FLAG_TEXT	| FLAG_NUMBER | FLAG_MARK;

	SetDlg(vx, vy, max);
}

CInputDlg::CInputDlg()
{
	flag= FLAG_TEXT	| FLAG_NUMBER | FLAG_MARK;

	SetDlg(250,230,140);	//�⺻��
}

void CInputDlg::SetDlg(int vx, int vy, int max)
{
	Clear();
	str_max=max;

	x=vx;
	y=vy;
	width=Max(30,max);
	width/=10;

	str_max*=10;

	setting = TRUE;
}

void CInputDlg::Clear()
{
	strcpy(input_text,"");
	strcpy(eng_buffer,"");
	old_len=text_length=0;
}

//�ѱ��Է�
char* CInputDlg::Input()
{
	//�޹�� ��� ����(���� �ִ� ��ȭâ�� �� ���� ���鼭 ������ �������� ���� ����
	jdd->DrawPicture("CommonBlank", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//������ �޼��� ó��

		jdd->DrawPicture(backbuffer, "CommonBlank", 0, 0, NULL);
		if(Inputing())break;	//�Է��� ������ �ߴ�

		jdd->Render();
	}

	return input_text;
}

//�Է°����� Ư�����ڵ�
char num_shift_char[]={')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
char special_char[]={';', '=', ',', '-', '.', '/', '`', '[', '\\', ']','\''};	// `�� �Ÿ������� ���� �� ��
char special_shift_char[]={':', '+', '<', '_', '>', '?', '~', '{', '|', '}','"'};

int CInputDlg::Inputing()
{
	//���
	DrawDlg();
	if(b_shadow)jdd->DrawText(backbuffer,input_text,*text_font,x+1,y+1,JColor(0,0,0));
	//�����̴� Ŀ��
	char show_text[101];
	strcpy(show_text, input_text);
	static int blink_count;
	++blink_count;
	if(blink_count<0 || blink_count>=40)blink_count=0;
	if(blink_count<20 && str_max>text_length+10)strcat(show_text,"_");
	//�Էµ� ���� ǥ��
	if(b_shadow)jdd->DrawText(backbuffer,show_text,*text_font,x+1,y+1,JColor(0,0,0));
	jdd->DrawText(backbuffer,show_text,*text_font,x,y, text_color);

	int eng_len=(int)strlen(eng_buffer);
	//�Է�
	if(eng_len<80)
	{
		BOOL done=FALSE;
		//����Ű
		if(flag & FLAG_TEXT)
		{
			for(int i=vkey_a; i<=vkey_z; ++i)
				if(GetKey((ValidKey)i))
				{
					size_t buffer_length=strlen(eng_buffer);
					if(_GetKeyState(VK_SHIFT))eng_buffer[buffer_length]='A'+(char)(i-vkey_a);
						else eng_buffer[buffer_length]='a'+(char)(i-vkey_a);
					eng_buffer[buffer_length+1]=NULL;

					done=TRUE;
					break;
				}
		}
		//����Ű
		if(flag & FLAG_NUMBER)
		{
			if(!done)for(int i=vkey_0; i<=vkey_9; ++i)
				if(GetKey((ValidKey)i))
				{
					size_t buffer_length=strlen(eng_buffer);
					if(_GetKeyState(VK_SHIFT))eng_buffer[buffer_length]=num_shift_char[i-vkey_0];
						else eng_buffer[buffer_length]='0'+(char)(i-vkey_0);
					eng_buffer[buffer_length+1]=NULL;

					done=TRUE;
					break;
				}
		}
		//��Ÿ ��ȣ
		if(flag & FLAG_MARK)
		{
			if(!done)for(int i=vkey_colon; i<=vkey_quotation; ++i)
				if(GetKey((ValidKey)i))
				{
					size_t buffer_length=strlen(eng_buffer);
					if(_GetKeyState(VK_SHIFT))eng_buffer[buffer_length]=special_shift_char[i-vkey_colon];
						else eng_buffer[buffer_length]=special_char[i-vkey_colon];
					eng_buffer[buffer_length+1]=NULL;

					done=TRUE;
					break;
				}
		}

		//�����̽�
		if(!done && GetKey(vkey_space))strcat(eng_buffer, " ");

		if(done && strlen(eng_buffer)>0)is_eng_buffer[strlen(eng_buffer)-1]=is_eng;
	}
	//Ư�� Ű
	if(GetKey(vkey_enter))return 1;	//����
	else if(GetKey(vkey_back, 10))	//�齺���̽�
	{
		if(eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
	}
	else if(GetKey(vkey_alt))
	{
		is_eng=1-is_eng;
	}

	//�Է� ���� ����
	if(old_len!=eng_len)
	{
		BOOL able_text=FALSE;
		while(!able_text)
		{
			//���� ��ü ���� ���
			strcpy(input_text, m_han.EngToHan(eng_buffer, is_eng_buffer));
			text_length=0;
			for(size_t i=0; i<strlen(input_text) ; ++i)text_length+=GetCharLength(input_text[i]);

			//���� ��ȭâ �̻��� ���̶�� �ڸ���
			if(str_max/10<text_length && eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
				else able_text=TRUE;
		}
		old_len=eng_len;

		//�����̽��� �׸���
		if(b_shadow)jdd->DrawText(backbuffer,input_text,*text_font,x+1,y+1,JColor(0,0,0));
		//�����̴� Ŀ��
		char show_text[101];
		strcpy(show_text, input_text);
		static int blink_count;
		++blink_count;
		if(blink_count<0 || blink_count>=40)blink_count=0;
		if(blink_count<20 && str_max>text_length+10)strcat(show_text,"_");
		//�Էµ� ���� ǥ��
		jdd->DrawText(backbuffer,show_text,*text_font,x,y, text_color);

	}

	return 0;
}

void CInputDlg::SetFlag(BOOL vflag)
{
	flag = vflag;
}