#include "srpg.h"
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

//이동 보너스
#define NORMAL_MOVE		0	//보통
#define WATER_MOVE		1	//물
#define FIRE_MOVE		2	//불
#define SPECIAL_MOVE	3	//유격대
#define SKY_MOVE		4	//비행
#define GHOST_MOVE		5	//유령

//명령
char* battle_commands[] = {"공격", "마법", "휴식", "대기"};
CCommand command(battle_commands);


////////////////////////////////////////////////////////////
// CUnit 메소드

//유닛 능력치 데이터
int unit_data[][20] = {
//   HP		힘	민첩이동력	이동보너스	
	{30,	0,	12,	12,		GHOST_MOVE},	//휴가루
	{99,	0,	8,	12,		GHOST_MOVE},	//루시버
	{30,	0,	9,	12,		FIRE_MOVE},		//가미얼
	{20,	0,	10,	12,		WATER_MOVE},	//시크벨
	{20,	0,	15,	12,		SKY_MOVE},		//미버가
	{30,	5,	5,	10,		NORMAL_MOVE},	//땅마왕
	{99,	3,	12,	12,		NORMAL_MOVE},	//휴루시
	{10,	1,	10,	12,		NORMAL_MOVE},	//남자
	{10,	1,	10,	12,		NORMAL_MOVE},	//여자
	{10,	0,	8,	12,		NORMAL_MOVE},	//위생병
	{15,	3,	10,	12,		SPECIAL_MOVE},	//유격대
	{12,	2,	15,	14,		SPECIAL_MOVE}	//도둑
};

CUnit::CUnit()
{
	life=false;
	moving=false;
	SetPos(0,0);
}

//설정
void CUnit::Set(int id)
{
	life = true;
	this->id = id;
	pic_id = jdd->GetHash(StrAdd("Unit%d", id));

	if(id<6)team = 0;
	else team = 1;

	//능력치 초기값 지정
	hp_max = unit_data[id][0];
	power = unit_data[id][1];
	quickness = unit_data[id][2];
	move = unit_data[id][3];
	move_bonus = unit_data[id][4];

	//공통 초기값
	ap = 100;
	hp = hp_max;
}

//사망
void CUnit::Die()
{
	life = false;
}

//생존 상태 얻기
bool CUnit::GetLife()
{
	return life;
}

//ID 얻기
int CUnit::GetID()
{
	return id;
}

int CUnit::GetPictureID()
{
	return pic_id;
}

//좌표 얻기
int CUnit::GetX()
{
	return x;
}

int CUnit::GetY()
{
	return y;
}

int CUnit::GetPX()
{
	return px;
}

int CUnit::GetPY()
{
	return py;
}

int CUnit::GetRealX()
{
	return x*TILESIZE+px;
}

int CUnit::GetRealY()
{
	return y*TILESIZE+py;
}

//방향 얻기, 설정
int CUnit::GetDir()
{
	return dir;
}

void CUnit::SetDir(int dir)
{
	this->dir = dir;
}

//바라보고 있는 곳 구하기
int CUnit::GetDirX()
{
	if(dir==DIR_LEFT)return x-1;
		else if(dir==DIR_RIGHT)return x+1;
		else return x;
}

int CUnit::GetDirY()
{
	if(dir==DIR_UP)return y-1;
		else if(dir==DIR_DOWN)return y+1;
		else return y;
}

//이벤트 번호 지정/구하기
void CUnit::SetEventNo(int event_no)
{
	this->event_no = event_no;
}

int CUnit::GetEventNo()
{
	return event_no;
}

//위치 지정
void CUnit::SetPos(int x, int y)
{
	this->x = x;
	this->y = y;
	px = py = 0;
	moving = false;
}

//이동 속도 지정
void CUnit::SetMoveSpeed(int move_speed)
{
	this->move_speed = move_speed;
}

