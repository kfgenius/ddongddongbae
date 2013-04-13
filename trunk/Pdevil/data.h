#define PRC MSG msg; if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) ){if ( !GetMessage(&msg, NULL, 0, 0) ) return msg.wParam;TranslateMessage( &msg );DispatchMessage( &msg );}else if(Act)
#define SCENE SetRect(&BackRect, 0, 0, 800, 600); _DrawBmp(BackRect, 0, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT)
#define SetR(lprc, left, top, right, bottom) SetRect(lprc, left, top, left+right, top+bottom);

BOOL ReplayFlag, Quit=false, key, key2, Act=true;
extern HSNDOBJ Sound[];
RECT BackRect;

char* midi[]={"stage1.mid", "stage2.mid", "stage3.mid", "stage4.mid", "stage5.mid"};
//char *snr1[]={
// 	};


BOOL _MidiPlay( char* pszMidiFN, BOOL bReplayFlag )
{
    char szMCISendString[256];

    wsprintf(szMCISendString,"open %s type sequencer alias MUSIC", pszMidiFN);
   
    if ( mciSendString ( "close all", NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( szMCISendString, NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, MainHwnd ) != 0) return(FALSE);

    ReplayFlag = bReplayFlag;
    return TRUE;
}

BOOL _MidiStop( void )
{ 
    if ( mciSendString ( "close all", NULL, 0, NULL) != 0 ) return ( FALSE );
    return TRUE;
}

BOOL _MidiReplay( void )
{
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, MainHwnd) != 0 ) return ( FALSE );
    return TRUE;
}

void _Play( int num )
{
    if ( SoundCard ) SndObjPlay( Sound[num], NULL );
}

int Click()
{
	bool loop=true;
	key=true;
	while(loop)
	{
		PRC
		if(LeftButton||_GetKeyState(VK_RETURN)||_GetKeyState(VK_SPACE))
		{
			if(!key)
			{
				key=true;
				loop=false;
			}
		}
		else key=false;	
	}
	return 0;
}

void Change(int to)
{
	//√ ±‚»≠
	int i;
	if(to==99)
	{
		for(i=1;i<=100;i++)
		{
			_DrawBar(400-i*4,300-i*3,400+i*4,300+i*3,BLACK);
			_CopyScreen(false);
		}
	}
	else
	{
		char* fname[]={"DATA//main.bmp","DATA//end1.bmp","DATA//end2.bmp"};
		BmpScreen[0] = DDLoadBitmap( DirectOBJ, fname[to], 0, 0, SYSTEM);
		for(i=1;i<=100;i++){
			SetRect(&BackRect,400-i*4,300-i*3, 400+i*4,300+i*3);
			_DrawBmp(BackRect,400-i*4,300-i*3, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			_CopyScreen(false);
		}
	}
}
