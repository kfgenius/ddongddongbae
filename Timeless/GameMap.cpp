#include "gamemap.h"
#include "donglib.h"

//�� ����
#define UNIT_HEIGHT	40
#define UNIT_PY		8	//������ ���ΰ� 40�̹Ƿ� 8�ȼ� ���� ����
#define FOCUS_MOVE_SPEED	4

//�׸� �̸�
#define PIC_MAP		"_Ground"
#define PIC_TILE1	"_tile1"
#define PIC_TILE2	"_tile2"

//�ִ� ��ȣ
#define ANI_TILE		UNIT_TYPE_MAX

//�̵� �Ұ�
#define MOVE_DISABLE	9999

///////////////////////////////////////////////////////////
// CGameMap �޼ҵ�

CGameMap::CGameMap(char* map_name, char* tile1_name, char* tile2_name)
{
	//�ִϸ��̼� �ʱ�ȭ
	ani.CreateAnimation(ANI_TILE, TILE_SOURCE_SIZE, TILE_SOURCE_SIZE);

	//Ÿ��, ������Ʈ, ����
	JPictureInfo jpi, tpi;
	jpi.SetColorKey(JColor(0,0,255));
	jdd->LoadPicture(PIC_TILE1, tile1_name, &jpi, true, &tpi);
	jdd->LoadPicture(PIC_TILE2, tile2_name, &jpi, true);	//2�� Ÿ��
	CreateUnit();

	//Ÿ�� �� ���
	tile_type_max = Min(tpi.GetWidth() / TILE_SOURCE_SIZE, TILE_TYPE_MAX);

	//�� ���� �ε�
	strcpy(this->map_name, map_name);

	FILE *fp;
	char map_file[80];
	sprintf(map_file, "%s.map",map_name);
	if(!(fp=fopen(map_file, "rb")))return false;

	char data_type[12];
	fread(&data_type,sizeof(char),11,fp);
	data_type[11]=NULL;

	//�ް��� ���� �ƴ�
	if(strcmp(data_type,"HYUGARU2MAP")!=0)
	{
		fclose(fp);
		OutputDebugString("�������� �������� �ƴմϴ�.");
		return;
	}

	//�� ũ��
	fread(&x_size, sizeof(char), 1, fp);
	fread(&y_size, sizeof(char), 1, fp);

	//1F ������
	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	{
		fread(&map[i][j].ground, sizeof(char), 1, fp);
	}

	//2F ������
	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	{
		fread(&map[i][j].object, sizeof(char), 1, fp);
	}

	//���� ������
	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	{
		fread(&map[i][j].unit, sizeof(char), 1, fp);
	}

	//�̺�Ʈ �� ������
	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	{
		fread(&map[i][j].event_no, sizeof(char), 1, fp);
	}

	//���ΰ� ������ �ʱ�ȭ
	unit[HERO].Set(HERO, party_ally, TILE_SIZE);
	ani.CreateAnimation(HERO, 32, 40, ani_exchange, 3, 20);
	ani.GetAni(HERO)->SetWalk(0, 80, 120, 40);

	//���� ������ �ʱ�ȭ
	unit_max = HERO + 1;

	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	if(map[i][j].unit != 0xff)
	{
		//������ ������ ��ҿ� ���� ��ġ
		unit[unit_max].Set(map[i][j].unit, party_enemy, TILE_SIZE);
		unit[unit_max].SetPos(i, j);
		unit[unit_max].SetDir(/*rand()%4*/DIR_DOWN);
		
		//���� ������ �Ѱ��� �� ������ ID�� ��ü
		map[i][j].unit = unit_max;
		unit[unit_max].SetEventNo(map[i][j].event_no);
		map[i][j].event_no = 0xff;

		//�ִϸ��̼� �ʱ�ȭ
		ani.CreateAnimation(unit_max, 32, 40, ani_exchange, 3, 20);
		ani.GetAni(unit_max)->SetWalk(40, 120, 0, 80);
		ani.GetAni(unit_max)->SetDirection((WalkDirection)unit[unit_max].GetDir());

		++unit_max;
	}

	//�� �̸� �׸���
	DrawGround();
	
	fclose(fp);

	//�� �Ӽ� �ҷ�����
	bool read_success = false;
	if(fp=fopen("MAP\\map_attr.sav", "rb"))
	{
		//�ĺ���
		char head[9];
		fread(head, sizeof(char), 8, fp);
		head[8] = NULL;
		if(strcmp(head, "MAPATTR0") == 0)
		{
			//Ÿ�� �Ӽ���
			for(int i = 0; i < 2; ++i)
			{
				fread(tile_mov[i], sizeof(int), TILE_TYPE_MAX, fp);
				fread(tile_attr[i], sizeof(int), TILE_TYPE_MAX, fp);
				fread(tile_damage[i], sizeof(int), TILE_TYPE_MAX, fp);
			}
			//���� �Ӽ���
			fread(unitdata, sizeof(CUnitData), TILE_MAX, fp);

			read_success = true;
		}

		//�������� �̵������� ��ȯ
		for(int j = 0; j < 2; ++j)
		for(int i = 0; i < TILE_MAX; ++i)
		{
			if(tile_mov[j][i] == 0)
			{
				tile_mov[j][i] = 99;
			}
			else
			{
				tile_mov[j][i] = tile_mov[j][i];
			}
		}

		fclose(fp);
	}

	//�б� �����ϸ� �ʱ�ȭ
	if(!read_success)
	{
		for(int j = 0; j < 2; ++j)
		for(int i = 0; i < TILE_MAX; ++i)
		{
			tile_mov[j][i] = tile_attr[j][i] = tile_damage[j][i] = 99;
		}
	}

	//�̵��� �� ����
	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	{
		if(map[i][j].object != 0xff)
		{
			map[i][j].move = tile_mov[1][map[i][j].object];
		}
		else
		{
			map[i][j].move = tile_mov[0][map[i][j].ground];
		}
		
		//�̵��� 0�̸� �̵� �Ұ� ����
		if(map[i][j].move == 99)
		{
			map[i][j].move = MOVE_DISABLE;
		}
	}
}

