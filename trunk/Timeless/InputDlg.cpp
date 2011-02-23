//입력을 위한 대화상자
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

	SetDlg(250,230,140);	//기본값
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

//한글입력
char* CInputDlg::Input()
{
	//뒷배경 잠시 저장(투명도 있는 대화창일 때 겹쳐 쓰면서 투명도가 없어지는 것을 방지
	jdd->DrawPicture("CommonBlank", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//윈도우 메세지 처리

		jdd->DrawPicture(backbuffer, "CommonBlank", 0, 0, NULL);
		if(Inputing())break;	//입력이 끝나면 중단

		jdd->Render();
	}

	return input_text;
}

//입력가능한 특수문자들
char num_shift_char[]={')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
char special_char[]={';', '=', ',', '-', '.', '/', '`', '[', '\\', ']','\''};	// `는 신명조에서 지원 안 됨
char special_shift_char[]={':', '+', '<', '_', '>', '?', '~', '{', '|', '}','"'};

int CInputDlg::Inputing()
{
	//출력
	DrawDlg();
	if(b_shadow)jdd->DrawText(backbuffer,input_text,*text_font,x+1,y+1,JColor(0,0,0));
	//깜빡이는 커서
	char show_text[101];
	strcpy(show_text, input_text);
	static int blink_count;
	++blink_count;
	if(blink_count<0 || blink_count>=40)blink_count=0;
	if(blink_count<20 && str_max>text_length+10)strcat(show_text,"_");
	//입력된 정보 표시
	if(b_shadow)jdd->DrawText(backbuffer,show_text,*text_font,x+1,y+1,JColor(0,0,0));
	jdd->DrawText(backbuffer,show_text,*text_font,x,y, text_color);

	int eng_len=(int)strlen(eng_buffer);
	//입력
	if(eng_len<80)
	{
		BOOL done=FALSE;
		//문자키
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
		//숫자키
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
		//기타 기호
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

		//스페이스
		if(!done && GetKey(vkey_space))strcat(eng_buffer, " ");

		if(done && strlen(eng_buffer)>0)is_eng_buffer[strlen(eng_buffer)-1]=is_eng;
	}
	//특수 키
	if(GetKey(vkey_enter))return 1;	//엔터
	else if(GetKey(vkey_back, 10))	//백스페이스
	{
		if(eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
	}
	else if(GetKey(vkey_alt))
	{
		is_eng=1-is_eng;
	}

	//입력 정보 갱신
	if(old_len!=eng_len)
	{
		BOOL able_text=FALSE;
		while(!able_text)
		{
			//문자 전체 길이 계산
			strcpy(input_text, m_han.EngToHan(eng_buffer, is_eng_buffer));
			text_length=0;
			for(size_t i=0; i<strlen(input_text) ; ++i)text_length+=GetCharLength(input_text[i]);

			//만약 대화창 이상의 길이라면 자르기
			if(str_max/10<text_length && eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
				else able_text=TRUE;
		}
		old_len=eng_len;

		//서페이스에 그리기
		if(b_shadow)jdd->DrawText(backbuffer,input_text,*text_font,x+1,y+1,JColor(0,0,0));
		//깜빡이는 커서
		char show_text[101];
		strcpy(show_text, input_text);
		static int blink_count;
		++blink_count;
		if(blink_count<0 || blink_count>=40)blink_count=0;
		if(blink_count<20 && str_max>text_length+10)strcat(show_text,"_");
		//입력된 정보 표시
		jdd->DrawText(backbuffer,show_text,*text_font,x,y, text_color);

	}

	return 0;
}

void CInputDlg::SetFlag(BOOL vflag)
{
	flag = vflag;
}