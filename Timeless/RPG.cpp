#include "rpg.h"
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

//이벤트 종류
#define EVENT_NONE		-1
#define EVENT_GO_OUT	999

////////////////////////////////////////////////////////////
// CRPG 메소드

CRPG::CRPG(char* map_name, char* tile1_name, char* tile2_name)
{
	//애니메이션 초기화
	ani.CreateAnimation(ANI_TILE, TILE_SOURCE_SIZE, TILE_SOURCE_SIZE);

	//타일, 오브젝트, 유닛
	JPictureInfo jpi, tpi;
	jpi.SetColorKey(JColor(0,0,255));
	jdd->LoadPicture(PIC_TILE1, tile1_name, &jpi, true);
	jdd->LoadPicture(PIC_TILE2, tile2_name, &jpi, true);	//2층 타일
	CreateUnit();

	//타일 수 계산
	jdd->GetPictureInfo(PIC_TILE1, &tpi);
	tile_type_max = Min(tpi.GetWidth() / TILE_SOURCE_SIZE, TILE_TYPE_MAX);

	//맵 파일 로드
	strcpy(this->map_name, map_name);

	FILE *fp;
	char map_file[80];
	sprintf(map_file, "%s.map",map_name);
	if(!(fp=fopen(map_file, "rb")))
	{
		char msg[80];
		sprintf(msg, "%s을 찾을 수 없습니다.", map_file);
		OutputDebugString(msg);
		exit(1);
	}

	char data_type[12];
	fread(&data_type,sizeof(char),11,fp);
	data_type[11]=NULL;

	//휴가루 맵이 아님
	if(strcmp(data_type,"HYUGARU2MAP")!=0)
	{
		fclose(fp);
		char msg[80];
		sprintf(msg, "%s는 정상적인 맵파일이 아닙니다.", map_file);
		OutputDebugString(msg);
		exit(1);
	}

	//맵 크기
	fread(&x_size, sizeof(char), 1, fp);
	fread(&y_size, sizeof(char), 1, fp);

	//1F 데이터
	for(int j = 0; j < y_size; ++j)
	{
		for(int i = 0; i < x_size; ++i)
		{
			fread(&map[i][j].ground, sizeof(char), 1, fp);
		}
	}

	//2F 데이터
	for(int j = 0; j < y_size; ++j)
	{
		for(int i = 0; i < x_size; ++i)
		{
			fread(&map[i][j].object, sizeof(char), 1, fp);
		}
	}

	//유닛 데이터
	for(int j = 0; j < y_size; ++j)
	{
		for(int i = 0; i < x_size; ++i)
		{
			fread(&map[i][j].unit, sizeof(char), 1, fp);
		}
	}

	//이벤트 맵 데이터
	for(int j = 0; j < y_size; ++j)
	{
		for(int i = 0; i < x_size; ++i)
		{
			fread(&map[i][j].event_no, sizeof(char), 1, fp);
		}
	}

	//주인공 데이터 초기화
	unit[HERO].Set(HERO, party_ally, TILE_SIZE);
	ani.CreateAnimation(HERO, 32, 40, ani_exchange, 3, 20);
	ani.GetAni(HERO)->SetWalk(0, 80, 120, 40);

	//유닛 데이터 초기화
	unit_max = HERO + 1;

	for(int j = 0; j < y_size; ++j)
	{
		for(int i = 0; i < x_size; ++i)
		{
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
				ani.GetAni(unit_max)->SetWalk(0, 80, 120, 40);
				ani.GetAni(unit_max)->SetDirection((WalkDirection)unit[unit_max].GetDir());

				++unit_max;
			}
		}
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
			fread(unitdata, sizeof(CUnitData), TILE_TYPE_MAX, fp);

			read_success = true;
		}

		//설정값을 이동값으로 변환
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

	//읽기 실패하면 초기화
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

	//이동용 맵 생성
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
			
			//이동력 0이면 이동 불가 지형
			if(map[i][j].move == 99)
			{
				map[i][j].move = MOVE_DISABLE;
			}
		}
	}

	//시작할 때 페이드인
	first_fade_in = TRUE;

	//주인공 위치 초기화
	SetAutoHeroXY(0);
	InitScroll(HERO);

	//스크립트 초기화
	m_script = new CScript(map_name);
	m_script->SetPage(0);
}

CRPG::~CRPG()
{
	jdd->DeleteSurface(PIC_TILE1);
	jdd->DeleteSurface(PIC_TILE2);
}

//유닛 생성
void CRPG::CreateUnit()
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

//맵을 미리 그리는 함수
void CRPG::DrawGround()
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

	//오브젝트
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

//이동 가능성
int CRPG::GetMoveSpeed(int x, int y)
{
	if(x < 0 || y < 0 || x >= x_size || y >= y_size)return 0;

	if(map[x][y].unit != 0xff)	//유닛이 있으면 이동 불가능
	{
		return 0;
	}
	else if(map[x][y].object != 0xff)	//2층 타일에 의한 이동
	{
		return TILE_SIZE / (2 << map[x][y].move);
	}
	else if(tile_attr[0][map[x][y].ground] > 0)	//평상시엔 일반 길 외에는 이동 불가능
	{
		return 0;
	}
	else	//일반 1층 타일 이동
	{
		return TILE_SIZE / (2 << map[x][y].move);
	}
}

//충돌
int CRPG::Crush(int x, int y)
{
	//화면 밖으로 나감
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_GO_OUT;

	//유닛 이벤트
	if(map[x][y].unit!=0xff)
	{
		CUnit* unit = GetUnit(map[x][y].unit);
		return unit->GetEventNo();
	}

	//지형 이벤트
	if(map[x][y].event_no!=0xff)return map[x][y].event_no;

	return EVENT_NONE;
}

