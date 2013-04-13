// Battle.cpp: implementation of the CBattle class.
//
//////////////////////////////////////////////////////////////////////

#include "Battle.h"
#include "extern.h"
#include "ddutil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
RECT MRect, CRect, SR1, SR2;
bool keyET, keyUP=false, keyB, keyH, Def;
BOOL LeftButton, RightButton;
char* StageName[]={"헤멤의 숲","카베사막","칼날계곡","죽음의 늪","마왕성"};

int CBattle::Max(int x, int y)
{
	if(x>y)return x;
		else return y;
}

int CBattle::Min(int x, int y)
{
	if(x<y)return x;
		else return y;
}

void CAni::SetAni(int sframe, bool sloop, int sx, int sy, int sxx, int syy,
		int sax, int say, int saxx, int sayy, int sdx, int sdy, int sdxx, int sdyy, int x3)
{
	frame=sframe;
	if(frame<3)loop=false;
		else loop=sloop;
	x=sx; xx=sxx; y=sy; yy=syy; x2=x3;

	if(sax==-1)
	{
		ax=999; axx=999; ay=999; ayy=999;
	}
	else if(sax==-2)
	{
		ax=0; axx=sxx*x3; ay=0; ayy=syy*x3;
	}
	else
	{
		ax=sax*x3; axx=saxx*x3; ay=say*x3; ayy=sayy*x3;
	}

	if(sdx==-1)
	{
		dx=999; dxx=999; dy=999; dyy=999;
	}
	else if(sdx==-2)
	{
		dx=0; dxx=sxx*x3; dy=0; dyy=syy*x3;
	}
	else
	{
		dx=sdx*x3; dxx=sdxx*x3; dy=sdy*x3; dyy=sdyy*x3;
	}
}

void CAni::PutAni(int px, int py, int temp, bool aid)
{
	if(loop)
	{
		int f=frame*2-2;
		temp=temp%f;
		if(temp>=frame)temp=f-temp;
	}
	else temp%=frame;

	SetR(&MRect, x+(temp*xx), y, xx, yy);
	if(aid)_DrawBmp2(MRect, px, py, BmpScreen[2], DDBLT_WAIT | DDBLT_KEYSRC, CRect, x2);
		else _DrawBmp2(MRect, px, py, BmpScreen[2], DDBLT_DDFX | DDBLT_WAIT | DDBLT_KEYSRC, CRect, x2);
}

void CChr::SetChr(int t1, int p1, int t2, int p2, int t3, int p3, int t4, int p4, int t5, int p5)
{
	type[0]=t1; type[1]=t2; type[2]=t3; type[3]=t4; type[4]=t5; 
	pic[0]=p1; pic[1]=p2; pic[2]=p3; pic[3]=p4; pic[4]=p5;
}

void CSpr::NewSpr(int vx, int vy, int c, int k, int h, int m, int a, int d, int s, int te, int nw)
{
	x=vx; y=vy;
	chr=c; kind=k; temp=0; tempo=te;
	hp=h; mp=m; att=a; def=d; speed=s; dir=true;
	now=nw; delay=0; life=true; odd=0;
}