//이동
bool CUnit::Move(int dir)
{
	if(moving)return false;
	this->dir = dir;

	if(dir == DIR_LEFT)
	{
		--x;
		px=TILESIZE;
	}
	else if(dir == DIR_UP)
	{
		--y;
		py=TILESIZE;
	}
	else if(dir == DIR_RIGHT)
	{
		++x;
		px=-TILESIZE;
	}
	else if(dir == DIR_DOWN)
	{
		++y;
		py=-TILESIZE;
	}

	moving = true;
	return true;
}

void CUnit::Action()
{
	if(!moving)return;

	//좌로 이동
	if(px>0)
	{
		px -= move_speed;
	}
	//우로 이동
	else if(px<0)
	{
		px += move_speed;
	}
	//위로 이동
	else if(py>0)
	{
		py -= move_speed;
	}
	//아래로 이동
	else if(py<0)
	{
		py += move_speed;
	}
	//이동이 끝나면 키 입력 가능
	else moving = false;
}

////////////////////////////////////////////////////////////
// CSRPG 메소드

CSRPG::CSRPG()
{
	//애니메이션 초기화
	ani.CreateAnimation(ANI_TILE, 48, 48);

	//타일, 오브젝트, 유닛
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0,0,255));
	jdd->LoadPicture(PIC_TILE1, "User\\tile1.gif", &jpi, true);
	jdd->LoadPicture(PIC_TILE2, "User\\tile2.gif", &jpi, true);	//2층 타일
	CreateUnit();

	//명령용 창
	command.comdlg.SetDlg(0, 0, 100, 4);
	command.comdlg.MakeDlgBox("command_dlg");
	command.comdlg.SetOpacity(0.7f);
}

//로드
bool CSRPG::Load(char* map_name, bool battle)
{
	this->battle = battle;
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
	{
		if(k==0)fread(&map[i][j].ground, sizeof(char), 1, fp);
			else if(k==1)fread(&map[i][j].object, sizeof(char), 1, fp);
			else if(k==2)fread(&map[i][j].unit, sizeof(char), 1, fp);
	}

	//이벤트 맵 데이터
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
		fread(&map[i][j].evnt, sizeof(char), 1, fp);

	//주인공 데이터 초기화
	unit[HERO].Set(HERO);
	ani.CreateAnimation(HERO, 32, 40, ani_exchange, 3, 20);
	ani.GetAni(HERO)->SetWalk(40, 120, 0, 80);

	//유닛 데이터 초기화
	unit_max=1;

	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	if(map[i][j].unit!=0xff)
	{
		//유닛이 서있을 장소에 유닛 배치
		unit[unit_max].Set(map[i][j].unit);
		unit[unit_max].SetPos(i, j);
		unit[unit_max].SetDir(/*rand()%4*/DIR_DOWN);
		
		//유닛 정보를 넘겨준 후 유닛의 ID로 교체
		map[i][j].unit = unit_max;
		unit[unit_max].SetEventNo(map[i][j].evnt);
		map[i][j].evnt = 0xff;

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

			read_success = true;
		}

		//설정값을 이동값으로 변환
		for(int j=0; j<2; ++j)
		for(int i=0; i<TILE_MAX; ++i)
		{
			if(tile_mov[j][i]==0)tile_mov[j][i]=99;
			else tile_mov[j][i]=tile_mov[j][i];
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

	//이동용 맵 생성
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	{
		if(map[i][j].object!=0xff)map[i][j].move = tile_mov[1][map[i][j].object];
		else map[i][j].move = tile_mov[0][map[i][j].ground];
		
		//이동력 0이면 이동 불가 지형
		if(map[i][j].move == 99)map[i][j].move = MOVE_DISABLE;
	}

	//기타 초기화
	active_unit = -1;

	//주인공 위치 초기화
	unit[HERO].SetPos(0, 0);
	InitScroll(HERO);
	event_no=-1;

	return true;
}

CSRPG::~CSRPG()
{
}

//유닛 생성
void CSRPG::CreateUnit()
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
void CSRPG::DrawGround()
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
			if(map[i][j].ground==k)ani.GetAni(ANI_TILE)->Draw(PIC_MAP, PIC_TILE1, i*TILESIZE-8, j*TILESIZE-8, k);
	}

	//오브젝트
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	if(map[i][j].object!=0xff)
	{
		SetRect(&tile_rect, map[i][j].object*TILESIZE, 8, (map[i][j].object+1)*TILESIZE, UNIT_Y);
		jdd->DrawPicture(PIC_MAP, PIC_TILE2, i*TILESIZE, j*TILESIZE, &tile_rect);
	}
}

