#include <string.h>
#include <tchar.h>

#include "donglib.h"

using namespace Gdiplus;

JDirectDraw* jdd;
JResourceManager* jre;

char* backbuffer;

JFont global_font;

HWND hwnd;
MSG msg;
BOOL activate;
BOOL gameover;

//»ç¿îµå
LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

BOOL SoundCard;
BOOL ReplayFlag;

BOOL mouse_control;	//¸¶¿ì½º »ç¿ë ¿©ºÎ
BOOL keyboard_control;	//Å°º¸µå »ç¿ë ¿©ºÎ

int MouseX, MouseY;	//¸¶¿ì½º ÁÂÇ¥
BOOL LButton=FALSE, RButton=FALSE;	//¸¶¿ì½º Å¬¸¯¿©ºÎ
BOOL left_click=FALSE, right_click=FALSE;	//¸¶¿ì½º Å¬¸¯Àº ÇÑ¹ø¸¸ °Ë»çÇÏ±â À§ÇÑ º¯¼ö

//ÇÑ±Û Ã³¸®
CHan m_han;
static int is_eng=1;

//ÇÁ·¹ÀÓ ÃøÁ¤
int fps_n=0, fps_count=0;
DWORD dwStart = GetTickCount();

//¸Ş¼¼Áö Ã³¸®
BOOL ProcessMessage()
{
	if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	{
		if(!GetMessage(&msg,NULL,0,0)) return FALSE;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//ÇÁ·¹ÀÓ ÃøÁ¤
	/*++fps_count;
	if(GetTickCount()-dwStart>=1000)
	{
		dwStart = GetTickCount();
		fps_n=fps_count;
		fps_count=0;
	}*/

	if(gameover)return FALSE;	//°­Á¦ Á¾·á

	return TRUE;
}

int GetFPS()
{
	return fps_n;
}

//¹Ìµğ ¿¬ÁÖ
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
    if ( SoundCard ) SndObjPlay( sound, NULL );
}

//µ¿¿µ»ó Àç»ı
DWORD AviCount;

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
            (DWORD)(MCI_DGV_OPEN_PARENT|MCI_DGV_OPEN_WS|MCI_OPEN_TYPE|MCI_OPEN_ELEMENT),
            (DWORD)(LPVOID)&mciOpen );

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

	mciSendCommand( *wDeviceID, MCI_PUT, MCI_DGV_PUT_WINDOW | MCI_DGV_RECT, (DWORD)(LPVOID)&mciRect );

    mciStatus.dwItem = MCI_STATUS_LENGTH;
    mciSendCommand( *wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatus );
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

    return mciSendCommand(wDevID, MCI_PLAY, dwFlags, (DWORD)(LPVOID)&mciPlay);
} 

DWORD CloseMovie( WORD wDevID )
{
    return mciSendCommand(wDevID, MCI_CLOSE, 0, NULL);
}

DWORD _PlayAVI( char* name )
{
    MCIDEVICEID mcidec;
    DSBUFFERDESC DSB_desc;
    MCI_STATUS_PARMS mciStatus;
    FILE *file;

    file = fopen( name, "rb" ); if ( file == NULL ) return FALSE;
    fclose( file );

    MciOpen( hwnd, &mcidec, name );
    PlayMovie( mcidec, 0, 0 );

    mciStatus.dwItem = MCI_STATUS_POSITION;

    while ( !_GetKeyState(VK_ESCAPE) )
    {
        mciSendCommand( mcidec, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatus );
        if ( mciStatus.dwReturn == AviCount ) break;
    }
    CloseMovie( mcidec );

    return TRUE;
}

//ÃÖ´ë ÃÖ¼Ò°ª
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

//Å° Ã³¸®
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

//¸¶¿ì½º ¹öÆ°
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

//ÆùÆ®°¡ '±Ã¼­', Å©±â20ÀÏ ¶§(global_font), °¢ Ä³¸¯ÅÍµéÀÇ ±æÀÌ¸¦ µ¹·ÁÁÖ´Â ÇÔ¼ö
//(±Ã¼­°¡ ¾Æ´Ï¸é ÀÚµ¿ ÁÙ¹Ù²ŞÀÌ Á¦´ë·Î ÀÛµ¿ ¾È ÇÒ ¼ö ÀÖÀ½
//±Û²Ã¸¶´Ù ±æÀÌ°¡ Æ²¸®´Ï ¾îÂ¿ ¼ö ¾øÀÌ ±Ã¼­¸¦ Ç¥ÁØÀ¸·Î ÇßÀ½
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

//±Ù»ç°ª, ¼­ÆäÀÌ½º¸¦ ¸¸µé¾î Ãâ·ÂÇÏ´Â ¹æ½ÄÀÌ¶ó¸é ¾µ »ı°¢ÀÌ¾úÀ½
// int GetCharLength(char tmp_char)
// {
// 	if(tmp_char>=0x41 && tmp_char<=0x5a)return 15;
// 	if(tmp_char=='~')return 16;
// 
// 	return 10;
// }

//¹®ÀÚ¿­ Ã³¸® ÇÔ¼ö
char str_buffer[3][1024];	//¹öÆÛ

char* StrAdd(char* msg, ...)
{
	static int buffer_id;		//Áö±İ »ç¿ëÇÏ´Â ¹öÆÛ(3°³ÀÇ ¹öÆÛ¸¦ µ¹¾Æ°¡¸é¼­ ½á¼­ µ¿½Ã 3°³Ã³¸® °¡´ÉÇÏ°Ô ÇÔ)
	if(++buffer_id>=3)buffer_id=0;
	char* use_buffer=str_buffer[buffer_id];
	size_t buffer_pointer=0;	//ÇöÀç ¹öÆÛ¿¡ ¾²°í ÀÖ´Â À§Ä¡

	strcpy(use_buffer,"");
	va_list ap;
	va_start(ap,msg);
	size_t max=strlen(msg);
	for(size_t i=0; i<max; ++i)
	{
		//º¯¼ö Áı¾î ³Ö±â
		if(msg[i]=='%' && i<max-1)
		{
			//¹®ÀÚ
			if(msg[i+1]=='s')
			{
				use_buffer[buffer_pointer]=NULL;
				strcat(use_buffer,va_arg(ap,char*));
				buffer_pointer=strlen(use_buffer);
				++i;
			}
			//¼ıÀÚ
			else if(msg[i+1]=='d')
			{
				use_buffer[buffer_pointer]=NULL;
				char int_buffer[10];
				_itoa(va_arg(ap,int),int_buffer,10);
				strcat(use_buffer,int_buffer);
				buffer_pointer=strlen(use_buffer);
				++i;
			}
		}
		//±×³É º¹»ç
		else
		{
			use_buffer[buffer_pointer]=msg[i];			
			++buffer_pointer;
		}
	}
	use_buffer[buffer_pointer]=NULL;

	return use_buffer;
}

//Àá½Ã Á¤Áö
void Pause()
{
	while(1)
	{
		if(!ProcessMessage())break;		//À©µµ¿ì ¸Ş¼¼Áö Ã³¸®
        //¸¶¿ì½º³ª Å°º¸µåÀÇ Esc³ª ¿£ÅÍ¸¦ ´©¸£¸é °è¼Ó ÁøÇà
		if(keyboard_control && (GetKey(vkey_esc) || GetKey(vkey_enter)) || mouse_control && LeftDown())break;
	}
}

//È­¸é Áö¿ì±â
void ClearScreen()
{
	ColorMatrix2 CM_bg={{{0,0,0,0},{0,0,0,0},{0,0,0,0}}};
	jdd->ApplyColorMatrix(backbuffer, backbuffer, 0, 0, NULL, CM_bg);
}

//ÆäÀÌµå ÀÎ/¾Æ¿ô
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

//ÆäÀÌµå ¾Æ¿ô ÈÄ ¹Ù·Î ÆäÀÌµåÀÎ ÇÒ ¼ö ¾ø½À´Ï´Ù.(BackBuffer°¡ °Ë°Ô Áö¿öÁ³±â ¶§¹®¿¡)
//´ÙÀ½ Àå¸éÀ» ±×¸° ÈÄ FadeInÀ» ÇØÁÖ¼¼¿ä. WhiteOutµµ ¸¶Âù°¡Áö
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

//È­ÀÌÆ® ÀÎ/¾Æ¿ô
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

//////////////////////////////////////////////////////////
//ÇÑ±Û Å¬·¡½º ¸Ş¼Òµå
#define HAN_MAX 2364
#define	MOUM	19