CBattle::CBattle()
{
	ani[0].SetAni(3, true, 0, 0, 90, 150, -1, 0, 0, 0, -1, 0, 0, 0); //용사
	ani[1].SetAni(2, true, 270, 0, 100, 150, 0, 0, 30, 150, -1, 0, 0, 0);
	ani[2].SetAni(1, true, 470, 0, 90, 150, -2, 0, 0, 0, -2, 0, 0, 0);
	ani[3].SetAni(1, true, 560, 0, 70, 150, -1, 0, 0, 0, -2, 0, 0, 0);
	ani[4].SetAni(3, true, 0, 150, 90, 100, -1, 0, 0, 0, -1, 0, 0, 0); //방갈
	ani[5].SetAni(1, true, 270, 150, 90, 100, -1, 0, 0, 0, -1, 0, 0, 0); 
	ani[6].SetAni(1, true, 0, 250, 90, 110, -2, 0, 0, 0, -1, 0, 0, 0); //배추
	ani[7].SetAni(3, true, 360, 150, 90, 130, -2, 0, 0, 0, -1, 0, 0, 0); //큰바위
	ani[8].SetAni(1, true, 90, 250, 90, 130, -1, 0, 0, 0, -2, 0, 0, 0); 
	ani[9].SetAni(3, true, 0, 380, 90, 150, -1, 0, 0, 0, -1, 0, 0, 0); //기사
	ani[10].SetAni(2, true, 270, 380, 100, 150, 0, 60, 40, 75, -1, 0, 0, 0);
	ani[11].SetAni(1, true, 470, 380, 75, 150, -1, 0, 0, 0, 0, 0, 40, 150);
	ani[12].SetAni(1, true, 545, 380, 85, 150, -1, 0, 0, 0, -2, 0, 0, 0); 
	ani[13].SetAni(1, true, 180, 250, 70, 60, -1, 0, 0, 0, -1, 0, 0, 0); //마왕
	ani[14].SetAni(1, true, 250, 250, 70, 60, -1, 0, 0, 0, -1, 0, 0, 0);
	ani[15].SetAni(3, true, 0, 150, 90, 100, -1, 0, 0, 0, -1, 0, 0, 0, 2); //왕방갈
	ani[17].SetAni(3, true, 390, 300, 80, 80, -1, 0, 0, 0, -2, 0, 0, 0, 2); //폭발
	ani[18].SetAni(3, true, 390, 300, 80, 80, -2, 0, 0, 0, -2, 0, 0, 0, 2); //폭발마법
	ani[19].SetAni(3, true, 390, 300, 80, 80, -2, 0, 0, 0, -2, 0, 0, 0, 4); //대폭발마법
	ani[20].SetAni(3, true, 390, 300, 80, 80, -1, 0, 0, 0, -2, 0, 0, 0, 4); //대폭발
	ani[21].SetAni(3, true, 0, 150, 90, 100, -1, 0, 0, 0, -1, 0, 0, 0, 2); //방갈
	ani[22].SetAni(1, true, 270, 150, 90, 100, -1, 0, 0, 0, -1, 0, 0, 0, 2); 
	ani[23].SetAni(1, true, 0, 250, 90, 110, -2, 0, 0, 0, -1, 0, 0, 0, 2); //배추
	ani[24].SetAni(3, true, 360, 150, 90, 130, -2, 0, 0, 0, -1, 0, 0, 0, 2); //큰바위
	ani[25].SetAni(1, true, 90, 250, 90, 130, -1, 0, 0, 0, -2, 0, 0, 0, 2); 
	ani[26].SetAni(3, true, 0, 380, 90, 150, -1, 0, 0, 0, -1, 0, 0, 0, 2); //기사
	ani[27].SetAni(2, true, 270, 380, 100, 150, 0, 60, 40, 75, -1, 0, 0, 0, 2);
	ani[28].SetAni(1, true, 470, 380, 75, 150, -1, 0, 0, 0, 0, 0, 40, 150, 2);
	ani[29].SetAni(1, true, 545, 380, 85, 150, -1, 0, 0, 0, -2, 0, 0, 0, 2); 
	ani[30].SetAni(3, true, 390, 300, 80, 80, -2, 0, 0, 0, -2, 0, 0, 0); //파이어볼
	ani[31].SetAni(1, true, 180, 310, 30, 30, -2, 0, 0, 0, -2, 0, 0, 0); //물방울
	ani[32].SetAni(1, true, 180, 310, 30, 30, -2, 0, 0, 0, -2, 0, 0, 0, 2); //왕물방울
	chr[0].SetChr(0,0,1,1,2,2,3,3,4,17); //용사
	chr[1].SetChr(0,4,3,5,0,0,0,0,4,17); //방갈
	chr[2].SetChr(0,6,6,6,0,0,0,0,4,17); //배추
	chr[3].SetChr(0,7,3,8,0,0,0,0,4,17); //큰바위
	chr[4].SetChr(0,9,1,10,2,11,3,12,4,17); //기사
	chr[5].SetChr(0,13,3,14,0,0,0,0,4,17); //마왕손
	chr[6].SetChr(0,18,0,0,0,0,0,0,0,0); //폭파마법
	chr[7].SetChr(0,19,0,0,0,0,0,0,0,0); //대폭파마법
	chr[8].SetChr(0,21,3,22,0,0,0,0,4,20); //왕방갈
	chr[9].SetChr(0,23,6,23,0,0,0,0,4,20); //왕배추
	chr[10].SetChr(0,24,3,25,0,0,0,0,4,20); //왕큰바위
	chr[11].SetChr(0,26,1,27,2,28,3,29,4,20); //왕기사
	chr[12].SetChr(0,30,0,0,0,0,0,0,0,0); //파이어 볼
	chr[13].SetChr(0,31,0,0,0,0,0,0,0,0); //물방울
	chr[14].SetChr(0,32,0,0,0,0,0,0,0,0); //왕물방울
	menu=0; drag=0; drop=0;
	stage=0; big=0; win=0;
	tolk=-1; tdelay=0; 
}