//이동 가능성
int CSRPG::GetMoveSpeed(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return 0;

	//전투시
	if(battle)
	{		
		if(map[x][y].distance == MOVE_DISABLE || map[x][y].move == MOVE_DISABLE)return 0;	//이동 불가
		else if(map[x][y].object!=0xff)return Max(TILESIZE/(2<<map[x][y].move), 1);			//2층 타일에 의한 이동
		else return Max(TILESIZE/(2<<map[x][y].move), 1);									//일반 1층 타일 이동
	}
	//평상시
	else
	{
		if(map[x][y].unit!=0xff)return 0;									//유닛이 있으면 이동 불가능
		else if(map[x][y].object!=0xff)return TILESIZE/(2<<map[x][y].move);	//2층 타일에 의한 이동
		else if(tile_attr[0][map[x][y].ground]>0)return 0;					//평상시엔 일반 길 외에는 이동 불가능
		else return TILESIZE/(2<<map[x][y].move);							//일반 1층 타일 이동
	}

}

//충돌
int CSRPG::Crush(int x, int y)
{
	//화면 밖으로 나감
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_GO_OUT;

	//유닛 이벤트
	if(map[x][y].unit!=0xff)//return GetUnit(map[x][y].unit)->GetEventNo();
	{
		CUnit* unit;
		unit = GetUnit(map[x][y].unit);
		int ev_no = unit->GetEventNo();
		return ev_no;
	}

	//지형 이벤트
	if(map[x][y].evnt!=0xff)return map[x][y].evnt;

	return EVENT_NONE;
}

//대화
int CSRPG::Talk(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return EVENT_NONE;

	//유닛 이벤트
	if(map[x][y].unit!=0xff)return GetUnit(map[x][y].unit)->GetEventNo();

	//지형 이벤트
	if(map[x][y].evnt!=0xff)return map[x][y].evnt;

	return EVENT_NONE;
}

//유닛 이동
void CSRPG::UnitMove(int id,  int dir)
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
		if(event_no == 0)event_no = -1;	//0번 이벤트는 무시(특수 이벤트)
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
void CSRPG::InitScroll(int unit_id)
{
	next_scroll_x=scroll_x=MaxMin(unit[unit_id].GetRealX()-320, 0, x_size*TILESIZE-640);
	next_scroll_y=scroll_y=MaxMin(unit[unit_id].GetRealY()-240, 0, y_size*TILESIZE-480);
}

//포커스
void CSRPG::Focus(int unit_id)
{
	//X축
	int ux = MaxMin(unit[unit_id].GetRealX()-320, 0, x_size*TILESIZE-640);
	if(scroll_x < ux)
	{
		scroll_x = Min(scroll_x+4, ux);
	}
	else if(scroll_x > ux)
	{
		scroll_x = Max(scroll_x-4, ux);
	}

	//Y축
	int uy = MaxMin(unit[unit_id].GetRealY()-240, 0, y_size*TILESIZE-480);
	if(scroll_y < uy)
	{
		scroll_y = Min(scroll_y+4, uy);
	}
	else if(scroll_y > uy)
	{
		scroll_y = Max(scroll_y-4, uy);
	}
}

//조작
void CSRPG::Control()
{
	if(battle)BattleControl();
		else NormalControl();
}

//평상시 조작
void CSRPG::NormalControl()
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
	for(int i=0; i<unit_max; ++i)
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

