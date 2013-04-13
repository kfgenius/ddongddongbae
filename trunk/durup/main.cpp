#include <dsound.h>

#include "JDirectDraw.h"
#include "Dsutil.h"
#include "JResourceManager.h"
#include "resource.h"
//#include "extern.h"

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <crtdbg.h>
#include <gdiplus.h>

using namespace Gdiplus;

JDirectDraw* jdd;
JResourceManager* jre;

char* backbuffer;
JFont font20, font12;
MSG msg;

LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam);

#include "data.h"

//////////////////////////////////////////////////////////////////
//�⺻ ����

#define SCREEN_X	640
#define SCREEN_Y	480
#define CLIP_Y		440

void CHugeDlg::Function(int command)
{
}

CAnimation ani_worm(25, 9, EXCHANGE_TYPE_ANI, 3, 5);
CAnimation ani_fly(14, 12, EXCHANGE_TYPE_ANI, 3, 10);
CAnimation ani_boom(40, 40, EXCHANGE_TYPE_ANI, 3, 2);
CAnimation ani_gas(60, 60, EXCHANGE_TYPE_ANI, 3, 20);
CAnimation ani_parasite(12, 4, EXCHANGE_TYPE_ANI, 3, 5);
CAnimation ani_fire(22, 55, LOOP_TYPE_ANI, 3, 5);
CAnimation ani_cell(23, 23, EXCHANGE_TYPE_ANI, 3, 20);
CAnimation ani_robot(49, 56, EXCHANGE_TYPE_ANI, 3, 10);
CAnimation ani_missile(12, 5, EXCHANGE_TYPE_ANI, 3, 5);
CAnimation ani_jelly(154, 122, EXCHANGE_TYPE_ANI, 3, 10);
CAnimation ani_shield(6, 30, EXCHANGE_TYPE_ANI, 3, 5);
CAnimation ani_wakeup(360, 190);
CAnimation ani_weapon_boom1(5, 5, LOOP_TYPE_ANI, 3, 5);
CAnimation ani_weapon_boom3(20, 181, LOOP_TYPE_ANI, 3, 5);
CAnimation ani_weapon_boom4(55, 52, EXCHANGE_TYPE_ANI, 3, 2);
CAnimation ani_thunder(78, 19, EXCHANGE_TYPE_ANI, 3, 5);
CAnimation ani_mainmenu(200, 45);
CAnimation ani_title(393, 53);

//���� ���� ����, �Լ��� ������Ÿ��

int difficult;	//���̵�
int boss_count;		//������ ���� ��
int boss_hp;		//������ ü��
int boss_tempo;	//���� ������

int GetAbleID(bool life, int start, int end);
int CreateUnit(int type, int x, int y, char* name, double dir, bool boss=false);
void Boom(int id);

#define UNIT_ENEMY		1
#define UNIT_WEAPON		2
#define UNIT_ANI		3

#define SHOT_FROM		-100
#define CENTER_FROM		-200

//////////////////////////////////////////////////////////////////
//ĳ���� Ŭ���� : ���� ĳ������ ����Ǵ� ����
#define CARACTER_MAX 60

class CCaracter
{
public:
	char* name;			//�׸� �̸�
	CAnimation* ani;	//�ִϸ��̼�

	int hp_max;			//ü��
	int width;			//����
	int height;			//����
	int cx, cy;			//�߽� ��ǥ
	int pattern;		//�ൿ ����
	int power;			//���ݷ�
	double speed;		//�ӵ�

	void SetCaracter(char* vname, int vhp_max, int vpattern, int vpower, double vspeed, int center_x, int center_y, CAnimation* vani=NULL);
	~CCaracter();
};

void CCaracter::SetCaracter(char* vname, int vhp_max, int vpattern, int vpower, double vspeed, int center_x, int center_y, CAnimation* vani)
{
	//�̸� ����
	if(name!=NULL)
	{
		delete[] name;
		name=NULL;
	}

	name=new char[strlen(vname)+1];
	strcpy(name, vname);
	name[strlen(vname)]=NULL;

	//ũ�� �˾Ƴ���
	JPictureInfo jpi;
	jdd->GetPictureInfo(name, &jpi);
	if(vani==NULL)width=jpi.GetWidth();
		else width=vani->GetXSize();
	height=jpi.GetHeight();

	//�Ӽ� ����
	hp_max=vhp_max;
	pattern=vpattern;
	power=vpower;
	speed=vspeed;
	cx=center_x;
	cy=center_y;

	ani=vani;
}

CCaracter::~CCaracter()
{
	if(name!=NULL)
	{
		delete[] name;
		name=NULL;
	}
}

CCaracter cara[CARACTER_MAX];

//////////////////////////////////////////////////////
//���� Ŭ����
#define UNIT_MAX	200
#define WEAPON_MAX	400
#define ANI_MAX		800

class CUnit
{
public:
	//�⺻ ����
	bool life;
	bool first;	//ó���� ���ؾ� �� �׼��� ���� ���� ����ġ

	CCaracter* caracter;
	CAnimation ani;
	bool animation;

	int my_id;
	int caracter_id;
	int x,y;
	int sx, sy;
	int delay;
	double rx, ry;
	int hp;
	double dir;
	int tempo;
	
	//�������� ����
	bool boss;

	bool SetUnit(int vx, int vy, char* caracter_name, double vdir);
	void SetXY(int vx, int vy);
	void Move(double dx, double dy);
	void DrawUnit();
	void Action();
	bool Damage(int damage);
};

CUnit unit[ANI_MAX];

bool CUnit::SetUnit(int vx, int vy, char* caracter_name, double vdir)
{	
	for(int i=0; i<CARACTER_MAX; i++)
		if(cara[i].name!=NULL && strcmp(cara[i].name, caracter_name)==0)
		{
			life=true;
			first=true;

			//�⺻ ���� ����
			SetXY(vx, vy);
			sx=sy=delay=0;
			dir=vdir;
			tempo=0;
			
			//ĳ���� ���� �Ӽ� �ޱ�
			caracter=&cara[i];
			caracter_id=i;
			hp=caracter->hp_max;
			boss=false;
			if((caracter->ani)==NULL)
				animation=false;
			else
			{
				animation=true;
				memcpy(&ani, caracter->ani, sizeof(CAnimation));
			}

			return true;
		}

	return false;
}

void CUnit::SetXY(int vx, int vy)
{
	x=vx;
	y=vy;
	rx=(double)x;
	ry=(double)y;
}

void CUnit::Move(double dx, double dy)
{
	rx+=dx;
	ry+=dy;
	x=(int)rx;
	y=(int)ry;

	if(my_id==0)
	{
		if(x<0)SetXY(0, y);
			else if(x>SCREEN_X-(caracter->width))SetXY(SCREEN_X-(caracter->width), y);
		if(y<0)SetXY(x, 0);
			else if(y>CLIP_Y-(caracter->height))SetXY(x, CLIP_Y-(caracter->height));
	}
	else if(!boss && x <= -(caracter->width) || x > SCREEN_X || y<=-(caracter->height) || y > CLIP_Y)life=false;
}

void CUnit::DrawUnit()
{
	if(life)
	{
		if(!animation)
		{
			if(my_id==0)jdd->DrawPicture(backbuffer, "SkullBody", x-159, y-56, NULL);	//���ΰ�
			DrawPicture(caracter->name, x+sx, y+sy, 0);
			if(caracter_id==10)jdd->DrawText(backbuffer, StrAdd("d", hp), font20, x+sx+5, y+sy+5, JColor(0,0,0));	//256
		}
		else DrawPicture(caracter->name, x+sx, y+sy, -1, &ani);
	}	
}

bool CUnit::Damage(int damage)
{
	if(life)
	{
		hp-=damage;
		if(boss)boss_hp-=damage;
		//���� ȿ��
		if(caracter->pattern!=100 && caracter->pattern!=999)delay=10;
		//���
		if(hp<=0)
		{
			life=false;
			if(boss)boss_count++;

			return true;
		}
		else return false;
	}
	else return false;
}

