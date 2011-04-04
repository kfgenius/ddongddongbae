#include "gamemap.h"
#include "donglib.h"

//맵 관련
#define UNIT_HEIGHT	40
#define UNIT_PY		8	//유닛은 세로가 40이므로 8픽셀 위에 찍음
#define FOCUS_MOVE_SPEED	4

//그림 이름
#define PIC_MAP		"_Ground"
#define PIC_TILE1	"_tile1"
#define PIC_TILE2	"_tile2"

//애니 번호
#define ANI_TILE		UNIT_TYPE_MAX

//이동 불가
#define MOVE_DISABLE	9999

///////////////////////////////////////////////////////////
// CGameMap 메소드

CGameMap::CGameMap(char* map_name, char* tile1_name, char* tile2_name)
{
	//애니메이션 초기화
	ani.CreateAnimation(ANI_TILE, TILE_SOURCE_SIZE, TILE_SOURCE_SIZE);

	//타일, 오브젝트, 유닛
	JPictureInfo jpi, tpi;
	jpi.SetColorKey(JColor(0,0,255));
	jdd->LoadPicture(PIC_TILE1, tile1_name, &jpi, true, &tpi);
	jdd->LoadPicture(PIC_TILE2, tile2_name, &jpi, true);	//2층 타일
	CreateUnit();

	//타일 수 계산
	tile_type_max = Min(tpi.GetWidth() / TILE_SOURCE_SIZE, TILE_TYPE_MAX);

	//맵 파일 로드
	strcpy(this->map_name, map_name);

	FILE *fp;
	char map_file[80];
	sprintf(map_file, "%s.map",map_name);
	if(!(fp=fopen(map_file, "rb")))return false;

	char data_type[12];
	fread(&data_type,sizeof(char),11,fp);
	data_type[11]=NULL;

	//휴가루 맵이 아님
	if(strcmp(data_type,"HYUGARU2MAP")!=0)
	{
		fclose(fp);
		OutputDebugString("정상적인 맵파일이 아닙니다.");
		return;
	}

	//맵 크기
	fread(&x_size, sizeof(char), 1, fp);
	fread(&y_size, sizeof(char), 1, fp);

	//1F 데이터
	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	{
		fread(&map[i][j].ground, sizeof(char), 1, fp);
	}

	//2F 데이터
	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	{
		fread(&map[i][j].object, sizeof(char), 1, fp);
	}

	//유닛 데이터
	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	{
		fread(&map[i][j].unit, sizeof(char), 1, fp);
	}

	//이벤트 맵 데이터
	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	{
		fread(&map[i][j].event_no, sizeof(char), 1, fp);
	}

	//주인공 데이터 초기화
	unit[HERO].Set(HERO, party_ally, TILE_SIZE);
	ani.CreateAnimation(HERO, 32, 40, ani_exchange, 3, 20);
	ani.GetAni(HERO)->SetWalk(0, 80, 120, 40);

	//유닛 데이터 초기화
	unit_max = HERO + 1;

	for(int j = 0; j < y_size; ++j)
	for(int i = 0; i < x_size; ++i)
	if(map[i][j].unit != 0xff)
	{
		//유닛이 서있을 장소에 유닛 배치
		unit[unit_max].Set(map[i][j].unit, party_enemy, TILE_SIZE);
		unit[unit_max].SetPos(i, j);
		unit[unit_max].SetDir(/*rand()%4*/DIR_DOWN);
		
		//유닛 정보를 넘겨준 후 유닛의 ID로 교체
		map[i][j].unit = unit_max;
		unit[unit_max].SetEventNo(map[i][j].event_no);
		map[i][j].event_no = 0xff;

		//애니메이션 초기화
		ani.CreateAnimation(unit_max, 32, 40, ani_exchange, 3, 20);
		ani.GetAni(unit_max)->SetWalk(40, 120, 0, 80);
		ani.GetAni(unit_max)->SetDirection((WalkDirection)unit[unit_max].GetDir());

		++unit_max;
	}

	//맵 미리 그리기
	DrawGround();
	
	fclose(fp);

	//맵 속성 불러오기
	bool read_success = false;
	if(fp=fopen("MAP\\map_attr.sav", "rb"))
	{
		//식별자
		char head[9];
		fread(head, sizeof(char), 8, fp);
		head[8] = NULL;
		if(strcmp(head, "MAPATTR0") == 0)
		{
			//타일 속성값
			for(int i = 0; i < 2; ++i)
			{
				fread(tile_mov[i], sizeof(int), TILE_TYPE_MAX, fp);
				fread(tile_attr[i], sizeof(int), TILE_TYPE_MAX, fp);
				fread(tile_damage[i], sizeof(int), TILE_TYPE_MAX, fp);
			}
			//유닛 속성값
			fread(unitdata, sizeof(CUnitData), TILE_MAX, fp);

			read_success = true;
		}

		//설정값을 이동값으로 변환
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

	//읽기 실패하면 초기화
	if(!read_success)
	{
		for(int j = 0; j < 2; ++j)
		for(int i = 0; i < TILE_MAX; ++i)
		{
			tile_mov[j][i] = tile_attr[j][i] = tile_damage[j][i] = 99;
		}
	}

	//이동용 맵 생성
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
		
		//이동력 0이면 이동 불가 지형
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

//유닛 생성
void CGameMap::CreateUnit()
{
	//파일에서 읽기
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

			//해당 유닛 파일에서 정면 모습 하나를 가져다 찍음
			char unit_name[20];
			sprintf(unit_name, "Unit%d", file_no);
			jdd->LoadPicture(unit_name, src_file, &pi, true);

			++file_no;
		}
		else break;
	}
}

