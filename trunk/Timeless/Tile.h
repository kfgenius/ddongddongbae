#pragma once

class CTile
{
public:
	int ground;		//바닥
	int object;		//위의 물건
	int unit;		//서 있는 유닛
	int evnt;		//이벤트
	
	//길 찾기용
	int waste_move;		//필요 이동력
	int road;			//길을 검색하는 맵용
	int distance;		//거리를 나타내는 맵용

	CTile(void);
	~CTile(void);
};