char* han1[]={"°¡","rk","°¢","rkr","A","rkR","°£","rks","°¤","rke","°¥","rkf","°¦","rkfr","°§","rkfa","E","rkfq","F","rkft",
			"G","rkfx","H","rkfv","I","rkfg","°¨","rka","°©","rkq","°ª","rkqt","°«","rkt","°¬","rkT","°­","rkd","°®","rkw",
			"°¯","rkc","°°","rkx","°±","rkv","°²","rkg","°³","ro","°´","ror","°µ","ros","°¶","roe","°·","roa","°¸","roq",
			"°¹","rot","°º","roT","°»","rod","°¼","ri","°½","rir","°¾","ris","°¿","rif","°À","rit","°Á","rid","°Â","rO",
			"°Ã","rOs","°Ä","rOf","°Å","rj","°Æ","rjr","°Ç","rjs","°È","rje","°É","rjf","°Ê","rjfa","°Ë","rja","°Ì","rjq",
			"°Í","rjt","°Î","rjT","°Ï","rjd","°Ğ","rjw","°Ñ","rjx","°Ò","rjv","°Ó","rjg","°Ô","rp","°Õ","rps","°Ö","rpf",
			"°×","rpa","°Ø","rpq","°Ù","rpt","°Ú","rpT","°Û","rpd","°Ü","ru","°İ","rur","°Ş","rur","°ß","rus","°à","rue",
			"°á","ruf","°â","rua","°ã","ruq","°ä","rut","°å","ruT","°æ","rud","°ç","rux","°è","rP","°é","rPs","°ê","rPf",
			"°ë","rPq","°ì","rPt","°í","rh","°î","rhr","°ï","rhs","°ğ","rhe","°ñ","rhf","°ò","rhfa","°ó","rhft","°ô","rhfg",
			"°õ","rha","°ö","rhq","°÷","rht","°ø","rhd","°ù","rhw","°ú","rhk","°û","rhkr","°ü","rhks","°ı","rhkf","°ş","rhkfa",
			"±¡","rhka","±¢","rhkq","±£","rhkt","±¤","rhkd","±¥","rho","±¦","rhos","±§","rhof","±¨","rhoq","±©","rhoT","±ª","rhod",
			"±«","rhl","±¬","rhlr","±­","rhls","±®","rhlf","±¯","rhla","±°","rhlq","±±","rhlt","±²","rhld","±³","ry","±´","rys",
			"±µ","ryf","±¶","ryq","±·","ryt","±¸","rn","±¹","rnr","±º","rns","±»","rne","±¼","rnf","±½","rnfr","±¾","rnfa",
			"±¿","rnfg","±À","rna","±Á","rnq","±Â","rnt","±Ã","rnd","±Ä","rnw","±Å","rnj","±Æ","rnjr","±Ç","rnjs","±È","rnjf",
			"±É","rnjT","±Ê","rnjd","±Ë","rnp","±Ì","rnpt","±Í","rnl","±Î","rnlr","±Ï","rnls","±Ğ","rnlf","±Ñ","rnla","±Ò","rnlq",
			"±Ó","rnlt","±Ô","rb","±Õ","rbs","±Ö","rbf","±×","rm","±Ø","rmr","±Ù","rms","±Ú","rme","±Û","rmf","±Ü","rmfr",
			"±İ","rma","±Ş","rmq","±ß","rmt","±à","rmd","±á","rml","±â","rl","±ã","rlr","±ä","rls","±å","rle","±æ","rlf",
			"±ç","rlfa","±è","rla","±é","rlq","±ê","rlt","±ë","rld","±ì","rlw","±í","rlv","±î","Rk","±ï","Rkr","±ğ","RkR",
			"±ñ","Rks","±ò","Rkf","±ó","Rkfa","±ô","Rka","±õ","Rkq","±ö","Rkt","±÷","RkT","±ø","Rkd","±ù","Rkx","±ú","Ro",
			"±û","Ror","±ü","Ros","±ı","Rof","±ş","Roa","²¡","Roq","²¢","Rot","²£","RoT","²¤","Rod","²¥","Ri","²¦","Rir",
			"²§","Rif","²¨","Rj","²©","Rjr","²ª","RjR","²«","Rjs","²¬","Rjf","²­","Rja","²®","Rjq","²¯","Rjt","²°","RjT",
			"²±","Rjd","²²","Rp","²³","Rpr","ƒô","RpR","²´","Rps","²µ","Rpa","²¶","Rpt","²·","Rpd","²¸","Ru","²¹","Rus",
			"²º","Ruf","²»","Rut","²¼","RuT","²½","Rux","²¾","RP","²¿","Rh","²À","Rhr","²Á","Rhs","²Ä","Rha","²Â","Rhsg",
			"²Ã","Rhf","²Ä","Rha","²Å","Rhq","²Æ","Rht","²Ç","Rhd","²È","Rhw","²É","Rhc","²Ê","Rhk","²Ë","Rhkr","²Ì","Rhkf",
			"²Í","RhkT","²Î","Rhkd","²Ï","Rho","²Ğ","Rhor","²Ñ","Rhod","²Ò","Rhl","²Ó","Rhls","²Ô","RHlf","²Õ","Rhla","²Ö","Rhlq",
			"²×","Rhld","²Ø","Ry","²Ù","Rn","²Ú","Rnr","²Û","Rns","²Ü","Rnf","²İ","Rnfg","²Ş","Rna","²ß","Rnq","²à","Rnt",
			"²á","Rnd","²â","Rnw","²ã","Rnj","²ä","Rnjf","²å","RnjT","²æ","Rnjd","²ç","Rnp","²è","Rnpr","²é","Rnps","²ê","Rnpf",
			"²ë","Rnpa","²ì","Rnpq","²í","RnpT","²î","Rnl","²ï","Rnls","²ğ","Rnlf","²ñ","Rnla","²ò","Rnlq","²ó","Rb","²ô","Rm",
			"²õ","Rmr","²ö","Rms","²÷","Rmsg","²ø","Rmf","²ù","Rmfa","²ú","Rmfg","²û","Rma","²ü","Rmq","²ı","Rmt","²ş","Rmd",
			"³¡","Rmx","³¢","Rl","³£","Rlr","³¤","Rls","³¥","Rlf","³¦","Rla","³§","Rlq","³¨","Rlt","³©","Rld","³ª","sk",
			"³«","skr","³¬","skR","³­","sks","³®","ske","³¯","skf","³°","skfr","³±","skfa","³²","ska","³³","skq","³´","skt",
			"³µ","skT","³¶","skd","³·","skw","³¸","skc","³¹","skx","³º","skg","³»","so","³¼","sor","³½","sos","³¾","sof",
			"³¿","soa","³À","soq","³Á","sot","³Â","soT","³Ã","sod","³Ä","si","³Å","sir","³Æ","sis","³Ç","sif","³È","sia",
			"³É","sid","³Ê","sj","³Ë","sjr","³Ì","sjt","³Í","sjs","³Î","sjf","³Ï","sjfa","³Ğ","sjfq","³Ñ","sja","³Ò","sjq",
			"³Ó","sjt","³Ô","sjT","³Õ","sjd","³Ö","sjg","³×","sp","³Ø","spr","³Ù","sps","³Ú","spf","³Û","spa","³Ü","spq",
			"³İ","spt","³Ş","spT","³ß","spd","³à","su","³á","sur","³â","sus","³ã","suf","³ä","sua","³å","suq","³æ","suT",
			"³ç","sud","³è","suz","³é","sP","³ê","sPs","³ë","sh","³ì","shr","³í","shs","³î","shf","³ï","shfa","³ğ","sha",
			"³ñ","shq","³ò","sht","³ó","shd","³ô","shv","³õ","shg","³ö","shk","³÷","shks","³ø","shkf","³ù","shkT","³ú","shl",
			"³û","shls","³ü","shlf","³ı","shla","³ş","shlq","´¡","shlt","´¢","sy","´£","syr","´¤","sys","´¥","syf","´¦","syq",
			"´§","syt","´¨","syd","´©","sn","´ª","snr","´«","sns","´¬","sne","´­","snf","´®","sna","´¯","snq","´°","snt",
			"´±","snd","´²","snj","´³","snjT","´´","snp","´µ","snl","´¶","snls","´·","snlf","´¸","snla","´¹","snlq","´º","sb",
			"´»","sbr","´¼","sbf","´½","sba","´¾","sbq","´¿","sbd","´À","sm","´Á","smr","´Â","sms","´Ã","smf","´Ä","smfr",
			"´Å","smfa","´Æ","sma","´Ç","smq","´È","smt","´É","smd","´Ê","smw","´Ë","smv","´Ì","sml","´Í","smls","´Î","smlf",
			"´Ï","sl","´Ğ","slr","´Ñ","sls","´Ò","slf","´Ó","slfa","´Ô","sla","´Õ","slq","´Ö","slt","´×","sld","´Ø","slv",
			"´Ù","ek","´Ú","ekr","´Û","ekR","´Ü","eks","´İ","eke","´Ş","ekf","´ß","ekfr","´à","ekfa","´á","ekfq","´â","ekfg",
			"´ã","eka","´ä","ekq","´å","ekt","´æ","ekT","´ç","ekd","´è","ekw","´é","ekc","´ê","ekg","´ë","eo","´ì","eor",
			"´í","eos","´î","eof","´ï","eoa","´ğ","eoq","´ñ","eot","´ò","eoT","´ó","eod","´ô","ei","´õ","ej","´ö","ejr",
			"´÷","ejR","´ø","ejs","´ù","eje","´ú","ejf","´û","ejfa","´ü","ejfq","´ı","eja","´ş","ejq","µ¡","ejt","µ¢","ejd",
			"µ£","ejc","µ¤","ejv","µ¥","ep","µ¦","epr","µ§","eps","µ¨","epf","µ©","epa","µª","epq","µ«","ept","µ¬","epT",
			"µ­","epd","µ®","eu","µ¯","eus","µ°","euf","µ±","euT","µ²","eud","µ³","eP","µ´","ePs","µµ","eh","µ¶","ehr",
			"µ·","ehs","µ¸","ehe","µ¹","ehf","µº","ehfa","µ»","ehft","µ¼","eha","µ½","ehq","µ¾","eht","µ¿","ehd","µÀ","ehc",
			"µÁ","ehx","µÂ","ehk","µÃ","ehks","µÄ","ehkf","µÅ","eho","µÆ","ehoT","µÇ","ehl","µÈ","ehls","µÉ","ehlf","µÊ","ehla",
			"µË","ehlq","µÌ","ehlt","µÍ","ey","µÎ","en","µÏ","enr","µĞ","ens","µÑ","enf","µÒ","ena","µÓ","enq","µÔ","ent",
			"µÕ","end","µÖ","enj","µ×","enjT","µØ","enp","µÙ","enpd","µÚ","enl","µÛ","enls","µÜ","enlf","µİ","enlq","µŞ","enlt",
			"µß","enld","µà","eb","µá","ens","µâ","enf","µã","ena","µä","end","µå","em","µæ","emr","µç","ems","µè","eme",
			"µé","emf","µê","emfa","µë","ema","µì","emq","µí","emt","µî","emd","µï","eml","µğ","el","µñ","elr","µò","els",
			"µó","ele","µô","elf","µõ","ela","µö","elq","µ÷","elt","µø","elT","µù","eld","µú","elw","µû","Ek","µü","Ekr",
			"µı","Eks","µş","Ekf","¶¡","Eka","¶¢","Ekq","¶£","Ekt","¶¤","EkT","¶¥","Ekd","¶¦","Ekg","¶§","Eo","¶¨","Eor",
			"¶©","Eos","¶ª","Eof","¶«","Eoa","¶¬","Eoq","¶­","Eot","¶®","EoT","¶¯","Eod","¶°","Ej","¶±","Ejr","¶²","Ejs",
			"¶³","Ejf","¶´","Ejfa","¶µ","Ejfq","¶¶","Eja","¶·","Ejq","¶¸","Ejt","¶¹","EjT","¶º","Ejd","¶»","Ejg","¶¼","Ep",
			"¶½","Epr","¶¾","Eps","¶¿","Epf","¶À","Epa","¶Á","Epq","¶Â","Ept","¶Ã","EpT","¶Ä","Epd","¶Å","Eu","¶Æ","EuT",
			"¶Ç","Eh","¶È","Ehr","¶É","Ehs","¶Ê","Ehf","¶Ë","Ehd","¶Ì","Ehk","¶Í","Ehkf","¶Î","Eho","¶Ï","Ehl","¶Ğ","Ehls",
			"¶Ñ","En","¶Ò","Enr","¶Ó","Ens","¶Ô","Enf","¶Õ","Enfg","¶Ö","Ena","¶×","End","¶Ø","Enp","¶Ù","Enl","¶Ú","Enls",
			"¶Û","Enlf","¶Ü","Enla","¶İ","Enlq","¶Ş","Enld","¶ß","Em","¶à","Emr","¶á","Ems","¶â","Eme","¶ã","Emf","¶ä","Ema",
			"¶å","Emq","¶æ","Emt","¶ç","Eml","¶è","Emls","¶é","Emlf","¶ê","Emla","¶ë","Emlq","¶ì","El","¶í","Els","¶î","Elf",
			"¶ï","Ela","¶ğ","Elq","¶ñ","Elt","¶ò","Eld","¶ó","fk","¶ô","fkr","¶õ","fks","¶ö","fkf","¶÷","fka","¶ø","fkq",
			"¶ù","fkt","¶ú","fkT","¶û","fkd","¶ü","fkw","¶ı","fkv","¶ş","fkg","·¡","fo","·¢","for","·£","fos","·¤","fof",
			"·¥","foa","·¦","foq","·§","fot","·¨","foT","·©","fod","·ª","fi","·«","fir","·¬","fis","·­","fit","·®","fid",
			"·¯","fj","·°","fjr","·±","fjs","·²","fjf","·³","fja","·´","fjq","·µ","fjt","·¶","fjT","··","fjd","·¸","fjg",
			"·¹","fp","·º","fpr","·»","fps","·¼","fpf","·½","fpa","·¾","fpq","·¿","fpt","·À","fpd","·Á","fu","·Â","fur",
			"·Ã","fus","·Ä","fuf","·Å","fua","·Æ","fuq","·Ç","fut","·È","fuT","·É","fud","·Ê","fP","·Ë","fPs","·Ì","fPq",
			"·Í","fPt","·Î","fh","·Ï","fhr","·Ğ","fhs","·Ñ","fhf","·Ò","fha","·Ó","fhq","·Ô","fht","·Õ","fhd","·Ö","fhk",
			"·×","fhks","·Ø","fhkd","·Ù","fhoT","·Ú","fhl","·Û","fhls","·Ü","fhlf","·İ","fhla","·Ş","fhlq","·ß","fhlt","·à","fhld",
			"·á","fy","·â","fys","·ã","fyf","·ä","fyq","·å","fyt","·æ","fyd","·ç","fn","·è","fnr","·é","fns","·ê","fnf",
			"·ë","fna","·ì","fnq","·í","fnt","·î","fnd","·ï","fnj","·ğ","fnjT","·ñ","fnp","·ò","fnl","·ó","fnlr","·ô","fnls",
			"·õ","fnlf","·ö","fnla","·÷","fnlt","·ø","fnld","·ù","fb","·ú","fbr","·û","fbs","·ü","fbf","·ı","fba","·ş","fbq",
			"¸¡","fbt","¸¢","fbd","¸£","fm","¸¤","fmr","¸¥","fms","¸¦","fmf","¸§","fma","¸¨","fmq","¸©","fmt","¸ª","fmd",
			"¸«","fmw","¸¬","fmx","¸­","fmv","¸®","fl","¸¯","flr","¸°","fls","¸±","flf","¸²","fla","¸³","flq","¸´","flt",
			"¸µ","fld","¸¶","ak","¸·","akr","¸¸","aks","¸¹","aksg","¸º","ake","¸»","akf","¸¼","akfr","¸½","aka","¸¾","aka",
			"¸¿","akq","¸À","akt","¸Á","akd","¸Â","akw","¸Ã","akx","¸Ä","akg","¸Å","ao","¸Æ","aor","¸Ç","aos","¸È","aof","¸É","aoa",
			"¸Ê","aoq","¸Ë","aot","¸Ì","aoT","¸Í","aod","¸Î","aow","¸Ï","ai","¸Ğ","air","¸Ñ","aif","¸Ò","aid","¸Ó","aj","¸Ô","ajr",
			"¸Õ","ajs","¸Ö","ajf","¸×","ajfa","¸Ø","aja","¸Ù","ajq","¸Ú","ajt","¸Û","ajd","¸Ü","ajw","¸İ","ajg","¸Ş","ap","¸ß","apr",
			"¸à","aps","¸á","apf","¸â","apa","¸ã","apq","¸ä","apt","¸å","apT","¸æ","apd","¸ç","au","¸è","aur","¸é","aus",
			"¸ê","auf","¸ë","aut","¸ì","auT","¸í","aud","¸î","auc","¸ï","aP","¸ğ","ah","¸ñ","ahr","¸ò","ahrt","¸ó","ahs","¸ô","ahf",
			"¸õ","ahfa","¸ö","aha","¸÷","ahq","¸ø","aht","¸ù","ahd","¸ú","ahk","¸û","ahks","¸ü","ahkT","¸ı","ahkd","¸ş","ahl","¹¡","ahls",
			"¹¢","ahlf","¹£","ahlq","¹¤","ahlt","¹¥","ahld","¹¦","ay","¹§","ays","¹¨","ayf","¹©","ayq","¹ª","ayt","¹«","an","¹¬","anr",
			"¹­","anR","¹®","ans","¹¯","ane","¹°","anf","¹±","anfr","¹²","anfa","¹³","ana","¹´","anq","¹µ","ant","¹¶","and",
			"¹·","anx","¹¸","ang","¹¹","anj","¹º","anjs","¹»","anjf","¹¼","anjq","¹½","anjt","¹¾","anp","¹¿","anl","¹À","anls","¹Á","anlf",
			"¹Â","ab","¹Ã","abs","¹Ä","abf","¹Å","aba","¹Æ","abt","¹Ç","am","¹È","ams","¹É","amf","¹Ê","ama","¹Ë","amt","¹Ì","al",
			"¹Í","alr","¹Î","als","¹Ï","ale","¹Ğ","alf","¹Ñ","alfa","¹Ò","ala","¹Ó","alq","¹Ô","alt","¹Õ","alT","¹Ö","ald","¹×","alC",
			"¹Ø","alx","¹Ù","qk","¹Ú","qkr","¹Û","qkR","¹Ü","qkrt","¹İ","qks","¹Ş","qke","¹ß","qkf","¹à","qkfr","¹á","qkfa",
			"¹â","qkfq","¹ã","qka","¹ä","qkq","¹å","qkt","¹æ","qkd","¹ç","qkx","¹è","qo","¹é","qor","¹ê","qos","¹ë","qof","¹ì","qoa",
			"¹í","qoq","¹î","qot","¹ï","qoT","¹ğ","qod","¹ñ","qox","¹ò","qi","¹ó","qir","¹ô","qis","¹õ","qiq","¹ö","qj","¹÷","qjr",
			"¹ø","qjs","¹ù","qje","¹ú","qjf","¹û","qjfa","¹ü","qja","¹ı","qjq","¹ş","qjt","º¡","qjd","º¢","qjw","º£","qp","º¤","qpr",
			"º¥","qps","º¦","qpe","º§","qpf","º¨","qpa","º©","qpq","ºª","qpt","º«","qpT","º¬","qpd","º­","qu","º®","qur",
			"º¯","qus","º°","quf","º±","quq","º²","qut","º³","quT","º´","qud","ºµ","qux","º¶","qP","º·","qPs","º¸","qh","º¹","qhr",
			"ºº","qhR","º»","qhs","º¼","qhf","º½","qha","º¾","qhq","º¿","qht","ºÀ","qhd","ºÁ","qhk","ºÂ","qhks","ºÃ","qhkT","ºÄ","qho",
			"ºÅ","qhoT","ºÆ","qhl","ºÇ","qhlr","ºÈ","qhls","ºÉ","qhlf","ºÊ","qhla","ºË","qhlq","ºÌ","qy","ºÍ","qys","ºÎ","qn","ºÏ","qnr",
			"ºĞ","qns","ºÑ","qne","ºÒ","qnf","ºÓ","qnfr","ºÔ","qnfa","ºÕ","qna","ºÖ","qnq","º×","qnt","ºØ","qnd","ºÙ","qnx",
			"ºÚ","qnv","ºÛ","qnj","ºÜ","qnjf","ºİ","qnjT","ºŞ","qnp","”í","qnpf","”î","qnpfr","ºß","qnl","ºà","qnlr","ºá","qnls","ºâ","qnlf","ºã","qnld","ºä","qb",
			"ºå","qbs","ºæ","qbf","ºç","qba","ºè","qbt","ºé","qbd","ºê","qm","ºë","qmr","ºì","qms","ºí","qmf","ºî","qma","ºï","qmq",
			"ºğ","qmt","ºñ","ql","ºò","qlr","ºó","qls","ºô","qlf","ºõ","qlfa","ºö","qla","º÷","qlq","ºø","qlt","ºù","qld","ºú","qlw",
			"ºû","qlc","ºü","Qk","ºı","Qkr","ºş","Qks","»¡","Qkf","»¢","Qkfa","»£","Qka","»¤","Qkq","»¥","Qkt","»¦","QkT",
			"»§","Qkd","»¨","Qkg","»©","Qo","»ª","Qor","»«","Qos","»¬","Qof","»­","Qoa","»®","Qoq","»¯","Qot","»°","QoT","»±","Qod",
			"»²","Qi","»³","Qir","»´","Qia","»µ","Qj","»¶","Qjr","»·","Qjs","»¸","Qje","»¹","Qjf","»º","Qja","»»","Qjt","»¼","QjT",
			"»½","Qjd","»¾","Qp","»¿","Qpd","»À","Qu","»Á","Qur","»Â","Qua","»Ã","Quq","»Ä","Qut","»Å","QuT","»Æ","Qud","»Ç","Qh",
			"»È","Qhr","»É","Qhs","»Ê","Qhf","»Ë","Qha","»Ì","Qhq","»Í","Qhd","»Î","Qhl","»Ï","Qy","»Ğ","Qyd","»Ñ","Qn",
			"»Ò","Qnr","»Ó","Qns","»Ô","Qnf","»Õ","Qna","»Ö","Qnt","»×","Qnd","»Ø","Qb","»Ù","Qbd","»Ú","Qm","»Û","Qms","»Ü","Qmf",
			"»İ","Qma","»Ş","Qmq","»ß","Ql","»à","Qlr","»á","Qls","»â","Qlf","»ã","Qla","»ä","Qlq","»å","Qlt","»æ","Qld","»ç","tk",
			"»è","tkr","»é","tkrt","»ê","tks","»ë","tke","»ì","tkf","»í","tkfr","»î","tkfa","»ï","tka","»ğ","tkq","»ñ","tkt","»ò","tkT",
			"»ó","tkd","»ô","tkx","»õ","to","»ö","tor","»÷","tos","»ø","tof","»ù","toa","»ú","toq","»û","tot","»ü","toT",
			"»ı","tod","»ş","ti","¼¡","tir","¼¢","tis","¼£","tif","¼¤","tia","¼¥","tiq","¼¦","tit","¼§","tid","¼¨","tO",
			"¼©","tOs","¼ª","tOf","¼«","tOa","¼¬","tOd","¼­","tj","¼®","tjr","¼¯","tjR","¼°","tjrt","¼±","tjs","¼²","tje",
			"¼³","tjf","¼´","tjfa","¼µ","tjfq","¼¶","tja","¼·","tjq","¼¸","tjt","¼¹","tjT","¼º","tjd","¼»","tjv","¼¼","tp",
			"¼½","tpR","¼¾","tps","¼¿","tpf","¼À","tpa","¼Á","tpq","¼Â","tpt","¼Ã","tpT","¼Ä","tpd","¼Å","tu","¼Æ","tur",
			"¼Ç","tus","¼È","tuf","¼É","tua","¼Ê","tuq","¼Ë","tut","¼Ì","tuT","¼Í","tud","¼Î","tP","¼Ï","tPs","¼Ğ","tPf",
			"¼Ñ","tPd","¼Ò","th","¼Ó","thr","¼Ô","thR","¼Õ","ths","¼Ö","thf","¼×","thfa","¼Ø","tha","¼Ù","thq","¼Ú","tht",
			"¼Û","thd","¼Ü","thx","¼İ","thk","¼Ş","thkr","¼ß","thks","¼à","thkf","¼á","thkd","¼â","tho","¼ã","thos","¼ä","thof",
			"¼å","thoa","¼æ","thot","¼ç","thoT","¼è","thl","¼é","thls","¼ê","thlf","¼ë","thla","¼ì","thlq","¼í","thlt","¼î","ty",
			"¼ï","tyr","¼ğ","tys","¼ñ","tyf","¼ò","tya","¼ó","tyq","¼ô","tyt","¼õ","tyd","¼ö","tn","¼÷","tnr","¼ø","tns",
			"¼ù","tne","¼ú","tnf","¼û","tna","¼ü","tnq","¼ı","tnt","¼ş","tnd","½¡","tnc","½¢","tnx","½£","tnv","½¤","tnj",
			"½¥","tnjT","½¦","tnp","½§","tnpr","½¨","tnps","½©","tnpf","½ª","tnpa","½«","tnpd","½¬","tnl","½­","tnlr","½®","tnls",
			"½¯","tnlf","½°","tnla","½±","tnlq","½²","tnlt","½³","tnld","½´","tb","½µ","tbr","½¶","tbf","½·","tba","½¸","tbt",
			"½¹","tbd","½º","tm","½»","tmr","½¼","tms","½½","tmf","½¾","tmfr","½¿","tma","½À","tmq","½Á","tmt","½Â","tmd",
			"½Ã","tl","½Ä","tlr","½Å","tls","½Æ","tle","½Ç","tlf","½È","tlfg","½É","tla","½Ê","tlq","½Ë","tlt","½Ì","tld",
			"½Í","tlv","½Î","Tk","½Ï","Tkr","½Ğ","Tkrt","½Ñ","Tks","½Ò","Tkf","½Ó","Tka","½Ô","Tkq","½Õ","TkT","½Ö","Tkd",
			"½×","Tkg","½Ø","To","½Ù","Tor","½Ú","Tos","½Û","Tof","½Ü","Toa","½İ","Toq","½Ş","ToT","½ß","Tod","½à","Tid",
			"½á","Tj","½â","Tjr","½ã","Tjs","½ä","Tjf","½å","Tjfa","½æ","Tja","½ç","Tjq","½è","TjT","½é","Tjd","½ê","Tp",
			"›®","Tpr","½ë","Tps","½ì","Tpf","½í","TPs","½î","Th","½ï","Thr","½ğ","Ths","½ñ","The","½ò","Thf","½ó","Thfa",
			"½ô","Tha","½õ","Thq","½ö","Thd","½÷","Thk","½ø","Thkr","½ù","Thks","½ú","ThkT","½û","Tho","½ü","ThoT","½ı","Thl",
			"½ş","Thls","¾¡","Thlf","¾¢","Thla","¾£","Thlq","¾¤","Ty","¾¥","Tn","¾¦","Tnr","¾§","Tns","¾¨","Tnf","¾©","Tna",
			"¾ª","Tnq","¾«","Tnd","¾¬","Tnj","¾­","TnT","¾®","Tnp","¾¯","Tnl","¾°","Tnls","¾±","Tbd","¾²","Tm","¾³","Tmr",
			"¾´","Tms","¾µ","Tmf","¾¶","Tmfa","¾·","Tmfg","¾¸","Tma","¾¹","Tmq","¾º","Tml","¾»","Tmls","¾¼","Tmlf","¾½","Tmla",
			"¾¾","Tl","¾¿","Tlr","¾À","Tls","¾Á","Tlf","¾Â","Tla","¾Ã","Tlq","¾Ä","Tlt","¾Å","Tld","¾Æ","dk","¾Ç","dkr",
			"¾È","dks","¾É","dksw","¾Ê","dksg","¾Ë","dkf","¾Ì","dkfr","¾Í","dkfa","¾Î","dkfg","¾Ï","dka","¾Ğ","dkq","¾Ñ","dkt",
			"¾Ò","dkT","¾Ó","dkd","¾Ô","dkx","¾Õ","dkv","¾Ö","do","¾×","dor","¾Ø","dos","¾Ù","dof","¾Ú","doa","¾Û","doq",
			"¾Ü","dot","¾İ","doT","¾Ş","dod","¾ß","di","¾à","dir","¾á","dis","¾â","dif","¾ã","difq","¾ä","dia","¾å","diq",
			"¾æ","dit","¾ç","did","¾è","dix","¾é","dig","¾ê","dO","¾ë","dOs","¾ì","dOf","¾í","dOq","¾î","dj","¾ï","djr",
			"¾ğ","djs","¾ñ","djsw","¾ò","dje","¾ó","djf","¾ô","djfr","¾õ","djfa","¾ö","dja","¾÷","djq","¾ø","djqt","¾ù","djt",
			"¾ú","djT","¾û","djd","¾ü","djw","¾ı","djz","¾ş","djv","¿¡","dp","¿¢","dpr","¿£","dps","¿¤","dpf","¿¥","dpa",
			"¿¦","dpq","¿§","dpt","¿¨","dpd","¿©","du","¿ª","dur","¿«","duR","¿¬","dus","¿­","duf","¿®","dufa","¿¯","dufq",
			"¿°","dua","¿±","duq","¿²","duqt","¿³","dut","¿´","duT","¿µ","dud","¿¶","dux","¿·","duv","¿¸","dug","¿¹","dP",
			"¿º","dPs","¿»","dPf","¿¼","dPa","¿½","dPq","¿¾","dPt","¿¿","dPT","¿À","dh","¿Á","dhr","¿Â","dhs","¿Ã","dhf",
			"¿Ä","dhfr","¿Å","dhfa","¿Æ","dhft","¿Ç","dhfg","¿È","dha","¿É","dhq","¿Ê","dht","¿Ë","dhd","¿Ì","dhc","¿Í","dhk",
			"¿Î","dhkr","¿Ï","dhks","¿Ğ","dhkf","¿Ñ","dhka","¿Ò","dhkq","¿Ó","dhkt","¿Ô","dhkT","¿Õ","dhkd","¿Ö","dho","¿×","dhor",
			"¿Ø","dhos","¿Ù","dhoa","¿Ú","dhot","¿Û","dhod","¿Ü","dhl","¿İ","dhlr","¿Ş","dhls","¿ß","dhlf","¿à","dhla","¿á","dhlq",
			"¿â","dhlt","¿ã","dhld","¿ä","dy","¿å","dyr","¿æ","dys","¿ç","dyf","¿è","dya","¿é","dyq","¿ê","dyr","¿ë","dyd",
			"¿ì","dn","¿í","dnr","¿î","dns","¿ï","dnf","¿ğ","dnfr","¿ñ","dnfa","¿ò","dna","¿ó","dnq","¿ô","dnt","¿õ","dnd",
			"¿ö","dnj","¿÷","dnjr","¿ø","dnjs","¿ù","dnjf","¿ú","dnja","¿û","dnjq","¿ü","dnjT","¿ı","dnjd","¿ş","dnp","À¡","dnpr",
			"À¢","dnps","À£","dnpf","À¤","dnpa","À¥","dnpq","À¦","dnpd","À§","dnl","À¨","dnlr","À©","dnls","Àª","dnlf","À«","dnla",
			"À¬","dnlq","À­","dnlt","À®","dnld","À¯","db","À°","dbr","À±","dbs","À²","dbf","À³","dba","À´","dbq","Àµ","dbt",
			"À¶","dbd","À·","dbc","À¸","dm","À¹","dmr","Àº","dms","À»","dmf","À¼","dmfv","À½","dma","À¾","dnq","À¿","dmt",
			"ÀÀ","dmd","ÀÁ","dmw","ÀÂ","dmc","ÀÃ","dmz","ÀÄ","dmx","ÀÅ","dmv","ÀÆ","dmg","ÀÇ","dml","ÀÈ","dmls","ÀÉ","dmlf",
			"ÀÊ","dmla","ÀË","dmlt","ÀÌ","dl","ÀÍ","dlr","ÀÎ","dls","ÀÏ","dlf","ÀĞ","dlfr","ÀÑ","dlfa","ÀÒ","dlfg","ÀÓ","dla",
			"ÀÔ","dlq","ÀÕ","dlt","ÀÖ","dlT","À×","dld","ÀØ","dlw","ÀÙ","dlv","ÀÚ","wk","ÀÛ","wkr","ÀÜ","wks","Àİ","wksg",
			"ÀŞ","wke","Àß","wkf","Àà","wkfa","Àá","wka","Àâ","wkq","Àã","wkt","Àä","wkT","Àå","wkd","Àæ","wkw","Àç","wo",
			"Àè","wor","Àé","wos","Àê","wof","Àë","woa","Àì","woq","Àí","wot","Àî","woT","Àï","wod","Àğ","wi","Àñ","wir",
			"Àò","wis","Àó","wisg","Àô","wif","Àõ","wia","Àö","wid","À÷","wO","Àø","wOs","Àù","wOf","Àú","wj","Àû","wjr",
			"Àü","wjs","Àı","wjf","Àş","wjfa","Á¡","wja","Á¢","wjq","Á£","wjt","Á¤","wjd","Á¥","wjw","Á¦","wp","Á§","wpr",
			"Á¨","wps","Á©","wpf","Áª","wpa","Á«","wpq","Á¬","wpt","Á­","wpd","Á®","wu","Á¯","wus","Á°","wuf","Á±","wua",
			"Á²","wuq","Á³","wuT","Á´","wud","Áµ","wP","Á¶","wh","Á·","whr","Á¸","whs","Á¹","whf","Áº","whfa","Á»","wha",
			"Á¼","whq","Á½","wht","Á¾","whd","Á¿","whw","ÁÀ","whc","ÁÁ","whg","ÁÂ","whk","ÁÃ","whkr","ÁÄ","whkf","ÁÅ","whkq",
			"ÁÆ","whkt","ÁÇ","whkd","ÁÈ","who","ÁÉ","whoT","ÁÊ","whod","ÁË","whl","ÁÌ","whls","ÁÍ","whlf","ÁÎ","whla","ÁÏ","whlq",
			"ÁĞ","whlt","ÁÑ","whld","ÁÒ","wy","ÁÓ","wyr","ÁÔ","wys","ÁÕ","wyd","ÁÖ","wn","Á×","wnr","ÁØ","wns","ÁÙ","wnf",
			"ÁÚ","wnfr","ÁÛ","wnfa","ÁÜ","wna","Áİ","wnq","ÁŞ","wnt","Áß","wnd","Áà","wnj","Áá","wnjT","Áâ","wnp","Áã","wnl",
			"Áä","wnlr","Áå","wnls","Áæ","wnlf","Áç","wnla","Áè","wnlq","Áé","wnlt","Áê","wb","Áë","wbs","Áì","wbf","Áí","wba",
			"Áî","wm","Áï","wmr","Áğ","wms","Áñ","wmf","Áò","wma","Áó","wmq","Áô","wmt","Áõ","wmd","Áö","wl","Á÷","wlr",
			"Áø","wls","Áù","wle","Áú","wlf","Áû","wlfa","Áü","wla","Áı","wlq","Áş","wlt","Â¡","wld","Â¢","wlw","Â£","wlx",
			"Â¤","wlv","Â¥","Wk","Â¦","Wkr","Â§","Wks","Â¨","Wksg","Â©","Wkf","Âª","Wkfq","Â«","Wka","Â¬","Wkq","Â­","Wkt",
			"Â®","WkT","Â¯","Wkd","Â°","Wo","Â±","Wor","Â²","Wos","Â³","Wof","Â´","Woa","Âµ","Woq","Â¶","Wot","Â·","WoT",
			"Â¸","Wod","Â¹","Wi","Âº","Wis","Â»","Wid","Â¼","Wj","Â½","Wjr","Â¾","Wjs","Â¿","Wjf","ÂÀ","Wja","ÂÁ","Wjq",
			"ÂÂ","Wjt","ÂÃ","WjT","ÂÄ","Wjd","ÂÅ","Wp","ÂÆ","Wpd","ÂÇ","Wu","ÂÈ","WuT","ÂÉ","Wh","ÂÊ","Whr","ÂË","Whs",
			"ÂÌ","Whf","ÂÍ","Wha","ÂÎ","Whq","ÂÏ","Wht","ÂĞ","Whd","ÂÑ","Whc","ÂÒ","Whk","ÂÓ","Whkr","ÂÔ","Whkf","ÂÕ","WhkT",
			"ÂÖ","Who","Â×","WhoT","ÂØ","Whl","ÂÙ","Whls","ÂÚ","Whlf","ÂÛ","Whla","ÂÜ","Whlq","Âİ","Wyd","ÂŞ","Wn","Âß","Wnr",
			"Âà","Wns","Âá","Wnf","Ââ","Wna","Âã","Wnq","Âä","Wnd","Âå","Wnj","Âæ","WnjT","Âç","Wnjd","Âè","Wnl","Âé","Wb",
			"Âê","Wm","Âë","Wma","Âì","Wmt","Âí","Wmd","Âî","Wl","Âï","Wlr","Âğ","Wls","Âñ","Wlf","Âò","Wla","Âó","Wlq",
			"Âô","Wld","Âõ","Wlw","Âö","Wlg","Â÷","ck","Âø","ckr","Âù","cks","Âú","cksg","Âû","ckf","Âü","cka","Âı","ckq",
			"Âş","ckt","Ã¡","ckT","Ã¢","ckd","Ã£","ckw","Ã¤","co","Ã¥","cor","Ã¦","cos","Ã§","cof","Ã¨","coa","Ã©","coq",
			"Ãª","cot","Ã«","coT","Ã¬","cod","Ã­","ci","Ã®","cis","Ã¯","cisg","Ã°","cif","Ã±","cia","Ã²","cid","Ã³","cj",
			"Ã´","cjr","Ãµ","cjs","Ã¶","cjf","Ã·","cja","Ã¸","cjq","Ã¹","cjt","Ãº","cjT","Ã»","cjd","Ã¼","cp","Ã½","cpr",
			"Ã¾","cps","Ã¿","cpf","ÃÀ","cpa","ÃÁ","cpq","ÃÂ","cpt","ÃÃ","cpd","ÃÄ","cu","ÃÅ","cus","ÃÆ","cuT","ÃÇ","cP",
			"ÃÈ","cPs","ÃÉ","cPd","ÃÊ","ch","ÃË","chr","ÃÌ","chs","ÃÍ","chf","ÃÎ","cha","ÃÏ","chq","ÃĞ","cht","ÃÑ","cha",
			"ÃÒ","chk","ÃÓ","chks","ÃÔ","chkf","ÃÕ","chkd","ÃÖ","chl","Ã×","chls","ÃØ","chlf","ÃÙ","chla","ÃÚ","chlq","ÃÛ","chlt",
			"ÃÜ","chld","Ãİ","cy","ÃŞ","cya","­h","cyq","Ãß","cn","Ãà","cnr","Ãá","cns","Ãâ","cnf","Ãã","cna","Ãä","cnq",
			"Ãå","cnt","Ãæ","cnd","Ãç","cnj","Ãè","cnjT","Ãé","cnp","Ãê","cnps","®X","cnpt","Ãë","cnl","Ãì","cnls","Ãí","cnlf",
			"Ãî","cnla","Ãï","cnlq","Ãğ","cnlt","Ãñ","cnld","Ãò","cb","Ãó","cbs","Ãô","cbf","Ãõ","cba","Ãö","cbd","Ã÷","cm",
			"Ãø","cmr","Ãù","cms","Ãú","cmf","Ãû","cma","Ãü","cmq","Ãı","cmt","Ãş","cmd","Ä¡","cl","Ä¢","clr","Ä£","cls",
			"Ä¤","cle","Ä¥","clf","Ä¦","clfr","Ä§","cla","Ä¨","clq","Ä©","clt","Äª","cld","Ä«","zk","Ä¬","zkr","Ä­","zks",
			"Ä®","zkf","Ä¯","zka","Ä°","zkq","Ä±","zkt","Ä²","zkd","Ä³","zo","Ä´","zor","Äµ","zos","Ä¶","zof","Ä·","zoa",
			"Ä¸","zoq","Ä¹","zot","Äº","zoT","Ä»","zod","Ä¼","zi","Ä½","zir","Ä¾","zid","Ä¿","zj","ÄÀ","zjr","ÄÁ","zjs",
			"ÄÂ","zje","ÄÃ","zjf","ÄÄ","zja","ÄÅ","zjq","ÄÆ","zjt","ÄÇ","zjT","ÄÈ","zjd","ÄÉ","zp","ÄÊ","zp","ÄË","zps",
			"ÄÌ","zpf","ÄÍ","zpa","ÄÎ","zpq","ÄÏ","zpt","ÄĞ","zpd","ÄÑ","zu","ÄÒ","zus","ÄÓ","zuf","ÄÔ","zua","ÄÕ","zuq",
			"ÄÖ","zut","Ä×","zuT","ÄØ","zud","ÄÙ","zP","ÄÚ","zh","ÄÛ","zhr","ÄÜ","zhs","Äİ","zhf","ÄŞ","zha","Äß","zhq",
			"Äà","zht","Äá","zhd","Äâ","zhk","Äã","zhkr","Ää","zhks","Äå","zhkf","Äæ","zhka","Äç","zhkd","Äè","zho","Äé","zhod",
			"Äê","zhl","Äë","zhlf","Äì","zy","Äí","zn","Äî","znr","Äï","zns","Äğ","znf","Äñ","zna","Äò","znq","Äó","znt",
			"Äô","znd","Äõ","znj","Äö","znjs","Ä÷","znjf","Äø","znjd","Äù","znp","Äú","znpd","Äû","znl","Äü","znlr","Äı","znls",
			"Äş","znlf","Å¡","znla","Å¢","znlq","Å£","znlt","Å¤","znld","Å¥","zb","Å¦","zbs","Å§","zbf","Å¨","zba","Å©","zm",
			"Åª","zmr","Å«","zms","Å¬","zmf","Å­","zma","Å®","zmq","Å¯","zmd","Å°","zl","Å±","zlr","Å²","zls","Å³","zlf",
			"Å´","zla","Åµ","zlq","Å¶","zlt","Å·","zld","Å¸","xk","Å¹","xkr","Åº","xks","Å»","xkf","Å¼","xkfr","Å½","xka",
			"Å¾","xkq","Å¿","xkt","ÅÀ","xkT","ÅÁ","xkd","ÅÂ","xo","ÅÃ","xor","ÅÄ","xos","ÅÅ","xof","ÅÆ","xoa","ÅÇ","xoq",
			"ÅÈ","xot","ÅÉ","xoT","ÅÊ","xod","ÅË","xi","ÅÌ","xid","ÅÍ","xj","ÅÎ","xjr","ÅÏ","xjs","ÅĞ","xjf","ÅÑ","xjfa",
			"ÅÒ","xja","ÅÓ","xjq","ÅÔ","xjt","ÅÕ","xjT","ÅÖ","xjd","Å×","xp","ÅØ","xpr","ÅÙ","xps","ÅÚ","xpf","ÅÛ","xpa",
			"ÅÜ","xpq","Åİ","xpt","ÅŞ","xpd","Åß","xu","Åà","xus","Åá","xuT","Åâ","xP","Åã","xPs","Åä","xh","Åå","xhr",
			"Åæ","xhs","Åç","xhf","Åè","xha","Åé","xhq","Åê","xht","Åë","xhd","Åì","xhv","Åí","xhk","Åî","xhks","Åï","xho",
			"Åğ","xhl","Åñ","xhls","Åò","xhlt","Åó","xhld","Åô","xy","Åõ","xn","Åö","xnr","Å÷","xns","Åø","xnf","Åù","xna",
			"Åú","xnq","Åû","xnt","Åü","xnd","Åı","xnj","Åş","xnjT","Æ¡","xnp","Æ¢","xnl","Æ£","xnlr","Æ¤","xnls","Æ¥","xnlf",
			"Æ¦","xnla","Æ§","xnlq","Æ¨","xnld","Æ©","xb","Æª","xbs","Æ«","xb","Æ¬","xba","Æ­","xbd","Æ®","xm","Æ¯","xmr",
			"Æ°","xms","Æ±","xme","Æ²","xmf","Æ³","xmfa","Æ´","xma","Æµ","xmq","Æ¶","xmt","Æ·","xml","Æ¸","xls","Æ¹","xmlf",
			"Æº","xmla","Æ»","xmlq","Æ¼","xl","Æ½","xlr","Æ¾","xls","Æ¿","xlf","ÆÀ","xla","ÆÁ","xlq","ÆÂ","xlt","ÆÃ","xld",
			"ÆÄ","vk","ÆÅ","vkr","ÆÆ","vkR","ÆÇ","vks","ÆÈ","vkf","ÆÉ","vkfa","ÆÊ","vka","ÆË","vkq","ÆÌ","vkt","ÆÍ","vkT",
			"ÆÎ","vkd","ÆÏ","vkx","ÆĞ","vko","ÆÑ","vor","ÆÒ","vos","ÆÓ","vof","ÆÔ","voa","ÆÕ","voq","ÆÖ","vot","Æ×","voT",
			"ÆØ","vod","ÆÙ","vi","ÆÚ","vir","ÆÛ","vj","ÆÜ","vjr","Æİ","vjs","ÆŞ","vjf","Æß","vja","Æà","vjq","Æá","vjt",
			"Æâ","vjT","Æã","vjd","Æä","vp","Æå","vpr","Ææ","vps","Æç","vpf","Æè","vpa","Æé","vpq","Æê","vpt","Æë","vpd",
			"Æì","vu","Æí","vus","Æî","vuf","Æï","vua","Æğ","vuq","Æñ","vuT","Æò","vud","Æó","vP","Æô","vPf","Æõ","vPq",
			"Æö","vPt","Æ÷","vh","Æø","vhr","Æù","vhs","Æú","vhf","Æû","vha","Æü","vhq","Æı","vht","Æş","vhd","Ç¡","vhk",
			"Ç¢","vhkd","Ç£","vhl","Ç¤","vhls","Ç¥","vy","Ç¦","vys","Ç§","vyf","Ç¨","vyq","Ç©","vyt","Çª","vn","Ç«","vnr",
			"Ç¬","vns","Ç­","vne","Ç®","vnf","Ç¯","vnfa","Ç°","vna","Ç±","vnq","Ç²","vnt","Ç³","vnd","Ç´","vnj","Çµ","vnjd",
			"Ç¶","vnl","Ç·","vnls","Ç¸","vnlf","Ç¹","vnla","Çº","vnlt","Ç»","vb","Ç¼","vbs","Ç½","vbf","Ç¾","vba","Ç¿","vbt",
			"ÇÀ","vbd","ÇÁ","vm","ÇÂ","vms","ÇÃ","vmf","ÇÄ","vma","ÇÅ","vmq","ÇÆ","vmt","ÇÇ","vl","ÇÈ","vlr","ÇÉ","vls",
			"ÇÊ","vlf","ÇË","vla","ÇÌ","vlq","ÇÍ","vlt","ÇÎ","vld","ÇÏ","gk","ÇĞ","gkr","ÇÑ","gks","ÇÒ","gkf","ÇÓ","gkfx",
			"ÇÔ","gka","ÇÕ","gkq","ÇÖ","gkt","Ç×","gkd","ÇØ","go","ÇÙ","gor","ÇÚ","gos","ÇÛ","gof","ÇÜ","goa","Çİ","goq",
			"ÇŞ","got","Çß","goT","Çà","god","Ád","gog","Çá","gi","Çâ","gid","Çã","gj","Çä","gjr","Çå","gjs","Çæ","gjf",
			"Çç","gjfa","Çè","gja","Çé","gjq","Çê","gjt","Çë","gjd","Çì","gp","Çí","gpr","Çî","gps","Çï","gpf","Çğ","gpa",
			"Çñ","gpq","Çò","gpt","Çó","gpd","Çô","gu","Çõ","gur","Çö","gus","Ç÷","guf","Çø","gua","Çù","guq","Çú","gut",
			"Çû","guT","Çü","gud","Çı","gP","Çş","gPs","È¡","gPf","È¢","gPq","È£","gh","È¤","ghr","È¥","ghs","È¦","ghf",
			"È§","ghfx","È¨","gha","È©","ghq","Èª","ght","È«","ghd","È¬","ghx","È­","ghk","È®","ghkr","È¯","ghks","È°","ghkf",
			"È±","ghkt","È²","ghkd","È³","gho","È´","ghor","Èµ","ghos","È¶","ghot","È·","ghod","È¸","ghl","È¹","ghlr","Èº","ghls",
			"È»","ghlf","È¼","ghlq","È½","ghlt","È¾","ghld","È¿","gy","ÈÀ","gys","ÈÁ","gyf","ÈÂ","gyq","ÈÃ","gyt","ÈÄ","gn",
			"ÈÅ","gnr","ÈÆ","gns","ÈÇ","gnf","ÈÈ","gnfx","ÈÉ","gna","ÈÊ","gnt","ÈË","gnd","ÈÌ","gnj","ÈÍ","gnjs","ÈÎ","gnjf",
			"ÈÏ","gnja","ÈĞ","gnjd","ÈÑ","gnp","ÈÒ","gnpr","ÈÓ","gnps","ÈÔ","gnpf","ÈÕ","gnpd","ÈÖ","gnl","È×","gnlr","ÈØ","gnls",
			"ÈÙ","gnlf","ÈÚ","gnla","ÈÛ","gnlq","ÈÜ","gnlt","Èİ","gnld","ÈŞ","gb","Èß","gbr","Èà","gbs","Èá","gbf","Èâ","gba",
			"Èã","gbt","Èä","gbd","Èå","gm","Èæ","gmr","Èç","gms","Èè","gmsg","Èé","gme","Èê","gmf","Èë","gmfr","Èì","gma",
			"Èí","gmq","Èî","gmt","Èï","gmd","Èğ","gmx","Èñ","gml","Èò","gmls","Èó","gmlf","Èô","gmla","Èõ","gmlq","Èö","gmld",
			"È÷","gl","Èø","glr","Èù","gls","Èú","glf","Èû","gla","Èü","glq","Èı","glt","Èş","gld"};

