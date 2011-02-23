#pragma once

#include <map>

using namespace std;

//�ִϸ��̼� ������ Ŭ����
enum AniType	//�ִϸ��̼� Ÿ��
{
	ani_normal,
	ani_loop,
	ani_exchange,
	ani_once
};

enum WalkDirection	//�ȴ� ����
{
	dir_up,
	dir_left,
	dir_down,
	dir_right,
	dir_turn,
	dir_turn_left,
	dir_turn_right,
	dir_random
};

class CAnimationData
{
private:
	//���� ��
	int x_size, y_size;
	BOOL walk;					//���� ����(�⺻��: FALSE)
	int up, down, left, right;	//���� �׷����� ���� �����¿� �ִϸ��̼� �׸��� ��ġ(Y��ǥ�� ����)
	char type;
	int frame_max;
	int delay_max;

	//�ڽ� ���� ��
	int frame;
	int delay;
	int y_point;

public:
	RECT ani_rect;

	void Set(int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
	void Process();
	void SetFrame(int vframe);
	void Reset();		//�ִϸ��̼��� ����������
	int GetXSize();
	int GetYSize();

	//�����¿� �ȱ� ����
	void SetWalk(int up_ypos, int down_ypos, int left_ypos, int right_ypos);
	void SetDirection(WalkDirection dir);

	//�׸���
	void Draw(char* dest, char* src, int x, int y, int frame=-1);
	void Draw(int dest, int src, int x, int y, int frame=-1);
	void DrawEx(char* dest, char* src, int x, int y, DWORD dwFlags, int frame=-1);
	void DrawEx(int dest, int src, int x, int y, DWORD dwFlags, int frame=-1);

	//������
	CAnimationData(int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
};

//�ִϸ��̼� ���� Ŭ����
class CAnimation
{
private:
	map<int, CAnimationData*> anidata;

public:
	void CreateAnimation(int id, int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
	void Process();	//�ִϸ��̼��� �����ϴ� �Լ�(�̰��� �������� ������ �������� ����)
	CAnimationData* GetAni(int id);

	//������ �Ҹ���
	CAnimation();
	~CAnimation();
};