void CBattle::PutBack()
{
	SetRect(&MRect, 0, 0, 800, 400);
	_DrawBmp(MRect, 0, 300, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	SetR(&MRect, 0, 40, 80, 300);
	for(int i=0;i<11;i++)
		_DrawBmp(MRect, i*80-((scroll%240)/3), 0, BmpScreen[1], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);

	//State
	PutFontOutline(100,320,YELLOW,"Level.%d   Exp: %d/%d",lv,exp,next);
	PutFontOutline(100,460,WHITE,"Stage.%d   %s   진행도: %d%%",stage+1,StageName[stage],Min(scroll/100,100));

	//HP
	SetR(&MRect, 0, 360, 3, 20);
	for(i=0;i<spr[0].hp;i++)
		_DrawBmp(MRect, i*4+101, 340, BmpScreen[2], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	for(i=0;i<spr[SMAX].hp;i++)
		_DrawBmp(MRect, i*4+101, 420, BmpScreen[2], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	PutFontOutline(510,340,RED,"%d%%",spr[0].hp);
	PutFontOutline(510,420,RED,"%d%%",spr[SMAX].hp);
	
	//MP
	SetR(&MRect, 3, 360, 3, 20);
	for(i=0;i<spr[0].mp;i++)
		_DrawBmp(MRect, i*4+101, 360, BmpScreen[2], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	for(i=0;i<spr[SMAX].mp;i++)
		_DrawBmp(MRect, i*4+101, 440, BmpScreen[2], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
	PutFontOutline(510,360,BLUE,"%d%%",spr[0].mp);
	PutFontOutline(510,440,BLUE,"%d%%",spr[SMAX].mp);

	//명령
	SetR(&MRect, 180, 340, 40, 40);
	if(drag>=25)_DrawBmp(MRect, 630+(drag/25-1)*40, 410, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	if(drop>=25)_DrawBmp(MRect, 630+(drop/25-1)*40, 450, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

void CBattle::PutFront()
{
	SetR(&MRect, 0, 0, 80, 40);
	for(int i=0;i<15;i++)
		_DrawBmp(MRect, i*60-(scroll%60), 260, BmpScreen[1], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	SetR(&MRect, 570, 280, 20, 20);
	for(i=0;i<big;i++)
		_DrawBmp(MRect, i*20, 0, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

	if(tolk>=0)
	{
		switch(tolk)
		{
			case 0: Talk(0, "불속성 마법 [화르]!!!", "", ""); break;
			case 1: Talk(0, "폭열성 마법 [콰쿠우오]!!!", "", ""); break;
			case 2: Talk(0, "회복 마법 [피가모가]!!!", "", ""); break;
			case 3: Talk(0, "방어막!!!", "", ""); break;
			case 4: Talk(1, "나와랏! 나의 귀여운 물방울, 방갈.", "", ""); break;
			case 5: Talk(1, "절묘한 얼굴로 놈을 제압해라! 파마배추!", "", ""); break;
			case 6: Talk(1, "몸으로 밀어 붙여라! 큰 바위!", "", ""); break;
			case 7: Talk(1, "우리 마왕군의 정예병, 암흑기사!", "눈앞의 적을 섬멸해라!", ""); break;
			case 8: Talk(1, "폭열성 마법 [콰쿠우]", "", ""); break;
			case 9: Talk(1, "후후후, 한 동안 못 움직일 거다!", "", ""); break;
			case 10: Talk(1, "후후후, 곧 나의 거대한 부하가 널 짓밟아 줄 것이다.", "기대하고 있거라!", ""); break;
			case 11: Talk(1, "자! 힘내라! 우리 마왕군들이여!", "용사를 물리치고 마계의 평화를 찾자!", ""); break;
			case 12: Talk(0, "크아아아악!", "아버지 죄송... 미안해, 형...", "커억..."); break;
			case 13: Talk(1, "끄아아악! 하지만 이대로 끝은 아니다!", "반드시 다시 되살아나 이 세계를 마족의 세상으로 만들겠다.", "잠시동안 평화를 누려라. 인간들아! 우하하하!"); break;
			case 14: Talk(1, "나의 5스테이즈를 모두 통과하다니 적이지만 훌륭하군.", "...가 아니군. 용사를 못 해치우다니...", "난 이제 대마왕님께 죽었다!!!"); break;
		}
		tdelay--;
		if(tdelay<0)tolk=-1;
	}
}

void CBattle::Clear()
{
	scroll=0;
	SetR(&CRect, 0, 0, 799, 299);
	for(int i=0; i<SMAX; i++)
		spr[i].life=false;
	spr[0].NewSpr(0, 150, 0, 0, 100, 100, 1, 0, 3, 10, chr[0].type[0]);
	spr[SMAX].NewSpr(500, 150, 5, 10, 100, 100, 1, 0, 99, 10, chr[5].type[0]);
	jump=JUMPMAX;
	frz=0;
}

void CBattle::CtrSpr()
{
	int tmp, df, j;
	for(int i=0; i<=SMAX; i++)
	{
		if(!spr[i].life)continue;
		if(spr[i].hp<=0 && spr[i].now!=4)
		{
			spr[i].now=4;
			spr[i].delay=spr[i].tempo*3;
			AddSND(1);
		}
		tmp=State(i);
		//원상복귀
		if(!normal(i)){
			if(spr[i].delay<=0){
				spr[i].now=chr[spr[i].chr].type[0];
				spr[i].temp=0;
			}
			spr[i].delay--;
		}
		//종류행동
		spr[i].temp++;
		
		if(spr[i].y+ani[tmp].yy*ani[tmp].x2>CRect.bottom+1)spr[i].y--;
		else switch(spr[i].kind)
		{
			case 11: if(!normal(i))break;
					 spr[i].x-=spr[i].speed;
					 if(spr[i].x<spr[0].x)spr[i].dir=false;
				 		else spr[i].dir=true;
					 if(spr[i].temp>=200)
					 {
						 j=SprNo();
						 if(j!=-1)
						 {
							 if(spr[i].chr==1)spr[j].NewSpr(spr[i].x+90+(-120*spr[i].dir),spr[i].y+70,13,22,200,0,1,0,5,5,chr[13].type[0]);
								else spr[j].NewSpr(spr[i].x+180+(-240*spr[i].dir),spr[i].y+140,14,22,200,0,2,0,5,5,chr[14].type[0]);
							 AddSND(8);
						 }
						 spr[i].temp=0; spr[j].dir=spr[i].dir;
					 }
					 break;
			case 12: spr[i].x-=spr[i].speed;
					 if(normal(i)){
						spr[i].delay=120;
						spr[i].now=6; spr[i].temp=0;
						AddSND(11);;
					 }
					break;
			case 13: if(!normal(i))break;
					 walk(i,true);
					 if(spr[i].x<=0)spr[i].dir=false;
					 if(spr[i].x+ani[tmp].xx*ani[tmp].x2>=800)spr[i].dir=true;
					 break;
			case 14: if(!normal(i))break;
					if(spr[i].x<spr[0].x)spr[i].dir=false;
						else spr[i].dir=true;
					 df=spr[0].x-spr[i].x;
					 if(df<0)df=-(df)+10;
					 if(spr[0].now==1 && spr[0].dir!=spr[i].dir && df<100)
					 {
			 			spr[i].now=2; spr[i].temp=0;
						spr[i].delay=30;
					 }
					 else if(df<100)
					 {
			 			spr[i].now=1; spr[i].temp=0;
						spr[i].delay=spr[i].tempo*2;
						AddSND(10);
					 }
					 else walk(i,true);
					 break;
			case 21: spr[i].hp--;
					 if(spr[i].hp<=0)spr[i].life=false;
					 break;
			case 22: spr[i].hp--; walk(i,true);
					 if(spr[i].hp<=0 || spr[i].x+ani[tmp].xx<0 || spr[i].x>800)spr[i].life=false;
					 break;
		}
		if(spr[i].x< ani[tmp].xx*ani[tmp].x2*-1)spr[i].life=false;
		//특수행동
		if(spr[i].now==3 && i!=50)walk(i,false);
		if(spr[i].now==4 && spr[i].delay<=0)spr[i].life=false;
		if(spr[i].now==6)spr[i].y+=((spr[i].temp-60)/6);
		//이동제한
		if(i==0)
		{
			if(spr[i].x<0)spr[i].x=0;
			if((spr[i].x+ani[tmp].xx)>799)spr[i].x=799-ani[tmp].xx;
		}

		//충돌검사
		if(spr[i].dir)
		{
			SR1.left=spr[i].x+ani[tmp].ax;
			SR1.right=ani[tmp].axx+SR1.left;
			SR1.top=spr[i].y+ani[tmp].ay;
			SR1.bottom=ani[tmp].ayy+SR1.top;
		}
		else
		{
			SR1.right=spr[i].x+(ani[tmp].xx-ani[tmp].ax);
			SR1.left=SR1.right-ani[tmp].axx;
			SR1.top=spr[i].y+ani[tmp].ay;
			SR1.bottom=ani[tmp].ayy+SR1.top;
		}
		//if(i==2)PutFontOutline(0,60,WHITE,"%d",spr[i].now);
		
		if(i<2)
		{
			for(j=1; j<=SMAX; j++)
				if(!spr[j].life)continue;
				else Bump(i,j);
		}
		else Bump(i,0);
		
		if(spr[i].odd==2)
			ani[tmp].PutAni(spr[i].x, spr[i].y, 0, spr[i].dir);
		else
			ani[tmp].PutAni(spr[i].x, spr[i].y, spr[i].temp/spr[i].tempo, spr[i].dir);
		//상태이상
		if(spr[i].odd>0){
			SetR(&MRect, 570+20*spr[i].odd, 280, 20, 20);
			_DrawBmp(MRect, spr[i].x, spr[i].y, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		}
	}
}

void CBattle::Key()
{
	int No, i;
	if(MouseX>740)MouseX=740;
	if(MouseY>240)MouseY=240;
	spr[SMAX].x=MouseX; spr[SMAX].y=MouseY;
//승리검사
	if(!spr[SMAX].life && !win && win!=99)
	{
		win=1;
		SetTolk(13, 500);
		for(i=2;i<SMAX;i++)
			spr[i].hp=0;
	}

	if(!spr[0].life && !win && win!=99)
	{
		win=-1;
		SetTolk(12, 500);
	}
//마왕
	if(LeftButton && spr[SMAX].life)
	{
		drag++;
		if(!(drag%25) && drag)AddSND(9);;
		if(spr[SMAX].mp< (drag/25)*5|| drag>=125)drag=0;
	}
	else if(drag>0)
	{
		No=SprNo();
		if(No!=-1)
		{
			int attUP=(type[0]==3)?1:0;
			//소환
			if(big<=0)switch(drag/25)
			{
				case 1:	spr[No].NewSpr(spr[SMAX].x,300,1,11,3,0,1+attUP,0,1,10,chr[1].type[0]); AddSND(5); SetTolk(4); break;
				case 2:	spr[No].NewSpr(spr[SMAX].x,190,2,12,1,0,2+attUP,0,1,10,chr[2].type[0]); AddSND(5); SetTolk(5); break;
				case 3:	spr[No].NewSpr(spr[SMAX].x,190,3,13,5,0,1+attUP,0,2,10,chr[3].type[0]); AddSND(5); SetTolk(6); break;
				case 4:	spr[No].NewSpr(spr[SMAX].x,300,4,14,10,0,3+attUP,0,3,10,chr[4].type[0]); AddSND(5); SetTolk(7); break;
			}
			else
			{
				switch(drag/25)
				{
					case 1:	spr[No].NewSpr(spr[SMAX].x,300,8,11,6,0,1+attUP,0,1,10,chr[8].type[0]); big--; AddSND(5); SetTolk(4); break;
					case 2:	spr[No].NewSpr(spr[SMAX].x,190,9,12,1,0,2+attUP,0,1,10,chr[9].type[0]); big--; AddSND(5); SetTolk(5); break;
					case 3:	spr[No].NewSpr(spr[SMAX].x,190,10,13,10,0,1+attUP,0,2,10,chr[10].type[0]); big--; AddSND(5); SetTolk(6); break;
					case 4:	spr[No].NewSpr(spr[SMAX].x,300,11,14,20,0,3+attUP,0,3,10,chr[11].type[0]); big--; AddSND(5); SetTolk(7); break;
				}				
			}
			spr[SMAX].mp-=(drag/25);
		}
		drag=0;
	}

	if(RightButton && spr[SMAX].life)
	{
		drop++;
		if(!(drop%25) && drop)AddSND(9);;
		if(spr[SMAX].mp< drop/25|| drop>=125)drop=0;
	}
	else if(drop>0)
	{
		No=SprNo();
		if(No!=-1)
		{
			switch(drop/25)
			{
				case 1:	spr[No].NewSpr(spr[SMAX].x,spr[SMAX].y,6,21,100,0,1,0,1,5,chr[6].type[0]); AddSND(1); SetTolk(8); break;
				case 2:	frz=100; spr[0].odd=2; AddSND(4); SetTolk(9); break;
				case 3:	big++; SetTolk(10); break;
				case 4:	for(i=2;i<SMAX;i++){spr[i].att*=2; spr[i].odd=1;} AddSND(6); SetTolk(11); break;
			}
			spr[SMAX].mp-=(drop/25)*5;
		}
		drop=0;
	}
//용사
	if(frz>0 || !spr[0].life)
	{
		frz--;
		if(frz==0)spr[0].odd=0;
		return;
	}
	//컴퓨터
	if(com)
	{
		//정황검사
		int mon=0, spot, Obeet=999, target=-1, it;
		bool jum=false;
		for(i=2;i<SMAX+1; i++)
		{
			if(spr[i].life)
			{
				int beet=999;
				if(spr[i].chr==13 || spr[i].chr==14)
				{
					if(spr[i].dir && spr[i].x<spr[0].x+200)jum=true;
						else if(!spr[i].dir && spr[i].x>spr[0].x+100)jum=true;
				}
				else mon++;

				it=ani[chr[spr[i].chr].type[0]].xx*ani[chr[spr[i].chr].type[0]].x2;
				if(spr[0].x+90<spr[i].x)beet=spr[i].x-spr[0].x-100;
					else if(spr[0].x>spr[i].x+it)beet=spr[0].x-spr[i].x-it;
				if(beet<Obeet)
				{
					Obeet=beet;
					target=i;
				}
			}
		}
		if(target==-1 && normal(SMAX))target=SMAX;
		if(spr[0].x<200)spot=0;
			else if(spr[0].x<400)spot=1;
			else spot=2;
		//행동
		if(target>=0)
		{
			if(spr[0].x+90<spr[target].x)Hero(0);
				else if(spr[0].x>spr[target].x+it-20)Hero(1);
			if(spr[target].y<100)jum=true;
		}

		if(jum&&jump>0&&!keyUP)Hero(3);
		else if((spr[0].y+ani[chr[spr[0].kind].pic[0]].yy)<BASE)Hero(4);

		if(spr[0].hp<100 && spr[0].mp==100)Hero(7);
			else if(spr[0].hp<=50 && spr[0].now!=2 && spr[0].mp>=20)Hero(8);
			else if(spr[0].hp<=50 && spr[0].mp>=15)Hero(7);
			else if(mon>5 && spr[0].mp>=5 && spr[0].y>100 && !spr[1].life)
			{
				if(spot==0){spr[0].dir=false; Hero(5);}
					else if(spot==2){spr[0].dir=true; Hero(5);}
					else if(spr[0].mp>=10)Hero(6);
			}
			else if(mon>5 && spot==1 && spr[0].mp>=10 && !spr[1].life)Hero(6);
			else if(normal(0))Hero(2);
		return;
	}
//조종
	if(_GetKeyState(VK_RIGHT))Hero(0);
	else if(_GetKeyState(VK_LEFT))Hero(1);
	if(_GetKeyState(VK_RETURN)){
		if(!keyET&&spr[0].now==0)Hero(2);
	}
	else keyET=false;
	
	if(_GetKeyState(VK_UP)&&jump>0&&!keyUP)Hero(3);
	else if((spr[0].y+ani[chr[spr[0].kind].pic[0]].yy)<BASE)Hero(4);
//마법
	if(_GetKeyState('F') && !spr[1].life && spr[0].mp>=5)Hero(5);
	if(_GetKeyState('E') && !spr[1].life && spr[0].mp>=10)Hero(6);
	if(_GetKeyState('H')){
		if(!keyH && spr[0].hp<100 && spr[0].mp>=15){
			keyH=true; Hero(7);
		}
	}
	else keyH=false;
	if(_GetKeyState('B')){
		if(!keyB && spr[0].now!=2 && spr[0].mp>=20){
			keyB=true;
			Hero(8);
		}
	}
	else keyB=false;
}

void CBattle::Hero(int doing)
{
	switch(doing)
	{
		case 0:
			spr[0].dir=false;
			spr[0].x+=spr[0].speed;
			break;
		case 1:
			spr[0].dir=true;
			spr[0].x-=spr[0].speed;
			break;
		case 2:
			spr[0].now=1; spr[0].temp=0;
			spr[0].delay=spr[0].tempo*2;
			AddSND(10);
			keyET=true;
			break;
		case 3:
			spr[0].y-=(jump/(JUMPMAX/10));
			if(jump==JUMPMAX)AddSND(12);
			jump--;
			if(jump==0)keyUP=true;
			break;
		case 4:
			keyUP=true;
			spr[0].y+=(jump/(JUMPMAX/10));
			if(jump<JUMPMAX)jump++;
			if((spr[0].y+ani[chr[spr[0].kind].pic[0]].yy)>=BASE)
			{
				keyUP=false;
				spr[0].y=BASE-ani[chr[spr[0].kind].pic[0]].yy;
			}
			break;
		case 5:
			spr[1].NewSpr(spr[0].x+5,spr[0].y+35,12,22,200,0,spr[0].att,0,5,5,chr[12].type[0]);
			spr[1].dir=spr[0].dir; spr[0].mp-=5; AddSND(0);	SetTolk(0);
			break;
		case 6:
			spr[1].NewSpr(spr[0].x,50,7,21,100,0,spr[0].att,0,1,5,chr[7].type[0]);
			spr[0].mp-=10; AddSND(1); SetTolk(1);
			break;
		case 7:
			spr[0].hp=Min(spr[0].hp+50, 100); spr[0].mp-=15;
			AddSND(2); SetTolk(2);
			break;
		case 8:
			spr[0].now=2; spr[0].temp=0;
			spr[0].delay+=500; spr[0].mp-=20;
			AddSND(3); SetTolk(3);
			break;
	}
}

int CBattle::SprNo()
{
	for(int i=2;i<SMAX;i++)
		if(!spr[i].life)return i;
	return -1;
}

int CBattle::Crush(int tg)
{
	int tmp2=State(tg);
	SR2.left=spr[tg].x;
	SR2.right=ani[tmp2].xx*ani[tmp2].x2+SR2.left;
	SR2.top=spr[tg].y;
	SR2.bottom=ani[tmp2].yy*ani[tmp2].x2+SR2.top;
	if(SR1.left<SR2.right && SR1.right>SR2.left && SR1.top<SR2.bottom && SR1.bottom>SR2.top)
	{
		if(ani[tmp2].dx==-2)
		SR2.left=spr[tg].x+ani[tmp2].dx;
		SR2.right=ani[tmp2].dxx+SR2.left;
		SR2.top=spr[tg].y+ani[tmp2].dy;
		SR2.bottom=ani[tmp2].dyy+SR2.top;
		if(SR1.left<SR2.right && SR1.right>SR2.left && SR1.top<SR2.bottom && SR1.bottom>SR2.top)
			return -1; //방어
		else
			return 1; // 공격
	}
	else
		return 0; //충돌 안 함
}

void CBattle::Bump(int i, int j)
{
	if(Crush(j)==1 && spr[j].now!=3)
	{
		spr[j].now=3;
		spr[j].dir=(spr[i].dir)?false:true;
		spr[j].delay=20;
		spr[j].hp-=Max(spr[i].att-spr[j].def/2,1);
		spr[j].hp=Max(spr[j].hp,0);
		if(i<2)
		{
			exp+=(spr[j].kind-9);
			if(type[1]==1)exp++;
		}
		AddSND(7);;
	}
}

CBattle::State(int no)
{
	for(int j=0;j<5;j++)
		if(chr[spr[no].chr].type[j]==spr[no].now)break;
	return chr[spr[no].chr].pic[j];
}

bool CBattle::normal(int i)
{
	if(spr[i].now==chr[spr[i].chr].type[0])return true;
		else return false;
}

void CBattle::LevelUp()
{
	lv++;
	spr[0].att++; spr[0].def++;
	next+=lv*lv*64;
}

void CBattle::walk(int i, bool front)
{
	if(front)
	{
	 if(spr[i].dir)spr[i].x-=spr[i].speed;
		else spr[i].x+=spr[i].speed;
	}
	else
	{
	 if(spr[i].dir)spr[i].x+=spr[i].speed*2;
		else spr[i].x-=spr[i].speed*2;
	}
}

void CBattle::AddSND(int no)
{
	for(int i=0;i<SNDMAX;i++)
		if(play[i]==-1){
			play[i]=no;
			break;
		}
}

void CBattle::Talk(int caracter, char* sen1, char* sen2, char* sen3)
{
	char* name[]={"용사:아들내미","마왕"};
	SetR(&MRect, (caracter%4)*160, (caracter/4)*120, 160, 120);
	if(caracter<8)_DrawBmp(MRect, 0, 480, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	if(caracter!=-1)PutFontOutline(180,510,YELLOW,"[%s]",name[caracter]);
	PutFontOutline(180,530,WHITE,sen1);
	PutFontOutline(180,550,WHITE,sen2);
	PutFontOutline(180,570,WHITE,sen3);
}

void CBattle::SetTolk(int what, int del)
{
	tolk=what; tdelay=del;
}

int* CBattle::Battle()
{
	if(!win)scroll++;
	for(int i=0;i<SNDMAX+1;i++)play[i]=-1;
	if(!(scroll%100))
	{
		//용사의 회복력
		if(spr[0].life && type[1]==0)spr[0].hp=Min(spr[0].hp+1,100);		
		if(spr[0].life)spr[0].mp=Min(spr[0].mp+1,100);
		//마왕의 회복력
		if(spr[SMAX].life && normal(SMAX))
		{
			int MPplus=(type[0]==1)?3:2;
			int HPplus=(type[0]==0)?3:1;
			spr[SMAX].mp=Min(spr[SMAX].mp+MPplus,100);
			spr[SMAX].hp=Min(spr[SMAX].hp+HPplus,100);
		}
	}
	if(Min(scroll/100,100)==100)
	{
		stage++; scroll=0;
		if(stage==5)
		{
			win=1; stage=4;
			return play;
			SetTolk(13,500);
		}
		if(stage==1)BmpScreen[1] = DDLoadBitmap( DirectOBJ, "data//stage2.bmp", 0, 0, SYSTEM);
		if(stage==2)BmpScreen[1] = DDLoadBitmap( DirectOBJ, "data//stage3.bmp", 0, 0, SYSTEM);
		if(stage==3)BmpScreen[1] = DDLoadBitmap( DirectOBJ, "data//stage4.bmp", 0, 0, SYSTEM);
		if(stage==4)BmpScreen[1] = DDLoadBitmap( DirectOBJ, "data//stage5.bmp", 0, 0, SYSTEM);
		play[SNDMAX]=stage;
		DDSetColorKey( BmpScreen[1], RGB(0,0,255) );
	}
	if(exp>=next)
		LevelUp();
	PutBack();
	Key();
	CtrSpr();
	PutFront();
	return play;
}