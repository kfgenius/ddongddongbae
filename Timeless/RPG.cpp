#include "rpg.h"
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

//�̺�Ʈ ����
#define EVENT_NONE		-1
#define EVENT_GO_OUT	999

////////////////////////////////////////////////////////////
// CRPG �޼ҵ�

CRPG::CRPG(char* map_name, char* tile1_name, char* tile2_name)
{
	//�ִϸ��̼� �ʱ�ȭ
	ani.CreateAnimation(ANI_TILE, TILE_SOURCE_SIZE, TILE_SOURCE_SIZE);

	//Ÿ��, ������Ʈ, ����
	JPictureInfo jpi, tpi;
	jpi.SetColorKey(JColor(0,0,255));
	jdd->LoadPicture(PIC_TILE1, tile1_name, &jpi, true);
	jdd->LoadPicture(PIC_TILE2, tile2_name, &jpi, true);	//2�� Ÿ��
	CreateUnit();

	//Ÿ�� �� ���
	jdd->GetPictureInfo(PIC_TILE1, &tpi);
	tile_type_max = Min(tpi.GetWidth() / TILE_SOURCE_SIZE, TILE_TYPE_MAX);

	//�� ���� �ε�
	strcpy(this->map_name, map_name);

	FILE *fp;
	char map_file[80];
	sprintf(map_file, "%s.map",map_name);
	if(!(fp=fopen(map_file, "rb")))
	{
		char msg[80];
		sprintf(msg, "%s�� ã�� �� �����ϴ�.", map_file);
		OutputDebugString(msg);
		exit(1);
	}

	char data_type[12];
	fread(&data_type,sizeof(char),11,fp);
	data_type[11]=NULL;

	//�ް��� ���� �ƴ�
	if(strcmp(data_type,"HYUGARU2MAP")!=0)
	{
		fclose(fp);
		char msg[80];
		sprintf(msg, "%s�� �������� �������� �ƴմϴ�.", map_file);
		OutputDebugString(msg);
		exit(1);
	}

	//�� ũ��
	fread(&x_size, sizeof(char), 1, fp);
	fread(&y_size, sizeof(char), 1, fp);

	//1F ������
	for(int j = 0; j < y_size; ++j)
	{
		for(int i = 0; i < x_size; ++i)
		{
			fread(&map[i][j].ground, sizeof(char), 1, fp);
		}
	}

	//2F ������
	for(int j = 0; j < y_size; ++j)
	{
		for(int i = 0; i < x_size; ++i)
		{
			fread(&map[i][j].object, sizeof(char), 1, fp);
		}
	}

	//���� ������
	for(int j = 0; j < y_size; ++j)
	{
		for(int i = 0; i < x_size; ++i)
		{
			fread(&map[i][j].unit, sizeof(char), 1, fp);
		}
	}

	//�̺�Ʈ �� ������
	for(int j = 0; j < y_size; ++j)
	{
		for(int i = 0; i < x_size; ++i)
		{
			fread(&map[i][j].event_no, sizeof(char), 1, fp);
		}
	}

	//���ΰ� ������ �ʱ�ȭ
	unit[HERO].Set(HERO, party_ally, TILE_SIZE);
	ani.CreateAnimation(HERO, 32, 40, ani_exchange, 3, 20);
	ani.GetAni(HERO)->SetWalk(0, 80, 120, 40);

	//���� ������ �ʱ�ȭ
	unit_max = HERO + 1;

	for(int j = 0; j < y_size; ++j)
	{
		for(int i = 0; i < x_size; ++i)
		{
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
				ani.GetAni(unit_max)->SetWalk(0, 80, 120, 40);
				ani.GetAni(unit_max)->SetDirection((WalkDirection)unit[unit_max].GetDir());

				++unit_max;
			}
		}
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
			fread(unitdata, sizeof(CUnitData), TILE_TYPE_MAX, fp);

			read_success = true;
		}

		//�������� �̵������� ��ȯ
		for(int j = 0; j < 2; ++j)
		{
			for(int i = 0; i < TILE_TYPE_MAX; ++i)
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
		}

		fclose(fp);
	}

	//�б� �����ϸ� �ʱ�ȭ
	if(!read_success)
	{
		for(int j = 0; j < 2; ++j)
		{
			for(int i = 0; i < TILE_TYPE_MAX; ++i)
			{
				tile_mov[j][i] = tile_attr[j][i] = tile_damage[j][i] = 99;
			}
		}
	}

	//�̵��� �� ����
	for(int j = 0; j < y_size; ++j)
	{
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

	//������ �� ���̵���
	first_fade_in = TRUE;

	//���ΰ� ��ġ �ʱ�ȭ
	SetAutoHeroXY(0);
	InitScroll(HERO);

	//��ũ��Ʈ �ʱ�ȭ
	m_script = new CScript(map_name);
	m_script->SetPage(0);
}

CRPG::~CRPG()
{
	jdd->DeleteSurface(PIC_TILE1);
	jdd->DeleteSurface(PIC_TILE2);
}

