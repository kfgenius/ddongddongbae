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
#define EVENT_NONE		-1
#define EVENT_GO_OUT	999

//애니 번호
#define HERO			0
#define ANI_TILE		128

//애니메이션
CAnimation ani;

CGame::CGame()
{
	//애니메이션 초기화
	ani.CreateAnimation(ANI_TILE, 48, 48);

	//타일, 오브젝트, 유닛
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0,0,255));
	jdd->LoadPicture(PIC_TILE1, "User\\tile1.gif", &jpi, true);
	jdd->LoadPicture(PIC_TILE2, "User\\tile2.gif", &jpi, true);	//2층 타일
	CreateUnit();
}

CGame::~CGame()
{

}

////////////////////////////////////////////////////////////
// CGame 메소드

//로드
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

	//휴가루맵이 아님
	if(strcmp(data_type,"HYUGARU2MAP")!=0)
	{
		fclose(fp);
		return false;
	}

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

	//주인공 데이터 초기화
	unit[HERO].Set(HERO);
	ani.CreateAnimation(HERO, 32, 40, ani_exchange, 3, 20);
	ani.GetAni(HERO)->SetWalk(40, 120, 0, 80);

	//유닛 데이터 초기화
	unit_max=1;

	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	if(map[2][i][j]!=0xff)
	{
		//맵 정보 초기화
		unit[unit_max].Set(map[2][i][j]);
		unit[unit_max].SetPos(i, j);
		unit[unit_max].SetDir(rand()%4);

		//애니메이션 초기화
		ani.CreateAnimation(unit_max, 32, 40, ani_exchange, 3, 20);
		ani.GetAni(unit_max)->SetWalk(40, 120, 0, 80);
		ani.GetAni(unit_max)->SetDirection((WalkDirection)unit[unit_max].GetDir());

		++unit_max;
	}

	//출력 순서 초기화
	for(int i=0; i<UNIT_MAX; ++i)unit_order[i] = i;

	//맵 미리 그리기
	DrawGround();
	
	fclose(fp);

	//맵 속성 불러오기
	BOOL read_success = FALSE;
	if(fp=fopen("MAP\\map_attr.sav", "rb"))
	{
		//식별자
		char head[9];
		fread(head, sizeof(char), 8, fp);
		head[8] = NULL;
		if(strcmp(head, "MAPATTR0")==0)
		{
			//타일 속성값
			for(int i=0; i<2; ++i)
			{
				fread(tile_mov[i], sizeof(int), TILE_MAX, fp);
				fread(tile_attr[i], sizeof(int), TILE_MAX, fp);
				fread(tile_damage[i], sizeof(int), TILE_MAX, fp);
			}
			//유닛 속성값
			fread(unitdata, sizeof(CUnitData), TILE_MAX, fp);

			read_success = TRUE;
		}

		//설정값을 이동값으로 변환
		for(int j=0; j<2; ++j)
		for(int i=0; i<TILE_MAX; ++i)
		{
			if(tile_mov[j][i]==0)tile_mov[j][i]=99;
			else tile_mov[j][i]=2<<tile_mov[j][i];
		}

		fclose(fp);
	}

	//읽기 실패하면 초기화
	if(!read_success)
	{
		for(int j=0; j<2; ++j)
		for(int i=0; i<TILE_MAX; ++i)
			tile_mov[j][i]=tile_attr[j][i]=tile_damage[j][i]=99;
	}

	return true;
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
			if(map[0][i][j]==k)ani.GetAni(ANI_TILE)->Draw(PIC_MAP, PIC_TILE1, i*TILESIZE-8, j*TILESIZE-8, k);
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
	else if(map[1][x][y]!=0xff)return TILESIZE/tile_mov[1][map[1][x][y]];
	else if(tile_attr[0][map[0][x][y]]!=0)return 0;	//월드맵에선 물이나 용암을 건널 수 없음
	else return TILESIZE/tile_mov[0][map[0][x][y]];
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

//유닛 이동
void CGame::UnitMove(int id,  int dir)
{
	int old_x = unit[id].GetX();
	int old_y = unit[id].GetY();

	unit[id].Move(dir);

	if(id==HERO)	//충돌 이벤트
	{
		int event_id = Crush(unit[id].GetX(), unit[id].GetY());
		event_no = event_id;
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
	//변수 초기화
	unit[HERO].SetPos(0, 0);
	InitScroll();

	event_no=-1;
}

//키 조작
void CGame::Control()
{
	event_no = -1;

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
}

int CGame::Process()
{
	unit[HERO].Action();

	InitScroll();
	ani.Process();

	//Dscript에 정보 보내기
	return event_no;
}

void CGame::Draw()
{
	//배경 출력
	RECT screen_rect;
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

		//존재하지 않거나 화면 밖일 때는 출력 안 함
		if(!unit[p].GetLife() || unit[p].GetX() < start_x || unit[p].GetX() > end_x
			|| unit[p].GetY() < start_y || unit[p].GetY() > end_y)continue;

		//출력
		ani.GetAni(p)->Draw(backbuffer, StrAdd("Unit%d",unit[p].GetID()), unit[p].GetRealX()-scroll_x, unit[p].GetRealY()-scroll_y-UNIT_PY);
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

void CGame::SetHeroXY(int x, int y)
{
	x = MaxMin(x, 0, x_size-1);
	y = MaxMin(y, 0, y_size-1);
	unit[HERO].SetPos(x, y);	
}

void CGame::SetAutoHeroXY()
{
	//일단 주인공을 맵 밖에 둠
	unit[HERO].SetPos(-1, 0);

	//이벤트 맵 데이터
	bool quit=false;
	for(int j=0; j<y_size && !quit; ++j)
	for(int i=0; i<x_size && !quit; ++i)
	if(eventmap[i][j] == 0)	//0번 이벤트가 있으면 그곳에 주인공을 두고 전 내용 삭제
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

//해당 위치 유닛 찾기
int CGame::FindUnit(int x, int y)
{
	for(int i=0; i<unit_max; ++i)
		if(unit[i].GetX()==x && unit[i].GetY()==y)return i;

	return -1;
}

//게임 저장
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

//게임 불러오기
void CGame::LoadGame(FILE *load_fp)
{
	//맵 로드
	fread(map_name, sizeof(int), 80, load_fp);
	Load(map_name);

	//정보 읽기
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