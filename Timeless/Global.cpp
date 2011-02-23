#include <dsound.h>

#include "Global.h"
#include "JDirectDraw.h"

JDirectDraw* jdd;

char* backbuffer;

JFont global_font;

HWND hwnd;
MSG msg;
BOOL activate;
BOOL gameover;

//����
LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

BOOL ReplayFlag;

BOOL mouse_control;	//���콺 ��� ����
BOOL keyboard_control;	//Ű���� ��� ����

int MouseX, MouseY;	//���콺 ��ǥ
BOOL LButton=FALSE, RButton=FALSE;	//���콺 Ŭ������
BOOL left_click=FALSE, right_click=FALSE;	//���콺 Ŭ���� �ѹ��� �˻��ϱ� ���� ����
BOOL mouse_move=FALSE;

//�ѱ� ó��
static int is_eng=1;

//������ ����
int fps_n=0, fps_count=0;
DWORD dwStart = GetTickCount();

//�޼��� ó��
BOOL ProcessMessage()
{
	if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	{
		if(!GetMessage(&msg,NULL,0,0)) return FALSE;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//������ ����
	/*++fps_count;
	if(GetTickCount()-dwStart>=1000)
	{
		dwStart = GetTickCount();
		fps_n=fps_count;
		fps_count=0;
	}*/

	if(gameover)return FALSE;	//���� ����

	return TRUE;
}

int GetFPS()
{
	return fps_n;
}

//�̵� ����
BOOL _MidiPlay(char* pszMidiFN, BOOL bReplayFlag)
{
    char szMCISendString[256];

    wsprintf(szMCISendString,"open %s type sequencer alias MUSIC", pszMidiFN);

    if ( mciSendString ( "close all", NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( szMCISendString, NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, hwnd ) != 0) return(FALSE);

    ReplayFlag = bReplayFlag; 
    return TRUE;
}

BOOL _MidiStop()
{
    if ( mciSendString ( "close all", NULL, 0, NULL) != 0 ) return ( FALSE );
    return TRUE;
}

BOOL _MidiReplay()
{
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, hwnd) != 0 ) return ( FALSE );
    return TRUE;
}

void _Play( HSNDOBJ sound )
{
    if ( SoundOBJ ) SndObjPlay( sound, NULL );
}

//������ ���
DWORD_PTR AviCount;

BOOL MciOpen( HWND hwnd, MCIDEVICEID *wDeviceID, char *filename )
{
    DWORD dwError;
    MCI_DGV_OPEN_PARMS mciOpen;
    MCI_STATUS_PARMS mciStatus;
    MCI_DGV_RECT_PARMS mciRect;

    mciOpen.lpstrDeviceType="avivideo";//MPEGVideo";//"avivideo";//"ActiveMovie";
    mciOpen.hWndParent=hwnd;
    mciOpen.dwStyle=WS_CHILD;
    mciOpen.lpstrElementName=filename;

    dwError=mciSendCommand (
            (WORD)NULL, 
            (WORD)MCI_OPEN,
            (DWORD_PTR)(MCI_DGV_OPEN_PARENT|MCI_DGV_OPEN_WS|MCI_OPEN_TYPE|MCI_OPEN_ELEMENT),
            (DWORD_PTR)(LPVOID)&mciOpen );

    if (dwError)
    {
        char lpszErrorText[80];
        mciGetErrorString( dwError, lpszErrorText, 79 );
    }
    *wDeviceID=mciOpen.wDeviceID;

    mciRect.rc.left   = 0;
    mciRect.rc.top    = 0;
    mciRect.rc.right  = 639;
    mciRect.rc.bottom = 479;

	mciSendCommand( *wDeviceID, MCI_PUT, MCI_DGV_PUT_WINDOW | MCI_DGV_RECT, (DWORD_PTR)(LPVOID)&mciRect );

    mciStatus.dwItem = MCI_STATUS_LENGTH;
    mciSendCommand( *wDeviceID, MCI_STATUS, (DWORD_PTR)MCI_STATUS_ITEM, (DWORD_PTR)(LPVOID)&mciStatus );
    AviCount = mciStatus.dwReturn;

    return TRUE;
}