//전투시 조작
void CSRPG::BattleControl()
{
	if(active_unit >=0)
	{
		//조종할 수 있는 아군
		if(mode==0)
		{
			if(unit[active_unit].team==0)
			{
				//이동
				if(GetKey(vkey_up,0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_UP);
						else unit[active_unit].SetDir(DIR_UP);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_down,0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_DOWN);
						else unit[active_unit].SetDir(DIR_DOWN);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_left,0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_LEFT);
						else unit[active_unit].SetDir(DIR_LEFT);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_right,0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_RIGHT);
						else unit[active_unit].SetDir(DIR_RIGHT);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_enter))
				{
					//명령창 생성
					int px = unit[active_unit].GetRealX()+TILESIZE-scroll_x;
					int py = unit[active_unit].GetRealY()+TILESIZE-scroll_y;
					if(px+100 > SCREEN_X)px-=100+TILESIZE;
					if(py+80 > SCREEN_Y)py-=100+TILESIZE;
					command.comdlg.SetDlg(px, py, 100, 4);
					command.Init(battle_commands);
					command.AddCom(1);
					command.AddCom(2);
					command.AddCom(3);

					mode = 1;
				}
			}
		}
		//명령 선택
		else if(mode==1)
		{
			if(result != -1)mode=2;
		}
		else if(mode==2)
		{
			//다른 유닛에게 차례를 넘김
			map[unit[active_unit].GetX()][unit[active_unit].GetY()].unit = active_unit;
			unit[active_unit].ap = 0;
			active_unit = -1;
		}

		return;
	}

	//이동하고 있는 유닛이 없을 때 다음 이동할 유닛 찾기
	bool moveable = false;
	for(int i=0; i<unit_max; ++i)
	if(unit[i].ap >= 100 && unit[i].GetLife())
	{
		if(unit[i].team>0)continue;
		SetActiveUnit(i);
		SetStartPoint(unit[i].GetX(), unit[i].GetY(), unit[i].move);
		moveable = true;
		mode = 0;
		break;
	}

	//AP 회복
	if(!moveable)
	{
		for(int i=0; i<unit_max; ++i)unit[i].ap += unit[i].quickness;
	}
}

void CSRPG::Process()
{
	//유닛들 행동
	for(int i=0; i<unit_max; ++i)unit[i].Action();

	//포커스
	if(battle)
	{
		if(active_unit>=0)Focus(active_unit);
	}
	else InitScroll(HERO);

	ani.Process();
}

void CSRPG::Draw()
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

	//2층 레이어
	for(int j=start_y; j<=end_y; ++j)
	for(int i=start_x; i<=end_x; ++i)
	{
		int px = i*TILESIZE-scroll_x;
		int py = j*TILESIZE-scroll_y;

		//2층
		if(map[i][j].object!=0xff)
		{
			RECT tile_rect;
			SetRect(&tile_rect, map[i][j].object*TILESIZE, 0, (map[i][j].object+1)*TILESIZE, 8);
			jdd->DrawPicture(backbuffer, PIC_TILE2, px, py-UNIT_PY, &tile_rect);
		}

		//일반 유닛
		if(map[i][j].unit!=0xff)
		{
			int p = map[i][j].unit;
			if(unit[p].move_bonus < SKY_MOVE)
			{
				ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
			}
		}
	}

	//이동 가능 범위
	if(battle)
	{
		static int delay;
		++delay;
		if(delay>=64)delay=0;

		float strlength = float(abs(32-delay))/100;
		JPictureInfo pi;
		pi.SetOpacity(strlength);
		jdd->SetPictureInfo("BLUE", &pi);

		for(int j=start_y; j<=end_y; ++j)
		for(int i=start_x; i<=end_x; ++i)
		{
			int px = i*TILESIZE-scroll_x;
			int py = j*TILESIZE-scroll_y;

			//전투시 이동 가능 범위
			if(map[i][j].distance != MOVE_DISABLE)
			{
				RECT tile_rect;
				SetRect(&tile_rect, px, py, px+TILESIZE, py+TILESIZE);
				jdd->DrawPicture(backbuffer, "BLUE", px, py, NULL);
			}
		}
	}

	//3층 레이어
	for(int j=start_y; j<=end_y; ++j)
	for(int i=start_x; i<=end_x; ++i)
	{
		int px = i*TILESIZE-scroll_x;
		int py = j*TILESIZE-scroll_y;

		//비행 유닛
		if(map[i][j].unit!=0xff)
		{
			int p = map[i][j].unit;
			if(unit[p].move_bonus >= SKY_MOVE)
			{
				ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
			}
		}
		//활성화된 유닛
		if(active_unit>=0 && i == unit[active_unit].GetX() && j == unit[active_unit].GetY())
		{
			int p = active_unit;
			ani.GetAni(p)->Draw(jdd->GetBackBufferID(), unit[p].GetPictureID(), px+unit[p].GetPX(), py+unit[p].GetPY()-UNIT_PY);
		}
	}

	//명령 선택
	if(battle && mode==1)result = command.CommandSelecting();
}

