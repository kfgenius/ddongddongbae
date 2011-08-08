//대화창 기본 클래스
#pragma once

#include "JDirectDraw.h"

#define DLG_MINIMUM_WIDTH	30
#define DLG_MINIMUM_LINE	1
#define DLG_WIDTH_ONE		10
#define DLG_LINE_ONE		20

#define TEXT_BUFFER_SIZE	1024

class CDlg
{
protected:
	//초기화 되었는지를 확인
	BOOL is_set_dlg, is_set_content;

	//대화창 정보
	unsigned int buffer_size;
	int n_of_e;						//줄 바꿈 횟수
	int x, y, width, line;			//X,Y 위치, 가로 크기, 세로 줄 수
	BOOL is_shadow;					//그림자를 넣는지 여부
	char* dlgbox;					//대화창 그림
	BOOL is_show_dlgbox;				//대화창을 표시하는지 여부
	JColor text_color;				//글자색
	int frame_x, frame_y;			//대화창 틀의 가로,세로 크기(기본 5, 임의의 대화창을 쓸때 조절)
	float opacity;					//창 투명도

	//대사 출력을 위한 변수
	int length;

	//사용 폰트
	JFont* text_font;

	void DrawDlg();					//대화창 보이기
	BOOL MouseInBox();				//마우스가 대화상자에 있는가를 검사

public:
	//생성자
	CDlg();
	~CDlg();

	void SetDlg();											//대화창 설정
	void Move(int mov_x, int mov_y);						//대화창 이동
	void ShowDlg();											//글상자 보이기
	void HideDlg();											//글상자 숨기기
	void ShowShadow();										//그림자 사용
	void HideShadow();										//그림자 숨기기
	
	int GetX(){ return x; }									//X좌표 가져오기
	int GetY(){ return y; }									//Y좌표 가져오기
	int GetWidth(){ return width * DLG_WIDTH_ONE; }			//가로 크기 가져오기
	int GetLine(){ return line; }							//세로 크기(줄 수) 가져오기
	float GetOpacity(){ return opacity; }					//불투명도 가져오기

	void SetX(int vx){ x = vx; }							//X좌표 설정
	void SetY(int vy){ y = vy; }							//Y좌표 설정
	void SetFont(JFont* font);								//폰트 설정
	void SetTextColor(BYTE r, BYTE g, BYTE b);				//글자색 설정
	void SetOpacity(float opacity);							//대화창 불투명도 설정
	void SetFrameSize(int vx, int vy);						//대화창 틀 크기 설정

	void SetDlgBox(char* dlgbox);							//글상자 설정
	void MakeDlgBox(char* dlgbox);							//글상자 만들기
};