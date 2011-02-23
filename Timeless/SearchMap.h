#pragma once

#include <windows.h>

//길찾기 클래스
#define MOVE_DISABLE	9999

typedef struct
{
	int x;
	int y;
}MapPoint;

class CSearchMap
{
private:
	int* map;			//검색할 맵
	int** roadmap;		//길을 검색하는 맵
	int** distancemap;	//거리를 나타내는 맵
	MapPoint point;		//지금 사물이 있는 위치

	int x_size;			//맵의 크기
	int y_size;

	void FindRoad(int** vmap, int x, int y, int expense, int move_max=-1);

public:
	CSearchMap(int* p_map, int x, int y);
	~CSearchMap();

	void SetStartPoint(MapPoint p, int move_max);	//시작위치 지정
	void SetStartPoint(int x, int y, int move_max);
	void SetEndPoint(MapPoint p);					//도착지점 지정
	void SetEndPoint(int x, int y);

	BOOL MoveNextPointFast(int move);			//가장 가까운 다음 위치로 이동
	BOOL MoveNextPoint(int move);				//가장 가까운 다음 위치로 자연스럽게 이동
	BOOL MoveNextPointStupid(int move, int iq);	//약간 멍청하게 길을 찾아감
										//iq : 1~10까지, 낮을 수록 멍청함

	int GetRoadMap(int x, int y);				//RoadMap값 가져오기
	int GetDistanceMap(int x, int y);			//DistanceMap값 가져오기
	int GetPointX();
	int GetPointY();
};