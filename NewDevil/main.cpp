#include <time.h>
#include <ddraw.h>
#include <dsound.h>
#include <stdio.h>

#include "extern.h"
#include "battle.h"
#include "data.h"
#define MAX_X	8
#define MAX_Y	10
#define HUMANS	320

//������ �κ�
bool gam=false;
char map[10][12];
char build[8][10];
int waters, max_human;		//�����ϴ� ���� ��, �ù� �ִ��
int chk_speed, noise=0;
bool key_left=false;		//���콺 ���� ��ư Ŭ������ �Ǵ�
int next_grade[]={12,25,40,75,110,150,200,9999};
int game_mode=0;	//�޴�, ���ӵ��� ����
int river=0; //���� �帧
bool game_start=false;	//���ӽ��� ����
int popup=-1;	//�˾�â ����
int play_time, difficult=1;	//�ð�, ���̵�

//���̺�, �ε� ���� ����
int Select;
char* fname[]={"1.sav","2.sav","3.sav","4.sav","5.sav"};
int fnum[5], fmp[5], fdifficult[5], ftime[5];
bool fabl[5];	//���������� �ƴ��� �˻�

CBattle battle;

/////////////////////////////////////////
//���� Ŭ����
class CDevil
{
public:
	int eyeX, eyeY;			//�����ִ� �� ��ġ
	int eyeland;			//�����ִ� �� ����
	int grade;
	int tempo;
	int mp, inc_mp, dec_mp;	//�����Ѱ�, ���¼���, ���� �Һ�

	void Clear();
};

void CDevil::Clear()
{
	eyeX=eyeY=-1;
	eyeland=5;
	grade=0;
}

/////////////////////////////////////////
//��� Ŭ����
class CHero
{
public:
	bool life;	//���翩��
	int x, y;	//Ȱ����ġ
	int sleep;	//���� �ִ��� ����

	void Clear();
};

void CHero::Clear()
{
	life=false;
	x=0;
	sleep=999;
}

CDevil devil;
CHero hero[4];

/////////////////////////////////////////
//���� �ֹ� Ŭ����
class CHuman
{
public:
	bool life;	//���翩��
	char happy;	//�ູ��, ����
};

/////////////////////////////////////////
//��ü �ֹ� Ŭ����
class CHumans
{
public:
	int number;
	int happy;
	CHuman human[HUMANS];

	void CalHappy();
	void IncHappy();
	int Count(int x, int y);
	bool NewHuman(int x, int y);
	void Clear();
};

//�ູ���� �α����� �˻��ϴ� �Լ�
void CHumans::CalHappy()
{
	int count, average;
	count=average=0;
	for(int i=0; i<HUMANS; i++)
		if(human[i].life)
		{
			average+=human[i].happy;
			count++;
		}
	if(count>0)happy=average/count;
		else happy=0;
	number=count;
}

//�ູ�� ��ȭ
void CHumans::IncHappy()
{
	int x=-1, y=0;
	for(int i=0; i<HUMANS; i++)
	{
		if((i%4)==0)x++;
		if(x>=MAX_X)
		{
			x=0; y++;
		}

		if(human[i].life)
		{
			if(human[i].happy>-101)
			{
				int happy=0;

				happy+=1-(i%4);		//�ֹ� ��ȣ�� ���� �ູ��
				int forest=4-difficult;	//���� �ູ��
				if(build[x][y]==0)happy+=forest;	//���� ����
					else happy+=forest-map[x+1][y+1];	//������ ���� �ູ��
				for(int j=0; j<4; j++)
					if(hero[j].life)happy--;	//��簡 ���� ���
				if(x==devil.eyeX && y==devil.eyeY)happy=0;			//���� ���
				if(build[x][y]==2)happy=Max(happy,-1);	//õ���� ����

				human[i].happy=MaxMin(human[i].happy+happy,100,-100);
				if(human[i].happy==-100 && (i%4)==3 && human[i-1].happy>=0)human[i].happy=-101;	//�г�
			}

			if(human[i].happy==100 && number<max_human)	//�ູ���� 100�϶� �α� ����
			{
				human[i].happy=0;
				NewHuman(x,y);
			}
		}
	}
}

//���ο� �ֹ� ����� �Լ�
bool CHumans::NewHuman(int x, int y)
{
	if(map[x+1][y+1]!=5)	//������ �˻�
	{
		int start=(y*MAX_X+x)*4;
		for(int i=start; i<start+4; i++)
			if(!human[i].life) //�ڸ��� ����� ��� �� ����� ��ġ
			{
				human[i].life=true;
				human[i].happy=0;
				return true;
			}
	}
	int bx=x+1, by=y+1;	//�������� �Ǵ� ����
	//�α��� ���� �� ���� �� �ٸ� �� Ž��
	for(int j=1; j<5; j++)
	{
		for(int k=0; k<j; k++)
		{
			int sx, sy;
			for(int l=0; l<4; l++)	//�����¿� �˻�
			{
				if(l==0){sx=bx-j+k; sy=by-k;}
					else if(l==1){sx=bx+j-k; sy=by+k;}
					else if(l==2){sx=bx+k; sy=by-j+k;}
					else {sx=bx-k; sy=by+j-k;}

				if(sx<1 || sx>8 || sy<1 || sy>10 || map[sx][sy]>=5)continue;

				int start=((sy-1)*MAX_X+(sx-1))*4;
				for(int i=start; i<start+4; i++)
					if(!human[i].life) //�ڸ��� ����� ��� �� ����� ��ġ
					{
						human[i].life=true;
						human[i].happy=0;
						return true;
					}
			}
		}
	}
	return false;
}

CHumans citizen;

//�� Ÿ�Ͽ� �ִ� �ֹ� �� �˻�
int CHumans::Count(int x, int y)
{
	int count=0;
	for(int i=0; i<4; i++)
		if(human[(y*MAX_X+x)*4+i].life)count++;
	return count;
}

void CHumans::Clear()
{
	for(int i=0; i<HUMANS; i++)human[i].life=false;
}

/////////////////////////////////////////////////
//�Լ���
int Xpos(int num)
{
	if(num<10)return 0;
		else if (num<100)return -4;
			else return -8;
}