DWORD PlayMovie( WORD wDevID, DWORD dwFrom, DWORD dwTo ) 
{ 
    DWORD dwFlags = 0; 
    MCI_DGV_PLAY_PARMS mciPlay;
 
    if (dwFrom)
    {
        mciPlay.dwFrom = dwFrom;
        dwFlags |= MCI_FROM;
    } 
 
    if (dwTo)
    {
        mciPlay.dwTo = dwTo;
        dwFlags |= MCI_TO;
    } 
    dwFlags |= MCI_NOTIFY;

    return mciSendCommand(wDevID, MCI_PLAY, (DWORD_PTR)dwFlags, (DWORD_PTR)(LPVOID)&mciPlay);
} 

DWORD CloseMovie( WORD wDevID )
{
    return mciSendCommand(wDevID, MCI_CLOSE, 0, NULL);
}

DWORD _PlayAVI( char* name )
{
    MCIDEVICEID mcidec;
    MCI_STATUS_PARMS mciStatus;
    FILE *file;

    file = fopen( name, "rb" ); if ( file == NULL ) return FALSE;
    fclose( file );

    MciOpen( hwnd, &mcidec, name );
    PlayMovie( mcidec, 0, 0 );

    mciStatus.dwItem = MCI_STATUS_POSITION;

    while ( !_GetKeyState(VK_ESCAPE) )
    {
        mciSendCommand( mcidec, MCI_STATUS, (DWORD_PTR)MCI_STATUS_ITEM, (DWORD_PTR)(LPVOID)&mciStatus );
        if ( mciStatus.dwReturn == AviCount ) break;
    }
    CloseMovie( mcidec );

    return TRUE;
}

//�ִ� �ּҰ�
int Max(int x, int y)
{
	if(x>y)return x;
		else return y;
}

int Min(int x, int y)
{
	if(x<y)return x;
		else return y;
}

int MaxMin(int x, int min, int max)
{
	if(x<min)return min;
		else if(x>max) return max;
		else return x;
}

BOOL Between(int value, int min, int max)
{
	if(value>=min && value<=max)return TRUE;
		else return FALSE;
}

//Ű ó��
int keypush[ALL_KEYS];

BOOL GetKey(ValidKey key, int delay)
{
	if(!activate)return false;

	BOOL push;
	if(delay==-1)push=FALSE;
		else push=TRUE;

	if(_GetKeyState(keys[key]))
	{
		if(!keypush[key])
		{
			keypush[key]=delay;
			return TRUE;
		}
		else if(push)
		{
			keypush[key]=Max(0,keypush[key]-1);
		}
	}
	else keypush[key]=0;
	
	return FALSE;
}

//���콺 ��ư
BOOL LeftDown()
{
	if(LButton)
	{
		if(!left_click)
		{
			left_click=TRUE;
			return TRUE;
		}
	}
	else left_click=FALSE;

	return FALSE;
}

BOOL RightDown()
{
	if(RButton)
	{
		if(!right_click)
		{
			right_click=TRUE;
			return TRUE;
		}
	}
	else right_click=FALSE;

	return FALSE;
}

