#include "GameProcess.h"
#include "GameMap.h"

//////////////////////////////
//���� Ŭ����
class CRPG : public CGameProcess
{
private:
	//�� ������
	CGameMap* m_game_map;

	//���� �帧 ����
	void Control();		//Ű �Է�
	void Process();		//���� ó��

	//�̵� �ӵ� ���
	int GetMoveSpeed(int x, int y);

public:
	//������, �Ҹ���
	CRPG();
	~CRPG();
};
