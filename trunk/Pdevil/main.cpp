#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <ddraw.h>
#include <dsound.h>
#include <stdio.h>

#include "dsutil.h"
#include "ddutil.h"
#include "extern.h"
#include "battle.h"
#include "data.h"

bool gam=false;
DWORD Time, Ctime=0;

long FAR PASCAL WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch ( message )
	{
		case	WM_ACTIVATEAPP	:	Act=wParam;
									if(Act && gam){
										_ScreenMode( SCREEN_X, SCREEN_Y, BPP, FULL_SCREEN );
										_CopyScreen(false);
									}
									break;
		case	WM_LBUTTONDOWN	:	LeftButton = TRUE;
									break;
		case	WM_LBUTTONUP	:	LeftButton = FALSE;
									break;
		case	WM_RBUTTONDOWN	:	RightButton = TRUE;
									break;
		case	WM_RBUTTONUP	:	RightButton = FALSE;
									break;
        case    WM_MOUSEMOVE    :   MouseX = LOWORD(lParam);
	                                MouseY = HIWORD(lParam);
		                            break;
	   case    MM_MCINOTIFY    :    if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL ) _MidiReplay();
		                            break;
		case    WM_DESTROY      :  _WindowMode();
	                                PostQuitMessage( 0 );
		                            break;
	}
    return DefWindowProc( hWnd, message, wParam, lParam );
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    if ( !_GameMode(hInstance, nCmdShow, 800, 600, 16, true) ) return FALSE;

	//한글 초기화
    InitXddFont( "HANGUL.FNT", "ENGLISH.FNT" );
    SetFontPattern( NORMAL_PATTERN );

	//그래픽 초기화
	BmpScreen[0] = DDLoadBitmap( DirectOBJ, "data//main.bmp", 0, 0, SYSTEM);
	BmpScreen[1] = DDLoadBitmap( DirectOBJ, "data//stage1.bmp", 0, 0, SYSTEM);
	BmpScreen[2] = DDLoadBitmap( DirectOBJ, "data//sprite.bmp", 0, 0, SYSTEM);
	BmpScreen[3] = DDLoadBitmap( DirectOBJ, "data//face.bmp", 0, 0, SYSTEM);
	DDSetColorKey( BmpScreen[1], RGB(0,0,255) );
	DDSetColorKey( BmpScreen[2], RGB(0,0,255) );
	DDSetColorKey( BmpScreen[3], RGB(0,0,255) );

	//음향 초기화
	if ( SoundCard )
		{
			Sound[0] = SndObjCreate(SoundOBJ,"Sound//fire.WAV",2);
			Sound[1] = SndObjCreate(SoundOBJ,"Sound//boom.WAV",2);
			Sound[2] = SndObjCreate(SoundOBJ,"Sound//heel.WAV",2);
			Sound[3] = SndObjCreate(SoundOBJ,"Sound//wall.WAV",2);
			Sound[4] = SndObjCreate(SoundOBJ,"Sound//frz.WAV",2);
			Sound[5] = SndObjCreate(SoundOBJ,"Sound//summon.WAV",2);
			Sound[6] = SndObjCreate(SoundOBJ,"Sound//war.WAV",2);
			Sound[7] = SndObjCreate(SoundOBJ,"Sound//puck.WAV",2);
			Sound[8] = SndObjCreate(SoundOBJ,"Sound//bang.WAV",2);
			Sound[9] = SndObjCreate(SoundOBJ,"Sound//magic.WAV",2);
			Sound[10] = SndObjCreate(SoundOBJ,"Sound//wind.WAV",2);
			Sound[11] = SndObjCreate(SoundOBJ,"Sound//jump.WAV",2);
			Sound[12] = SndObjCreate(SoundOBJ,"Sound//hat.WAV",2);
        }
	//변수 초기화
	CBattle battle;
	battle.Clear();
	randomize();
	_MidiPlay("main.mid",true);
	int mode=0, cur=0, doki, doki2;
	DWORD curTempo=GetTickCount();
	gam=true; battle.com=true;
	int type[2];
	type[0]=type[1]=0;
	battle.lv=1; battle.exp=0; battle.next=64;
	Change(0);
	MSG msg;
	int i;
	char* TYPE[]={"  강엄한  "," 고뇌하는 ","  순수한  ","통솔력있는",
				"무식한","영악한","멍청한","비열한"};
	while( !Quit )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &msg, NULL, 0, 0 ) )
			{
				 return msg.wParam;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if( Act )
		{
			switch(mode){
			case 0:
			SetR(&BackRect,0,0,800,600);
			_DrawBmp(BackRect, 0, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			if(cur<3)doki=cur;
				else doki=1;
			doki2=(battle.com)?440: 600;
			_DrawBox(doki2+doki-1,299+doki,162-doki*2,122-doki*2,GREEN);
			_DrawBox(doki2+doki,300+doki,160-doki*2,120-doki*2,GREEN);
			if(GetTickCount()-curTempo>100)
			{
				curTempo=GetTickCount();
				cur++;
				if(cur>3)cur=0;
				Time=GetTickCount();
			}
			//성격 결정
			for(i=0;i<4;i++)
			{
				if(type[0]!=i)PutFontOutline(30+(80*i), 440, RGB2(0,0,0),TYPE[i]);
					else PutFontOutline(30+(80*i), 440, YELLOW,TYPE[i]);
			}
			for(i=0;i<4;i++)
			{
				if(type[1]!=i)PutFontOutline(430+(80*i), 440, RGB2(0,0,0),TYPE[i+4]);
					else PutFontOutline(430+(80*i), 440, YELLOW,TYPE[i+4]);
			}
			SetR(&BackRect,180, 250, 70, 60);
			_DrawBmp(BackRect, MouseX, MouseY, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			if(LeftButton)
			{
				if(MouseY>500)
				{
					mode=1;
					battle.type[0]=type[0];
					battle.type[1]=type[1];
					if(type[1]==3)battle.exp=64;
					BmpScreen[0] = DDLoadBitmap( DirectOBJ, "data//status.bmp", 0, 0, SYSTEM);
					_MidiStop();
					Change(99);
					_MidiPlay("stage1.mid",true);
				}
				if(MouseY>250 && MouseY<420)
				{
					if(MouseX>=440 && MouseX<600)battle.com=true;
						else if(MouseX>=600 && MouseX<760)battle.com=false;
				}
				if(MouseY>440 && MouseY<460)
				{
					for(i=0; i<4; i++)
						if(MouseX>30+(80*i) && MouseX<110+(80*i))type[0]=i;
					for(i=0; i<4; i++)
						if(MouseX>430+(80*i) && MouseX<510+(80*i))type[1]=i;
				}
			}
			_CopyScreen(false);
			break;
			case 1:
			if(_GetKeyState(VK_ESCAPE))Quit=true;
			int* wav;
			wav=battle.Battle();
			for(int i=0;i<SNDMAX;i++)
				if(*(wav+i)>=0)_Play(*(wav+i));
				if(*(wav+SNDMAX)>=0){
					_MidiStop();
					_MidiPlay(midi[*(wav+SNDMAX)],true);
				}
			_CopyScreen(false);
			if(battle.win && !Ctime)Ctime=(GetTickCount()-Time)/1000;
			if(battle.win && !battle.tdelay)
			{
				Change(99);
				_MidiStop();
				if(battle.win==1){Change(1); _MidiPlay("end1.mid", false);}
					else{Change(2); _MidiPlay("end2.mid", false);}
				Quit=true;
				PutFontOutline(300,0,YELLOW,"클리어 타임: %d초",Ctime);
				_CopyScreen(false);
				Click();
			}
			break;
			}
		}
		else
		{
			WaitMessage();
		}
	}
	_MidiStop();
	Change(99);
	DestroyWindow( MainHwnd );

    return TRUE;
}