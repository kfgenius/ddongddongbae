#define PRC MSG msg; if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) ){if ( !GetMessage(&msg, NULL, 0, 0) ) return msg.wParam;TranslateMessage( &msg );DispatchMessage( &msg );}else if(Act)
#define SCENE SetRect(&BackRect, 0, 0, 800, 600); _DrawBmp(BackRect, 0, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT)

BOOL ReplayFlag, Quit=false, key, key2, Act=true;
extern HSNDOBJ Sound[];
RECT BackRect;

char* midi[]={"MUSIC//menu.mid","MUSIC//beast.mid","MUSIC//monster.mid","MUSIC//devil.mid",
			"MUSIC//earlking.mid","MUSIC//victory.mid","MUSIC//die.mid"};
//char *snr1[]={
// 	};


BOOL _MidiPlay( char* pszMidiFN, BOOL bReplayFlag = true )
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
		for(i=1;i<=130;i++)
		{
			SetR(&BackRect,0,0,100,600);
			_DrawBmp(BackRect,i*8-100,0, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			_Delay(1);
			_CopyScreen(false);
		}
	}
	else if(to==100)
	{
		for(i=1;i<=130;i++)
		{
			SetR(&BackRect,0,0,100,100);
			_DrawBmp(BackRect,i*8-100,500, BmpScreen[9], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			_Delay(1);
			_CopyScreen(false);
		}
	}
	else
	{
		char* fname[]={"DATA//main.bmp","DATA//end1.bmp","DATA//end2.bmp"};
		BmpScreen[0] = DDLoadBitmap( DirectOBJ, fname[to], 0, 0, SYSTEM);
		for(i=1;i<=101;i++){
			SetRect(&BackRect,400-i*4,300-i*3, 400+i*4,300+i*3);
			_DrawBmp(BackRect,400-i*4,300-i*3, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			_CopyScreen(false);
		}
	}
}