//��Ʈ�� '�ü�', ũ��20�� ��(global_font), �� ĳ���͵��� ���̸� �����ִ� �Լ�
//(�ü��� �ƴϸ� �ڵ� �ٹٲ��� ����� �۵� �� �� �� ����
//�۲ø��� ���̰� Ʋ���� ��¿ �� ���� �ü��� ǥ������ ����
int GetCharLength(char tmp_char)
{
 	if(tmp_char<0x20 || tmp_char>0x7e)return 10;
 
 	int char_length[95]={7/*sp*/, 7/*!*/, 7/*"*/, 11/*#*/, 10/*$*/, 13/*%*/, 12/*&*/, 7/*'*/,
		7/*(*/, 7/*)*/, 10/***/, 10/*+*/, 7/*,*/, 10/*-*/, 7/*.*/, 7/*/*/, 11/*0*/, 11, 11,
		11, 11, 11, 11, 11, 11, 11/*9*/, 7/*:*/, 7/*;*/, 13/*<*/, 11/*=*/, 13/*>*/, 7/*?*/,
		13/*@*/, 13/*A*/, 12, 12, 12, 12, 11, 13, 13/*H*/, 8, 11, 12, 12, 14, 12, 13/*O*/,
		11, 14, 12, 11, 10, 13, 12/*V*/, 16, 12, 12, 11/*Z*/, 10/*[*/, 16/*\*/, 10/*]*/,
		11/*^*/, 10/*_*/, 7/*`*/, 11/*a*/, 13, 12, 12, 12, 8, 12, 12/*h*/, 8, 9, 11, 8, 14, 12,
		12/*o*/, 12, 12, 10, 10, 9, 12, 12/*v*/, 13, 10, 11, 11/*z*/, 10/*{*/, 10/*|*/, 10/*}*/, 16/*~*/};

	return char_length[tmp_char-0x20];
}

//ȭ�� �����
void ClearScreen()
{
	ColorMatrix2 CM_bg={{{0,0,0,0},{0,0,0,0},{0,0,0,0}}};
	jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_bg);
}

//���̵� ��/�ƿ�
void FadeOut(int delay)
{
	jdd->DrawPicture("CommonBlank", backbuffer, 0, 0, NULL);
	for(int fade=0; fade<=256; fade+=8)
	{
		jdd->ApplyColorMatrix(backbuffer, "CommonBlank", 0, 0, NULL, CM_fadeout(fade));
		jdd->Render();
		Sleep(delay);
	}
}

//���̵� �ƿ� �� �ٷ� ���̵��� �� �� �����ϴ�.(BackBuffer�� �˰� �������� ������)
//���� ����� �׸� �� FadeIn�� ���ּ���. WhiteOut�� ��������
void FadeIn(int delay)
{
	jdd->DrawPicture("CommonBlank", backbuffer, 0, 0, NULL);
	for(int fade=256; fade>=0; fade-=8)
	{
		jdd->ApplyColorMatrix(backbuffer, "CommonBlank", 0, 0, NULL, CM_fadeout(fade));
		jdd->Render();
		Sleep(delay);
	}
}

//ȭ��Ʈ ��/�ƿ�
void WhiteOut(int delay)
{
	jdd->DrawPicture("CommonBlank", backbuffer, 0, 0, NULL);
	for(int fade=0; fade<=256; fade+=8)
	{
		jdd->ApplyColorMatrix(backbuffer, "CommonBlank", 0, 0, NULL, CM_whiteout(fade));
		jdd->Render();
		Sleep(delay);
	}
}

void WhiteIn(int delay)
{
	jdd->DrawPicture("CommonBlank", backbuffer, 0, 0, NULL);
	for(int fade=256; fade>=0; fade-=8)
	{
		jdd->ApplyColorMatrix(backbuffer, "CommonBlank", 0, 0, NULL, CM_whiteout(fade));
		jdd->Render();
		Sleep(delay);
	}
}


////////////////////////////////////////////////////////////
//�ʱ�ȭ �۾�(�Ź� �ٲ��� �����ϱ� main���� donglib�� �Ű���
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
		SetRect(&crt, 0, 0, SCREEN_X, SCREEN_Y);
		AdjustWindowRect(&crt, ws, FALSE);

		hwnd = CreateWindow("Game", window_name, ws, 100, 100, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);
	    ShowCursor( TRUE );
	}
	//��ü ȭ��
	else
	{
		hwnd = CreateWindow("Game", window_name, WS_POPUP|WS_VISIBLE, 0, 0, SCREEN_X, SCREEN_Y, NULL, NULL, hInstance, NULL);
	    ShowCursor( FALSE );
	}

	jdd->Initialize(NULL,hwnd,SCREEN_X,SCREEN_Y,16,true,window_mode);

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
	jpi.SetWidth(SCREEN_X);
	jpi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonBlank", &jpi, TRUE);
	
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