//���� ����
void CRPG::CreateUnit()
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

//���� �̸� �׸��� �Լ�
void CRPG::DrawGround()
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
		{
			for(int i=0; i<x_size; ++i)
			{
				if(map[i][j].ground == k)
				{
					ani.GetAni(ANI_TILE)->Draw(PIC_MAP, PIC_TILE1, i * TILE_SIZE - ((TILE_SOURCE_SIZE - TILE_SIZE) / 2), j * TILE_SIZE - ((TILE_SOURCE_SIZE - TILE_SIZE) / 2), k);
				}
			}
		}
	}

	//������Ʈ
	for(int j=0; j<y_size; ++j)
	{
		for(int i=0; i<x_size; ++i)
		{
			if(map[i][j].object != 0xff)
			{
				SetRect(&tile_rect, map[i][j].object * TILE_SIZE, 8, (map[i][j].object + 1) * TILE_SIZE, UNIT_HEIGHT);
				jdd->DrawPicture(PIC_MAP, PIC_TILE2, i * TILE_SIZE, j * TILE_SIZE, &tile_rect);
			}
		}
	}
}

//�̵� ���ɼ�
int CRPG::GetMoveSpeed(int x, int y)
{
	if(x < 0 || y < 0 || x >= x_size || y >= y_size)return 0;

	if(map[x][y].unit != 0xff)	//������ ������ �̵� �Ұ���
	{
		return 0;
	}
	else if(map[x][y].object != 0xff)	//2�� Ÿ�Ͽ� ���� �̵�
	{
		return TILE_SIZE / (2 << map[x][y].move);
	}
	else if(tile_attr[0][map[x][y].ground] > 0)	//���ÿ� �Ϲ� �� �ܿ��� �̵� �Ұ���
	{
		return 0;
	}
	else	//�Ϲ� 1�� Ÿ�� �̵�
	{
		return TILE_SIZE / (2 << map[x][y].move);
	}
}

//�浹
int CRPG::Crush(int x, int y)
{
	//ȭ�� ������ ����
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_GO_OUT;

	//���� �̺�Ʈ
	if(map[x][y].unit!=0xff)
	{
		CUnit* unit = GetUnit(map[x][y].unit);
		return unit->GetEventNo();
	}

	//���� �̺�Ʈ
	if(map[x][y].event_no!=0xff)return map[x][y].event_no;

	return EVENT_NONE;
}

//��ȭ
int CRPG::Talk(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_NONE;

	//���� �̺�Ʈ
	if(map[x][y].unit!=0xff)return GetUnit(map[x][y].unit)->GetEventNo();

	//���� �̺�Ʈ
	if(map[x][y].event_no!=0xff)return map[x][y].event_no;

	return EVENT_NONE;
}

//���� �̵�
void CRPG::UnitMove(int id,  int dir)
{
	//���� ��ġ ���
	int old_x = unit[id].GetX();
	int old_y = unit[id].GetY();

	//�̵����̸� ó�� ���� �ʰ� ����
	if(!unit[id].Move(dir))return;

	//���ο� ��ġ ���
	int new_x = unit[id].GetX();
	int new_y = unit[id].GetY();

	//�浹 �̺�Ʈ
	if(id==HERO)
	{
		event_no = Crush(new_x, new_y);
		//if(event_no == 0)event_no = EVENT_NONE;	//0�� �̺�Ʈ�� ����(Ư�� �̺�Ʈ)
	}

	//�̵� �ӵ� ���ϱ�, �̵� �� �� ������ ���� ��ġ��
	int move_speed = GetMoveSpeed(new_x, new_y);
	unit[id].SetMoveSpeed(move_speed);
	
	//�̵� �Ұ��̸� ����ġ
	if(move_speed <= 0)
	{
		unit[id].SetPos(old_x, old_y);
	}
	//������ġ�� �Բ� �ʿ� ǥ�õǴ� ���� ��ġ�� ����
	else if(id != active_unit)
	{
		map[old_x][old_y].unit = 0xff;
		map[unit[id].GetX()][unit[id].GetY()].unit = id;
	}
}

//��ũ�� �ʱ�ȭ
void CRPG::InitScroll(int unit_id)
{
	next_scroll_x = scroll_x = MaxMin(unit[unit_id].GetRealX() - (SCREEN_WIDTH / 2), 0, x_size * TILE_SIZE - SCREEN_WIDTH);
	next_scroll_y = scroll_y = MaxMin(unit[unit_id].GetRealY() - (SCREEN_HEIGHT / 2), 0, y_size * TILE_SIZE - SCREEN_HEIGHT);
}

//��Ŀ��
void CRPG::Focus(int unit_id)
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

