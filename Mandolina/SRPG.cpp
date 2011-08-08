#include "srpg.h"
#include "donglib.h"

#define COMMAND_WINDOW_WIDTH	100

//���
char* battle_commands[] = {"����", "����", "�޽�", "���"};

enum
{
	mode_control_unit,
	mode_enemy_action,
	mode_command_select,
	mode_next_unit
};

////////////////////////////////////////////////////////////
// CSRPG �޼ҵ�

CSRPG::CSRPG(char* map_name, char* tile1_name, char* tile2_name) : CRPG(map_name, tile1_name, tile2_name)
{
	jdd->LoadPicture("BLUE", "DATA\\blue.gif", NULL, TRUE);	//�̵� ���� ����

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

//�̵� ���ɼ�
int CSRPG::GetMoveSpeed(int x, int y)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return 0;

	if(map[x][y].distance == MOVE_DISABLE || map[x][y].move == MOVE_DISABLE)return 0;	//�̵� �Ұ�
	else if(map[x][y].object != 0xff)return Max(TILE_SIZE / (2 << map[x][y].move), 1);			//2�� Ÿ�Ͽ� ���� �̵�
	else return Max(TILE_SIZE/(2 << map[x][y].move), 1);									//�Ϲ� 1�� Ÿ�� �̵�
}

