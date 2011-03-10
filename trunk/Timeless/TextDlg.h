#pragma once

#include "dlg.h"

#define TEXT_DLG_END	999
#define TEXT_DLG_ERROR	-1

//출력을 위한 대화상자
class CTextDlg:public CDlg
{
private:
	char* dlg_buffer;
	char* text_buffer;
	BOOL lock;
	int tp, sp;			//한글자씩 출력하기 위한 포인터, tp는 지금 출력되는 대화창의 포인터, sp는 전체 내용의 포인터
	BOOL scroll_end;	//글자가 출력되는 애니메이션이 끝났는지 여부
	BOOL text_auto;

public:
	//생성자 소멸자
	CTextDlg(unsigned int vbuffer_size=1024);
	~CTextDlg();

	void SetDlg(int vx, int vy, int vwidth, int vline);
	void MakeText(char* content);
	void ShowText();
	void Print(char* content);
	void Next();
	void PrintOneCharacter();
	int Printing();

	void Lock();		//창을 못 닫게 함
	void UnLock();		//창을 닫을 수 있게 함

	//오토 관련(대화 자동 넘김)
	void SetTextAuto(BOOL on_off);
	BOOL GetTextAuto();
};