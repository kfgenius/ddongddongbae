#include "Game.h"
#include "donglib.h"

//�� ����
#define UNIT_Y		40
#define UNIT_PY		8	//������ ���ΰ� 40�̹Ƿ� 8�ȼ� ���� ����

//�׸� �̸�
#define PIC_MAP		"Map"
#define PIC_TILE1	"tile1"
#define PIC_TILE2	"tile2"

//�̺�Ʈ ����
#define EVENT_NONE		-1
#define EVENT_GO_OUT	999

//�ִ� ��ȣ
#define HERO			0
#define ANI_TILE		128

//�ִϸ��̼�
CAnimation ani;

CGame::CGame()
{
	//�ִϸ��̼� �ʱ�ȭ
	ani.CreateAnimation(ANI_TILE, 48, 48);

	//Ÿ��, ������Ʈ, ����
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0,0,255));
	jdd->LoadPicture(PIC_TILE1, "User\\tile1.gif", &jpi, true);
	jdd->LoadPicture(PIC_TILE2, "User\\tile2.gif", &jpi, true);	//2�� Ÿ��
	CreateUnit();
}

CGame::~CGame()
{

}

////////////////////////////////////////////////////////////
// CGame �޼ҵ�

//�ε�
bool CGame::Load(char* map_name)
{
	strcpy(this->map_name, map_name);

	FILE *fp;
	char map_file[80];
	sprintf(map_file, "%s.map",map_name);
	if(!(fp=fopen(map_file, "rb")))return false;

	char data_type[12];
	fread(&data_type,sizeof(char),11,fp);
	data_type[11]=NULL;

	//�ް������ �ƴ�
	if(strcmp(data_type,"HYUGARU2MAP")!=0)
	{
		fclose(fp);
		return false;
	}

	//�� ũ��
	fread(&x_size, sizeof(char), 1, fp);
	fread(&y_size, sizeof(char), 1, fp);
	//�� ������
	for(int k=0; k<3; ++k)
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
		fread(&map[k][i][j], sizeof(char), 1, fp);

	//�̺�Ʈ �� ������
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
		fread(&eventmap[i][j], sizeof(char), 1, fp);

	//���ΰ� ������ �ʱ�ȭ
	unit[HERO].Set(HERO);
	ani.CreateAnimation(HERO, 32, 40, ani_exchange, 3, 20);
	ani.GetAni(HERO)->SetWalk(40, 120, 0, 80);

	//���� ������ �ʱ�ȭ
	unit_max=1;

	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	if(map[2][i][j]!=0xff)
	{
		//�� ���� �ʱ�ȭ
		unit[unit_max].Set(map[2][i][j]);
		unit[unit_max].SetPos(i, j);
		unit[unit_max].SetDir(rand()%4);

		//�ִϸ��̼� �ʱ�ȭ
		ani.CreateAnimation(unit_max, 32, 40, ani_exchange, 3, 20);
		ani.GetAni(unit_max)->SetWalk(40, 120, 0, 80);
		ani.GetAni(unit_max)->SetDirection((WalkDirection)unit[unit_max].GetDir());

		++unit_max;
	}

	//��� ���� �ʱ�ȭ
	for(int i=0; i<UNIT_MAX; ++i)unit_order[i] = i;

	//�� �̸� �׸���
	DrawGround();
	
	fclose(fp);

	//�� �Ӽ� �ҷ�����
	BOOL read_success = FALSE;
	if(fp=fopen("MAP\\map_attr.sav", "rb"))
	{
		//�ĺ���
		char head[9];
		fread(head, sizeof(char), 8, fp);
		head[8] = NULL;
		if(strcmp(head, "MAPATTR0")==0)
		{
			//Ÿ�� �Ӽ���
			for(int i=0; i<2; ++i)
			{
				fread(tile_mov[i], sizeof(int), TILE_MAX, fp);
				fread(tile_attr[i], sizeof(int), TILE_MAX, fp);
				fread(tile_damage[i], sizeof(int), TILE_MAX, fp);
			}
			//���� �Ӽ���
			fread(unitdata, sizeof(CUnitData), TILE_MAX, fp);

			read_success = TRUE;
		}

		//�������� �̵������� ��ȯ
		for(int j=0; j<2; ++j)
		for(int i=0; i<TILE_MAX; ++i)
		{
			if(tile_mov[j][i]==0)tile_mov[j][i]=99;
			else tile_mov[j][i]=2<<tile_mov[j][i];
		}

		fclose(fp);
	}

	//�б� �����ϸ� �ʱ�ȭ
	if(!read_success)
	{
		for(int j=0; j<2; ++j)
		for(int i=0; i<TILE_MAX; ++i)
			tile_mov[j][i]=tile_attr[j][i]=tile_damage[j][i]=99;
	}

	return true;
}

