//길 찾기 메서드

#include "SearchMap.h"
#include "Global.h"

CSearchMap::CSearchMap(int* p_map, int x, int y)
{
	//지도 크기 정하기
	x_size=x;
	y_size=y;

	//이동력이 적힌 맵 주소 가져오기
	map=p_map;

	//이동력을 나타낼 맵 생성
	roadmap = new int*[x_size];
	for(int i=0; i<x_size; ++i)
		roadmap[i] = new int[y_size];

	//거리를 나타낼 맵 생성
	distancemap = new int*[x_size];
	for(int i=0; i<x_size; ++i)
		distancemap[i] = new int[y_size];

	point.x=point.y=0;
}

CSearchMap::~CSearchMap()
{
	//메모리 소거
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

	//맵 초기화
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			roadmap[i][j]=MOVE_DISABLE;

	//길 찾기
	FindRoad(roadmap, point.x, point.y, 0, move_max);
}

void CSearchMap::SetEndPoint(MapPoint p)
{
	SetEndPoint(p.x, p.y);
}

void CSearchMap::SetEndPoint(int x, int y)
{
	//맵 초기화
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			distancemap[i][j]=MOVE_DISABLE;

	//이동비용 검사
	FindRoad(distancemap, x, y, 0);
}

void CSearchMap::FindRoad(int** vmap, int x, int y, int expense, int move_max)
{
	vmap[x][y]=expense;
	if(move_max>=0 && expense>move_max)return;	//이동력 한계일 때는 더 이상 검사하지 않음

	if(x>0 && vmap[x-1][y]>expense+map[(x-1)+y*x_size])
		FindRoad(vmap, x-1, y, expense+map[(x-1)+y*x_size], move_max);	//왼쪽 검사
	if(x<x_size-1 && vmap[x+1][y]>expense+map[(x+1)+y*x_size])
		FindRoad(vmap, x+1, y, expense+map[(x+1)+y*x_size], move_max);	//오른쪽 검사
	if(y>0 && vmap[x][y-1]>expense+map[x+(y-1)*x_size])
		FindRoad(vmap, x, y-1, expense+map[x+(y-1)*x_size], move_max);	//위 검사
	if(y<y_size-1 && vmap[x][y+1]>expense+map[x+(y+1)*x_size])
		FindRoad(vmap, x, y+1, expense+map[x+(y+1)*x_size], move_max);	//아래 검사
}

BOOL CSearchMap::MoveNextPointFast(int move)
{
	//가장 좋은 길을 찾기
	int best_dir=-1, best_expense=distancemap[point.x][point.y];

	//왼쪽 검사
	if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
	{
		best_dir=0;
		best_expense=distancemap[point.x-1][point.y];
	}
	//오른쪽 검사
	if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
	{
		best_dir=1;
		best_expense=distancemap[point.x+1][point.y];
	}
	//위 검사
	if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
	{
		best_dir=2;
		best_expense=distancemap[point.x][point.y-1];
	}
	//아래 검사
	if(point.y<y_size-1 && distancemap[point.x][point.y+1]<best_expense && map[point.x+(point.y+1)*x_size]<=move)
	{
		best_dir=3;
		best_expense=distancemap[point.x][point.y+1];
	}

	//가장 가까운 곳으로 이동(아니면 가만히 있음)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//목적지에 도착하면 TRUE, 아니면 FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

BOOL CSearchMap::MoveNextPoint(int move)
{
	//가장 좋은 길을 찾기
	int best_dir=-1, best_expense=distancemap[point.x][point.y];

	//첫째로 검사하는 곳을 랜덤하게 정하기 위한 플래그
	//(우선순위가 정해져 있으면 패턴이 단순해서 보기에 안 좋다.)
	BOOL left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;

	while(!left_ok || !right_ok || !up_ok || !down_ok)
	{
		int ran=rand()%4;

		//왼쪽 검사
		if(ran==0)
		{
			if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
			{
				best_dir=0;
				best_expense=distancemap[point.x-1][point.y];
			}
			left_ok=TRUE;
		}
		//오른쪽 검사
		else if(ran==1)
		{
			if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
			{
				best_dir=1;
				best_expense=distancemap[point.x+1][point.y];
			}
			right_ok=TRUE;
		}
		//위 검사
		else if(ran==2)
		{
			if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
			{
				best_dir=2;
				best_expense=distancemap[point.x][point.y-1];
			}
			up_ok=TRUE;
		}
		//아래 검사
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

	//가장 가까운 곳으로 이동(아니면 가만히 있음)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//목적지에 도착하면 TRUE, 아니면 FALSE
	if(distancemap[point.x][point.y]==0)return TRUE;
		else return FALSE;
}

BOOL CSearchMap::MoveNextPointStupid(int move, int iq)
{
	iq=MaxMin(iq, 1, 10);	//아이큐는 1~10까지

	//가장 좋은 길을 찾기
	int best_dir=-1, best_expense=distancemap[point.x][point.y]+(10-iq);

	//멍청하기 때문에 모두 검사하지 않고 몇개를 랜덤으로 시도해 본 뒤 그 중에서 이동
	BOOL left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;
	int count=0;

	while(count<iq && (!left_ok || !right_ok || !up_ok || !down_ok))
	{
		int ran=rand()%4;

		//왼쪽 검사
		if(ran==0)
		{
			if(point.x>0 && distancemap[point.x-1][point.y]<best_expense && map[(point.x-1)+point.y*x_size]<=move)
			{
				best_dir=0;
				best_expense=distancemap[point.x-1][point.y];
			}
			left_ok=TRUE;
		}
		//오른쪽 검사
		else if(ran==1)
		{
			if(point.x<x_size-1 && distancemap[point.x+1][point.y]<best_expense && map[(point.x+1)+point.y*x_size]<=move)
			{
				best_dir=1;
				best_expense=distancemap[point.x+1][point.y];
			}
			right_ok=TRUE;
		}
		//위 검사
		else if(ran==2)
		{
			if(point.y>0 && distancemap[point.x][point.y-1]<best_expense && map[point.x+(point.y-1)*x_size]<=move)
			{
				best_dir=2;
				best_expense=distancemap[point.x][point.y-1];
			}
			up_ok=TRUE;
		}
		//아래 검사
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

	//가장 가까운 곳으로 이동(아니면 가만히 있음)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//목적지에 도착하면 TRUE, 아니면 FALSE
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