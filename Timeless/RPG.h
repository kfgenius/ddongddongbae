#include <stdio.h>

#include "GameMap.h"

//////////////////////////////
//���� Ŭ����
class CRPG : public CGameMap
{
private:
	//���� �帧 ����
	void Control();		//Ű �Է�
	void Process();		//���� ó��

	//������, �Ҹ���
	CRPG();
	~CRPG();
};