void CUnit::Action()
{
	//���� ������ �ൿ
	if(life)
	{
		//�ִϸ��̼� ó��
		if(animation)ani.Process();

		//����
		if(delay>0)
		{
			delay--;
			sx=rand()%3-1;
			sy=rand()%3-1;
		}
		else sx=sy=0;

		//�ൿ
		int pattern=caracter->pattern;
		//��������
		if(pattern==1)
		{
			Move(-(caracter->speed), (rand()%3)-1);
		}
		//ȭ�� ��
		else if(pattern==2)
		{
			tempo++;
			//����
			if(first)
			{
				if(tempo<15)Move(-(caracter->speed), 0);
					else first=false;
			}			
			else
			{
				//�̵�
				Move(-(caracter->speed), (tempo-20)/4);

				//����
				if(tempo>=40)
				{
					CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MThunder", 180);
					tempo=0;
				}
			}
		}
		//������ ��
		else if(pattern==3)
		{
			tempo++;
			//����
			if(first)
			{
				if(tempo<50)Move(-(caracter->speed), 0);
					else first=false;
			}
			//����
			else
			{				
				if(tempo>=360)tempo=0;
				if((tempo%(18-(difficult*3)))==0)CreateUnit(UNIT_ENEMY, CENTER_FROM, my_id, "MLight", tempo);
			}
		}
		//�ĸ�
		else if(pattern==4)
		{
			tempo++;
			//����
			if(first)
			{
				if(tempo<25)Move(-(caracter->speed), 0);
					else
					{
						first=false;
						tempo=0;
					}
			}
			//������&����
			else
			{
				if(tempo>=160)tempo=0;
				int act_no=tempo/20;
				if(act_no==0)Move(-(caracter->speed), -(caracter->speed));
					else if(act_no==1)Move((caracter->speed), (caracter->speed));
					else if(act_no==2)Move((caracter->speed), -(caracter->speed));
					else if(act_no==3)Move(-(caracter->speed), (caracter->speed));
					else if(act_no==4)Move((caracter->speed), (caracter->speed));
					else if(act_no==5)Move(-(caracter->speed), -(caracter->speed));
					else if(act_no==6)Move(-(caracter->speed), (caracter->speed));
					else if(act_no==7)Move((caracter->speed), -(caracter->speed));

				if((tempo%(30-(difficult*5)))==0)CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MDot", 180);
			}
		}
		//��
		else if(pattern==5)
		{
			tempo++;
			
			//����
			if(first)
			{
				if(tempo<caracter->width)Move(-1, 0);
					else first=false;
			}
			//����
			else
			{
				if((tempo%30)==0)CreateUnit(UNIT_ENEMY, CENTER_FROM, my_id, "Worm", 180);
				if(tempo>=120)
				{
					tempo=0;
					CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MGas", 180+(rand()%45));
				}
			}
		}
		//��
		else if(pattern==6)
		{
			tempo++;
			
			//�̵�
			if(tempo<30)Move(-(caracter->speed), -2);
				else if(tempo<40)Move(-(caracter->speed), -1);
				else if(tempo<50)Move(-(caracter->speed), 1);
				else if(tempo<110)Move(-(caracter->speed), 2);
				else if(tempo<120)Move(-(caracter->speed), 1);
				else if(tempo<130)Move(-(caracter->speed), -1);
				else if(tempo<160)Move(-(caracter->speed), -2);
				else tempo=0;
		}
		//����Ű�� ��
		else if(pattern==7)
		{
			tempo++;
			
			//����
			if(first)
			{
				if(tempo<15)Move(-(caracter->speed), 0);
					else first=false;
			}
			//����
			else
			{
				if(tempo>=20)
				{
					tempo=0;
					CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MEgg", 180);
				}

				if((rand()%3)==0)
				{
					if((unit[0].caracter->cy)+unit[0].y<y)Move(0,-(caracter->speed));
						else Move(0, caracter->speed);
				}
			}
		}
		//�ָ�, ��
		else if(pattern==8 || pattern==9)
		{
			tempo++;
			
			//����
			if(first)
			{
				if(tempo>=100-(15*difficult))first=false;
			}
			//�̵�
			else Move(cos(dir*0.017453)*(caracter->speed), sin(dir*0.017453)*(caracter->speed));
		}
		//256
		else if(pattern==10)
		{
			tempo++;
			
			//����
			if(first)
			{
				if(tempo<30)Move(-(caracter->speed), 0);
					else first=false;
			}
			//����
			else
			{
				if(tempo>=5)
				{
					tempo=0;
					CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MEgg", 135+(rand()%90));
					if(Damage(1))Boom(my_id);
				}
			}
		}
		//��
		else if(pattern==11)
		{
			tempo++;
			
			//����
			if(first)
			{
				if(tempo<caracter->width)Move(-1, 0);
					else first=false;
			}
			//����
			else
			{				
				if(tempo>=9-(difficult*2))
				{
					tempo=0;
					CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "Parasite", 135+(rand()%90));
				}
			}
		}
		//����
		else if(pattern==12)
		{
			Move(cos(dir*0.017453)*(caracter->speed), sin(dir*0.017453)*(caracter->speed));
			//���� �ε����� ���� ��ȯ
			if(x<0)
			{
				SetXY(0, y);
				dir+=(75+rand()%30);
			}
			else if(x>SCREEN_X-(caracter->width))
			{
				SetXY(SCREEN_X-(caracter->width), y);
				dir+=(75+rand()%30);
			}
			if(y<0)
			{
				SetXY(x, 0);
				dir+=(75+rand()%30);
			}
			else if(y>CLIP_Y-(caracter->height))
			{
				SetXY(x, CLIP_Y-(caracter->height));
				dir+=(75+rand()%30);
			}
			
			if(dir>=360)dir-=360;
			
			tempo++;
			if(tempo>=3000)life=false;
		}
		//���� ����
		else if(pattern==15)
		{
			tempo++;
			//����
			if(first)
			{
				if(tempo<30)Move(-(caracter->speed), 0);
					else first=false;
			}
			else
			{
				Move(cos(dir*0.017453)*(caracter->speed), sin(dir*0.017453)*(caracter->speed));
				//���� �ε����� ���� ��ȯ
				if(x<0)
				{
					SetXY(0, y);
					dir+=(75+rand()%30);
				}
				else if(x>SCREEN_X-(caracter->width))
				{
					SetXY(SCREEN_X-(caracter->width), y);
					dir+=(75+rand()%30);
				}
				if(y<0)
				{
					SetXY(x, 0);
					dir+=(75+rand()%30);
				}
				else if(y>CLIP_Y-(caracter->height))
				{
					SetXY(x, CLIP_Y-(caracter->height));
					dir+=(75+rand()%30);
				}
				
				if(dir>=360)dir-=360;
				
				if(tempo>=360)tempo=0;
				if((tempo%(6-difficult))==0)CreateUnit(UNIT_ENEMY, CENTER_FROM, my_id, "MEgg", tempo);
			}
		}
		//���� ���
		else if(pattern==37)
		{
			Move(-(caracter->speed), 0);
		}
		//���� ����
		else if(pattern==39)
		{
			Move(cos(dir*0.017453)*(caracter->speed), sin(dir*0.017453)*(caracter->speed));
			if(unit[0].y>y)Move(0, ((caracter->speed)*0.2));
				else Move(0, -((caracter->speed)*0.2));
		}
		//�ذ� ����
		else if(pattern==40)
		{
			tempo++;
			if(tempo>=60-(difficult*8))
			{
				CreateUnit(UNIT_ENEMY, CENTER_FROM, my_id, "MiniCell", 135+(rand()%90));
				tempo=0;
			}
		}
		//�Ǹ��� ��
		else if(pattern==41)
		{
			if(tempo==0)
			{
				if(x>(SCREEN_X-(caracter->width)))Move(-(caracter->speed), 0);
					else tempo=1;
			}
			else if(tempo==1)
			{
				CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MLaser", 180);
				tempo=2;
			}
			else if(tempo==2)
			{
				if(x<SCREEN_X)Move(caracter->speed, 0);
					else tempo=3;
			}
			else if(tempo==3)
			{
				SetXY(x, rand()%(CLIP_Y-(caracter->width)));
				tempo=0;
			}
		}
		//�κ�
		else if(pattern==43)
		{
			tempo++;
			//����
			if(first)
			{
				if(tempo<30)Move(-(caracter->speed), 0);
					else first=false;
			}
			else
			{
				Move(cos(dir*0.017453)*(caracter->speed), sin(dir*0.017453)*(caracter->speed));
				//���� �ε����� ���� ��ȯ
				if(y<0)dir=90;
				else if(y>CLIP_Y-(caracter->height))dir=270;
				if((tempo%(60-(difficult*5)))==0)CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MMissile", 180);
			}
		}
		//���� �̻���
		else if(pattern==44)
		{
			tempo++;
			double speed=(caracter->speed)+(double)tempo/10;
			Move(cos(dir*0.017453)*speed, sin(dir*0.017453)*speed);
			if((tempo%10)==0)Boom(my_id);
		}
		//���� ��
		else if(pattern==46)
		{
			tempo++;
			//����
			if(first)
			{
				if(tempo<15)Move(-(caracter->speed), 0);
					else
					{
						CreateUnit(UNIT_ENEMY, CENTER_FROM, my_id, "Jelly", 180);
						first=false;
					}
			}
			else
			{
				if((tempo%(60-(difficult*5)))==0)CreateUnit(UNIT_ENEMY, CENTER_FROM, my_id, "MJelly", 90+(rand()%180));
			}
		}
		//���� �̻���
		else if(pattern==47)
		{
			tempo++;
			if(tempo>=720)tempo=0;
			Move(cos(abs(360-tempo)*0.017453), sin(abs(360-tempo)*0.017453));
			Move(cos(dir*0.017453)*(caracter->speed), sin(dir*0.017453)*(caracter->speed));
		}
		//�����
		else if(pattern==48)
		{
			tempo++;
			
			//����
			if(first)
			{
				//��
				if(tempo==1)
				{
					int shield_id=CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "Shield", 180);
					unit[shield_id].tempo=my_id;
				}
				if(tempo<75)Move(-(caracter->speed), 0);
					else first=false;
			}
			//����
			else
			{
				if(tempo>=30-(difficult*3))
				{
					tempo=0;
					CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MEgg", 180);
					CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MEgg", 170);
					CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MEgg", 190);
				}

				if((rand()%3)==0)
				{
					if((unit[0].caracter->cy)+unit[0].y<y)Move(0,-(caracter->speed));
						else Move(0, caracter->speed);
				}
			}
		}
		//��
		else if(pattern==49)
		{
			SetXY(unit[tempo].x+(unit[tempo].caracter->cx), unit[tempo].y+(unit[tempo].caracter->cy)-10);
		}
		//�������� ��
		else if(pattern==51)
		{
			tempo++;
			//����
			if(tempo>=90)tempo=0;
			if((tempo%(18-(difficult*3)))==0)
			{
				CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MLight", tempo);
				CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MLight", tempo+90);
				CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MLight", tempo+180);
				CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MLight", tempo+270);
			}
		}
		//��
		else if(pattern==53)
		{
			//����
			if(x<505)
				Move(1, 0);
			//����
			else
			{
				tempo++;
				boss_tempo++;
				if(tempo>=220)tempo=0;
				//��
				if(boss_tempo>=160)boss_tempo=0;
					else if(boss_tempo<=difficult*10 && (tempo%2)==0)CreateUnit(UNIT_ENEMY, SHOT_FROM, my_id, "MFire", abs(90-tempo)+160);
			}			
		}
		//��
		else if(pattern==57)
		{
			tempo++;
			
			//����
			if(first)
			{
				if(tempo<338)Move(-(caracter->speed), 0);
					else first=false;
			}
		}
		//�̻���
		else if(pattern==100)
		{
			Move(cos(dir*0.017453)*(caracter->speed), sin(dir*0.017453)*(caracter->speed));
		}
		//Ư���� ����
		else if(pattern==998)
		{
			Move(cos(dir*0.017453)*(caracter->speed), sin(dir*0.017453)*(caracter->speed));
			SetXY(x, rand()%CLIP_Y);
			hp--;
			if(hp<=0)life=false;
		}
		//����
		else if(pattern==999)
		{
			Move(cos(dir*0.017453)*(caracter->speed), sin(dir*0.017453)*(caracter->speed));
			hp--;
			if(hp<=0)life=false;
		}
	}
}