char jamo[34]="rRseEfaqQtTdwWczxvgkoiOjpuPhynbml";
char* han_jamo[33]={"¤¡","¤¢","¤¤","¤§","¤¨","¤©","¤±","¤²","¤³","¤µ","¤¶","¤·","¤¸","¤¹","¤º","¤»","¤¼","¤½","¤¾",
					"¤¿","¤À","¤Á","¤Â","¤Ã","¤Ä","¤Å","¤Æ","¤Ç","¤Ë","¤Ì","¤Ğ","¤Ñ","¤Ó"};

CHan::CHan()
{
	//°Ë»ö ¼Óµµ Çâ»óÀ» À§ÇÑ »¡¸®°¡±â
	int mp=0;
	char* check_text[19]={"°¡","±î","³ª","´Ù","µû","¶ó","¸¶","¹Ù","ºü","»ç","½Î","¾Æ","ÀÚ","Â¥","Â÷","Ä«","Å¸","ÆÄ","ÇÏ"};
	for(unsigned int i=0; i<HAN_MAX; ++i)
	{
		if(strcmp(han1[i*2], check_text[mp])==0)
		{
			mark[mp]=i;
			++mp;
			if(mp>=19)break;
		}
	}
	mark[19]=HAN_MAX;
}

unsigned int CHan::FindMark(char eng)
{
	for(unsigned int j=0; j<19; ++j)
		if(eng==jamo[j])return j;

	return 0;
}

