#include <time.h>
#include <ddraw.h>
#include <dsound.h>
#include <stdio.h>

#include "extern.h"
#include "data.h"

bool Act=true;

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

/*long FAR PASCAL WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch ( message )
    {
		case	WM_ACTIVATEAPP	:	Act=wParam;
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
        case    MM_MCINOTIFY    :   if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL ) _MidiReplay();
                                    break;
		case    WM_DESTROY      :  
                                    PostQuitMessage( 0 );
                                    break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}*/

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

		case WM_SYSCOMMAND	 :  //닫기 메시지 가로채기
								if(wParam==SC_CLOSE)
								{
									wParam=0;
									exit(0);
								}
								break;
	}
    return DefWindowProc( hWnd, message, wParam, lParam );
}

int SColor1(int hp)
{
	return (hp<50)?255:250-(hp-50)*5;
}

int SColor2(int hp)
{
	return (hp<50)?hp*5:255;
}

void ani(int limit)
{
	Count++;
	if(Count>limit)
	{
		RFrame++;
		if(RFrame>3)RFrame=0;
		if(RFrame==3)Frame=1;
			else Frame=RFrame;
		Count=0;
	}
}

int Click()
{
	bool loop=true;
	key=true;
	while(loop)
	{
		PRC
		if(LeftButton||_GetKeyState(VK_RETURN))
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
	//초기화
	int i;
	if(to==99)
	{
		for(i=1;i<=101;i++)
		{
			_DrawBar(400-i*4,300-i*3,400+i*4,300+i*3,BLACK);
			_CopyScreen(false);
		}
	}
	else
	{
		char* fname[]={"DATA//first.bmp","DATA//back.bmp","DATA//water.bmp",
						"DATA//event1.bmp","DATA//event2.bmp","DATA//event3.bmp",
						"DATA//event4.bmp","DATA//time.bmp", "DATA//event5.bmp",
						"DATA//end1.bmp","DATA//end2.bmp", "DATA//end3.bmp",
						"DATA//end4.bmp","DATA//end5.bmp", "DATA//end6.bmp",
						"DATA//end.bmp", "DATA//drink.bmp"};
		BmpScreen[0] = DDLoadBitmap( DirectOBJ, fname[to], 0, 0, SYSTEM);
		for(i=1;i<=101;i++){
			SetRect(&BackRect,400-i*4,300-i*3, 400+i*4,300+i*3);
			_DrawBmp(BackRect,400-i*4,300-i*3, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
			_CopyScreen(false);
		}
	}
}

void Talk(int caracter, char* sen1, char* sen2, char* sen3)
{
	char name[20];
	switch(caracter)
	{
		case 0: strcpy(name, "중얼영감");break;
		case 1: strcpy(name, "분노의 대갈");break;
		case 2: strcpy(name, "아들내미");break;
		case 3: strcpy(name, "평범한 농부");break;
		case 4: strcpy(name, "숲의 성자");break;
		case 5: strcpy(name, "히죽히죽");break;
		case 6: strcpy(name, "나불나불");break;
		case 7: strcpy(name, "엘프");break;
		case 8: strcpy(name, "의문의 마도사");break;
		case 9: strcpy(name, "벨라");break;
		case 10: strcpy(name, "벨라");break;
		case 12: strcpy(name, "진흙골렘");break;
		case 13: strcpy(name, "전설의 거인");break;
		case 14: strcpy(name, "마을주민");break;
		case 15: strcpy(name, "상인");break;
		case 16: strcpy(name, "술집주인");break;
	}
	SetRect(&BackRect, (caracter%4)*160, (caracter/4)*160, (caracter%4)*160+160, (caracter/4)*160+160);
	if(caracter<12)_DrawBmp(BackRect, 20, 420, BmpScreen[1], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	if(caracter!=99)PutFontOutline(200,440,YELLOW,"[%s]",name);
	PutFontOutline(200,480,WHITE,sen1);
	PutFontOutline(200,500,WHITE,sen2);
	PutFontOutline(200,520,WHITE,sen3);
}

void MainScr()
{
	SCENE;
	SetRect(&BackRect, 0, 0, 600, 400);
	_DrawBmp(BackRect, 100, 0, BmpScreen[3], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	//정보
	PutFontOutlineBig(120,0,BLUE,"%d째주 %d번째 날",gm.day/7+1,gm.day%7+1);
	PutFontOutlineBig(300,0,BLUE,stage[gm.day/7]);
	SetRect(&BackRect, 0, 0, 20, 20);
	_DrawBmp(BackRect, 550, 0, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	PutFontOutline(580,0,BLUE,"%d",gm.money);

	for(int i=0;i<2;i++)
	{
		int hx=(!i)?0:700;
		PutFontOutline(hx,20,WHITE,"체력");
		_DrawBar(hx,40,hx+mn[i].hp,60,JColor(SColor1(mn[i].hp),SColor2(mn[i].hp),0));
		_DrawBox(hx,40,hx+99,60,WHITE);
		PutFontOutline(hx,80,WHITE,"무술(%d)",mn[i].exp);
		SetRect(&BackRect, 40, 0, 60, 20);
		for(int i2=0;i2<mn[i].pow;i2++)
			_DrawBmp(BackRect, hx+i2*20, 100, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		PutFontOutline(hx,140,WHITE,"정의감");
		_DrawBar(hx,160,hx+mn[i].just,180,JColor(SColor1(mn[i].just),SColor2(mn[i].just),0));
		_DrawBox(hx,160,hx+99,180,WHITE);
		char buffer[20];
		sprintf(buffer, "%d", mn[i].just);
		int ex = 0;
		if(mn[i].just < 10)
		{
			ex = 4;
		}
		else if(mn[i].just < 100)
		{
			ex = 8;
		}
		else
		{
			ex = 12;
		}

		PutFontOutline(hx + 50 - ex,162,WHITE,buffer);

		PutFontOutline(hx,200,WHITE,"무기");
		if(!i)PutFontOutline(hx,220,GREEN,stk[mn[i].att]);
			else PutFontOutline(hx,220,GREEN,swd[mn[i].att]);
		PutFontOutline(hx,260,WHITE,"신발");
		PutFontOutline(hx,280,GREEN,shoes[mn[i].shoes]);
	}
}

void Story(int page, int end, bool back)
{
	int fpage=page;
	while(page<(fpage+end))
	{
		if(back){SCENE;}
			else MainScr();
		int face=atoi(snr1[page*4]);
		Talk(face,snr1[page*4+1],snr1[page*4+2],snr1[page*4+3]);
		_CopyScreen(false);
		Click();
		page++;
	}
}

void Opening()
{
	gm.day=gm.out=0; gm.money=10;
	gm.vella_flag = 0;

	for(int i=0;i<2;i++)
	{
		mn[i].att=mn[i].shoes=mn[i].exp=mn[i].just=0;
		mn[i].pow=(!i)?3:1;
		mn[i].hp=100;
	}
	Change(0);
	Click();
	Change(1);
	_MidiPlay("music//first.mid", true);
	Story(0,1,true);
	Change(99);	Change(1);
	Story(1,1,true);
	Change(99);	Change(1);
	Story(2,8,true);
	Change(99);	Change(1);
	Story(10,4,true);
	Change(99);
	_MidiPlay("music//stage1.mid", true);
	BmpScreen[3] = DDLoadBitmap( DirectOBJ, scr[gm.day/7], 0, 0, SYSTEM);
	Change(1); Nsave=true;
	Story(14,6,false);
}

void CBattle::SetData(int num, char* Name, int X, int Y, int SizeX, int SizeY, int Speed, int Hp, int At)
{
	name[num]=Name;
	mondat[num][0]=X;
	mondat[num][1]=Y;
	mondat[num][2]=SizeX;
	mondat[num][3]=SizeY;
	mondat[num][4]=Speed;
	monHP[num]=Hp;
	monAT[num]=At;
}

void CBattle::PreBattle()
{
	/**************************************************************
						전투데이터 초기화
	**************************************************************/
	int X=0;
	//숫자, 이름, XY좌표, XY크기, 속도, 체력, 힘
	SetData(0,"분노의 대갈",0,250,100,100,3,100,X);
	SetData(1,"아들내미",0,170,50,80,3,100,X);
	SetData(2,"하트",150,120,50,50,10,X,X);
	SetData(3,"검",200,0,70,70,10,X,X);
	SetData(4,"콩알탄",60,0,20,20,3,X,X);
	SetData(5,"진흙슬라임",0,120,50,50,1,20,1);
	SetData(6,"데굴데굴",300,70,50,50,2,20,1);
	SetData(7,"물슬라임",150,70,50,50,1,30,2);
	SetData(8,"진흙골렘",480,0,100,110,5,100,X);
	SetData(9,"불",300,120,60,100,X,10,X);
	SetData(10,"불똥",120,0,20,20,3,X,X);
	SetData(11,"데구르르",480,110,50,50,2,30,2);
	SetData(12,"춤불",300,120,60,100,5,20,3);
	SetData(13,"레이져",450,70,10,50,5,X,X);
	SetData(14,"히죽나무",330,220,90,150,X,50,X);
	SetData(15,"바위골렘",600,210,50,75,1,80,2);
	SetData(16,"전설의 거인",0,390,200,300,1,300,10);
	SetData(17,"전차",600,360,80,40,3,80,X);
	SetData(18,"소용돌이",480,160,50,50,X,X,X);
	SetData(19,"보트",0,350,110,40,5,1,X);
	SetData(20,"로봇",600,285,50,75,1,120,3);
	SetData(21,"회오리",630,110,50,50,10,40,2);
	SetData(22,"돌새",600,400,80,50,10,40,3);
	SetData(23,"엘프",600,450,60,130,X,150,X);
	SetData(24,"히죽히죽",600,580,80,150,5,200,X);
	SetData(25,"아들내미",150,170,50,80,3,500,X);
	SetData(26,"의문의 마도사",0,690,90,150,X,150,X);
	SetData(27,"벨라",0,690,90,150,X,300,X);
	SetData(28,"술병",750,160,15,60,X,X,X);
	SetData(29,"술먹는 분노의 대갈",0,250,100,100,3,100,X);

	//방향
	int OdirX[9]={-1,-1,-1,0,1,1,1,0,0}, OdirY[9]={1,0,-1,-1,-1,0,1,1,0};
	for(int i1=0;i1<9;i1++)
	{
		dirX[i1]=OdirX[i1];
		dirY[i1]=OdirY[i1];
	}
	//적파티
	int Oenemy[ENEMYS][6]={
		{0,0,5,5,0,10},{0,5,5,5,0,14},{0,5,5,6,5,22},//풀풀(0~2)
		{2,0,7,7,0,10},{2,0,7,7,6,16},{2,6,7,7,6,24},//촬촬(3~5)
		{0,14,7,0,0,16},{0,14,7,5,0,18},{0,14,0,0,14,20},//쑥쑥(6~8)
		{4,0,6,6,0,12},{4,0,6,6,11,16},{4,11,6,6,15,24},//휭휭(9~11)
		{3,0,9,9,0,14},{3,0,9,12,0,12},{3,9,12,12,9,24},//활활(12~14)
		{5,0,17,20,0,12},{5,17,20,20,0,16},{5,17,17,20,20,24},//땡땡(15~17)
		{0,21,21,21,21,20},{0,22,21,21,22,20},{0,22,22,22,22,20},//솔솔(18~20)
		{1,0,8,0,0,10},{3,9,9,9,9,28},{4,0,16,0,0,28},//골렘, 화재, 전설의 거인(21~23)
		{5,17,17,17,17,20},{0,0,23,0,0,0},{3,0,24,0,0,30},//전차부대, 엘프,히죽히죽(24~26)
		{0,0,25,0,0,0},{0,0,26,0,0,0},{0,0,27,0,0,0}};//아들내미, 벨라 1, 2(27~29)
	for(int i1=0;i1<ENEMYS;i1++)
		for(int i2=0;i2<6;i2++)enemy[i1][i2]=Oenemy[i1][i2];
}

int CBattle::Key()
{
	int direct=8;
	bool left, up, right, down;
	left=up=right=down=false;
	if(_GetKeyState(VK_LEFT))left=true;
	if(_GetKeyState(VK_UP))up=true;
	if(_GetKeyState(VK_RIGHT))right=true;
	if(_GetKeyState(VK_DOWN))down=true;
	if(left && !up && !right && down)direct=0;
	if(left && !up && !right && !down)direct=1;
	if(left && up && !right && !down)direct=2;
	if(!left && up && !right && !down)direct=3;
	if(!left && up && right && !down)direct=4;
	if(!left && !up && right && !down)direct=5;
	if(!left && !up && right && down)direct=6;
	if(!left && !up && !right && down)direct=7;

	return direct;
}

int CBattle::CenterX(int s1, int s2)
{
	return spr[s1].x+spr[s1].sizeX/2-mondat[s2][2]/2;
}

int CBattle::CenterY(int s1, int s2)
{
	return spr[s1].y+spr[s1].sizeY/2-mondat[s2][3]/2;
}

bool CBattle::Crush(int x, int y)
{
	int left1=x2, left2=spr[y].x;
	int top1=y2, top2=spr[y].y;
	int right1=x2+spr[x].sizeX, right2=spr[y].x+spr[y].sizeX;
	int bottom1=y2+spr[x].sizeY, bottom2=spr[y].y+spr[y].sizeY;
	if(left1<right2 && right1>left2 && top1<bottom2 && bottom1>top2)
		return true;
	else
		return false;
}

bool CBattle::Bump(int i1)
{
	bool bump=false;
	for(int i2=0;i2<2;i2++)
		if(Crush(i1,i2)&&!spr[i2].dam){
			if(spr[i2].hp>0)spr[i2].hp-=spr[i1].att;
			spr[i2].dam=15;
			bump=true;
		}
	return bump;
}

void CBattle::PutSpr(int num)
{
	ani(5);
	bool ok;
	int putN[SMAX], tmp;
	for(int i1=0;i1<SMAX;i1++)putN[i1]=i1;
	//스프라이터
	do{
		ok=false;
		for(int i1=0; i1<SMAX-1; i1++)
		{
			if((spr[putN[i1]].y+spr[putN[i1]].sizeY)>(spr[putN[i1+1]].y+spr[putN[i1+1]].sizeY)){
				tmp=putN[i1+1];
				putN[i1+1]=putN[i1];
				putN[i1]=tmp;
				ok=true;
			}
		}
	}while(ok);
	for(int i1=0;i1<SMAX;i1++)
	{
		if(!spr[putN[i1]].life || spr[putN[i1]].dam%2)continue;
		if(spr[putN[i1]].kind!=3&&spr[putN[i1]].kind!=18)//이식할때 수정
			SetRect(&BackRect, spr[putN[i1]].sizeX*Frame+mondat[spr[putN[i1]].kind][0], mondat[spr[putN[i1]].kind][1], spr[putN[i1]].sizeX*(Frame+1)+mondat[spr[putN[i1]].kind][0], mondat[spr[putN[i1]].kind][1]+spr[putN[i1]].sizeY);
		else
			SetRect(&BackRect, spr[putN[i1]].sizeX*RFrame+mondat[spr[putN[i1]].kind][0], mondat[spr[putN[i1]].kind][1], spr[putN[i1]].sizeX*(RFrame+1)+mondat[spr[putN[i1]].kind][0], mondat[spr[putN[i1]].kind][1]+spr[putN[i1]].sizeY);
		_DrawBmp(BackRect, spr[putN[i1]].x, spr[putN[i1]].y, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	}
	//데이터
	for(int i1=0;i1<6;i1++)
	{
		if(!spr[i1].life || spr[i1].kind==19 || spr[i1].kind==29)continue;//데이터를 표시하지 않는 타입들
		PutFontOutline(i1*130, 560, WHITE, name[spr[i1].kind]);
		if(spr[i1].hp>100)
			_DrawBar(i1*130, 580, i1*130+100, 595, BLACK);
		else
			_DrawBar(i1*130, 580, i1*130+spr[i1].hp, 595, JColor(SColor1(spr[i1].hp),SColor2(spr[i1].hp),0));
		if(monHP[spr[i1].kind]>100)
			_DrawBox(i1*130, 580, i1*130+100, 595, WHITE);
		else
			_DrawBox(i1*130, 580, i1*130+monHP[spr[i1].kind], 595, WHITE);
	}
	//승리시
	if(vic>0)
	{
		PutFontOutline(300,270,WHITE,"전투에서 승리했습니다!");
		PutFontOutline(300,290,WHITE,"경험치+%d",enemy[num][5]);
		int plusmoney=0;
		for(int i1=1;i1<5;i1++){
			if(enemy[num][i1])plusmoney++;
		}
		PutFontOutline(300,310,WHITE,"돈+%dK",plusmoney);
		if(vic==2)gm.money+=plusmoney;
		vic--;
	}
	//패배시
	if(vic<0)PutFontOutline(300,290,RED,"G A M E   O V E R");
}

void CBattle::NewSpr(int n, int kind, int x, int y, int hp, int at, int dr, int master)
{
	spr[n].life=true;
	spr[n].kind=kind;

	spr[n].x=x;
	spr[n].y=y;
	spr[n].sizeX=mondat[spr[n].kind][2];
	spr[n].sizeY=mondat[spr[n].kind][3];
	spr[n].speed=mondat[spr[n].kind][4];
	if(n<2)spr[n].speed+=mn[n].shoes;//다른곳에 이식할 땐 이 부분은 제거

	spr[n].hp=hp;
	spr[n].att=at;
	spr[n].dir=dr;
	spr[n].dam=0;
	spr[n].master = master;
}

void CBattle::CtrSpr()
{
	temp++;
	for(int i1=0;i1<SMAX;i1++)
	{
		if(vic==1)win=1;
		if(!spr[i1].life)continue;
		if(spr[i1].dam>0)spr[i1].dam--;
		if(spr[i1].hp<=0&&spr[i1].life){
			spr[i1].life=false;
			if(i1<6){
				_Play(2);
				ENSPR NewSpr(i2,9,CenterX(i1,9),CenterY(i1,9),100,0,8);
			}
		}
		if(!spr[2].life && !spr[3].life && !spr[4].life && !spr[5].life && !vic){
			vic=400;
			_MidiPlay("music//win.mid",false);
		}//승리
		if(!spr[0].life && !spr[1].life && !vic){
			vic=-1;
			_MidiPlay("music//gameover.mid",true);
			Quit=true;
		}//패배
		x2=spr[i1].x, y2=spr[i1].y;
		//움직임
		if(i1==0 || i1==1)
		{
			x2+=(dirX[dir]*spr[i1].speed);
			y2+=(dirY[dir]*spr[i1].speed);
		}
		if(i1>=2 && i1<=5)
		{
			int herodir = 0, target, tmp1, tmp2;
			target=(spr[0].life)?0:1;
			tmp1=spr[i1].x-spr[target].x;
			tmp2=spr[i1].y-spr[target].y;
			if(tmp1>=spr[target].sizeX&&tmp2<0)herodir=0;
			if(tmp1>=spr[target].sizeX&&tmp2>=0&&tmp2<spr[target].sizeY)herodir=1;
			if(tmp1>spr[target].sizeX&&tmp2>=spr[target].sizeY)herodir=2;
			if(tmp1<=spr[target].sizeX&&tmp1>0&&tmp2>=spr[target].sizeY)herodir=3;
			if(tmp1<=0&&tmp2>spr[target].sizeY)herodir=4;
			if(tmp1<=0&&tmp2<=spr[target].sizeY&&tmp2>0)herodir=5;
			if(tmp1<0&&tmp2<=0)herodir=6;
			if(tmp1<spr[target].sizeX&&tmp1>=0&&tmp2<=0)herodir=7;
			if(herodir<0 || herodir>7)herodir=rand()%7;

			if(spr[i1].kind==5||spr[i1].kind==6||spr[i1].kind==7||
				spr[i1].kind==11||spr[i1].kind==15||spr[i1].kind==16)//돌격형 몬스터
			{
				if(Crush(i1,0)&&spr[0].life||Crush(i1,1)&&spr[1].life)Bump(i1);
				else{
					x2+=dirX[herodir]*spr[i1].speed;
					y2+=dirY[herodir]*spr[i1].speed;
				}
				if(spr[i1].kind==16 && !(temp%30))_Play(5);
			}
			if(spr[i1].kind==8)//진흙골렘
			{
				int golem;
				if((temp%800)>=600)golem=4;
				else if((temp%800)>=400)golem=7;
				else if((temp%800)>=200)golem=2;
				else golem=7;
				x2+=(dirX[golem]*spr[i1].speed);
				y2+=(dirY[golem]*spr[i1].speed);
				if(!(temp%30)){
					ENSPR NewSpr(i2,4,CenterX(i1,4),CenterY(i1,4),1000,2,herodir);
				}
			}
			if(spr[i1].kind==9)//불
			{
				if(!(temp%30)){
					for(int i3=0;i3<3;i3++){
						ENSPR NewSpr(i2,10,CenterX(i1,10),CenterY(i1,10),1000,5,i3);
					}
				}
			}
			if(spr[i1].kind==12||spr[i1].kind==21)//춤불, 회오리
			{
				Bump(i1);
				int fire=temp%80/10;
				if(!(temp%10))fire=herodir;
				x2+=(dirX[fire]*spr[i1].speed);
				y2+=(dirY[fire]*spr[i1].speed);
			}
			if(spr[i1].kind==14)//히죽나무
			{
				if(!(temp%10)){
					ENSPR NewSpr(i2,4,CenterX(i1,10),CenterY(i1,10),1000,2,rand()%8);
				}
			}
			if(spr[i1].kind==20)//로봇
			{
				if(Crush(i1,0)&&spr[0].life||Crush(i1,1)&&spr[1].life)Bump(i1);
				else{
					x2+=dirX[herodir]*spr[i1].speed;
					y2+=dirY[herodir]*spr[i1].speed;
				}
				if(!(temp%100)){
					ENSPR NewSpr(i2,4,CenterX(i1,10),CenterY(i1,10),1000,2,herodir);
				}
			}
			if(spr[i1].kind==17)//전차
			{
				if(spr[i1].dir!=3&&spr[i1].dir!=7)spr[i1].dir=3;
				if((temp%100)<90)
					y2+=(spr[i1].speed*dirY[spr[i1].dir]);
				else x2+=spr[i1].speed;
				if(y2<0)spr[i1].dir=7;
				if(y2>560)spr[i1].dir=3;
				if((temp%100)>90){
					ENSPR NewSpr(i2,13,spr[i1].x-5,CenterY(i1,10),1000,3,1);
				}
			}
			if(spr[i1].kind==22)//돌새
			{
				Bump(i1);
				if(spr[i1].dir<0 || spr[i1].dir>7)spr[i1].dir=8;
				if(!(temp%50))spr[i1].dir=herodir;
				x2+=(dirX[spr[i1].dir]*spr[i1].speed);
				y2+=(dirY[spr[i1].dir]*spr[i1].speed);
			}
			if(spr[i1].kind==23)//엘프
			{
				if(!(temp%80)){
					x2=rand()%(800-spr[i1].sizeX);
					y2=rand()%(600-spr[i1].sizeY);
					_Play(6);
				}
				if(!(temp%10)){
					int elf=temp%80/10;
					ENSPR NewSpr(i2,4,CenterX(i1,10),CenterY(i1,10),1000,2,elf);
				}
			}
			if(spr[i1].kind==24)//히죽히죽
			{
				if(spr[i1].dir<0 || spr[i1].dir>7)spr[i1].dir=8;
				if(!(temp%80))spr[i1].dir=rand()%8;
				if(!(temp%200))
				{
					int i2 = 0;
					for(i2=2;i2<6;i2++)
						if(!spr[i2].life)break;
					int hijuk=rand()%3+5;
					if(i2<6){
						NewSpr(i2,hijuk,spr[i1].x,spr[i1].y,monHP[hijuk],monAT[hijuk],0,i1);
						_Play(9);
					}
				}
				x2+=(dirX[spr[i1].dir]*spr[i1].speed);
				y2+=(dirY[spr[i1].dir]*spr[i1].speed);
			}
			if(spr[i1].kind==25)//아들내미
			{
				if(spr[i1].dir<0 || spr[i1].dir>7)spr[i1].dir=8;
				if(!(temp%80)){
					for(int i3=0;i3<3;i3++){
						int son=herodir+8;
						ENSPR NewSpr(i2,3,CenterX(i1,3),CenterY(i1,3),1000,2,(son+i3-1)%8);
					}
					spr[i1].dir=rand()%8;
				}
				x2+=(dirX[spr[i1].dir]*spr[i1].speed);
				y2+=(dirY[spr[i1].dir]*spr[i1].speed);
			}
			else if(spr[i1].kind==26 || spr[i1].kind==27)//벨라
			{
				int cool_time = 200;
				int wind_power = 1;
				if(spr[i1].kind == 27)
				{
					cool_time = 50;
					wind_power = 2;
				}

				//_Play(9);

				if((temp % cool_time) == 1)
				{
					int i2 = 0;
					for(i2 = 2;i2 < 6; i2++)
					{
						if(!spr[i2].life)
						{
							NewSpr(i2,21,spr[i1].x,spr[i1].y,monHP[21],monAT[21],0,i1);
							break;
						}
					}
				}

				spr[0].x = Max(spr[0].x - wind_power, 0);
				spr[1].x = Max(spr[1].x - wind_power, 0);
			}
		}
		if(i1==6 || i1==7)
		{
			for(int i2=2;i2<6;i2++)
				if(Crush(i1,i2)&&spr[i1].life&&!spr[i2].dam){
					if(spr[i2].hp>0)spr[i2].hp-=spr[i1].att;
					spr[i2].dam=15;
					spr[i1].life=false;
				}
			x2=spr[i1].x+dirX[spr[i1].dir]*spr[i1].speed;
			y2=spr[i1].y+dirY[spr[i1].dir]*spr[i1].speed;
			spr[i1].hp--;
		}
		if(i1>=8)
		{
			spr[i1].hp--;
			if(spr[i1].kind==9)continue;
			if(Bump(i1))spr[i1].life=false;
			x2=spr[i1].x+dirX[spr[i1].dir]*spr[i1].speed;
			y2=spr[i1].y+dirY[spr[i1].dir]*spr[i1].speed;
		}
		if(i1<6)
		{
			x2=Max(x2,0);
			y2=Max(y2,0);
			x2=Min(x2,800-spr[i1].sizeX);
			y2=Min(y2,600-spr[i1].sizeY);
		}
		else if(x2<0 || y2<0 || (x2-spr[i1].sizeX)>800 || (y2-spr[i1].sizeY)>600)
			spr[i1].life=false;
		spr[i1].x=x2; spr[i1].y=y2;

		//소환체는 주인이 죽으면 사망
		if(spr[i1].master >= 0 && spr[spr[i1].master].life == false)
		{
			spr[i1].hp = 0;
		}
	}
}

int CBattle::Battle(int num, bool party)
{
	PreBattle();
	//전투 기본 설정(1):초기화
	int round=enemy[num][0];
	for(int i1=0;i1<SMAX;i1++){
		spr[i1].life=false;
		spr[i1].dam=0;
	}
	//(2)아군 생성
	NewSpr(0,0,100,100,mn[0].hp,mn[0].pow,0);
	if(party)NewSpr(1,1,100,400,mn[1].hp,mn[1].pow,0);
	//(3)적군 생성
	for(int i1=2;i1<6;i1++){//적 데이터
		int kind=enemy[num][i1-1];
		if(kind)NewSpr(i1,kind,600,(i1-2)*150,monHP[kind],monAT[kind],0);
	}

	temp=win=vic=0;
		
	_MidiPlay("music//battle.mid",true);
	while (!win){PRC{
		SetRect(&BackRect, round*40, 0, round*40+40, 40);
		for(int i1=0;i1<20;i1++)
			for(int i2=0;i2<15;i2++)
				_DrawBmp(BackRect, i1*40, i2*40, BmpScreen[4], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
		//조작
		dir=Key();
		if(_GetKeyState(VK_RETURN))
		{
			if(spr[0].life&&!spr[6].life){
				NewSpr(6,2,CenterX(0,2),CenterY(0,6),(mn[0].att+1)*10+10,mn[0].pow,(dir==8)?5:dir);
				_Play(0);
			}
			if(spr[1].life&&!spr[7].life){
				NewSpr(7,3,CenterX(1,3),CenterY(1,7),(mn[1].att+1)*10+10,mn[1].pow,(dir==8)?5:dir);
				_Play(1);
			}
		}
		if(vic==-1)
			if(_GetKeyState(VK_ESCAPE) || LeftButton)win=2;

		PutSpr(num);
		CtrSpr();
		_CopyScreen(false);
	}}
	if(win==1)
	{
		mn[0].hp=spr[0].hp; mn[0].exp+=enemy[num][5];
		if(party){
			mn[1].hp=spr[1].hp;
			mn[1].exp+=enemy[num][5];
			mn[1].just++;
		}
		Change(1);
		_MidiPlay(bgm[gm.day/7],true);
	}
	else{
		Change(99);
		exit(0);
	}
	return 0;
}

int Shopping()
{
	const int fafa_x = 200;
	const int son_x = 400;
	
	const int help_y = 120;
	const int weapon_y = 160;
	const int shoes_y = 240;
	const int exit_y = 320;
	
	const int price[] = {5, 20, 35};

	while(1)
	{
		PRC
		{
			int selected_item = -1;
			int selected_price = 0;
			JColor color;

			ani(10);

			MainScr();

			_DrawBar(fafa_x - 20, help_y - 20, son_x + 140, exit_y + 40, BLACK);

			PutFontOutline(fafa_x, help_y, YELLOW, "구입할 무기를 선택하세요.");

			if(MouseX >= son_x + 60 && MouseX < son_x + 140 && MouseY >= exit_y && MouseY < exit_y + 20)
			{
				color = GREEN;
				selected_item = 999;
			}
			else
			{
				color = WHITE;
			}
			
			PutFontOutline(son_x + 60, exit_y, color, "나가기");

			//아버지 무기
			for(int i = 0; i < 3; i++)
			{
				if(mn[0].att - 1 >= i)continue;

				const int wy = weapon_y + i * 20;

				if(price[i] > gm.money)
				{
					color = GRAY;
				}
				else if(MouseX >= fafa_x && MouseX < fafa_x + 140 && MouseY >= wy && MouseY < wy + 20)
				{
					color = GREEN;
					selected_item = i;
					selected_price = price[i];
				}
				else
				{
					color = WHITE;
				}

				PutFontOutline(fafa_x, wy, color, "%s", stk[i + 1]);
				PutFontOutline(fafa_x + 100, wy, color, "%d", price[i]);
			}
			//아들 무기
			for(int i = 0; i < 3; i++)
			{
				if(mn[1].att - 1 >= i)continue;

				const int wy = weapon_y + i * 20;

				if(price[i] > gm.money)
				{
					color = GRAY;
				}
				else if(MouseX >= son_x && MouseX < son_x + 140 && MouseY >= wy && MouseY < wy + 20)
				{
					color = GREEN;
					selected_item = i + 100;
					selected_price = price[i];
				}
				else
				{
					color = WHITE;
				}

				PutFontOutline(son_x, wy, color, "%s", swd[i + 1]);
				PutFontOutline(son_x + 100, wy, color, "%d", price[i]);
			}
			//아버지 신발
			for(int i = 0; i < 3; i++)
			{
				if(mn[0].shoes - 1 >= i)continue;

				const int sy = shoes_y + i * 20;

				if(price[i] > gm.money)
				{
					color = GRAY;
				}
				else if(MouseX >= fafa_x && MouseX < fafa_x + 140 && MouseY >= sy && MouseY < sy + 20)
				{
					color = GREEN;
					selected_item = i + 10;
					selected_price = price[i];
				}
				else
				{
					color = WHITE;
				}

				PutFontOutline(fafa_x, sy, color, "%s", shoes[i + 1]);
				PutFontOutline(fafa_x + 100, sy, color, "%d", price[i]);
			}
			//아들 신발
			for(int i = 0; i < 3; i++)
			{
				if(mn[1].shoes - 1 >= i)continue;

				const int sy = shoes_y + i * 20;

				if(price[i] > gm.money)
				{
					color = GRAY;
				}
				else if(MouseX >= son_x && MouseX < son_x + 140 && MouseY >= sy && MouseY < sy + 20)
				{
					color = GREEN;
					selected_item = i + 110;
					selected_price = price[i];
				}
				else
				{
					color = WHITE;
				}

				PutFontOutline(son_x + 100, sy, color, "%d", price[i]);
				PutFontOutline(son_x, sy, color, "%s", shoes[i + 1]);
			}

			_DrawBox(MouseX-(5+Frame),MouseY-(5+Frame),MouseX+(5+Frame),MouseY+(5+Frame),RED);

			_CopyScreen(false);

			if(LeftButton)
			{
				if(!key)
				{
					if(selected_item == 999)
					{
						break;
					}
					//아버지 장비 교체
					else if(selected_item >= 0 && selected_item < 3)
					{
						mn[0].att = selected_item + 1;
					}
					else if(selected_item >= 10 && selected_item < 13)
					{
						mn[0].shoes = selected_item + 1 - 10;
					}
					//아들 장비 교체
					else if(selected_item >= 100 && selected_item < 103)
					{
						mn[1].att = selected_item + 1 - 100;
					}
					else if(selected_item >= 110 && selected_item < 113)
					{
						mn[1].shoes = selected_item + 1 - 110;
					}

					//지불
					if(selected_item >= 0 && selected_item < 999)
					{
						_Play(11);
						gm.money -= selected_price;
					}
					
					key = true;
				}
			}
			else key=false;
		}
	}

	return 0;
}

int CBattle::Boat()
{
	Change(2);
	PreBattle();
	_MidiPlay("music//boat.mid",true);
	int cm, ms, temp, time, put, sound;
	ms=1; cm=temp=time=put=sound=0;
	vic=0;
	for(int i1=0;i1<SMAX;i1++){
		spr[i1].life=false;
		spr[i1].dam=0;
	}
	NewSpr(0,19,0,280,0,0,0);
	//초기화 끝
	while(cm<100000){PRC{
		SetRect(&BackRect, 0, 0, 800, 600);
		for(int i1=0;i1<2;i1++)
			_DrawBmp(BackRect, i1*800-cm%800, 0, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
		
		if(100000 - cm <= SCREEN_X)
		{
			_DrawBar(100000-cm,0,100002-cm,600,RED);
		}
		
		//시간, 속도, 소용돌이
		cm+=ms; temp++; if(sound)sound--;
		if(!(temp%100)){ms++; time++;}
		if(cm>put){
			ENSPR NewSpr(i2,18,800,rand()%550,0,0,0);
			put+=500;
			_Play(4);
		}
		//제어
		dir=Key();
		x2=spr[0].x; y2=spr[0].y;
		x2+=(dirX[dir]*spr[0].speed);
		y2+=(dirY[dir]*spr[0].speed);
		x2=Max(x2,0);
		y2=Max(y2,0);
		x2=Min(x2,800-spr[0].sizeX);
		y2=Min(y2,600-spr[0].sizeY);
		spr[0].x=x2; spr[0].y=y2;
		for(int i1=8;i1<SMAX;i1++)
		{
			if(!spr[i1].life)continue;
			spr[i1].x-=ms;
			if(Crush(0,i1)&&ms>1){
				ms--;
				if(!sound){
					_Play(3);
					sound=50;
				}
			}
			if(spr[i1].x<0)spr[i1].life=false;
		}
		//출력
		PutSpr(0);
		SetRect(&BackRect, spr[0].sizeX*Frame+mondat[spr[0].kind][0], mondat[spr[0].kind][1], spr[0].sizeX*(Frame+1)+mondat[spr[0].kind][0], mondat[spr[0].kind][1]+spr[0].sizeY);
		_DrawBmp(BackRect, spr[0].x, spr[0].y, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		PutFontOutline(0,0,WHITE,"거리: %d/1000m",cm/100);
		PutFontOutline(200,0,WHITE,"속도: %dm/s",ms);
		PutFontOutline(400,0,WHITE,"시간: %d초",time);
		_CopyScreen(false);
	}}
	Click();
	Change(1);
	_MidiPlay(bgm[gm.day/7],true);
	return time;
}

int CBattle::Drink()
{
	PreBattle();
	_MidiPlay("music//train.mid",true);
	int time = 10;
	int bottle = 10;
	int msec = 99;
	int x_state = 1;
	int y_state = 1;
	
	for(int i1=0;i1<SMAX;i1++){
		spr[i1].life=false;
		spr[i1].dam=0;
	}

	NewSpr(0,29,0,280,0,0,0);

	for(int i = 0; i < 10; i++)
	{
		NewSpr(8 + i,28,150 * (i / 2) + 100,50 + (i % 2) * 450,0,0,0);
	}

	//초기화 끝
	while(1)
	{
		PRC
		{
			SetRect(&BackRect, 240, 0, 280, 40);
			for(int i1=0;i1<20;i1++)
				for(int i2=0;i2<15;i2++)
					_DrawBmp(BackRect, i1*40, i2*40, BmpScreen[4], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

			//제어
			dir=Key();
			x2=spr[0].x; y2=spr[0].y;
			x2+=(dirX[dir] * x_state * spr[0].speed);
			y2+=(dirY[dir] * y_state * spr[0].speed);
			x2=Max(x2,0);
			y2=Max(y2,0);
			x2=Min(x2,800-spr[0].sizeX);
			y2=Min(y2,600-spr[0].sizeY);
			spr[0].x=x2; spr[0].y=y2;

			//술 처리
			for(int i1=8;i1<SMAX;i1++)
			{
				if(!spr[i1].life)continue;
				if(Crush(0,i1)){
					spr[i1].life = false;
					bottle--;
					_Play(10);

					//방향 이상 발생
					if((rand() % 2) == 0)
					{
						x_state = 1;
					}
					else
					{
						x_state = -1;
					}

					if((rand() % 2) == 0)
					{
						y_state = 1;
					}
					else
					{
						y_state = -1;
					}
				}
			}

			//시간 감소
			msec--;
			if(msec < 0)
			{
				msec = 99;
				time--;
			}

			//출력
			PutSpr(0);
			SetRect(&BackRect, spr[0].sizeX*Frame+mondat[spr[0].kind][0], mondat[spr[0].kind][1], spr[0].sizeX*(Frame+1)+mondat[spr[0].kind][0], mondat[spr[0].kind][1]+spr[0].sizeY);
			_DrawBmp(BackRect, spr[0].x, spr[0].y, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			PutFontOutline(400,0,WHITE,"시간: %d초",time);
			_CopyScreen(false);

			if(time == 0 || bottle == 0)
			{
				break;
			}
		}
	}

	Change(1);
	_MidiPlay(bgm[gm.day/7],true);
	return time;
}

void save()
{
	Nsave=false;
	FILE* fp;
	unsigned char part1, part2;
	fp=fopen("hero.sav","wb");
	part1=gm.money/256;
	part2=gm.money%256;
	fprintf(fp,"%c%c%c%c",gm.day,gm.out,part1,part2);
	for(int i1=0;i1<2;i1++)
		fprintf(fp,"%c%c%c%c%c%c",mn[i1].att,mn[i1].exp,mn[i1].hp,mn[i1].just,mn[i1].pow,mn[i1].shoes);
	fprintf(fp,"%c", gm.vella_flag);
	fclose(fp);
}

void Event(int fafaDo, int sonDo)
{
	bool son=true, sonDid=false;
	int luck, boat;
	if(fafaDo==sonDo)
	{
		switch(fafaDo)
		{
			case 0:
				Change(99);
				_Play(7);
				_Delay(3000);
				mn[0].hp=mn[1].hp=100;
				break;
			case 1:
				Story(20,3,false);
				mn[0].exp+=(5+rand()%5);
				mn[1].exp+=(20+rand()%5);
				break;
			case 2:
				luck=rand()%6;
				if(luck<3)
				{
					Story(27,5,false);
					spr[0].Battle((gm.day/7)*3+luck,true);
				}
				else
				{
					Story(23,4,false);
					gm.money+=(luck+2);
					PutFontOutline(200,540,GREEN,"돈+%dK",luck+2);
					_CopyScreen(false);
					Click();
				}
				break;
			case 3:
				switch(gm.out)
				{
					case 0: //첫 나들이
						Story(46,2,true);
						Change(3);
						Click();
						gm.out++;
						break;
					case 1: //진흙골렘
						Story(48,4,true);
						Change(99); Change(1);
						Story(52,2,true);
						spr[0].Battle(21,true);
						Story(54,4,true);
						gm.out++;
						break;
					case 2: //농사돕기
						Story(58,4,true);
						mn[1].just+=5;
						break;
					case 3: //보트대회
						Story(62,5,true);
						Change(4);
						Story(67,2,true);
						boat=spr[0].Boat();
						Story(73,3,true);
						Change(99); Change(1);
						if(boat<50){
							Story(76,4,true);
							mn[0].att=mn[1].att=4;
						}
						else if(boat<60){
							Story(80,4,true);
							mn[0].att=4;
						}
						else if(boat<70){
							Story(84,4,true);
							gm.money+=30;
						}
						else Story(88,2,true);
						gm.out++;
						break;
					case 4: //포세이돈
						Story(90,5,true);
						Change(99); Change(1);
						Story(95,4,true);
						Change(5);
						Story(99,2,true);
						gm.out++;
						break;
					case 5: //식음전폐
						Story(101,2,true);
						mn[0].hp=100;
						break;
					case 6: //엘프
						Story(120,2,true);
						Change(99); Change(1);
						Story(122,6,true);
						spr[0].Battle(25,true);
						Story(128,2,true);
						gm.out++;
						break;
					case 7: //벨라 첫 전투
						Story(267,11,true);
						spr[0].Battle(28,true);
						Story(278,6,true);
						gm.vella_flag = 1;
						gm.out++;
						break;
					case 8: //숲의 성자
						Story(130,5,true);
						mn[1].just+=6;
						break;
					case 9: //사진
						Story(142,3,true);
						Change(6);
						Story(145,1,true);
						gm.out++;
						break;
					case 10: //전설의 거인
						Story(146,3,true);
						Change(99); Change(1);
						Story(149,2,true);
						spr[0].Battle(23,true);
						Story(151,3,true);
						gm.out++;
						break;
					case 11: //멍~
						Story(154,6,true);
						break;
					case 12: //히죽히죽
						Story(181,5,true);
						spr[0].Battle(26,true);
						Story(186,6,true);
						gm.out++;
						break;
					case 13: //불 끄기
						Story(192,4,true);
						spr[0].Battle(22,true);
						gm.money+=5;
						mn[1].just+=3;
					case 15: //벨라 두번째 전투
						if(gm.vella_flag == 1)
						{
							Story(284,7,true);
							spr[0].Battle(29,true);
							Story(291,9,true);
							gm.vella_flag=2;
							gm.out++;
							break;
						}
					case 16: //용병 뛰기
						Story(196,6,true);
						spr[0].Battle(24,true);
						gm.money+=10;
						mn[1].just+=3;
						break;
					case 18: //마지막 주
						Change(8);
						Story(216,6,true);
						gm.out++;
						break;
					case 19: //숲의 성자 or 벨라 최종 스토리
						if(gm.vella_flag == 2)
						{
							Story(300,50,true);
							gm.vella_flag=3;
							gm.out++;
							mn[1].just = 100;
						}
						else
						{
							Story(222,2,true);
							mn[1].just+=6;
						}
						break;
					case 20:	//벨라 후기
					Story(350,2,true);
					break;
				}
				break;
		}
	}
	else
	{
		switch(fafaDo)
		{
			case 0:
				Change(99);
				_Play(7);
				_Delay(3000);
				mn[0].hp=100;
				Change(1);
				luck=rand()%5;
				if(luck==1&&gm.money>0)//돈 훔쳐감
				{
					Story(33,3,false);
					gm.money--;
					sonDid=true;
				}
				if(luck==2)//때리고 토낌
				{
					Story(32,3,false);
					mn[0].hp=50;
					sonDid=true;
				}
				break;
			case 1:
				Story(36,1,false);
				mn[0].exp+=(15+rand()%5);
				break;
			case 2:
				luck=rand()%6;
				if(luck<3)
				{
					Story(37,1,false);
					spr[0].Battle((gm.day/7)*3+luck,false);
				}
				else
				{
					Story(23,1,false);
					gm.money+=(luck+2);
					PutFontOutline(200,540,GREEN,"돈+%dK",luck+2);
					_CopyScreen(false);
					Click();
				}
				break;
			case 3:
				if(mn[0].att >= 3 && mn[0].shoes >= 3 && mn[1].att >= 3 && mn[1].shoes >= 3)
				{
					Story(260, 1, true);
					Change(16);

					//술먹기 게임
					if(mn[0].att < 4 || mn[1].att < 4)
					{
						Story(261, 4, true);
						if(spr[0].Drink() > 0)
						{
							Story(265, 1, true);
							if(mn[0].shoes < 4)
							{
								mn[0].shoes = 4;
							}
							else
							{
								mn[1].shoes = 4;
							}
						}
						else
						{
							Story(266, 1, true);
						}
					}
					else
					{
						Click();
					}
				}
				else
				{
					Story(259, 1, true);
					Shopping();
				}

				break;
		}
		if(!sonDid) switch(sonDo)
		{
			case 0:
				mn[1].hp=100;
				break;
			case 1:
				mn[1].exp+=(10+rand()%5);
				break;
			case 2:
				luck=rand()%6;
				if(luck<3)
				{
					Change(1);
					int sonPow=(mn[1].pow+mn[1].att+mn[1].shoes)*5;
					int pow1, pow2;
					for(int i1=0;i1<gm.day+10 && mn[1].hp>0;)
					{
						pow1=rand()%sonPow;
						pow2=rand()%gm.day;
						if(pow1>pow2)i1++;
							else mn[1].hp--;
					}
					if(mn[1].hp>0)//승리
					{
						Story(38,5,true);
						mn[1].exp+=(20+gm.day/7);
						mn[1].just+=2;
					}
					else //패배
					{
						mn[1].hp++;
						Story(43,3,true);
					}
				}
				break;
			case 3:
				mn[1].just+=(3+rand()%2);
				break;
		}
	}
	for(int i1=0;i1<2;i1++){
		if(mn[i1].hp<0)mn[i1].hp=1;
		if(mn[i1].exp>99){
			mn[i1].exp-=100;
			if(mn[i1].pow<5)mn[i1].pow++;
		}
		if(mn[i1].just>100)mn[i1].just=100;
	}

	Change(7); Change(1);
	gm.day++; Nsave=true;
	if(!(gm.day%7)){
		switch(gm.day/7)
		{
			case 1:
				Story(69,4,true);
				break;
			case 2:
				Story(103,5,true);
				Change(99); Change(1);
				_MidiPlay("music//first.mid",true);
				Story(108,8,true);
				spr[0].Battle(8,true);
				Story(116,4,true);
				break;
			case 3:
				Story(135,7,true);
				break;
			case 4:
				Story(160,3,true);
				BmpScreen[3] = DDLoadBitmap( DirectOBJ, scr[gm.day/7], 0, 0, SYSTEM);
				Change(99); Change(1);
				_MidiPlay(bgm[gm.day/7], true);
				Story(163,10,false);
				spr[0].Battle(22,true);
				Story(173,8,false);
				gm.money+=10;
				break;
			case 5:
				Story(202, 3, true);
				Change(99); Change(1);
				_MidiPlay(bgm[gm.day/7], true);
				Story(205, 6, true);
				break;
			case 6:
				Story(211, 3, true);
				BmpScreen[3] = DDLoadBitmap( DirectOBJ, scr[gm.day/7], 0, 0, SYSTEM);
				Change(99); Change(1);
				_MidiPlay(bgm[gm.day/7], true);
				Story(214, 2, false);
				break;
			case 7:
				_MidiPlay("music//first.mid",true);
				Story(224,4,true);
				spr[0].Battle(27,false);
				_MidiPlay("music//first.mid",true);
				Story(228,13,true);
				Change(99); Change(1);
				_MidiPlay("music//end.mid", true);
				Story(241,4,true);

				//엔딩 분기
				if(mn[1].pow >= 5 && gm.vella_flag == 3)	//이혼 엔딩
				{
					Change(13);
					Story(352,4,true);
				}
				else if(mn[1].pow >= 5 && mn[1].just >= 100)	//용사 엔딩
				{
					Change(9);
					Story(248,6,true);
				}
				else if(mn[1].pow >= 5)	//마왕 엔딩
				{
					Change(10);
					Story(245,3,true);
				}
				else if(mn[1].just >= 100)	//사망 엔딩
				{
					Change(11);
					Story(254,2,true);
				}
				else if(mn[1].pow >= 3)	//병사 엔딩
				{
					Change(12);
					Story(256,3,true);
				}
				else	//효도 엔딩
				{
					Change(14);
					Story(356,5,true);
				}

				Change(15);
				Click();
				Quit = true;
				break;
		}

		_MidiPlay(bgm[gm.day/7], true);
		BmpScreen[3] = DDLoadBitmap( DirectOBJ, scr[gm.day/7], 0, 0, SYSTEM);
		gm.out=(gm.day/7)*3;
	}
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	jdd=CreateDirectDraw();
	jre=CreateDXResourceManager(jdd);

	WNDCLASS wc={0};
	wc.hIcon=LoadIcon(hInstance,"icon1.ico");
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

		MainHwnd = CreateWindow("Game", "용사탄생", ws, 100, 100, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);
		ShowCursor( FALSE );
	}
	else
	{
		MainHwnd=CreateWindow("Game","용사탄생",WS_POPUP|WS_VISIBLE,0,0,SCREEN_X,SCREEN_Y,NULL,NULL,hInstance,NULL);
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
	jdd->SetFrameRate(100);
	
	//윈도우창 이동
	if(window_mode)
	{
		jdd->OnMove(100, 100);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}

	Init();

	//한글 초기화
    //InitXddFont( "HANGUL.FNT", "ENGLISH.FNT" );
    //SetFontPattern( NORMAL_PATTERN );

	//그래픽 초기화
	BmpScreen[1] = DDLoadBitmap( DirectOBJ, "DATA//face.bmp", 0, 0, SYSTEM);
	BmpScreen[2] = DDLoadBitmap( DirectOBJ, "DATA//etc.bmp", 0, 0, SYSTEM);
	BmpScreen[4] = DDLoadBitmap( DirectOBJ, "DATA//tile.bmp", 0, 0, SYSTEM);
	DDSetColorKey( BmpScreen[2], JColor(0,0,255) );

	//음향 초기화
	_MidiPlay("music//open.mid", true);
	if ( SoundCard )
		{
			Sound[0] = SndObjCreate(SoundOBJ,"Sound//uuu.WAV",2);
			Sound[1] = SndObjCreate(SoundOBJ,"Sound//boomerang.WAV",2);
			Sound[2] = SndObjCreate(SoundOBJ,"Sound//die.WAV",2);
			Sound[3] = SndObjCreate(SoundOBJ,"Sound//oh.WAV",2);
			Sound[4] = SndObjCreate(SoundOBJ,"Sound//wind.WAV",2);
			Sound[5] = SndObjCreate(SoundOBJ,"Sound//land.WAV",2);
			Sound[6] = SndObjCreate(SoundOBJ,"Sound//hoho.WAV",2);
			Sound[7] = SndObjCreate(SoundOBJ,"Sound//zzz.WAV",2);
			Sound[8] = SndObjCreate(SoundOBJ,"Sound//drug.WAV",2);
			Sound[9] = SndObjCreate(SoundOBJ,"Sound//summon.WAV",2);
			Sound[10] = SndObjCreate(SoundOBJ,"Sound//drink.WAV",2);
			Sound[11] = SndObjCreate(SoundOBJ,"Sound//sell.WAV",2);
        }
	//변수 초기화
	srand( (unsigned)time( NULL ) );

	FILE* fp;
	if(!(fp=fopen("hero.sav","rb")))Opening();
	else{
		unsigned char part1, part2;
		fscanf(fp,"%c%c%c%c",&gm.day,&gm.out,&part1,&part2);
		gm.money=part1*256+part2;
		for(int i1=0;i1<2;i1++)
			fscanf(fp,"%c%c%c%c%c%c",&mn[i1].att,&mn[i1].exp,&mn[i1].hp,&mn[i1].just,&mn[i1].pow,&mn[i1].shoes);
		fscanf(fp,"%c", &gm.vella_flag);
		_MidiPlay(bgm[gm.day/7], true);
		BmpScreen[3] = DDLoadBitmap( DirectOBJ, scr[gm.day/7], 0, 0, SYSTEM);
		Change(1); Nsave=false;
		fclose(fp);
	}
	bool PreQuit=false;
	while (!Quit){PRC{
		if(_GetKeyState(VK_ESCAPE)){
			if(!key2){
				if(!PreQuit)PreQuit=true;
					else Quit=true;
				key2=true;
			}
		}
		else key2=false;
		ani(10);
		MainScr();
		SetRect(&BackRect, 160, 0, 320, 160);
		_DrawBmp(BackRect, 20, 420, BmpScreen[1], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
		PutFontOutline(200,420,YELLOW,"[분노의 대갈]");
		PutFontOutline(200,440,WHITE,"자! 그럼 스케쥴을 짜 볼까?");
		PutFontOutline(200,460,WHITE,"(그림을 클릭하면 예정을 바꿀 수 있습니다)");
		PutFontOutline(200,480,BLUE,"분노의 대갈   아들내미");
		SetRect(&BackRect, doing[0]*50, 20, doing[0]*50+50, 70);
		_DrawBmp(BackRect, 220, 510, BmpScreen[2], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
		SetRect(&BackRect, doing[1]*50, 20, doing[1]*50+50, 70);
		_DrawBmp(BackRect, 330, 510, BmpScreen[2], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
		SetRect(&BackRect, 100, 70, 150, 120);
		if(Nsave)_DrawBmp(BackRect, 730, 420, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		if(MouseX>=440 && MouseX<490 && MouseY>=510 && MouseY<560&&LeftButton)SetRect(&BackRect, 50, 70, 100, 120);
			else SetRect(&BackRect, 0, 70, 50, 120);
		_DrawBmp(BackRect, 440, 510, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		if(PreQuit)PutFontOutline(200,200,YELLOW,"한번 더 ESC를 누르면 나갑니다.");

		//마우스 포인트
		_DrawBox(MouseX-(5+Frame),MouseY-(5+Frame),MouseX+(5+Frame),MouseY+(5+Frame),RED);
		_CopyScreen(false);

		if(LeftButton)
		{
			if(!key)
			{
				if(MouseX>=220 && MouseX<270 && MouseY>=510 && MouseY<560)doing[0]++;
				if(MouseX>=330 && MouseX<380 && MouseY>=510 && MouseY<560)doing[1]++;
				doing[0]%=4; doing[1]%=4;
				if(MouseX>=730 && MouseX<780 && MouseY>=420 && MouseY<470)save();
				if(MouseX>=440 && MouseX<490 && MouseY>=510 && MouseY<560)Event(doing[0], doing[1]);
				key=true;
			}
			PreQuit=false;
		}
		else key=false;
	}}
	Change(99);
	_MidiStop();
	DestroyWindow( MainHwnd );

    return TRUE;
}