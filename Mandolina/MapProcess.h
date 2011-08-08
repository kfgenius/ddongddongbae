#pragma once
#include "gameprocess.h"
#include "GameMap.h"

class CMapProcess :
	public CGameProcess
{
private:
	CGameMap map;

public:
	CMapProcess(void);
	~CMapProcess(void);
};
