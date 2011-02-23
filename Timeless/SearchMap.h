#pragma once

#include <windows.h>

//��ã�� Ŭ����
#define MOVE_DISABLE	9999

typedef struct
{
	int x;
	int y;
}MapPoint;

class CSearchMap
{
private:
	int* map;			//�˻��� ��
	int** roadmap;		//���� �˻��ϴ� ��
	int** distancemap;	//�Ÿ��� ��Ÿ���� ��
	MapPoint point;		//���� �繰�� �ִ� ��ġ

	int x_size;			//���� ũ��
	int y_size;

	void FindRoad(int** vmap, int x, int y, int expense, int move_max=-1);

public:
	CSearchMap(int* p_map, int x, int y);
	~CSearchMap();

	void SetStartPoint(MapPoint p, int move_max);	//������ġ ����
	void SetStartPoint(int x, int y, int move_max);
	void SetEndPoint(MapPoint p);					//�������� ����
	void SetEndPoint(int x, int y);

	BOOL MoveNextPointFast(int move);			//���� ����� ���� ��ġ�� �̵�
	BOOL MoveNextPoint(int move);				//���� ����� ���� ��ġ�� �ڿ������� �̵�
	BOOL MoveNextPointStupid(int move, int iq);	//�ణ ��û�ϰ� ���� ã�ư�
										//iq : 1~10����, ���� ���� ��û��

	int GetRoadMap(int x, int y);				//RoadMap�� ��������
	int GetDistanceMap(int x, int y);			//DistanceMap�� ��������
	int GetPointX();
	int GetPointY();
};