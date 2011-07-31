//선택을 위한 대화상자

#include "Global.h"
#include "SelectDlg.h"

#define DLG_KEY_DELAY		10

CSelectDlg::CSelectDlg(int select_max, unsigned int vbuffer_size)
{
	buffer_size = vbuffer_size;
	dlg_buffer = new char[buffer_size];
	select_point = new char*[select_max];

	is_set_content = FALSE;
}

CSelectDlg::~CSelectDlg()
{
	if(dlg_buffer)
	{
		delete [] dlg_buffer;
		dlg_buffer = NULL;
	}

	if(select_point)
	{
		delete [] select_point;
		select_point = NULL;
	}
}

void CSelectDlg::SetDlg(int x, int y, int width, int line, BOOL is_row, int start)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->line = line;
	select = start;

	//가로 크기를 DLG_WIDTH_ONE으로 떨어지게 수정
	width = Max(width, DLG_MINIMUM_WIDTH);
	this->width = width / DLG_WIDTH_ONE;

	//가로선택지인지 세로 선택지인지 확인, 길이 조정
	line = Max(line, DLG_MINIMUM_LINE);
	this->is_row = is_row;

	is_set_dlg = TRUE;
}

void CSelectDlg::MakeSelection(char* input_dlg)
{
	if(is_set_dlg == FALSE || input_dlg == NULL)
	{
		return;
	}

	//버퍼 크기를 넘어가는 것을 방지
	strncpy(dlg_buffer, input_dlg, buffer_size);
	if(strlen(input_dlg) >= buffer_size)
	{
		dlg_buffer[buffer_size - 1] = NULL;
	}

	int length = (int)strlen(dlg_buffer);

	//대화를 출력하기 알맞게 처리
	n_of_e = 0;
	int bp = 1;
	select_point[0] = dlg_buffer;
	for(int i = 0; i < length; i++)
	{
		if(dlg_buffer[i] == '\\')
		{
			dlg_buffer[i] = NULL;
			select_point[bp] = dlg_buffer + i + 1;
			n_of_e++;
			++bp;
			continue;
		}

		if(dlg_buffer[i] == 0x0d)
		{
			continue;
		}
	}

	//스크립트에서 읽어 온 문장의 끝에 음표(0x0d)가 생기는 것 방지
	if(dlg_buffer[length - 1] == 0x0d)
	{
		dlg_buffer[length - 1] = NULL;
	}//@_@

	//대화창 표시에 필요한 것들 계산
	if(is_row)
	{
		gap = (width * DLG_WIDTH_ONE) / line;

		sx = x + select % line * gap;
		sy = y + (select / line) * DLG_LINE_ONE;
	}
	else
	{
		gap = (width * DLG_WIDTH_ONE) / (n_of_e / line + 1);

		sx = x + select / line * gap;
		sy = y + (select % line) * DLG_LINE_ONE;
	}

	is_set_content = TRUE;
}

int CSelectDlg::Select(char* input_dlg, BOOL cancel)
{
	//초기화가 안 되어 있으면 리턴
	if(is_set_dlg == FALSE || is_set_content == FALSE)
	{
		printf("Warning : 대화창이 초기화 되어 있지 않습니다.");
		return SELECT_CANCEL;
	}

	MakeSelection(input_dlg);

	//뒷배경 잠시 저장(투명도 있는 대화창일 때 겹쳐 쓰면서 투명도가 없어지는 것을 방지
	jdd->DrawPicture("_CommonBlank", backbuffer, 0, 0, NULL);

	while(TRUE)
	{
		if(!ProcessMessage())break;		//윈도우 메세지 처리

		jdd->DrawPicture(backbuffer, "_CommonBlank", 0, 0, NULL);

		int selected = Selecting();
		if(selected>=0 || (cancel && selected == SELECT_CANCEL))	//선택하거나 취소하면 중단
		{
			break;
		}

		jdd->Render();
	}

	return select;
}

