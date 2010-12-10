#include "Game.h"
#include "donglib.h"

//맵 관련
#define UNIT_Y		40
#define UNIT_PY		8	//유닛은 세로가 40이므로 8픽셀 위에 찍음

//그림 이름
#define PIC_MAP		"Map"
#define PIC_TILE1	"tile1"
#define PIC_TILE2	"tile2"

//이벤트 종류
#define EVENT_NONE		10001
#define EVENT_GO_OUT	10002

//애니메이션
CAnimation ani;

#define ANI_TILE		0

CGame::CGame()
{
	//애니메이션 초기화
	ani.CreateAnimation(ANI_TILE, 48, 48);
}

CGame::~CGame()
{
}

////////////////////////////////////////////////////////////
// CGame 메소드

//로드
bool CGame::Load(int map_id)
{
	this->map_id = map_id;

	FILE *fp;
	char map_name[20];
	sprintf(map_name, "MAP\\map%02d.map",map_id);
	if(fp=fopen(map_name, "rb"))
	{
		char data_type[12];
		fread(&data_type,sizeof(char),11,fp);
		data_type[11]=NULL;

		if(strcmp(data_type,"HYUGARU2MAP")==0)
		{
			//맵 크기
			fread(&x_size, sizeof(char), 1, fp);
			fread(&y_size, sizeof(char), 1, fp);
			//맵 데이터
			for(int k=0; k<3; ++k)
			for(int j=0; j<y_size; ++j)
			for(int i=0; i<x_size; ++i)
				fread(&map[k][i][j], sizeof(char), 1, fp);

			//이벤트 맵 데이터
			for(int j=0; j<y_size; ++j)
			for(int i=0; i<x_size; ++i)
				fread(&eventmap[i][j], sizeof(char), 1, fp);

			//유닛 데이터 초기화
			unit_max=1;
			unit[HERO].Set(HERO);

			for(int j=0; j<y_size; ++j)
			for(int i=0; i<x_size; ++i)
			if(map[2][i][j]!=0xff)
			{
				unit[unit_max].Set(map[2][i][j]);
				unit[unit_max].SetPos(i, j);
				++unit_max;
			}

			for(int i=0; i<UNIT_MAX; ++i)
			{
				unit_order[i] = i;
				unit[i].SetDir(rand()%4);
			}

			//맵 미리 그리기
			DrawGround();
			
			fclose(fp);
			return true;
		}

		fclose(fp);
		return false;
	}

	return false;
}

//유닛 생성
void CGame::CreateUnit()
{
	//파일에서 읽기
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

			//해당 유닛 파일에서 정면 모습 하나를 가져다 찍음
			jdd->LoadPicture(StrAdd("Unit%d", file_no), src_file, &pi, true);

			++file_no;
		}
		else break;
	}
}

//맵을 미리 그리는 함수
void CGame::DrawGround()
{
	//이전맵 지우기
	jdd->DeleteSurface(PIC_MAP);

	//새로운 맵 생성
	JPictureInfo mpi;
	mpi.SetWidth(x_size * TILESIZE);
	mpi.SetHeight(y_size * TILESIZE);
	jdd->CreateSurface(PIC_MAP, &mpi, true);

	//타일
	RECT tile_rect;
	for(int k=0; k<18; ++k)
	{
		SetRect(&tile_rect, k*48, 0, (k+1)*48, 48);

		for(int j=0; j<y_size; ++j)
		for(int i=0; i<x_size; ++i)
			if(map[0][i][j]==k)ani.GetAni(0)->Draw(PIC_MAP, PIC_TILE1, i*TILESIZE-8, j*TILESIZE-8, k);
	}

	//오브젝트
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	if(map[1][i][j]!=0xff)
	{
		SetRect(&tile_rect, map[1][i][j]*TILESIZE, 8, (map[1][i][j]+1)*TILESIZE, UNIT_Y);
		jdd->DrawPicture(PIC_MAP, PIC_TILE2, i*TILESIZE, j*TILESIZE, &tile_rect);
	}
}

//이동 가능성
int CGame::GetMoveSpeed(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return 0;

	//지형에 따른 이동 속도
	if(map[2][x][y]!=0xff)return 0;
		else if(map[1][x][y]!=0xff)return TILESIZE/tile2_mov[map[1][x][y]];
		else return TILESIZE/tile1_mov[map[0][x][y]];
}

//충돌
int CGame::Crush(int x, int y)
{
	//화면 밖으로 나감
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_GO_OUT;

	//이벤트
	if(eventmap[x][y]!=0xff)return eventmap[x][y];

	return EVENT_NONE;
}

//대화
int CGame::Talk(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_NONE;

	//이벤트
	if(eventmap[x][y]!=0xff)return eventmap[x][y];

	return -1;
}

//이벤트 넣기
void CGame::InsertEvent(int event_id, int v1, int v2, int v3, int v4)
{
	event_stack.event_id = event_id;
	event_stack.value[0] = v1;
	event_stack.value[1] = v2;
	event_stack.value[2] = v3;
	event_stack.value[3] = v4;
}