char* CHan::EngToHan(char* text, char* han_area)
{
	unsigned int pp=0;
	unsigned int begin[STR_MAX], end[STR_MAX];

	//ÀÔ·Â ¹Ş±â
	strcpy(han, "");	

	//ÇÑ±Û¿¡ ´ëÄ¡ ¾È µÇ´Â ´ë¹®ÀÚ¸¦ ¼Ò¹®ÀÚ·Î ¹Ù²Ş
	for(unsigned int i=0; i<strlen(text); ++i)
	{
		if(han_area[i]==0)continue;
		if(text[i]>='A' && text[i]<='Z' && text[i]!='Q' && text[i]!='W' && text[i]!='E'	&& text[i]!='R'
			&& text[i]!='T' && text[i]!='O' && text[i]!='P')text[i]+=0x20;
	}

	//¸ğÀ½, ÀÚÀ½ ÇÑ±Û ¾Æ´Ô ¿©ºÎ È®ÀÎ(ÀÚÀ½ ÀÎ ºÎºĞÀ» Æ÷ÀÎÆ®·Î µÎ¾î¼­ º¯È¯¿¡ µµ¿òÀÌ µÇ°Ô ÇÔ)
	for(unsigned int i=1; i<strlen(text); ++i)
	{
		//ÀÚÀ½ÀÎÁö ¸ğÀ½ÀÎÁö ±¸ºĞ
		for(int j=0; j<33; ++j)
			if(text[i]==jamo[j])
			{
				if(j>=MOUM)
				{
					begin[pp]=i-1;
					if(pp>0)end[pp-1]=i-2;
					++pp;
					++i;	//ÀÌÁß ¸ğÀ½ÀÇ °æ¿ì¸¦ »ı°¢ÇØ¼­ ´ÙÀ½À» ³Ñ±è(ÀÌÁß ¸ğÀ½À» ²÷¾î ÀĞ¾î ±ÛÀÚ°¡ µÇ´Â °æ¿ì´Â ¾øÀ¸¹Ç·Î ¿¹: µŞ»ê(O) µÎ¤Ó¤µ»ê(X))
				}
				break;
			}
	}
	if(pp>0)end[pp-1]=(unsigned int)strlen(text)-1;

	//Æ÷ÀÎÆ® Á¶Á¤(ÇÑ±ÛÀÎ ºÎºĞ¸¸ ÇØ¼®)
	for(unsigned int i=0; i<pp; ++i)
	{
		char buffer[10];
		int bp=0;
		BOOL head=TRUE;	//¾Õ ºÎºĞÀÎÁö ±ÛÀÚ µŞºÎºĞÀÎÁö ¾Ë¾Æ³¿(Àß¶ó³»¾ß ÇÏ´Â ºÎºĞ ¿¹: 1@°¡2$ -> 1@Àº head=TRUEÀÎ »óÅÂ, 2$´Â head=FALSEÀÎ »óÅÂ¿¡¼­ ÀĞÀ½
		for(unsigned int j=begin[i]; j<=end[i]; ++j)
		{
			if(han_area[j] && (text[j]>='a' && text[j]<='z' || text[j]>='A' && text[j]<='Z'))
			{
				buffer[bp]=text[j];
				++bp;
				head=FALSE;
			}
			else
			{
				if(head)++begin[i];
					else end[i]=j-1;
			}
		}
		buffer[bp]=NULL;

		//¿µ¹®À» ÇÑ±Û·Î ÃÆÀ» ¶§ ÀÏÄ¡ÇÏ´Â ±ÛÀÚ Ã£±â
		while(strlen(buffer)>1)
		{
			BOOL find=FALSE;
			unsigned int loop_start=FindMark(buffer[0]);
			unsigned int loop_end=mark[loop_start+1];
			loop_start=mark[loop_start];

			for(unsigned int j=loop_start; j<loop_end; ++j)
				if(strcmp(han1[j*2+1], buffer)==0)
				{
					find=TRUE;
					break;
				}

			//Ã£Áö ¸ø ÇßÀ» ¶§ ¿µ¿ªÀ» ÁÙ¿©¼­ Ã£¾Æº½
			if(!find)
			{
				buffer[strlen(buffer)-1]=NULL;
				--end[i];
			}
			else break;
		}

		//ÀĞÁö ¸øÇÏ´Â ÇÑ±ÛÀÓÀ» Ç¥½Ã
		if(strlen(buffer)<=1)begin[i]=9999;
	}

	//º¯È¯
	unsigned int pp2=0;
	for(unsigned int i=0; i<strlen(text); ++i)
	{
		while(begin[pp2]==9999)++pp2;

		//ÇÑ±ÛÀÎ ºÎºĞ
		if(pp2<pp && i==begin[pp2])
		{
			char buffer[10];
			int bp=0;
			for(unsigned int j=begin[pp2]; j<=end[pp2]; ++j)
			{
				buffer[bp]=text[j];
				++bp;
			}
			buffer[bp]=NULL;

			unsigned int loop_start=FindMark(buffer[0]);
			unsigned int loop_end=mark[loop_start+1];
			loop_start=mark[loop_start];
			for(unsigned int j=loop_start; j<loop_end; ++j)
				if(strcmp(han1[j*2+1], buffer)==0)
				{
					strcat(han, han1[j*2]);
					break;
				}

			i=end[pp2];
			++pp2;
		}
		//¾Æ´Ñ ºÎºĞ
		else
		{			
			//ÇÑ±Û ÀÏ¶§´Â ²÷¾îÁø ¸ğÀ½ ÀÚÀ½À¸·Î Ç¥Çö
			if(han_area[i]!=0 && (text[i]>='a' && text[i]<='z' || text[i]>='A' && text[i]<='Z'))
			{
				for(int j=0; j<33; ++j)
				{
					if(text[i]==jamo[j])
					{
						strcat(han, han_jamo[j]);
						break;
					}
				}
			}
			//°á±¹ ¾øÀ» ¶© ¹®ÀÚ ±×´ë·Î
			else
			{
				char tmp[2];
				tmp[0]=text[i];
				tmp[1]=NULL;
				strcat(han, tmp);
			}			
		}
	}

	han[strlen(han)]=NULL;
	return han;
}

