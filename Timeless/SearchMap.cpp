//�� ã�� �޼���

#include "SearchMap.h"
#include "Global.h"

CSearchMap::CSearchMap(int* p_map, int x, int y)
{
	//���� ũ�� ���ϱ�
	x_size=x;
	y_size=y;

	//�̵����� ���� �� �ּ� ��������
	map=p_map;

	//�̵����� ��Ÿ�� �� ����
	roadmap = new int*[x_size];
	for(int i=0; i<x_size; ++i)
		roadmap[i] = new int[y_size];

	//�Ÿ��� ��Ÿ�� �� ����
	distancemap = new int*[x_size];
	for(int i=0; i<x_size; ++i)
		distancemap[i] = new int[y_size];

	point.x=point.y=0;
}

CSearchMap::~CSearchMap()
{
	//�޸� �Ұ�
	if(roadmap)
	{
		for(int i=0; i<x_size; ++i)
		{
			delete [] roadmap[i];
			roadmap[i]=NULL;
		}
		delete [] roadmap;
		roadmap=NULL;
	}

	if(distancemap)
	{
		for(int i=0; i<x_size; ++i)
		{
			delete [] distancemap[i];
			distancemap[i]=NULL;
		}
		delete [] distancemap;
		distancemap=NULL;
	}
}

void CSearchMap::SetStartPoint(MapPoint p, int move_max)
{
	SetStartPoint(p.x, p.y, move_max);
}

void CSearchMap::SetStartPoint(int x, int y, int move_max)
{
	point.x=MaxMin(x, 0, x_size-1);
	point.y=MaxMin(y, 0, y_size-1);

	//�� �ʱ�ȭ
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			roadmap[i][j]=MOVE_DISABLE;

	//�� ã��
	FindRoad(roadmap, point.x, point.y, 0, move_max);
}

void CSearchMap::SetEndPoint(MapPoint p)
{
	SetEndPoint(p.x, p.y);
}

void CSearchMap::SetEndPoint(int x, int y)
{
	//�� �ʱ�ȭ
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			distancemap[i][j]=MOVE_DISABLE;

	//�̵���� �˻�
	FindRoad(distancemap, x, y, 0);
}

void CSearchMap::FindRoad(int** vmap, int x, int y, int expense, int move_max)
{
	vmap[x][y]=expense;
	if(move_max>=0 && expense>move_max)return;	//�̵��� �Ѱ��� ���� �� �̻� �˻����� ����

	if(x>0 && vmap[x-1][y]>expense+map[(x-1)+y*x_size])
		FindRoad(vmap, x-1, y, expense+map[(x-1)+y*x_size], move_max);	//���� �˻�
	if(x<x_size-1 && vmap[x+1][y]>expense+map[(x+1)+y*x_size])
		FindRoad(vmap, x+1, y, expense+map[(x+1)+y*x_size], move_max);	//������ �˻�
	if(y>0 && vmap[x][y-1]>expense+map[x+(y-1)*x_size])
		FindRoad(vmap, x, y-1, expense+map[x+(y-1)*x_size], move_max);	//�� �˻�
	if(y<y_size-1 && vmap[x][y+1]>expense+map[x+(y+1)*x_size])
		FindRoad(vmap, x, y+1, expense+map[x+(y+1)*x_size], move_max);	//�Ʒ� �˻�
}

BOOL CSearchMap::MoveNextPointFast(int move)
{
	//���� ���� ���� ã��
	int best_dir=-1, best_expense=distancemap[point.x][point.y];

	//���� �˻�
	if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
	{
		best_dir=0;
		best_expense=distancemap[point.x-1][point.y];
	}
	//������ �˻�
	if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
	{
		best_dir=1;
		best_expense=distancemap[point.x+1][point.y];
	}
	//�� �˻�
	if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
	{
		best_dir=2;
		best_expense=distancemap[point.x][point.y-1];
	}
	//�Ʒ� �˻�
	if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
	{
		best_dir=3;
		best_expense=distancemap[point.x][point.y+1];
	}

	//���� ����� ������ �̵�(�ƴϸ� ������ ����)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//�������� �����ϸ� TRUE, �ƴϸ� FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

BOOL CSearchMap::MoveNextPoint(int move)
{
	//���� ���� ���� ã��
	int best_dir=-1, best_expense=distancemap[point.x][point.y];

	//ù°�� �˻��ϴ� ���� �����ϰ� ���ϱ� ���� �÷���
	//(�켱������ ������ ������ ������ �ܼ��ؼ� ���⿡ �� ����.)
	BOOL left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;

	while(!left_ok || !right_ok || !up_ok || !down_ok)
	{
		int ran=rand()%4;

		//���� �˻�
		if(ran==0)
		{
			if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
			{
				best_dir=0;
				best_expense=distancemap[point.x-1][point.y];
			}
			left_ok=TRUE;
		}
		//������ �˻�
		else if(ran==1)
		{
			if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
			{
				best_dir=1;
				best_expense=distancemap[point.x+1][point.y];
			}
			right_ok=TRUE;
		}
		//�� �˻�
		else if(ran==2)
		{
			if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
			{
				best_dir=2;
				best_expense=distancemap[point.x][point.y-1];
			}
			up_ok=TRUE;
		}
		//�Ʒ� �˻�
		else if(ran==3)
		{
			if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
			{
				best_dir=3;
				best_expense=distancemap[point.x][point.y+1];
			}
			down_ok=TRUE;
		}
	}

	//���� ����� ������ �̵�(�ƴϸ� ������ ����)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//�������� �����ϸ� TRUE, �ƴϸ� FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

BOOL CSearchMap::MoveNextPointStupid(int move, int iq)
{
	iq=MaxMin(iq, 1, 10);	//����ť�� 1~10����

	//���� ���� ���� ã��
	int best_dir=-1, best_expense=distancemap[point.x][point.y]+(10-iq);

	//��û�ϱ� ������ ��� �˻����� �ʰ� ��� �������� �õ��� �� �� �� �߿��� �̵�
	BOOL left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;
	int count=0;

	while(count<iq && (!left_ok || !right_ok || !up_ok || !down_ok))
	{
		int ran=rand()%4;

		//���� �˻�
		if(ran==0)
		{
			if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
			{
				best_dir=0;
				best_expense=distancemap[point.x-1][point.y];
			}
			left_ok=TRUE;
		}
		//������ �˻�
		else if(ran==1)
		{
			if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
			{
				best_dir=1;
				best_expense=distancemap[point.x+1][point.y];
			}
			right_ok=TRUE;
		}
		//�� �˻�
		else if(ran==2)
		{
			if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
			{
				best_dir=2;
				best_expense=distancemap[point.x][point.y-1];
			}
			up_ok=TRUE;
		}
		//�Ʒ� �˻�
		else if(ran==3)
		{
			if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
			{
				best_dir=3;
				best_expense=distancemap[point.x][point.y+1];
			}
			down_ok=TRUE;
		}

		++count;
	}

	//���� ����� ������ �̵�(�ƴϸ� ������ ����)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//�������� �����ϸ� TRUE, �ƴϸ� FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

int CSearchMap::GetRoadMap(int x, int y)
{
	return roadmap[x][y];
}

int CSearchMap::GetDistanceMap(int x, int y)
{
	return distancemap[x][y];
}

int CSearchMap::GetPointX()
{
	return point.x;
}

int CSearchMap::GetPointY()
{
	return point.y;
}