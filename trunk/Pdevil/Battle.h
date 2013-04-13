// Battle.h: interface for the CBattle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BATTLE_H__F753A8AF_F6D9_4DE7_A36D_F6021E15959C__INCLUDED_)
#define AFX_BATTLE_H__F753A8AF_F6D9_4DE7_A36D_F6021E15959C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SMAX 50
#define JUMPMAX 40
#define SNDMAX 3
#define BASE 300
#define SetR(lprc, left, top, right, bottom) SetRect(lprc, left, top, left+right, top+bottom);

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
	void PutAni(int px, int py, int temp, bool aid);
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
	int chr, kind, temp, tempo;
	int x, y;
	int hp, mp, att, def, speed;
	bool dir;
	int now, odd, delay;

	bool life;
	void NewSpr(int vx, int vy, int c, int k, int h, int m, int a, int d, int s, int te, int nw);
};

class CBattle  
{
	CAni ani[35];
	CChr chr[15];
	CSpr spr[SMAX+1];

	int scroll, menu, drag, drop, stage;
	int jump, frz;
	int big;
	int play[SNDMAX+1];
	int tolk;

	void PutBack();
	void PutFront();
	void CtrSpr();
	void Key();
	int Crush(int tg);
	int SprNo();
	int Max(int x, int y);
	int Min(int x, int y);
	int State(int no);
	void Bump(int i, int j);
	bool normal(int i);
	void LevelUp();
	void walk(int i, bool front);
	void AddSND(int no);
	void Talk(int caracter, char* sen1, char* sen2, char* sen3);
	void SetTolk(int what, int del=200);
	void Hero(int doing);
public:
	int type[2];
	int lv, exp, next, win, tdelay;
	bool com;
	CBattle();
	void Clear();
	int* Battle();
};

#endif // !defined(AFX_BATTLE_H__F753A8AF_F6D9_4DE7_A36D_F6021E15959C__INCLUDED_)