/////////////////////////////////////////////////////
//±âº» ´ëÈ­ Ã³¸® Å¬·¡½º ¸Ş¼Òµå
CDlg::CDlg()
{
	setting=setting2=FALSE;
	b_shadow=FALSE;
	dlgbox=NULL;
	show_dlgbox=TRUE;
	SetColor(0,0,0);
	text_font=&global_font;
	frame_x=frame_y=5;
}

void CDlg::Move(int vx, int vy)
{
	if(!setting || !setting2)return;	//´ëÈ­Ã¢ÀÌ ¿Ï¼ºµÇ±â Àü¿¡ ÀÌµ¿½ÃÅ°¸é ¹ö±×°¡ ÀÏ¾î³²

	x+=vx;
	y+=vy;
	x=MaxMin(x, 0, SCREEN_X-x_size);
	y=MaxMin(y, 0, SCREEN_Y-y_size);
}

void CDlg::SetColor(BYTE r, BYTE g, BYTE b)
{
	color.r=r;
	color.g=g;
	color.b=b;
}

void CDlg::SetDlgBox(char* box_name)
{
	//»ç¿ëÀÚ°¡ ÁöÁ¤ÇÑ ´ëÈ­Ã¢ ¾²±â
	dlgbox=box_name;
}

void CDlg::SetFont(JFont *font)
{
	if(font!=NULL)text_font=font;
}

void CDlg::DrawDlg()
{
	if(!setting || !show_dlgbox)return;	//ÃÊ±âÈ­°¡ ¾È µÇ¾î ÀÖÀ¸¸é ¸®ÅÏ

	if(dlgbox==NULL)
	{
		//À§
		jdd->DrawPicture(backbuffer,"CommonDlg1",x-frame_x,y-frame_y,NULL);
		for(int i=0; i<width; ++i)jdd->DrawPicture(backbuffer,"CommonDlg2",x+(i*10),y-frame_y,NULL);
		jdd->DrawPicture(backbuffer,"CommonDlg3",x+x_size,y-frame_y,NULL);
		//Áß°£
		for(int i=0; i<height; ++i)
		{
			int tmp_y=y+i*20;
			jdd->DrawPicture(backbuffer,"CommonDlg4",x-frame_x,tmp_y,NULL);
			for(int j=0; j<width; ++j)jdd->DrawPicture(backbuffer,"CommonDlg5",x+(j*10),tmp_y,NULL);
			jdd->DrawPicture(backbuffer,"CommonDlg4",x+x_size,tmp_y,NULL);
		}
		
		//¾Æ·¡
		jdd->DrawPicture(backbuffer,"CommonDlg7",x-frame_x,y+y_size,NULL);
		for(int i=0; i<width; ++i)jdd->DrawPicture(backbuffer,"CommonDlg2",x+(i*10),y+y_size,NULL);
		jdd->DrawPicture(backbuffer,"CommonDlg9",x+x_size,y+y_size,NULL);
	}
	else
	{
		jdd->DrawPicture(backbuffer,dlgbox,x-frame_x,y-frame_y,NULL);
	}
	/*¼Óµµ°¡ ´À·Á¼­ strech·Î ´ëÈ­Ã¢À» Ãâ·ÂÇÏ´Â ½ÄÀ¸·Î ¹Ù²Ü »ı°¢(»ç¿ëµéÀÌ ÀÚ½Å¸¸ÀÇ ´ëÈ­Ã¢ ¸¸µé±âµµ ½¬¿ò)*/
}

void CDlg::ShowShadow(BOOL show_or_hide)
{
	b_shadow=show_or_hide;
}

void CDlg::ShowDlg(BOOL show_or_hide)
{
	show_dlgbox=show_or_hide;
}

BOOL CDlg::MouseInBox()
{
	if(mouse_control && MouseX>=x && MouseX<x+width*10 && MouseY>=y && MouseY<y+line*20)return TRUE;
		else return FALSE;
}

void CDlg::SetOpacity(float opacity)
{
	JPictureInfo pi;
	pi.SetOpacity(opacity);
	jdd->SetPictureInfo("CommonDlg1", &pi);
	jdd->SetPictureInfo("CommonDlg2", &pi);
	jdd->SetPictureInfo("CommonDlg3", &pi);
	jdd->SetPictureInfo("CommonDlg4", &pi);
	jdd->SetPictureInfo("CommonDlg5", &pi);
	jdd->SetPictureInfo("CommonDlg6", &pi);
	jdd->SetPictureInfo("CommonDlg7", &pi);
	jdd->SetPictureInfo("CommonDlg8", &pi);
	jdd->SetPictureInfo("CommonDlg9", &pi);
}

void CDlg::SetFrameSize(int vx, int vy)
{
	frame_x=vx;
	frame_y=vy;
}
//////////////////////////////////////////////////////
//Ãâ·ÂÀ» À§ÇÑ ´ëÈ­»óÀÚ
#define SNR_START 0

CTextDlg::CTextDlg(unsigned int vbuffer_size)
{
	buffer_size=vbuffer_size;
	dlg_buffer=new char[buffer_size];	//´ëÈ­ ³»¿ë ¹öÆÛ
	text_buffer=new char[buffer_size];	//ÇöÀç±îÁö Ãâ·ÂÇÑ ³»¿ëÀÇ ¹öÆÛ

	lock=FALSE;
}

CTextDlg::~CTextDlg()
{
	//¹öÆÛ Áö¿ì±â
	if(dlg_buffer!=NULL)
	{
		delete[] dlg_buffer;
		dlg_buffer=NULL;
	}

	if(text_buffer!=NULL)
	{
		delete[] text_buffer;
		text_buffer=NULL;
	}
}

void CTextDlg::SetDlg(int vx, int vy, int vwidth, int vline)
{
	x=vx;
	y=vy;
	width=Max(30,vwidth);
	width/=10;
	line=Max(1,vline);

	setting=TRUE;
}

//´ëÈ­ ¸¸µé±â
void CTextDlg::MakeText(char* content)
{
	if(!setting || content==NULL)return;	//ÃÊ±âÈ­°¡ ¾È µÇ¾î ÀÖÀ¸¸é ¸®ÅÏ

	//´ëÈ­³»¿ë º¹»ç
	length=(int)strlen(content);
	strncpy(dlg_buffer, content, buffer_size);
	if(strlen(content) >= buffer_size)dlg_buffer[buffer_size-1]=NULL;
	ani_end=FALSE;

	//´ëÈ­¸¦ Ãâ·ÂÇÏ±â ¾Ë¸Â°Ô Ã³¸®
	sp=SNR_START;	//Ã³¸®µÇ°í ÀÖ´Â Æ÷ÀÎÆ®
	int space=-1;	//ÀÌÀü±îÁö Ã³¸®ÇÑ Æ÷ÀÎÆ®, space°¡ ÀÖ´ø °÷
	int senlen=0;	//¹®ÀåÀÇ ±æÀÌ
	
	//´ëÈ­Ã¢¿¡ ¸Â°Ô ¹®ÀåÀ» ÁÙ ¹Ù²Ş
	if(text_font==&global_font)
	while(sp<length)
	{
		BOOL length_check=TRUE;		//±æÀÌ Ã¼Å©¸¦ ÇÒÁö ¾È ÇÒÁö °Ë»ç
		int sp_add=1;				//ÀÌµ¿ÇÒ Ä­ ¼ö, ÇÑ±ÛÀÏ °æ¿ì¸¸ 2Ä­.

		//ÁÙ ¹Ù²Ş
		if(dlg_buffer[sp]=='\\')
		{
			dlg_buffer[sp]='\n';
			space=-1;
			senlen=0;
			length_check=FALSE;
		}
		//ÇÑ±Û
		else if(dlg_buffer[sp]&0x80)
		{
			senlen+=20;
			sp_add=2;
		}
		//±âÅ¸
		else
		{
			if(dlg_buffer[sp]==' ')space=sp;
			senlen+=GetCharLength(dlg_buffer[sp]);
		}
		
		//±æÀÌ°¡ ´ëÈ­Ã¢À» ³Ñ¾ú´ÂÁö °Ë»ç
		if(length_check && senlen>width*10)
		{
			if(space<0)	//½ºÆäÀÌ½º°¡ ÇÏ³ªµµ ¾øÀ»¶§ °­Á¦ ÁÙ º¯È¯
			{
				//»çÀÌ¿¡ CRÀ» ³Ö¾îÁÜ
				char* tmp_text;
				tmp_text = new char[length+2];

				//»õ·Î¿î ¹®ÀÚ¿­(Áö±İ °Íº¸´Ù ±æÀÌ°¡ 1 Å« °Í)¿¡ ³»¿ë º¹»ç
				for(int i=0; i<sp; ++i)tmp_text[i]=dlg_buffer[i];
				tmp_text[sp]='\n';
				for(int i=sp+1; i<length+1; ++i)tmp_text[i]=dlg_buffer[i-1];
				tmp_text[length+1]=NULL;
				strcpy(dlg_buffer,tmp_text);
				delete[] tmp_text;

				//Áö±İ ¹®ÀÚ¿­ ±æÀÌ´Â Áö±İ ¹®ÀÚÀÇ ±æÀÌ
				if(dlg_buffer[sp+1]&0x80)senlen=20;
					else senlen=GetCharLength(dlg_buffer[sp+1]);

				++length;
				++sp;
			}
			else
			{
				dlg_buffer[space]='\n';
				senlen=0;
				for(int i=space+1;i<=sp;++i)
				{
					if(dlg_buffer[i]&0x80)
					{
						senlen+=20;
						++i;
					}
					//±âÅ¸
					else senlen+=GetCharLength(dlg_buffer[i]);
				}
				space=-1;
			}
		}

		//ÇÑ±ÛÀÏ ¶§ 2Ä­ ¶Ù°í ³ª¸ÓÁö´Â 1Ä­
		sp+=sp_add;
	}

	sp=SNR_START, tp=0;;	//Ãâ·ÂÀ§Ä¡ ÃÊ±âÈ­
	n_of_e=0;				//ÁÙ¹Ù²Ş È½¼ö
	//´ëÈ­Ã¢ Å©±â ¼³Á¤
	x_size=width*10;
	y_size=line*20;
	height=line;

	setting2=TRUE;
}

//´ëÈ­ Ãâ·Â(Á¤ÁöµÈ »óÅÂ¿¡¼­)
void CTextDlg::Print(char* content)
{
	if(!setting)return;	//ÃÊ±âÈ­°¡ ¾È µÇ¾î ÀÖÀ¸¸é ¸®ÅÏ

	MakeText(content);

	//µŞ¹è°æ Àá½Ã ÀúÀå(Åõ¸íµµ ÀÖ´Â ´ëÈ­Ã¢ÀÏ ¶§ °ãÃÄ ¾²¸é¼­ Åõ¸íµµ°¡ ¾ø¾îÁö´Â °ÍÀ» ¹æÁö
	JPictureInfo pi;
	pi.SetWidth(SCREEN_X);
	pi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonCopyScreen", &pi, TRUE);
	jdd->DrawPicture("CommonCopyScreen", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//À©µµ¿ì ¸Ş¼¼Áö Ã³¸®

		jdd->DrawPicture(backbuffer, "CommonCopyScreen", 0, 0, NULL);
		if(Printing()==999)break;	//³»¿ëÀ» ´Ù Ãâ·ÂÇÏ¸é Áß´Ü
		jdd->Render();
		//»¡¸® ³Ñ±â±â
		if(keyboard_control && GetKey(vkey_enter, 0) || mouse_control && LButton);
			else Sleep(10);
	}

	jdd->DeleteSurface("CommonCopyScreen");
}

//´ëÈ­Ãâ·Â(½Ç½Ã°£À¸·Î)
int CTextDlg::Printing()
{
	if(!setting || !setting2)return -1;	//ÃÊ±âÈ­°¡ ¾È µÇ¾î ÀÖÀ¸¸é ¸®ÅÏ

	if(!ani_end)
	{
		if(sp<length && n_of_e<line)
		{
			if(dlg_buffer[sp]=='\n')++n_of_e;
			text_buffer[tp]=dlg_buffer[sp];
			if(dlg_buffer[sp]&0x80)	//ÇÑ±ÛÀÏ °æ¿ì 2Ä­ ÀüÁø
			{
				++sp; ++tp;
				text_buffer[tp]=dlg_buffer[sp];
			}
			text_buffer[tp+1]=NULL;
			//Æ÷ÀÎÅÍ Áõ°¡
			++sp; ++tp;
		}
		else ani_end=TRUE;
	}
	else
	{
		//´ÙÀ½À¸·Î
		if(keyboard_control && GetKey(vkey_enter) || mouse_control && LeftDown())
		{
			if(sp<length)
			{
				ani_end=FALSE;
				n_of_e=0;
				tp=0;
			}
			else
			{
				if(!lock)return 999;
			}
		}
	}
	//´ëÈ­ Ãâ·Â
	DrawDlg();

	RECT text, shadow;
	//±×¸²ÀÚ
	if(b_shadow)
	{
		SetRect(&shadow,x+1,y+1,SCREEN_X,SCREEN_Y);
		jdd->DrawText(backbuffer,text_buffer,*text_font,&shadow,JColor(0,0,0));
	}
	//±ÛÀÚ
	SetRect(&text,x,y,SCREEN_X,SCREEN_Y);
	jdd->DrawText(backbuffer,text_buffer,*text_font,&text,color);

	return 0;
}