//////////////////////////////////////////////////////
//���� ���� ����, �Լ�

int scroll, game_time;
int stage, chapter;
int score, hi_score, score_x, next_x, next_bonus;
char hi_name[20];
double weapon_angle;
int eye_wake;
int core_y;
int weapon[4];
bool boss_chapter;
int clear_delay;
bool pause, gameover, game_clear, mainmenu;
int stage_title_delay;

char* weapon_name[]={"WBase", "WThunder", "WBeam", "WSolar"};

int base_sound, solar_sound;

//��� ������ ID �˾Ƴ���
int GetAbleID(bool life, int start, int end)
{
	for(int i=start; i<end; i++)
		if(unit[i].life==life)return i;

	return -1;
}

//���� ����
int CreateUnit(int type, int x, int y, char* name, double dir, bool vboss)
{
	int unit_id=-1;
	if(type==UNIT_ENEMY)
	{
		if(vboss)unit_id=GetAbleID(false, 1, 20);								//����
			else unit_id=GetAbleID(false, 20, UNIT_MAX);						//��
	}
	else if(type==UNIT_WEAPON)unit_id=GetAbleID(false, UNIT_MAX, WEAPON_MAX);	//�Ʊ� �̻���
	else if(type==UNIT_ANI)unit_id=GetAbleID(false, WEAPON_MAX, ANI_MAX);		//�ִϸ��̼�

	//����
	if(unit_id>=0 && unit_id<ANI_MAX)
	{
		//�߾� ��ǥ���� ����
		if(x==SHOT_FROM)
		{
			if(y>=0 && y<ANI_MAX)
			{
				unit[unit_id].SetUnit(0, 0, name, dir);
				unit[unit_id].SetXY(unit[y].x+unit[y].caracter->cx-unit[unit_id].caracter->cx, unit[y].y+unit[y].caracter->cy-unit[unit_id].caracter->cy);
			}
			else return -1;
		}
		//ĳ���� �߾ӿ��� ����
		else if(x==CENTER_FROM)
		{
			if(y>=0 && y<ANI_MAX)
			{
				unit[unit_id].SetUnit(0, 0, name, dir);
				unit[unit_id].SetXY(unit[y].x+((unit[y].caracter->width)/2)-((unit[unit_id].caracter->width)/2), unit[y].y+((unit[y].caracter->height)/2)-((unit[unit_id].caracter->height)/2));
			}
			else return -1;
		}
		//�Ϲ� ��ǥ���� ����
		else unit[unit_id].SetUnit(x, y, name, dir);

		unit[unit_id].boss=vboss;

		return unit_id;
	}
	else return -1;
}

//�浹 �˻�
bool Crush(int id1, int id2)
{
	if(unit[id1].x < unit[id2].x+unit[id2].caracter->width && unit[id1].x+unit[id1].caracter->width > unit[id2].x &&
		unit[id1].y < unit[id2].y+unit[id2].caracter->height && unit[id1].y+unit[id1].caracter->height > unit[id2].y)
		return true;
	else
		return false;
}

//���� �߻�
void Shot(int weapon_id)
{	
	if(weapon[weapon_id]>0)
	{
		//�⺻ ����
		if(weapon_id==0)
		{
			CreateUnit(UNIT_WEAPON, SHOT_FROM, 0, weapon_name[weapon_id], weapon_angle);
			weapon_angle++;
			if(weapon_angle>=450)weapon_angle=271;
		}
		//����
		else if(weapon_id==1)
		{
			CreateUnit(UNIT_WEAPON, SHOT_FROM, 0, weapon_name[weapon_id], 0);
			CreateUnit(UNIT_ANI, SHOT_FROM, 0, "WeaponBoom2", 0);
		}
		//��, �� ���
		else CreateUnit(UNIT_WEAPON, SHOT_FROM, 0, weapon_name[weapon_id], 0);

		//���� �Ǵ� ����� �ò����� �ʰ� ���� ����
		if(weapon_id==0)
		{
			if(base_sound==0)
			{
				_Play(weapon_id);
				base_sound=5;
			}
		}
		else if(weapon_id==3)
		{
			if(solar_sound==0)
			{
				_Play(weapon_id);
				solar_sound=10;
			}
		}
		else _Play(weapon_id);

		weapon[weapon_id]--;
	}
}

//���ʽ�
void Bonus()
{
	weapon[0]+=5000;
	weapon[1]+=200;
	weapon[2]+=500;
	weapon[3]+=1000;

	unit[0].hp+=640;

	_Play(14);
}

//���� ȹ��
void GetScore(int point)
{
	score+=point;
	while(score>=next_x)
	{
		next_x*=10;
		score_x-=11;
	}
	//���ʽ�
	if(score>=next_bonus)
	{
		Bonus();
		next_bonus+=next_bonus;
	}
}