CGameMap::~CGameMap()
{
	jdd->DeleteSurface(PIC_TILE1);
	jdd->DeleteSurface(PIC_TILE2);
}

//���� ����
void CGameMap::CreateUnit()
{
	//���Ͽ��� �б�
	int file_no = 0;

	JPictureInfo pi;
	pi.SetColorKey(JColor(0,0,255));

	FILE *fp;
	while(1)
	{
		char src_file[80];
		sprintf(src_file, "User\\Unit%d.png", file_no);

		if(fp = fopen(src_file, "rb"))
		{
			fclose(fp);

			//�ش� ���� ���Ͽ��� ���� ��� �ϳ��� ������ ����
			char unit_name[20];
			sprintf(unit_name, "Unit%d", file_no);
			jdd->LoadPicture(unit_name, src_file, &pi, true);

			++file_no;
		}
		else break;
	}
}

//���� �̸� �׸��� �Լ�(�Ź� �׸��� �����Ƿ�)
void CGameMap::DrawGround()
{
	//������ �����
	jdd->DeleteSurface(PIC_MAP);

	//���ο� �� ����
	JPictureInfo mpi;
	mpi.SetWidth(x_size * TILE_SIZE);
	mpi.SetHeight(y_size * TILE_SIZE);
	jdd->CreateSurface(PIC_MAP, &mpi, true);

	//Ÿ��
	RECT tile_rect;
	for(int k = 0; k < tile_type_max; ++k)
	{
		SetRect(&tile_rect, k * TILE_SOURCE_SIZE, 0, (k + 1) * TILE_SOURCE_SIZE, TILE_SOURCE_SIZE);

		for(int j=0; j<y_size; ++j)
		for(int i=0; i<x_size; ++i)
		{
			if(map[i][j].ground == k)
			{
				ani.GetAni(ANI_TILE)->Draw(PIC_MAP, PIC_TILE1, i * TILE_SIZE - ((TILE_SOURCE_SIZE - TILE_SIZE) / 2), j * TILE_SIZE - ((TILE_SOURCE_SIZE - TILE_SIZE) / 2), k);
			}
		}
	}

	//������Ʈ
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	{
		if(map[i][j].object != 0xff)
		{
			SetRect(&tile_rect, map[i][j].object * TILE_SIZE, 8, (map[i][j].object + 1) * TILE_SIZE, UNIT_HEIGHT);
			jdd->DrawPicture(PIC_MAP, PIC_TILE2, i * TILE_SIZE, j * TILE_SIZE, &tile_rect);
		}
	}
}

//��ũ�� �ʱ�ȭ
void CGameMap::InitScroll(int unit_id)
{
	next_scroll_x = scroll_x = MaxMin(unit[unit_id].GetRealX() - (SCREEN_WIDTH / 2), 0, x_size * TILE_SIZE - SCREEN_WIDTH);
	next_scroll_y = scroll_y = MaxMin(unit[unit_id].GetRealY() - (SCREEN_HEIGHT / 2), 0, y_size * TILE_SIZE - SCREEN_HEIGHT);
}

//��Ŀ��
void CGameMap::Focus(int unit_id)
{
	//X��
	int ux = MaxMin(unit[unit_id].GetRealX() - (SCREEN_WIDTH / 2), 0, x_size * TILE_SIZE - SCREEN_WIDTH);
	if(scroll_x < ux)
	{
		scroll_x = Min(scroll_x + FOCUS_MOVE_SPEED, ux);
	}
	else if(scroll_x > ux)
	{
		scroll_x = Max(scroll_x - FOCUS_MOVE_SPEED, ux);
	}

	//Y��
	int uy = MaxMin(unit[unit_id].GetRealY() - (SCREEN_HEIGHT / 2), 0, y_size * TILE_SIZE - SCREEN_HEIGHT);
	if(scroll_y < uy)
	{
		scroll_y = Min(scroll_y + FOCUS_MOVE_SPEED, uy);
	}
	else if(scroll_y > uy)
	{
		scroll_y = Max(scroll_y - FOCUS_MOVE_SPEED, uy);
	}
}