void CTextDlg::Lock()
{
	lock=TRUE;
}

void CTextDlg::UnLock()
{
	lock=FALSE;
}

//////////////////////////////////////////////////////
//¼±ÅÃÀ» À§ÇÑ ´ëÈ­»óÀÚ
CSelectDlg::CSelectDlg(int select_max, unsigned int vbuffer_size)
{
	buffer_size=vbuffer_size;
	dlg_buffer=new char[buffer_size];
	select_point=new char*[select_max];

	setting2=FALSE;
}

CSelectDlg::~CSelectDlg()
{
	//¹öÆÛ Áö¿ì±â
	if(dlg_buffer!=NULL)
	{
		delete [] dlg_buffer;
		dlg_buffer=NULL;
	}

	if(select_point!=NULL)
	{
		delete [] select_point;
		select_point=NULL;
	}
}

void CSelectDlg::SetDlg(int vx, int vy, int vgap, int vline, BOOL vrow, int start)
{
	x=vx;
	y=vy;
	gap=vgap;
	line=vline;
	select=start;

	//gapÅ©±â¸¦ 10À¸·Î ¶³¾îÁö°Ô ¼öÁ¤
	gap=Max(gap, 30);
	gap-=(gap%10);

	//°¡·Î¼±ÅÃÁöÀÎÁö ¼¼·Î ¼±ÅÃÁöÀÎÁö È®ÀÎ, ±æÀÌ Á¶Á¤
	if(line<=0)line=1;
	row=vrow;

	//¼¿·ºÆ®¹Ù À§Ä¡ ÃÊ±âÈ­
	if(row)
	{
		sx=x+select%line*gap;
		sy=y+(select/line)*20;
	}
	else
	{
		sx=x+select/line*gap;
		sy=y+(select%line)*20;
	}

	setting=TRUE;
}

void CSelectDlg::MakeSelection(char* input_dlg)
{
	if(!setting || input_dlg==NULL)return;

	//¹öÆÛ Å©±â¸¦ ³Ñ¾î°¡´Â °ÍÀ» ¹æÁö
	strncpy(dlg_buffer, input_dlg, buffer_size);
	if(strlen(input_dlg)>=buffer_size)dlg_buffer[buffer_size-1]=NULL;

	int length=(int)strlen(dlg_buffer);

	//´ëÈ­¸¦ Ãâ·ÂÇÏ±â ¾Ë¸Â°Ô Ã³¸®
	n_of_e=0;
	int bp=1;
	select_point[0]=dlg_buffer;
	for(int i=0; i<length; ++i)
	{
		if(dlg_buffer[i]=='\\')
		{
			dlg_buffer[i]=NULL;
			select_point[bp]=dlg_buffer+i+1;
			++n_of_e;
			++bp;
			continue;
		}

		if(dlg_buffer[i]==13)
		{
			continue;
		}
	}

	//½ºÅ©¸³Æ®¿¡¼­ ÀĞ¾î ¿Â ¹®ÀåÀÇ ³¡¿¡ À½Ç¥(0x0d)°¡ »ı±â´Â °Í ¹æÁö
	if(dlg_buffer[length-1]==13)dlg_buffer[length-1]=NULL;

	//´ëÈ­Ã¢ Ç¥½Ã¿¡ ÇÊ¿äÇÑ °Íµé °è»ê
	if(row)
	{
		col=(n_of_e+1)/line+((((n_of_e+1)%line)==0)?0:1);
		x_size=line*gap;
		y_size=col*20;
		width=gap/10*line;
		height=col;
	}
	else
	{
		col=(n_of_e+1)/line+((((n_of_e+1)%line)==0)?0:1);
		x_size=col*gap;
		y_size=line*20;
		width=gap/10*col;
		height=line;
	}

	setting2=TRUE;
}

int CSelectDlg::Select(char* input_dlg, BOOL cancel)
{
	if(!setting)return -999;

	MakeSelection(input_dlg);
	//µŞ¹è°æ Àá½Ã ÀúÀå(Åõ¸íµµ ÀÖ´Â ´ëÈ­Ã¢ÀÏ ¶§ °ãÃÄ ¾²¸é¼­ Åõ¸íµµ°¡ ¾ø¾îÁö´Â °ÍÀ» ¹æÁö
	JPictureInfo pi;
	pi.SetWidth(SCREEN_X);
	pi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonCopyScreen", &pi, TRUE);
	jdd->DrawPicture("CommonCopyScreen", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//À©µµ¿ì ¸Ş¼¼Áö Ã³¸®

		jdd->DrawPicture(backbuffer, "CommonCopyScreen", 0, 0, NULL);
		int selected=Selecting();
		if(selected>=0 || (cancel && selected==-999))break;	//¼±ÅÃÇÏ°Å³ª Ãë¼ÒÇÏ¸é Áß´Ü

		jdd->Render();
	}

	jdd->DeleteSurface("CommonCopyScreen");

	return select;
}

int CSelectDlg::Selecting()
{
	if(!setting2)return -999;

	//´ëÈ­»óÀÚ
	if(keyboard_control)
	{
		if(row)
		{
			//À§ ¾Æ·¡ ÀÌµ¿
			if(GetKey(vkey_right,10))select=Min(n_of_e,select+1);
				else if(GetKey(vkey_left,10))select=Max(0,select-1);
			//ÁÂ¿ì ÀÌµ¿
			if(GetKey(vkey_down,10))
			{
				if(select+line<=n_of_e)select+=line;
			}
			else if(GetKey(vkey_up,10))
			{
				if(select-line>=0)select-=line;
			}

			sx=x+select%line*gap;
			sy=y+(select/line)*20;
		}
		else
		{
			//À§ ¾Æ·¡ ÀÌµ¿
			if(GetKey(vkey_down,10))select=Min(n_of_e,select+1);
				else if(GetKey(vkey_up,10))select=Max(0,select-1);

			//ÁÂ¿ì ÀÌµ¿
			if(GetKey(vkey_right,10))
			{
				if(select+line<=n_of_e)select+=line;
			}
			else if(GetKey(vkey_left,10))
			{
				if(select-line>=0)select-=line;
			}

			sx=x+select/line*gap;
			sy=y+(select%line)*20;
		}
	}

	//¸¶¿ì½º·Î ¼±ÅÃ
	BOOL mouse_select=FALSE;
	if(mouse_control && MouseInBox())
	{
		for(int i=0; i<=n_of_e; ++i)
		{
			int tsx, tsy;
			if(row)
			{
				tsx=x+(i%line)*gap;
				tsy=y+(i/line)*20;
			}
			else
			{
				tsx=x+(i/line)*gap;
				tsy=y+(i%line)*20;
			}

			if(MouseX>=tsx && MouseX<tsx+gap && MouseY>=tsy && MouseY<tsy+20)
			{
				select=i;
				mouse_select=TRUE;
				sx=tsx;
				sy=tsy;
				break;
			}
		}
	}


	DrawDlg();
	//¼±ÅÃ Ä¿¼­
	RECT src_rect;
	SetRect(&src_rect,0,0,gap/2,20);
	jdd->DrawPicture(backbuffer,"CommonSelect",sx,sy,&src_rect);
	SetRect(&src_rect,320-gap/2,0,320,20);
	jdd->DrawPicture(backbuffer,"CommonSelect",sx+gap/2,sy,&src_rect);

	//¼±ÅÃÁöµé
	for(int i=0; i<=n_of_e; ++i)
	{
		int tx,ty;
		if(row)
		{
			tx=(i%line)*gap+x;
			ty=(i/line)*20+y;
		}
		else
		{
			tx=(i/line)*gap+x;
			ty=(i%line)*20+y;
		}

		if(b_shadow)jdd->DrawText(backbuffer,select_point[i],*text_font,tx+1,ty+1,JColor(0,0,0));
		jdd->DrawText(backbuffer,select_point[i],*text_font,tx,ty,color);
	}

	if(keyboard_control && GetKey(vkey_enter) || mouse_select && LeftDown())return select;
		else if(keyboard_control && GetKey(vkey_esc) || mouse_control && RightDown())return -999;
		else return -1;
}

void CSelectDlg::SetSelect(int select)
{
	this->select = select;
}

//////////////////////////////////////////////////////
//ÀÔ·ÂÀ» À§ÇÑ ´ëÈ­»óÀÚ
CInputDlg::CInputDlg(int vx, int vy, int max)
{
	flag= FLAG_TEXT	| FLAG_NUMBER | FLAG_MARK;

	SetDlg(vx, vy, max);
}

CInputDlg::CInputDlg()
{
	flag= FLAG_TEXT	| FLAG_NUMBER | FLAG_MARK;

	SetDlg(250,230,140);	//±âº»°ª
}

void CInputDlg::SetDlg(int vx, int vy, int max)
{
	Clear();
	str_max=max;

	x=vx;
	y=vy;
	width=Max(30,max);
	width/=10;

	str_max*=10;
	//´ëÈ­Ã¢ Å©±â ¼³Á¤
	setting=TRUE;
	x_size=str_max/10;
	y_size=20;
	height=line=1;

	setting=TRUE;
}

void CInputDlg::Clear()
{
	strcpy(input_text,"");
	strcpy(eng_buffer,"");
	old_len=text_length=0;
}

//ÇÑ±ÛÀÔ·Â
char* CInputDlg::Input()
{
	//µŞ¹è°æ Àá½Ã ÀúÀå(Åõ¸íµµ ÀÖ´Â ´ëÈ­Ã¢ÀÏ ¶§ °ãÃÄ ¾²¸é¼­ Åõ¸íµµ°¡ ¾ø¾îÁö´Â °ÍÀ» ¹æÁö
	JPictureInfo pi;
	pi.SetWidth(SCREEN_X);
	pi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonCopyScreen", &pi, TRUE);
	jdd->DrawPicture("CommonCopyScreen", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//À©µµ¿ì ¸Ş¼¼Áö Ã³¸®

		jdd->DrawPicture(backbuffer, "CommonCopyScreen", 0, 0, NULL);
		if(Inputing())break;	//ÀÔ·ÂÀÌ ³¡³ª¸é Áß´Ü

		jdd->Render();
	}

	jdd->DeleteSurface("CommonCopyScreen");

	return input_text;
}

