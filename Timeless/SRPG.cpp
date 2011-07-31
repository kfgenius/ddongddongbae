#include "srpg.h"
#include "donglib.h"

#define COMMAND_WINDOW_WIDTH	100

//명령
char* battle_commands[] = {"공격", "마법", "휴식", "대기"};

enum
{
	mode_control_unit,
	mode_enemy_action,
	mode_command_select,
	mode_next_unit
};

////////////////////////////////////////////////////////////
// CSRPG 메소드

CSRPG::CSRPG(char* map_name, char* tile1_name, char* tile2_name) : CRPG(map_name, tile1_name, tile2_name)
{
	jdd->LoadPicture("BLUE", "DATA\\blue.gif", NULL, TRUE);	//이동 가능 영역

	for(int i = 0; i < x_size; i++)
	{
		for(int j = 0; j < y_size; j++)
		{
			map[i][j].distance = MOVE_DISABLE;
		}
	}

	command = new CCommand(battle_commands, 0, 0, 630, 4);
}

CSRPG::~CSRPG()
{
	delete command;
	command = NULL;
}

//이동 가능성
int CSRPG::GetMoveSpeed(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return 0;

	if(map[x][y].distance == MOVE_DISABLE || map[x][y].move == MOVE_DISABLE)return 0;	//이동 불가
	else if(map[x][y].object != 0xff)return Max(TILE_SIZE / (2 << map[x][y].move), 1);			//2층 타일에 의한 이동
	else return Max(TILE_SIZE/(2 << map[x][y].move), 1);									//일반 1층 타일 이동
}

//조작
void CSRPG::Control()
{
	if(active_unit >=0)
	{
		//조종할 수 있는 아군
		if(mode == mode_control_unit)
		{
			if(unit[active_unit].team == party_ally)
			{
				//이동
				if(GetKey(vkey_up, 0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_UP);
						else unit[active_unit].SetDir(DIR_UP);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_down, 0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_DOWN);
						else unit[active_unit].SetDir(DIR_DOWN);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_left, 0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_LEFT);
						else unit[active_unit].SetDir(DIR_LEFT);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_right, 0))
				{
					if(!_GetKeyState(VK_SHIFT))UnitMove(active_unit, DIR_RIGHT);
						else unit[active_unit].SetDir(DIR_RIGHT);
					ani.GetAni(active_unit)->SetDirection((WalkDirection)unit[active_unit].GetDir());
				}
				else if(GetKey(vkey_enter))
				{
					//명령창 위치
					int px = unit[active_unit].GetRealX() + TILE_SIZE-scroll_x;
					int py = unit[active_unit].GetRealY() + TILE_SIZE-scroll_y;
					if(px + COMMAND_WINDOW_WIDTH > SCREEN_WIDTH)
					{
						px -= COMMAND_WINDOW_WIDTH + TILE_SIZE;
					}
					if(py + 80 > SCREEN_HEIGHT)
					{
						py -= COMMAND_WINDOW_WIDTH + TILE_SIZE;
					}

					//명령창 생성
					command->com_dlg.SetDlg(px, py, COMMAND_WINDOW_WIDTH, 4);
					command->com_dlg.MakeDlgBox("_commandbox");
					command->Init(battle_commands);
					command->AddCom(1);
					command->AddCom(2);
					command->AddCom(3);

					mode = mode_command_select;
				}
			}
		}
		//명령 선택
		else if(mode == mode_command_select)
		{
			if(result != -1)
			{
				mode = mode_next_unit;
			}
		}
		else if(mode == mode_next_unit)
		{
			//다른 유닛에게 차례를 넘김
			map[unit[active_unit].GetX()][unit[active_unit].GetY()].unit = active_unit;
			unit[active_unit].ap = 0;
			active_unit = -1;
		}

		return;
	}

	//이동하고 있는 유닛이 없을 때 다음 이동할 유닛 찾기
	bool moveable = FALSE;
	for(int i = 0; i < unit_max; i++)
	{
		if(unit[i].ap >= 100 && unit[i].GetLife())
		{
			//적의 움직임
			if(unit[i].team != party_ally)
			{
				continue;
			}

			SetActiveUnit(i);
			SetStartPoint(unit[i].GetX(), unit[i].GetY(), unit[i].move);
			moveable = TRUE;
			mode = mode_control_unit;
			break;
		}
	}

	//AP 회복
	if(!moveable)
	{
		for(int i=0; i<unit_max; i++)
		{
			unit[i].ap += unit[i].quickness;
		}
	}
}

