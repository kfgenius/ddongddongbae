#pragma once

#include "Tile.h"

class CGameMap
{
private:
	CTile** tile;
	void Draw();

public:
	CGameMap(void);
	~CGameMap(void);
};