//����
void Boom(int id)
{
	char* boom_name[]={"Boom", "Boom2", "Boom3", "Boom4"};
	
	//���ΰ�
	if(id==0)
	{
		for(int i=0; i<360; i+=15)CreateUnit(UNIT_ANI, CENTER_FROM, id, boom_name[rand()%4], i);
		if(stage!=5)_Play(11);
	}
	//����
	else if(unit[id].boss)
	{
		for(int i=0; i<360; i+=45)CreateUnit(UNIT_ANI, CENTER_FROM, id, "MFire", i);
		GetScore(100);
		_Play(10);
	}
	//�̻���
	else if(id>=UNIT_MAX && id<WEAPON_MAX)
	{
		if(unit[id].caracter_id==27)CreateUnit(UNIT_ANI, CENTER_FROM, id, "WeaponBoom1", unit[id].dir);
			else if(unit[id].caracter_id==29)CreateUnit(UNIT_ANI, CENTER_FROM, id, "WeaponBoom3", unit[id].dir);
			else if(unit[id].caracter_id==30)CreateUnit(UNIT_ANI, CENTER_FROM, id, "WeaponBoom4", unit[id].dir);
	}
	//�����
	else if(unit[id].caracter_id==37)
	{
		CreateUnit(UNIT_ANI, CENTER_FROM, id, "Boom5", unit[id].dir);
		if((unit[id].caracter->pattern)!=100)_Play((rand()%4)+4);
		GetScore(10);
	}
	//����
	else if(unit[id].caracter->hp_max>=5)
	{
		CreateUnit(UNIT_ANI, CENTER_FROM, id, boom_name[rand()%4], unit[id].dir);
		if((unit[id].caracter->pattern)!=100)_Play((rand()%4)+4);
		GetScore(10);
	}
	//����
	else
	{
		CreateUnit(UNIT_ANI, CENTER_FROM, id, boom_name[rand()%4], unit[id].dir);
		if(unit[id].caracter_id!=44)_Play((rand()%2)+8);
	}
}

//�� ����
void BoomAll()
{
	for(int i=1; i<UNIT_MAX; i++)
		if(unit[i].life)
		{
			unit[i].life=false;
			Boom(i);
		}
}

//���� �ܰ�� �Ѿ��
void NextChapter(int t)
{
	if(game_time>=t)
	{
		game_time=0;
		boss_count=0;
		chapter++;
	}
}

//ü�� ǥ��
void ShowHP(int hp, int y)
{
	for(int i=0; i<640; i++)
	{
		if(hp<=i)break;
		
		if(hp>1920+i)jdd->DrawPicture(backbuffer, "HP4", i, y, NULL);
		else if(hp>1280+i)jdd->DrawPicture(backbuffer, "HP3", i, y, NULL);
		else if(hp>640+i)jdd->DrawPicture(backbuffer, "HP2", i, y, NULL);
			else jdd->DrawPicture(backbuffer, "HP", i, y, NULL);
	}
}

void SetStage(int stage_no)
{
	stage=stage_no;

	//���� �ʱ�ȭ
	for(int i=1; i<ANI_MAX; i++)
	{
		unit[i].life=false;
		unit[i].my_id=i;
	}
	unit[0].SetXY(100, 240);
	scroll=game_time=boss_count=0;
	clear_delay=0;
	chapter=1;
	boss_chapter=false;

	//�������� ����
	stage_title_delay=600;
	//���� ���
	if(stage==1)_MidiPlay("music\\Stage1.mid", true);
		else if(stage==2)_MidiPlay("music\\Stage2.mid", true);
		else if(stage==3)_MidiPlay("music\\Stage3.mid", true);
		else if(stage==4)
		{
			_MidiPlay("music\\boss.mid", true);
			eye_wake=0;
		}
		else if(stage==5)_MidiPlay("music\\ending.mid", true);
}

//���̽��ھ�
void HiScore()
{
	if(hi_score<score)
	{
		//���� ����
		hi_score=score;
		CDlg m_dlg;
		strcpy(hi_name, m_dlg.InputHan(290, 280, 8));

		//��ȣȭ
		unsigned char save_data[48];
		int code_score=hi_score+457;
		save_data[0]=255-(unsigned char)((hi_score&0xff000000)>>24);
		save_data[1]=(unsigned char)(code_score&0x000000ff);
		save_data[2]=255-(unsigned char)((hi_score&0x00ff0000)>>16);
		save_data[3]=(unsigned char)((code_score&0x0000ff00)>>8);
		save_data[4]=255-(unsigned char)((hi_score&0x0000ff00)>>8);
		save_data[5]=(unsigned char)((code_score&0x00ff0000)>>16);
		save_data[6]=255-(unsigned char)(hi_score&0x000000ff);
		save_data[7]=(unsigned char)((code_score&0xff000000)>>24);

		for(int i=0; i<20; i++)
		{
			save_data[(i*2)+8]=hi_name[i]&0xf0;
			save_data[(i*2)+9]=hi_name[19-i]&0x0f;
		}

		//���Ϸ� ����
		FILE* fp;
		if(fp=fopen("hiscore.sav", "wb"))
		{
			fwrite(save_data, sizeof(char), 48, fp);
			fclose(fp);
		}
	}
}