//1�� Ÿ�� ���
void CGameMap::Draw1F()
{
	RECT screen_rect;
	SetRect(&screen_rect, scroll_x, scroll_y, scroll_x + SCREEN_WIDTH, scroll_y + SCREEN_HEIGHT);
	jdd->DrawPicture(backbuffer, PIC_MAP, 0, 0, &screen_rect);
}

//2�� ������Ʈ ���
void CGameMap::Draw2F()
{
	//���� ����
	int start_x = Max((scroll_x / TILE_SIZE) - 1, 0);
	int start_y = Max((scroll_y / TILE_SIZE) - 1, 0);
	int end_x = Min(start_x + (SCREEN_WIDTH / TILE_SIZE) + 1, x_size - 1);
	int end_y = Min(start_y + (SCREEN_HEIGHT / TILE_SIZE) + 1, y_size - 1);

	//2�� ���̾�
	for(int j = start_y; j <= end_y; ++j)
	for(int i = start_x; i <= end_x; ++i)
	{
		int px = i * TILE_SIZE - scroll_x;
		int py = j * TILE_SIZE - scroll_y;

		//2��
		if(map[i][j].object!=0xff)
		{
			RECT tile_rect;
			SetRect(&tile_rect, map[i][j].object*TILE_SIZE, 0, (map[i][j].object+1)*TILE_SIZE, 8);
			jdd->DrawPicture(backbuffer, PIC_TILE2, px, py-UNIT_PY, &tile_rect);
		}

		//�Ϲ� ����
		if(map[i][j].unit!=0xff)
		{
			int p = map[i][j].unit;
			if(unit[p].move_bonus < SKY_MOVE)
			{
				ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px + unit[p].GetPX(), py + unit[p].GetPY() - UNIT_PY);
			}
		}
	}
}

void CGameMap::Draw3F()
{
	//���� ����
	int start_x = Max((scroll_x / TILE_SIZE) - 1, 0);
	int start_y = Max((scroll_y / TILE_SIZE) - 1, 0);
	int end_x = Min(start_x + (SCREEN_WIDTH / TILE_SIZE) + 1, x_size - 1);
	int end_y = Min(start_y + (SCREEN_HEIGHT / TILE_SIZE) + 1, y_size - 1);

	//3�� ���̾�
	for(int j = start_y; j <= end_y; ++j)
	for(int i = start_x; i <= end_x; ++i)
	{
		int px = i * TILE_SIZE - scroll_x;
		int py = j * TILE_SIZE - scroll_y;

		//���� ����
		if(map[i][j].unit != 0xff)
		{
			int p = map[i][j].unit;
			if(unit[p].move_bonus >= SKY_MOVE)
			{
				ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
			}
		}
		
		//Ȱ��ȭ�� ����
		/*if(active_unit >= 0 && i == unit[active_unit].GetX() && j == unit[active_unit].GetY())
		{
			int p = active_unit;
			ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
		}*/
	}
}

//�� ������ ���
TileData GetMapData(int x, int y)
{
	return &map[x][y];
}

//���ΰ� ��ġ ����
void CGameMap::SetHeroXY(int x, int y)
{
	x = MaxMin(x, 0, x_size-1);
	y = MaxMin(y, 0, y_size-1);
	unit[HERO].SetPos(x, y);	
	map[x][y].unit = HERO;
}

//���ΰ� ��ġ�� �̺�Ʈ ��ȣ�� �ڵ� ��ġ
void CGameMap::SetAutoHeroXY(int ev)
{
	//�ϴ� ���ΰ��� �� �ۿ� ��
	unit[HERO].SetPos(-1, 0);

	//�̺�Ʈ �� ������
	bool quit=false;
	for(int j=0; j<y_size && !quit; ++j)
	for(int i=0; i<x_size && !quit; ++i)
	if(map[i][j].event_no == ev)	//�ش� �̺�Ʈ ��ġ�� ���ΰ� ��ġ
	{
		unit[HERO].SetPos(i, j);
		map[i][j].unit = HERO;	//���� �ʿ� ���ΰ� ��ġ
		map[i][j].event_no = 0xff;
		quit=true;
	}

	if(unit[HERO].GetX() == -1)unit[HERO].SetPos(0, 0);
}

CUnit* CGameMap::GetUnit(int id)
{
	return &unit[id];
}

//�ش� ��ġ ���� ã��
int CGameMap::FindUnit(int x, int y)
{
	for(int i = 0; i < unit_max; ++i)
	{
		if(unit[i].GetX()==x && unit[i].GetY()==y)
		{
			return i;
		}
	}

	return -1;
}
