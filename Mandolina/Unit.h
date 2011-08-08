//���� ����
#define DIR_UP		0
#define DIR_LEFT	1
#define DIR_DOWN	2
#define DIR_RIGHT	3

//�̵� ���
#define NORMAL_MOVE		0	//����
#define WATER_MOVE		1	//��
#define FIRE_MOVE		2	//��
#define SPECIAL_MOVE	3	//���ݴ�
#define SKY_MOVE		4	//����
#define GHOST_MOVE		5	//����


/////////////////////////////
//�� �����Ϳ��� �Ѿ���� ��
struct CUnitData
{
	int hp;
	int mp;

	int pow;
	int mpow;
	int def;
	int mdef;
	int agl;
	int mov;
	int jump;
	int swim;
	int see;
	int act;

	int attr;
};

//////////////////////////////
//���� Ŭ����
class CUnit
{
private:
	bool life;
	int id;
	int pic_id;
	int x, y;		//��ǥ
	int px, py;		//��� ���� ��ǥ ������ ���� ����(x * TILESIZE + px ������ ���)

	//�̵� ����
	bool moving;	//�̵� ������ Ȯ��(�̵��߿��� �̵� �Ұ���)
	int move_speed;	//�̵��� ���ǵ�(�Ͻ���)
	int dir;		//�ٶ󺸴� ����

	//�� ����
	int tile_size;	//���� Ÿ�� ũ��, ������ �־� �ش�.

public:
	//�ɷ�ġ
	int lv, exp, next_exp;
	int hp, hp_max;
	int mp, mp_max;
	int power, defense, wisdom, quickness, move, ap;
	int pattern;
	int move_bonus;		//�̵� ���ʽ�(���̳� ��)

	//��
	int team;

	//�߻� �̺�Ʈ
	int event_no;

	//���� ��ġ, ���� ����
	void Set(int id, int team, int tile_size);
	void Die();
	bool GetLife();

	//ID ���
	int GetID();
	int GetPictureID();

	//��ǥ ���
	int GetX();
	int GetY();
	int GetPX();
	int GetPY();
	int GetRealX();
	int GetRealY();

	//���� ���, ����
	int GetDir();
	void SetDir(int dir);

	//�ٶ󺸰� �ִ� �� ���ϱ�
	int GetDirX();
	int GetDirY();

	//�̺�Ʈ ��ȣ ����/���ϱ�
	void SetEventNo(int event_no);
	int GetEventNo();

	//��ġ ����
	void SetPos(int x, int y);

	//�̵�
	bool Move(int dir);
	void Action();
	void SetMoveSpeed(int move_speed);

	//������
	CUnit();
};