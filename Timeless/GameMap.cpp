#include "GameMap.h"

#include <stdio.h>

CGameMap::CGameMap(void)
{
	tile = new CTile*[20];
	for(int x = 0; x < 20; ++x)
	{
		tile[x] = new CTile[15];
	}

	//타일 값 초기화
	for(int y = 0; y < 15; ++y)
	{
		for(int x = 0; x < 20; ++x)
		{
			tile[x][y].ground = 1;
		}
	}

	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0, 0, 255));
	jdd->LoadPicture("Tile1F", "DATA\\tile1.gif", &jpi, true, NULL);
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

	jdd->DeleteSurface("Tile1F");
}

void CGameMap::Control()
{
}

void CGameMap::Process()
{
}

void CGameMap::Render()
{
	for(int y = 0; y < 15; ++y)
	{
		for(int x = 0; x < 20; ++x)
		{
			RECT src_rect;
			SetRect(&src_rect, tile[x][y].ground * 48, 0, (tile[x][y].ground + 1) * 48, 48);

			jdd->DrawPicture(backbuffer, "Tile1F", (x * 32) - 8, (y * 32) - 8, &src_rect);
		}
	}
}