//이동 가능 범위 표시
void CSRPG::ShowMoveAble()
{
	//페이드인이 끝난 후 표시
	if(first_fade_in)
	{
		return;
	}

	//찍을 범위
	int start_x = Max((scroll_x / TILE_SIZE) - 1, 0);
	int start_y = Max((scroll_y / TILE_SIZE) - 1, 0);
	int end_x = Min(start_x + (SCREEN_WIDTH / TILE_SIZE) + 1, x_size - 1);
	int end_y = Min(start_y + (SCREEN_HEIGHT / TILE_SIZE) + 1, y_size - 1);

	//투명해졌다가 뚜렷해지는 효과
	static int delay;
	++delay;
	if(delay >= 64)
	{
		delay = 0;
	}

	float strlength = float(abs(32 - delay)) / 100;
	JPictureInfo pi;
	pi.SetOpacity(strlength);
	jdd->SetPictureInfo("BLUE", &pi);

	for(int j = start_y; j <= end_y; j++)
	{
		for(int i = start_x; i <= end_x; i++)
		{
			int px = i * TILE_SIZE - scroll_x;
			int py = j * TILE_SIZE - scroll_y;

			//전투시 이동 가능 범위
			if(map[i][j].distance != MOVE_DISABLE)
			{
				RECT tile_rect;
				SetRect(&tile_rect, px, py, px + TILE_SIZE, py + TILE_SIZE);
				jdd->DrawPicture(backbuffer, "BLUE", px, py, NULL);
			}
		}
	}
}

void CSRPG::Process()
{
	InitEvent();

	if(!m_script->IsReady())
	{
		Control();
	}

	//유닛들 행동
	for(int i=0; i<unit_max; i++)
	{
		unit[i].Action();
	}

	//포커스
	if(active_unit >= 0)
	{
		Focus(active_unit);
	}

	//애니메이션 처리
	ani.Process();

	//출력
	Draw1F();
	ShowMoveAble();
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

	//명령 선택
	if(mode == mode_command_select)
	{
		result = command->CommandSelecting();
	}

	m_script->Scripting();

	CGameProcess::Process();
}

void CSRPG::SetStartPoint(int x, int y, int move_max)
{
	point.x = MaxMin(x, 0, x_size-1);
	point.y = MaxMin(y, 0, y_size-1);

	//맵 초기화
	for(int i = 0; i < x_size; i++)
	{
		for(int j = 0; j < y_size; j++)
		{
			map[i][j].road = MOVE_DISABLE;
		}
	}

	//맵 초기화(임시)
	for(int i = 0; i < x_size; i++)
	{
		for(int j = 0; j < y_size; j++)
		{
			map[i][j].distance = MOVE_DISABLE;
		}
	}

	//길 찾기
	FindRoad(point.x, point.y, 0, move_max);
}

void CSRPG::SetEndPoint(int x, int y)
{
	//맵 초기화
	for(int i = 0; i < x_size; i++)
	{
		for(int j = 0; j < y_size; j++)
		{
			map[i][j].distance = MOVE_DISABLE;
		}
	}

	//이동비용 검사
	FindRoad(x, y, 0);
}

void CSRPG::FindRoad(int x, int y, int expense, int move_max)
{
	if(move_max >= 0 && expense > move_max)return;	//이동력 한계일 때는 더 이상 검사하지 않음
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
	if(point.x > 0 && map[point.x-1][point.y].distance < best_expense && map[point.x-1][point.y].move<=move)
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

	//목적지에 도착하면 TRUE, 아니면 FALSE
	if(map[point.x][point.y].distance==0)return TRUE;
		else return FALSE;
}