void CSRPG::SetHeroXY(int x, int y)
{
	x = MaxMin(x, 0, x_size-1);
	y = MaxMin(y, 0, y_size-1);
	unit[HERO].SetPos(x, y);	
	map[x][y].unit = HERO;
}

void CSRPG::SetAutoHeroXY(int ev)
{
	//일단 주인공을 맵 밖에 둠
	unit[HERO].SetPos(-1, 0);

	//이벤트 맵 데이터
	bool quit=false;
	for(int j=0; j<y_size && !quit; ++j)
	for(int i=0; i<x_size && !quit; ++i)
	if(map[i][j].evnt == ev)	//해당 이벤트 위치에 주인공 배치
	{
		unit[HERO].SetPos(i, j);
		map[i][j].unit = HERO;
		quit=true;
	}

	if(unit[HERO].GetX() == -1)unit[HERO].SetPos(0, 0);
}

CUnit* CSRPG::GetUnit(int id)
{
	return &unit[id];
}

//해당 위치 유닛 찾기
int CSRPG::FindUnit(int x, int y)
{
	for(int i=0; i<unit_max; ++i)
		if(unit[i].GetX()==x && unit[i].GetY()==y)return i;

	return -1;
}

//게임 저장
void CSRPG::SaveGame(FILE *save_fp)
{
	fwrite(map_name, sizeof(int), 80, save_fp);
	fwrite(&x_size, sizeof(char), 1, save_fp);
	fwrite(&y_size, sizeof(char), 1, save_fp);
	fwrite(map, sizeof(TileData), x_size*y_size, save_fp);
	fwrite(&scroll_x, sizeof(int), 1, save_fp);
	fwrite(&scroll_y, sizeof(int), 1, save_fp);
	fwrite(&next_scroll_x, sizeof(int), 1, save_fp);
	fwrite(&next_scroll_y, sizeof(int), 1, save_fp);
	fwrite(&unit_max, sizeof(int), 1, save_fp);
	fwrite(&unit, sizeof(CUnit), UNIT_MAX, save_fp);
}

//게임 불러오기
void CSRPG::LoadGame(FILE *load_fp)
{
	//맵 로드
	fread(map_name, sizeof(int), 80, load_fp);
	Load(map_name, battle);

	//정보 읽기
	fread(&x_size, sizeof(char), 1, load_fp);
	fread(&y_size, sizeof(char), 1, load_fp);
	fread(map, sizeof(TileData), x_size*y_size, load_fp);
	fread(&scroll_x, sizeof(int), 1, load_fp);
	fread(&scroll_y, sizeof(int), 1, load_fp);
	fread(&next_scroll_x, sizeof(int), 1, load_fp);
	fread(&next_scroll_y, sizeof(int), 1, load_fp);
	fread(&unit_max, sizeof(int), 1, load_fp);
	fread(&unit, sizeof(CUnit), UNIT_MAX, load_fp);
}

