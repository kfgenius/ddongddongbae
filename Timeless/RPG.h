#include <stdio.h>

#include "GameMap.h"

//////////////////////////////
//게임 클래스
class CRPG : public CGameMap
{
private:
	//게임 흐름 제어
	void Control();		//키 입력
	void Process();		//게임 처리

	//생성자, 소멸자
	CRPG();
	~CRPG();
};