//���� ����
void CGame::CreateUnit()
{
	//���Ͽ��� �б�
	int file_no = 0;

	JPictureInfo pi;
	pi.SetColorKey(JColor(0,0,255));

	FILE *fp;
	while(1)
	{
		char src_file[80];
		strcpy(src_file, StrAdd("User\\Unit%d.png", file_no));

		if(fp = fopen(src_file, "rb"))
		{
			fclose(fp);

			//�ش� ���� ���Ͽ��� ���� ��� �ϳ��� ������ ����
			jdd->LoadPicture(StrAdd("Unit%d", file_no), src_file, &pi, true);

			++file_no;
		}
		else break;
	}
}

//���� �̸� �׸��� �Լ�
void CGame::DrawGround()
{
	//������ �����
	jdd->DeleteSurface(PIC_MAP);

	//���ο� �� ����
	JPictureInfo mpi;
	mpi.SetWidth(x_size * TILESIZE);
	mpi.SetHeight(y_size * TILESIZE);
	jdd->CreateSurface(PIC_MAP, &mpi, true);

	//Ÿ��
	RECT tile_rect;
	for(int k=0; k<18; ++k)
	{
		SetRect(&tile_rect, k*48, 0, (k+1)*48, 48);

		for(int j=0; j<y_size; ++j)
		for(int i=0; i<x_size; ++i)
			if(map[0][i][j]==k)ani.GetAni(ANI_TILE)->Draw(PIC_MAP, PIC_TILE1, i*TILESIZE-8, j*TILESIZE-8, k);
	}

	//������Ʈ
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	if(map[1][i][j]!=0xff)
	{
		SetRect(&tile_rect, map[1][i][j]*TILESIZE, 8, (map[1][i][j]+1)*TILESIZE, UNIT_Y);
		jdd->DrawPicture(PIC_MAP, PIC_TILE2, i*TILESIZE, j*TILESIZE, &tile_rect);
	}
}

//�̵� ���ɼ�
int CGame::GetMoveSpeed(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return 0;

	//������ ���� �̵� �ӵ�
	if(map[2][x][y]!=0xff)return 0;
	else if(map[1][x][y]!=0xff)return TILESIZE/tile_mov[1][map[1][x][y]];
	else if(tile_attr[0][map[0][x][y]]!=0)return 0;	//����ʿ��� ���̳� ����� �ǳ� �� ����
	else return TILESIZE/tile_mov[0][map[0][x][y]];
}

//�浹
int CGame::Crush(int x, int y)
{
	//ȭ�� ������ ����
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_GO_OUT;

	//�̺�Ʈ
	if(eventmap[x][y]!=0xff)return eventmap[x][y];

	return EVENT_NONE;
}

//��ȭ
int CGame::Talk(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_NONE;

	//�̺�Ʈ
	if(eventmap[x][y]!=0xff)return eventmap[x][y];

	return -1;
}

//���� �̵�
void CGame::UnitMove(int id,  int dir)
{
	int old_x = unit[id].GetX();
	int old_y = unit[id].GetY();

	unit[id].Move(dir);

	if(id==HERO)	//�浹 �̺�Ʈ
	{
		int event_id = Crush(unit[id].GetX(), unit[id].GetY());
		event_no = event_id;
	}

	//�̵� �ӵ� ���ϱ�, �̵� �� �� ������ ���� ��ġ��
	int move_speed = GetMoveSpeed(unit[id].GetX(), unit[id].GetY());
	unit[id].SetMoveSpeed(move_speed);
	if(move_speed <= 0)unit[id].SetPos(old_x, old_y);
}

//������ Y��ǥ ������ ����
void CGame::UnitSort()
{
	for (int i=unit_max-1; i>0; --i)
	{
		bool sort_ok=true;

		for (int j=0; j<i; ++j)
		if(unit[unit_order[j]].GetRealY() > unit[unit_order[j+1]].GetRealY())
		{
			int temp=unit_order[j];
			unit_order[j]=unit_order[j+1];
			unit_order[j+1]=temp;

			sort_ok=false;
		}

		if(sort_ok)break;	//�ٲ��� �߻����� �ʾҴٸ� ������ �Ϸ�� ����
	}
}

//��ũ�� �ʱ�ȭ
void CGame::InitScroll()
{
	next_scroll_x=scroll_x=MaxMin(unit[HERO].GetRealX()-320, 0, x_size*TILESIZE-640);
	next_scroll_y=scroll_y=MaxMin(unit[HERO].GetRealY()-240, 0, y_size*TILESIZE-480);
}

//���� ����
void CGame::Start()
{
	//���� �ʱ�ȭ
	unit[HERO].SetPos(0, 0);
	InitScroll();

	event_no=-1;
}

//Ű ����
void CGame::Control()
{
	event_no = -1;

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
}

int CGame::Process()
{
	unit[HERO].Action();

	InitScroll();
	ani.Process();

	//Dscript�� ���� ������
	return event_no;
}