//이벤트 초기화
void CSRPG::InitEvent()
{
	event_no = -1;
}

//이벤트 번호 얻기
int CSRPG::GetEventNo()
{
	return event_no;
}

void CSRPG::SetStartPoint(int x, int y, int move_max)
{
	point.x=MaxMin(x, 0, x_size-1);
	point.y=MaxMin(y, 0, y_size-1);

	//맵 초기화
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			map[i][j].road=MOVE_DISABLE;

	//맵 초기화(임시)
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			map[i][j].distance=MOVE_DISABLE;

	//길 찾기
	FindRoad(point.x, point.y, 0, move_max);
}

void CSRPG::SetEndPoint(int x, int y)
{
	//맵 초기화
	for(int i=0; i<x_size; ++i)
		for(int j=0; j<y_size; ++j)
			map[i][j].distance=MOVE_DISABLE;

	//이동비용 검사
	FindRoad(x, y, 0);
}

void CSRPG::FindRoad(int x, int y, int expense, int move_max)
{
	if(move_max>=0 && expense>move_max)return;	//이동력 한계일 때는 더 이상 검사하지 않음
	if(map[x][y].unit != 0xff)return;			//유닛이 있으면 이동 불가능
	map[x][y].distance = expense;

	if(x>0 && map[x-1][y].distance>expense+map[x-1][y].move)
		FindRoad(x-1, y, expense+map[x-1][y].move, move_max);	//왼쪽 검사
	if(x<x_size-1 && map[x+1][y].distance>expense+map[x+1][y].move)
		FindRoad(x+1, y, expense+map[x+1][y].move, move_max);	//오른쪽 검사
	if(y>0 && map[x][y-1].distance>expense+map[x][y-1].move)
		FindRoad(x, y-1, expense+map[x][y-1].move, move_max);	//위 검사
	if(y<y_size-1 && map[x][y+1].distance>expense+map[x][y+1].move)
		FindRoad(x, y+1, expense+map[x][y+1].move, move_max);	//아래 검사
}

bool CSRPG::MoveNextPointFast(int move)
{
	//가장 좋은 길을 찾기
	int best_dir=-1, best_expense=map[point.x][point.y].distance;

	//왼쪽 검사
	if(point.x>0 && map[point.x-1][point.y].distance<best_expense && map[point.x-1][point.y].move<=move)
	{
		best_dir=0;
		best_expense=map[point.x-1][point.y].distance;
	}
	//오른쪽 검사
	if(point.x<x_size-1 && map[point.x+1][point.y].distance<best_expense && map[point.x+1][point.y].move<=move)
	{
		best_dir=1;
		best_expense=map[point.x+1][point.y].distance;
	}
	//위 검사
	if(point.y>0 && map[point.x][point.y-1].distance<best_expense && map[point.x][point.y-1].move<=move)
	{
		best_dir=2;
		best_expense=map[point.x][point.y-1].distance;
	}
	//아래 검사
	if(point.y<y_size-1 && map[point.x][point.y+1].distance<best_expense && map[point.x][point.y+1].move<=move)
	{
		best_dir=3;
		best_expense=map[point.x][point.y+1].distance;
	}

	//가장 가까운 곳으로 이동(아니면 가만히 있음)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//목적지에 도착하면 true, 아니면 false
	if(map[point.x][point.y].distance==0)return true;
		else return false;
}

