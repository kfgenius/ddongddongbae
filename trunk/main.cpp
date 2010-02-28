#include <stdio.h>
#include <math.h>

#include "donglib.h"
#include "dscript.h"

bool window_mode=false;
CScript* m_script;

//////////////////////////////////////////////////
//메인 코드
//////////////////////////////////////////////////
int main(char* arg[])
{
	//_crtBreakAlloc = 12057;

	FILE *mainfp;

	//시스템이 있는지 검사
	if(mainfp=fopen("system.dlg", "rb"))
	{
		fclose(mainfp);
	}
	else if(mainfp=fopen("system.txt", "rb"))
	{
		fclose(mainfp);
	}
	else return 0;	

	//윈도우 초기화
	char window_name[80];
	if(mainfp = fopen("setup.txt", "rt"))
	{
		if(fgets(window_name, 79, mainfp))
		{
			//제목 끝의 문자 제거
			for(size_t i=strlen(window_name)-1; i>0; --i)
			{
				if(window_name[i]==0x0a || window_name[i]==0x0d)window_name[i]=NULL;
				else break;
			}

			char win_buffer[5];
			if(fgets(win_buffer, 4, mainfp))
				if(strcmp(win_buffer, "WIN") == 0)window_mode=true;
		}

		fclose(mainfp);
	}
	else strcpy(window_name, "HungCript Ver1.5t");

	//창 생성
	if(!MainInitialize(window_name, TRUE, FALSE, window_mode))return 0;

	//윈도우창 이동
	if(window_mode)
	{
		jdd->OnMove(100, 100);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}
	
	//메인 실행
	m_script = new CScript("SYSTEM");
	m_script->Script(0);
	delete m_script;
	m_script = NULL;

	jdd->DeleteFont(global_font);

	return 0;
}

//윈도우 메세지 처리
LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch ( msg )
    {
		case WM_LBUTTONDOWN  :	if(mouse_control)LButton=TRUE;
								SetCapture(hwnd);
								break;

		case WM_RBUTTONDOWN  :	if(mouse_control)RButton=TRUE;
								SetCapture(hwnd);
								break;

		case WM_LBUTTONUP    :	if(mouse_control)LButton=FALSE;
								ReleaseCapture();
								break;

		case WM_RBUTTONUP    :	if(mouse_control)RButton=FALSE;
								ReleaseCapture();
								break;

		case WM_MOUSEMOVE    :	if(mouse_control)
								{
									MouseX=LOWORD(lParam);
									MouseY=HIWORD(lParam);
									mouse_move = TRUE;
									ShowCursor(FALSE);
								}
								break;

		case MM_MCINOTIFY    :	/*if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL ) */_MidiReplay();
								break;

		case WM_DESTROY 	 :	_CrtDumpMemoryLeaks();
								break;

		case WM_SYSCOMMAND	 :  //닫기 메시지 가로채기
								if(wParam==SC_CLOSE)
								{
									wParam=0;
									gameover=TRUE;
								}
								break;

		case WM_SIZE		 :	if(wParam == SIZE_MINIMIZED)activate=false;
								else activate=true;
								break;

		case WM_MOVE		 :	if(jdd)jdd->OnMove(LOWORD(lParam), HIWORD(lParam));
								break;
		
		case WM_ACTIVATE	 : if(LOWORD(wParam))activate=true;
								else activate=false;
							   break;
	}

	return DefWindowProc(wnd,msg,wParam,lParam);
}