//////////////////////////////////////////////////
//������ �� ȭ��, ���μ���, ����
//////////////////////////////////////////////////
void Screen()
{
	//���
	char* background[]={"", "Back1", "Back2", "Back3"};
	if(stage<4)
	{
		for(int y=0; y<3; y++)
			for(int x=0; x<5; x++)
				jdd->DrawPicture(backbuffer, background[stage], x*160-scroll, y*160, NULL);
	}
	else if(stage==4)
	{
		jdd->DrawPicture(backbuffer, "BossLast", -(640+scroll), 0, NULL);
		if(eye_wake<20)DrawPicture("WakeUp", 160-scroll, 10, eye_wake, &ani_wakeup);
	}
	else if(stage==5)
	{
		for(int y=0; y<3; y++)
			for(int x=0; x<5; x++)
				jdd->DrawPicture(backbuffer, "BackE", x*160-scroll, y*160, NULL);
	}

	//���� ���� �׷��� �Ǵ� Ư�� ������
	for(int i=1; i<UNIT_MAX; i++)
		if(unit[i].caracter_id>=50 && unit[i].caracter_id<=53)unit[i].DrawUnit();
	//���� ���
	for(int i=0; i<ANI_MAX; i++)
		if(unit[i].caracter_id<50 || unit[i].caracter_id>53)unit[i].DrawUnit();

	//����
	if(stage==5)
	{
		float dark_opacity;
		if(chapter==1)dark_opacity=0.7f;
			else if(chapter==2)
			{
				dark_opacity=0.7f-((float)game_time*0.002f);
				if(dark_opacity<0)dark_opacity=0;
			}

		//ȭ���� ��Ӱ�
		if(chapter<3)
		{
			JPictureInfo pi;
			pi.SetOpacity(dark_opacity);
			jdd->SetPictureInfo(PIC_BLACK,&pi);
			for(int j=0; j<22; j++)
				jdd->DrawPicture(backbuffer,PIC_BLACK,0,j*20,NULL);
		}

		//���ʷα�
		if(chapter==1)
		{
			char* epilog[]={
				"��ȹ: �ڵ���1",
				"��ȹ�� �ڵ����Դϴ�. ����� ������ ������ ���ͼ� ����� �����ϴ�.",
				"������ �ٸ� ������ ����� ���̾��µ�, ����ٰ� ������ �� ������ �ؼ�",
				"��ģ �Ӹ��� ���� �� ������ ���� �ϳ� ����� ���ڰ� ������ �ſ���.",
				"�׳� �Ӹ��� �������� ��� ����� ���� ��ſ� ��ȹ�̾����ϴ�.",
				"���߿� ����� �����淡 ��� �ٵ�� ���� ���� ������ �� �� �մϴ�.",
				"���� ��ſ� �������� ���� ������ ��մ� �� �����ϴ�.",
				"",
				"���α׷���: �ڵ���2",
				"���α׷��� �� �ڵ���2�Դϴ�.",
				"�ڵ���1�� ��ȹ�� ��ſ�������, ó���� ������ ����ٰ� �ϴ���",
				"�����ٸ鼭 ��� ��ȹ�� �ø��� �ٶ��� �� �Ծ����ϴ�.",
				"�̻��ϵ� ó���� 8���⸸ ������µ�, ���߿� �Ҽ��� ��ǥ�� ��ġ��",
				"������ �ﰢ�Լ��� ����ؼ� �̻����� �� �Ƹ���� ������ �߽��ϴ�.",
				"�ƹ�ư ������ ���µ� ������� �ﰢ�Լ� ���α׷� �ϴ��� ��������ϴ�.",
				"",
				"�׷���: �ڵ���3",
				"��, ���� �ڵ���1������ ó�� ���� �׸� �׸��� �� ������.",
				"�ű�� �ִϸ��̼ǵ� �����µ� �� ���� �ְ�...",
				"�ƹ�ư �� ���⿡ �´� �͵��� �׷��� ���ҽ��ϴ�.",
				"",
				"����: �ڵ���4",
				"���� �� ���� ������. �� ���� �״��� ���...",
				"ȯ���� �� ���Ƽ� �ҽ��� ���� �� �������ϴ�.",
				"��¿ �� ���� ���� ��� ȿ������ �⺻ ������ 3D�ɺ����� ��������.",
				"�ƹ�ư �ٸ� �ڵ���鵵 ���� �ϼ̽��ϴ�.",
				"",
				"õ��: �ڵ���5",
				"��������! ���� �� ����� ���� ��...",
				"���� �ΰ�: �ڵ���6",
				"����! ��Ҹ� �׸�...",
				"����: �ڵ���7",
				"��... ��ȭ ���� �ʹ�.",
				"�ڵ���99A+1: ������",
				"����... �̰� ����!",
				"^((&)*&#)*$&#$%H#*(HR*#&YR#&*$*@...",
				"......",
				"�������� ������ ���������� �� ���� �ϰڽ��ϴ�.",
				"�� ������ ���ֽ� �����е鲲 �������� ���� �޾ƾ� �ڽ��ϴ�."
			};
			for(int k=0; k<39; k++)
				jdd->DrawText(backbuffer, epilog[k], font12, 50, 440-(game_time/4)+k*20, JColor(255,255,255));
		}
	}

	//����â
	jdd->DrawPicture(backbuffer, "Bottom", 0, CLIP_Y, NULL);

	//�����&���� ������
	ShowHP(unit[0].hp, 440);
	if(boss_chapter)ShowHP(boss_hp, 0);

	//���� �ܷ�
	jdd->DrawText(backbuffer, StrAdd("d",weapon[0]), font20, 70, 450, JColor(255,255,255));
	jdd->DrawText(backbuffer, StrAdd("d",weapon[1]), font20, 205, 450, JColor(255,255,255));
	jdd->DrawText(backbuffer, StrAdd("d",weapon[2]), font20, 320, 450, JColor(255,255,255));
	jdd->DrawText(backbuffer, StrAdd("d",weapon[3]), font20, 450, 450, JColor(255,255,255));
	jdd->DrawText(backbuffer, StrAdd("d",score), font20, score_x, 450, JColor(255,255,0));

	//����
	if(pause)
	{
		jdd->DrawPicture(backbuffer, "Pause", 256, 195, NULL);
		jdd->DrawText(backbuffer, "ESC : �޴���     ENTER : ����ϱ�", font20, 161, 251, JColor(255,255,255));
		jdd->DrawText(backbuffer, "ESC : �޴���     ENTER : ����ϱ�", font20, 160, 250, JColor(0,0,0));
	}
	//���� ����
	else if(gameover)
	{
		jdd->DrawPicture(backbuffer, "Gameover", 146, 195, NULL);
		if(mainmenu)
		{
			jdd->DrawText(backbuffer, "����� �ְ�����! �̸��� �Է��� �ֽʽÿ�.", font20, 121, 251, JColor(255,255,255));
			jdd->DrawText(backbuffer, "����� �ְ�����! �̸��� �Է��� �ֽʽÿ�.", font20, 120, 250, JColor(0,0,0));
		}
	}
	//���� ��
	if(game_clear)
	{
		jdd->DrawPicture(backbuffer, "TheEnd", 531, 296, NULL);
		if(mainmenu)
		{
			jdd->DrawText(backbuffer, "����� �ְ�����! �̸��� �Է��� �ֽʽÿ�.", font20, 121, 251, JColor(255,255,255));
			jdd->DrawText(backbuffer, "����� �ְ�����! �̸��� �Է��� �ֽʽÿ�.", font20, 120, 250, JColor(0,0,0));
		}
	}
	//�������� Ÿ��Ʋ
	else if(stage<=4 && stage_title_delay>0)
	{
		JPictureInfo jpi;
		jdd->GetPictureInfo("StageTitle", &jpi);
		float title_opacity=jpi.GetOpacity();
		if(stage_title_delay==600)title_opacity=0.0f;
			else if(stage_title_delay>=500)title_opacity+=0.01f;
			else if(stage_title_delay<=100)title_opacity-=0.01f;
		jpi.SetOpacity(title_opacity);
		jdd->SetPictureInfo("StageTitle", &jpi);
		DrawPicture("StageTitle", 123, 193, stage-1, &ani_title);		
		stage_title_delay--;
	}
}