void CGame::Draw()
{
	//��� ���
	RECT screen_rect;
	SetRect(&screen_rect, scroll_x, scroll_y, scroll_x+640, scroll_y+480);
	jdd->DrawPicture(backbuffer, PIC_MAP, 0, 0, &screen_rect);

	//���� ����
	int start_x = scroll_x/32;
	int start_y = scroll_y/32;
	int end_x = Min(start_x+20, x_size-1);
	int end_y = Min(start_y+15, y_size-1);
	
	//����
	UnitSort();
	for(int i=0; i<unit_max; ++i)
	{
		int p = unit_order[i];	//��� ������� �˻�

		//�������� �ʰų� ȭ�� ���� ���� ��� �� ��
		if(!unit[p].GetLife() || unit[p].GetX() < start_x || unit[p].GetX() > end_x
			|| unit[p].GetY() < start_y || unit[p].GetY() > end_y)continue;

		//���
		ani.GetAni(p)->Draw(backbuffer, StrAdd("Unit%d",unit[p].GetID()), unit[p].GetRealX()-scroll_x, unit[p].GetRealY()-scroll_y-UNIT_PY);
	}

	//2�� ���̾�
	for(int j=start_y; j<=end_y; ++j)
	for(int i=start_x; i<=end_x; ++i)
	if(map[1][i][j]!=0xff)
	{
		RECT tile_rect;
		SetRect(&tile_rect, map[1][i][j]*TILESIZE, 0, (map[1][i][j]+1)*TILESIZE, 8);
		jdd->DrawPicture(backbuffer, PIC_TILE2, i*TILESIZE-scroll_x, j*TILESIZE-scroll_y-UNIT_PY, &tile_rect);
	}
}

void CGame::End()
{
}

void CGame::SetHeroXY(int x, int y)
{
	x = MaxMin(x, 0, x_size-1);
	y = MaxMin(y, 0, y_size-1);
	unit[HERO].SetPos(x, y);	
}

void CGame::SetAutoHeroXY()
{
	//�ϴ� ���ΰ��� �� �ۿ� ��
	unit[HERO].SetPos(-1, 0);

	//�̺�Ʈ �� ������
	bool quit=false;
	for(int j=0; j<y_size && !quit; ++j)
	for(int i=0; i<x_size && !quit; ++i)
	if(eventmap[i][j] == 0)	//0�� �̺�Ʈ�� ������ �װ��� ���ΰ��� �ΰ� �� ���� ����
	{
		int unit_no = FindUnit(i, j);
		if(unit_no>=0 && unit_no<UNIT_MAX)unit[unit_no].Die();
		eventmap[i][j] = map[2][i][j] = 0xff;		
		unit[HERO].SetPos(i, j);
		quit=true;
	}

	if(unit[HERO].GetX() == -1)unit[HERO].SetPos(0, 0);
}

CUnit* CGame::GetUnit(int id)
{
	return &unit[id];
}

//�ش� ��ġ ���� ã��
int CGame::FindUnit(int x, int y)
{
	for(int i=0; i<unit_max; ++i)
		if(unit[i].GetX()==x && unit[i].GetY()==y)return i;

	return -1;
}

//���� ����
void CGame::SaveGame(FILE *save_fp)
{
	fwrite(map_name, sizeof(int), 80, save_fp);
	fwrite(&x_size, sizeof(char), 1, save_fp);
	fwrite(&y_size, sizeof(char), 1, save_fp);
	fwrite(map, sizeof(char), LAYER_MAX*x_size*y_size, save_fp);
	fwrite(eventmap, sizeof(char), x_size*y_size, save_fp);
	fwrite(&scroll_x, sizeof(int), 1, save_fp);
	fwrite(&scroll_y, sizeof(int), 1, save_fp);
	fwrite(&next_scroll_x, sizeof(int), 1, save_fp);
	fwrite(&next_scroll_y, sizeof(int), 1, save_fp);
	fwrite(&unit_max, sizeof(int), 1, save_fp);
	fwrite(&unit, sizeof(CUnit), UNIT_MAX, save_fp);
	fwrite(&unit_order, sizeof(int), UNIT_MAX, save_fp);
}

//���� �ҷ�����
void CGame::LoadGame(FILE *load_fp)
{
	//�� �ε�
	fread(map_name, sizeof(int), 80, load_fp);
	Load(map_name);

	//���� �б�
	fread(&x_size, sizeof(char), 1, load_fp);
	fread(&y_size, sizeof(char), 1, load_fp);
	fread(map, sizeof(char), LAYER_MAX*x_size*y_size, load_fp);
	fread(eventmap, sizeof(char), x_size*y_size, load_fp);
	fread(&scroll_x, sizeof(int), 1, load_fp);
	fread(&scroll_y, sizeof(int), 1, load_fp);
	fread(&next_scroll_x, sizeof(int), 1, load_fp);
	fread(&next_scroll_y, sizeof(int), 1, load_fp);
	fread(&unit_max, sizeof(int), 1, load_fp);
	fread(&unit, sizeof(CUnit), UNIT_MAX, load_fp);
	fread(&unit_order, sizeof(int), UNIT_MAX, load_fp);
}