int CSelectDlg::Selecting()
{
	//초기화가 안 되어 있으면 리턴
	if(!is_set_content)
	{
		printf("Warning : 대화창이 초기화 되어 있지 않습니다.");
		return SELECT_CANCEL;
	}

	//대화상자
	if(keyboard_control)
	{
		if(is_row)
		{
			//위 아래 이동
			if(GetKey(vkey_right, DLG_KEY_DELAY))
			{
				select = Min(n_of_e, select + 1);
			}
			else if(GetKey(vkey_left, DLG_KEY_DELAY))
			{
				select = Max(0, select - 1);
			}

			//좌우 이동
			if(GetKey(vkey_down,DLG_KEY_DELAY))
			{
				if(select + line <= n_of_e)
				{
					select += line;
				}
			}
			else if(GetKey(vkey_up,DLG_KEY_DELAY))
			{
				if(select - line >= 0)
				{
					select-=line;
				}
			}

			sx = x + select % line * gap;
			sy = y + (select / line) * DLG_LINE_ONE;
		}
		else
		{
			//위 아래 이동
			if(GetKey(vkey_down, DLG_KEY_DELAY))
			{
				select=Min(n_of_e, select + 1);
			}
			else if(GetKey(vkey_up, DLG_KEY_DELAY))
			{
				select=Max(0, select - 1);
			}

			//좌우 이동
			if(GetKey(vkey_right, DLG_KEY_DELAY))
			{
				if(select + line <= n_of_e)
				{
					select += line;
				}
			}
			else if(GetKey(vkey_left, DLG_KEY_DELAY))
			{
				if(select-line >= 0)
				{
					select -= line;
				}
			}

			sx = x + select / line * gap;
			sy = y + (select % line) * DLG_LINE_ONE;
		}
	}

	//마우스로 선택
	BOOL mouse_select = FALSE;
	if(mouse_control && MouseInBox())
	{
		for(int i = 0; i <= n_of_e; i++)
		{
			//선택지 영역 계산
			int tsx, tsy;
			if(is_row)
			{
				tsx = x + (i % line) * gap;
				tsy = y + (i / line) * DLG_LINE_ONE;
			}
			else
			{
				tsx = x + (i / line) * gap;
				tsy = y + (i % line) * DLG_LINE_ONE;
			}

			if(MouseX >= tsx && MouseX < tsx + gap && MouseY >= tsy && MouseY < tsy + DLG_LINE_ONE)
			{
				//이동이 있을 때만 검사
				if(mouse_move)
				{
					select = i;
					sx = tsx;
					sy = tsy;
					mouse_move = FALSE;
				}

				mouse_select = TRUE;
				break;
			}
		}
	}

	DrawDlg();

	//선택 커서
	RECT src_rect;
	SetRect(&src_rect, 0, 0, gap / 2, DLG_LINE_ONE);
	jdd->DrawPicture(backbuffer, "CommonSelect", sx, sy, &src_rect);
	SetRect(&src_rect, 320 - gap/2, 0, 320, DLG_LINE_ONE);
	jdd->DrawPicture(backbuffer, "CommonSelect", sx + gap / 2, sy, &src_rect);

	//선택지들
	for(int i = 0; i <= n_of_e; i++)
	{
		int tx, ty;
		if(is_row)
		{
			tx = (i % line) * gap + x;
			ty = (i / line) * DLG_LINE_ONE + y;
		}
		else
		{
			tx = (i / line) * gap + x;
			ty = (i % line) * DLG_LINE_ONE + y;
		}

		if(is_shadow)
		{
			jdd->DrawText(backbuffer, select_point[i], *text_font, tx + 1, ty + 1, JColor(0,0,0));
		}
		jdd->DrawText(backbuffer, select_point[i], *text_font, tx, ty, text_color);
	}

	//마우스 결정
	if(LeftDown() && mouse_select)
	{
		return select;
	}
	if(RightDown() && mouse_control)
	{
		return SELECT_CANCEL;
	}

	//키보드 결정
	if(keyboard_control && GetKey(vkey_enter))
	{
		return select;
	}
	if(keyboard_control && GetKey(vkey_esc))
	{
		return SELECT_CANCEL;
	}
	
	return SELECT_NONE;
}

void CSelectDlg::SetSelect(int select)
{
	this->select = select;
}