bool CSRPG::MoveNextPoint(int move)
{
	//가장 좋은 길을 찾기
	int best_dir=-1, best_expense=map[point.x][point.y].distance;

	//첫째로 검사하는 곳을 랜덤하게 정하기 위한 플래그
	//(우선순위가 정해져 있으면 패턴이 단순해서 보기에 안 좋다.)
	bool left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=false;

	while(!left_ok || !right_ok || !up_ok || !down_ok)
	{
		int ran=rand()%4;

		//왼쪽 검사
		if(ran==0)
		{
			if(point.x>0 && map[point.x-1][point.y].distance<best_expense && map[point.x-1][point.y].move<=move)
			{
				best_dir=0;
				best_expense=map[point.x-1][point.y].distance;
			}
			left_ok=true;
		}
		//오른쪽 검사
		else if(ran==1)
		{
			if(point.x<x_size-1 && map[point.x+1][point.y].distance<best_expense && map[point.x+1][point.y].move<=move)
			{
				best_dir=1;
				best_expense=map[point.x+1][point.y].distance;
			}
			right_ok=true;
		}
		//위 검사
		else if(ran==2)
		{
			if(point.y>0 && map[point.x][point.y-1].distance<best_expense && map[point.x][point.y-1].move<=move)
			{
				best_dir=2;
				best_expense=map[point.x][point.y-1].distance;
			}
			up_ok=true;
		}
		//아래 검사
		else if(ran==3)
		{
			if(point.y<y_size-1 && map[point.x][point.y+1].distance<best_expense && map[point.x][point.y+1].move<=move)
			{
				best_dir=3;
				best_expense=map[point.x][point.y+1].distance;
			}
			down_ok=true;
		}
	}

	//가장 가까운 곳으로 이동(아니면 가만히 있음)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//목적지에 도착하면 true, 아니면 false
	if(map[point.x][point.y].distance==0)return true;
		else return false;
}

//유닛 활성화
void CSRPG::SetActiveUnit(int id)
{
	active_unit = id;
	map[unit[active_unit].GetX()][unit[active_unit].GetY()].unit = 0xff;	//활성화 유닛은 특수취급

	//유닛의 속성에 따라 이동용 맵 생성
	for(int j=0; j<y_size; ++j)
	for(int i=0; i<x_size; ++i)
	{
		if(unit[active_unit].move_bonus == NORMAL_MOVE)
		{
			if(map[i][j].object!=0xff)map[i][j].move = tile_mov[1][map[i][j].object];
			else map[i][j].move = tile_mov[0][map[i][j].ground];
		}
		else if(unit[active_unit].move_bonus == WATER_MOVE)
		{
			if(map[i][j].object!=0xff)map[i][j].move = tile_mov[1][map[i][j].object]+1;
			else if(tile_attr[0][map[i][j].ground] == 1)map[i][j].move = 1; //수상 유닛의 이동 보너스
			else map[i][j].move = tile_mov[0][map[i][j].ground]+1;
		}
		else if(unit[active_unit].move_bonus == FIRE_MOVE)
		{
			if(map[i][j].object!=0xff)map[i][j].move = tile_mov[1][map[i][j].object];
			else if(tile_attr[0][map[i][j].ground] == 2)map[i][j].move = 1; //불 유닛의 이동 보너스
			else map[i][j].move = tile_mov[0][map[i][j].ground];
		}
		else if(unit[active_unit].move_bonus == SPECIAL_MOVE)
		{
			if(map[i][j].object!=0xff)map[i][j].move = tile_mov[1][map[i][j].object];
			else map[i][j].move = tile_mov[0][map[i][j].ground];

			if(map[i][j].move != 99)map[i][j].move = 2;		//이동 가능한 모든 지형은 똑같은 힘으로 이동
		}
		else if(unit[active_unit].move_bonus == SKY_MOVE)
		{
			if(tile_mov[0][map[i][j].ground] != 99)map[i][j].move = 2;	//이동 불가 지형(벽, 암흑지역) 빼고 모두 보통 이동력
			else map[i][j].move = 99;
		}
		else if(unit[active_unit].move_bonus == GHOST_MOVE)
		{
			map[i][j].move = 2;		//무조건  통과
		}

		//이동력 0이면 이동 불가 지형
		if(map[i][j].move == 99)map[i][j].move = MOVE_DISABLE;
	}
}