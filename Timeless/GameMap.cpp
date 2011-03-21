#include "GameMap.h"
#include "donglib.h"

#include <stdio.h>

CGameMap::CGameMap(void)
{
	tile = new CTile*[20];
	for(int i = 0; i < 15; ++i)
	{
		tile[i] = new CTile;
		tile[i]->ground = 1;
	}
}

CGameMap::~CGameMap(void)
{
	for(int i = 0; i < 20; ++i)
	{
		delete [] tile[i];
		tile[i] = NULL;
	}

	delete [] tile;
	tile = NULL;
}

void CGameMap::Draw()
{
	jdd->DrawPicture(backbuffer, "Tile");
	jdd->DrawPictrue();
}
