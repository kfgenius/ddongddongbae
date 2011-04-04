#include <stdio.h>
#include <math.h>

#include "donglib.h"
#include "GameProcessManager.h"

bool window_mode=true;

////////////////////////////////////////////////////////////
//�ʱ�ȭ �۾�
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

	//â ���
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
	//��ü ȭ��
	else
	{
		hwnd = CreateWindow("Game", window_name, WS_POPUP|WS_VISIBLE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);
	    ShowCursor( FALSE );
	}

	jdd->Initialize(NULL,hwnd,SCREEN_WIDTH,SCREEN_HEIGHT,16,true,window_mode);

	//�׷��� �ʱ�ȭ
	backbuffer=jdd->GetBackBuffer();
	global_font=jdd->CreateFont("�ü�",20);
	
	//�ε��� �� �ð��� �ɸ� ���� ����� ȭ��
	if(jdd->LoadPicture("CommonLoading", "Loading.jpg", NULL, TRUE) || jdd->LoadPicture("CommonLoading", "Loading.gif", NULL, TRUE))
	{
		jdd->DrawPicture(backbuffer, "CommonLoading", 0, 0, NULL);
		jdd->Render();
		jdd->DeleteSurface("CommonLoading");
	}

	//�ӽ� �����̽� ����
	JPictureInfo jpi;
	jpi.SetWidth(SCREEN_WIDTH);
	jpi.SetHeight(SCREEN_HEIGHT);
	jdd->CreateSurface("_CommonBlank", &jpi, TRUE);
	
	jdd->SetFrameRate(100,TRUE);
	jdd->SetVerticalSync(FALSE);

	//��ȭâ��
	jdd->LoadPicture("_dlgbox", "DATA\\_dlgbox.gif", NULL, TRUE);

	//���� �ʱ�ȭ
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

	//��Ÿ �ʱ�ȭ
	srand( (unsigned)time( NULL ) );
	keyboard_control=use_keyboard;
	mouse_control=use_mouse;
	gameover=FALSE;

	return TRUE;
}

//////////////////////////////////////////////////
//���� �ڵ�
//////////////////////////////////////////////////
int main(char* arg[])
{
	//_crtBreakAlloc = 12057;

	//â ����
	if(!MainInitialize("Ÿ�Ӹ���", TRUE, TRUE, window_mode))return 0;

	//������â �̵�
	if(window_mode)
	{
		jdd->OnMove(100, 100);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}

	//���� ����
	CGameProcessManager* m_gameprocessmanager = new CGameProcessManager();

	//���� ����
	while(1)
	{
		//������ �޽��� ó��
		if(!ProcessMessage())
		{
			break;
		}

		//���� ���μ��� ó��
		if(!(m_gameprocessmanager->Process()))
		{
			break;
		}

		//������
		jdd->Render();
	}

	//�� ó��
	delete m_gameprocessmanager;
	m_gameprocessmanager = NULL;

	jdd->DeleteFont(global_font);

	return 0;
}

//������ �޼��� ó��
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

		case WM_SYSCOMMAND	 :  //�ݱ� �޽��� ����ä��
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