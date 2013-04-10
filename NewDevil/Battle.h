// Battle.h: interface for the CBattle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BATTLE_H__F753A8AF_F6D9_4DE7_A36D_F6021E15959C__INCLUDED_)
#define AFX_BATTLE_H__F753A8AF_F6D9_4DE7_A36D_F6021E15959C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SMAX		60		//���� ��������Ʈ �Ѱ�
#define HERO_SPR	10		//����� ��������Ʈ ��
#define JUMPMAX		20		//�����Ѱ�
#define SNDMAX		3		//���� ��� ���� ��
#define BASE		240		//����
#define BTLBMP		3		//��¿� �� �׸� ��ȣ
#define MAX_LEFT	480		//â�� ���� ���
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
	int chr, kind, temp, tempo;		//ĳ���� �׸�, ĳ���� ����, ����, ������ ��ȭ��(���� ���� �ʰ� ����)
	int x, y;						//��ġ
	int hp, mp, att, def, speed;	//ü��, ����, ��, ����, ������
	bool dir;						//����
	int now, odd[3], delay;			//���ݻ���, �����̻�, ���ݻ��¸� �����ϴ� �ð�
	int etc[3];						//�� ĳ���͸� ������ ������ ����

	bool life;						//���翩��
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
	int jump;		//���ΰ��� ����
	int drag;		//������ ������ ����
	int big;		//�Ŵ�ȭ ����
	int tolk;		//��ȭ ����
	int tdelay;
    int etc[13];	//��Ÿ �⹫�� ���� ���� ����(���Ӹ��� �뵵�� Ʋ���� �� �ִ� ����)
	bool com;

	CBattle();
	void Battle(int ctrl); //��Ʈ�� ����: �ܺο��� ����� ������ ���ؼ� ����
};

#endif // !defined(AFX_BATTLE_H__F753A8AF_F6D9_4DE7_A36D_F6021E15959C__INCLUDED_)

/*-------------------------------------
etc ���� ��볻��
0~3: �ֹ��� �ູ��
4: ������ ����� ��ȣ
5: �ֹε��� �� �� �ִ� ������ ����
6: ������ ���� ���� ����
7,8: ��������
9: ���� ����� ȿ��
10: ������ ���
11: ������
12: �Ǽ�����
-------------------------------------*/