bool CSRPG::MoveNextPoint(int move)
{
	//가장 좋은 길을 찾기
	int best_dir=-1, best_expense=map[point.x][point.y].distance;

	//첫째로 검사하는 곳을 랜덤하게 정하기 위한 플래그
	//(우선순위가 정해져 있으면 패턴이 단순해서 보기에 안 좋다.)
	bool left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;

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
			left_ok=TRUE;
		}
		//오른쪽 검사
		else if(ran==1)
		{
			if(point.x<x_size-1 && map[point.x+1][point.y].distance<best_expense && map[point.x+1][point.y].move<=move)
			{
				best_dir=1;
				best_expense=map[point.x+1][point.y].distance;
			}
			right_ok=TRUE;
		}
		//위 검사
		else if(ran==2)
		{
			if(point.y>0 && map[point.x][point.y-1].distance<best_expense && map[point.x][point.y-1].move<=move)
			{
				best_dir=2;
				best_expense=map[point.x][point.y-1].distance;
			}
			up_ok=TRUE;
		}
		//아래 검사
		else if(ran==3)
		{
			if(point.y<y_size-1 && map[point.x][point.y+1].distance<best_expense && map[point.x][point.y+1].move<=move)
			{
				best_dir=3;
				best_expense=map[point.x][point.y+1].distance;
			}
			down_ok=TRUE;
		}
	}

	//가장 가까운 곳으로 이동(아니면 가만히 있음)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//목적지에 도착하면 TRUE, 아니면 FALSE
	if(map[point.x][point.y].distance==0)return TRUE;
		else return FALSE;
}

//유닛 활성화
void CSRPG::SetActiveUnit(int id)
{
	active_unit = id;
	map[unit[active_unit].GetX()][unit[active_unit].GetY()].unit = 0xff;	//활성화 유닛은 특수취급

	//유닛의 속성에 따라 이동용 맵 생성
	for(int j = 0; j < y_size; j++)
	{
		for(int i = 0; i < x_size; i++)
		{
			if(unit[active_unit].move_bonus == NORMAL_MOVE)
			{
				if(map[i][j].object != 0xff)
				{
					map[i][j].move = tile_mov[1][map[i][j].object];
				}
				else
				{
					map[i][j].move = tile_mov[0][map[i][j].ground];
				}
			}
			else if(unit[active_unit].move_bonus == WATER_MOVE)
			{
				if(map[i][j].object != 0xff)
				{
					map[i][j].move = tile_mov[1][map[i][j].object] + 1;
				}
				else if(tile_attr[0][map[i][j].ground] == 1)
				{
					map[i][j].move = 1; //수상 유닛의 이동 보너스
				}
				else
				{
					map[i][j].move = tile_mov[0][map[i][j].ground] + 1;	//지상 이동 핸디캡
				}
			}
			else if(unit[active_unit].move_bonus == FIRE_MOVE)
			{
				if(map[i][j].object != 0xff)
				{
					map[i][j].move = tile_mov[1][map[i][j].object];
				}
				else if(tile_attr[0][map[i][j].ground] == 2)
				{
					map[i][j].move = 1; //불 유닛의 이동 보너스
				}
				else
				{
					map[i][j].move = tile_mov[0][map[i][j].ground];
				}
			}
			else if(unit[active_unit].move_bonus == SPECIAL_MOVE)
			{
				if(map[i][j].object != 0xff)
				{
					map[i][j].move = tile_mov[1][map[i][j].object];
				}
				else
				{
					map[i][j].move = tile_mov[0][map[i][j].ground];
				}

				if(map[i][j].move != 99)
				{
					map[i][j].move = 2;		//이동 가능한 모든 지형은 똑같은 힘으로 이동
				}
			}
			else if(unit[active_unit].move_bonus == SKY_MOVE)
			{
				if(tile_mov[0][map[i][j].ground] != 99)
				{
					map[i][j].move = 2;	//이동 불가 지형(벽, 암흑지역) 빼고 모두 보통 이동력
				}
				else
				{
					map[i][j].move = 99;
				}
			}
			else if(unit[active_unit].move_bonus == GHOST_MOVE)
			{
				map[i][j].move = 2;		//무조건  통과
			}

			//이동력 0이면 이동 불가 지형
			if(map[i][j].move == 99)
			{
				map[i][j].move = MOVE_DISABLE;
			}
		}
	}
}