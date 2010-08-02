#include "Dungeon.h"

#define KEY_DELAY	10	//키 연속 입력 딜레이

#define TILE_WIDTH		40
#define TILE_HEIGHT		40

#define MAIN_MAP	0

enum
{
	tile_wall,
	tile_field,
	tile_down_stair,
	tile_warp,
	tile_red_box,
	tile_gold_box,
	tile_hero,
	tile_enemy
};

//맵 정보
unsigned char main_map[12][16] =
{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 4, 1, 4, 1, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 2, 1, 1, 1, 3, 1, 1, 1, 2, 0, 0, 0, 0},
	{0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 1, 4, 1, 5, 1, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

CDungeon::CDungeon(void)
{
	//맵 초기화
	map_id = MAIN_MAP;

	//주인공 초기화
	hero_x = 7;
	hero_y = 5;
	SetRect(&hero_rect, tile_hero * TILE_WIDTH, 0, (tile_hero + 1) * TILE_WIDTH, TILE_HEIGHT);

	//대화창 초기화
	m_dlg.SetDlg(5, 395, 630, 4);
	m_dlg.SetTextColor(255, 0, 0);
	m_dlg.MakeDlgBox("tmp_dlg_box");
	m_dlg.SetOpacity(0.8f);
}

CDungeon::~CDungeon(void)
{
}

//주인공 이동 시키기
bool CDungeon::HeroMove(int x, int y)
{
	int next_hero_x = hero_x + x;
	int next_hero_y = hero_y + y;

	//이동
	if(next_hero_x >= 0 && next_hero_x < SCREEN_MAP_WIDTH
		&& next_hero_y >= 0 && next_hero_y < SCREEN_MAP_HEIGHT
		&& main_map[next_hero_y][next_hero_x] != tile_wall)
	{
		hero_x = next_hero_x;
		hero_y = next_hero_y;

		return true;
	}
	else
	{
		return false;
	}
}

//이동 이벤트
void CDungeon::HeroMoveEvent()
{
	if(main_map[hero_y][hero_x] == tile_red_box)
	{
		m_dlg.Print("우왕 굳!");
		
		main_map[hero_y][hero_x] = tile_field;
	}
	else if(main_map[hero_y][hero_x] == tile_gold_box)
	{
		m_dlg.Print("무생물");
		
		main_map[hero_y][hero_x] = tile_field;
	}
}

//조작
void CDungeon::Control()
{
	bool hero_move = false;

	//좌우 이동
	if(GetKey(vkey_left, KEY_DELAY))
	{
		hero_move |= HeroMove(-1, 0);
	}
	else if(GetKey(vkey_right, KEY_DELAY))
	{
		hero_move |= HeroMove(1, 0);
	}
	
	//상하 이동
	if(GetKey(vkey_up, KEY_DELAY))
	{
		hero_move |= HeroMove(0, -1);
	}
	else if(GetKey(vkey_down, KEY_DELAY))
	{
		hero_move |= HeroMove(0, 1);
	}

	//이동 후 이벤트
	if(hero_move)
	{
		HeroMoveEvent();
	}
}

//처리
void CDungeon::Process()
{
}

//보여주기
void CDungeon::Render()
{
	//출력
	for(int y = 0; y < 12; ++y)
	for(int x = 0; x < 16; ++x)
	{
		RECT rect;
		SetRect(&rect, main_map[y][x] * TILE_WIDTH, 0, (main_map[y][x] + 1) * TILE_WIDTH, TILE_HEIGHT);
		jdd->DrawPicture(jdd->GetBackBuffer(), "SPRITE", x * TILE_WIDTH, y * TILE_HEIGHT, &rect);

		if(hero_x == x && hero_y == y)
		{
			jdd->DrawPicture(jdd->GetBackBuffer(), "SPRITE", x * TILE_WIDTH, y * TILE_HEIGHT, &hero_rect);
		}
	}
}