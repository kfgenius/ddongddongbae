// Battle.cpp: implementation of the CBattle class.
//
//////////////////////////////////////////////////////////////////////

#include "Battle.h"
#include "extern.h"
#include "ddutil.h"

#define PANIC 3
#define DIE   4
#define JUMP  6
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
RECT MRect, CRect, SR1, SR2;
bool keyET, keyUP=false, keyB, keyH, Def;
BOOL LeftButton, RightButton;

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

void CAni::PutAni(int px, int py, int temp, bool aid, int special)
{
	if(loop)
	{
		int f=frame*2-2;
		temp=temp%f;
		if(temp>=frame)temp=f-temp;
	}
	else temp%=frame;

	if(temp>4)temp=4;//마왕탄생에서만
	SetR(&MRect, x+(temp*xx), y, xx, yy);
	if(special<SMAX)
	{
		if(aid)_DrawBmp2(MRect, px, py, BmpScreen[BTLBMP], DDBLT_WAIT | DDBLT_KEYSRC, CRect, x2);
			else _DrawBmp2(MRect, px, py, BmpScreen[BTLBMP], DDBLT_DDFX | DDBLT_WAIT | DDBLT_KEYSRC, CRect, x2);
	}
	else _DrawBmp(MRect, px, py, BmpScreen[BTLBMP], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
}

//새로운 캐릭터 설정
void CChr::SetChr(int t1, int p1, int t2, int p2, int t3, int p3, int t4, int p4, int t5, int p5)
{
	type[0]=t1; type[1]=t2; type[2]=t3; type[3]=t4; type[4]=t5; 
	pic[0]=p1; pic[1]=p2; pic[2]=p3; pic[3]=p4; pic[4]=p5;
}

//새로운 스프라이트 출력
void CSpr::NewSpr(int vx, int vy, int vchr, int vkind, int vhp, int vmp,
				  int vatt, int vdef, int vspeed, int vtempo, int vnow)
{
	x=vx; y=vy;
	chr=vchr; kind=vkind; temp=0; tempo=vtempo;
	hp=vhp; mp=vmp; att=vatt; def=vdef; speed=vspeed; dir=true;
	now=vnow; delay=0; life=true;
	for(int i=0; i<3; i++)odd[i]=0;
}
/*-----------------------------------------------------------------
kind 변수 설정 상황
[0~9]용사측
0: 용사  1:주민
[10~20]마왕측
10: 마왕손 11: 방갈 12: 배추 13:큰바위 14:기사
[21~]마법
21: 대폭발 마법 22:파이어 볼 23:단검
------------------------------------------------------------------*/

CBattle::CBattle()
{
	ani[0].SetAni(3, true, 300, 0, 36, 64, -1, 0, 0, 0, -1, 0, 0, 0); //용사
	ani[1].SetAni(3, true, 285, 617, 48, 64, 0, 0, 30, 64, -1, 0, 0, 0);
	ani[2].SetAni(1, true, 380, 155, 36, 64, -1, 0, 0, 0, -2, 0, 0, 0);
	ani[3].SetAni(3, true, 225, 553, 44, 64, -1, 0, 0, 0, -1, 0, 0, 0);//천계의 장수
	ani[4].SetAni(3, true, 0, 70, 60, 70, -1, 0, 0, 0, -1, 0, 0, 0); //방갈
	ani[5].SetAni(1, true, 180, 70, 60, 70, -1, 0, 0, 0, -1, 0, 0, 0); 
	ani[6].SetAni(1, true, 0, 140, 60, 75, -2, 0, 0, 0, -1, 0, 0, 0); //배추
	ani[7].SetAni(3, true, 240, 70, 60, 85, -2, 0, 0, 0, -1, 0, 0, 0); //큰바위
	ani[8].SetAni(1, true, 60, 140, 60, 85, -1, 0, 0, 0, -2, 0, 0, 0); 
	ani[9].SetAni(3, true, 0, 225, 60, 100, -1, 0, 0, 0, -1, 0, 0, 0); //기사
	ani[10].SetAni(2, true, 180, 225, 70, 100, 0, 40, 30, 5, -1, 0, 0, 0);
	ani[11].SetAni(1, true, 320, 225, 50, 100, -1, 0, 0, 0, 0, 25, 10, 20);
	ani[12].SetAni(1, true, 370, 225, 60, 100, -1, 0, 0, 0, -2, 0, 0, 0); 
	ani[13].SetAni(1, true, 120, 165, 70, 60, -2, 0, 0, 0, -1, 0, 0, 0); //마왕
	ani[14].SetAni(1, true, 190, 165, 70, 60, -1, 0, 0, 0, -1, 0, 0, 0);
	ani[15].SetAni(3, true, 0, 0, 30, 64, -1, 0, 0, 0, -1, 0, 0, 0); //남자주민
	ani[17].SetAni(3, true, 240, 325, 60, 100, -1, 0, 0, 0, -2, 0, 0, 0); //불길
	ani[18].SetAni(3, true, 260, 155, 40, 40, -2, 0, 0, 0, -2, 0, 0, 0, 2); //폭발마법
	ani[19].SetAni(3, true, 260, 155, 40, 40, -2, 0, 0, 0, -2, 0, 0, 0, 4); //대폭발마법
	ani[20].SetAni(3, true, 260, 155, 40, 40, -1, 0, 0, 0, -2, 0, 0, 0, 4); //대폭발
	ani[21].SetAni(3, true, 90, 0, 30, 64, -1, 0, 0, 0, -1, 0, 0, 0); //여자주민
	ani[22].SetAni(3, true, 60, 425, 30, 64, -1, 0, 0, 0, -1, 0, 0, 0); //행복한 남자주민
	ani[23].SetAni(3, true, 150, 425, 30, 64, -1, 0, 0, 0, -1, 0, 0, 0); //행복한 여자주민
	ani[24].SetAni(3, true, 240, 425, 30, 64, -1, 0, 0, 0, -1, 0, 0, 0); //투쟁하는 남자주민
	ani[25].SetAni(3, true, 330, 425, 30, 64, -1, 0, 0, 0, -1, 0, 0, 0); //투쟁하는 여자주민
	ani[26].SetAni(3, true, 60, 489, 30, 64, -1, 0, 0, 0, -1, 0, 0, 0); //분노한 남자주민
	ani[27].SetAni(3, true, 150, 489, 30, 64, -1, 0, 0, 0, -1, 0, 0, 0); //분노한 여자주민
	ani[28].SetAni(3, true, 180, 0, 30, 64, -1, 0, 0, 0, -1, 0, 0, 0);//카오
	ani[29].SetAni(1, true, 270, 0, 30, 64, -1, 0, 0, 0, -2, 0, 0, 0);
	ani[30].SetAni(3, true, 260, 155, 40, 40, -2, 0, 0, 0, -2, 0, 0, 0); //파이어볼
	ani[31].SetAni(1, true, 120, 140, 15, 15, -2, 0, 0, 0, -2, 0, 0, 0); //물방울
	ani[32].SetAni(3, true, 240, 489, 42, 64, -1, 0, 0, 0, -1, 0, 0, 0);//여마법사
	ani[33].SetAni(1, true, 366, 489, 42, 64, -1, 0, 0, 0, -2, 0, 0, 0);
	ani[34].SetAni(10, true, 0, 553, 45, 96, -1, 0, 0, 0, -2, 0, 0, 0); //해골
	ani[35].SetAni(1, true, 357, 553, 44, 64, -2, 0, 0, 0, -2, 0, 0, 0);//천계의 장수 공격
	ani[36].SetAni(1, true, 175, 140, 30, 17, -2, 0, 0, 0, -2, 0, 0, 0); //단검

	chr[0].SetChr(0,0,1,1,2,2,3,3,4,17); 
	chr[1].SetChr(0,4,3,5,0,0,0,0,4,17); //방갈
	chr[2].SetChr(0,6,6,6,0,0,0,0,4,17); //배추
	chr[3].SetChr(0,7,3,8,0,0,0,0,4,17); //큰바위
	chr[4].SetChr(0,9,1,10,2,11,3,12,4,17); //기사
	chr[5].SetChr(0,13,3,14,0,0,0,0,4,17); //마왕손
	chr[6].SetChr(0,18,0,0,0,0,0,0,0,0); //폭파마법
	chr[7].SetChr(0,19,0,0,0,0,0,0,0,0); //대폭파마법
	chr[8].SetChr(0,15,0,0,0,0,0,0,4,34); //남자주민
	chr[9].SetChr(0,21,0,0,0,0,0,0,4,34); //여자주민
	chr[10].SetChr(0,22,0,0,0,0,0,0,4,34); //행복한 남자주민
	chr[11].SetChr(0,23,0,0,0,0,0,0,4,34); //행복한 여자주민
	chr[12].SetChr(0,30,0,0,0,0,0,0,0,0); //파이어 볼
	chr[13].SetChr(0,31,0,0,0,0,0,0,0,0); //물방울
	chr[14].SetChr(0,24,0,0,0,0,0,0,4,34); //투쟁하는 남자주민
	chr[15].SetChr(0,25,0,0,0,0,0,0,4,34); //투쟁하는 여자주민
	chr[16].SetChr(0,26,0,0,0,0,0,0,4,34); //분노한 남자주민
	chr[17].SetChr(0,27,0,0,0,0,0,0,4,34); //분노한 여자주민
	chr[18].SetChr(0,28,3,29,0,0,0,0,4,34); //카오
	chr[19].SetChr(0,32,3,33,0,0,0,0,4,34); //여마법사
	chr[20].SetChr(0,0,1,1,0,0,3,2,4,34); //용사
	chr[21].SetChr(0,3,2,35,3,3,0,0,4,19); //천계의 장수
	chr[22].SetChr(0,36,0,0,0,0,0,0,0,0); //단검
	
	SetRect(&CRect, MAX_LEFT, 0, 799, BASE-1);
}

void CBattle::Clear()
{
	for(int i=0; i<SMAX; i++)
		spr[i].life=false;
	for(i=0; i<SNDMAX; i++)
		play[i]=-1;
	spr[SMAX].NewSpr(500, 150, 5, 10, 100, 100, 1, 0, 99, 10);
	jump=JUMPMAX;

	drag=0; big=0;
	tolk=-1; tdelay=0;
}

void CBattle::Change()
{
	for(int i=0; i<SMAX; i++)
		spr[i].life=false;
	for(i=0; i<4; i++)
		if(etc[i]>=-101)
		{
			int happy;
			if(etc[i]>=0)happy=10;				//행복한 주민
				else if(etc[i]>-100)happy=8;	//불만 주민
				else if(etc[i]==-100)happy=14;	//투쟁하는 주민
				else happy=16;					//분노하는 주민
			spr[i+1].NewSpr(i*80+480+rand()%50, 176, happy+((etc[6]+i)%2), 1, 1, 0, 0, 0, 1, 10);
			spr[i+1].dir=((rand()%2)==0)?true:false;
		}
	if(etc[4]>=0)spr[0].NewSpr(0, 150, 18+etc[4], 0, 50, 0, 1, 0, 3, 10);	//용사 등장
}

void CBattle::CtrSpr()
{
	int tmp, df, j;
	for(int i=0; i<=SMAX; i++)
	{
		if(i==SMAX) //마왕탄생에서만 사용. 관찰 창 만들기
		{
			SetR(&MRect,0,0,320,300);
			_DrawBmp(MRect, 480, 0, BmpScreen[2], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		}

		if(!spr[i].life)continue;			//죽은 경우 넘기기
		for(int i2=0; i2<2; i2++)	//상태이상
			if(spr[i].odd[i2]>0)
			{
				spr[i].odd[i2]--;
				if(i2==1 && (spr[i].odd[i2]%50)==0)spr[i].hp--;
			}

		if(spr[i].hp<=0 && spr[i].now!=4)	//사망판정
		{
			if(i==0)etc[4]+=4;
				else if(i<5)etc[i-1]=-111;
			spr[i].now=4;
			spr[i].temp=0;
			spr[i].tempo=5;
			spr[i].delay=50;
			if(i>5)AddSND(0);
				else
				{
					if(spr[i].chr==8 || spr[i].chr==10 || spr[i].chr==14)
						AddSND(2);
					else if(spr[i].chr==18 || spr[i].chr==20 || spr[i].chr==21)
						AddSND(19);
					else
						AddSND(10);
				}
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
		
		if(spr[i].y+ani[tmp].yy*ani[tmp].x2>CRect.bottom+1)spr[i].y-=2;
		else if(spr[i].hp>0)switch(spr[i].kind)
		{
			case 1:  if(!normal(i) || spr[i].odd[0]>0)break;
					 walk(i,true);
					 if(spr[i].x<=MAX_LEFT)spr[i].dir=false;
					 if(spr[i].x+ani[tmp].xx*ani[tmp].x2>=800)spr[i].dir=true;
					 break;
			case 11: if(spr[i].x<=MAX_LEFT)spr[i].dir=false;
					 if(spr[i].x+ani[tmp].xx*ani[tmp].x2>=800)spr[i].dir=true;
					 if(!normal(i))break;
					 walk(i,true);
					 if(spr[i].temp>=200)
					 {
						 j=SprNo(HERO_SPR,SMAX);
						 if(j!=-1)
						 {
							 if(spr[i].chr==1)spr[j].NewSpr(spr[i].x+40+(-40*spr[i].dir),spr[i].y+45,13,22,200,0,1,0,5,5);
							 AddSND(5);
						 }
						 spr[i].temp=0; spr[j].dir=spr[i].dir;
					 }
					 break;
			case 12: if(spr[i].x<=MAX_LEFT)spr[i].dir=false;
					 if(spr[i].x+ani[tmp].xx*ani[tmp].x2>=800)spr[i].dir=true;
					 walk(i,true);
					 if(normal(i)){
						spr[i].delay=90;
						spr[i].now=6; spr[i].temp=0;
						AddSND(7);
					 }
					break;
			case 13: if(!normal(i))break;
					 walk(i,true);
					 if(spr[i].x<=MAX_LEFT)spr[i].dir=false;
					 if(spr[i].x+ani[tmp].xx*ani[tmp].x2>=800)spr[i].dir=true;
					 break;
			case 14: if(!normal(i) || !spr[0].life)break;
					 if(spr[i].x<spr[0].x)spr[i].dir=false;
						else spr[i].dir=true;
					 df=spr[0].x-spr[i].x;
					 if(df<0)df=-(df)+40;
					 /*if(spr[0].now==1 && spr[0].dir!=spr[i].dir)
					 {
			 			spr[i].now=2; spr[i].temp=0;
						spr[i].delay=30;
					 }
					 else*/ if(df<70)
					 {
			 			spr[i].now=1; spr[i].temp=0;
						spr[i].delay=spr[i].tempo*2;
						AddSND(6);
					 }
					 walk(i,true);
					 break;
			case 21: spr[i].hp--;
					 if(spr[i].hp<=0)spr[i].life=false;
					 break;
			case 22: case 23:
					 spr[i].hp--; walk(i,true);
					 if(spr[i].hp<=0 || spr[i].x+ani[tmp].xx<MAX_LEFT || spr[i].x>800)spr[i].life=false;
					 break;
		}
		//특수행동
		if(spr[i].now==PANIC && i!=SMAX)walk(i,false);				//맞은 경우 뒷걸음질
		if(spr[i].now==DIE && spr[i].delay<=0)spr[i].life=false;	//죽은 경우
		if(spr[i].now==JUMP)spr[i].y+=((spr[i].temp-45)/6);			//점프인 경우
		//이동제한
		if(i==0)
		{
			if(spr[i].x<479)spr[i].x=479;
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
		
		//충돌 체크
		if(i<HERO_SPR)		//용사측 공격
		{
			for(j=HERO_SPR; j<=SMAX; j++)
				if(!spr[j].life)continue;
				else Bump(i,j);
		}
		else if(i<SMAX)				//마왕측 공격
		{
			int limit=HERO_SPR;
			if(spr[i].chr==4)limit=1;
			for(j=0; j<limit; j++)
				if(!spr[j].life)continue;
				else Bump(i,j);
		}
		else	//마왕손
		{
			if(LeftButton)for(j=1; j<5; j++)
				if(spr[j].life && spr[j].now!=4 && etc[11]<=0 && Crush(j))
				{
					if(spr[j].chr==8 || spr[j].chr==10 || spr[j].chr==14)	//남자먹기
					{
						spr[i].hp=Min(spr[i].hp+1,100);
						spr[j].hp=0;
						AddSND(9);
						SetTolk(12);
					}
					else if(spr[j].chr==9 || spr[j].chr==11 || spr[j].chr==15)	//여자먹기
					{
						etc[9]+=1;
						spr[j].hp=0;
						AddSND(9);
						SetTolk(12);
					}
					else	//괴물로 바꾸기
					{
						spr[j].life=false;
						etc[j-1]=-111;
						int No=SprNo(HERO_SPR,SMAX);
						if(No!=-1)spr[No].NewSpr(spr[j].x,spr[j].y,etc[10],10+etc[10],1,0,1,0,1,10);
						AddSND(11);
						SetTolk(13);
					}
				}
		}

		if(etc[11]>0 && i<SMAX)continue;	//노이즈

		//마비체크
		if(spr[i].odd[0]>0)
			ani[tmp].PutAni(spr[i].x, spr[i].y, 0, spr[i].dir, i);
		else
			ani[tmp].PutAni(spr[i].x, spr[i].y, spr[i].temp/spr[i].tempo, spr[i].dir, i);
		//이상표시
		for(i2=0; i2<2; i2++)
		{
			if(spr[i].odd[i2]>0)	//마비
			{
				SetR(&MRect, 135+i2*20, 140, 20, 20);
				if(spr[i].x>(480-i2*20))_DrawBmp(MRect, spr[i].x+i2*20, spr[i].y, BmpScreen[BTLBMP], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
		}
	}
}

void CBattle::Key()
{
	int No, i;
//마왕
	if(RightButton && spr[SMAX].life)
	{
		if(drag<124)drag++;
		if(!(drag%25) && drag)AddSND(16);
		if(LeftButton)drag=0;
	}
	else if(drag>=25)
	{
		if(etc[8]==0)	//소환마법
		{
			No=SprNo(HERO_SPR,SMAX);
			if(No!=-1)
			{
				int vmp=drag/25;
				if(spr[SMAX].mp>=vmp)
				{
					spr[SMAX].mp-=vmp;
					SetTolk(vmp-1);
					switch(vmp)
					{
						case 1:	spr[No].NewSpr(spr[SMAX].x,240,1,11,etc[10],0,1,0,1,10); AddSND(14); break;
						case 2:	spr[No].NewSpr(spr[SMAX].x,240,2,12,1,0,2+etc[10],0,1,10); AddSND(14); break;
						case 3:	spr[No].NewSpr(spr[SMAX].x,240,3,13,4+etc[10],0,1,0,2,10); AddSND(14); break;
						case 4:	spr[No].NewSpr(spr[SMAX].x,240,4,14,5+etc[10]*5,0,2,0,3,10); AddSND(14); break;
					}
				}
				else			//마력 부족
				{
					AddSND(15);
					SetTolk(14);
				}
			}
		}
		else if(etc[8]==1)	//공격마법
		{
			int vmp=(drag/25)*5;
			if(spr[SMAX].mp>=vmp)
			{
				spr[SMAX].mp-=vmp;
				SetTolk((vmp/5)+3);
				switch(vmp)
				{
					case 5:
						No=SprNo(HERO_SPR,SMAX);
						if(No!=-1)
						{
							spr[No].NewSpr(spr[SMAX].x,spr[SMAX].y,6,21,20*etc[10],0,1,0,1,3);
							AddSND(13);
						}
						break;
					case 10: for(i=0; i<5; i++)spr[i].odd[0]=100*etc[10]; AddSND(3); break;
					case 15: for(i=0; i<5; i++)spr[i].odd[1]=500*etc[10]; AddSND(5); break;
					case 20: 
						for(i=0;i<4;i++)
						{
							if(spr[i+1].life && spr[i+1].now!=4)
							{
								spr[i+1].life=false;
								etc[i]=-111;
								No=SprNo(HERO_SPR,SMAX);
								if(No!=-1)
								{
									if(etc[10]==1)
										spr[No].NewSpr(spr[i+1].x,spr[i+1].y,1,11,1,0,1,0,1,10);
									else if(etc[10]==2)
										spr[No].NewSpr(spr[i+1].x,spr[i+1].y,2,12,1,0,4,0,1,10);
									else
										spr[No].NewSpr(spr[i+1].x,spr[i+1].y,3,13,15,0,1,0,1,10);
								}
							}
						}
						AddSND(11);
						break;
				}
			}
			else			//마력 부족
			{
				AddSND(15);
				SetTolk(14);
			}
		}
		else if(etc[8]==2)	//건설마법
		{
			etc[12]=drag/25-1;
		}
		drag=0;
	}
	else drag=0;

	//컴퓨터
	if(spr[0].hp<=0 || spr[0].odd[0]>0)return;		

	if(1)//com)
	{
		bool jum=false;
		if(etc[4]==0)
		{
			if(spr[0].dir)spr[0].dir=false;
			jum=true;
			if(spr[0].temp>20)
			{
				int no=SprNo(5,HERO_SPR);
				if(no>=0)
				{
					AddSND(6);
					spr[no].NewSpr(spr[0].x+5,spr[0].y+10,22,23,200,0,spr[0].att,0,5,5);
					spr[no].dir=spr[0].dir;
				}
				spr[0].temp=0;
			}
		}
		else if(etc[4]==1)
		{
			walk(0,true);
			if(spr[0].x<=MAX_LEFT)spr[0].dir=false;
			int tmp=State(0);
			if(spr[0].x+ani[tmp].xx>=800)spr[0].dir=true;
			if(spr[0].temp>100)
			{
				Hero(6);
				spr[0].temp=0;
			}
			else if(spr[0].temp==50)Hero(5);
		}
		else if(etc[4]==2)
		{
			//정황검사
			int mon=0, spot, Obeet=999, target=-1, it;
			for(i=HERO_SPR;i<SMAX; i++)
			{
				if(spr[i].life)
				{
					int beet=999;
					if(spr[i].chr==13)	//물방울일 경우 점프
					{
						if(spr[i].dir && spr[i].x<spr[0].x+200)jum=true;
							else if(!spr[i].dir && spr[i].x>spr[0].x+100)jum=true;
					}
					else mon++;	//아닐 경우 몬스터 수 증가

					it=ani[chr[spr[i].chr].type[0]].xx*ani[chr[spr[i].chr].type[0]].x2;
					if(spr[0].x+30<spr[i].x)beet=spr[i].x-spr[0].x-100;
						else if(spr[0].x>spr[i].x+it)beet=spr[0].x-spr[i].x-it;
					if(beet<Obeet)
					{
						Obeet=beet;
						target=i;
					}
				}
			}
			if(target==-1 && normal(SMAX) && spr[SMAX].x>480 && spr[SMAX].y<300)target=SMAX;
			if(spr[0].x<200)spot=0;
				else if(spr[0].x<400)spot=1;
				else spot=2;
			//행동
			if(target>=0)
			{
				if(spr[0].x+30<spr[target].x)Hero(0);
					else if(spr[0].x>spr[target].x+it-20)Hero(1);
				if(spr[target].y<80)jum=true;
			}
			//마법
			if(spr[0].temp>200 && spr[0].hp<50){Hero(7); spr[0].temp=0;}
				else if(normal(0) && target>=0)Hero(2);
		}
		else if(etc[4]==3)
		{
			jum=true;
			walk(0,true);
			if(spr[0].x<=MAX_LEFT)spr[0].dir=false;
			int tmp=State(0);
			if(spr[0].x+ani[tmp].xx>=800)spr[0].dir=true;
			if(spr[0].now==PANIC)Hero(8);
		}

		if(jum&&jump>0&&!keyUP)Hero(3);
		else if((spr[0].y+ani[chr[spr[0].kind].pic[0]].yy)<BASE)Hero(4);

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
		case 0:		//우로 이동
			spr[0].dir=false;
			spr[0].x+=spr[0].speed;
			break;
		case 1:		//좌로 이동
			spr[0].dir=true;
			spr[0].x-=spr[0].speed;
			break;
		case 2:		
			spr[0].now=1; spr[0].temp=0;
			spr[0].delay=spr[0].tempo*3;
			AddSND(8);
			keyET=true;
			break;
		case 3:
			spr[0].y-=(jump/(JUMPMAX/10));
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
			spr[5].NewSpr(spr[0].x+5,spr[0].y+10,12,22,200,0,spr[0].att,0,5,5);
			spr[5].dir=spr[0].dir; spr[0].mp-=5; AddSND(0);
			break;
		case 6:
			spr[5].NewSpr(spr[0].x-60,50,7,21,100,0,spr[0].att,0,1,5);
			spr[0].mp-=10; AddSND(13);
			break;
		case 7:
			spr[0].hp=Min(spr[0].hp+50, 100); spr[0].mp-=15;
			AddSND(1);
			break;
		case 8:
			spr[0].now=2; spr[0].temp=0;
			spr[0].delay=10; spr[0].mp-=20;
			AddSND(0);
			break;
	}
}

int CBattle::SprNo(int from, int to)
{
	for(int i=from;i<to;i++)
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
		spr[j].delay=10;
		spr[j].hp-=Max(spr[i].att-spr[j].def/2,1);
		spr[j].hp=Max(spr[j].hp,0);
		AddSND(4);
		if(spr[i].kind==23)spr[i].life=false; //소모용
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

void CBattle::SetTolk(int what, int del)
{
	tolk=what; tdelay=del;
}

void CBattle::Battle(int ctrl)
{
	if(ctrl==0)
	{
		//마법 종류
		if(MouseX<440)etc[8]=2;				//건설마법
			else if(MouseY<300)etc[8]=1;	//공격마법
			else etc[8]=0;					//소환마법
		if(etc[8]!=etc[7])	//다른 마법영역으로 옮겼을 때
		{
			etc[7]=etc[8];
			drag=0;
		}
		tdelay=Max(tdelay-1,0);
		//마법 사용
		if(drag>0)
		{
			SetR(&MRect,60,325,180,100);
			_DrawBmp(MRect, 603, 349, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			SetR(&MRect,drag/25*110,etc[8]*80,110,80);
			_DrawBmp(MRect, 634, 353, BmpScreen[4], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		}
		//대화
		else if(tdelay>0)
		{
			SetR(&MRect,60,325,180,100);
			_DrawBmp(MRect, 603, 349, BmpScreen[3], DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			_LockScreen();
			if(tolk==0)
			{
				PutFont(623,379,BLACK,"나와랏! 나의 부하,");
				PutFont(623,399,BLACK,"     <방갈>");
			}
			else if(tolk==1)
			{
				PutFont(623,379,BLACK,"나와랏! 나의 부하,");
				PutFont(623,399,BLACK,"   <파마배추>");
			}
			else if(tolk==2)
			{
				PutFont(623,379,BLACK,"나와랏! 나의 부하,");
				PutFont(623,399,BLACK,"     <큰바위>");
			}
			else if(tolk==3)
			{
				PutFont(623,379,BLACK,"나와랏! 나의 부하,");
				PutFont(623,399,BLACK,"    <암흑기사>");
			}
			else if(tolk==4)
			{
				PutFont(628,379,BLACK,"폭열성 마법!");
				PutFont(628,399,BLACK,"  <콰쿠우>");
			}
			else if(tolk==5)
			{
				PutFont(628,379,BLACK,"신경성 마법");
				PutFont(628,399,BLACK,"  <찌리>");
			}
			else if(tolk==6)
			{
				PutFont(628,379,BLACK,"독속성 마법");
				PutFont(628,399,BLACK,"  <커어>");
			}
			else if(tolk==7)
			{
				PutFont(623,379,BLACK,"주민들이여!");
				PutFont(623,399,BLACK,"잠든 본성에 눈떠라!");
			}
			else if(tolk==12)
			{
				PutFont(623,379,BLACK,"음, 우걱우걱우걱...");
				PutFont(623,399,BLACK,"맛있군! 역시!");
			}
			else if(tolk==13)
			{
				PutFont(618,379,BLACK,"너에게 힘을 주겠다!");
				PutFont(618,399,BLACK,"너의 분노를 펼쳐라!");
			}
			else if(tolk==14)
			{
				PutFont(628,379,BLACK,"젠장");
				PutFont(628,399,BLACK,"마력이 부족해!");
			}
			_UnlockScreen();
		}
		//처리되는 작업들
		spr[SMAX].x=MouseX; spr[SMAX].y=MouseY;
		if(etc[5]<999)
		{
			//주민의 변화
			for(int i=0; i<4; i++)
			{
				if(etc[i]>=-101)
				{
					int happy;
					if(etc[i]>=0)happy=10;				//행복한 주민
						else if(etc[i]>-100)happy=8;	//불만 주민
						else if(etc[i]==-100)happy=14;	//투쟁하는 주민
						else happy=16;					//분노하는 주민
					happy+=(etc[6]+i)%2;
					//새주민 탄생
					if(!spr[i+1].life)
						spr[i+1].NewSpr(450-i*30, 176, happy, 1, 1, 0, 0, 0, 1, 10);
				}
			}
			if(etc[4]>=0 && !spr[0].life)spr[0].NewSpr(450, 176, 18+etc[4], 0, 50, 0, 1, 0, 3, 10);
			
			Key();
		}
		else if(drag>0)drag=0; //아닌 경우 마법 사용금지
	
		CtrSpr();
	}
	else if(ctrl==1)
	{
		Clear();
	}
	else if(ctrl==2)
	{
		Change();
	}
}