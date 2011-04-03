//출력을 위한 대화상자
#include "Global.h"
#include "TextDlg.h"

#define SNR_START 0

CTextDlg::CTextDlg(unsigned int vbuffer_size)
{
	buffer_size = vbuffer_size;
	dlg_buffer = new char[buffer_size];		//대화 내용 버퍼
	text_buffer = new char[buffer_size];	//현재까지 출력한 내용의 버퍼

	lock = FALSE;
	text_auto = FALSE;
}

CTextDlg::~CTextDlg()
{
	//버퍼 지우기
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

//대화 만들기
void CTextDlg::MakeText(char* content)
{
	//초기화가 안 되어 있으면 리턴
	if(!setting)
	{
		printf("Warning : 대화창이 초기화 되어 있지 않습니다.");
		return;
	}

	if(content == NULL)
	{
		printf("Warning : 내용이 없습니다.");
		return;
	}

	//대화내용 복사
	length = (int)strlen(content);
	strncpy(dlg_buffer, content, buffer_size);
	if(strlen(content) >= buffer_size)
	{
		dlg_buffer[buffer_size - 1] = NULL;
	}
	scroll_end = FALSE;

	//대화를 출력하기 알맞게 처리
	sp = SNR_START;	//처리되고 있는 포인트
	int space = -1;	//이전까지 처리한 포인트, space가 있던 곳
	int senlen = 0;	//문장의 길이
	
	//대화창에 맞게 문장을 줄 바꿈
	if(text_font == &global_font)
	while(sp<length)
	{
		BOOL length_check = TRUE;	//길이 체크를 할지 안 할지 검사
		int sp_add = 1;				//이동할 칸 수, 한글일 경우만 2칸.

		//줄 바꿈
		if(dlg_buffer[sp] == '\\')
		{
			dlg_buffer[sp] = '\n';
			space = -1;
			senlen = 0;
			length_check = FALSE;
		}
		//한글
		else if(dlg_buffer[sp]&0x80)
		{
			senlen+=20;
			sp_add=2;
		}
		//기타
		else
		{
			if(dlg_buffer[sp]==' ')space=sp;
			senlen+=GetCharLength(dlg_buffer[sp]);
		}
		
		//길이가 대화창을 넘었는지 검사
		if(length_check && senlen > width*10)
		{
			if(space < 0)	//스페이스가 하나도 없을때 강제 줄 변환
			{
				//사이에 CR을 넣어줌
				char* tmp_text;
				tmp_text = new char[length + 2];

				//새로운 문자열(지금 것보다 길이가 1 큰 것)에 내용 복사
				for(int i = 0; i < sp; ++i)tmp_text[i] = dlg_buffer[i];
				tmp_text[sp] = '\n';
				for(int i = sp + 1; i < length + 1; ++i)tmp_text[i] = dlg_buffer[i - 1];
				tmp_text[length + 1] = NULL;
				strcpy(dlg_buffer, tmp_text);
				delete [] tmp_text;

				//지금 문자열 길이는 지금 문자의 길이
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
					//기타
					else senlen += GetCharLength(dlg_buffer[i]);
				}
				space = -1;
			}
		}

		//한글일 때 2칸 뛰고 나머지는 1칸
		sp += sp_add;
	}

	sp = SNR_START, tp = 0;;	//출력위치 초기화
	n_of_e = 0;					//줄바꿈 횟수

	setting2 = TRUE;
}

//대화 출력(정지된 상태에서)
void CTextDlg::Print(char* content)
{
	//초기화가 안 되어 있으면 리턴
	if(!setting)
	{
		printf("Warning : 대화창이 초기화 되어 있지 않습니다.");
		return;
	}

	MakeText(content);

	//뒷배경 잠시 저장(투명도 있는 대화창일 때 겹쳐 쓰면서 투명도가 없어지는 것을 방지
	jdd->DrawPicture("_CommonBlank", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//윈도우 메세지 처리

		jdd->DrawPicture(backbuffer, "_CommonBlank", 0, 0, NULL);
		if(Printing() == TEXT_DLG_END)break;	//내용을 다 출력하면 중단
		jdd->Render();

		//빨리 넘기기
		if(keyboard_control && GetKey(vkey_enter, 0) || mouse_control && LeftDown());
			else Sleep(10);
	}
}

//대화출력(실시간으로)
int CTextDlg::Printing()
{
	//초기화가 안 되어 있으면 리턴
	if(!setting || !setting2)
	{
		printf("Warning : 대화창이 초기화 되어 있지 않습니다.");
		return TEXT_DLG_ERROR;
	}

	if(!scroll_end)
	{
		if(sp < length && n_of_e < line)
		{
			if(dlg_buffer[sp] == '\n')++n_of_e;
			text_buffer[tp] = dlg_buffer[sp];
			
			//한글일 경우 2칸 전진
			if(dlg_buffer[sp] & 0x80)
			{
				++sp;
				++tp;
				text_buffer[tp] = dlg_buffer[sp];
			}
			
			text_buffer[tp+1] = NULL;
			
			//포인터 증가
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
		//다음으로
		if((keyboard_control && GetKey(vkey_enter)) || (mouse_control && LeftDown()) || text_auto)
		{
			text_buffer[0] = NULL;	//텍스트 버퍼 지우기

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

//대화 출력
void CTextDlg::ShowText()
{
	//초기화가 안 되어 있으면 리턴
	if(!setting || !setting2)
	{
		printf("Warning : 대화창이 초기화 되어 있지 않습니다.");
		return;
	}

	//대화창 출력
	DrawDlg();

	RECT text_rect, shadow_rect;
	
	//그림자
	if(b_shadow)
	{
		SetRect(&shadow_rect, x+1, y+1, SCREEN_X, SCREEN_Y);
		jdd->DrawText(backbuffer, text_buffer, *text_font, &shadow_rect, JColor(0,0,0));
	}
	
	//글자
	SetRect(&text_rect, x, y, SCREEN_X, SCREEN_Y);
	jdd->DrawText(backbuffer, text_buffer, *text_font, &text_rect, text_color);
}

//대화창 락/언락
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