//유닛 이동
void CGame::UnitMove(int id,  int dir)
{
	int old_x = unit[id].GetX();
	int old_y = unit[id].GetY();

	unit[id].Move(dir);

	if(id==HERO)	//충돌 이벤트
	{
		int event_id = Crush(unit[id].GetX(), unit[id].GetY());

		//맵 밖으로 나감
		if(event_id==EVENT_GO_OUT)
		{
			if(!map_pos.empty())
			{
				std::vector<MapPos>::iterator it = map_pos.end();
				--it;
				InsertEvent(0, it->map_id, it->x, it->y);
				map_pos.pop_back();
			}
		}
		else if(event_id==EVENT_NONE)
		{
		}
		else
		{
			InsertEvent(0, event_id);

			MapPos tmp_pos;
			tmp_pos.map_id = map_id;
			tmp_pos.x = unit[HERO].GetX();
			tmp_pos.y = unit[HERO].GetY();
			map_pos.push_back(tmp_pos);
		}
	}

	//이동 속도 구하기, 이동 할 수 없으면 원래 위치로
	int move_speed = GetMoveSpeed(unit[id].GetX(), unit[id].GetY());
	unit[id].SetMoveSpeed(move_speed);
	if(move_speed <= 0)unit[id].SetPos(old_x, old_y);
}

//유닛을 Y좌표 순으로 정렬
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

		if(sort_ok)break;	//바꿈이 발생하지 않았다면 정렬이 완료된 상태
	}
}

//스크롤 초기화
void CGame::InitScroll()
{
	next_scroll_x=scroll_x=MaxMin(unit[HERO].GetRealX()-320, 0, x_size*TILESIZE-640);
	next_scroll_y=scroll_y=MaxMin(unit[HERO].GetRealY()-240, 0, y_size*TILESIZE-480);
}

//게임 시작
void CGame::Start()
{
	//타일, 오브젝트, 유닛
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0,0,255));
	jdd->LoadPicture(PIC_TILE1, "User\\tile1.gif", &jpi, true);
	jdd->LoadPicture(PIC_TILE2, "User\\tile2.gif", &jpi, true);	//2층 타일
	CreateUnit();

	//맵 불러오기
	Load(0);

	//변수 초기화
	unit[HERO].SetPos(0, 0);
	InitScroll();

	event_stack.event_id=-1;
}

void CGame::Process()
{
	//이벤트 처리
	if(event_stack.event_id>=0)
	{
		if(event_stack.value[0]==0 || event_stack.value[0]>=60)
		{
			//맵 불러오기
			Load(event_stack.value[0]);

			if(event_stack.value[1]<0)
			{
				for(int j=0; j<y_size; ++j)
				for(int i=0; i<x_size; ++i)
				if(eventmap[i][j]==0)
				{
					unit[HERO].SetPos(i, j);
					break;
				}

				//못 찾았을 경우 (0, 0)으로
				if(unit[HERO].GetX()==-1)unit[HERO].SetPos(0, 0);
			}
			else unit[HERO].SetPos(event_stack.value[1], event_stack.value[2]);

			InitScroll();
			
			event_stack.event_id = -1;
		}
	}

	//이동
	if(GetKey(vkey_up,0))UnitMove(HERO, DIR_UP);
		else if(GetKey(vkey_down,0))UnitMove(HERO, DIR_DOWN);
		else if(GetKey(vkey_left,0))UnitMove(HERO, DIR_LEFT);
		else if(GetKey(vkey_right,0))UnitMove(HERO, DIR_RIGHT);

	unit[HERO].Action();
	InitScroll();
}

void CGame::Draw()
{
	//배경 출력
	RECT screen_rect, unit_rect;
	SetRect(&screen_rect, scroll_x, scroll_y, scroll_x+640, scroll_y+480);
	jdd->DrawPicture(backbuffer, PIC_MAP, 0, 0, &screen_rect);

	//찍을 범위
	int start_x = scroll_x/32;
	int start_y = scroll_y/32;
	int end_x = Min(start_x+20, x_size-1);
	int end_y = Min(start_y+15, y_size-1);
	
	//유닛
	UnitSort();
	for(int i=0; i<unit_max; ++i)
	{
		int p = unit_order[i];	//출력 순서대로 검사

		//화면 밖일 때는 출력 안 함
		if(unit[p].GetX() < start_x || unit[p].GetX() > end_x
			|| unit[p].GetY() < start_y || unit[p].GetY() > end_y)continue;

		//출력
		SetRect(&unit_rect, 0, unit[p].GetDir()*UNIT_Y, TILESIZE, (unit[p].GetDir()+1)*UNIT_Y);
		jdd->DrawPicture(backbuffer, StrAdd("Unit%d",unit[p].GetID()), unit[p].GetRealX()-scroll_x, unit[p].GetRealY()-scroll_y-UNIT_PY, &unit_rect);
	}

	//2층 레이어
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