#include <stdio.h>
#include <math.h>

#include "donglib.h"
#include "GameProcessManager.h"

bool window_mode=true;

////////////////////////////////////////////////////////////
//초기화 작업
BOOL MainInitialize(char* window_name, BOOL use_keyboard, BOOL use_mouse, bool window_mode)
{
	jdd=CreateDirectDraw();

	HINSTANCE hInstance=(HINSTANCE)0x00400000;

	WNDCLASS wc={0};
	wc.hIcon=LoadIcon(hInstance,"ICON2.ico");
	wc.hCursor=LoadCursor(hInstance,IDC_ARROW);
	wc.lpfnWndProc=WndProc;
	wc.hInstance=hInstance;
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName="Game";
	RegisterClass(&wc);

	//창 모드
	if(window_mode)
	{
		LONG ws=WS_OVERLAPPEDWINDOW|WS_VISIBLE;
		ws &= ~WS_THICKFRAME;
		ws &= ~WS_MAXIMIZEBOX;

		RECT crt;
		SetRect(&crt, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		AdjustWindowRect(&crt, ws, FALSE);

		hwnd = CreateWindow("Game", window_name, ws, 100, 100, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);
	    ShowCursor( TRUE );
	}
	//전체 화면
	else
	{
		hwnd = CreateWindow("Game", window_name, WS_POPUP|WS_VISIBLE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);
	    ShowCursor( FALSE );
	}

	jdd->Initialize(NULL,hwnd,SCREEN_WIDTH,SCREEN_HEIGHT,16,true,window_mode);

	//그래픽 초기화
	backbuffer=jdd->GetBackBuffer();
	global_font=jdd->CreateFont("궁서",20);
	
	//로딩에 긴 시간이 걸릴 때를 대비한 화면
	if(jdd->LoadPicture("CommonLoading", "Loading.jpg", NULL, TRUE) || jdd->LoadPicture("CommonLoading", "Loading.gif", NULL, TRUE))
	{
		jdd->DrawPicture(backbuffer, "CommonLoading", 0, 0, NULL);
		jdd->Render();
		jdd->DeleteSurface("CommonLoading");
	}

	//임시 서페이스 생성
	JPictureInfo jpi;
	jpi.SetWidth(SCREEN_WIDTH);
	jpi.SetHeight(SCREEN_HEIGHT);
	jdd->CreateSurface("_CommonBlank", &jpi, TRUE);
	
	jdd->SetFrameRate(100,TRUE);
	jdd->SetVerticalSync(FALSE);

	//대화창용
	jdd->LoadPicture("_dlgbox", "DATA\\_dlgbox.gif", NULL, TRUE);

	//사운드 초기화
	if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
	{
		if (SoundOBJ->SetCooperativeLevel(hwnd,DSSCL_PRIORITY)!=DS_OK) return FALSE;

		memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
		DSB_desc.dwSize = sizeof(DSBUFFERDESC);
		DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

		if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK) return FALSE;
		SoundDSB -> SetVolume(0);
		SoundDSB -> SetPan(0);
	}

	//기타 초기화
	srand( (unsigned)time( NULL ) );
	keyboard_control=use_keyboard;
	mouse_control=use_mouse;
	gameover=FALSE;

	return TRUE;
}

//////////////////////////////////////////////////
//메인 코드
//////////////////////////////////////////////////
int main(char* arg[])
{
	//_crtBreakAlloc = 12057;

	//창 생성
	if(!MainInitialize("타임리스", TRUE, TRUE, window_mode))return 0;

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

	//뒤 처리
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
		case WM_LBUTTONDOWN  :	if(mouse_control)
								{
									LButton = TRUE;
								}
								SetCapture(hwnd);
								break;

		case WM_RBUTTONDOWN  :	if(mouse_control)
								{
									RButton = TRUE;
								}
								SetCapture(hwnd);
								break;

		case WM_LBUTTONUP    :	if(mouse_control)
								{
									LButton = FALSE;
								}
								ReleaseCapture();
								break;

		case WM_RBUTTONUP    :	if(mouse_control)
								{
									RButton = FALSE;
								}
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

		case MM_MCINOTIFY    :	if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL )
								{
									_MidiReplay();
								}
								break;

		case WM_DESTROY 	 :	_CrtDumpMemoryLeaks();
								break;

		case WM_SYSCOMMAND	 :  //닫기 메시지 가로채기
								if(wParam == SC_CLOSE)
								{
									wParam = 0;
									gameover = TRUE;
								}
								break;

		case WM_SIZE		 :	if(wParam == SIZE_MINIMIZED)
								{
									activate=false;
								}
								else
								{
									activate=true;
								}
								break;

		case WM_MOVE		 :	if(jdd)
								{
									jdd->OnMove(LOWORD(lParam), HIWORD(lParam));
								}
								break;
		
		case WM_ACTIVATE	 :	if(LOWORD(wParam))
								{
									activate=true;
								}
								else
								{
									activate=false;
								}
								break;
	}

	return DefWindowProc(wnd,msg,wParam,lParam);
}