//����
void CSRPG::Control()
{
	if(active_unit >=0)
	{
		//������ �� �ִ� �Ʊ�
		if(mode == mode_control_unit)
		{
			if(unit[active_unit].team == party_ally)
			{
				//�̵�
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
					//���â ��ġ
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

					//���â ����
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
		//��� ����
		else if(mode == mode_command_select)
		{
			if(result != -1)
			{
				mode = mode_next_unit;
			}
		}
		else if(mode == mode_next_unit)
		{
			//�ٸ� ���ֿ��� ���ʸ� �ѱ�
			map[unit[active_unit].GetX()][unit[active_unit].GetY()].unit = active_unit;
			unit[active_unit].ap = 0;
			active_unit = -1;
		}

		return;
	}

	//�̵��ϰ� �ִ� ������ ���� �� ���� �̵��� ���� ã��
	bool moveable = FALSE;
	for(int i = 0; i < unit_max; i++)
	{
		if(unit[i].ap >= 100 && unit[i].GetLife())
		{
			//���� ������
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

	//AP ȸ��
	if(!moveable)
	{
		for(int i=0; i<unit_max; i++)
		{
			unit[i].ap += unit[i].quickness;
		}
	}
}

//�̵� ���� ���� ǥ��
void CSRPG::ShowMoveAble()
{
	//���̵����� ���� �� ǥ��
	if(first_fade_in)
	{
		return;
	}

	//���� ����
	int start_x = Max((scroll_x / TILE_SIZE) - 1, 0);
	int start_y = Max((scroll_y / TILE_SIZE) - 1, 0);
	int end_x = Min(start_x + (SCREEN_WIDTH / TILE_SIZE) + 1, x_size - 1);
	int end_y = Min(start_y + (SCREEN_HEIGHT / TILE_SIZE) + 1, y_size - 1);

	//���������ٰ� �ѷ������� ȿ��
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

			//������ �̵� ���� ����
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

	//���ֵ� �ൿ
	for(int i=0; i<unit_max; i++)
	{
		unit[i].Action();
	}

	//��Ŀ��
	if(active_unit >= 0)
	{
		Focus(active_unit);
	}

	//�ִϸ��̼� ó��
	ani.Process();

	//���
	Draw1F();
	ShowMoveAble();
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

	//��� ����
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

	//�� �ʱ�ȭ
	for(int i = 0; i < x_size; i++)
	{
		for(int j = 0; j < y_size; j++)
		{
			map[i][j].road = MOVE_DISABLE;
		}
	}

	//�� �ʱ�ȭ(�ӽ�)
	for(int i = 0; i < x_size; i++)
	{
		for(int j = 0; j < y_size; j++)
		{
			map[i][j].distance = MOVE_DISABLE;
		}
	}

	//�� ã��
	FindRoad(point.x, point.y, 0, move_max);
}

void CSRPG::SetEndPoint(int x, int y)
{
	//�� �ʱ�ȭ
	for(int i = 0; i < x_size; i++)
	{
		for(int j = 0; j < y_size; j++)
		{
			map[i][j].distance = MOVE_DISABLE;
		}
	}

	//�̵���� �˻�
	FindRoad(x, y, 0);
}

void CSRPG::FindRoad(int x, int y, int expense, int move_max)
{
	if(move_max >= 0 && expense > move_max)return;	//�̵��� �Ѱ��� ���� �� �̻� �˻����� ����
	if(map[x][y].unit != 0xff)return;			//������ ������ �̵� �Ұ���
	map[x][y].distance = expense;

	if(x>0 && map[x-1][y].distance>expense+map[x-1][y].move)
		FindRoad(x-1, y, expense+map[x-1][y].move, move_max);	//���� �˻�
	if(x<x_size-1 && map[x+1][y].distance>expense+map[x+1][y].move)
		FindRoad(x+1, y, expense+map[x+1][y].move, move_max);	//������ �˻�
	if(y>0 && map[x][y-1].distance>expense+map[x][y-1].move)
		FindRoad(x, y-1, expense+map[x][y-1].move, move_max);	//�� �˻�
	if(y<y_size-1 && map[x][y+1].distance>expense+map[x][y+1].move)
		FindRoad(x, y+1, expense+map[x][y+1].move, move_max);	//�Ʒ� �˻�
}

bool CSRPG::MoveNextPointFast(int move)
{
	//���� ���� ���� ã��
	int best_dir=-1, best_expense=map[point.x][point.y].distance;

	//���� �˻�
	if(point.x > 0 && map[point.x-1][point.y].distance < best_expense && map[point.x-1][point.y].move<=move)
	{
		best_dir=0;
		best_expense=map[point.x-1][point.y].distance;
	}
	//������ �˻�
	if(point.x<x_size-1 && map[point.x+1][point.y].distance<best_expense && map[point.x+1][point.y].move<=move)
	{
		best_dir=1;
		best_expense=map[point.x+1][point.y].distance;
	}
	//�� �˻�
	if(point.y>0 && map[point.x][point.y-1].distance<best_expense && map[point.x][point.y-1].move<=move)
	{
		best_dir=2;
		best_expense=map[point.x][point.y-1].distance;
	}
	//�Ʒ� �˻�
	if(point.y<y_size-1 && map[point.x][point.y+1].distance<best_expense && map[point.x][point.y+1].move<=move)
	{
		best_dir=3;
		best_expense=map[point.x][point.y+1].distance;
	}

	//���� ����� ������ �̵�(�ƴϸ� ������ ����)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//�������� �����ϸ� TRUE, �ƴϸ� FALSE
	if(map[point.x][point.y].distance==0)return TRUE;
		else return FALSE;
}

bool CSRPG::MoveNextPoint(int move)
{
	//���� ���� ���� ã��
	int best_dir=-1, best_expense=map[point.x][point.y].distance;

	//ù°�� �˻��ϴ� ���� �����ϰ� ���ϱ� ���� �÷���
	//(�켱������ ������ ������ ������ �ܼ��ؼ� ���⿡ �� ����.)
	bool left_ok, right_ok, up_ok, down_ok;
	left_ok=right_ok=up_ok=down_ok=FALSE;

	while(!left_ok || !right_ok || !up_ok || !down_ok)
	{
		int ran=rand()%4;

		//���� �˻�
		if(ran==0)
		{
			if(point.x>0 && map[point.x-1][point.y].distance<best_expense && map[point.x-1][point.y].move<=move)
			{
				best_dir=0;
				best_expense=map[point.x-1][point.y].distance;
			}
			left_ok=TRUE;
		}
		//������ �˻�
		else if(ran==1)
		{
			if(point.x<x_size-1 && map[point.x+1][point.y].distance<best_expense && map[point.x+1][point.y].move<=move)
			{
				best_dir=1;
				best_expense=map[point.x+1][point.y].distance;
			}
			right_ok=TRUE;
		}
		//�� �˻�
		else if(ran==2)
		{
			if(point.y>0 && map[point.x][point.y-1].distance<best_expense && map[point.x][point.y-1].move<=move)
			{
				best_dir=2;
				best_expense=map[point.x][point.y-1].distance;
			}
			up_ok=TRUE;
		}
		//�Ʒ� �˻�
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

	//���� ����� ������ �̵�(�ƴϸ� ������ ����)
	if(best_dir==0)--point.x;
		else if(best_dir==1)++point.x;
		else if(best_dir==2)--point.y;
		else if(best_dir==3)++point.y;

	//�������� �����ϸ� TRUE, �ƴϸ� FALSE
	if(map[point.x][point.y].distance==0)return TRUE;
		else return FALSE;
}

//���� Ȱ��ȭ
void CSRPG::SetActiveUnit(int id)
{
	active_unit = id;
	map[unit[active_unit].GetX()][unit[active_unit].GetY()].unit = 0xff;	//Ȱ��ȭ ������ Ư�����

	//������ �Ӽ��� ���� �̵��� �� ����
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
					map[i][j].move = 1; //���� ������ �̵� ���ʽ�
				}
				else
				{
					map[i][j].move = tile_mov[0][map[i][j].ground] + 1;	//���� �̵� �ڵ�ĸ
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
					map[i][j].move = 1; //�� ������ �̵� ���ʽ�
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
					map[i][j].move = 2;		//�̵� ������ ��� ������ �Ȱ��� ������ �̵�
				}
			}
			else if(unit[active_unit].move_bonus == SKY_MOVE)
			{
				if(tile_mov[0][map[i][j].ground] != 99)
				{
					map[i][j].move = 2;	//�̵� �Ұ� ����(��, ��������) ���� ��� ���� �̵���
				}
				else
				{
					map[i][j].move = 99;
				}
			}
			else if(unit[active_unit].move_bonus == GHOST_MOVE)
			{
				map[i][j].move = 2;		//������  ���
			}

			//�̵��� 0�̸� �̵� �Ұ� ����
			if(map[i][j].move == 99)
			{
				map[i][j].move = MOVE_DISABLE;
			}
		}
	}
}