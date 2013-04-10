// Battle.h: interface for the CBattle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BATTLE_H__F753A8AF_F6D9_4DE7_A36D_F6021E15959C__INCLUDED_)
#define AFX_BATTLE_H__F753A8AF_F6D9_4DE7_A36D_F6021E15959C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SMAX		60		//출현 스프라이트 한계
#define HERO_SPR	10		//용사측 스프라이트 수
#define JUMPMAX		20		//점프한계
#define SNDMAX		3		//동시 재생 사운드 수
#define BASE		240		//지평선
#define BTLBMP		3		//출력에 쓸 그림 번호
#define MAX_LEFT	480		//창의 왼쪽 경계
#define SetR(lprc, left, top, right, bottom) SetRect(lprc, left, top, left+right, top+bottom)

class CAni
{
public:
	int frame;
	bool loop;
	int x, y, xx, yy;
	int ax, ay, axx, ayy;
	int dx, dy, dxx, dyy;
	int x2;

	void SetAni(int sframe, bool sloop, int sx, int sy, int sxx, int syy,
		int sax, int say, int saxx, int sayy, int sdx, int sdy, int sdxx, int sdyy, int x3=1);
	void PutAni(int px, int py, int temp, bool aid, int special);
};

class CChr
{
public:
	int type[5], pic[5];
	int patt;

	void SetChr(int t1, int p1, int t2, int p2, int t3, int p3, int t4, int p4, int t5, int p5);
};

class CSpr
{
public:
	int chr, kind, temp, tempo;		//캐릭터 그림, 캐릭터 종류, 템포, 템포의 변화도(높을 수록 늦게 변함)
	int x, y;						//위치
	int hp, mp, att, def, speed;	//체력, 마력, 힘, 맷집, 빠르기
	bool dir;						//방향
	int now, odd[3], delay;			//지금상태, 상태이상, 지금상태를 유지하는 시간
	int etc[3];						//그 캐릭터만 가지는 고유의 변수

	bool life;						//존재여부
	void NewSpr(int vx, int vy, int vchr, int vkind, int vhp, int vmp,
				  int vatt, int vdef, int vspeed, int vtempo, int vnow=0);
};

class CBattle  
{
	CAni ani[40];
	CChr chr[30];

	int Crush(int tg);
	int SprNo(int from, int to);
	int State(int no);
	void Bump(int i, int j);
	bool normal(int i);
	void walk(int i, bool front);
	void AddSND(int no);
	void SetTolk(int what, int del=100);
	void Hero(int doing);
	void CtrSpr();
	void Key();
	void Clear();
	void Change();
public:
	CSpr spr[SMAX+1];
	int play[SNDMAX+1];
	int jump;		//주인공의 점프
	int drag;		//마법력 모으는 변수
	int big;		//거대화 변수
	int tolk;		//대화 변수
	int tdelay;
    int etc[13];	//기타 잡무에 쓰기 위한 변수(게임마다 용도가 틀려질 수 있는 변수)
	bool com;

	CBattle();
	void Battle(int ctrl); //컨트롤 변수: 외부에서 명령을 내리기 위해서 존재
};

#endif // !defined(AFX_BATTLE_H__F753A8AF_F6D9_4DE7_A36D_F6021E15959C__INCLUDED_)

/*-------------------------------------
etc 변수 사용내역
0~3: 주민의 행복도
4: 출현한 용사의 번호
5: 주민들이 살 수 있는 땅인지 여부
6: 성별을 섞기 위한 변수
7,8: 마법종류
9: 먹은 사람의 효과
10: 마왕의 등급
11: 노이즈
12: 건설마법
-------------------------------------*/