void SetMap()
{
	int x,y;
	//�� Ÿ�� ���
	if(++river>=60)river=0;
	SetR(&BackRect,350+river,river,60,60);
	for(x=1; x<9; x++)	
		for(y=1; y<11; y++)
			if(map[x][y]==5)_DrawBmp(BackRect, (x-1)*60, (y-1)*60, BmpScreen[1], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	//�⺻ �� ���
	for(x=1; x<9; x++)	
		for(y=1; y<11; y++)
		{
			if(map[x][y]>=2 && map[x][y]<=4)
			{
				if(map[x+1][y]==5 || map[x][y+1]==5)
				{
					SetR(&BackRect,0,0,70,70);
					_DrawBmp(BackRect, (x-1)*60, (y-1)*60, BmpScreen[1], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				}
			}
			else if(map[x][y]!=5)
			{
				SetR(&BackRect,map[x][y]*70,0,70,70);
				_DrawBmp(BackRect, (x-1)*60, (y-1)*60, BmpScreen[1], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
		}
	//��Ÿ ���� ���
	for(x=1; x<9; x++)	
		for(y=1; y<11; y++)
		{
			if(map[x][y]>=2 && map[x][y]<=4)
			{
				int left, right, top, bottom;
				left=right=top=bottom=0;
				if(map[x-1][y]==5)left+=5;
				if(map[x+1][y]==5)right-=5;
				if(map[x][y-1]==5)top+=5;
				if(map[x][y+1]==5)bottom-=5;
				SetR(&BackRect,map[x][y]*70+left,top,70+right-left,70+bottom-top);
				_DrawBmp(BackRect, (x-1)*60-5+left, (y-1)*60-5+top, BmpScreen[1], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
			//�μ� ��, �ǹ�, �ູ�� �˱�
			for(int jd=0; jd<4; jd++)
				if(citizen.human[((y-1)*MAX_X+(x-1))*4+jd].life)
				{
					SetR(&BackRect,225,617,60,34);
					_DrawBmp(BackRect, (x-1)*60, (y-1)*60+25, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
					break;
				}
		}
	for(x=0; x<MAX_X; x++)	
		for(y=0; y<MAX_Y; y++)
		{
			if(build[x][y]>=0)
			{
				SetR(&BackRect,build[x][y]*30+260,195,30,30);
				_DrawBmp(BackRect, x*60+30, y*60+30, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
			else if(build[x][y]<-1)
			{
				int frame[]={0,40,80,40,0,40,80,40,0};
				SetR(&BackRect,260+frame[(build[x][y]+30)/3],155,40,40);
				_DrawBmp(BackRect, x*60+25, y*60+25, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				build[x][y]++;
			}
		}
	//����� ���
	static int hero_tmp;
	for(int i=0;i<4;i++)
	{
		if(hero[i].life)
		{
			int left, right, top, bottom;
			left=Max(hero[i].x-1,0);
			right=Min(hero[i].x+2,MAX_X);
			top=Max(hero[i].y-1,0);
			bottom=Min(hero[i].y+2,MAX_Y);
			
			bool brk=false;
			for(int x2=left; x2<right; x2++)	//���� ������ �˻�
			{
				for(int y2=top; y2<bottom; y2++)
				{
					if(build[x2][y2]==1)
					{
						if(hero_tmp<0 || hero_tmp>29)hero_tmp=0;
						if(i==0)SetR(&BackRect,180+(hero_tmp/10*30),0,30,64);
							else if(i==1)SetR(&BackRect,240+(hero_tmp/10*42),489,42,64);
							else if(i==2)SetR(&BackRect,300+(hero_tmp/10*36),0,36,64);
							else SetR(&BackRect,225+(hero_tmp/10*44),553,44,64);
						_DrawBmp(BackRect, hero[i].x*60+5, hero[i].y*60-5, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
						brk=true;
						break;
					}
				}
				if(brk)break;
			}
		}
	}
	hero_tmp++;
}

void FileIn(int num, FILE* Fp)
{
	for(int i=0; i<3; i++)
	{
		fprintf(Fp,"%1c",num%256);
		num<<=8;
	}
	fprintf(Fp,"%1c",num);
}

char load(char* fname)
{
	int i;
	FILE* Fp;
	if(!(Fp=fopen(fname,"rb")))return false;
	//��������
	fread(&devil.grade, sizeof(int), 1, Fp);
	fread(&devil.mp, sizeof(int), 1, Fp);
	
	//���̵�, �ð�(���̺����� ����ǥ�ø� ���� �߰��� ����)
	fread(&play_time, sizeof(int), 1, Fp);
	fread(&difficult, sizeof(int), 1, Fp);

	//��� ����
	for(int i=0; i<4; i++)
	{
		fread(&hero[i].life, sizeof(char), 1, Fp);
		fread(&hero[i].x, sizeof(int), 1, Fp);
		fread(&hero[i].y, sizeof(int), 1, Fp);
		fread(&hero[i].sleep, sizeof(int), 1, Fp);
	}
	//���� ����
	fread(map, sizeof(char), 120, Fp);
	fread(build, sizeof(char), 80, Fp);
	//�ֹε� ����
	for(int i=0; i<HUMANS; i++)
	{
		fread(&citizen.human[i].life, sizeof(char), 1, Fp);
		fread(&citizen.human[i].happy, sizeof(char), 1, Fp);
	}
	//��������Ʈ ����
	fread(&battle.spr[SMAX].hp, sizeof(int), 1, Fp);
	fread(&battle.spr[SMAX].mp, sizeof(int), 1, Fp);

	fclose(Fp);

	return true;
}

void save(char* fname)
{
	int i;
	FILE* Fp;
	Fp=fopen(fname,"wb");
	//��������
	fwrite(&devil.grade, sizeof(int), 1, Fp);
	fwrite(&devil.mp, sizeof(int), 1, Fp);
		
	//���̵�, �ð�(���̺����� ����ǥ�ø� ���� �߰��� ����)
	fwrite(&play_time, sizeof(int), 1, Fp);
	fwrite(&difficult, sizeof(int), 1, Fp);

	//��� ����
	for(int i=0; i<4; i++)
	{
		fwrite(&hero[i].life, sizeof(char), 1, Fp);
		fwrite(&hero[i].x, sizeof(int), 1, Fp);
		fwrite(&hero[i].y, sizeof(int), 1, Fp);
		fwrite(&hero[i].sleep, sizeof(int), 1, Fp);
	}
	//���� ����
	fwrite(map, sizeof(char), 120, Fp);
	fwrite(build, sizeof(char), 80, Fp);
	//�ֹε� ����
	for(int i=0; i<HUMANS; i++)
	{
		fwrite(&citizen.human[i].life, sizeof(char), 1, Fp);
		fwrite(&citizen.human[i].happy, sizeof(char), 1, Fp);
	}
	//��������Ʈ ����
	fwrite(&battle.spr[SMAX].hp, sizeof(int), 1, Fp);
	fwrite(&battle.spr[SMAX].mp, sizeof(int), 1, Fp);

	fclose(Fp);
}

///////////////////////////////////////////////////////////////////////////////////////////
//�����ڵ�
///////////////////////////////////////////////////////////////////////////////////////////
long FAR PASCAL WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch ( message )
	{
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

		case WM_MOVE		 :	if(jdd)jdd->OnMove(LOWORD(lParam), HIWORD(lParam));
								break;
		
		case WM_SIZE		 :	if(wParam == SIZE_MINIMIZED)activate=false;
								else activate=true;
								break;
		
		case WM_ACTIVATE	 : if(LOWORD(wParam))activate=true;
								else activate=false;
							   break;

		case WM_SYSCOMMAND	 :  //�ݱ� �޽��� ����ä��
								if(wParam==SC_CLOSE)
								{
									wParam=0;
									exit(0);
								}
								break;
	}
    return DefWindowProc( hWnd, message, wParam, lParam );
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{	
	jdd=CreateDirectDraw();
	jre=CreateDXResourceManager(jdd);

	WNDCLASS wc={0};
	wc.hIcon=LoadIcon(hInstance,"newdevil.ico");
	wc.hCursor=LoadCursor(hInstance,IDC_ARROW);
	wc.lpfnWndProc=WindowProc;
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

		MainHwnd = CreateWindow("Game", "����ź��", ws, 100, 100, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);
		ShowCursor( TRUE );
	}
	else
	{
		MainHwnd=CreateWindow("Game","����ź��",WS_POPUP|WS_VISIBLE,0,0,SCREEN_X,SCREEN_Y,NULL,NULL,hInstance,NULL);
		ShowCursor( FALSE );
	}

	if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
	{
		SoundCard = TRUE;
		if (SoundOBJ->SetCooperativeLevel(MainHwnd,DSSCL_PRIORITY)!=DS_OK) return 0;

		memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
		DSB_desc.dwSize = sizeof(DSBUFFERDESC);
		DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

		if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK) return 0;
		SoundDSB -> SetVolume(0);
		SoundDSB -> SetPan(0);
	}
	else SoundCard = FALSE;

	jdd->Initialize(NULL,MainHwnd,SCREEN_X,SCREEN_Y,16,true,window_mode);
	
	//������â �̵�
	if(window_mode)
	{
		jdd->OnMove(100, 100);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}

	Init();

	//�ѱ� �ʱ�ȭ
    //InitXddFont( "HANGUL.FNT", "ENGLISH.FNT" );
    //SetFontPattern( NORMAL_PATTERN );

	//�׷��� �ʱ�ȭ
	BmpScreen[0] = DDLoadBitmap( DirectOBJ, "data//back.bmp", 0, 0, SYSTEM);
	BmpScreen[1] = DDLoadBitmap( DirectOBJ, "data//land.bmp", 0, 0, SYSTEM);
	BmpScreen[2] = DDLoadBitmap( DirectOBJ, "data//state.bmp", 0, 0, SYSTEM);
	BmpScreen[3] = DDLoadBitmap( DirectOBJ, "data//sprite.bmp", 0, 0, SYSTEM);
	BmpScreen[4] = DDLoadBitmap( DirectOBJ, "data//magic.bmp", 0, 0, SYSTEM);
	BmpScreen[5] = DDLoadBitmap( DirectOBJ, "data//grade.bmp", 0, 0, SYSTEM);
	BmpScreen[6] = DDLoadBitmap( DirectOBJ, "data//menu.bmp", 0, 0, SYSTEM);
	BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//logo.bmp", 0, 0, SYSTEM);
	BmpScreen[8] = DDLoadBitmap( DirectOBJ, "data//text.bmp", 0, 0, SYSTEM);
	BmpScreen[9] = DDLoadBitmap( DirectOBJ, "data//mask2.bmp", 0, 0, SYSTEM);
	DDSetColorKey( BmpScreen[1], JColor(0,0,255) );
	DDSetColorKey( BmpScreen[2], JColor(0,0,255) );
	DDSetColorKey( BmpScreen[3], JColor(0,0,255) );
	DDSetColorKey( BmpScreen[4], JColor(0,0,255) );
	DDSetColorKey( BmpScreen[5], JColor(0,0,255) );
	DDSetColorKey( BmpScreen[6], JColor(0,0,255) );
	DDSetColorKey( BmpScreen[8], JColor(0,0,255) );
	DDSetColorKey( BmpScreen[9], JColor(0,0,0) );

	//���� �ʱ�ȭ
	if ( SoundCard )
		{
			Sound[0] = SndObjCreate(SoundOBJ,"Sound//fire.WAV",2);
			Sound[1] = SndObjCreate(SoundOBJ,"Sound//heel.WAV",2);
			Sound[2] = SndObjCreate(SoundOBJ,"Sound//scream.WAV",2);
			Sound[3] = SndObjCreate(SoundOBJ,"Sound//frz.WAV",2);
			Sound[4] = SndObjCreate(SoundOBJ,"Sound//puck.WAV",2);
			Sound[5] = SndObjCreate(SoundOBJ,"Sound//bang.WAV",2);
			Sound[6] = SndObjCreate(SoundOBJ,"Sound//wind.WAV",2);
			Sound[7] = SndObjCreate(SoundOBJ,"Sound//jump.WAV",2);
			Sound[8] = SndObjCreate(SoundOBJ,"Sound//hat.WAV",2);
			Sound[9] = SndObjCreate(SoundOBJ,"Sound//eat.WAV",2);
			Sound[10] = SndObjCreate(SoundOBJ,"Sound//scream2.WAV",2);
			Sound[11] = SndObjCreate(SoundOBJ,"Sound//evil.WAV",2);
			Sound[12] = SndObjCreate(SoundOBJ,"Sound//jjik.WAV",2);
			Sound[13] = SndObjCreate(SoundOBJ,"Sound//boom.WAV",2);
			Sound[14] = SndObjCreate(SoundOBJ,"Sound//summon.WAV",2);
			Sound[15] = SndObjCreate(SoundOBJ,"Sound//warning.WAV",2);
			Sound[16] = SndObjCreate(SoundOBJ,"Sound//magic.WAV",2);
			Sound[17] = SndObjCreate(SoundOBJ,"Sound//build.WAV",2);
			Sound[18] = SndObjCreate(SoundOBJ,"Sound//revolt.WAV",2);
			Sound[19] = SndObjCreate(SoundOBJ,"Sound//scream3.WAV",2);
			Sound[20] = SndObjCreate(SoundOBJ,"Sound//heart.WAV",2);
        }
	//���� �ʱ�ȭ
	randomize();
	DWORD curTempo=GetTickCount();
	gam=true;
	
	int x, y;
	int scroll = 0, text_y, end_mode = 0;	//������ ���Ǵ� ����
	while( !Quit ){
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		bool study;						//�������� �˻�
		int lesson, delay;	//���� ���� ����
		//////////////////////////////////////////////////////////////////
		//������
		if(game_mode==0)
		{
			//ȥ���� ��� �ΰ�
			SetR(&BackRect,0,0,800,600);
			_DrawBmp(BackRect, 0, 0, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			_CopyScreen(false);
			_Delay(1000);
			Change(99);
			//�����ڿ�ũ�� �ΰ�
			BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//rw.bmp", 0, 0, SYSTEM);
			BmpScreen[9] = DDLoadBitmap( DirectOBJ, "data//mask.bmp", 0, 0, SYSTEM);
			DDSetColorKey( BmpScreen[9], JColor(255,255,255) );
			SetR(&BackRect,0,0,800,600);
			_DrawBmp(BackRect, 0, 0, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			_CopyScreen(false);
			_Delay(1000);
			Change(99);
			BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//open1.bmp", 0, 0, SYSTEM);
			scroll=600; end_mode=0;
			_MidiPlay("MUSIC//open.mid",false);
			game_mode=7;
		}
		else if(game_mode==7)
		{
			int ty;
			if(end_mode==0)
			{
				SetR(&BackRect,0,0,640,480);
				_DrawBmp(BackRect, 80, 20, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				if(scroll>520)ty=scroll;
					else if(scroll>440)ty=520;
					else
					{
						Change(99);
						BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//open2.bmp", 0, 0, SYSTEM);
						ty=scroll=600; end_mode=1;
					}
				_LockScreen();
				_DrawBar(0,520,800,600,BLACK);
				PutFont(40,ty,WHITE,"��� ȥ���� ����� ��...");
				PutFont(40,ty+20,WHITE,"����� ���� ������ ������ ȥ���� ������⹰�� �����ϴ� ������ ������ �����ϴ� ���̾���.");
				PutFont(40,ty+40,WHITE,"������ �ű⼭ �� ������ ���ܳ��� �״� ������ �ٿ��� �ȴ�.");
				PutFont(40,ty+60,WHITE,"�׸��� �� �� �������� ���� ����, ����� 1���� �����Ѵ�.");
				_UnlockScreen();
				scroll--;
			}
			else if(end_mode==1)
			{
				SetR(&BackRect,0,0,640,480);
				_DrawBmp(BackRect, 80, 20, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				if(scroll>520)ty=scroll;
					else if(scroll>440)ty=520;
					else
					{
						Change(99);
						BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//open3.bmp", 0, 0, SYSTEM);
						ty=scroll=600; end_mode=2;
					}
				_LockScreen();
				_DrawBar(0,520,800,600,BLACK);
				PutFont(40,ty,WHITE,"����� ���� ������ ���� �Ǿ���, ���ռ��� ����� ������ ������ �����Ѵ�.");
				PutFont(40,ty+20,WHITE,"������ ����� ������ ���ؿ� ����� ��Ÿ������ �� ����� ������ �����.");
				PutFont(40,ty+40,WHITE,"�׸��� ����� 2���� ����� �������� ���Ƴ����� �� ����� �ٽ� ����ȴ�.");
				PutFont(40,ty+60,WHITE,"����� 2���� �Ƶ��� �Ǹ��� ������ ������� �ΰ����� ��� ȥ���� ������� ���Եȴ�.");
				_UnlockScreen();
				scroll--;
			}
			else
			{
				SetR(&BackRect,0,0,540,300);
				_DrawBmp(BackRect, 160, 150, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				if(scroll>520)ty=scroll;
					else if(scroll>440)ty=520;
					else
					{
						Change(100);
						ty=scroll=600; end_mode++;
					}
				_LockScreen();
				_DrawBar(0,520,800,600,BLACK);
				if(end_mode==2)
				{
					PutFont(40,ty,WHITE,"��, ���Ŷ�. �Ƶ��. ���Ⱑ �ΰ����� ��� ���̴�.");
					PutFont(40,ty+20,WHITE,"�������� ������ �Ǳ� ���� ������ ���۵ȴ�.");
					PutFont(40,ty+40,WHITE,"�ʴ� �� ���� ������� ��Ƹ����� ������ �Ǿ�� �Ѵ�.");
					PutFont(40,ty+60,WHITE,"�ֹ��� �� �װų�, ��糪 �ֹε鿡�� ���Ѵٸ� �� ������ �ڰ��� ����.");
				}
				else PutFont(40,ty,WHITE,"�� ��   �� �� ��   �� �� �� �� .");
				_UnlockScreen();
				scroll--;
			}
			_CopyScreen(false);
			_Delay(100);
			//�ߴ�
			if(_GetKeyState(VK_ESCAPE) || _GetKeyState(VK_RETURN) || end_mode==4)
			{
				_MidiStop();
				BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//main.bmp", 0, 0, SYSTEM);
				game_mode=1;
				Change(99);
			}
		}
		//////////////////////////////////////////////////////////////////
		//�޴�
		else if(game_mode==1)
		{
			int ButtonX[]={261,466,31,333,688};
			int ButtonY[]={304,314,483,453,490};

			static int heart;
			if(heart<0 || heart>=4)heart=0;
			int frame=heart;
			if(frame==3)frame=1;

			Select=-1;
			if(popup<0)
			{
				SetR(&BackRect,0,0,800,600);	//���
				_DrawBmp(BackRect, 0, 0, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			}
			else if(popup==0)	//���̵� ����
			{
				SetR(&BackRect,0,300,800,300);
				_DrawBmp(BackRect, 0, 0, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				_DrawBmp(BackRect, 0, 300, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				SetR(&BackRect,0,popup*30,60,30);
				_DrawBmp(BackRect, 370, 15, BmpScreen[8], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

				//���̵� ����
				_LockScreen();
				PutFontOutline(385,60,YELLOW,"����");
				PutFontOutline(255,80,WHITE,"�������� �ֹ��ູ ������� ����ǳ��");
				PutFontOutline(385,110,GREEN,"����");
				PutFontOutline(255,130,WHITE,"�������� �ֹ���� ������ ���º���");
				PutFontOutline(377,160,RED,"�����");
				PutFontOutline(255,180,WHITE,"����ô�� �ֹκҸ� ���ٹ� ���º���");
				_UnlockScreen();
				//����
				for(int i=0; i<3; i++)
				{
					if(MouseX>200 && MouseX<600 && MouseY>60+(i*50) && MouseY<100+(i*50))
					{
						_DrawBox(250-frame,60-frame+(i*50),300+(frame*2),40+(frame*2),RED);
						if(LeftButton)Select=i;
						break;
					}
				}
				if(Select>=0)
				{
					difficult=Select+1;
					study=false;
					game_mode=2;
				}
			}
			else if(popup==3)	//���̵� ����
			{
				SetR(&BackRect,0,300,800,300);
				_DrawBmp(BackRect, 0, 0, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				_DrawBmp(BackRect, 0, 300, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				SetR(&BackRect,0,popup*30,60,30);
				_DrawBmp(BackRect, 370, 15, BmpScreen[8], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

				//���̵� ����
				_LockScreen();
				PutFontOutline(285,60,WHITE ,"        ���� ȭ�� ����");
				PutFontOutline(285,80,WHITE ,"           �������");
				PutFontOutline(285,100,WHITE,"          �ֹε� ����");
				//����
				for(int i=0; i<3; i++)
				{
					if(MouseX>200 && MouseX<600 && MouseY>60+(i*20) && MouseY<80+(i*20))
					{
						_DrawBox(250-frame,60-frame+(i*20),300+(frame*2),20+(frame*2),RED);
						if(LeftButton)Select=i;
						break;
					}
				}
				_UnlockScreen();
				if(Select>=0)
				{
					lesson=Select;
					delay=3600;
					study=true;
					game_mode=2;
				}
			}
			else if(popup<3)	//����â
			{
				SetR(&BackRect,0,300,800,300);
				_DrawBmp(BackRect, 0, 0, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				_DrawBmp(BackRect, 0, 300, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				SetR(&BackRect,0,popup*30,60,30);
				_DrawBmp(BackRect, 370, 15, BmpScreen[8], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

				//���̺� ���ϵ�
				for(int i=0; i<5; i++)
				{
					int ty=60+i*50;
					if(fabl[i])
					{
						SetR(&BackRect,0,fnum[i]*40,120,40);
						_DrawBmp(BackRect, 250, ty, BmpScreen[5], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
						_LockScreen();
						PutFontOutline(375,ty,WHITE,"����:");
						PutFontOutline(415,ty,JColor(128,128,255),"%d",fmp[i]);
						PutFontOutline(460,ty,WHITE,"���̵�:");
						if(fdifficult[i]==1)PutFontOutline(520,ty,YELLOW,"��");
							else if(fdifficult[i]==2)PutFontOutline(520,ty,GREEN,"��");
							else PutFontOutline(520,ty,RED,"��");
						PutFontOutline(375,ty+20,WHITE,"�ҿ�ð�:");
						PutFontOutline(450,ty+20,JColor(128,255,128),"%d",ftime[i]);
						_UnlockScreen();
					}
					else
					{
						_LockScreen();
						PutFontOutline(360,ty+10,BLUE,"����ִٳ�.");
						_UnlockScreen();
					}
				}
				//����
				for(int i=0; i<5; i++)
				{
					if(MouseX>200 && MouseX<600 && MouseY>60+(i*50) && MouseY<100+(i*50))
					{
						_DrawBox(250-frame,60-frame+(i*50),300+(frame*2),40+(frame*2),RED);
						if(LeftButton)
						{
							if(popup==1 && !fabl[i])
								continue;
							else
								Select=i;
						}
						break;
					}
				}
				if(Select>=0)
				{
					if(popup==1)	//�ε�
						game_mode=5;
					else			//���̺�
						save(fname[Select]);
					popup=-1;
				}
			}

			int result=-1;
			for(int i=0; i<5; i++)
			{
				if(i==3 && !game_start)continue;
				SetR(&BackRect,frame*100,i*100,100,100);	//����
				_DrawBmp(BackRect, ButtonX[i], ButtonY[i], BmpScreen[6], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			
				if(LeftButton)
				{
					if(MouseX>ButtonX[i]-25 && MouseY>ButtonY[i]-20 && MouseX<ButtonX[i]+100 && MouseY<ButtonY[i]+100)
					{
						result=i;
						LeftButton=false;
						_Play(12);
					}
				}
			}
			SetR(&BackRect,120,165,65,55);	//���ռ�
			_DrawBmp(BackRect, MouseX, MouseY, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

			if(result==0)popup=0;
				else if(result==2)popup=3;	//����
				else if(result==1 || result==3) //���, �̾
				{
					if(result==1)popup=1;
						else popup=2;
					for(int i=0;i<5;i++)
					{
						fabl[i]=true;
						FILE* Fp;
						if(!(Fp=fopen(fname[i],"rb"))){
							fabl[i]=false;
							continue;
						}
						fread(&fnum[i],sizeof(int),1,Fp);
						fread(&fmp[i],sizeof(int),1,Fp);
						fread(&ftime[i],sizeof(int),1,Fp);
						fread(&fdifficult[i],sizeof(int),1,Fp);
						fclose(Fp);
					}
				}
				else if(result==4)Quit=true;

			if(curTempo+200<GetTickCount())
			{
				if(heart==3)_Play(20);
				heart++;
				curTempo=GetTickCount();
			}
			_CopyScreen(true);
			//���� ��������
			static bool key_esc;
			if(_GetKeyState(VK_ESCAPE))
			{
				if(!key_esc)
				{
					if(popup>=0)
						popup=-1;
					else if(game_start)
					{
						Change(99);
						game_mode=3;
					}
					key_esc=true;
				}
			}
			else key_esc=false;
		}
		//////////////////////////////////////////////////////////////////
		//�ʱ�ȭ
		else if(game_mode==2)
		{
			//��������
			devil.Clear();
			for(int k=0; k<4; k++)
			{
				hero[k].Clear();
				hero[k].y=k*3;
			}
			citizen.Clear();
			
			//�� ����
			for(x=0; x<10; x++)
				for(y=0; y<12; y++)
				{
					if(x==0 || y==0 || x==10 || y==12)map[x][y]=5;
					int tmp=rand()%4;
					if(tmp==0)map[x][y]=0;
					else map[x][y]=1;
				}
			//�ٸ� ���� �����
			for(int j=0; j<3; j++)
			{
				x=rand()%8+1; y=rand()%10+1;
				for(int i=0; i<(difficult*5); i++)
				{
					map[x][y]=2+j;
					int tmp=rand()%4;
					if(tmp==0)x++;
					else if(tmp==1)x--;
					else if(tmp==2)y++;
					else y--;
					x=MaxMin(x,8,1);
					y=MaxMin(y,10,1);
				}
			}
			//�� �����
			if(!study)
			{
				x=rand()%8+1; y=rand()%10+1;
				do
				{
					map[x][y]=5;
					int tmp=rand()%4;
					if(tmp==0)x++;
					else if(tmp==1)x--;
					else if(tmp==2)y++;
					else y--;
				}
				while(x>0 && y>0 && x<9 && y<11);
			}

			//���� �ʱ�ȭ
			for(int i=0; i<10; i++)citizen.NewHuman(4,4);
			battle.Battle(1);
			devil.mp=battle.spr[SMAX].mp=0;
			//�� Ÿ�� ��, ���� ���� ���
			waters=devil.inc_mp=0;
			for(x=1; x<9; x++)
				for(y=1; y<11; y++)
				{
					if(map[x][y]==5)waters++;
						else devil.inc_mp+=(map[x][y]/difficult);
				}
			max_human=HUMANS-(waters*4);
			devil.dec_mp=0;
			for(x=0; x<8; x++)
				for(y=0; y<10; y++)
					build[x][y]=-1;

			Change(99);
			game_start=true;
			popup=-1;
			play_time=chk_speed=0;
			if(study)
			{
				game_mode=8;
				_MidiPlay("MUSIC//study.mid",true);
			}
			else
			{
				game_mode=3;
				_MidiPlay("MUSIC//beast.mid",true);				
			}
		}
		//////////////////////////////////////////////////////////////////
		//����
		else if(game_mode==3)
		{
			/*if(curTempo+10>GetTickCount())continue;	//��ǻ��CPU�� ������� �ʰ�
				else curTempo=(GetTickCount()+(GetTickCount()-(curTempo+10)));*/
			SetMap();
			//�ֱ⿡ ���� ��� �� �׸�
			if(chk_speed++ > 20)
			{
				play_time++;
				//����� ����
				for(int i=0; i<4; i++)
				{
					if(hero[i].sleep>0)
					{
						if(hero[i].sleep<999)hero[i].sleep--;
					}
					else
					{
						if(!hero[i].life)
						{
							bool find=false;
							for(int k=0; k<4; k++)
							{
								int tmp=((hero[i].y*MAX_X+hero[i].x)*4)+k;
								if(citizen.human[tmp].life && citizen.human[tmp].happy==-100)
								{
									hero[i].life=find=true;	//�����ϴ� �ù��� ������ ����
									break;
								}
							}
							//����� �̵�
							if(!find)
							{
								bool is_hero;
								do{
									{
										hero[i].x++;
										if(hero[i].x>=MAX_X)
										{
											hero[i].x=0;
											hero[i].y++;
											if(hero[i].y>=MAX_Y)hero[i].y=0;
										}
									}
									//���� ��ġ�� �ٸ� ��簡 ������ �˻�
									is_hero=false;
									for(int k=0; k<4; k++)
									{
										if(i==k)continue;
										if(hero[k].life && hero[i].y==hero[k].y && hero[i].x==hero[k].x)is_hero=true;
									}
								}while(is_hero);
							}
						}
					}
				}
				
				static int delay;
				if(delay<0 || delay>difficult*2)
				{
					delay=0;
					battle.spr[SMAX].mp=Min(battle.spr[SMAX].mp+devil.inc_mp-devil.dec_mp, devil.mp);	//���� ����
				}
				delay++;
				citizen.IncHappy();		//�ູ�� ��ȭ
				chk_speed=0;
			}
			//���� ����
			citizen.CalHappy();
			//���� ���
			if(devil.eyeland<5 && noise<=0)
			{
				SetR(&BackRect,devil.eyeland*320,0,320,300);
				_DrawBmp(BackRect, 480, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			}
			else
			{
				if(noise<0)noise=29;
				SetR(&BackRect,1600+(noise/10)*320,0,320,300);
				_DrawBmp(BackRect, 480, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				noise--;
			}
			//����â
			//ü��
			int hp=100-battle.spr[SMAX].hp;
			SetR(&BackRect,20,600,10,2);
			for(int i=0; i<100; i++)
			{
				_DrawBmp(BackRect, 520, 350+i*2, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				if(hp==i)SetR(&BackRect,0,600,10,2);
			}
			//����
			int mp;
			
			if(devil.mp>0)mp=100-(battle.spr[SMAX].mp*100/devil.mp);
				else mp=-1;
			SetR(&BackRect,20,600,10,2);
			for(int i=0; i<100; i++)
			{
				_DrawBmp(BackRect, 560, 350+i*2, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				if(mp==i)SetR(&BackRect,10,600,10,2);
			}
			SetR(&BackRect,0,300,320,300);
			_DrawBmp(BackRect, 480, 300, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			SetR(&BackRect,0,devil.grade*40,120,40);
			_DrawBmp(BackRect, 660, 305, BmpScreen[5], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			//ü��,���� ��ġ
			_LockScreen();
			PutFontOutline(20,0,YELLOW,"�α���:%d/%d",citizen.number,max_human);
			PutFontOutline(140,0,YELLOW,"�ູ��:%d",citizen.happy);
			PutFontOutline(260,0,YELLOW,"���¼���:%d",devil.inc_mp);
			PutFontOutline(380,0,YELLOW,"���¼Һ�:%d",devil.dec_mp);
			PutFontOutline(520+Xpos(battle.spr[SMAX].hp),330,JColor(195,0,0),"%d",battle.spr[SMAX].hp);
			PutFontOutline(560+Xpos(battle.spr[SMAX].mp),330,JColor(128,128,255),"%d",battle.spr[SMAX].mp);
			_UnlockScreen();
			
			//���캸��
			devil.tempo++;
			if(devil.tempo>=30 || devil.tempo<0)devil.tempo=0;
			SetR(&BackRect,0,325+(devil.tempo/10)*65,60,65);
			_DrawBmp(BackRect, devil.eyeX*60, devil.eyeY*60, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			if(LeftButton)
			{
				if(!key_left)
				{
					if(MouseX<455 && MouseY<580)
					{
						int lx=(MouseX+25)/60; int ly=(MouseY+20)/60;
						if(devil.eyeX!=lx || devil.eyeY!=ly)
						{
							devil.eyeX=lx; devil.eyeY=ly;
							devil.eyeland=map[lx+1][ly+1];

							bool revolt=false;
							for(int i2=0; i2<4; i2++)
							{
								int tmp=(devil.eyeY*MAX_X+devil.eyeX)*4+i2;
								if(citizen.human[tmp].life)
								{
									battle.etc[i2]=citizen.human[tmp].happy;
									if(battle.etc[i2]==-100)revolt=true;
								}
								else battle.etc[i2]=-111;
							}
							if(revolt)_Play(18); //�ݶ��� �Լ�
							//��� ���翩��
							battle.etc[4]=-1;
							for(int i2=0; i2<4; i2++)
							{
								if(hero[i2].life)
								{
									if(devil.eyeX==hero[i2].x && devil.eyeY==hero[i2].y)
									{
										battle.etc[4]=i2;
										break;
									}
								}
							}
							battle.etc[6]=(devil.eyeX+devil.eyeY)%2;	//������ ���� ���� ����
							battle.Battle(2);
							noise=29;		//�ٷ� â�� �ٲ��� �ʰ�
						}
					}
					key_left=true;
				}
			}
			else key_left=false;
			//���� ��Ȳ ó��
			//���� �Է�
			if(devil.eyeland<5)
			{
				for(int i2=0; i2<4; i2++)
				{
					int tmp=(devil.eyeY*MAX_X+devil.eyeX)*4+i2;
					if(citizen.human[tmp].life)battle.etc[i2]=citizen.human[tmp].happy;
						else battle.etc[i2]=-111;
				}
				battle.etc[4]=-1;
				for(int i2=0; i2<4; i2++)
				{
					if(hero[i2].life)
					{
						if(devil.eyeX==hero[i2].x && devil.eyeY==hero[i2].y)
						{
							battle.etc[4]=i2;
							break;
						}
					}
				}
				//����
				battle.etc[5]=battle.etc[9]=0;
				if(devil.grade<1)battle.etc[10]=1;
					else if(devil.grade<4)battle.etc[10]=2;
					else battle.etc[10]=3;
				battle.etc[11]=noise;
				battle.etc[12]=-1;
				battle.Battle(0);
				//��� �ޱ�
				for(int i2=0; i2<4; i2++)	//�ֹ� �������
				{
					if(battle.etc[i2]==-111)citizen.human[(devil.eyeY*MAX_X+devil.eyeX)*4+i2].life=false;
				}
				if(battle.etc[4]>3)	//��� �������
				{
					int tmp=battle.etc[4]-4;
					hero[tmp].life=false;
					hero[tmp].sleep=200*(4-difficult);
					bool is_hero;
					do{
						hero[tmp].x=rand()%MAX_X;
						hero[tmp].y=rand()%MAX_Y;
						is_hero=false;
						for(int k=0; k<4; k++)
						{
							if(tmp==k)continue;
							if(hero[k].life && hero[tmp].y==hero[k].y && hero[tmp].x==hero[k].x)is_hero=true;
						}
					}while(is_hero);
				}
				if(battle.etc[9]>0)	//��������
				{
					devil.mp+=battle.etc[9];
					battle.etc[9]=0;
					if(devil.mp>=next_grade[devil.grade])
					{
						devil.grade++;
						if(devil.grade==1)	//ī�� ����
						{
							hero[0].sleep=0;
							_MidiPlay("MUSIC//monster.mid",true);
						}
						else if(devil.grade==3)	//�������� ����
						{
							hero[1].sleep=0;
						}
						else if(devil.grade==4)	//��� ����
						{
							hero[2].sleep=0;
							_MidiPlay("MUSIC//devil.mid",true);
						}
						else if(devil.grade==6)	//õ���� ��� ����
						{
							hero[3].sleep=0;
						}
					}
				}
				if(battle.etc[12]>=0)
				{
					int build_mp[]={3,1,2};
					if(battle.etc[12]<3)
					{
						if(build[devil.eyeX][devil.eyeY]<0)
						{
							if(devil.inc_mp>=devil.dec_mp+build_mp[battle.etc[12]])
							{
								build[devil.eyeX][devil.eyeY]=battle.etc[12];
								devil.dec_mp+=build_mp[battle.etc[12]];
								_Play(17);
							}
							else _Play(15);
						}
					}
					else if(build[devil.eyeX][devil.eyeY]>=0)
					{
						devil.dec_mp-=build_mp[build[devil.eyeX][devil.eyeY]];
						build[devil.eyeX][devil.eyeY]=-30;
						_Play(13);
					}
				}
			}
			else
			{
				battle.etc[5]=999;
				battle.Battle(0);
			}

			//���ֺκ�
			for(int i=0;i<SNDMAX;i++)
				if(battle.play[i]>=0)
				{
					_Play(battle.play[i]);
					battle.play[i]=-1;
				}

			_CopyScreen(true);

			//�޴���
			if(_GetKeyState(VK_ESCAPE))
			{
				Change(99);
				game_mode=1;
			}

			//���ӿ��� ó��
			if(!battle.spr[SMAX].life)
			{
				BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//end1.bmp", 0, 0, SYSTEM);
				_MidiPlay(midi[5]);
				Change(99);
				game_mode=4;
			}
			else if(citizen.happy<=-100 && citizen.number>=50)
			{
				BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//end2.bmp", 0, 0, SYSTEM);
				_MidiPlay(midi[5]);
				Change(99);
				game_mode=4;
			}
			else if(citizen.number<=0)
			{
				BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//end3.bmp", 0, 0, SYSTEM);
				_MidiPlay(midi[6]);
				Change(99);
				game_mode=4;
			}
			else if(devil.grade==7)
			{
				BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//end4.bmp", 0, 0, SYSTEM);
				_MidiPlay(midi[4]);
				Change(99);
				scroll=558;
				text_y=600;
				end_mode=0;
				game_mode=6;
			}
		}
		else if(game_mode==4)	//���ӿ���
		{
			SetR(&BackRect,0,0,800,600);	//���
			_DrawBmp(BackRect, 0, 0, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			_CopyScreen(true);
			if(_GetKeyState(VK_ESCAPE) || _GetKeyState(VK_RETURN))
			{
				BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//main.bmp", 0, 0, SYSTEM);
				Change(99);
				_MidiStop();
				game_start=false;
				game_mode=1;
			}
		}
		else if(game_mode==5)	//�ε带 ���� �ʱ�ȭ
		{
			//��������
			devil.Clear();
			for(int k=0; k<4; k++)hero[k].Clear();
			citizen.Clear();
			battle.Battle(1);
			
			load(fname[Select]);

			//�� Ÿ�� ��, ���� ���� ���
			waters=devil.inc_mp=devil.dec_mp=0;
			for(x=1; x<9; x++)
				for(y=1; y<11; y++)
				{
					if(map[x][y]==5)waters++;
						else devil.inc_mp+=(map[x][y]/difficult);
				}
			max_human=HUMANS-(waters*4);
			for(x=0; x<8; x++)
				for(y=0; y<10; y++)
				{
					int build_mp[]={3,1,2};
					if(build[x][y]>=0)devil.dec_mp+=build_mp[build[x][y]];
				}

			game_start=key_left=true;
			for(int i=0; i<SNDMAX; i++)battle.play[i]=-1;
			if(devil.grade<1)_MidiPlay("MUSIC//beast.mid",true);
				else if(devil.grade<4)_MidiPlay("MUSIC//monster.mid",true);
				else _MidiPlay("MUSIC//devil.mid",true);
			Change(99);
			chk_speed=0;
			game_mode=3;
		}
		else if(game_mode==6)	//����, ���
		{
			if(end_mode==0)SetR(&BackRect,0,scroll,800,600);
				else SetR(&BackRect,0,0,800,600);
			_DrawBmp(BackRect, 0, 0, BmpScreen[7], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			_LockScreen();
			if(end_mode==0)
			{
				PutFontOutline(50,text_y,WHITE,"������ ź���Ͽ�����!");
				PutFontOutline(50,text_y+40,WHITE,"ó�� 200���� ��Ƹ԰� ������ ź���Ͽ�����.");
				PutFontOutline(50,text_y+80,WHITE,"���� �̸��� ����� 3��.");
				PutFontOutline(50,text_y+120,WHITE,"����� ����� 3��° �����̵���.");
				PutFontOutline(50,text_y+160,WHITE,"�׸��� �ΰ�, ����, �ֽ��ڵ���");
				PutFontOutline(50,text_y+200,WHITE,"��������� �ô��� ����...");
				PutFontOutline(50,text_y+520,WHITE,"�װ� ��·�ٰ�...");
			}
			else
			{
				PutFontOutline(40,40,WHITE,"���̵�:");
				if(difficult==1)PutFontOutline(100,40,YELLOW,"��");
					else if(difficult==2)PutFontOutline(100,40,GREEN,"��");
					else PutFontOutline(100,40,RED,"��");
				PutFontOutline(40,60,WHITE,"�ҿ�ð�:");
				PutFontOutline(115,60,JColor(128,255,128),"%d",play_time);
				PutFontOutline(40,100,WHITE,"����� �Ƿ���");
				int imax=(300-scroll)/10;
				for(int i=0; i<imax; i++)
					PutFontOutline(145+i*5,100,WHITE,".");
			}
			_UnlockScreen();
			if(end_mode==1 && scroll==0)
			{
				int score=Max(0,(500*difficult*difficult+500)-play_time);
				if(score>=1000)devil.grade=7;
					else if(score>=900)devil.grade=6;
					else if(score>=700)devil.grade=5;
					else if(score>=500)devil.grade=4;
					else if(score>=300)devil.grade=3;
					else if(score>=150)devil.grade=2;
					else if(score>=50)devil.grade=1;
					else devil.grade=0;
				_LockScreen();
				PutFontOutline(40,160,WHITE,"����:%d",score);
				_UnlockScreen();
				SetR(&BackRect,0,devil.grade*40,120,40);
				_DrawBmp(BackRect, 40, 180, BmpScreen[5], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
			_CopyScreen(true);
			_Delay(1);
			if(scroll>0)
				scroll--;
			else
			{
				if(end_mode==0)
				{
					_Delay(5000);
					end_mode=1;
					scroll=300;
				}
				else
				{
					if(_GetKeyState(VK_ESCAPE) || _GetKeyState(VK_RETURN))
					{
						BmpScreen[7] = DDLoadBitmap( DirectOBJ, "data//main.bmp", 0, 0, SYSTEM);
						Change(99);
						_MidiStop();
						game_mode=1;
					}
				}
			}
			if(text_y>40)text_y--;
			
		}
		//////////////////////////////////////////////////////////////////
		//����
		else if(game_mode==8)
		{
			if(curTempo+10>GetTickCount())continue;	//��ǻ��CPU�� ������� �ʰ�
				else curTempo=GetTickCount();
			SetMap();
			//�ֱ⿡ ���� ��� �� �׸�
			if(chk_speed++ > 20)
			{
				battle.spr[SMAX].mp=Min(battle.spr[SMAX].mp+devil.inc_mp-devil.dec_mp, devil.mp);	//���� ����
				citizen.IncHappy();		//�ູ�� ��ȭ
				chk_speed=0;
			}
			//���� ����
			citizen.CalHappy();
			//���� ���
			if(devil.eyeland<5 && noise<=0)
			{
				SetR(&BackRect,devil.eyeland*320,0,320,300);
				_DrawBmp(BackRect, 480, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			}
			else
			{
				if(noise<0)noise=29;
				SetR(&BackRect,1600+(noise/10)*320,0,320,300);
				_DrawBmp(BackRect, 480, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
				noise--;
			}
			//����â
			//ü��
			int hp=100-battle.spr[SMAX].hp;
			SetR(&BackRect,20,600,10,2);
			for(int i=0; i<100; i++)
			{
				_DrawBmp(BackRect, 520, 350+i*2, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				if(hp==i)SetR(&BackRect,0,600,10,2);
			}
			//����
			int mp;
			
			if(devil.mp>0)mp=100-(battle.spr[SMAX].mp*100/devil.mp);
				else mp=-1;
			SetR(&BackRect,20,600,10,2);
			for(int i=0; i<100; i++)
			{
				_DrawBmp(BackRect, 560, 350+i*2, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				if(mp==i)SetR(&BackRect,10,600,10,2);
			}
			SetR(&BackRect,0,300,320,300);
			_DrawBmp(BackRect, 480, 300, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			SetR(&BackRect,0,devil.grade*40,120,40);
			_DrawBmp(BackRect, 660, 305, BmpScreen[5], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			//ü��,���� ��ġ
			
			//���캸��
			devil.tempo++;
			if(devil.tempo>=30 || devil.tempo<0)devil.tempo=0;
			SetR(&BackRect,0,325+(devil.tempo/10)*65,60,65);
			_DrawBmp(BackRect, devil.eyeX*60, devil.eyeY*60, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			if(LeftButton)
			{
				if(!key_left)
				{
					if(MouseX<455 && MouseY<580)
					{
						int lx=(MouseX+25)/60; int ly=(MouseY+20)/60;
						if(devil.eyeX!=lx || devil.eyeY!=ly)
						{
							devil.eyeX=lx; devil.eyeY=ly;
							devil.eyeland=map[lx+1][ly+1];

							bool revolt=false;
							for(int i2=0; i2<4; i2++)
							{
								int tmp=(devil.eyeY*MAX_X+devil.eyeX)*4+i2;
								if(citizen.human[tmp].life)
								{
									battle.etc[i2]=citizen.human[tmp].happy;
									if(battle.etc[i2]==-100)revolt=true;
								}
								else battle.etc[i2]=-111;
							}
							if(revolt)_Play(18); //�ݶ��� �Լ�
							//��� ���翩��
							battle.etc[4]=-1;
							for(int i2=0; i2<4; i2++)
							{
								if(hero[i2].life)
								{
									if(devil.eyeX==hero[i2].x && devil.eyeY==hero[i2].y)
									{
										battle.etc[4]=i2;
										break;
									}
								}
							}
							battle.etc[6]=(devil.eyeX+devil.eyeY)%2;	//������ ���� ���� ����
							battle.Battle(2);
							noise=29;		//�ٷ� â�� �ٲ��� �ʰ�
						}
					}
					key_left=true;
				}
			}
			else key_left=false;
			//���� ��Ȳ ó��
			//���� �Է�
			if(devil.eyeland<5)
			{
				for(int i2=0; i2<4; i2++)
				{
					int tmp=(devil.eyeY*MAX_X+devil.eyeX)*4+i2;
					if(citizen.human[tmp].life)battle.etc[i2]=citizen.human[tmp].happy;
						else battle.etc[i2]=-111;
				}
				battle.etc[4]=-1;
				for(int i2=0; i2<4; i2++)
				{
					if(hero[i2].life)
					{
						if(devil.eyeX==hero[i2].x && devil.eyeY==hero[i2].y)
						{
							battle.etc[4]=i2;
							break;
						}
					}
				}
				//����
				battle.etc[5]=battle.etc[9]=0;
				if(devil.grade<1)battle.etc[10]=1;
					else if(devil.grade<4)battle.etc[10]=2;
					else battle.etc[10]=3;
				battle.etc[11]=noise;
				battle.etc[12]=-1;
				battle.Battle(0);
				//��� �ޱ�
				for(int i2=0; i2<4; i2++)	//�ֹ� �������
				{
					if(battle.etc[i2]==-111)citizen.human[(devil.eyeY*MAX_X+devil.eyeX)*4+i2].life=false;
				}
				if(battle.etc[4]>3)	//��� �������
				{
					int tmp=battle.etc[4]-4;
					hero[tmp].life=false;
					hero[tmp].sleep=200*(4-difficult);
					bool is_hero;
					do{
						hero[tmp].x=rand()%MAX_X;
						hero[tmp].y=rand()%MAX_Y;
						is_hero=false;
						for(int k=0; k<4; k++)
						{
							if(tmp==k)continue;
							if(hero[k].life && hero[tmp].y==hero[k].y && hero[tmp].x==hero[k].x)is_hero=true;
						}
					}while(is_hero);
				}
				if(battle.etc[9]>0)	//��������
				{
					devil.mp+=battle.etc[9];
					battle.etc[9]=0;
					if(devil.mp>=next_grade[devil.grade])devil.grade++;
				}
				if(battle.etc[12]>=0)
				{
					int build_mp[]={3,1,2};
					if(battle.etc[12]<3)
					{
						if(build[devil.eyeX][devil.eyeY]<0)
						{
							if(devil.inc_mp>=devil.dec_mp+build_mp[battle.etc[12]])
							{
								build[devil.eyeX][devil.eyeY]=battle.etc[12];
								devil.dec_mp+=build_mp[battle.etc[12]];
								_Play(17);
							}
							else _Play(15);
						}
					}
					else if(build[devil.eyeX][devil.eyeY]>=0)
					{
						devil.dec_mp-=build_mp[build[devil.eyeX][devil.eyeY]];
						build[devil.eyeX][devil.eyeY]=-30;
						_Play(13);
					}
				}
			}
			else
			{
				battle.etc[5]=999;
				battle.Battle(0);
			}

			//���ֺκ�
			for(int i=0;i<SNDMAX;i++)
				if(battle.play[i]>=0)
				{
					_Play(battle.play[i]);
					battle.play[i]=-1;
				}

			if(delay>0)
			{
				SetR(&BackRect,60,325,180,100);
				_DrawBmp(BackRect, 3, 349, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
				SetR(&BackRect,60,0,79,120);
				_DrawBmp(BackRect, 0, 480, BmpScreen[8], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
			_LockScreen();
			//����ġ�� �κ�
			if(delay>0)
			{
				if(lesson==0)	//����ȭ�� ����
				{
					if(delay>3300)
					{
						PutFont(18,370,BLACK,"���ݺ��� ����ź����");
						PutFont(18,390,BLACK,"��������� �սô�.");
					}
					else if(delay>3000)
					{
						PutFont(18,370,BLACK,"���� ȭ���� 3����");
						PutFont(18,390,BLACK,"â���� �����Ǿ�");
						PutFont(18,410,BLACK,"�ֽ��ϴ�.");
					}
					else if(delay>2700)
					{
						PutFont(18,370,BLACK,"������ ����â");
						PutFont(18,390,BLACK,"������ �Ʒ��� ����â");
						PutFont(18,410,BLACK,"������ ���� ����â");
					}
					else if(delay>2400)
					{
						PutFont(18,370,BLACK,"����â���� �� ����");
						PutFont(18,390,BLACK,"�����ϸ� �� �����");
						PutFont(18,410,BLACK,"����â�� ���ɴϴ�.");
					}
					else if(delay>2100)
					{
						PutFont(18,370,BLACK,"������ �����ϸ�");
						PutFont(18,390,BLACK,"������� ���Դϴ�.");
					}
					else if(delay>1800)
					{
						PutFont(18,370,BLACK,"����� Ŭ���ϸ�");
						PutFont(18,390,BLACK,"��Ƹ��� ��");
						PutFont(18,410,BLACK,"�ֽ��ϴ�.");
					}
					else if(delay>1500)
					{
						PutFont(18,370,BLACK,"���ڸ� ������");
						PutFont(18,390,BLACK,"ü���� 1 ȸ���ǰ�");
					}
					else if(delay>1200)
					{
						PutFont(18,370,BLACK,"���ڸ� ������");
						PutFont(18,390,BLACK,"���� �Ѱ谡");
						PutFont(18,410,BLACK,"1 �����ϴ�.");
					}
					else if(delay>900)
					{
						PutFont(18,370,BLACK,"���� 200���� ���");
						PutFont(18,390,BLACK,"������ ������ �Ǿ�");
						PutFont(18,410,BLACK,"������ �����ϴ�.");
					}
					else if(delay>600)
					{
						PutFont(18,370,BLACK,"����â���� �α���");
						PutFont(18,390,BLACK,"�ູ���� ǥ�õǾ�");
						PutFont(18,410,BLACK,"�ֽ��ϴ�.");
					}
					else if(delay>300)
					{
						PutFont(18,370,BLACK,"�α��� 0�� �ǰų�");
						PutFont(18,390,BLACK,"�α��� 50�̻��ε�");
						PutFont(18,410,BLACK,"�ູ���� -100�̸�");
					}
					else
					{
						PutFont(18,370,BLACK,"���ӿ����� �˴ϴ�.");
						PutFont(18,390,BLACK,"���� ������");
						PutFont(18,410,BLACK,"�ǽ��غ�����.");
					}
				}
				else if(lesson==1)	//������� ����
				{
					if(delay>3300)
					{							                       
						PutFont(18,370,BLACK,"���� �ƴ� ������");
						PutFont(18,390,BLACK,"���콺 �����ʹ�ư��");
						PutFont(18,410,BLACK,"������ �־�ʽÿ�.");
					}
					else if(delay>3000)
					{
						PutFont(18,370,BLACK,"���콺���� ���� ����");
						PutFont(18,390,BLACK,"������ �ߵ��˴ϴ�.");
						PutFont(18,410,BLACK,"��Ҵ� ���ʹ�ư.");
					}
					else if(delay>2700)
					{
						PutFont(18,370,BLACK,"���� �ִ� â�� ����");
						PutFont(18,390,BLACK,"3������ ������");
						PutFont(18,410,BLACK,"�� �� �ֽ��ϴ�.");
					}
					else if(delay>2400)
					{
						PutFont(18,370,BLACK,"����â���� �Ǽ�����");
						PutFont(18,390,BLACK,"����â���� ���ݸ���");
						PutFont(18,410,BLACK,"����â���� ��ȯ����");
					}
					else if(delay>2100)
					{
						PutFont(18,370,BLACK,"�Ǽ������� 4����");
						PutFont(18,390,BLACK,"��������, ��������");
						PutFont(18,410,BLACK,"õ��������, ö��");
					}
					else if(delay>1800)
					{
						PutFont(18,370,BLACK,"���� ������");
						PutFont(18,390,BLACK,"������ �ູ����");
						PutFont(18,410,BLACK,"�����ݴϴ�.");
					}
					else if(delay>1500)
					{
						for(int i=0; i<4; i++)hero[i].life=true;
						hero[0].x=5; hero[0].y=5;
						hero[1].x=3; hero[1].y=5;
						hero[2].x=5; hero[2].y=3;
						hero[3].x=3; hero[3].y=3;
						build[4][4]=1;
						PutFont(18,370,BLACK,"����������");
						PutFont(18,390,BLACK,"��ó�� ��簡 ��Ÿ��");
						PutFont(18,410,BLACK,"����� �˷� �ݴϴ�.");
					}
					else if(delay>1200)
					{
						PutFont(18,370,BLACK,"õ���� ������");
						PutFont(18,390,BLACK,"������ ȿ���� ����");
						PutFont(18,410,BLACK,"�Ҹ��� �ּ�ȭ�մϴ�.");
					}
					else if(delay>900)
					{
						PutFont(18,370,BLACK,"����� ���ռ��� �ִ�");
						PutFont(18,390,BLACK,"ĭ�� ������ ȿ����");
						PutFont(18,410,BLACK,"�ູ ��ȭ�� �����ϴ�.");
					}
					else if(delay>600)
					{
						PutFont(18,370,BLACK,"�׸��� ������ ����");
						PutFont(18,390,BLACK,"�Ǽ����� ���� ������");
						PutFont(18,410,BLACK,"�ٸ� ������ ����");
					}
					else if(delay>300)
					{
						PutFont(18,370,BLACK,"ȿ���� ��������");
						PutFont(18,390,BLACK,"�˴ϴ�.");
					}
					else
					{
						devil.mp=50;
						PutFont(18,370,BLACK,"������ ������");
						PutFont(18,390,BLACK,"���� �غ��� ���״�");
						PutFont(18,410,BLACK,"�ǽ��غ�����.");
					}
				}
				if(lesson==2)	//�ֹ� ����
				{
					if(delay>3300)
					{
						PutFont(18,370,BLACK,"�ֹε��� 4����");
						PutFont(18,390,BLACK,"���¸� �����ϴ�.");
						PutFont(18,410,BLACK,"�ູ ���� ���� �г�");
					}
					else if(delay>3000)
					{
						devil.eyeX=4; devil.eyeY=4;
						devil.eyeland=map[5][5];
						citizen.human[144].life=true;
						citizen.human[144].happy=100;

						PutFont(18,370,BLACK,"�ູ�� �ֹ���");
						PutFont(18,390,BLACK,"�α��� �ø��ϴ�.");
					}
					else if(delay>2400)
					{
						devil.eyeX=4; devil.eyeY=4;
						for(int i=0; i<4; i++)battle.spr[i+1].chr=14+(i%2);

						PutFont(18,370,BLACK,"�����ϴ� �ֹ���");
						PutFont(18,390,BLACK,"��縦 �θ���");
						PutFont(18,410,BLACK,"�˴ϴ�.");
					}
					else if(delay>1800)
					{
						devil.eyeX=4; devil.eyeY=4;
						for(int i=0; i<4; i++)battle.spr[i+1].chr=16+(i%2);

						PutFont(18,370,BLACK,"�г��� �ֹ���");
						PutFont(18,390,BLACK,"Ŭ���ϸ� �����");
						PutFont(18,410,BLACK,"���ϰ� �˴ϴ�.");
					}
					else if(delay>1200)
					{
						PutFont(18,370,BLACK,"�� ��簡 �����ϸ�");
						PutFont(18,390,BLACK,"��ü������ �ູ����");
						PutFont(18,410,BLACK,"�������ϴ�.");
					}
					else
					{
						PutFont(18,370,BLACK,"�˸��� �ǹ�,�������");
						PutFont(18,390,BLACK,"�Ҹ����� ���ŷ�");
						PutFont(18,410,BLACK,"������ �����Ͻʽÿ�.");
					}
				}
				delay--;
			}

			PutFontOutline(20,0,YELLOW,"�α���:%d/%d",citizen.number,max_human);
			PutFontOutline(140,0,YELLOW,"�ູ��:%d",citizen.happy);
			PutFontOutline(260,0,YELLOW,"���¼���:%d",devil.inc_mp);
			PutFontOutline(380,0,YELLOW,"���¼Һ�:%d",devil.dec_mp);
			PutFontOutline(520+Xpos(battle.spr[SMAX].hp),330,JColor(195,0,0),"%d",battle.spr[SMAX].hp);
			PutFontOutline(560+Xpos(battle.spr[SMAX].mp),330,JColor(128,128,255),"%d",battle.spr[SMAX].mp);
			_UnlockScreen();

			_CopyScreen(true);

			//�޴���
			if(_GetKeyState(VK_ESCAPE))
			{
				_MidiStop();
				Change(99);
				game_mode=1;
				game_start=false;
			}
		}
	}
	_MidiStop();
	Change(99);
	DestroyWindow( MainHwnd );

	Term();

    return TRUE;
}