//맵을 미리 그리는 함수(매번 그리면 느리므로)
void CGameMap::DrawGround()
{
	//이전맵 지우기
	jdd->DeleteSurface(PIC_MAP);

	//새로운 맵 생성
	JPictureInfo mpi;
	mpi.SetWidth(x_size * TILE_SIZE);
	mpi.SetHeight(y_size * TILE_SIZE);
	jdd->CreateSurface(PIC_MAP, &mpi, true);

	//타일
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

	//오브젝트
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

//스크롤 초기화
void CGameMap::InitScroll(int unit_id)
{
	next_scroll_x = scroll_x = MaxMin(unit[unit_id].GetRealX() - (SCREEN_WIDTH / 2), 0, x_size * TILE_SIZE - SCREEN_WIDTH);
	next_scroll_y = scroll_y = MaxMin(unit[unit_id].GetRealY() - (SCREEN_HEIGHT / 2), 0, y_size * TILE_SIZE - SCREEN_HEIGHT);
}

//포커스
void CGameMap::Focus(int unit_id)
{
	//X축
	int ux = MaxMin(unit[unit_id].GetRealX() - (SCREEN_WIDTH / 2), 0, x_size * TILE_SIZE - SCREEN_WIDTH);
	if(scroll_x < ux)
	{
		scroll_x = Min(scroll_x + FOCUS_MOVE_SPEED, ux);
	}
	else if(scroll_x > ux)
	{
		scroll_x = Max(scroll_x - FOCUS_MOVE_SPEED, ux);
	}

	//Y축
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

//1층 타일 출력
void CGameMap::Draw1F()
{
	RECT screen_rect;
	SetRect(&screen_rect, scroll_x, scroll_y, scroll_x + SCREEN_WIDTH, scroll_y + SCREEN_HEIGHT);
	jdd->DrawPicture(backbuffer, PIC_MAP, 0, 0, &screen_rect);
}

//2층 오브젝트 출력
void CGameMap::Draw2F()
{
	//찍을 범위
	int start_x = Max((scroll_x / TILE_SIZE) - 1, 0);
	int start_y = Max((scroll_y / TILE_SIZE) - 1, 0);
	int end_x = Min(start_x + (SCREEN_WIDTH / TILE_SIZE) + 1, x_size - 1);
	int end_y = Min(start_y + (SCREEN_HEIGHT / TILE_SIZE) + 1, y_size - 1);

	//2층 레이어
	for(int j = start_y; j <= end_y; ++j)
	for(int i = start_x; i <= end_x; ++i)
	{
		int px = i * TILE_SIZE - scroll_x;
		int py = j * TILE_SIZE - scroll_y;

		//2층
		if(map[i][j].object!=0xff)
		{
			RECT tile_rect;
			SetRect(&tile_rect, map[i][j].object*TILE_SIZE, 0, (map[i][j].object+1)*TILE_SIZE, 8);
			jdd->DrawPicture(backbuffer, PIC_TILE2, px, py-UNIT_PY, &tile_rect);
		}

		//일반 유닛
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
	//찍을 범위
	int start_x = Max((scroll_x / TILE_SIZE) - 1, 0);
	int start_y = Max((scroll_y / TILE_SIZE) - 1, 0);
	int end_x = Min(start_x + (SCREEN_WIDTH / TILE_SIZE) + 1, x_size - 1);
	int end_y = Min(start_y + (SCREEN_HEIGHT / TILE_SIZE) + 1, y_size - 1);

	//3층 레이어
	for(int j = start_y; j <= end_y; ++j)
	for(int i = start_x; i <= end_x; ++i)
	{
		int px = i * TILE_SIZE - scroll_x;
		int py = j * TILE_SIZE - scroll_y;

		//비행 유닛
		if(map[i][j].unit != 0xff)
		{
			int p = map[i][j].unit;
			if(unit[p].move_bonus >= SKY_MOVE)
			{
				ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
			}
		}
		
		//활성화된 유닛
		/*if(active_unit >= 0 && i == unit[active_unit].GetX() && j == unit[active_unit].GetY())
		{
			int p = active_unit;
			ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
		}*/
	}
}

//맵 데이터 얻기
TileData GetMapData(int x, int y)
{
	return &map[x][y];
}

//주인공 위치 설정
void CGameMap::SetHeroXY(int x, int y)
{
	x = MaxMin(x, 0, x_size-1);
	y = MaxMin(y, 0, y_size-1);
	unit[HERO].SetPos(x, y);	
	map[x][y].unit = HERO;
}

//주인공 위치를 이벤트 번호로 자동 배치
void CGameMap::SetAutoHeroXY(int ev)
{
	//일단 주인공을 맵 밖에 둠
	unit[HERO].SetPos(-1, 0);

	//이벤트 맵 데이터
	bool quit=false;
	for(int j=0; j<y_size && !quit; ++j)
	for(int i=0; i<x_size && !quit; ++i)
	if(map[i][j].event_no == ev)	//해당 이벤트 위치에 주인공 배치
	{
		unit[HERO].SetPos(i, j);
		map[i][j].unit = HERO;	//유닛 맵에 주인공 배치
		map[i][j].event_no = 0xff;
		quit=true;
	}

	if(unit[HERO].GetX() == -1)unit[HERO].SetPos(0, 0);
}

CUnit* CGameMap::GetUnit(int id)
{
	return &unit[id];
}

//해당 위치 유닛 찾기
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
