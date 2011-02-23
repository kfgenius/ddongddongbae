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

//사운드
LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

BOOL ReplayFlag;

BOOL mouse_control;	//마우스 사용 여부
BOOL keyboard_control;	//키보드 사용 여부

int MouseX, MouseY;	//마우스 좌표
BOOL LButton=FALSE, RButton=FALSE;	//마우스 클릭여부
BOOL left_click=FALSE, right_click=FALSE;	//마우스 클릭은 한번만 검사하기 위한 변수
BOOL mouse_move=FALSE;

//한글 처리
static int is_eng=1;

//프레임 측정
int fps_n=0, fps_count=0;
DWORD dwStart = GetTickCount();

//메세지 처리
BOOL ProcessMessage()
{
	if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	{
		if(!GetMessage(&msg,NULL,0,0)) return FALSE;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//프레임 측정
	/*++fps_count;
	if(GetTickCount()-dwStart>=1000)
	{
		dwStart = GetTickCount();
		fps_n=fps_count;
		fps_count=0;
	}*/

	if(gameover)return FALSE;	//강제 종료

	return TRUE;
}

int GetFPS()
{
	return fps_n;
}

//미디 연주
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

//동영상 재생
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

//최대 최소값
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

//키 처리
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

//마우스 버튼
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

//폰트가 '궁서', 크기20일 때(global_font), 각 캐릭터들의 길이를 돌려주는 함수
//(궁서가 아니면 자동 줄바꿈이 제대로 작동 안 할 수 있음
//글꼴마다 길이가 틀리니 어쩔 수 없이 궁서를 표준으로 했음
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

//화면 지우기
void ClearScreen()
{
	ColorMatrix2 CM_bg={{{0,0,0,0},{0,0,0,0},{0,0,0,0}}};
	jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_bg);
}

//페이드 인/아웃
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

//페이드 아웃 후 바로 페이드인 할 수 없습니다.(BackBuffer가 검게 지워졌기 때문에)
//다음 장면을 그린 후 FadeIn을 해주세요. WhiteOut도 마찬가지
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

//화이트 인/아웃
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
//초기화 작업(매번 바뀌지 않으니까 main에서 donglib로 옮겼음
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
		SetRect(&crt, 0, 0, SCREEN_X, SCREEN_Y);
		AdjustWindowRect(&crt, ws, FALSE);

		hwnd = CreateWindow("Game", window_name, ws, 100, 100, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);
	    ShowCursor( TRUE );
	}
	//전체 화면
	else
	{
		hwnd = CreateWindow("Game", window_name, WS_POPUP|WS_VISIBLE, 0, 0, SCREEN_X, SCREEN_Y, NULL, NULL, hInstance, NULL);
	    ShowCursor( FALSE );
	}

	jdd->Initialize(NULL,hwnd,SCREEN_X,SCREEN_Y,16,true,window_mode);

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
	jpi.SetWidth(SCREEN_X);
	jpi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonBlank", &jpi, TRUE);
	
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
