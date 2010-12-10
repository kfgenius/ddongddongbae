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

//����
LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

BOOL SoundCard;
BOOL ReplayFlag;

BOOL mouse_control;	//���콺 ��� ����
BOOL keyboard_control;	//Ű���� ��� ����

int MouseX, MouseY;	//���콺 ��ǥ
BOOL LButton=FALSE, RButton=FALSE;	//���콺 Ŭ������
BOOL left_click=FALSE, right_click=FALSE;	//���콺 Ŭ���� �ѹ��� �˻��ϱ� ���� ����

//�ѱ� ó��
CHan m_han;
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
    if ( SoundCard ) SndObjPlay( sound, NULL );
}

//������ ���
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

//�ٻ簪, �����̽��� ����� ����ϴ� ����̶�� �� �����̾���
// int GetCharLength(char tmp_char)
// {
// 	if(tmp_char>=0x41 && tmp_char<=0x5a)return 15;
// 	if(tmp_char=='~')return 16;
// 
// 	return 10;
// }

//���ڿ� ó�� �Լ�
char str_buffer[3][1024];	//����

char* StrAdd(char* msg, ...)
{
	static int buffer_id;		//���� ����ϴ� ����(3���� ���۸� ���ư��鼭 �Ἥ ���� 3��ó�� �����ϰ� ��)
	if(++buffer_id>=3)buffer_id=0;
	char* use_buffer=str_buffer[buffer_id];
	size_t buffer_pointer=0;	//���� ���ۿ� ���� �ִ� ��ġ

	strcpy(use_buffer,"");
	va_list ap;
	va_start(ap,msg);
	size_t max=strlen(msg);
	for(size_t i=0; i<max; ++i)
	{
		//���� ���� �ֱ�
		if(msg[i]=='%' && i<max-1)
		{
			//����
			if(msg[i+1]=='s')
			{
				use_buffer[buffer_pointer]=NULL;
				strcat(use_buffer,va_arg(ap,char*));
				buffer_pointer=strlen(use_buffer);
				++i;
			}
			//����
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
		//�׳� ����
		else
		{
			use_buffer[buffer_pointer]=msg[i];			
			++buffer_pointer;
		}
	}
	use_buffer[buffer_pointer]=NULL;

	return use_buffer;
}

//��� ����
void Pause()
{
	while(1)
	{
		if(!ProcessMessage())break;		//������ �޼��� ó��
        //���콺�� Ű������ Esc�� ���͸� ������ ��� ����
		if(keyboard_control && (GetKey(vkey_esc) || GetKey(vkey_enter)) || mouse_control && LeftDown())break;
	}
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

//////////////////////////////////////////////////////////
//�ѱ� Ŭ���� �޼ҵ�
#define HAN_MAX 2364
#define	MOUM	19

char* han1[]={"��","rk","��","rkr","�A","rkR","��","rks","��","rke","��","rkf","��","rkfr","��","rkfa","�E","rkfq","�F","rkft",
			"�G","rkfx","�H","rkfv","�I","rkfg","��","rka","��","rkq","��","rkqt","��","rkt","��","rkT","��","rkd","��","rkw",
			"��","rkc","��","rkx","��","rkv","��","rkg","��","ro","��","ror","��","ros","��","roe","��","roa","��","roq",
			"��","rot","��","roT","��","rod","��","ri","��","rir","��","ris","��","rif","��","rit","��","rid","��","rO",
			"��","rOs","��","rOf","��","rj","��","rjr","��","rjs","��","rje","��","rjf","��","rjfa","��","rja","��","rjq",
			"��","rjt","��","rjT","��","rjd","��","rjw","��","rjx","��","rjv","��","rjg","��","rp","��","rps","��","rpf",
			"��","rpa","��","rpq","��","rpt","��","rpT","��","rpd","��","ru","��","rur","��","rur","��","rus","��","rue",
			"��","ruf","��","rua","��","ruq","��","rut","��","ruT","��","rud","��","rux","��","rP","��","rPs","��","rPf",
			"��","rPq","��","rPt","��","rh","��","rhr","��","rhs","��","rhe","��","rhf","��","rhfa","��","rhft","��","rhfg",
			"��","rha","��","rhq","��","rht","��","rhd","��","rhw","��","rhk","��","rhkr","��","rhks","��","rhkf","��","rhkfa",
			"��","rhka","��","rhkq","��","rhkt","��","rhkd","��","rho","��","rhos","��","rhof","��","rhoq","��","rhoT","��","rhod",
			"��","rhl","��","rhlr","��","rhls","��","rhlf","��","rhla","��","rhlq","��","rhlt","��","rhld","��","ry","��","rys",
			"��","ryf","��","ryq","��","ryt","��","rn","��","rnr","��","rns","��","rne","��","rnf","��","rnfr","��","rnfa",
			"��","rnfg","��","rna","��","rnq","��","rnt","��","rnd","��","rnw","��","rnj","��","rnjr","��","rnjs","��","rnjf",
			"��","rnjT","��","rnjd","��","rnp","��","rnpt","��","rnl","��","rnlr","��","rnls","��","rnlf","��","rnla","��","rnlq",
			"��","rnlt","��","rb","��","rbs","��","rbf","��","rm","��","rmr","��","rms","��","rme","��","rmf","��","rmfr",
			"��","rma","��","rmq","��","rmt","��","rmd","��","rml","��","rl","��","rlr","��","rls","��","rle","��","rlf",
			"��","rlfa","��","rla","��","rlq","��","rlt","��","rld","��","rlw","��","rlv","��","Rk","��","Rkr","��","RkR",
			"��","Rks","��","Rkf","��","Rkfa","��","Rka","��","Rkq","��","Rkt","��","RkT","��","Rkd","��","Rkx","��","Ro",
			"��","Ror","��","Ros","��","Rof","��","Roa","��","Roq","��","Rot","��","RoT","��","Rod","��","Ri","��","Rir",
			"��","Rif","��","Rj","��","Rjr","��","RjR","��","Rjs","��","Rjf","��","Rja","��","Rjq","��","Rjt","��","RjT",
			"��","Rjd","��","Rp","��","Rpr","��","RpR","��","Rps","��","Rpa","��","Rpt","��","Rpd","��","Ru","��","Rus",
			"��","Ruf","��","Rut","��","RuT","��","Rux","��","RP","��","Rh","��","Rhr","��","Rhs","��","Rha","��","Rhsg",
			"��","Rhf","��","Rha","��","Rhq","��","Rht","��","Rhd","��","Rhw","��","Rhc","��","Rhk","��","Rhkr","��","Rhkf",
			"��","RhkT","��","Rhkd","��","Rho","��","Rhor","��","Rhod","��","Rhl","��","Rhls","��","RHlf","��","Rhla","��","Rhlq",
			"��","Rhld","��","Ry","��","Rn","��","Rnr","��","Rns","��","Rnf","��","Rnfg","��","Rna","��","Rnq","��","Rnt",
			"��","Rnd","��","Rnw","��","Rnj","��","Rnjf","��","RnjT","��","Rnjd","��","Rnp","��","Rnpr","��","Rnps","��","Rnpf",
			"��","Rnpa","��","Rnpq","��","RnpT","��","Rnl","��","Rnls","��","Rnlf","��","Rnla","��","Rnlq","��","Rb","��","Rm",
			"��","Rmr","��","Rms","��","Rmsg","��","Rmf","��","Rmfa","��","Rmfg","��","Rma","��","Rmq","��","Rmt","��","Rmd",
			"��","Rmx","��","Rl","��","Rlr","��","Rls","��","Rlf","��","Rla","��","Rlq","��","Rlt","��","Rld","��","sk",
			"��","skr","��","skR","��","sks","��","ske","��","skf","��","skfr","��","skfa","��","ska","��","skq","��","skt",
			"��","skT","��","skd","��","skw","��","skc","��","skx","��","skg","��","so","��","sor","��","sos","��","sof",
			"��","soa","��","soq","��","sot","��","soT","��","sod","��","si","��","sir","��","sis","��","sif","��","sia",
			"��","sid","��","sj","��","sjr","��","sjt","��","sjs","��","sjf","��","sjfa","��","sjfq","��","sja","��","sjq",
			"��","sjt","��","sjT","��","sjd","��","sjg","��","sp","��","spr","��","sps","��","spf","��","spa","��","spq",
			"��","spt","��","spT","��","spd","��","su","��","sur","��","sus","��","suf","��","sua","��","suq","��","suT",
			"��","sud","��","suz","��","sP","��","sPs","��","sh","��","shr","��","shs","��","shf","��","shfa","��","sha",
			"��","shq","��","sht","��","shd","��","shv","��","shg","��","shk","��","shks","��","shkf","��","shkT","��","shl",
			"��","shls","��","shlf","��","shla","��","shlq","��","shlt","��","sy","��","syr","��","sys","��","syf","��","syq",
			"��","syt","��","syd","��","sn","��","snr","��","sns","��","sne","��","snf","��","sna","��","snq","��","snt",
			"��","snd","��","snj","��","snjT","��","snp","��","snl","��","snls","��","snlf","��","snla","��","snlq","��","sb",
			"��","sbr","��","sbf","��","sba","��","sbq","��","sbd","��","sm","��","smr","��","sms","��","smf","��","smfr",
			"��","smfa","��","sma","��","smq","��","smt","��","smd","��","smw","��","smv","��","sml","��","smls","��","smlf",
			"��","sl","��","slr","��","sls","��","slf","��","slfa","��","sla","��","slq","��","slt","��","sld","��","slv",
			"��","ek","��","ekr","��","ekR","��","eks","��","eke","��","ekf","��","ekfr","��","ekfa","��","ekfq","��","ekfg",
			"��","eka","��","ekq","��","ekt","��","ekT","��","ekd","��","ekw","��","ekc","��","ekg","��","eo","��","eor",
			"��","eos","��","eof","��","eoa","��","eoq","��","eot","��","eoT","��","eod","��","ei","��","ej","��","ejr",
			"��","ejR","��","ejs","��","eje","��","ejf","��","ejfa","��","ejfq","��","eja","��","ejq","��","ejt","��","ejd",
			"��","ejc","��","ejv","��","ep","��","epr","��","eps","��","epf","��","epa","��","epq","��","ept","��","epT",
			"��","epd","��","eu","��","eus","��","euf","��","euT","��","eud","��","eP","��","ePs","��","eh","��","ehr",
			"��","ehs","��","ehe","��","ehf","��","ehfa","��","ehft","��","eha","��","ehq","��","eht","��","ehd","��","ehc",
			"��","ehx","��","ehk","��","ehks","��","ehkf","��","eho","��","ehoT","��","ehl","��","ehls","��","ehlf","��","ehla",
			"��","ehlq","��","ehlt","��","ey","��","en","��","enr","��","ens","��","enf","��","ena","��","enq","��","ent",
			"��","end","��","enj","��","enjT","��","enp","��","enpd","��","enl","��","enls","��","enlf","��","enlq","��","enlt",
			"��","enld","��","eb","��","ens","��","enf","��","ena","��","end","��","em","��","emr","��","ems","��","eme",
			"��","emf","��","emfa","��","ema","��","emq","��","emt","��","emd","��","eml","��","el","��","elr","��","els",
			"��","ele","��","elf","��","ela","��","elq","��","elt","��","elT","��","eld","��","elw","��","Ek","��","Ekr",
			"��","Eks","��","Ekf","��","Eka","��","Ekq","��","Ekt","��","EkT","��","Ekd","��","Ekg","��","Eo","��","Eor",
			"��","Eos","��","Eof","��","Eoa","��","Eoq","��","Eot","��","EoT","��","Eod","��","Ej","��","Ejr","��","Ejs",
			"��","Ejf","��","Ejfa","��","Ejfq","��","Eja","��","Ejq","��","Ejt","��","EjT","��","Ejd","��","Ejg","��","Ep",
			"��","Epr","��","Eps","��","Epf","��","Epa","��","Epq","��","Ept","��","EpT","��","Epd","��","Eu","��","EuT",
			"��","Eh","��","Ehr","��","Ehs","��","Ehf","��","Ehd","��","Ehk","��","Ehkf","��","Eho","��","Ehl","��","Ehls",
			"��","En","��","Enr","��","Ens","��","Enf","��","Enfg","��","Ena","��","End","��","Enp","��","Enl","��","Enls",
			"��","Enlf","��","Enla","��","Enlq","��","Enld","��","Em","��","Emr","��","Ems","��","Eme","��","Emf","��","Ema",
			"��","Emq","��","Emt","��","Eml","��","Emls","��","Emlf","��","Emla","��","Emlq","��","El","��","Els","��","Elf",
			"��","Ela","��","Elq","��","Elt","��","Eld","��","fk","��","fkr","��","fks","��","fkf","��","fka","��","fkq",
			"��","fkt","��","fkT","��","fkd","��","fkw","��","fkv","��","fkg","��","fo","��","for","��","fos","��","fof",
			"��","foa","��","foq","��","fot","��","foT","��","fod","��","fi","��","fir","��","fis","��","fit","��","fid",
			"��","fj","��","fjr","��","fjs","��","fjf","��","fja","��","fjq","��","fjt","��","fjT","��","fjd","��","fjg",
			"��","fp","��","fpr","��","fps","��","fpf","��","fpa","��","fpq","��","fpt","��","fpd","��","fu","��","fur",
			"��","fus","��","fuf","��","fua","��","fuq","��","fut","��","fuT","��","fud","��","fP","��","fPs","��","fPq",
			"��","fPt","��","fh","��","fhr","��","fhs","��","fhf","��","fha","��","fhq","��","fht","��","fhd","��","fhk",
			"��","fhks","��","fhkd","��","fhoT","��","fhl","��","fhls","��","fhlf","��","fhla","��","fhlq","��","fhlt","��","fhld",
			"��","fy","��","fys","��","fyf","��","fyq","��","fyt","��","fyd","��","fn","��","fnr","��","fns","��","fnf",
			"��","fna","��","fnq","��","fnt","��","fnd","��","fnj","��","fnjT","��","fnp","��","fnl","��","fnlr","��","fnls",
			"��","fnlf","��","fnla","��","fnlt","��","fnld","��","fb","��","fbr","��","fbs","��","fbf","��","fba","��","fbq",
			"��","fbt","��","fbd","��","fm","��","fmr","��","fms","��","fmf","��","fma","��","fmq","��","fmt","��","fmd",
			"��","fmw","��","fmx","��","fmv","��","fl","��","flr","��","fls","��","flf","��","fla","��","flq","��","flt",
			"��","fld","��","ak","��","akr","��","aks","��","aksg","��","ake","��","akf","��","akfr","��","aka","��","aka",
			"��","akq","��","akt","��","akd","��","akw","��","akx","��","akg","��","ao","��","aor","��","aos","��","aof","��","aoa",
			"��","aoq","��","aot","��","aoT","��","aod","��","aow","��","ai","��","air","��","aif","��","aid","��","aj","��","ajr",
			"��","ajs","��","ajf","��","ajfa","��","aja","��","ajq","��","ajt","��","ajd","��","ajw","��","ajg","��","ap","��","apr",
			"��","aps","��","apf","��","apa","��","apq","��","apt","��","apT","��","apd","��","au","��","aur","��","aus",
			"��","auf","��","aut","��","auT","��","aud","��","auc","��","aP","��","ah","��","ahr","��","ahrt","��","ahs","��","ahf",
			"��","ahfa","��","aha","��","ahq","��","aht","��","ahd","��","ahk","��","ahks","��","ahkT","��","ahkd","��","ahl","��","ahls",
			"��","ahlf","��","ahlq","��","ahlt","��","ahld","��","ay","��","ays","��","ayf","��","ayq","��","ayt","��","an","��","anr",
			"��","anR","��","ans","��","ane","��","anf","��","anfr","��","anfa","��","ana","��","anq","��","ant","��","and",
			"��","anx","��","ang","��","anj","��","anjs","��","anjf","��","anjq","��","anjt","��","anp","��","anl","��","anls","��","anlf",
			"��","ab","��","abs","��","abf","��","aba","��","abt","��","am","��","ams","��","amf","��","ama","��","amt","��","al",
			"��","alr","��","als","��","ale","��","alf","��","alfa","��","ala","��","alq","��","alt","��","alT","��","ald","��","alC",
			"��","alx","��","qk","��","qkr","��","qkR","��","qkrt","��","qks","��","qke","��","qkf","��","qkfr","��","qkfa",
			"��","qkfq","��","qka","��","qkq","��","qkt","��","qkd","��","qkx","��","qo","��","qor","��","qos","��","qof","��","qoa",
			"��","qoq","��","qot","��","qoT","��","qod","��","qox","��","qi","��","qir","��","qis","��","qiq","��","qj","��","qjr",
			"��","qjs","��","qje","��","qjf","��","qjfa","��","qja","��","qjq","��","qjt","��","qjd","��","qjw","��","qp","��","qpr",
			"��","qps","��","qpe","��","qpf","��","qpa","��","qpq","��","qpt","��","qpT","��","qpd","��","qu","��","qur",
			"��","qus","��","quf","��","quq","��","qut","��","quT","��","qud","��","qux","��","qP","��","qPs","��","qh","��","qhr",
			"��","qhR","��","qhs","��","qhf","��","qha","��","qhq","��","qht","��","qhd","��","qhk","��","qhks","��","qhkT","��","qho",
			"��","qhoT","��","qhl","��","qhlr","��","qhls","��","qhlf","��","qhla","��","qhlq","��","qy","��","qys","��","qn","��","qnr",
			"��","qns","��","qne","��","qnf","��","qnfr","��","qnfa","��","qna","��","qnq","��","qnt","��","qnd","��","qnx",
			"��","qnv","��","qnj","��","qnjf","��","qnjT","��","qnp","��","qnpf","��","qnpfr","��","qnl","��","qnlr","��","qnls","��","qnlf","��","qnld","��","qb",
			"��","qbs","��","qbf","��","qba","��","qbt","��","qbd","��","qm","��","qmr","��","qms","��","qmf","��","qma","��","qmq",
			"��","qmt","��","ql","��","qlr","��","qls","��","qlf","��","qlfa","��","qla","��","qlq","��","qlt","��","qld","��","qlw",
			"��","qlc","��","Qk","��","Qkr","��","Qks","��","Qkf","��","Qkfa","��","Qka","��","Qkq","��","Qkt","��","QkT",
			"��","Qkd","��","Qkg","��","Qo","��","Qor","��","Qos","��","Qof","��","Qoa","��","Qoq","��","Qot","��","QoT","��","Qod",
			"��","Qi","��","Qir","��","Qia","��","Qj","��","Qjr","��","Qjs","��","Qje","��","Qjf","��","Qja","��","Qjt","��","QjT",
			"��","Qjd","��","Qp","��","Qpd","��","Qu","��","Qur","��","Qua","��","Quq","��","Qut","��","QuT","��","Qud","��","Qh",
			"��","Qhr","��","Qhs","��","Qhf","��","Qha","��","Qhq","��","Qhd","��","Qhl","��","Qy","��","Qyd","��","Qn",
			"��","Qnr","��","Qns","��","Qnf","��","Qna","��","Qnt","��","Qnd","��","Qb","��","Qbd","��","Qm","��","Qms","��","Qmf",
			"��","Qma","��","Qmq","��","Ql","��","Qlr","��","Qls","��","Qlf","��","Qla","��","Qlq","��","Qlt","��","Qld","��","tk",
			"��","tkr","��","tkrt","��","tks","��","tke","��","tkf","��","tkfr","��","tkfa","��","tka","��","tkq","��","tkt","��","tkT",
			"��","tkd","��","tkx","��","to","��","tor","��","tos","��","tof","��","toa","��","toq","��","tot","��","toT",
			"��","tod","��","ti","��","tir","��","tis","��","tif","��","tia","��","tiq","��","tit","��","tid","��","tO",
			"��","tOs","��","tOf","��","tOa","��","tOd","��","tj","��","tjr","��","tjR","��","tjrt","��","tjs","��","tje",
			"��","tjf","��","tjfa","��","tjfq","��","tja","��","tjq","��","tjt","��","tjT","��","tjd","��","tjv","��","tp",
			"��","tpR","��","tps","��","tpf","��","tpa","��","tpq","��","tpt","��","tpT","��","tpd","��","tu","��","tur",
			"��","tus","��","tuf","��","tua","��","tuq","��","tut","��","tuT","��","tud","��","tP","��","tPs","��","tPf",
			"��","tPd","��","th","��","thr","��","thR","��","ths","��","thf","��","thfa","��","tha","��","thq","��","tht",
			"��","thd","��","thx","��","thk","��","thkr","��","thks","��","thkf","��","thkd","��","tho","��","thos","��","thof",
			"��","thoa","��","thot","��","thoT","��","thl","��","thls","��","thlf","��","thla","��","thlq","��","thlt","��","ty",
			"��","tyr","��","tys","��","tyf","��","tya","��","tyq","��","tyt","��","tyd","��","tn","��","tnr","��","tns",
			"��","tne","��","tnf","��","tna","��","tnq","��","tnt","��","tnd","��","tnc","��","tnx","��","tnv","��","tnj",
			"��","tnjT","��","tnp","��","tnpr","��","tnps","��","tnpf","��","tnpa","��","tnpd","��","tnl","��","tnlr","��","tnls",
			"��","tnlf","��","tnla","��","tnlq","��","tnlt","��","tnld","��","tb","��","tbr","��","tbf","��","tba","��","tbt",
			"��","tbd","��","tm","��","tmr","��","tms","��","tmf","��","tmfr","��","tma","��","tmq","��","tmt","��","tmd",
			"��","tl","��","tlr","��","tls","��","tle","��","tlf","��","tlfg","��","tla","��","tlq","��","tlt","��","tld",
			"��","tlv","��","Tk","��","Tkr","��","Tkrt","��","Tks","��","Tkf","��","Tka","��","Tkq","��","TkT","��","Tkd",
			"��","Tkg","��","To","��","Tor","��","Tos","��","Tof","��","Toa","��","Toq","��","ToT","��","Tod","��","Tid",
			"��","Tj","��","Tjr","��","Tjs","��","Tjf","��","Tjfa","��","Tja","��","Tjq","��","TjT","��","Tjd","��","Tp",
			"��","Tpr","��","Tps","��","Tpf","��","TPs","��","Th","��","Thr","��","Ths","��","The","��","Thf","��","Thfa",
			"��","Tha","��","Thq","��","Thd","��","Thk","��","Thkr","��","Thks","��","ThkT","��","Tho","��","ThoT","��","Thl",
			"��","Thls","��","Thlf","��","Thla","��","Thlq","��","Ty","��","Tn","��","Tnr","��","Tns","��","Tnf","��","Tna",
			"��","Tnq","��","Tnd","��","Tnj","��","TnT","��","Tnp","��","Tnl","��","Tnls","��","Tbd","��","Tm","��","Tmr",
			"��","Tms","��","Tmf","��","Tmfa","��","Tmfg","��","Tma","��","Tmq","��","Tml","��","Tmls","��","Tmlf","��","Tmla",
			"��","Tl","��","Tlr","��","Tls","��","Tlf","��","Tla","��","Tlq","��","Tlt","��","Tld","��","dk","��","dkr",
			"��","dks","��","dksw","��","dksg","��","dkf","��","dkfr","��","dkfa","��","dkfg","��","dka","��","dkq","��","dkt",
			"��","dkT","��","dkd","��","dkx","��","dkv","��","do","��","dor","��","dos","��","dof","��","doa","��","doq",
			"��","dot","��","doT","��","dod","��","di","��","dir","��","dis","��","dif","��","difq","��","dia","��","diq",
			"��","dit","��","did","��","dix","��","dig","��","dO","��","dOs","��","dOf","��","dOq","��","dj","��","djr",
			"��","djs","��","djsw","��","dje","��","djf","��","djfr","��","djfa","��","dja","��","djq","��","djqt","��","djt",
			"��","djT","��","djd","��","djw","��","djz","��","djv","��","dp","��","dpr","��","dps","��","dpf","��","dpa",
			"��","dpq","��","dpt","��","dpd","��","du","��","dur","��","duR","��","dus","��","duf","��","dufa","��","dufq",
			"��","dua","��","duq","��","duqt","��","dut","��","duT","��","dud","��","dux","��","duv","��","dug","��","dP",
			"��","dPs","��","dPf","��","dPa","��","dPq","��","dPt","��","dPT","��","dh","��","dhr","��","dhs","��","dhf",
			"��","dhfr","��","dhfa","��","dhft","��","dhfg","��","dha","��","dhq","��","dht","��","dhd","��","dhc","��","dhk",
			"��","dhkr","��","dhks","��","dhkf","��","dhka","��","dhkq","��","dhkt","��","dhkT","��","dhkd","��","dho","��","dhor",
			"��","dhos","��","dhoa","��","dhot","��","dhod","��","dhl","��","dhlr","��","dhls","��","dhlf","��","dhla","��","dhlq",
			"��","dhlt","��","dhld","��","dy","��","dyr","��","dys","��","dyf","��","dya","��","dyq","��","dyr","��","dyd",
			"��","dn","��","dnr","��","dns","��","dnf","��","dnfr","��","dnfa","��","dna","��","dnq","��","dnt","��","dnd",
			"��","dnj","��","dnjr","��","dnjs","��","dnjf","��","dnja","��","dnjq","��","dnjT","��","dnjd","��","dnp","��","dnpr",
			"��","dnps","��","dnpf","��","dnpa","��","dnpq","��","dnpd","��","dnl","��","dnlr","��","dnls","��","dnlf","��","dnla",
			"��","dnlq","��","dnlt","��","dnld","��","db","��","dbr","��","dbs","��","dbf","��","dba","��","dbq","��","dbt",
			"��","dbd","��","dbc","��","dm","��","dmr","��","dms","��","dmf","��","dmfv","��","dma","��","dnq","��","dmt",
			"��","dmd","��","dmw","��","dmc","��","dmz","��","dmx","��","dmv","��","dmg","��","dml","��","dmls","��","dmlf",
			"��","dmla","��","dmlt","��","dl","��","dlr","��","dls","��","dlf","��","dlfr","��","dlfa","��","dlfg","��","dla",
			"��","dlq","��","dlt","��","dlT","��","dld","��","dlw","��","dlv","��","wk","��","wkr","��","wks","��","wksg",
			"��","wke","��","wkf","��","wkfa","��","wka","��","wkq","��","wkt","��","wkT","��","wkd","��","wkw","��","wo",
			"��","wor","��","wos","��","wof","��","woa","��","woq","��","wot","��","woT","��","wod","��","wi","��","wir",
			"��","wis","��","wisg","��","wif","��","wia","��","wid","��","wO","��","wOs","��","wOf","��","wj","��","wjr",
			"��","wjs","��","wjf","��","wjfa","��","wja","��","wjq","��","wjt","��","wjd","��","wjw","��","wp","��","wpr",
			"��","wps","��","wpf","��","wpa","��","wpq","��","wpt","��","wpd","��","wu","��","wus","��","wuf","��","wua",
			"��","wuq","��","wuT","��","wud","��","wP","��","wh","��","whr","��","whs","��","whf","��","whfa","��","wha",
			"��","whq","��","wht","��","whd","��","whw","��","whc","��","whg","��","whk","��","whkr","��","whkf","��","whkq",
			"��","whkt","��","whkd","��","who","��","whoT","��","whod","��","whl","��","whls","��","whlf","��","whla","��","whlq",
			"��","whlt","��","whld","��","wy","��","wyr","��","wys","��","wyd","��","wn","��","wnr","��","wns","��","wnf",
			"��","wnfr","��","wnfa","��","wna","��","wnq","��","wnt","��","wnd","��","wnj","��","wnjT","��","wnp","��","wnl",
			"��","wnlr","��","wnls","��","wnlf","��","wnla","��","wnlq","��","wnlt","��","wb","��","wbs","��","wbf","��","wba",
			"��","wm","��","wmr","��","wms","��","wmf","��","wma","��","wmq","��","wmt","��","wmd","��","wl","��","wlr",
			"��","wls","��","wle","��","wlf","��","wlfa","��","wla","��","wlq","��","wlt","¡","wld","¢","wlw","£","wlx",
			"¤","wlv","¥","Wk","¦","Wkr","§","Wks","¨","Wksg","©","Wkf","ª","Wkfq","«","Wka","¬","Wkq","­","Wkt",
			"®","WkT","¯","Wkd","°","Wo","±","Wor","²","Wos","³","Wof","´","Woa","µ","Woq","¶","Wot","·","WoT",
			"¸","Wod","¹","Wi","º","Wis","»","Wid","¼","Wj","½","Wjr","¾","Wjs","¿","Wjf","��","Wja","��","Wjq",
			"��","Wjt","��","WjT","��","Wjd","��","Wp","��","Wpd","��","Wu","��","WuT","��","Wh","��","Whr","��","Whs",
			"��","Whf","��","Wha","��","Whq","��","Wht","��","Whd","��","Whc","��","Whk","��","Whkr","��","Whkf","��","WhkT",
			"��","Who","��","WhoT","��","Whl","��","Whls","��","Whlf","��","Whla","��","Whlq","��","Wyd","��","Wn","��","Wnr",
			"��","Wns","��","Wnf","��","Wna","��","Wnq","��","Wnd","��","Wnj","��","WnjT","��","Wnjd","��","Wnl","��","Wb",
			"��","Wm","��","Wma","��","Wmt","��","Wmd","��","Wl","��","Wlr","��","Wls","��","Wlf","��","Wla","��","Wlq",
			"��","Wld","��","Wlw","��","Wlg","��","ck","��","ckr","��","cks","��","cksg","��","ckf","��","cka","��","ckq",
			"��","ckt","á","ckT","â","ckd","ã","ckw","ä","co","å","cor","æ","cos","ç","cof","è","coa","é","coq",
			"ê","cot","ë","coT","ì","cod","í","ci","î","cis","ï","cisg","ð","cif","ñ","cia","ò","cid","ó","cj",
			"ô","cjr","õ","cjs","ö","cjf","÷","cja","ø","cjq","ù","cjt","ú","cjT","û","cjd","ü","cp","ý","cpr",
			"þ","cps","ÿ","cpf","��","cpa","��","cpq","��","cpt","��","cpd","��","cu","��","cus","��","cuT","��","cP",
			"��","cPs","��","cPd","��","ch","��","chr","��","chs","��","chf","��","cha","��","chq","��","cht","��","cha",
			"��","chk","��","chks","��","chkf","��","chkd","��","chl","��","chls","��","chlf","��","chla","��","chlq","��","chlt",
			"��","chld","��","cy","��","cya","�h","cyq","��","cn","��","cnr","��","cns","��","cnf","��","cna","��","cnq",
			"��","cnt","��","cnd","��","cnj","��","cnjT","��","cnp","��","cnps","�X","cnpt","��","cnl","��","cnls","��","cnlf",
			"��","cnla","��","cnlq","��","cnlt","��","cnld","��","cb","��","cbs","��","cbf","��","cba","��","cbd","��","cm",
			"��","cmr","��","cms","��","cmf","��","cma","��","cmq","��","cmt","��","cmd","ġ","cl","Ģ","clr","ģ","cls",
			"Ĥ","cle","ĥ","clf","Ħ","clfr","ħ","cla","Ĩ","clq","ĩ","clt","Ī","cld","ī","zk","Ĭ","zkr","ĭ","zks",
			"Į","zkf","į","zka","İ","zkq","ı","zkt","Ĳ","zkd","ĳ","zo","Ĵ","zor","ĵ","zos","Ķ","zof","ķ","zoa",
			"ĸ","zoq","Ĺ","zot","ĺ","zoT","Ļ","zod","ļ","zi","Ľ","zir","ľ","zid","Ŀ","zj","��","zjr","��","zjs",
			"��","zje","��","zjf","��","zja","��","zjq","��","zjt","��","zjT","��","zjd","��","zp","��","zp","��","zps",
			"��","zpf","��","zpa","��","zpq","��","zpt","��","zpd","��","zu","��","zus","��","zuf","��","zua","��","zuq",
			"��","zut","��","zuT","��","zud","��","zP","��","zh","��","zhr","��","zhs","��","zhf","��","zha","��","zhq",
			"��","zht","��","zhd","��","zhk","��","zhkr","��","zhks","��","zhkf","��","zhka","��","zhkd","��","zho","��","zhod",
			"��","zhl","��","zhlf","��","zy","��","zn","��","znr","��","zns","��","znf","��","zna","��","znq","��","znt",
			"��","znd","��","znj","��","znjs","��","znjf","��","znjd","��","znp","��","znpd","��","znl","��","znlr","��","znls",
			"��","znlf","š","znla","Ţ","znlq","ţ","znlt","Ť","znld","ť","zb","Ŧ","zbs","ŧ","zbf","Ũ","zba","ũ","zm",
			"Ū","zmr","ū","zms","Ŭ","zmf","ŭ","zma","Ů","zmq","ů","zmd","Ű","zl","ű","zlr","Ų","zls","ų","zlf",
			"Ŵ","zla","ŵ","zlq","Ŷ","zlt","ŷ","zld","Ÿ","xk","Ź","xkr","ź","xks","Ż","xkf","ż","xkfr","Ž","xka",
			"ž","xkq","ſ","xkt","��","xkT","��","xkd","��","xo","��","xor","��","xos","��","xof","��","xoa","��","xoq",
			"��","xot","��","xoT","��","xod","��","xi","��","xid","��","xj","��","xjr","��","xjs","��","xjf","��","xjfa",
			"��","xja","��","xjq","��","xjt","��","xjT","��","xjd","��","xp","��","xpr","��","xps","��","xpf","��","xpa",
			"��","xpq","��","xpt","��","xpd","��","xu","��","xus","��","xuT","��","xP","��","xPs","��","xh","��","xhr",
			"��","xhs","��","xhf","��","xha","��","xhq","��","xht","��","xhd","��","xhv","��","xhk","��","xhks","��","xho",
			"��","xhl","��","xhls","��","xhlt","��","xhld","��","xy","��","xn","��","xnr","��","xns","��","xnf","��","xna",
			"��","xnq","��","xnt","��","xnd","��","xnj","��","xnjT","ơ","xnp","Ƣ","xnl","ƣ","xnlr","Ƥ","xnls","ƥ","xnlf",
			"Ʀ","xnla","Ƨ","xnlq","ƨ","xnld","Ʃ","xb","ƪ","xbs","ƫ","xb","Ƭ","xba","ƭ","xbd","Ʈ","xm","Ư","xmr",
			"ư","xms","Ʊ","xme","Ʋ","xmf","Ƴ","xmfa","ƴ","xma","Ƶ","xmq","ƶ","xmt","Ʒ","xml","Ƹ","xls","ƹ","xmlf",
			"ƺ","xmla","ƻ","xmlq","Ƽ","xl","ƽ","xlr","ƾ","xls","ƿ","xlf","��","xla","��","xlq","��","xlt","��","xld",
			"��","vk","��","vkr","��","vkR","��","vks","��","vkf","��","vkfa","��","vka","��","vkq","��","vkt","��","vkT",
			"��","vkd","��","vkx","��","vko","��","vor","��","vos","��","vof","��","voa","��","voq","��","vot","��","voT",
			"��","vod","��","vi","��","vir","��","vj","��","vjr","��","vjs","��","vjf","��","vja","��","vjq","��","vjt",
			"��","vjT","��","vjd","��","vp","��","vpr","��","vps","��","vpf","��","vpa","��","vpq","��","vpt","��","vpd",
			"��","vu","��","vus","��","vuf","��","vua","��","vuq","��","vuT","��","vud","��","vP","��","vPf","��","vPq",
			"��","vPt","��","vh","��","vhr","��","vhs","��","vhf","��","vha","��","vhq","��","vht","��","vhd","ǡ","vhk",
			"Ǣ","vhkd","ǣ","vhl","Ǥ","vhls","ǥ","vy","Ǧ","vys","ǧ","vyf","Ǩ","vyq","ǩ","vyt","Ǫ","vn","ǫ","vnr",
			"Ǭ","vns","ǭ","vne","Ǯ","vnf","ǯ","vnfa","ǰ","vna","Ǳ","vnq","ǲ","vnt","ǳ","vnd","Ǵ","vnj","ǵ","vnjd",
			"Ƕ","vnl","Ƿ","vnls","Ǹ","vnlf","ǹ","vnla","Ǻ","vnlt","ǻ","vb","Ǽ","vbs","ǽ","vbf","Ǿ","vba","ǿ","vbt",
			"��","vbd","��","vm","��","vms","��","vmf","��","vma","��","vmq","��","vmt","��","vl","��","vlr","��","vls",
			"��","vlf","��","vla","��","vlq","��","vlt","��","vld","��","gk","��","gkr","��","gks","��","gkf","��","gkfx",
			"��","gka","��","gkq","��","gkt","��","gkd","��","go","��","gor","��","gos","��","gof","��","goa","��","goq",
			"��","got","��","goT","��","god","�d","gog","��","gi","��","gid","��","gj","��","gjr","��","gjs","��","gjf",
			"��","gjfa","��","gja","��","gjq","��","gjt","��","gjd","��","gp","��","gpr","��","gps","��","gpf","��","gpa",
			"��","gpq","��","gpt","��","gpd","��","gu","��","gur","��","gus","��","guf","��","gua","��","guq","��","gut",
			"��","guT","��","gud","��","gP","��","gPs","ȡ","gPf","Ȣ","gPq","ȣ","gh","Ȥ","ghr","ȥ","ghs","Ȧ","ghf",
			"ȧ","ghfx","Ȩ","gha","ȩ","ghq","Ȫ","ght","ȫ","ghd","Ȭ","ghx","ȭ","ghk","Ȯ","ghkr","ȯ","ghks","Ȱ","ghkf",
			"ȱ","ghkt","Ȳ","ghkd","ȳ","gho","ȴ","ghor","ȵ","ghos","ȶ","ghot","ȷ","ghod","ȸ","ghl","ȹ","ghlr","Ⱥ","ghls",
			"Ȼ","ghlf","ȼ","ghlq","Ƚ","ghlt","Ⱦ","ghld","ȿ","gy","��","gys","��","gyf","��","gyq","��","gyt","��","gn",
			"��","gnr","��","gns","��","gnf","��","gnfx","��","gna","��","gnt","��","gnd","��","gnj","��","gnjs","��","gnjf",
			"��","gnja","��","gnjd","��","gnp","��","gnpr","��","gnps","��","gnpf","��","gnpd","��","gnl","��","gnlr","��","gnls",
			"��","gnlf","��","gnla","��","gnlq","��","gnlt","��","gnld","��","gb","��","gbr","��","gbs","��","gbf","��","gba",
			"��","gbt","��","gbd","��","gm","��","gmr","��","gms","��","gmsg","��","gme","��","gmf","��","gmfr","��","gma",
			"��","gmq","��","gmt","��","gmd","��","gmx","��","gml","��","gmls","��","gmlf","��","gmla","��","gmlq","��","gmld",
			"��","gl","��","glr","��","gls","��","glf","��","gla","��","glq","��","glt","��","gld"};

char jamo[34]="rRseEfaqQtTdwWczxvgkoiOjpuPhynbml";
char* han_jamo[33]={"��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��","��",
					"��","��","��","��","��","��","��","��","��","��","��","��","��","��"};

CHan::CHan()
{
	//�˻� �ӵ� ����� ���� ��������
	int mp=0;
	char* check_text[19]={"��","��","��","��","��","��","��","��","��","��","��","��","��","¥","��","ī","Ÿ","��","��"};
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

	//�Է� �ޱ�
	strcpy(han, "");	

	//�ѱۿ� ��ġ �� �Ǵ� �빮�ڸ� �ҹ��ڷ� �ٲ�
	for(unsigned int i=0; i<strlen(text); ++i)
	{
		if(han_area[i]==0)continue;
		if(text[i]>='A' && text[i]<='Z' && text[i]!='Q' && text[i]!='W' && text[i]!='E'	&& text[i]!='R'
			&& text[i]!='T' && text[i]!='O' && text[i]!='P')text[i]+=0x20;
	}

	//����, ���� �ѱ� �ƴ� ���� Ȯ��(���� �� �κ��� ����Ʈ�� �ξ ��ȯ�� ������ �ǰ� ��)
	for(unsigned int i=1; i<strlen(text); ++i)
	{
		//�������� �������� ����
		for(int j=0; j<33; ++j)
			if(text[i]==jamo[j])
			{
				if(j>=MOUM)
				{
					begin[pp]=i-1;
					if(pp>0)end[pp-1]=i-2;
					++pp;
					++i;	//���� ������ ��츦 �����ؼ� ������ �ѱ�(���� ������ ���� �о� ���ڰ� �Ǵ� ���� �����Ƿ� ��: �޻�(O) �ΤӤ���(X))
				}
				break;
			}
	}
	if(pp>0)end[pp-1]=(unsigned int)strlen(text)-1;

	//����Ʈ ����(�ѱ��� �κи� �ؼ�)
	for(unsigned int i=0; i<pp; ++i)
	{
		char buffer[10];
		int bp=0;
		BOOL head=TRUE;	//�� �κ����� ���� �޺κ����� �˾Ƴ�(�߶󳻾� �ϴ� �κ� ��: 1@��2$ -> 1@�� head=TRUE�� ����, 2$�� head=FALSE�� ���¿��� ����
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

		//������ �ѱ۷� ���� �� ��ġ�ϴ� ���� ã��
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

			//ã�� �� ���� �� ������ �ٿ��� ã�ƺ�
			if(!find)
			{
				buffer[strlen(buffer)-1]=NULL;
				--end[i];
			}
			else break;
		}

		//���� ���ϴ� �ѱ����� ǥ��
		if(strlen(buffer)<=1)begin[i]=9999;
	}

	//��ȯ
	unsigned int pp2=0;
	for(unsigned int i=0; i<strlen(text); ++i)
	{
		while(begin[pp2]==9999)++pp2;

		//�ѱ��� �κ�
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
		//�ƴ� �κ�
		else
		{			
			//�ѱ� �϶��� ������ ���� �������� ǥ��
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
			//�ᱹ ���� �� ���� �״��
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
//�⺻ ��ȭ ó�� Ŭ���� �޼ҵ�
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
	if(!setting || !setting2)return;	//��ȭâ�� �ϼ��Ǳ� ���� �̵���Ű�� ���װ� �Ͼ

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
	//����ڰ� ������ ��ȭâ ����
	dlgbox=box_name;
}

void CDlg::SetFont(JFont *font)
{
	if(font!=NULL)text_font=font;
}

void CDlg::DrawDlg()
{
	if(!setting || !show_dlgbox)return;	//�ʱ�ȭ�� �� �Ǿ� ������ ����

	if(dlgbox==NULL)
	{
		//��
		jdd->DrawPicture(backbuffer,"CommonDlg1",x-frame_x,y-frame_y,NULL);
		for(int i=0; i<width; ++i)jdd->DrawPicture(backbuffer,"CommonDlg2",x+(i*10),y-frame_y,NULL);
		jdd->DrawPicture(backbuffer,"CommonDlg3",x+x_size,y-frame_y,NULL);
		//�߰�
		for(int i=0; i<height; ++i)
		{
			int tmp_y=y+i*20;
			jdd->DrawPicture(backbuffer,"CommonDlg4",x-frame_x,tmp_y,NULL);
			for(int j=0; j<width; ++j)jdd->DrawPicture(backbuffer,"CommonDlg5",x+(j*10),tmp_y,NULL);
			jdd->DrawPicture(backbuffer,"CommonDlg4",x+x_size,tmp_y,NULL);
		}
		
		//�Ʒ�
		jdd->DrawPicture(backbuffer,"CommonDlg7",x-frame_x,y+y_size,NULL);
		for(int i=0; i<width; ++i)jdd->DrawPicture(backbuffer,"CommonDlg2",x+(i*10),y+y_size,NULL);
		jdd->DrawPicture(backbuffer,"CommonDlg9",x+x_size,y+y_size,NULL);
	}
	else
	{
		jdd->DrawPicture(backbuffer,dlgbox,x-frame_x,y-frame_y,NULL);
	}
	/*�ӵ��� ������ strech�� ��ȭâ�� ����ϴ� ������ �ٲ� ����(������ �ڽŸ��� ��ȭâ ����⵵ ����)*/
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
//����� ���� ��ȭ����
#define SNR_START 0

CTextDlg::CTextDlg(unsigned int vbuffer_size)
{
	buffer_size=vbuffer_size;
	dlg_buffer=new char[buffer_size];	//��ȭ ���� ����
	text_buffer=new char[buffer_size];	//������� ����� ������ ����

	lock=FALSE;
}

CTextDlg::~CTextDlg()
{
	//���� �����
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

//��ȭ �����
void CTextDlg::MakeText(char* content)
{
	if(!setting || content==NULL)return;	//�ʱ�ȭ�� �� �Ǿ� ������ ����

	//��ȭ���� ����
	length=(int)strlen(content);
	strncpy(dlg_buffer, content, buffer_size);
	if(strlen(content) >= buffer_size)dlg_buffer[buffer_size-1]=NULL;
	ani_end=FALSE;

	//��ȭ�� ����ϱ� �˸°� ó��
	sp=SNR_START;	//ó���ǰ� �ִ� ����Ʈ
	int space=-1;	//�������� ó���� ����Ʈ, space�� �ִ� ��
	int senlen=0;	//������ ����
	
	//��ȭâ�� �°� ������ �� �ٲ�
	if(text_font==&global_font)
	while(sp<length)
	{
		BOOL length_check=TRUE;		//���� üũ�� ���� �� ���� �˻�
		int sp_add=1;				//�̵��� ĭ ��, �ѱ��� ��츸 2ĭ.

		//�� �ٲ�
		if(dlg_buffer[sp]=='\\')
		{
			dlg_buffer[sp]='\n';
			space=-1;
			senlen=0;
			length_check=FALSE;
		}
		//�ѱ�
		else if(dlg_buffer[sp]&0x80)
		{
			senlen+=20;
			sp_add=2;
		}
		//��Ÿ
		else
		{
			if(dlg_buffer[sp]==' ')space=sp;
			senlen+=GetCharLength(dlg_buffer[sp]);
		}
		
		//���̰� ��ȭâ�� �Ѿ����� �˻�
		if(length_check && senlen>width*10)
		{
			if(space<0)	//�����̽��� �ϳ��� ������ ���� �� ��ȯ
			{
				//���̿� CR�� �־���
				char* tmp_text;
				tmp_text = new char[length+2];

				//���ο� ���ڿ�(���� �ͺ��� ���̰� 1 ū ��)�� ���� ����
				for(int i=0; i<sp; ++i)tmp_text[i]=dlg_buffer[i];
				tmp_text[sp]='\n';
				for(int i=sp+1; i<length+1; ++i)tmp_text[i]=dlg_buffer[i-1];
				tmp_text[length+1]=NULL;
				strcpy(dlg_buffer,tmp_text);
				delete[] tmp_text;

				//���� ���ڿ� ���̴� ���� ������ ����
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
					//��Ÿ
					else senlen+=GetCharLength(dlg_buffer[i]);
				}
				space=-1;
			}
		}

		//�ѱ��� �� 2ĭ �ٰ� �������� 1ĭ
		sp+=sp_add;
	}

	sp=SNR_START, tp=0;;	//�����ġ �ʱ�ȭ
	n_of_e=0;				//�ٹٲ� Ƚ��
	//��ȭâ ũ�� ����
	x_size=width*10;
	y_size=line*20;
	height=line;

	setting2=TRUE;
}

//��ȭ ���(������ ���¿���)
void CTextDlg::Print(char* content)
{
	if(!setting)return;	//�ʱ�ȭ�� �� �Ǿ� ������ ����

	MakeText(content);

	//�޹�� ��� ����(���� �ִ� ��ȭâ�� �� ���� ���鼭 ������ �������� ���� ����
	JPictureInfo pi;
	pi.SetWidth(SCREEN_X);
	pi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonCopyScreen", &pi, TRUE);
	jdd->DrawPicture("CommonCopyScreen", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//������ �޼��� ó��

		jdd->DrawPicture(backbuffer, "CommonCopyScreen", 0, 0, NULL);
		if(Printing()==999)break;	//������ �� ����ϸ� �ߴ�
		jdd->Render();
		//���� �ѱ��
		if(keyboard_control && GetKey(vkey_enter, 0) || mouse_control && LButton);
			else Sleep(10);
	}

	jdd->DeleteSurface("CommonCopyScreen");
}

//��ȭ���(�ǽð�����)
int CTextDlg::Printing()
{
	if(!setting || !setting2)return -1;	//�ʱ�ȭ�� �� �Ǿ� ������ ����

	if(!ani_end)
	{
		if(sp<length && n_of_e<line)
		{
			if(dlg_buffer[sp]=='\n')++n_of_e;
			text_buffer[tp]=dlg_buffer[sp];
			if(dlg_buffer[sp]&0x80)	//�ѱ��� ��� 2ĭ ����
			{
				++sp; ++tp;
				text_buffer[tp]=dlg_buffer[sp];
			}
			text_buffer[tp+1]=NULL;
			//������ ����
			++sp; ++tp;
		}
		else ani_end=TRUE;
	}
	else
	{
		//��������
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
	//��ȭ ���
	DrawDlg();

	RECT text, shadow;
	//�׸���
	if(b_shadow)
	{
		SetRect(&shadow,x+1,y+1,SCREEN_X,SCREEN_Y);
		jdd->DrawText(backbuffer,text_buffer,*text_font,&shadow,JColor(0,0,0));
	}
	//����
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
//������ ���� ��ȭ����
CSelectDlg::CSelectDlg(int select_max, unsigned int vbuffer_size)
{
	buffer_size=vbuffer_size;
	dlg_buffer=new char[buffer_size];
	select_point=new char*[select_max];

	setting2=FALSE;
}

CSelectDlg::~CSelectDlg()
{
	//���� �����
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

	//gapũ�⸦ 10���� �������� ����
	gap=Max(gap, 30);
	gap-=(gap%10);

	//���μ��������� ���� ���������� Ȯ��, ���� ����
	if(line<=0)line=1;
	row=vrow;

	//����Ʈ�� ��ġ �ʱ�ȭ
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

	//���� ũ�⸦ �Ѿ�� ���� ����
	strncpy(dlg_buffer, input_dlg, buffer_size);
	if(strlen(input_dlg)>=buffer_size)dlg_buffer[buffer_size-1]=NULL;

	int length=(int)strlen(dlg_buffer);

	//��ȭ�� ����ϱ� �˸°� ó��
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

	//��ũ��Ʈ���� �о� �� ������ ���� ��ǥ(0x0d)�� ����� �� ����
	if(dlg_buffer[length-1]==13)dlg_buffer[length-1]=NULL;

	//��ȭâ ǥ�ÿ� �ʿ��� �͵� ���
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
	//�޹�� ��� ����(���� �ִ� ��ȭâ�� �� ���� ���鼭 ������ �������� ���� ����
	JPictureInfo pi;
	pi.SetWidth(SCREEN_X);
	pi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonCopyScreen", &pi, TRUE);
	jdd->DrawPicture("CommonCopyScreen", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//������ �޼��� ó��

		jdd->DrawPicture(backbuffer, "CommonCopyScreen", 0, 0, NULL);
		int selected=Selecting();
		if(selected>=0 || (cancel && selected==-999))break;	//�����ϰų� ����ϸ� �ߴ�

		jdd->Render();
	}

	jdd->DeleteSurface("CommonCopyScreen");

	return select;
}

int CSelectDlg::Selecting()
{
	if(!setting2)return -999;

	//��ȭ����
	if(keyboard_control)
	{
		if(row)
		{
			//�� �Ʒ� �̵�
			if(GetKey(vkey_right,10))select=Min(n_of_e,select+1);
				else if(GetKey(vkey_left,10))select=Max(0,select-1);
			//�¿� �̵�
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
			//�� �Ʒ� �̵�
			if(GetKey(vkey_down,10))select=Min(n_of_e,select+1);
				else if(GetKey(vkey_up,10))select=Max(0,select-1);

			//�¿� �̵�
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

	//���콺�� ����
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
	//���� Ŀ��
	RECT src_rect;
	SetRect(&src_rect,0,0,gap/2,20);
	jdd->DrawPicture(backbuffer,"CommonSelect",sx,sy,&src_rect);
	SetRect(&src_rect,320-gap/2,0,320,20);
	jdd->DrawPicture(backbuffer,"CommonSelect",sx+gap/2,sy,&src_rect);

	//��������
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
//�Է��� ���� ��ȭ����
CInputDlg::CInputDlg(int vx, int vy, int max)
{
	flag= FLAG_TEXT	| FLAG_NUMBER | FLAG_MARK;

	SetDlg(vx, vy, max);
}

CInputDlg::CInputDlg()
{
	flag= FLAG_TEXT	| FLAG_NUMBER | FLAG_MARK;

	SetDlg(250,230,140);	//�⺻��
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
	//��ȭâ ũ�� ����
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

//�ѱ��Է�
char* CInputDlg::Input()
{
	//�޹�� ��� ����(���� �ִ� ��ȭâ�� �� ���� ���鼭 ������ �������� ���� ����
	JPictureInfo pi;
	pi.SetWidth(SCREEN_X);
	pi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonCopyScreen", &pi, TRUE);
	jdd->DrawPicture("CommonCopyScreen", backbuffer, 0, 0, NULL);

	while(1)
	{
		if(!ProcessMessage())break;		//������ �޼��� ó��

		jdd->DrawPicture(backbuffer, "CommonCopyScreen", 0, 0, NULL);
		if(Inputing())break;	//�Է��� ������ �ߴ�

		jdd->Render();
	}

	jdd->DeleteSurface("CommonCopyScreen");

	return input_text;
}

//�Է°����� Ư�����ڵ�
char num_shift_char[]={')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
char special_char[]={';', '=', ',', '-', '.', '/', '`', '[', '\\', ']','\''};	// `�� �Ÿ������� ���� �� ��
char special_shift_char[]={':', '+', '<', '_', '>', '?', '~', '{', '|', '}','"'};

int CInputDlg::Inputing()
{
	//���
	DrawDlg();
	if(b_shadow)jdd->DrawText(backbuffer,input_text,*text_font,x+1,y+1,JColor(0,0,0));
	//�����̴� Ŀ��
	char show_text[101];
	strcpy(show_text, input_text);
	static int blink_count;
	++blink_count;
	if(blink_count<0 || blink_count>=40)blink_count=0;
	if(blink_count<20 && str_max>text_length+10)strcat(show_text,"_");
	//�Էµ� ���� ǥ��
	if(b_shadow)jdd->DrawText(backbuffer,show_text,*text_font,x+1,y+1,JColor(0,0,0));
	jdd->DrawText(backbuffer,show_text,*text_font,x,y,color);

	int eng_len=(int)strlen(eng_buffer);
	//�Է�
	if(eng_len<80)
	{
		BOOL done=FALSE;
		//����Ű
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
		//����Ű
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
		//��Ÿ ��ȣ
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

		//�����̽�
		if(!done && GetKey(vkey_space))strcat(eng_buffer, " ");

		if(done && strlen(eng_buffer)>0)is_eng_buffer[strlen(eng_buffer)-1]=is_eng;
	}
	//Ư�� Ű
	if(GetKey(vkey_enter))return 1;	//����
	else if(GetKey(vkey_back, 10))	//�齺���̽�
	{
		if(eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
	}
	else if(GetKey(vkey_alt))
	{
		is_eng=1-is_eng;
	}

	//�Է� ���� ����
	if(old_len!=eng_len)
	{
		BOOL able_text=FALSE;
		while(!able_text)
		{
			//���� ��ü ���� ���
			strcpy(input_text, m_han.EngToHan(eng_buffer, is_eng_buffer));
			text_length=0;
			for(size_t i=0; i<strlen(input_text) ; ++i)text_length+=GetCharLength(input_text[i]);

			//���� ��ȭâ �̻��� ���̶�� �ڸ���
			if(str_max/10<text_length && eng_len>0)eng_buffer[strlen(eng_buffer)-1]=NULL;
				else able_text=TRUE;
		}
		old_len=eng_len;

		//�����̽��� �׸���
		if(b_shadow)jdd->DrawText(backbuffer,input_text,*text_font,x+1,y+1,JColor(0,0,0));
		//�����̴� Ŀ��
		char show_text[101];
		strcpy(show_text, input_text);
		static int blink_count;
		++blink_count;
		if(blink_count<0 || blink_count>=40)blink_count=0;
		if(blink_count<20 && str_max>text_length+10)strcat(show_text,"_");
		//�Էµ� ���� ǥ��
		jdd->DrawText(backbuffer,show_text,*text_font,x,y,color);

	}

	return 0;
}

void CInputDlg::SetFlag(BOOL vflag)
{
	flag = vflag;
}

/////////////////////////////////
//��� ó�� Ŭ���� �޼ҵ�
//(���� �迭�� char com={"�̵�","���"...}���� ����� ����, ������ ����� �־ ������ �� �ְ� �Ѵ�.

CCommand::CCommand(char* vsource[], int vx, int vy, int vgap, int vline, BOOL vrow, int max)
				//��ɾ� ���� �迭, X,Y��ǥ, ���̰���, � �̻� ǥ�õǸ� ���� �ٲܰ���, ���� or ����, �ִ�ġ (���ۻ����� �߰��� ��!)
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

void CCommand::AddComs(int n, ...)	//��� ������ �߰�
{
	va_list ap;
	va_start(ap,n);
	for(int i=0; i<n; ++i)AddCom(va_arg(ap,int));
	va_end(ap);
}

void CCommand::AddCom(int id)	//����� �� �ִ� ��� �߰�
{
	if(count>=command_max)return;	//��ɾ �� ��
	
	if(count>0)strcat(commands,"\\");
	strcat(commands,source[id]);
	comdlg.SetDlg(x, y, gap, Min(line_max, count+1), row);
	comdlg.MakeSelection(commands);
	com_id[count]=id;
	++count;
}

//���ڹ迭�� �̿����� �ʰ� ���� ���� �ֱ�
void CCommand::AddComStr(int id, char* str)
{
	if(count>=command_max)return;	//��ɾ �� ��
	
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

//����á���� Ȯ��
BOOL CCommand::IsFull()
{
	if(count>=command_max)return TRUE;
		else return FALSE;
}

//�ǽð����� ����(�ٸ� �۾��� ���ķ� ó��)
int CCommand::CommandSelecting()
{
	if(count<1)return -999;		//��� ���� ����� ������ �������� ����

	int selected;
	selected=comdlg.Selecting();

	if(selected>=0)return GetComID(selected);
		else return -1;
}

//���ø� ��(���μ��� ����)
int CCommand::CommandSelect()
{
	int selected;
	while(1)
	{
		if(!ProcessMessage())break;		//������ �޼��� ó��

		selected=comdlg.Selecting();	//��� ����
		if(selected>=0)break;
		else if(selected==-999)return -1;

		jdd->Render();
	}

	return GetComID(selected);
}

//������� TRUE, �� ������ FALSE�� ����
BOOL CCommand::Empty()
{
	if(count==0)return TRUE;
		else return FALSE;
}

//////////////////////////////////////
//���� ó�� Ŭ����
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
//CAnimation �޼ҵ�
CAnimationData::CAnimationData(int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	Set(x, y, vtype, vframe_max, vdelay_max);
	frame=delay=y_point=0;
	walk=FALSE;
}

//���� : Xũ��, Yũ��, �ִϸ��̼�Ÿ��, �׸� ��, �����ð�
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

//�ִϸ��̼� ó���� ����(�̰��� ������ ������ �ڵ����� �ִϸ��̼��� ���� ����)
void CAnimationData::Process()
{
	if(type==ani_loop)	//������ �ݺ� �ִϸ��̼�
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
	else if(type==ani_exchange)	//�Դٰ��� �ݺ� �ִϸ��̼�
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
	if(type==ani_once)	//������ �ѹ� �ִϸ��̼�
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

//�ִϸ��̼��� ����������
void CAnimationData::Reset()
{
	SetFrame(0);
	delay=0;
}

//�����¿� �ִϸ��̼��� �ϴ� ��� Y��ǥ�� �ٲ㼭 �ִϸ��̼�
void CAnimationData::SetWalk(int up_ypos, int down_ypos, int left_ypos, int right_ypos)
{
	up=up_ypos;
	down=down_ypos;
	left=left_ypos;
	right=right_ypos;
	
	walk=TRUE;
}

//��� ���ϱ�
void CAnimationData::SetFrame(int vframe)
{
	frame=vframe;
	SetRect(&ani_rect, x_size*vframe, y_point, x_size*(vframe+1), y_point+y_size);
}

//X,Y �� ��������
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

	//���� ����
	if(dir==dir_up)y_point=up;
	else if(dir==dir_left)y_point=left;
	else if(dir==dir_down)y_point=down;
	else if(dir==dir_right)y_point=right;
	//�ݴ�� ����
	else if(dir==dir_turn)
	{
		if(dir==dir_up)y_point=down;
		else if(dir==dir_left)y_point=right;
		else if(dir==dir_down)y_point=up;
		else if(dir==dir_right)y_point=left;
	}
	//�������� ����
	else if(dir==dir_turn_left)
	{
		if(dir==dir_up)y_point=left;
		else if(dir==dir_left)y_point=down;
		else if(dir==dir_down)y_point=right;
		else if(dir==dir_right)y_point=up;
	}
	//���������� ����
	else if(dir==dir_turn_right)
	{
		if(dir==dir_up)y_point=right;
		else if(dir==dir_left)y_point=up;
		else if(dir==dir_down)y_point=left;
		else if(dir==dir_right)y_point=down;
	}
	//�ƹ����� ����
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
	//�ڵ� �ִϸ��̼�
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
		OutputDebugString("�߸��� �ִϸ��̼� ID�� ��û�ϼ̽��ϴ�.\n");
		OutputDebugString("������ ���� ���� �ӽ� �ִϸ��̼� ������ �����ؼ� �����ϴ�.\n");
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
//�� ã�� �޼���

CSearchMap::CSearchMap(int* p_map, int x, int y)
{
	//���� ũ�� ���ϱ�
	x_size=x;
	y_size=y;

	//�̵����� ���� �� �ּ� ��������
	map=p_map;

	//�̵����� ��Ÿ�� �� ����
	roadmap = new int*[x_size];
	for(int i=0; i<x_size; ++i)
		roadmap[i] = new int[y_size];

	//�Ÿ��� ��Ÿ�� �� ����
	distancemap = new int*[x_size];
	for(int i=0; i<x_size; ++i)
		distancemap[i] = new int[y_size];

	point.x=point.y=0;
}

CSearchMap::~CSearchMap()
{
	//�޸� �Ұ�
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

	//�� �ʱ�ȭ
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			roadmap[i][j]=MOVE_DISABLE;

	//�� ã��
	FindRoad(roadmap, point.x, point.y, 0, move_max);
}

void CSearchMap::SetEndPoint(MapPoint p)
{
	SetEndPoint(p.x, p.y);
}

void CSearchMap::SetEndPoint(int x, int y)
{
	//�� �ʱ�ȭ
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			distancemap[i][j]=MOVE_DISABLE;

	//�̵���� �˻�
	FindRoad(distancemap, x, y, 0);
}

void CSearchMap::FindRoad(int** vmap, int x, int y, int expense, int move_max)
{
	vmap[x][y]=expense;
	if(move_max>=0 && expense>move_max)return;	//�̵��� �Ѱ��� ���� �� �̻� �˻����� ����

	if(x>0 && vmap[x-1][y]>expense+map[(x-1)+y*x_size])
		FindRoad(vmap, x-1, y, expense+map[(x-1)+y*x_size], move_max);	//���� �˻�
	if(x<x_size-1 && vmap[x+1][y]>expense+map[(x+1)+y*x_size])
		FindRoad(vmap, x+1, y, expense+map[(x+1)+y*x_size], move_max);	//������ �˻�
	if(y>0 && vmap[x][y-1]>expense+map[x+(y-1)*x_size])
		FindRoad(vmap, x, y-1, expense+map[x+(y-1)*x_size], move_max);	//�� �˻�
	if(y<y_size-1 && vmap[x][y+1]>expense+map[x+(y+1)*x_size])
		FindRoad(vmap, x, y+1, expense+map[x+(y+1)*x_size], move_max);	//�Ʒ� �˻�
}

BOOL CSearchMap::MoveNextPointFast(int move)
{
	//���� ���� ���� ã��
	int best_dir=-1, best_expense=distancemap[point.x][point.y];

	//���� �˻�
	if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
	{
		best_dir=0;
		best_expense=distancemap[point.x-1][point.y];
	}
	//������ �˻�
	if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
	{
		best_dir=1;
		best_expense=distancemap[point.x+1][point.y];
	}
	//�� �˻�
	if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
	{
		best_dir=2;
		best_expense=distancemap[point.x][point.y-1];
	}
	//�Ʒ� �˻�
	if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
	{
		best_dir=3;
		best_expense=distancemap[point.x][point.y+1];
	}

	//���� ����� ������ �̵�(�ƴϸ� ������ ����)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//�������� �����ϸ� TRUE, �ƴϸ� FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

BOOL CSearchMap::MoveNextPoint(int move)
{
	//���� ���� ���� ã��
	int best_dir=-1, best_expense=distancemap[point.x][point.y];

	//ù°�� �˻��ϴ� ���� �����ϰ� ���ϱ� ���� �÷���
	//(�켱������ ������ ������ ������ �ܼ��ؼ� ���⿡ �� ����.)
	BOOL left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;

	while(!left_ok || !right_ok || !up_ok || !down_ok)
	{
		int ran=rand()%4;

		//���� �˻�
		if(ran==0)
		{
			if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
			{
				best_dir=0;
				best_expense=distancemap[point.x-1][point.y];
			}
			left_ok=TRUE;
		}
		//������ �˻�
		else if(ran==1)
		{
			if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
			{
				best_dir=1;
				best_expense=distancemap[point.x+1][point.y];
			}
			right_ok=TRUE;
		}
		//�� �˻�
		else if(ran==2)
		{
			if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
			{
				best_dir=2;
				best_expense=distancemap[point.x][point.y-1];
			}
			up_ok=TRUE;
		}
		//�Ʒ� �˻�
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

	//���� ����� ������ �̵�(�ƴϸ� ������ ����)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//�������� �����ϸ� TRUE, �ƴϸ� FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

BOOL CSearchMap::MoveNextPointStupid(int move, int iq)
{
	iq=MaxMin(iq, 1, 10);	//����ť�� 1~10����

	//���� ���� ���� ã��
	int best_dir=-1, best_expense=distancemap[point.x][point.y]+(10-iq);

	//��û�ϱ� ������ ��� �˻����� �ʰ� ��� �������� �õ��� �� �� �� �߿��� �̵�
	BOOL left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;
	int count=0;

	while(count<iq && (!left_ok || !right_ok || !up_ok || !down_ok))
	{
		int ran=rand()%4;

		//���� �˻�
		if(ran==0)
		{
			if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
			{
				best_dir=0;
				best_expense=distancemap[point.x-1][point.y];
			}
			left_ok=TRUE;
		}
		//������ �˻�
		else if(ran==1)
		{
			if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
			{
				best_dir=1;
				best_expense=distancemap[point.x+1][point.y];
			}
			right_ok=TRUE;
		}
		//�� �˻�
		else if(ran==2)
		{
			if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
			{
				best_dir=2;
				best_expense=distancemap[point.x][point.y-1];
			}
			up_ok=TRUE;
		}
		//�Ʒ� �˻�
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

	//���� ����� ������ �̵�(�ƴϸ� ������ ����)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//�������� �����ϸ� TRUE, �ƴϸ� FALSE
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
//�ʱ�ȭ �۾�(�Ź� �ٲ��� �����ϱ� main���� donglib�� �Ű���
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

	//�׷��� �ʱ�ȭ
	backbuffer=jdd->GetBackBuffer();
	global_font=jdd->CreateFont("�ü�",20);
	//�ε��� �� �ð��� �ɸ� ���� ����� ȭ��
	if(!(jdd->LoadPicture("CommonLoading", "Loading.jpg", NULL, TRUE)))jdd->LoadPicture("CommonLoading", "Loading.gif", NULL, TRUE);
	jdd->DrawPicture(backbuffer, "CommonLoading", 0, 0, NULL);
	jdd->Render();

	//�ӽ� �����̽� ����
	JPictureInfo jpi;
	jpi.SetWidth(SCREEN_X);
	jpi.SetHeight(SCREEN_Y);
	jdd->CreateSurface("CommonBlank", &jpi, TRUE);
	
	jdd->SetFrameRate(100,TRUE);
	jdd->SetVerticalSync(FALSE);

	//���� �ʱ�ȭ
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

	//��Ÿ �ʱ�ȭ
	srand( (unsigned)time( NULL ) );
	keyboard_control=use_keyboard;
	mouse_control=use_mouse;
	gameover=FALSE;

	return TRUE;
}
