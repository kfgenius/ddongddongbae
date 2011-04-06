#pragma once

#include "GameProcess.h"
#include "RPG.h"

//////////////////////////////
//���� Ŭ����
class CSRPG : public CRPG
{
private:
	CCommand* command;	//���

	void Control();		//����

public:
	//���� �帧 ����
	void Process();		//���� ó��

	//���� ����
	void SetActiveUnit(int id);		//Ȱ��ȭ ���� ����

	//�̵� �ӵ�
	int GetMoveSpeed(int x, int y);
	
	//�̵� ���� ���� ����
	void ShowMoveAble();

	//��ã�� ����
	void SetStartPoint(int x, int y, int move_max);
	void SetEndPoint(int x, int y);
	void FindRoad(int x, int y, int expense, int move_max=99);
	bool MoveNextPointFast(int move);
	bool MoveNextPoint(int move);
	bool MoveNextPointStupid(int move, int iq);

	//������, �Ҹ���
	CSRPG(char* map_name, char* tile1_name, char* tile2_name);
	~CSRPG();
};
