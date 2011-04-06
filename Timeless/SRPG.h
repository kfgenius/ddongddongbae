#pragma once

#include "GameProcess.h"
#include "RPG.h"

//////////////////////////////
//게임 클래스
class CSRPG : public CRPG
{
private:
	CCommand* command;	//명령

	void Control();		//조작

public:
	//게임 흐름 제어
	void Process();		//게임 처리

	//유닛 관련
	void SetActiveUnit(int id);		//활성화 유닛 고르기

	//이동 속도
	int GetMoveSpeed(int x, int y);
	
	//이동 가능 영역 보기
	void ShowMoveAble();

	//길찾기 관련
	void SetStartPoint(int x, int y, int move_max);
	void SetEndPoint(int x, int y);
	void FindRoad(int x, int y, int expense, int move_max=99);
	bool MoveNextPointFast(int move);
	bool MoveNextPoint(int move);
	bool MoveNextPointStupid(int move, int iq);

	//생성자, 소멸자
	CSRPG(char* map_name, char* tile1_name, char* tile2_name);
	~CSRPG();
};
