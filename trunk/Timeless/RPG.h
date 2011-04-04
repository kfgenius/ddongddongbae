#include "GameProcess.h"
#include "GameMap.h"

//////////////////////////////
//게임 클래스
class CRPG : public CGameProcess
{
private:
	//맵 데이터
	CGameMap* m_game_map;

	//게임 흐름 제어
	void Control();		//키 입력
	void Process();		//게임 처리

	//이동 속도 얻기
	int GetMoveSpeed(int x, int y);

public:
	//생성자, 소멸자
	CRPG();
	~CRPG();
};