void Process()
{
	//���� �ð� �帧
	game_time++;

	if(!boss_chapter)
	{
		//��� ��ũ��
		scroll+=difficult;
		if(scroll>=160)scroll-=160;
	}

	static int temp_id;

	//��������1
	if(stage==1)
	{
		if(chapter==1)	//��������(����)
		{
			if((game_time%(30-(difficult*2)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Worm", 180);
			NextChapter(2000);
		}
		else if(chapter==2)	//������� ��
		{
			if((game_time%(100-(difficult*5)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-40), "AngryEye", 180);
			NextChapter(1000);
		}
		else if(chapter==3) //��������(����)
		{
			if((game_time%(20-(difficult*2)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Worm", 180);
			NextChapter(2000);
		}
		else if(chapter==4)	//������ ��
		{
			if((game_time%(300-(difficult*10)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-40), "BeautyEye", 180);
			NextChapter(1000);
		}
		else if(chapter==5)	//�߰� ���� �ĸ�
		{
			if(game_time==1)CreateUnit(UNIT_ENEMY, SCREEN_X, 200, "BossFly", 180, true);
			if(boss_count>=1)NextChapter(0);
		}
		else if(chapter==6)	//�� �յ� ����
		{
			if((game_time%(100-(difficult*5)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-40), "AngryEye", 180);
			if((game_time%(300-(difficult*10)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-40), "BeautyEye", 180);
			NextChapter(1000);
		}
		else if(chapter==7)	//�������� ��
		{
			if((game_time%(6-difficult))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Worm", 180);
			NextChapter(2000);
		}
		else if(chapter==8)	//�Ѱ���
		{
			if((game_time%(20-(difficult*2)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Worm", 180);
			if((game_time%(100-(difficult*5)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-40), "AngryEye", 180);
			if((game_time%(300-(difficult*10)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-40), "BeautyEye", 180);
			NextChapter(3000);
		}
		else if(chapter==9)	//���� ��
		{
			if(game_time==1)
			{
				BoomAll();
				_MidiStop();
				boss_hp=unit[CreateUnit(UNIT_ENEMY, SCREEN_X, CLIP_Y-39, "BossDung", 180, true)].hp;
				CreateUnit(UNIT_ENEMY, SCREEN_X, 100, "BossFly", 180);
				CreateUnit(UNIT_ENEMY, SCREEN_X, 300, "BossFly", 180);
				boss_chapter=true;
				_MidiPlay("music\\boss.mid", true);
			}
			else if(boss_hp<=0)	//Ŭ����
			{
				if(clear_delay==0)
				{
					BoomAll();
					_MidiStop();
					_Play(13);
				}
				
				if(clear_delay>900)
				{
					FadeOut();
					Bonus();
					SetStage(2);
				}
				else clear_delay++;
			}
		}
	}
	//��������2
	else if(stage==2)
	{
		if(chapter==1)	//��
		{
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Hand2", 180);
			NextChapter(2000);
		}
		else if(chapter==2)	//����Ű�� ��
		{
			if((game_time%(300-(difficult*10)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Hand1", 180);
			NextChapter(2000);
		}
		else if(chapter==3)	//���� ��
		{
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Hand2", 180);
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, rand()%(SCREEN_X-20), -20, "Foot", 90);
			NextChapter(1000);
		}
		else if(chapter==4)	//�ָ԰� ��
		{
			if((game_time%(300-(difficult*10)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Hand1", 180);
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X-15, rand()%(CLIP_Y-10), "Hand3", 180);
			NextChapter(1500);
		}
		else if(chapter==5)	//�߰����� 256
		{
			if(game_time==1)CreateUnit(UNIT_ENEMY, SCREEN_X, 200, "Box", 180, true);
			if(boss_count>=1)NextChapter(0);
		}
		else if(chapter==6)	//����Ű�� �հ� ��
		{
			if((game_time%(200-(difficult*10)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Hand1", 180);
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, rand()%(SCREEN_X-20), -20, "Foot", 90);
			NextChapter(1500);
		}
		else if(chapter==7)	//�߰� �ָ�
		{
			if((game_time%(50-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X-15, rand()%(CLIP_Y-10), "Hand3", 180);
			if((game_time%(50-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, rand()%(SCREEN_X-20), -20, "Foot", 90);
			NextChapter(2000);
		}
		else if(chapter==8)	//�Ѱ���
		{
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Hand2", 180);
			if((game_time%(200-(difficult*10)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "Hand1", 180);
			if((game_time%(50-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X-15, rand()%(CLIP_Y-10), "Hand3", 180);
			if((game_time%(50-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, rand()%(SCREEN_X-20), -20, "Foot", 90);
			NextChapter(3000);
		}
		else if(chapter==9)	//���� ��
		{
			if(game_time==1)
			{		
				BoomAll();
				_MidiStop();
				boss_hp=unit[CreateUnit(UNIT_ENEMY, SCREEN_X, 0, "BossFace", 180, true)].hp;
				boss_chapter=true;
				_MidiPlay("music\\boss.mid", true);
			}
			else if(boss_hp<=0)	//Ŭ����
			{
				if(clear_delay==0)
				{
					BoomAll();
					_MidiStop();
					_Play(13);
				}
				
				if(clear_delay>900)
				{
					FadeOut();
					Bonus();
					SetStage(3);
				}
				else clear_delay++;
			}
		}
	}
	//��������3
	else if(stage==3)
	{
		if(chapter==1)	//����
		{
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X-(50+rand()%50), rand()%(CLIP_Y-10), "Cell1", 135+(rand()%90));
			NextChapter(1500);
		}
		else if(chapter==2)	//����+���
		{
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X-(50+rand()%50), rand()%(CLIP_Y-10), "Cell1", 135+(rand()%90));
			if((game_time%(200-(difficult*20)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, 0, "CellPillar", 180);
			NextChapter(1500);
		}
		else if(chapter==3)	//����2
		{
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X-(50+rand()%50), rand()%(CLIP_Y-10), "Cell2", 135+(rand()%90));
			NextChapter(1500);
		}
		else if(chapter==4)	//���� ����
		{
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X-(50+rand()%50), rand()%(CLIP_Y-10), "CellT", 135+(rand()%90));
			NextChapter(1500);
		}
		else if(chapter==5)	//�ռ���
		{
			if(game_time==1)CreateUnit(UNIT_ENEMY, SCREEN_X, 200, "BossCell", 180, true);
			if(boss_count>=1)NextChapter(0);
		}
		else if(chapter==6)	//���� ����
		{
			if((game_time%(12-(difficult*2)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "MiniCell", 180);
			NextChapter(1500);
		}
		else if(chapter==7)	//������, ���
		{
			if((game_time%(200-(difficult*20)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, 0, "CellPillar", 180);
			if((game_time%(60-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X-(50+rand()%50), rand()%(CLIP_Y-10), "CellT", 135+(rand()%90));
			NextChapter(1500);
		}
		else if(chapter==8)	//�Ѱ���
		{
			if((game_time%(30-difficult*2))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, rand()%(CLIP_Y-10), "MiniCell", 180);
			if((game_time%(120-(difficult*6)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X-(50+rand()%50), rand()%(CLIP_Y-10), "Cell1", 135+(rand()%90));
			if((game_time%(200-(difficult*20)))==0)CreateUnit(UNIT_ENEMY, SCREEN_X, 0, "CellPillar", 180);
			if((game_time%(120-(difficult*6)))==60)CreateUnit(UNIT_ENEMY, SCREEN_X-(50+rand()%50), rand()%(CLIP_Y-10), "CellT", 135+(rand()%90));
			NextChapter(3000);
		}
		else if(chapter==9)	//���� �ذ� ����
		{
			if(game_time==1)
			{
				BoomAll();
				_MidiStop();
				boss_hp=0;
				for(int i=0; i<20; i++)
				{
					boss_hp+=unit[CreateUnit(UNIT_ENEMY, SCREEN_X-(90+rand()%80), 140+(rand()%160), "SkullCell", 180, true)].hp;
				}
				boss_chapter=true;
				_MidiPlay("music\\boss.mid", true);
			}
			else if(boss_hp<=0)	//Ŭ����
			{
				if(clear_delay==0)
				{
					BoomAll();
					_MidiStop();
					_Play(13);
				}
				
				if(clear_delay>900)
				{
					FadeOut();
					Bonus();
					SetStage(4);
				}
				else clear_delay++;
			}
			else
			{
				//���
				if(boss_hp<2000 && (game_time%500)==0)
				{
					int boss_id=CreateUnit(UNIT_ENEMY, SCREEN_X-(50+rand()%40), 200+(rand()%40), "SkullCell", 180, true);
					if(boss_id>=0)boss_hp+=unit[boss_id].hp;
				}
			}
		}
	}
	//��������4
	else if(stage==4)
	{
		boss_chapter=true;
		if(chapter==1)	//�ĸ�x3
		{
			if(game_time==1)
			{
				CreateUnit(UNIT_ENEMY, SCREEN_X, 100, "BossFly", 180, true);
				CreateUnit(UNIT_ENEMY, SCREEN_X, 200, "BossFly", 180, true);
				CreateUnit(UNIT_ENEMY, SCREEN_X, 300, "BossFly", 180, true);
			}
			else if(boss_count>=3)NextChapter(0);
		}
		else if(chapter==2)	//256x3
		{
			if(game_time==1)
			{
				CreateUnit(UNIT_ENEMY, SCREEN_X, 100, "Box", 180, true);
				CreateUnit(UNIT_ENEMY, SCREEN_X, 200, "Box", 180, true);
				CreateUnit(UNIT_ENEMY, SCREEN_X, 300, "Box", 180, true);
			}
			else if(boss_count>=3)NextChapter(0);
		}
		else if(chapter==3)	//����x2
		{
			if(game_time==1)
			{
				CreateUnit(UNIT_ENEMY, SCREEN_X, 100, "BossCell", 225, true);
				CreateUnit(UNIT_ENEMY, SCREEN_X, 300, "BossCell", 135, true);
			}
			else if(boss_count>=2)NextChapter(0);
		}
		else if(chapter==4)	//�Ǹ��� ��
		{
			if(game_time==1)
			{
				CreateUnit(UNIT_ENEMY, SCREEN_X, 100, "DevilEye", 180, true);
			}
			else if(boss_count>=1)NextChapter(0);
		}
		else if(chapter==5)	//�κ�
		{
			if(game_time==1)
			{
				CreateUnit(UNIT_ENEMY, SCREEN_X, 200, "Robot", 90, true);
			}
			else if(boss_count>=1)NextChapter(0);
		}
		else if(chapter==6)	//����
		{
			if(game_time==1)
			{
				CreateUnit(UNIT_ENEMY, SCREEN_X, 200, "JellyCore", 180, true);
				core_y=200;
			}
			else if(boss_count>=1)
			{
				BoomAll();
				NextChapter(0);
			}
		}
		else if(chapter==7)	//�����
		{
			if(game_time==1)
			{
				CreateUnit(UNIT_ENEMY, SCREEN_X, 200, "Flight", 180, true);
			}
			else if(boss_count>=1)
			{
				BoomAll();
				NextChapter(0);
			}
		}
		else if(chapter==8)	//��
		{
			if(game_time==1)
			{
				BoomAll();
				_MidiStop();
				boss_hp=1000;
				_MidiPlay("music\\last.mid", true);
			}
			else if(game_time<100)
			{
				if((game_time%5)==0 && eye_wake<20)eye_wake++;
			}
			else if(game_time==100)
			{
				eye_wake=20;
				boss_hp=unit[CreateUnit(UNIT_ENEMY, 188, 25, "BossEye", 180, true)].hp;
			}
			else if(game_time>100)
			{
				if(boss_hp<=0)	//Ŭ����
				{
					if(clear_delay==0)
					{
						BoomAll();
						temp_id=CreateUnit(UNIT_ENEMY, -135, 359, "Tongue", 180, true);
						clear_delay=1;
						eye_wake=19;
					}
					else
					{
						if(scroll>=-639)scroll--;
							else NextChapter(0);
						if((game_time%5)==0 && eye_wake>0)eye_wake--;
					}
				}
			}
		}
		else if(chapter==9)	//��
		{
			if(game_time==1)
			{
				boss_hp=unit[temp_id].hp;
			}
			else if(boss_hp<=0)	//Ŭ����
			{
				if(clear_delay==1)
				{
					BoomAll();
					_MidiStop();
					_Play(13);
				}
				
				if(clear_delay>=900)
				{
					FadeOut();
					SetStage(5);
				}
				else clear_delay++;
			}
		}
	}
	//����
	else if(stage==5)
	{
		//���ʷα�
		if(chapter==1)
		{
			NextChapter(4900);
		}
		//���� ã�� ���ΰ�
		else if(chapter==2)
		{
			if(game_time==300)CreateUnit(UNIT_ANI, 147, 177, "Nice", 0);
				else if(game_time==400)CreateUnit(UNIT_ANI, SCREEN_X, 229, "Handsome", 0);
				else if(game_time>600 && game_time<=700)unit[0].x+=2;
				else if(game_time>720 && game_time<=820 && (game_time%20)==0)Boom(0);

			NextChapter(1200);
		}
		//��
		else if(chapter==3)
		{
			game_clear=true;
		}
	}

	//���� �ൿ
	for(int i=0; i<ANI_MAX; i++)unit[i].Action();

	//�浹 üũ
	for(int i=1; i<UNIT_MAX; i++)
	{
		if(unit[i].life)
		{
			if(unit[i].caracter_id<50 || unit[i].caracter_id>53)
			{
				//���� ���ΰ�
				if(unit[0].life && Crush(0, i))
				{
					//���ΰ��� ����
					if(unit[0].Damage(unit[i].caracter->power))
					{
						Boom(0);
						gameover=true;
					}
					//���� ����
					if(unit[i].Damage(unit[0].caracter->power))
					{
						Boom(i);
						continue;
					}
				}
			}

			//���� ����
			if(unit[i].caracter_id==12)
			{
				for(int j=1; j<UNIT_MAX; j++)
					if(i!=j && unit[j].life && unit[j].caracter_id==12 && Crush(i, j))
					{
						unit[i].life=false;
						unit[j].life=false;
						CreateUnit(UNIT_ENEMY, unit[i].x, unit[i].y, "Cell2", unit[i].dir+unit[j].dir);
						continue;
					}
			}
			else if(unit[i].caracter_id==13)
			{
				for(int j=1; j<UNIT_MAX; j++)
					if(i!=j && unit[j].life && unit[j].caracter_id==13 && Crush(i, j))
					{
						unit[i].life=false;
						unit[j].life=false;
						CreateUnit(UNIT_ENEMY, unit[i].x, unit[i].y, "Cell4", unit[i].dir+unit[j].dir);
						continue;
					}
			}

			//�̻��ϰ� ��
			for(int j=UNIT_MAX; j<WEAPON_MAX; j++)
			{
				if(unit[j].life && Crush(i, j))
				{
					//����
					if(unit[i].caracter_id==45)
					{
						if(unit[j].y>core_y)unit[j].dir+=5;
							else unit[j].dir-=5;
					}
					//���
					else if(unit[i].caracter_id==49)
					{
						unit[j].life=false;
					}
					//�̻����� ����
					else
					{
						if(unit[j].Damage(1) && unit[j].caracter_id!=28)Boom(j);
					}

					//���� ����
					if(unit[i].caracter_id!=10)
					{
						if(unit[i].Damage(unit[j].caracter->power))Boom(i);
					}

					//����
					GetScore(1);
				}
			}
		}
	}
}

void Control()
{
	if(stage==5)return;

	if(unit[0].life)
	{
		//���� �̵�
		if(GetKey(3,1))unit[0].Move(0, unit[0].caracter->speed);
			else if(GetKey(0,1))unit[0].Move(0, -(unit[0].caracter->speed));
		if(GetKey(1,1))unit[0].Move(-(unit[0].caracter->speed), 0);
			else if(GetKey(2,1))unit[0].Move(unit[0].caracter->speed, 0);

		//����
		if(GetKey(31,1))Shot(0);
		if(GetKey(49,20))Shot(1);
		if(GetKey(34,10))Shot(2);
		if(GetKey(36,1))Shot(3);
	}

	base_sound=Max(base_sound-1, 0);
	solar_sound=Max(solar_sound-1, 0);
}

//////////////////////////////////////////////////
//���� �ڵ�
//////////////////////////////////////////////////
int main()
{	
	jdd=CreateDirectDraw();
	jre=CreateDXResourceManager(jdd);

	HINSTANCE hInstance=(HINSTANCE)0x00400000;

	WNDCLASS wc={0};
	wc.hIcon=LoadIcon(hInstance,"LIFE.ico");
	wc.hCursor=LoadCursor(hInstance,IDC_ARROW);
	wc.lpfnWndProc=WndProc;
	wc.hInstance=hInstance;
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName="Demo";
	RegisterClass(&wc);
	
	hwnd=CreateWindow("Demo","����",WS_POPUP|WS_VISIBLE,0,0,SCREEN_X,SCREEN_Y,NULL,NULL,hInstance,NULL);
    ShowCursor( FALSE );

	//���� �ʱ�ȭ
	if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
	{
		SoundCard = TRUE;
		if (SoundOBJ->SetCooperativeLevel(hwnd,DSSCL_PRIORITY)!=DS_OK) return 0;

		memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
		DSB_desc.dwSize = sizeof(DSBUFFERDESC);
		DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

		if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK) return 0;
		SoundDSB -> SetVolume(0);
		SoundDSB -> SetPan(0);
	}
	else SoundCard = FALSE;

	jdd->Initialize(NULL,hwnd,SCREEN_X,SCREEN_Y,16,true);
	
	//�ʱ�ȭ
	backbuffer=jdd->GetBackBuffer();
	font20=jdd->CreateFont("����ü",20,true,false,false,false,false);
	font12=jdd->CreateFont("HY�Ÿ���",16,true,false,false,false,false);
	srand( (unsigned)time( NULL ) );
	
	jdd->SetFrameRate(100,true);
	jdd->SetVerticalSync(false);

	//�׸� �ҷ�����
	jre->LoadResource("MLC\\common.mlc");
	jre->LoadResource("MLC\\shooting.mlc");
	jre->LoadResource("MLC\\menu.mlc");

	//�Ҹ� �ҷ�����
	if(SoundCard)
	{
		Sound[0] = SndObjCreate(SoundOBJ,"sound\\base.wav",2);
		Sound[1] = SndObjCreate(SoundOBJ,"sound\\thunder.wav",2);
		Sound[2] = SndObjCreate(SoundOBJ,"sound\\beam.wav",2);
		Sound[3] = SndObjCreate(SoundOBJ,"sound\\solar.wav",2);
		Sound[4] = SndObjCreate(SoundOBJ,"sound\\boom1.wav",2);
		Sound[5] = SndObjCreate(SoundOBJ,"sound\\boom2.wav",2);
		Sound[6] = SndObjCreate(SoundOBJ,"sound\\boom3.wav",2);
		Sound[7] = SndObjCreate(SoundOBJ,"sound\\boom4.wav",2);
		Sound[8] = SndObjCreate(SoundOBJ,"sound\\boom5.wav",2);
		Sound[9] = SndObjCreate(SoundOBJ,"sound\\boom6.wav",2);
		Sound[10] = SndObjCreate(SoundOBJ,"sound\\boss_boom.wav",2);
		Sound[11] = SndObjCreate(SoundOBJ,"sound\\die.wav",2);
		Sound[12] = SndObjCreate(SoundOBJ,"sound\\menu.wav",2);
		Sound[13] = SndObjCreate(SoundOBJ,"sound\\clear.wav",2);
		Sound[14] = SndObjCreate(SoundOBJ,"sound\\bonus.wav",2);
	}

	//���̽��ھ� �ҷ�����
	FILE* fp;
	if(fp=fopen("hiscore.sav","rb"))
	{
		unsigned char save_data[48];
		fread(save_data, sizeof(char), 48, fp);

		//�ص�
		int code_score=hi_score+457;
		hi_score=0;
		hi_score+=(int)(255-save_data[0])<<24;
		hi_score+=(int)(255-save_data[2])<<16;
		hi_score+=(int)(255-save_data[4])<<8;
		hi_score+=(int)(255-save_data[6]);
		code_score=0;
		code_score+=(int)save_data[1];
		code_score+=(int)save_data[3]<<8;
		code_score+=(int)save_data[5]<<16;
		code_score+=(int)save_data[7]<<24;
		code_score-=457;

		if(code_score==hi_score)
		{
			strcpy(hi_name, "");
			for(int i=0; i<20; i++)
			{
				hi_name[i]|=save_data[(i*2)+8];
				hi_name[19-i]|=save_data[(i*2)+9];
			}
		}
		else
		{
			hi_score=0;
			strcpy(hi_name, "��Ŀ");
		}

		fclose(fp);
	}
	else
	{
		hi_score=0;
		strcpy(hi_name, "����");
	}

	//���� �޴� ���� ����
	mainmenu=true;
	int selected_menu=0;

	//���� ����
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//���� �޴�
		if(mainmenu)
		{
			//�޴� ȭ��
			DrawPicture("Title", 0, 0);
			for(int i=0; i<5; i++)
			{
				if(selected_menu==i)DrawPicture("MainMenu", 220, 180+i*50, i+5, &ani_mainmenu);
					else DrawPicture("MainMenu", 220, 180+i*50, i, &ani_mainmenu);
			}
			jdd->DrawText(backbuffer, StrAdd("cccd","�ְ�����:", hi_name, " ", hi_score), font20, 190, 0, JColor(255,255,255));
			jdd->DrawText(backbuffer, "����: �ڵ���, 2005", font20, 225, 450, JColor(255,255,255));

			//�޴� ����
			if(GetKey(3))
			{
				selected_menu++;
				_Play(12);
			}
			else if(GetKey(0))
			{
				selected_menu--;
				_Play(12);
			}
			if(selected_menu>=5)selected_menu-=5;
				else if(selected_menu<0)selected_menu+=5;

			//������
			if(GetKey(5))break;
			//���� ����
			if(GetKey(4))
			{
				mainmenu=false;

				//���̵��� ���� ���� �ӵ�
				difficult=selected_menu+1;
				double difficult_speed=0.5+((double)difficult*0.5);

				//ĳ���� ����
				cara[0].SetCaracter("Skull", 640, 0, 1, 4, 29, 45);
				cara[1].SetCaracter("Worm", 1, 1, 1, difficult_speed, 0, 5, &ani_worm);
				cara[2].SetCaracter("AngryEye", 5, 2, 1, 1, 3, 20);
				cara[3].SetCaracter("BeautyEye", 10, 3, 1, 4, 19, 17);
				cara[4].SetCaracter("BossFly", 100, 4, 1, 4, 0, 7, &ani_fly);
				cara[5].SetCaracter("BossDung", 500, 5, 1, 0, 31, 1);
				cara[6].SetCaracter("Hand2", 5, 6, 1, difficult_speed, 34, 21);
				cara[7].SetCaracter("Hand1", 10, 7, 1, 4, 0, 9);
				cara[8].SetCaracter("Foot", 5, 8, 1, 3+difficult_speed, 27, 14);
				cara[9].SetCaracter("Hand3", 5, 9, 1, 3+difficult_speed, 14, 14);
				cara[10].SetCaracter("Box", 256, 10, 1, 4, 0, 15);
				cara[11].SetCaracter("BossFace", 1000, 11, 1, 4, 15, 107);
				cara[12].SetCaracter("Cell1", 5, 12, 1, 2+difficult_speed, 0, 0);
				cara[13].SetCaracter("Cell2", 10, 12, 2, 3+difficult_speed, 0, 0);
				cara[14].SetCaracter("Cell4", 20, 12, 4, 4+difficult_speed, 0, 0);
				cara[15].SetCaracter("BossCell", 300, 15, 1, 2+difficult_speed, 0, 0);
				cara[19].SetCaracter("MLight", 1, 100, difficult, difficult_speed, 17, 20);
				cara[20].SetCaracter("MDot", 1, 100, difficult, 4+difficult_speed, 2, 2);
				cara[21].SetCaracter("MGas", 10, 100, 1, difficult_speed, 10, 10, &ani_gas);
				cara[22].SetCaracter("MThunder", 3, 100, 1, 6+difficult_speed, 8, 6);
				cara[23].SetCaracter("MEgg", 1, 100, difficult, 6+difficult_speed, 2, 2);
				cara[24].SetCaracter("Parasite", 1, 100, difficult*2, 4, 12, 2, &ani_parasite);
				cara[27].SetCaracter(weapon_name[0], 1, 100, 1, 4, 0, 3);
				cara[28].SetCaracter(weapon_name[1], 50, 100, 1, 8, 20, 89, &ani_thunder);
				cara[29].SetCaracter(weapon_name[2], 1, 100, 5, 6, 0, 100);
				cara[30].SetCaracter(weapon_name[3], 1, 100, 1, 3, 30, -2);
				cara[31].SetCaracter("Boom", 100, 999, 1, 1, 20, 20, &ani_boom);
				cara[32].SetCaracter("Boom2", 100, 999, 1, 1, 20, 20, &ani_boom);
				cara[33].SetCaracter("Boom3", 100, 999, 1, 1, 20, 20, &ani_boom);
				cara[34].SetCaracter("Boom4", 100, 999, 1, 1, 20, 20, &ani_boom);
				cara[35].SetCaracter("MFire", 100, 100, 1, 2, 11, 44, &ani_fire);
				cara[36].SetCaracter("CellT", 5, 12, 1, 2+difficult_speed, 0, 0);
				cara[37].SetCaracter("CellPillar", 30, 37, difficult, difficult_speed, 0, 0);
				cara[38].SetCaracter("Boom5", 100, 998, 1, 1, 20, 20, &ani_boom);
				cara[39].SetCaracter("MiniCell", 1, 39, 1, 2+difficult_speed, 0, 0);
				cara[40].SetCaracter("SkullCell", 100, 40, 1, 0, 0, 0, &ani_cell);
				cara[41].SetCaracter("DevilEye", 100, 41, 1, 1+(difficult_speed*2), 3, 32);
				cara[42].SetCaracter("MLaser", 100, 100, 1, 3+difficult_speed, 55, 6);
				cara[43].SetCaracter("Robot", 200, 43, 1, 3, 38, 13, &ani_robot);
				cara[44].SetCaracter("MMissile", 4, 44, 5, 0, 7, 2, &ani_missile);
				cara[45].SetCaracter("Jelly", 100000, 45, 1, 0, 0, 0, &ani_jelly);
				cara[46].SetCaracter("JellyCore", 50, 46, 5, 6, 0, 0);
				cara[47].SetCaracter("MJelly", 10, 47, 1, 0.5, 0, 0);
				cara[48].SetCaracter("Flight", 100, 48, 1, 2, -6, 10);
				cara[49].SetCaracter("Shield", 1000, 49, 1, 0, 0, 0, &ani_shield);
				cara[51].SetCaracter("BossEye", 1000, 51, 0, 0, 168, 81);
				cara[53].SetCaracter("Tongue", 2000, 53, 0, 0, 0, 0);
				cara[54].SetCaracter("WeaponBoom1", 30, 999, 0, 0, 0, 0, &ani_weapon_boom1);
				cara[55].SetCaracter("WeaponBoom3", 15, 999, 0, 0, 0, 0, &ani_weapon_boom3);
				cara[56].SetCaracter("WeaponBoom4", 10, 999, 0, 0, 0, 0, &ani_weapon_boom4);
				cara[57].SetCaracter("Handsome", 1, 57, 1, 1, 0, 0);
				cara[58].SetCaracter("Nice", 300, 999, 1, 0, 0, 0);
				cara[59].SetCaracter("WeaponBoom2", 10, 999, 0, 0, 47, 100, &ani_weapon_boom4);

				//���� ���� �ʱ�ȭ
				unit[0].SetUnit(0, 0, "Skull", 0);
				SetStage(1);
				score=0;
				next_bonus=5000;
				score_x=625;
				next_x=10;
				weapon[0]=5000;
				weapon[1]=200;
				weapon[2]=500;
				weapon[3]=1000;
				weapon_angle=271;
				boss_tempo=0;
				//chapter=8;
				pause=gameover=game_clear=false;
				base_sound=solar_sound=0;

				FadeOut();
			}
		}
		//����
		else
		{
			//����, ó��, ȭ�� ���
			Control();
			Process();
			Screen();

			//���� ����
			if(gameover || game_clear)
			{
				if(GetKey(5) || GetKey(4))
				{
					mainmenu=true;
					HiScore();
					_MidiStop();
					FadeOut();
				}
			}
			//����
			else if(GetKey(5))
			{
				pause=true;
				Pause();
				if(_GetKeyState(VK_ESCAPE))
				{
					mainmenu=true;
					_MidiStop();
					FadeOut();
				}
				pause=false;
			}
		}

		//������
		jdd->Render();
	}
	FadeOut();

	return 0;
}

LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch ( msg )
    {
		case MM_MCINOTIFY    :	if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL ) _MidiReplay();
								break;

		case WM_DESTROY 	 : 	jdd->DeleteFont(font20);
								jdd->DeleteFont(font12);
								delete jdd;
								_CrtDumpMemoryLeaks();
								break;
	}

	return DefWindowProc(wnd,msg,wParam,lParam);
}