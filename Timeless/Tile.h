#pragma once

class CTile
{
public:
	int ground;		//�ٴ�
	int object;		//���� ����
	int unit;		//�� �ִ� ����
	int evnt;		//�̺�Ʈ
	
	//�� ã���
	int waste_move;		//�ʿ� �̵���
	int road;			//���� �˻��ϴ� �ʿ�
	int distance;		//�Ÿ��� ��Ÿ���� �ʿ�

	CTile(void);
	~CTile(void);
};
