#include "Dungeon.h"

#define KEY_DELAY	10	//Ű ���� �Է� ������

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

//�� ����
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
	//�� �ʱ�ȭ
	map_id = MAIN_MAP;

	//���ΰ� �ʱ�ȭ
	hero_x = 7;
	hero_y = 5;
	SetRect(&hero_rect, tile_hero * TILE_WIDTH, 0, (tile_hero + 1) * TILE_WIDTH, TILE_HEIGHT);

	//��ȭâ �ʱ�ȭ
	m_dlg.SetDlg(5, 395, 630, 4);
	m_dlg.SetTextColor(255, 0, 0);
	m_dlg.MakeDlgBox("tmp_dlg_box");
	m_dlg.SetOpacity(0.8f);
}

CDungeon::~CDungeon(void)
{
}

//���ΰ� �̵� ��Ű��
bool CDungeon::HeroMove(int x, int y)
{
	int next_hero_x = hero_x + x;
	int next_hero_y = hero_y + y;

	//�̵�
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

//�̵� �̺�Ʈ
void CDungeon::HeroMoveEvent()
{
	if(main_map[hero_y][hero_x] == tile_red_box)
	{
		m_dlg.Print("��� ��!");
		
		main_map[hero_y][hero_x] = tile_field;
	}
	else if(main_map[hero_y][hero_x] == tile_gold_box)
	{
		m_dlg.Print("������");
		
		main_map[hero_y][hero_x] = tile_field;
	}
}

//����
void CDungeon::Control()
{
	bool hero_move = false;

	//�¿� �̵�
	if(GetKey(vkey_left, KEY_DELAY))
	{
		hero_move |= HeroMove(-1, 0);
	}
	else if(GetKey(vkey_right, KEY_DELAY))
	{
		hero_move |= HeroMove(1, 0);
	}
	
	//���� �̵�
	if(GetKey(vkey_up, KEY_DELAY))
	{
		hero_move |= HeroMove(0, -1);
	}
	else if(GetKey(vkey_down, KEY_DELAY))
	{
		hero_move |= HeroMove(0, 1);
	}

	//�̵� �� �̺�Ʈ
	if(hero_move)
	{
		HeroMoveEvent();
	}
}

//ó��
void CDungeon::Process()
{
}

//�����ֱ�
void CDungeon::Render()
{
	//���
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