//����
void CRPG::Control()
{
	//�̵�
	if(GetKey(vkey_up,0))
	{
		UnitMove(HERO, DIR_UP);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}
	else if(GetKey(vkey_down,0))
	{
		UnitMove(HERO, DIR_DOWN);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}
	else if(GetKey(vkey_left,0))
	{
		UnitMove(HERO, DIR_LEFT);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}
	else if(GetKey(vkey_right,0))
	{
		UnitMove(HERO, DIR_RIGHT);
		ani.GetAni(HERO)->SetDirection((WalkDirection)unit[HERO].GetDir());
	}

	//NPC �̵�
	for(int i = 0; i < unit_max; ++i)
	{
		if(i != HERO)
		{
			static int delay;
			++delay;
			if(delay>100)
			{
				int dir = rand()%4;
				UnitMove(i, dir);
				ani.GetAni(i)->SetDirection((WalkDirection)dir);
				delay=0;
			}			
		}
	}
}

//1�� Ÿ�� ���
void CRPG::Draw1F()
{
	RECT screen_rect;
	SetRect(&screen_rect, scroll_x, scroll_y, scroll_x + SCREEN_WIDTH, scroll_y + SCREEN_HEIGHT);
	jdd->DrawPicture(backbuffer, PIC_MAP, 0, 0, &screen_rect);
}

//2�� ������Ʈ ���
void CRPG::Draw2F()
{
	//���� ����
	int start_x = Max((scroll_x / TILE_SIZE) - 1, 0);
	int start_y = Max((scroll_y / TILE_SIZE) - 1, 0);
	int end_x = Min(start_x + (SCREEN_WIDTH / TILE_SIZE) + 1, x_size - 1);
	int end_y = Min(start_y + (SCREEN_HEIGHT / TILE_SIZE) + 1, y_size - 1);

	//2�� ���̾�
	for(int j = start_y; j <= end_y; ++j)
	{
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
}

void CRPG::Draw3F()
{
	//���� ����
	int start_x = Max((scroll_x / TILE_SIZE) - 1, 0);
	int start_y = Max((scroll_y / TILE_SIZE) - 1, 0);
	int end_x = Min(start_x + (SCREEN_WIDTH / TILE_SIZE) + 1, x_size - 1);
	int end_y = Min(start_y + (SCREEN_HEIGHT / TILE_SIZE) + 1, y_size - 1);

	//3�� ���̾�
	for(int j = start_y; j <= end_y; ++j)
	{
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
			if(active_unit >= 0 && i == unit[active_unit].GetX() && j == unit[active_unit].GetY())
			{
				int p = active_unit;
				ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
			}
		}
	}
}

//���� ó��
void CRPG::Process()
{
	//�̺�Ʈ �ʱ�ȭ
	InitEvent();

	//��ũ��Ʈ ó��
	if(!m_script->IsReady())
	{
		Control();
	}

	//���ֵ� �ൿ
	for(int i=0; i<unit_max; ++i)
	{
		unit[i].Action();
	}

	//��Ŀ��
	InitScroll(HERO);

	//�ִϸ��̼� ó��
	ani.Process();

	//���
	Draw1F();
	Draw2F();
	Draw3F();

	//���̵���
	if(first_fade_in)
	{
		FadeIn();
		first_fade_in = FALSE;
	}

	//�̺�Ʈ ó��
	if(event_no >= 0)
	{
		m_script->SetPage(event_no);
	}

	m_script->Scripting();

	CGameProcess::Process();
}

void CRPG::SetHeroXY(int x, int y)
{
	x = MaxMin(x, 0, x_size - 1);
	y = MaxMin(y, 0, y_size - 1);
	unit[HERO].SetPos(x, y);	
	map[x][y].unit = HERO;
}

void CRPG::SetAutoHeroXY(int ev)
{
	//�ϴ� ���ΰ��� �� �ۿ� ��
	unit[HERO].SetPos(-1, 0);

	//�̺�Ʈ �� ������
	bool quit=false;
	for(int j=0; j<y_size && !quit; ++j)
	{
		for(int i=0; i<x_size && !quit; ++i)
		if(map[i][j].event_no == ev)	//�ش� �̺�Ʈ ��ġ�� ���ΰ� ��ġ
		{
			unit[HERO].SetPos(i, j);
			map[i][j].unit = HERO;	//���� �ʿ� ���ΰ� ��ġ
			map[i][j].event_no = 0xff;
			quit=true;
		}
	}

	if(!quit)unit[HERO].SetPos(0, 0);
}

CUnit* CRPG::GetUnit(int id)
{
	return &unit[id];
}

//�ش� ��ġ ���� ã��
int CRPG::FindUnit(int x, int y)
{
	for(int i = 0; i < unit_max; ++i)
	{
		if(unit[i].GetX() == x && unit[i].GetY() == y)
		{
			return i;
		}
	}

	return -1;
}

//�̺�Ʈ �ʱ�ȭ
void CRPG::InitEvent()
{
	event_no = EVENT_NONE;
}

//�̺�Ʈ ��ȣ ���
int CRPG::GetEventNo()
{
	return event_no;
}

//���� Ȱ��ȭ
void CRPG::SetActiveUnit(int id)
{
	active_unit = id;
	map[unit[active_unit].GetX()][unit[active_unit].GetY()].unit = 0xff;	//Ȱ��ȭ ������ Ư�����
}