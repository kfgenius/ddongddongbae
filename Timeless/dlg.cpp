#include <stdio.h>
//#include <string.h>
//#include <tchar.h>

#include "dlg.h"
#include "Global.h"

#define DLG_COLOR_KEY	BLUE

/////////////////////////////////////////////////////
//기본 대화 처리 클래스 메소드
#define DLG_TILE_SIZE	10

CDlg::CDlg()
{
	is_set_dlg = FALSE;
	is_set_content = FALSE;
	is_shadow = FALSE;
	dlgbox = NULL;
	is_show_dlgbox = FALSE;
	
	SetTextColor(0, 0, 0);
	
	text_font = &global_font;
	frame_x = 10;
	frame_y = 10;
	opacity = 1.0f;
}

CDlg::~CDlg()
{
	JPictureInfo pi;
	if(jdd->GetPictureInfo(dlgbox, &pi))
	{
		jdd->DeleteSurface(dlgbox);
	}
}

//대화창 조각을 조합해서 그려주는 함수
void CDlg::MakeDlgBox(char* dlgbox)
{
	JPictureInfo pi;
	if(jdd->GetPictureInfo(dlgbox, &pi))
	{
		jdd->DeleteSurface(dlgbox);	//이미 그림이 존재하면 지우기
	}

	//새로운 대화창 생성
	pi.SetWidth((frame_x * 2) + (width * DLG_WIDTH_ONE));
	pi.SetHeight((frame_y * 2) + (line * DLG_LINE_ONE));
	pi.SetColorKey(DLG_COLOR_KEY);
	pi.SetOpacity(opacity);
	jdd->CreateSurface(dlgbox, &pi, TRUE);

	//바탕은 투명색
	JBrush blue_brush;
	blue_brush = jdd->CreateBrush(DLG_COLOR_KEY);
	RECT pic_rect;
	SetRect(&pic_rect, 0, 0, (frame_x * 2) + (width * DLG_TILE_SIZE), (frame_y * 2) + (line * DLG_TILE_SIZE * 2));
	jdd->DrawRect(dlgbox, blue_brush, &pic_rect);
	jdd->DeleteBrush(blue_brush);

	//대화창 미리 그리기
	RECT src_rect[3];
	
	for(int i = -1; i <= line * 2; i++)
	{
		int tmp_y;
		//위, 아래, 중간
		if(i == -1)
		{
			tmp_y = 0;
		}
		else if(i == line * 2)
		{
			tmp_y = frame_y * 2;
		}
		else
		{
			tmp_y = frame_y;
		}

		//좌, 중, 우
		for(int j = 0; j < 3; ++j)
		{
			SetRect(&src_rect[j], frame_x * j, tmp_y, frame_x * (j + 1), tmp_y + frame_y);
		}

		//그리기
		for(int j = -1; j <= width; ++j)
		{
			if(j == -1)
			{
				jdd->DrawPicture(dlgbox,"_dlgbox", (j + 1) * DLG_TILE_SIZE, (i + 1) * DLG_TILE_SIZE, &src_rect[0]);
			}
			else if(j == width)
			{
				jdd->DrawPicture(dlgbox,"_dlgbox", (j + 1) * DLG_TILE_SIZE, (i + 1) * DLG_TILE_SIZE,&src_rect[2]);
			}
			else
			{
				jdd->DrawPicture(dlgbox,"_dlgbox", (j + 1) * DLG_TILE_SIZE, (i + 1) * DLG_TILE_SIZE, &src_rect[1]);
			}
		}
	}

	this->dlgbox = dlgbox;
	is_show_dlgbox = TRUE;
}

//대화창 이동
void CDlg::Move(int mov_x, int mov_y)
{
	if(!is_set_dlg || !is_set_content)
	{
		printf("Warning : 대화창이 초기화 되어 있지 않습니다.");
		return;
	}

	x += mov_x;
	y += mov_y;
}

void CDlg::SetTextColor(BYTE r, BYTE g, BYTE b)
{
	text_color.r = r;
	text_color.g = g;
	text_color.b = b;
}

void CDlg::SetDlgBox(char* dlgbox)
{
	//사용자가 지정한 대화창 쓰기
	this->dlgbox = dlgbox;
	is_show_dlgbox = TRUE;
}

void CDlg::SetFont(JFont *font)
{
	if(font == NULL)
	{
		printf("Warning : 폰트가 존재하지 않습니다.");
		return;
	}

	text_font = font;
}

void CDlg::DrawDlg()
{
	//초기화가 안 되어 있으면 리턴
	if(!is_set_dlg)
	{
		printf("Warning : 대화창이 초기화 되어 있지 않습니다.");
		return;
	}

	//대화창 출력
	if(is_show_dlgbox && dlgbox != NULL)
	{
		jdd->DrawPicture(backbuffer, dlgbox, x - frame_x, y - frame_y, NULL);
	}
}

//그림자 보이기 숨기기
void CDlg::ShowShadow()
{
	is_shadow = TRUE;
}

void CDlg::HideShadow()
{
	is_shadow = FALSE;
}

//글상자 보이기 숨기기
void CDlg::ShowDlg()
{
	is_show_dlgbox = TRUE;
}

void CDlg::HideDlg()
{
	is_show_dlgbox = FALSE;
}

//마우스가 대화창 위에 있는지 검사
BOOL CDlg::MouseInBox()
{
	if(mouse_control && MouseX >= x && MouseX < x + width * DLG_TILE_SIZE && MouseY >= y && MouseY < y + line * DLG_TILE_SIZE * 2)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//투명도 검사
void CDlg::SetOpacity(float opacity)
{
	//대화창이 없으면 돌아가기
	if(dlgbox == NULL)
	{
		printf("Warning : 대화창이 지정되어 있지 않습니다.");
		return;
	}

	JPictureInfo pi;
	jdd->GetPictureInfo(dlgbox, &pi);
	pi.SetOpacity(opacity);
	jdd->SetPictureInfo(dlgbox, &pi);

	this->opacity = opacity;
}

//대화창 여백 지정
void CDlg::SetFrameSize(int vx, int vy)
{
	frame_x = vx;
	frame_y = vy;
}