#pragma once

#include "Tile.h"
#include "GameProcess.h"

class CGameMap : public CGameProcess
{
private:
	CTile** tile;

public:
	void Control();
	void Process();
	void Render();

	CGameMap(void);
	~CGameMap(void);
};