//대화
int CRPG::Talk(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_NONE;

	//유닛 이벤트
	if(map[x][y].unit!=0xff)return GetUnit(map[x][y].unit)->GetEventNo();

	//지형 이벤트
	if(map[x][y].event_no!=0xff)return map[x][y].event_no;

	return EVENT_NONE;
}

//유닛 이동
void CRPG::UnitMove(int id,  int dir)
{
	//과거 위치 기억
	int old_x = unit[id].GetX();
	int old_y = unit[id].GetY();

	//이동중이면 처리 하지 않고 리턴
	if(!unit[id].Move(dir))return;

	//새로운 위치 기억
	int new_x = unit[id].GetX();
	int new_y = unit[id].GetY();

	//충돌 이벤트
	if(id==HERO)
	{
		event_no = Crush(new_x, new_y);
		//if(event_no == 0)event_no = EVENT_NONE;	//0번 이벤트는 무시(특수 이벤트)
	}

	//이동 속도 구하기, 이동 할 수 없으면 원래 위치로
	int move_speed = GetMoveSpeed(new_x, new_y);
	unit[id].SetMoveSpeed(move_speed);
	
	//이동 불가이면 원위치
	if(move_speed <= 0)
	{
		unit[id].SetPos(old_x, old_y);
	}
	//유닛위치와 함께 맵에 표시되는 유닛 위치도 변경
	else if(id != active_unit)
	{
		map[old_x][old_y].unit = 0xff;
		map[unit[id].GetX()][unit[id].GetY()].unit = id;
	}
}

//스크롤 초기화
void CRPG::InitScroll(int unit_id)
{
	next_scroll_x = scroll_x = MaxMin(unit[unit_id].GetRealX() - (SCREEN_WIDTH / 2), 0, x_size * TILE_SIZE - SCREEN_WIDTH);
	next_scroll_y = scroll_y = MaxMin(unit[unit_id].GetRealY() - (SCREEN_HEIGHT / 2), 0, y_size * TILE_SIZE - SCREEN_HEIGHT);
}

//포커스
void CRPG::Focus(int unit_id)
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

//조작
void CRPG::Control()
{
	//이동
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

	//NPC 이동
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

//1층 타일 출력
void CRPG::Draw1F()
{
	RECT screen_rect;
	SetRect(&screen_rect, scroll_x, scroll_y, scroll_x + SCREEN_WIDTH, scroll_y + SCREEN_HEIGHT);
	jdd->DrawPicture(backbuffer, PIC_MAP, 0, 0, &screen_rect);
}

//2층 오브젝트 출력
void CRPG::Draw2F()
{
	//찍을 범위
	int start_x = Max((scroll_x / TILE_SIZE) - 1, 0);
	int start_y = Max((scroll_y / TILE_SIZE) - 1, 0);
	int end_x = Min(start_x + (SCREEN_WIDTH / TILE_SIZE) + 1, x_size - 1);
	int end_y = Min(start_y + (SCREEN_HEIGHT / TILE_SIZE) + 1, y_size - 1);

	//2층 레이어
	for(int j = start_y; j <= end_y; ++j)
	{
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
}

void CRPG::Draw3F()
{
	//찍을 범위
	int start_x = Max((scroll_x / TILE_SIZE) - 1, 0);
	int start_y = Max((scroll_y / TILE_SIZE) - 1, 0);
	int end_x = Min(start_x + (SCREEN_WIDTH / TILE_SIZE) + 1, x_size - 1);
	int end_y = Min(start_y + (SCREEN_HEIGHT / TILE_SIZE) + 1, y_size - 1);

	//3층 레이어
	for(int j = start_y; j <= end_y; ++j)
	{
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
			if(active_unit >= 0 && i == unit[active_unit].GetX() && j == unit[active_unit].GetY())
			{
				int p = active_unit;
				ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
			}
		}
	}
}

//게임 처리
void CRPG::Process()
{
	//이벤트 초기화
	InitEvent();

	//스크립트 처리
	if(!m_script->IsReady())
	{
		Control();
	}

	//유닛들 행동
	for(int i=0; i<unit_max; ++i)
	{
		unit[i].Action();
	}

	//포커스
	InitScroll(HERO);

	//애니메이션 처리
	ani.Process();

	//출력
	Draw1F();
	Draw2F();
	Draw3F();

	//페이드인
	if(first_fade_in)
	{
		FadeIn();
		first_fade_in = FALSE;
	}

	//이벤트 처리
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
	//일단 주인공을 맵 밖에 둠
	unit[HERO].SetPos(-1, 0);

	//이벤트 맵 데이터
	bool quit=false;
	for(int j=0; j<y_size && !quit; ++j)
	{
		for(int i=0; i<x_size && !quit; ++i)
		if(map[i][j].event_no == ev)	//해당 이벤트 위치에 주인공 배치
		{
			unit[HERO].SetPos(i, j);
			map[i][j].unit = HERO;	//유닛 맵에 주인공 배치
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

//해당 위치 유닛 찾기
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

//이벤트 초기화
void CRPG::InitEvent()
{
	event_no = EVENT_NONE;
}

//이벤트 번호 얻기
int CRPG::GetEventNo()
{
	return event_no;
}

//유닛 활성화
void CRPG::SetActiveUnit(int id)
{
	active_unit = id;
	map[unit[active_unit].GetX()][unit[active_unit].GetY()].unit = 0xff;	//활성화 유닛은 특수취급
}