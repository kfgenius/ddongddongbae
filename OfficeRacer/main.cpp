#include <stdio.h>
#include <math.h>

#include "donglib.h"
#include "GameProcessManager.h"

bool window_mode=true;

//////////////////////////////////////////////////
//메인 코드
//////////////////////////////////////////////////
int main(char* arg[])
{
	//_crtBreakAlloc = 12057;

	//창 생성
	if(!MainInitialize("Fever! Office Race", FALSE, TRUE, window_mode))return 0;

	//윈도우창 이동
	if(window_mode)
	{
		jdd->OnMove(100, 100);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}

	//메인 실행
	CGameProcessManager* m_gameprocessmanager = new CGameProcessManager();

	//메인 루프
	while(1)
	{
		//윈도우 메시지 처리
		if(!ProcessMessage())
		{
			break;
		}

		//게임 프로세스 처리
		if(!(m_gameprocessmanager->Process()))
		{
			break;
		}

		//렌더링
		jdd->Render();
	}

	delete m_gameprocessmanager;
	m_gameprocessmanager = NULL;

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
									//ShowCursor(FALSE);
								}
								break;

		case MM_MCINOTIFY    :	if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL )_MidiReplay();
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