//ÀÔ·Â°¡´ÉÇÑ Æ¯¼ö¹®ÀÚµé
char num_shift_char[]={')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
char special_char[]={';', '=', ',', '-', '.', '/', '`', '[', '\\', ']','\''};	// `´Â ½Å¸íÁ¶¿¡¼­ Áö¿ø ¾È µÊ
char special_shift_char[]={':', '+', '<', '_', '>', '?', '~', '{', '|', '}','"'};

int CInputDlg::Inputing()
{
	//Ãâ·Â
	DrawDlg();
	if(b_shadow)jdd->DrawText(backbuffer,input_text,*text_font,x+1,y+1,JColor(0,0,0));
	//±ôºıÀÌ´Â Ä¿¼­
	char show_text[101];
	strcpy(show_text, input_text);
	static int blink_count;
	++blink_count;
	if(blink_count<0 || blink_count>=40)blink_count=0;
	if(blink_count<20 && str_max>text_length+10)strcat(show_text,"_");
	//ÀÔ·ÂµÈ Á¤º¸ Ç¥½Ã
	if(b_shadow)jdd->DrawText(backbuffer,show_text,*text_font,x+1,y+1,JColor(0,0,0));
	jdd->DrawText(backbuffer,show_text,*text_font,x,y,color);

	int eng_len=(int)strlen(eng_buffer);
	//ÀÔ·Â
	if(eng_len<80)
	{
		BOOL done=FALSE;
		//¹®ÀÚÅ°
		if(flag & FLAG_TEXT)
		{
			for(int i=vkey_a; i<=vkey_z; ++i)
				if(GetKey((ValidKey)i))
				{
					size_t buffer_length=strlen(eng_buffer);
					if(_GetKeyState(VK_SHIFT))eng_buffer[buffer_length]='A'+(char)(i-vkey_a);
						else eng_buffer[buffer_length]='a'+(char)(i-vkey_a);
					eng_buffer[buffer_length+1]=NULL;

					done=TRUE;
					break;
				}
		}
		//¼ıÀÚÅ°
		if(flag & FLAG_NUMBER)
		{
			if(!done)for(int i=vkey_0; i<=vkey_9; ++i)
				if(GetKey((ValidKey)i))
				{
					size_t buffer_length=strlen(eng_buffer);
					if(_GetKeyState(VK_SHIFT))eng_buffer[buffer_length]=num_shift_char[i-vkey_0];
						else eng_buffer[buffer_length]='0'+(char)(i-vkey_0);
					eng_buffer[buffer_length+1]=NULL;

					done=TRUE;
					break;
				}
		}
		//±âÅ¸ ±âÈ£
		if(flag & FLAG_MARK)
		{
			if(!done)for(int i=vkey_colon; i<=vkey_quotation; ++i)
				if(GetKey((ValidKey)i))
				{
					size_t buffer_length=strlen(eng_buffer);
					if(_GetKeyState(VK_SHIFT))eng_buffer[buffer_length]=special_shift_char[i-vkey_colon];
						else eng_buffer[buffer_length]=special_char[i-vkey_colon];
					eng_buffer[buffer_length+1]=NULL;

					done=TRUE;
					break;
				}
		}

		//½ºÆäÀÌ½º
		if(!done && GetKey(vkey_space))strcat(eng_buffer, " ");

		if(done && strlen(eng_buffer)>0)is_eng_buffer[strlen(eng_buffer)-1]=is_eng;
	}
	//Æ¯¼ö Å°
	if(GetKey(vkey_enter))return 1;	//¿£ÅÍ
	else if(GetKey(vkey_back, 10))	//¹é½ºÆäÀÌ½º
	{
		if(eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
	}
	else if(GetKey(vkey_alt))
	{
		is_eng=1-is_eng;
	}

	//ÀÔ·Â Á¤º¸ °»½Å
	if(old_len!=eng_len)
	{
		BOOL able_text=FALSE;
		while(!able_text)
		{
			//¹®ÀÚ ÀüÃ¼ ±æÀÌ °è»ê
			strcpy(input_text, m_han.EngToHan(eng_buffer, is_eng_buffer));
			text_length=0;
			for(size_t i=0; i<strlen(input_text) ; ++i)text_length+=GetCharLength(input_text[i]);

			//¸¸¾à ´ëÈ­Ã¢ ÀÌ»óÀÇ ±æÀÌ¶ó¸é ÀÚ¸£±â
			if(str_max/10<text_length && eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
				else able_text=TRUE;
		}
		old_len=eng_len;

		//¼­ÆäÀÌ½º¿¡ ±×¸®±â
		if(b_shadow)jdd->DrawText(backbuffer,input_text,*text_font,x+1,y+1,JColor(0,0,0));
		//±ôºıÀÌ´Â Ä¿¼­
		char show_text[101];
		strcpy(show_text, input_text);
		static int blink_count;
		++blink_count;
		if(blink_count<0 || blink_count>=40)blink_count=0;
		if(blink_count<20 && str_max>text_length+10)strcat(show_text,"_");
		//ÀÔ·ÂµÈ Á¤º¸ Ç¥½Ã
		jdd->DrawText(backbuffer,show_text,*text_font,x,y,color);

	}

	return 0;
}

void CInputDlg::SetFlag(BOOL vflag)
{
	flag = vflag;
}

/////////////////////////////////
//¸í·É Ã³¸® Å¬·¡½º ¸Ş¼Òµå
//(¹®ÀÚ ¹è¿­·Î char com={"ÀÌµ¿","´ë±â"...}µîÀ» ¸¸µé¾î ³õ°í, °¡´ÉÇÑ ¸í·ÉÀ» ³Ö¾î¼­ ¼±ÅÃÇÒ ¼ö ÀÖ°Ô ÇÑ´Ù.

CCommand::CCommand(char* vsource[], int vx, int vy, int vgap, int vline, BOOL vrow, int max)
				//¸í·É¾î ¹®ÀÚ ¹è¿­, X,YÁÂÇ¥, »çÀÌ°£°İ, ¸î°³ ÀÌ»ó Ç¥½ÃµÇ¸é ÁÙÀ» ¹Ù²Ü°ÇÁö, ¼¼·Î or °¡·Î, ÃÖ´ëÄ¡ (¹öÆÛ»çÀÌÁî Ãß°¡ÇÒ °Í!)
{
	comdlg.SetDlg(vx, vy, vgap, vline, vrow);
	Init(vsource,max);
	x=vx;
	y=vy;
	gap=vgap;
	line_max=vline;
	row=vrow;
}

void CCommand::Init(char* vsource[], int max)
{
	count=0;
	source=vsource;
	command_max=max;
	strcpy(commands,"");
	comdlg.MakeSelection(commands);
}

void CCommand::AddComs(int n, ...)	//¸í·É ¿©·¯°³ Ãß°¡
{
	va_list ap;
	va_start(ap,n);
	for(int i=0; i<n; ++i)AddCom(va_arg(ap,int));
	va_end(ap);
}

void CCommand::AddCom(int id)	//»ç¿ëÇÒ ¼ö ÀÖ´Â ¸í·É Ãß°¡
{
	if(count>=command_max)return;	//¸í·É¾î°¡ ²Ë Âü
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,source[id]);
	comdlg.SetDlg(x, y, gap, Min(line_max, count+1), row);
	comdlg.MakeSelection(commands);
	com_id[count]=id;
	++count;
}

//¹®ÀÚ¹è¿­À» ÀÌ¿ëÇÏÁö ¾Ê°í Á÷Á¢ Áı¾î ³Ö±â
void CCommand::AddComStr(int id, char* str)
{
	if(count>=command_max)return;	//¸í·É¾î°¡ ²Ë Âü
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,str);
	comdlg.SetDlg(x, y, gap, Min(line_max, count+1), row);
	comdlg.MakeSelection(commands);
	com_id[count]=id;
	++count;
}

int CCommand::GetComID(int count)
{
	if(count>=0 && count<command_max)return com_id[count];
		else return -1;
}

//°¡µæÃ¡´ÂÁö È®ÀÎ
BOOL CCommand::IsFull()
{
	if(count>=command_max)return TRUE;
		else return FALSE;
}

//½Ç½Ã°£À¸·Î ¼±ÅÃ(´Ù¸¥ ÀÛ¾÷°ú º´·Ä·Î Ã³¸®)
int CCommand::CommandSelecting()
{
	if(count<1)return -999;		//»ç¿ë °¡´É ¸í·ÉÀÌ ¾øÀ¸¸é ½ÇÇàÇÏÁö ¾ÊÀ½

	int selected;
	selected=comdlg.Selecting();

	if(selected>=0)return GetComID(selected);
		else return -1;
}

//¼±ÅÃ¸¸ ÇÔ(ÇÁ·Î¼¼½º µ¶Á¡)
int CCommand::CommandSelect()
{
	int selected;
	while(1)
	{
		if(!ProcessMessage())break;		//À©µµ¿ì ¸Ş¼¼Áö Ã³¸®

		selected=comdlg.Selecting();	//¸í·É ¼±ÅÃ
		if(selected>=0)break;
		else if(selected==-999)return -1;

		jdd->Render();
	}

	return GetComID(selected);
}

//ºñ¾úÀ¸¸é TRUE, Â÷ ÀÖÀ¸¸é FALSE¸¦ ¸®ÅÏ
BOOL CCommand::Empty()
{
	if(count==0)return TRUE;
		else return FALSE;
}

//////////////////////////////////////
//ÆÄÀÏ Ã³¸® Å¬·¡½º
CFiles::CFiles()
{
}

CFiles::~CFiles()
{
	std::vector<char*>::iterator it;
	for(it=filename.begin(); it!=filename.end(); ++it)
	{
		delete [] (*it);
	}

	filename.clear();
}

void CFiles::AddFile(char* name)
{
	char* newname = new char[strlen(name)+1];
	strcpy(newname, name);
	filename.push_back(newname);
}

void CFiles::SearchFile(char* dir, char* filetype)
{
	HANDLE hFind;
    WIN32_FIND_DATA FindData;
    int ErrorCode;
    BOOL Continue = TRUE;

	char search_for[256];
	sprintf(search_for, "%s\\*.%s", dir, filetype);

    hFind = FindFirstFile(search_for, &FindData);

    if(hFind == INVALID_HANDLE_VALUE)
    {
        ErrorCode = GetLastError();
        if (ErrorCode == ERROR_FILE_NOT_FOUND)
        {
            //printf("There are no files matching that path/mask\n");
        }
        else
        {
            //printf("FindFirstFile() returned error code %d", ErrorCode);
        }
        Continue = FALSE;
    }
    else
    {
        AddFile(FindData.cFileName);
    }

    if (Continue)
    {
        while (FindNextFile(hFind, &FindData))
        {
           AddFile(FindData.cFileName);
        }

        ErrorCode = GetLastError();

        if (ErrorCode != ERROR_NO_MORE_FILES)
        {
            //printf("FindNextFile() returned error code %d", ErrorCode);
        }

        if (!FindClose(hFind))
        {
            ErrorCode = GetLastError();
            //printf("FindClose() returned error code %d", ErrorCode);
        }
    }
}

//////////////////////////////////////////////////////////////////
//CAnimation ¸Ş¼Òµå
CAnimationData::CAnimationData(int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	Set(x, y, vtype, vframe_max, vdelay_max);
	frame=delay=y_point=0;
	walk=FALSE;
}

//¼³Á¤ : XÅ©±â, YÅ©±â, ¾Ö´Ï¸ŞÀÌ¼ÇÅ¸ÀÔ, ±×¸² ¼ö, Áö¿¬½Ã°£
void CAnimationData::Set(int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	x_size=x;
	y_size=y;
	SetRect(&ani_rect, 0, 0, x_size, y_size);
	type=vtype;
	if(type==2)frame_max=(vframe_max-1)*2;
		else frame_max=vframe_max;
	delay_max=vdelay_max;
}

//¾Ö´Ï¸ŞÀÌ¼Ç Ã³¸®¸¦ ÇØÁÜ(ÀÌ°ÍÀ» ÇØÁÖÁö ¾ÊÀ¸¸é ÀÚµ¿À¸·Î ¾Ö´Ï¸ŞÀÌ¼ÇÀÌ µÇÁö ¾ÊÀ½)
void CAnimationData::Process()
{
	if(type==ani_loop)	//ÀÏÁ÷¼± ¹İº¹ ¾Ö´Ï¸ŞÀÌ¼Ç
	{
		++delay;
		if(delay>=delay_max)
		{
			delay=0;
			++frame;
			if(frame>=frame_max)frame=0;
		}
		
		SetRect(&ani_rect, x_size*frame, y_point, x_size*(frame+1), y_point+y_size);
	}
	else if(type==ani_exchange)	//¿Ô´Ù°¬´Ù ¹İº¹ ¾Ö´Ï¸ŞÀÌ¼Ç
	{
		++delay;
		if(delay>=delay_max)
		{
			delay=0;
			++frame;
			if(frame>=frame_max)frame=0;
		}
		int frame2=abs((frame_max/2)-frame);
		SetRect(&ani_rect, x_size*frame2, y_point, x_size*(frame2+1), y_point+y_size);
	}
	if(type==ani_once)	//ÀÏÁ÷¼± ÇÑ¹ø ¾Ö´Ï¸ŞÀÌ¼Ç
	{
		++delay;
		if(delay>=delay_max)
		{
			delay=0;
			++frame;
			if(frame>=frame_max)frame=frame_max-1;
		}
		
		SetRect(&ani_rect, x_size*frame, y_point, x_size*(frame+1), y_point+y_size);
	}
}

//¾Ö´Ï¸ŞÀÌ¼ÇÀ» ½ÃÀÛÁ¡À¸·Î
void CAnimationData::Reset()
{
	SetFrame(0);
	delay=0;
}

//»óÇÏÁÂ¿ì ¾Ö´Ï¸ŞÀÌ¼ÇÀ» ÇÏ´Â °æ¿ì YÁÂÇ¥¸¦ ¹Ù²ã¼­ ¾Ö´Ï¸ŞÀÌ¼Ç
void CAnimationData::SetWalk(int up_ypos, int down_ypos, int left_ypos, int right_ypos)
{
	up=up_ypos;
	down=down_ypos;
	left=left_ypos;
	right=right_ypos;
	
	walk=TRUE;
}

//Àå¸é Á¤ÇÏ±â
void CAnimationData::SetFrame(int vframe)
{
	frame=vframe;
	SetRect(&ani_rect, x_size*vframe, y_point, x_size*(vframe+1), y_point+y_size);
}

//X,Y °ª °¡Á®¿À±â
int CAnimationData::GetXSize()
{
	return x_size;
}

int CAnimationData::GetYSize()
{
	return y_size;
}

void CAnimationData::SetDirection(WalkDirection dir)
{
	if(!walk)return;

	//¹æÇâ ¼³Á¤
	if(dir==dir_up)y_point=up;
	else if(dir==dir_left)y_point=left;
	else if(dir==dir_down)y_point=down;
	else if(dir==dir_right)y_point=right;
	//¹İ´ë·Î µ¹±â
	else if(dir==dir_turn)
	{
		if(dir==dir_up)y_point=down;
		else if(dir==dir_left)y_point=right;
		else if(dir==dir_down)y_point=up;
		else if(dir==dir_right)y_point=left;
	}
	//¿ŞÂÊÀ¸·Î µ¹±â
	else if(dir==dir_turn_left)
	{
		if(dir==dir_up)y_point=left;
		else if(dir==dir_left)y_point=down;
		else if(dir==dir_down)y_point=right;
		else if(dir==dir_right)y_point=up;
	}
	//¿À¸¥ÂÊÀ¸·Î µ¹±â
	else if(dir==dir_turn_right)
	{
		if(dir==dir_up)y_point=right;
		else if(dir==dir_left)y_point=up;
		else if(dir==dir_down)y_point=left;
		else if(dir==dir_right)y_point=down;
	}
	//¾Æ¹«µ¥³ª º¸±â
	else if(dir==dir_random)
	{
		int wd=rand()%4;
		if(wd==0)y_point=up;
		else if(wd==1)y_point=left;
		else if(wd==2)y_point=down;
		else if(wd==3)y_point=right;
	}

	SetFrame(frame);
}

void CAnimationData::Draw(char* dest, char* src, int x, int y, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPicture(dest, src, x, y, &ani_rect);
}

void CAnimationData::Draw(int dest, int src, int x, int y, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPicture(dest, src, x, y, &ani_rect);
}

void CAnimationData::DrawEx(char* dest, char* src, int x, int y, DWORD dwFlags, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPictureEx(dest, src, x, y, &ani_rect, dwFlags);
}

void CAnimationData::DrawEx(int dest, int src, int x, int y, DWORD dwFlags, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPictureEx(dest, src, x, y, &ani_rect, dwFlags);
}

void CAnimation::CreateAnimation(int id, int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	if(anidata[id] != NULL)delete anidata[id];
	anidata[id]=new CAnimationData(x, y, vtype, vframe_max, vdelay_max);
}

void CAnimation::Process()
{
	//ÀÚµ¿ ¾Ö´Ï¸ŞÀÌ¼Ç
	std::map<int, CAnimationData*>::iterator it;
	for(it=anidata.begin(); it!=anidata.end(); ++it)
	{
		it->second->Process();
	}
}

CAnimationData* CAnimation::GetAni(int id)
{
	if(anidata[id]==NULL)
	{
		OutputDebugString("Àß¸øµÈ ¾Ö´Ï¸ŞÀÌ¼Ç ID¸¦ ¿äÃ»ÇÏ¼Ì½À´Ï´Ù.\n");
		OutputDebugString("¿¡·¯¸¦ ¸·±â À§ÇØ ÀÓ½Ã ¾Ö´Ï¸ŞÀÌ¼Ç Á¤º¸¸¦ »ı¼ºÇØ¼­ º¸³À´Ï´Ù.\n");
		//anidata[id]=new CAnimationData(10, 10, ani_normal, 1, 1);
		anidata[id]=new CAnimationData(640, 480, ani_exchange, 3, 1000);
	}

	return anidata[id];
}

CAnimation::CAnimation()
{
}

CAnimation::~CAnimation()
{
	std::map<int, CAnimationData*>::iterator it;
	for(it=anidata.begin(); it!=anidata.end(); ++it)
	{
		if((it->second) != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	anidata.clear();
}

////////////////////////////////////////////
//±æ Ã£±â ¸Ş¼­µå

CSearchMap::CSearchMap(int* p_map, int x, int y)
{
	//Áöµµ Å©±â Á¤ÇÏ±â
	x_size=x;
	y_size=y;

	//ÀÌµ¿·ÂÀÌ ÀûÈù ¸Ê ÁÖ¼Ò °¡Á®¿À±â
	map=p_map;

	//ÀÌµ¿·ÂÀ» ³ªÅ¸³¾ ¸Ê »ı¼º
	roadmap = new int*[x_size];
	for(int i=0; i<x_size; ++i)
		roadmap[i] = new int[y_size];

	//°Å¸®¸¦ ³ªÅ¸³¾ ¸Ê »ı¼º
	distancemap = new int*[x_size];
	for(int i=0; i<x_size; ++i)
		distancemap[i] = new int[y_size];

	point.x=point.y=0;
}

CSearchMap::~CSearchMap()
{
	//¸Ş¸ğ¸® ¼Ò°Å
	if(roadmap)
	{
		for(int i=0; i<x_size; ++i)
		{
			delete [] roadmap[i];
			roadmap[i]=NULL;
		}
		delete [] roadmap;
		roadmap=NULL;
	}

	if(distancemap)
	{
		for(int i=0; i<x_size; ++i)
		{
			delete [] distancemap[i];
			distancemap[i]=NULL;
		}
		delete [] distancemap;
		distancemap=NULL;
	}
}

void CSearchMap::SetStartPoint(MapPoint p, int move_max)
{
	SetStartPoint(p.x, p.y, move_max);
}

void CSearchMap::SetStartPoint(int x, int y, int move_max)
{
	point.x=MaxMin(x, 0, x_size-1);
	point.y=MaxMin(y, 0, y_size-1);

	//¸Ê ÃÊ±âÈ­
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			roadmap[i][j]=MOVE_DISABLE;

	//±æ Ã£±â
	FindRoad(roadmap, point.x, point.y, 0, move_max);
}

void CSearchMap::SetEndPoint(MapPoint p)
{
	SetEndPoint(p.x, p.y);
}

void CSearchMap::SetEndPoint(int x, int y)
{
	//¸Ê ÃÊ±âÈ­
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			distancemap[i][j]=MOVE_DISABLE;

	//ÀÌµ¿ºñ¿ë °Ë»ç
	FindRoad(distancemap, x, y, 0);
}

void CSearchMap::FindRoad(int** vmap, int x, int y, int expense, int move_max)
{
	vmap[x][y]=expense;
	if(move_max>=0 && expense>move_max)return;	//ÀÌµ¿·Â ÇÑ°èÀÏ ¶§´Â ´õ ÀÌ»ó °Ë»çÇÏÁö ¾ÊÀ½

	if(x>0 && vmap[x-1][y]>expense+map[(x-1)+y*x_size])
		FindRoad(vmap, x-1, y, expense+map[(x-1)+y*x_size], move_max);	//¿ŞÂÊ °Ë»ç
	if(x<x_size-1 && vmap[x+1][y]>expense+map[(x+1)+y*x_size])
		FindRoad(vmap, x+1, y, expense+map[(x+1)+y*x_size], move_max);	//¿À¸¥ÂÊ °Ë»ç
	if(y>0 && vmap[x][y-1]>expense+map[x+(y-1)*x_size])
		FindRoad(vmap, x, y-1, expense+map[x+(y-1)*x_size], move_max);	//À§ °Ë»ç
	if(y<y_size-1 && vmap[x][y+1]>expense+map[x+(y+1)*x_size])
		FindRoad(vmap, x, y+1, expense+map[x+(y+1)*x_size], move_max);	//¾Æ·¡ °Ë»ç
}

BOOL CSearchMap::MoveNextPointFast(int move)
{
	//°¡Àå ÁÁÀº ±æÀ» Ã£±â
	int best_dir=-1, best_expense=distancemap[point.x][point.y];

	//¿ŞÂÊ °Ë»ç
	if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
	{
		best_dir=0;
		best_expense=distancemap[point.x-1][point.y];
	}
	//¿À¸¥ÂÊ °Ë»ç
	if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
	{
		best_dir=1;
		best_expense=distancemap[point.x+1][point.y];
	}
	//À§ °Ë»ç
	if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
	{
		best_dir=2;
		best_expense=distancemap[point.x][point.y-1];
	}
	//¾Æ·¡ °Ë»ç
	if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
	{
		best_dir=3;
		best_expense=distancemap[point.x][point.y+1];
	}

	//°¡Àå °¡±î¿î °÷À¸·Î ÀÌµ¿(¾Æ´Ï¸é °¡¸¸È÷ ÀÖÀ½)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//¸ñÀûÁö¿¡ µµÂøÇÏ¸é TRUE, ¾Æ´Ï¸é FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

BOOL CSearchMap::MoveNextPoint(int move)
{
	//°¡Àå ÁÁÀº ±æÀ» Ã£±â
	int best_dir=-1, best_expense=distancemap[point.x][point.y];

	//Ã¹Â°·Î °Ë»çÇÏ´Â °÷À» ·£´ıÇÏ°Ô Á¤ÇÏ±â À§ÇÑ ÇÃ·¡±×
	//(¿ì¼±¼øÀ§°¡ Á¤ÇØÁ® ÀÖÀ¸¸é ÆĞÅÏÀÌ ´Ü¼øÇØ¼­ º¸±â¿¡ ¾È ÁÁ´Ù.)
	BOOL left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;

	while(!left_ok || !right_ok || !up_ok || !down_ok)
	{
		int ran=rand()%4;

		//¿ŞÂÊ °Ë»ç
		if(ran==0)
		{
			if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
			{
				best_dir=0;
				best_expense=distancemap[point.x-1][point.y];
			}
			left_ok=TRUE;
		}
		//¿À¸¥ÂÊ °Ë»ç
		else if(ran==1)
		{
			if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
			{
				best_dir=1;
				best_expense=distancemap[point.x+1][point.y];
			}
			right_ok=TRUE;
		}
		//À§ °Ë»ç
		else if(ran==2)
		{
			if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
			{
				best_dir=2;
				best_expense=distancemap[point.x][point.y-1];
			}
			up_ok=TRUE;
		}
		//¾Æ·¡ °Ë»ç
		else if(ran==3)
		{
			if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
			{
				best_dir=3;
				best_expense=distancemap[point.x][point.y+1];
			}
			down_ok=TRUE;
		}
	}

	//°¡Àå °¡±î¿î °÷À¸·Î ÀÌµ¿(¾Æ´Ï¸é °¡¸¸È÷ ÀÖÀ½)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//¸ñÀûÁö¿¡ µµÂøÇÏ¸é TRUE, ¾Æ´Ï¸é FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

BOOL CSearchMap::MoveNextPointStupid(int move, int iq)
{
	iq=MaxMin(iq, 1, 10);	//¾ÆÀÌÅ¥´Â 1~10±îÁö

	//°¡Àå ÁÁÀº ±æÀ» Ã£±â
	int best_dir=-1, best_expense=distancemap[point.x][point.y]+(10-iq);

	//¸ÛÃ»ÇÏ±â ¶§¹®¿¡ ¸ğµÎ °Ë»çÇÏÁö ¾Ê°í ¸î°³¸¦ ·£´ıÀ¸·Î ½ÃµµÇØ º» µÚ ±× Áß¿¡¼­ ÀÌµ¿
	BOOL left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;
	int count=0;

	while(count<iq && (!left_ok || !right_ok || !up_ok || !down_ok))
	{
		int ran=rand()%4;

		//¿ŞÂÊ °Ë»ç
		if(ran==0)
		{
			if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
			{
				best_dir=0;
				best_expense=distancemap[point.x-1][point.y];
			}
			left_ok=TRUE;
		}
		//¿À¸¥ÂÊ °Ë»ç
		else if(ran==1)
		{
			if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
			{
				best_dir=1;
				best_expense=distancemap[point.x+1][point.y];
			}
			right_ok=TRUE;
		}
		//À§ °Ë»ç
		else if(ran==2)
		{
			if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
			{
				best_dir=2;
				best_expense=distancemap[point.x][point.y-1];
			}
			up_ok=TRUE;
		}
		//¾Æ·¡ °Ë»ç
		else if(ran==3)
		{
			if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
			{
				best_dir=3;
				best_expense=distancemap[point.x][point.y+1];
			}
			down_ok=TRUE;
		}

		++count;
	}

	//°¡Àå °¡±î¿î °÷À¸·Î ÀÌµ¿(¾Æ´Ï¸é °¡¸¸È÷ ÀÖÀ½)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//¸ñÀûÁö¿¡ µµÂøÇÏ¸é TRUE, ¾Æ´Ï¸é FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

int CSearchMap::GetRoadMap(int x, int y)
{
	return roadmap[x][y];
}

int CSearchMap::GetDistanceMap(int x, int y)
{
	return distancemap[x][y];
}

int CSearchMap::GetPointX()
{
	return point.x;
}

int CSearchMap::GetPointY()
{
	return point.y;
}

///////////////////////////////////////////////////////////////////////////////
//Direct Music
CDirectMusic::CDirectMusic()
{
	g_pMusicManager          = NULL;
	g_pMusicSegment          = NULL;
	g_hInst                  = NULL;
	g_hDMusicMessageEvent    = NULL;
}

CDirectMusic::~CDirectMusic()
{
    SAFE_DELETE( g_pMusicSegment );
    SAFE_DELETE( g_pMusicManager );
    CloseHandle( g_hDMusicMessageEvent );
}

HRESULT CDirectMusic::InitDirectMusic()
{
    HRESULT hr; 

    g_hDMusicMessageEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
    g_pMusicManager = new CMusicManager();
    if( NULL == g_pMusicManager )
        return E_OUTOFMEMORY;

    if( FAILED( hr = g_pMusicManager->Initialize( hwnd ) ) )
        return DXTRACE_ERR_MSGBOX( TEXT("Initialize"), hr );

    // Register segment notification
    IDirectMusicPerformance* pPerf = g_pMusicManager->GetPerformance();
    GUID guid = GUID_NOTIFICATION_SEGMENT;
    pPerf->AddNotificationType( guid );
    pPerf->SetNotificationHandle( g_hDMusicMessageEvent, 0 );  

    return S_OK;
}

HRESULT CDirectMusic::LoadSegmentFile(TCHAR* strFileName)
{
    HRESULT hr;

    // Free any previous segment, and make a new one
    SAFE_DELETE( g_pMusicSegment );

    // Have the loader collect any garbage now that the old 
    // segment has been released
    g_pMusicManager->CollectGarbage();

    // Set the media path based on the file name (something like C:\MEDIA)
    // to be used as the search directory for finding DirectMusic content
    // related to this file.
    TCHAR strMediaPath[MAX_PATH];
    _tcsncpy( strMediaPath, strFileName, MAX_PATH );
    strMediaPath[MAX_PATH-1] = 0;
    TCHAR* strLastSlash = _tcsrchr(strMediaPath, TEXT('\\'));
    if( strLastSlash )
    {
        *strLastSlash = 0;
        if( FAILED( hr = g_pMusicManager->SetSearchDirectory( strMediaPath ) ) )
            return DXTRACE_ERR_MSGBOX( TEXT("SetSearchDirectory"), hr );
    }

    // For DirectMusic must know if the file is a standard MIDI file or not
    // in order to load the correct instruments.
    BOOL bMidiFile = FALSE;
    if( strstr( strFileName, ".mid" ) != NULL ||
        strstr( strFileName, ".rmi" ) != NULL ) 
    {
        bMidiFile = TRUE;
    }

    BOOL bWavFile = FALSE;
    if( strstr( strFileName, ".wav" ) != NULL )
    {
        bWavFile = TRUE;
    }        

    // Load the file into a DirectMusic segment 
    if( FAILED( g_pMusicManager->CreateSegmentFromFile( &g_pMusicSegment, strFileName, 
                                                        TRUE, bMidiFile ) ) )
    {
        // Not a critical failure, so just update the status
        return S_FALSE; 
    }

    return S_OK;
}

HRESULT CDirectMusic::ProcessDirectMusicMessages()
{
    HRESULT hr;
    IDirectMusicPerformance8* pPerf = NULL;
    DMUS_NOTIFICATION_PMSG* pPMsg;
        
    if( NULL == g_pMusicManager )
        return S_OK;

    pPerf = g_pMusicManager->GetPerformance();

    // Get waiting notification message from the performance
    while( S_OK == pPerf->GetNotificationPMsg( &pPMsg ) )
    {
        switch( pPMsg->dwNotificationOption )
        {
        case DMUS_NOTIFICATION_SEGEND:
            if( pPMsg->punkUser )
            {
                IDirectMusicSegmentState8* pSegmentState   = NULL;
                IDirectMusicSegment*       pNotifySegment   = NULL;
                IDirectMusicSegment8*      pNotifySegment8  = NULL;
                IDirectMusicSegment8*      pPrimarySegment8 = NULL;

                // The pPMsg->punkUser contains a IDirectMusicSegmentState8, 
                // which we can query for the segment that the SegmentState refers to.
                if( FAILED( hr = pPMsg->punkUser->QueryInterface( IID_IDirectMusicSegmentState8,
                                                                  (VOID**) &pSegmentState ) ) )
                    return DXTRACE_ERR_MSGBOX( TEXT("QueryInterface"), hr );

                if( FAILED( hr = pSegmentState->GetSegment( &pNotifySegment ) ) )
                {
                    // Sometimes the segend arrives after the segment is gone
                    // This can happen when you load another segment as 
                    // a motif or the segment is ending
                    if( hr == DMUS_E_NOT_FOUND )
                    {
                        SAFE_RELEASE( pSegmentState );
                        return S_OK;
                    }

                    return DXTRACE_ERR_MSGBOX( TEXT("GetSegment"), hr );
                }

                if( FAILED( hr = pNotifySegment->QueryInterface( IID_IDirectMusicSegment8,
                                                                 (VOID**) &pNotifySegment8 ) ) )
                    return DXTRACE_ERR_MSGBOX( TEXT("QueryInterface"), hr );

                // Get the IDirectMusicSegment for the primary segment
                pPrimarySegment8 = g_pMusicSegment->GetSegment();

                // Cleanup
                SAFE_RELEASE( pSegmentState );
                SAFE_RELEASE( pNotifySegment );
                SAFE_RELEASE( pNotifySegment8 );
            }
            break;
        }

        pPerf->FreePMsg( (DMUS_PMSG*)pPMsg ); 
    }

    return S_OK;
}

HRESULT CDirectMusic::PlayAudio()
{
    HRESULT hr;

    // Set the segment to repeat many times
	if(g_pMusicSegment != NULL)
	{
		if( FAILED( hr = g_pMusicSegment->SetRepeats( DMUS_SEG_REPEAT_INFINITE ) ) )
			return DXTRACE_ERR_MSGBOX( TEXT("SetRepeats"), hr );

		// Play the segment and wait. The DMUS_SEGF_BEAT indicates to play on the 
		// next beat if there is a segment currently playing. 
		if( FAILED( hr = g_pMusicSegment->Play( DMUS_SEGF_BEAT ) ) )
			return DXTRACE_ERR_MSGBOX( TEXT("Play"), hr );

		return S_OK;
	}
	else return S_FALSE;	
}

void CDirectMusic::StopAudio()
{
    if( g_pMusicManager )
        g_pMusicManager->StopAll();
}

////////////////////////////////////////////////////////////
//ÃÊ±âÈ­ ÀÛ¾÷(¸Å¹ø ¹Ù²îÁö ¾ÊÀ¸´Ï±î main¿¡¼­ donglib·Î ¿Å°åÀ½
BOOL MainInitialize(char* window_name, BOOL use_keyboard, BOOL use_mouse, BOOL window_mode)
{
	jdd=CreateDirectDraw();
	jre=CreateDXResourceManager(jdd);

	HINSTANCE hInstance=(HINSTANCE)0x00400000;

	WNDCLASS wc={0};
	wc.hIcon=LoadIcon(hInstance,"ICON.ico");
	wc.hCursor=LoadCursor(hInstance,IDC_ARROW);
	wc.lpfnWndProc=WndProc;
	wc.hInstance=hInstance;
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName="Game";
	RegisterClass(&wc);
	
	if(window_mode)
	{
		LONG ws=WS_OVERLAPPEDWINDOW|WS_VISIBLE;
		ws &= ~WS_THICKFRAME;
		ws &= ~WS_MAXIMIZEBOX;

		RECT crt;
		SetRect(&crt, 0, 0, SCREEN_X, SCREEN_Y);
		AdjustWindowRect(&crt, ws, FALSE);

		hwnd = CreateWindow("Game", window_name, ws, CW_USEDEFAULT, CW_USEDEFAULT, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);
	    ShowCursor( TRUE );
	}
	else
	{
		hwnd=CreateWindow("Game",window_name,WS_POPUP|WS_VISIBLE,0,0,SCREEN_X,SCREEN_Y,NULL,NULL,hInstance,NULL);
	    ShowCursor( FALSE );
	}

	jdd->Initialize(NULL,hwnd,SCREEN_X,SCREEN_Y,16,TRUE,window_mode);

	//±×·¡ÇÈ ÃÊ±âÈ­
	backbuffer=jdd->GetBackBuffer();
	global_font=jdd->CreateFont("±Ã¼­",20);
	//·Îµù¿¡ ±ä ½Ã°£ÀÌ °É¸± ¶§¸¦ ´ëºñÇÑ È­¸é
	if(!(jdd->LoadPicture("CommonLoading", "Loading.jpg", NULL, TRUE)))jdd->LoadPicture("CommonLoading", "Loading.gif", NULL, TRUE);
	jdd->DrawPicture(backbuffer, "CommonLoading", 0, 0, NULL);
	jdd->Render();

	//ÀÓ½Ã ¼­ÆäÀÌ½º »ı¼º
	JPictureInfo jpi;
	jpi.SetWidth(SCREEN_X);
	jpi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonBlank", &jpi, TRUE);
	
	jdd->SetFrameRate(100,TRUE);
	jdd->SetVerticalSync(FALSE);

	//»ç¿îµå ÃÊ±âÈ­
	if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
	{
		SoundCard = TRUE;
		if (SoundOBJ->SetCooperativeLevel(hwnd,DSSCL_PRIORITY)!=DS_OK) return FALSE;

		memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
		DSB_desc.dwSize = sizeof(DSBUFFERDESC);
		DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

		if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK) return FALSE;
		SoundDSB -> SetVolume(0);
		SoundDSB -> SetPan(0);
	}
	else SoundCard = FALSE;

	//±âÅ¸ ÃÊ±âÈ­
	srand( (unsigned)time( NULL ) );
	keyboard_control=use_keyboard;
	mouse_control=use_mouse;
	gameover=FALSE;

	return TRUE;
}
