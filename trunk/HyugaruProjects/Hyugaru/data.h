#define CUSTOM_BLUE	JColor(149,190,209)
#define YELLOW		JColor(255,255,0)

HWND hwnd;

//�̵� ����
BOOL ReplayFlag;

BOOL _MidiPlay(char* pszMidiFN, BOOL bReplayFlag = TRUE)
{
    char szMCISendString[256];

    wsprintf(szMCISendString,"open %s type sequencer alias MUSIC", pszMidiFN);

    if ( mciSendString ( "close all", NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( szMCISendString, NULL, 0, NULL ) != 0 ) return ( FALSE );
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, hwnd ) != 0) return(FALSE);

    ReplayFlag = bReplayFlag; 
    return TRUE;
}

BOOL _MidiStop()
{
    if ( mciSendString ( "close all", NULL, 0, NULL) != 0 ) return ( FALSE );
    return TRUE;
}

BOOL _MidiReplay()
{
    if ( mciSendString ( "play MUSIC from 0 notify", NULL, 0, hwnd) != 0 ) return ( FALSE );
    return TRUE;
}

//////////////////////////////////////////////////////
//��� Ŭ����
class CEquip
{
public:
	char* name;
	int pow;
	int price;
	char type;
	char part;

	~CEquip();
	void Set(char* vname, int vpow, int vprice, char vtype, char vpart);
};

CEquip::~CEquip()
{
	if(name!=NULL)
	{
		delete[] name;
		name=NULL;
	}
}

void CEquip::Set(char* vname, int vpow, int vprice, char vtype, char vpart)
{
	if(name!=NULL)
	{
		delete [] name;
		name=NULL;
	}

	name=new char[strlen(vname)+1];
	strcpy(name, vname);
	name[strlen(vname)]=NULL;

	pow=vpow;
	price=vprice;
	type=vtype;
	part=vpart;
}

//��� ������ ��
#define EQUIP_MAX 17
CEquip equipment[EQUIP_MAX];

//////////////////////////////////////////////////////
//ĳ���� Ŭ����

//ĳ���� ID
#define HERO1		0
#define HERO2		1
#define HERO3		2
#define HERO4		3
#define HERO5		4
#define HERO6		5
#define SOLDIER1	6
#define SOLDIER2	7
#define SOLDIER3	8
#define BANDIT		9
#define BANG		10
#define BIGROCK		11
#define HANAZUM		12
#define KUOT		13
#define MAEQO		14
#define DARKQUIZ	15
#define NEYUM		16

//��� ���
#define EQ_ARMS		0
#define EQ_AMMOR	1

class CCaracter
{
public:
	//���� ĳ������ ����Ǵ� ����
	char* name;
	int hp_max;
	int move;
	int mizi_type;
	int move_type;

	//ǥ���� �׸�
	int normal_pic;
	int poison_pic;
	int command[4];

	void SetCaracter(char* vname, int vhp_max, int vmove, int vmizi_type, int vmove_type, int com1, int com2=-1, int com3=-1, int com4=-1);
	~CCaracter();
};

void CCaracter::SetCaracter(char* vname, int vhp_max, int vmove, int vmizi_type, int vmove_type, int com1, int com2, int com3, int com4)
{
	if(name!=NULL)
	{
		delete[] name;
		name=NULL;
	}

	name=new char[strlen(vname)+1];
	strcpy(name, vname);
	name[strlen(vname)]=NULL;

	hp_max=vhp_max;
	move=vmove;
	mizi_type=vmizi_type;
	move_type=vmove_type;

	command[0]=com1;
	command[1]=com2;
	command[2]=com3;
	command[3]=com4;
}

CCaracter::~CCaracter()
{
	delete[] name;
	name=NULL;
}

CCaracter cara[17];

//////////////////////////////////////////////////////
//���� Ŭ����
class CUnit
{
public:
	//�⺻ ����
	bool life, soul;
	char team;

	char caracter;
	int x,y;
	int sx, sy;			//�ִϸ��̼��� ���� Ư�� ��ǥ
	int hp, hp_max;
	int move;
	int mizi_type;
	int move_type;

	//���, �ɷ�
	int arms;
	int str;
	int ammor;
	int dp;
	int mizi_level;
	int skill[4];

	//����
	int wait;
	char state;			//0: ����	1: �� 	2: �͵�   3: �ູ   4: ���� �ູ

	//��Ƽ������ ��ȣ(�Ʊ��� �ش�)
	int party_id;

	void SetUnit(int vx, int vy, char vcaracter, char vteam);
	void Equip(int equip_id);
	void TakeOff(char part);

	CUnit& operator=(const CUnit& src_unit);
};

void CUnit::SetUnit(int vx, int vy, char vcaracter, char vteam)
{
	life=soul=true;

	caracter=vcaracter;
	x=vx;
	y=vy;
	sx=sy=0;
	team=vteam;
	
	//ĳ���� ���� �Ӽ� �ޱ�
	hp=cara[caracter].hp_max;
	hp_max=cara[caracter].hp_max;
	move=cara[caracter].move;
	mizi_type=cara[caracter].mizi_type;
	move_type=cara[caracter].move_type;

	wait=100;
	state=0;

	//ĳ������ �ʱ� ����(�Ʊ��� ��� �������� �ʾҴٰ� ����)
	str=1;
	arms=0;
	ammor=-1;
	dp=0;
	mizi_level=1;
	for(int i=0; i<4; i++)skill[i]=-1;

	//���� ����
	if(caracter==HERO1)			//�ް���
	{
		skill[0]=0;
		skill[1]=1;
	}
	else if(caracter==HERO2)	//���̾�
	{
		skill[0]=2;
		skill[1]=3;
	}
	else if(caracter==HERO3)	//��ũ��
	{
		skill[0]=4;
		skill[1]=5;
	}
	else if(caracter==HERO4)	//�级��
	{
		str=3;
		skill[0]=6;
		skill[1]=7;
	}
	else if(caracter==HERO5)	//�̹���
	{
		skill[0]=8;
	}
	else if(caracter==HERO6)	//�޷��
	{
		mizi_level=4;
		skill[0]=9;
		skill[1]=10;
	}
	else if(caracter==DARKQUIZ)	//��ũ����
	{
		mizi_level=4;
		skill[0]=11;
		skill[1]=12;
	}
	else if(caracter==BANG)
	{
		Equip(10);
	}
	else if(caracter==MAEQO)
	{
		Equip(11);
	}
	else if(caracter==KUOT)
	{
		Equip(12);
	}
	//���� ��쿡 ĳ���� ����
	if(team!=0)
	{
		if(caracter==HERO2)
		{
			Equip(4);
			Equip(14);
		}
		else if(caracter==HERO3)
		{
			Equip(9);
			Equip(15);
		}
		else if(caracter==HERO5)
		{
			Equip(6);
			Equip(13);
		}
		else if(caracter==HERO6)
		{
			Equip(3);
			Equip(16);
		}
		else if(caracter==BANDIT)
		{
			Equip(1);
		}
	}
}

void CUnit::Equip(int equip_id)
{
	char part=equipment[equip_id].part;
	if(part==EQ_ARMS)
	{
		if(arms!=0)TakeOff(EQ_ARMS);
		arms=equip_id;
		str+=equipment[arms].pow;
	}
	else if(part==EQ_AMMOR)
	{
		ammor=equip_id;
		dp=equipment[ammor].pow;
	}
}

void CUnit::TakeOff(char part)
{
	if(part==EQ_ARMS)
	{
		if(arms==0)return;
		str-=equipment[arms].pow;
		arms=0;
	}
	else if(part==EQ_AMMOR)
	{
		ammor=-1;
		dp=0;
	}
}

CUnit& CUnit::operator=(const CUnit& src_unit)
{
	arms=src_unit.arms;
	str=src_unit.str;
	ammor=src_unit.ammor;
	dp=src_unit.dp;
	mizi_level=src_unit.mizi_level;

	return *this;
}

//////////////////////////////////////////////////////
//ť Ŭ����
#define QUE_STEP	100

class QueData
{
public:
	unsigned char command;
	int id;
	int x;
	int y;
	WORD time;

	void Insert(unsigned char vcommand, int vid, int vx, int vy, WORD vtime);
	QueData& operator=(const QueData& quedata);
};

void QueData::Insert(unsigned char vcommand, int vid, int vx, int vy, WORD vtime)
{
	command=vcommand;
	id=vid;
	x=vx;
	y=vy;
	time=vtime;
}

QueData& QueData::operator=(const QueData& quedata)
{
	command=quedata.command;
	id=quedata.id;
	x=quedata.x;
	y=quedata.y;
	time=quedata.time;

	return *this;
}

class Que
{
	QueData *que;

	int que_max;
	int limit;
	bool empty;

public:
	WORD que_time;

	//����, �Ҹ�, ����, ����
	void Init();
	void Destroy();
	void Insert(unsigned char command, int id=0, int x=0, int y=0, WORD time=0xffff);
	bool Delete(int id);

	//�� ���
	int GetMax();
	QueData* GetQue(int id);
};

void Que::Init()
{
	que=new QueData[QUE_STEP];

	limit=QUE_STEP;
	que_max=0;
	que_time=1;
}

void Que::Destroy()
{
	if(que!=NULL)
	{
		delete [] que;
		que=NULL;
	}
}

void Que::Insert(unsigned char command, int id, int x, int y, WORD time)
{
	if(time==0xffff)time=que_time;
	//�� ť�� ����
	for(int i=0; i<que_max; i++)
	{
		if(que[i].time<que_time)
		{
			que[i].Insert(command, id, x, y, time);
			return;
		}
	}

	//ť �뷮 Ȯ��
	if(que_max>=limit)
	{
		//�����͸� ����ϰ� ������ ť�迭 ����
		QueData* tmp_que=new QueData[que_max];
		for(int i=0; i<que_max; i++)tmp_que[i]=que[i];
		delete [] que;
		//Ȯ���ϰ� �����͸� �ű�
		que=new QueData[limit+QUE_STEP];
		for(int i=0; i<que_max; i++)que[i]=tmp_que[i];
		delete [] tmp_que;
		tmp_que=NULL;

		limit+=QUE_STEP;		
	}

	que[que_max].Insert(command, id, x, y, time);
	que_max++;
}

bool Que::Delete(int id)
{
	que[id].time=0;
	while(id==que_max-1)
	{
		que_max--;
		if(id>0)
		{
			if(que[id-1].time<que_time)id--;
		}
		else
		{
			que_time=1;
			return true;
		}
	}
	return false;
}

int Que::GetMax()
{
	return que_max;
}

QueData* Que::GetQue(int id)
{
	return &que[id];
}

//////////////////////////////////////////////////////
//���� Ŭ����
class CNumber
{
private:
	CHungLib *lib;
	int buffer[10];
	int width;
	int img_id;
	int fontsize;
	int pre_number;
	bool minus;

public:
	CNumber(CHungLib *hung, int img_no, int size);
	CNumber();
	void Set(CHungLib *hung, int img_no, int size);
	void PutNumber(int x, int y, int value);
};

CNumber::CNumber(CHungLib *hung, int img_no, int size)
{
	Set(hung, img_no, size);
	CNumber();
}

CNumber::CNumber()
{
	//�ʱⰪ
	pre_number=0;
	width=1;
	buffer[0]=0;
	minus=false;
}

void CNumber::Set(CHungLib *hung, int img_no, int size)
{
	lib=hung;
	img_id=img_no;
	fontsize=size;
}

void CNumber::PutNumber(int x, int y, int value)
{
	if(value!=pre_number)
	{
		int cmp=10;		//�ڸ����� �˱� ���� ���ϴ� ��
		width=1;
		int tmp=abs(value);

		//����, ��� ����
		if(value<0)minus=true;
			else minus=false;

		//�ʿ��� ���� ���
		while(tmp>=cmp)
		{
			cmp*=10;
			width++;
			if(width>=10)break;
		}

		//�� �ڸ��� ���� ����� ���� ���ϱ�
		for(int i=0; i<width; i++)
		{
			buffer[(width-1)-i]=tmp%10;
			tmp/=10;
		}

		pre_number=value;
	}

	//���
	if(minus)lib->Image(x, y, img_id, 10);
	for(int i=width-1; i>=0; i--)
	{
		if(minus)lib->Image(x+(i+1)*fontsize, y, img_id, buffer[i]);
			else lib->Image(x+i*fontsize, y, img_id, buffer[i]);
	}
}

//////////////////////////////////////////////////////
//�̹��� ��ȣ Ŭ����(���� Ŭ������ ���� ���� ���ؼ�)
class CImgNo
{
public:
	int panel;
	int state_icon;
	int number;
	int unit;
	int select;
	int animation;
	int cursor;
};

CImgNo imgno;

//////////////////////////////////////////////////////
//��Ƽ Ŭ����
#define PARTY_MAX	20		//��Ƽ�� �Ѱ�

class CParty
{
public:
	CUnit unit[PARTY_MAX];
	int money;
	int item[EQUIP_MAX];
};

//////////////////////////////////////////////////////
//���� Ŭ����
#define UNIT_MAX	50		//���� �Ѱ�

//üũ�� ����
#define MOVE_TEST_MAP	0
#define MOVE_BACK_MAP	1
#define ATTACK_TEST_MAP	2
#define DAMAGE_TEST_MAP	3
#define ANIMATION_MAP	4
#define DELAY_MAP		5
#define DISTANCE_MAP	6

//��ɾ�
#define CM_FOCUS		0	//��Ŀ��
#define CM_MOVE			1	//�̵����� ����
#define CM_MOVEEND		2	//�̵� ��
#define CM_DAMAGE		3	//������
#define CM_HEEL			4	//ġ��
#define CM_DIE			5	//���
#define CM_WARP			6	//����
#define CM_DIALOG		7	//��ȭ
#define CM_NEXT_DIALOG	8	//���� ��ȭ
#define CM_QUESTION		9	//����
#define CM_ANSWER		10	//���
#define CM_SHAKE	11	//����

//�Լ�
#define FUNC_NONE		0	//�ƹ��� ����(�ݵ�� 0)
#define FUNC_MAINMENU	1	//���� �޴��� ��
#define FUNC_WORLDMAP	2	//���������� ��
#define FUNC_ENEMY		3	//���� ����
#define FUNC_BOSS		4	//������ ������
#define FUNC_QUIZ1		5	//��ũ������ ����
#define FUNC_ANSWER1	6
#define FUNC_QUIZ2		7
#define FUNC_ANSWER2	8
#define FUNC_QUIZ3		9
#define FUNC_ENDING		10
#define FUNC_FAILED		13
#define FUNC_GAMEOVER	14

//�̺�Ʈ
#define EV_START		0
#define EV_MOVE			1
#define EV_ACTION_END	2
#define EV_END			3

//�̺�Ʈ ����ġ
#define SWTICH_MAX			20

#define SW_PROLOGUE			0
#define SW_HYUGARU			1
#define SW_GAMIUL			2
#define SW_SIKUBEL			3
#define SW_MIBUGA			4
#define SW_RUBELBU			5
#define SW_HYURUSI			6
#define SW_GAMIUL_DEFEAT	7
#define SW_SIKUBEL_DEFEAT	8
#define SW_MIBUGA_DEFEAT	9
#define SW_RUBELBU_DEFEAT	10

//----------------------------------------------//
//������ �ʿ� ���� ���� ����
CHungLib* lib;	//���̺귯�� ������

int panel_x;	//���� ��ġ
int panel_y;
CNumber n_hp, n_ammor, n_shield, n_arms, n_mizi, n_move, n_wait;	//���� Ŭ����

//������ ������ üũ�ϱ� ���� ���̴� ����
bool first_check;
int prev_x, prev_y;
char att_dir;

int AI_MouseX, AI_MouseY;	//AI�� ����ϴ� ���콺
int AI_thinking;

bool main_menu;	//���� �޴����� �߰� �޴����� �Ǻ�
bool save_mode;	//���̺����� �ε����� �Ǻ�

//���̺� ������ ���� ǥ��
class CFileData
{
public:
	bool read_able;
	int map_id;
};
CFileData file_data[3];

//��ȭ&���� ���� ����
bool dialog, question, answer;
int answer_func, answer_func2;
int dialog_no;
int dialog_end;

//�����
int have_money[]={0, 200, 200, 200, 200, 0, 0, 0, 0, 60, 20, 40, 30, 45, 25, 0, 0};	//�� ��ġ��� ��� ��
char* mizi_name[]={"�ٶ�", "��", "��", "��", "����", "��", "��"};					//���� �̸�

//����
int shop[4];
int select_price;
bool able_unit[20];
int selected, selected2, selected3;

//ġƮ
bool no_enemy=false;
bool cheat_able=true;

int clear_count=0;	//Ŭ���� Ƚ��

//��ȭ �����
char* dialog_text[]={
/*000:*/	"-1",	"�ް��簡 ��� ���迡�� ���� �ҳ� �����Ѵٰ� ��������.\n��ũ ��� �� �� �ϳ�����.\n�ƹ��͵� �ƴ� �� ���� ��ũ����� ����Ե� ���踦 ����Ѵ�.\n�׷��� �������� �ް���� ��ũ��� ���ֱ� ���� ����濡 ���� ���̴�.",
/*001:*/	"-1",	"�ް��簡 ��� ���迡�� 5���� ������ �ִ�.\n�߾��� �����ο� �ٶ��� ���� �ް���,\n���� ���տ��� ���� ������ ���� ������ �������� �ް����� �����̴�.\n�� �ٷ� ���� ���ռ��� �ִ�.",
/*002:*/	"-1",	"������ �縷�� ���� ����, ���̾� ����.\n������ ������ ������ ����, �̹��� ����.\n������ ���� ���� ����, �级�� ����\n������ ���� ����, ��ũ�� ����.",
/*003:*/	"-1",	"������ 4���� ������ ������ 4���� �屺�� ��Ű�� �ִ�.",
/*004:*/	"-1",	"����, ���踦 ���ϵ���... ������...\n�����... �׵���...\n����� ����� ���~��",
/*005:*/	"0",	"<�ް���>\n�� ���� ������ �� �־�?\n�ƹ�ư ���踦 ���غ���? ���� �� �ϵ� ������...\n�װ� ������ �ƹ����� �ĵ��� �ǰ����� �ϴ� �������� ���� �ؾ���.",
/*006:*/	"0",	"<�ް���>\n���� ���� �����ϱ�!\n����������!",
/*007:*/	"-1",	"<�ֹ�>\n�ް����, ���� �� �̻� �� �����ϴ�.\n������ ������ ���� �ֽʽÿ�.",
/*008:*/	"-1",	"<�ֹ�>\n���� �� �� �ٶ�ϱ� �׳� ���� ���踸 �����ּ���.",
/*009:*/	"-1",	"<�ֹ�>\n��ũ ����� ó���� �� �� �ƴϾ��µ�,\n���� ���ϰ� 4���� �屺�� ��� ������������.\n�׷��� ���� ��ũ��� ���� �ο�� �� �� �þ��.",
/*010:*/	"-1",	"<�ֹ�>\n������ ���信�� ����� �Ȱ��� ��� ������ ���� �� �� ���� �̴ϴ�.\n�ް������ ��ǳ������� �Ȱ��� ���� �� �ְ���.",
/*011:*/	"-1",	"<�ֹ�>\n���� �߿� ������ �ְ��� �����ؿ�. ������ �帣��, �������� ������Ű��,\n���� �ؿ��� ����ġ�� ���� ������ ��û����. �谩���� �İ��� ���ص� ����.\n���װ��ʿ� �����Ǵ� �� ����������...",
/*012:*/	"-1",	"<�ֹ�>\n��ũ������ 4���� �屺�� ������ �ƴ϶� �ΰ��� �� ���ƿ�.",
/*013:*/	"17",	"<���̾�>\n��������! �� �Դ�.\n����� �Ĳ��Ĳ� �縷 ����.\n�� ���̾� ���� �� ��ġ�� �� ������ �Ǿ�� ����� ����.",
/*014:*/	"0",	"<�ް���>\n�� �������� �Ǿ�� ������ �� �ʹ�.",
/*015:*/	"17",	"<���̾�>\n����, �װ� �׷���.\n�׷� ��� �Ѵ�?",
/*016:*/	"0",	"<�ް���>\n���� �� �̱�� ��.",
/*017:*/	"17",	"<���̾�>\n��ȣ, �װ� ���� �����̱�.\n�� �� �� �� ������ �غ���.\n����! ���ڵ��!\n�� ���������ڲٳ�.",
/*018:*/	"30",	"<����>\n����!!!",
/*019:*/	"18",	"<��ũ��>� ���ʽÿ�.\n�� ������ �ع��Ű�� ������ �� �����߷��� �մϴ�.",
/*020:*/	"0",	"<�ް���>\n�� ���ڳ�? ���ڳ�?",
/*021:*/	"18",	"<��ũ��>\n�����Դϴ�.",
/*022:*/	"0",	"<�ް���>\n......",
/*023:*/	"0",	"<�ް���>\n�׿��ָ�.",
/*024:*/	"0",	"<�ް���>\n������ ����.",
/*025:*/	"20",	"<�̹���>\n�� ������ ���� ���� ��������.\n���� ���� ����, ���򽺷��� �����ָ�.",
/*026:*/	"0",	"<�ް���>\n��!",
/*027:*/	"0",	"<�ް���>\n�ѹ��� ���ּ���.",
/*028:*/	"20",	"<�̹���>\n�ȴ�.",
/*029:*/	"0",	"<�ް���>\n��â�� ���̱�.",
/*030:*/	"19",	"<�级��>\n��â�� �� �Ӹ��� �ƴ���...\n�� ���� ������ �� �˰� �ɰŴ�.\n���մԿ��� �����ϴ� �ڿ��Դ� ������...",
/*031:*/	"0",	"<�ް���>\n������ ��¾�ٰ�?",
/*032:*/	"19",	"<�级��>\n��¿��?",
/*033:*/	"0",	"<�ް���>\n������...\n...�Ѿ ����� �ϰ� ����.",
/*034:*/	"19",	"<�级��>\n�� �� �¾ƾ� �ڴ�.",
/*035:*/	"21",	"<�޷��>\n������! ���� ��ħ���� ������ ���Ƹ��� �༮!\n���� ���Ⱑ ����� ���� �鿩������!",
/*036:*/	"0",	"<�ް���>\n�ƴ�! ����� ���� ���, �޷��!\n��°�� ��ũ ������ ���ϰ� �Ǿ���?",
/*037:*/	"21",	"<�޷��>\n��ũ������̾� ���� �� ���踦 �����ϱ⿡ �ùٸ� ���̽ñ� �����̴�.\n��ũ������� ���ϰ� �ǰŶ�!",
/*038:*/	"0",	"<�ް���>\n�ȴ�! �� �ΰ����� ���� �ο� ���̴�.",
/*039:*/	"0",	"<�ް���>\n�ΰ��� �־�� ���� �������!\n�̾� ����� �ʹ� ����� ����.\n�ڽ��� �踸 ä�� ���ĸ� ������ �����ϰ�!",
/*040:*/	"0",	"<�ް���>\n��굵 �� ������ �ο�⸸ ������ �ο��...\n��! �̰� �̻��ȸ��.",
/*041:*/	"21",	"<�޷��>\n�׷���?",
/*042:*/	"0",	"<�ް���>\n�׷��� �� �Ⱦ�!\n�� �����ο� �ٶ��� ���ϱ�!",
/*043:*/	"21",	"<�޷��>\n������, �׷� ó���ϴ� �� �ۿ�...",
/*044:*/	"0",	"<�ް���>\n��, ���� ����, ���� ����.",
/*045:*/	"0",	"<�ް���>\n������ �ѷ����� ���� ������.\n����� ������ �� �� ����.",
/*046:*/	"0",	"<�ް���>\n���� �ѷ����� ���� ������.\n������ ��ĥ �� ����.",
/*047:*/	"0",	"<�ް���>\n������ �ѷ����� ���� �б�. ��������� ���� �� ����.\n������ �������� �ƴ� ���� ������ ������ ����.\n�ٸ� ���迡�� �����̶�� �ϴ���, �ణ Ʋ��.",
/*048:*/	"0",	"<�ް���>\n���������� �ѷ����� ���� �˼���.\n�뺴�� ����� �� ����. �ְ� 15�����.\n�뺴�� �������� �������� �������ٱ�.",
/*049:*/	"0",	"<�ް���>\n�������� ��� �� ������ ���ڸ� �̵����¿���\n���콺 ������ ��ư�� Ŭ���ϸ� ĳ���� ������ �� �� �ִµ�,\nĳ���Ͱ� ���� ���� Ŭ���ϸ� ������ ������ �� �� ����.",
/*050:*/	"0",	"<�ް���>\n����������! �� ģ���� ����!",
/*051:*/	"0",	"<�ް���>\n���Ҿ�. �̰ɷ� '����, �츮 ���� �ְ�~��'�� ���� �غ�� �� �ƴ�.\n ��ũ���� ���ְ� �⿬�ؾ���.",
/*052:*/	"-1",	"����ġ�ðڽ��ϱ�?",
/*053:*/	"-1",	"������ ������ �̵��ϰڽ��ϱ�?",
/*054:*/	"-1",	"�ν���...",
/*055:*/	"0",	"<�ް���>\n��? ���ΰ�?",
/*056:*/	"-1",	"�������� �¸��Ͽ����ϴ�.\n���ʽ�: 50ī��",
/*057:*/	"2",	"<��ũ��>\n��... �Ӹ���... ���?",
/*058:*/	"0",	"<�ް���>\n������ �ϰ�!!!",
/*059:*/	"2",	"<��ũ��>\n��ø� ��ٷ� �ֽʽÿ�.\nȤ�� ����� �� ������ �ٶ��� ��� �ް���?",
/*060:*/	"0",	"<�ް���>\n������, ���� �� ��������.",
/*061:*/	"2",	"<��ũ��>\n�� ��� ��ũ��� �����߸����� �߽��ϴ�.\n�� ���� ������� ���� �������ѵ� �ͱ���.",
/*062:*/	"0",	"<�ް���>\n������, �׷� ������ ���� ���� ��ũ��� ����ڰ�...",
/*063:*/	"2",	"<��ũ��>\n��, �˰ڽ��ϴ�.",
/*064:*/	"-1",	"��ũ���� ���ᰡ �Ǿ���!",
/*065:*/	"1",	"<���̾�>\nũ��! �׾��! ��ũ����! ��?",
/*066:*/	"0",	"<�ް���>\n�´��� ���� ����? �� �̷�?",
/*067:*/	"1",	"<���̾�>\n�� ��ũ����� �ο�� �־��µ�...",
/*068:*/	"0",	"<�ް���>\n���� ��ũ������ �屺�� �ο�� �־��µ�?",
/*069:*/	"1",	"<���̾�>\nũ��, �� ���̾���� ��ũ����� �������Ͻ� �� �ʹ�.\n�ΰ���, ��ũ����.",
/*070:*/	"-1",	"���̾��� ���ᰡ �Ǿ���!",
/*071:*/	"3",	"<�级��>\n......",
/*072:*/	"0",	"<�ް���>\n���ڱ� �� �׷�?",
/*073:*/	"3",	"<�级��>\n�̾��ϴ�. ���� �ƹ����� ��ũ����� �������� �� �ͱ�.",
/*074:*/	"0",	"<�ް���>\n�̾��ϸ� �پ�? �̾��ϸ� �ٸ� ������ �� �ְھ�?",
/*075:*/	"3",	"<�级��>\n�� ��, ������� �İڴ�.",
/*076:*/	"0",	"<�ް���>\n����! ����̾�!",
/*077:*/	"3",	"<�级��>\n�Ƴ�, ���տ��� �����ϴٴ�.\n�� ����� ����.",
/*078:*/	"0",	"<�ް���>\n�ƴ�, �־�.",
/*079:*/	"3",	"<�级��>\n����ü ����...",
/*080:*/	"0",	"<�ް���>\n��, ��. ������... ����... ��...\n�Բ� ��ũ��� ����ġ�ڰ�.",
/*081:*/	"3",	"<�级��>\n��ũ��� ������ �˸� ������� �ǰ�?\n������ ��� ����.",
/*082:*/	"0",	"<�ް���>\n����~ �ǰ��� Ÿ���̱�.",
/*083:*/	"-1",	"�级���� ���ᰡ �Ǿ���!",
/*084:*/	"4",	"<�̹���>\n��ȣ!",
/*085:*/	"0",	"<�ް���>\n�� �̷�? �̰� ���Ƴ�?",
/*086:*/	"4",	"<�̹���>\n�ƴ�, �� ��ũ������ ������ �޾Ҵµ�, �� �°� ���ϱ� �װ� Ǯ������.\n�׷��� ���ٰ� �Ϸ��� �Ŵ�.",
/*087:*/	"0",	"<�ް���>\n��, �׷��ž�.",
/*088:*/	"4",	"<�̹���>\n����, ������! ��ũ���� ���ַ� ������.",
/*089:*/	"0",	"<�ް���>\n(��... �̳�, �� ������ �ƴϴ�.)",
/*090:*/	"-1",	"�̹����� ���ᰡ �Ǿ���!",
/*091:*/	"5",	"<�޷��>\n��, ��... �����?",
/*092:*/	"0",	"<�ް���>\n���� ������ �� �ǰ�?",
/*093:*/	"5",	"<�޷��>\n����... �׷���. ����. ���� ��ũ����κ��� �ع���� �༭.",
/*094:*/	"0",	"<�ް���>\n����ü ��� �� �ž�?",
/*095:*/	"5",	"<�޷��>\n��ũ����... �༮�� ���� ������ �ʾ�.\n����ϰ� �ο� ������.\nƯ�� ���� ������ڸ� �����ض�.",
/*096:*/	"0",	"<�ް���>\n���� ����?",
/*097:*/	"5",	"<�޷��>\n�༮�� ���� ������ �翬�� �ؾ� �� ����� ���� ����.\n�װ��� �༮�� ������ڸ� ���ϴ� ����̴�.",
/*098:*/	"0",	"<�ް���>\n���� �翬�� ����� �ϰ� �ȴٸ�...?",
/*099:*/	"5",	"<�޷��>\n��ó�� �༮�� ���ϰ� �Ǿ� ������.\n��! ����... �׷� ����������...\n�� ���... �� ���̴�.",
/*100:*/	"0",	"<�ް���>\n��, �׷���.",
/*101:*/	"5",	"<�޷��>\n����, ��~",
/*102:*/	"0",	"<�ް���>\n�ƹ�ư ���� ��ũ������ �ձ��� �����ߴ�!\n��ġ����!",
/*103:*/	"15",	"<��ũ����>\n���ɵ� ������� �Ա���. �Ǹ��ϴ� ��翩.",
/*104:*/	"0",	"<�ް���>\n�� �ϳ�~.",
/*105:*/	"15",	"<��ũ����>\n������... �� ����� ������ ��±�.",
/*106:*/	"15",	"<��ũ����>\n���� ���ϰ� �ǰڴ���?",
/*107:*/	"0",	"<�ް���>\n����... �Ӹ���...",
/*108:*/	"15",	"<��ũ����>\nũũũ, �ɷȱ�. ���� �� ���� ���ϴ�!",
/*109:*/	"-1",	"�׷��� �ް��縦 ���Ϸ� ���� ��ũ����� ���踦 �����߽��ϴ�.\n-���ӿ���-",
/*110:*/	"15",	"<��ũ����>\n�̺�, �׵� ��簡 �����!\n",
/*111:*/	"15",	"<��ũ����>\n����, �׷� ���� ���ϰ� �Ǳ� �ȴٸ� ���� ������ �Ǿ� �ٿ�.",
/*112:*/	"15",	"<��ũ����>\nũ��, �׷���... �̷� ��û�� �ź��ϴٴ�...",
/*113:*/	"15",	"<��ũ����>\n���� ������ڿ� ���� �ƴ°� ����.",
/*114:*/	"15",	"<��ũ����>\n����! �ο���!",
/*115:*/	"15",	"<��ũ����>\n�����... �ʹ� ����...\n���� �¾ ������ڸ� ��� ���� �� �� �����Ϸ� �߰Ǹ�...\n���� �� �� �ǳ�...",
/*116:*/	"0",	"<�ް���>\n�׷�, �� ������.",
/*117:*/	"15",	"<��ũ����>\nũũ, ������ �������� �༮�̷α�.\n���������� ��翩...\n���� �����ϳ�...?",
/*118:*/	"0",	"<�ް���>\n�Ⱦ����� �ʾ�.",
/*119:*/	"15",	"<��ũ����>\n�׷�...\n�����̴�.",
/*120:*/	"0",	"<�ް���>\n���� �׾�.",
/*121:*/	"-1",	"�̷��� �ް���� ��ũ��� �����ƽ��ϴ�.\n�׸��� �׳� ���� <����, �츮 ���� �ְ�~��>�� �����Ͽ� ������ڸ� ���,\n������ ��� �ڱ������� ����� ����ߴٰ� �մϴ�.",
/*122:*/	"-1",	"�ƹ�ư �̰����� �����ο� �ٶ��� ��� �ް����� �̾߱�� ���� �����ϴ�.",
/*123:*/	"-1",	"���� ���� �����ο� �ٶ��� ������ �̾߱� <�ް��� ����2>�� ã�ƺ˰ڽ��ϴ�.",
/*124:*/	"-1",	"�������� ���� �޾ƾ� �ڽ��ϴ�.\n��, ���� ������� �������.",
/*125:*/	"-1",	"<�ֹ�>\n�̾˵��� ������ ���� �ΰ����� ������ ���մϴ�.\n������ ���ϰ� �Ǳ⵵ �ϰ�, �� ����̸� �ΰ��� ���� �Ǿ� �ֱ⵵ ����.",
/*126:*/	"-1",	"<�ֹ�>\n���� �����̶�� ���̽��ϱ�?\n������ 99�� ���� �� �Ǵ� �����̶�� �ϴ�����.",
/*127:*/	"-1",	"<�ֹ�>\n������ ���� ������ ����ġ�̴� ���� ��� �޷�ô��̶�� �����.\n������ ��� ���̴��� �𸣰ڱ���.",
/*128:*/	"-1",	"<�ǹ��� �ֹ�>\n������ ���� �Ƴ�...",
/*129:*/	"-1",	"<�ֹ�>\n���̾���� �������� �����ּż� �����մϴ�.",
/*130:*/	"-1",	"<�ֹ�>\n���� �߾��� ���ƽý��� �ſ� �߿��� ������.\n��ũ���Բ��� ����� �ּ���.\n��, �׸��� �����嵵 ���� ���� ������.",
/*131:*/	"-1",	"<�ֹ�>\n�̹��� ���� �� �������� ����� �ּż� �����մϴ�.",
/*132:*/	"0",	"<�ް���>\n�ƴ�, �� ���°� �� �����̶���?",
/*133:*/	"-1",	"<�ֹ�>\n��������...",
/*134:*/	"-1",	"<�ֹ�>\n���� �������� ������ ���� �ļ� ���� ������ �ǰ� ������.",
/*135:*/	"0",	"<�ް���>\n�ٸ����� �Ȱ�����?",
/*136:*/	"-1",	"<�ֹ�>\n���ΰ� �ٸ� ������. ������!",
/*137:*/	"-1",	"<�ֹ�>\n���� ô���ؼ� �� ��Ⱑ �������.",
/*138:*/	"0",	"<�ް���>\n�׷� �� ���� ��� ��µ�?",
/*139:*/	"-1",	"<�ֹ�>\n���� �ڽľ�! ���� ��� �; �糪?\n�׷��� �� �� ��¼���!\n�츮�� ������ ��� �;�. �׷��� ���� ����!",
/*140:*/	"-1",	"<�ֹ�>\n�ΰ��� ��վ�. ũũũ...",
/*141:*/	"-1",	"<�ֹ�>\n��ũ������ �� ������ ���Դϴ�.\n�ٸ� �������� ������ �ּ���.",
/*142:*/	"-1",	"<�ֹ�>\n��ũ���� �ʹ� �� ����̾�.\n������",
/*143:*/	"-1",	"<�ֹ�>\n���� �����̶� �̵��� �� �����մϴ�.",
/*144:*/	"-1",	"<�ֹ�>\n���� ����. ������?",
/*145:*/	"-1",	"<�ֹ�>\n�츮�� �踦 ����� ���� �ٵ�...",
/*146:*/	"-1",	"<�ֹ�>\n���ְ� Ǯ���� �����Դϴ�.\n������ ������ �뼭 ���� �� ã�Ҵٰ��...",
/*147:*/	"-1",	"<�ֹ�>\n��... ������ ������ ������ �� �� �����ϴ�.\n�������� ������ �� �� �׳� ������ �־ ���� ����µ�...\n���ʺ��� �츮 ������ �߸� ����� ����.",
/*148:*/	"-1",	"<�ֹ�>\n��ũ������ �������� �������� �����ϰ� �ڶ����ϴ�.",
};

//----------------------------------------------//

CParty party;	//��Ƽ ����

class CGame
{
private:
	//�ʰ� ����ʿ����� ��ġ
	int map_id;
	int world_x;
	int world_y;

	//�� ������
	char map[48][48][3];
	char unit_map[48][48];
	bool fog_map[48][48];
	unsigned char check_map[7][48][48];
	unsigned char move_map[48][48];

	int x_size, y_size;
	bool battle;
	bool pre_battle;
	bool boss;

	//���� ����
	CUnit unit[UNIT_MAX];
	char unit_id;
	int unit_x;
	int unit_y;
	int state_id;
	int att_code;

	//�̺�Ʈ ó���� ����ġ
	bool event_switch[SWTICH_MAX];
	int test;

	//ť Ŭ����
	Que que;

	/////////////////////////////
	//Private �޼ҵ�

	//���� ���� ����
	void SetUnit(int id, int vx, int vy, char vcaracter, char vteam);
	int GetUnitID();
	void AddUnit(int vx, int vy, char vcaracter, char vteam);

	//�˻�� �� ����
	void InitCheckMap(int id, unsigned char value=99);
	void SetCheckMap(int id, int x, int y, unsigned char value);
	void Search(int id, int x, int y, int dist, int type, char team);
	void SearchRoad(int id, int x, int y, int type, char team);
	unsigned char SearchNear(int id, int x, int y);
	void SetDiamondArea(int id, int x, int y, int dist_min, int dist_max, int value);
	void SetSquareArea(int id, int x, int y, int dist_min, int dist_max, int value);
	void TransMission(int id, int x, int y, int value, bool water=true, bool human=true);
	void MakeMoveMap();

	//�� ����
	void Focus(int x, int y, WORD time=0xffff);

	//���� ����
	void SetAttackArea();
	void SetDamageArea(int x, int y);
	void Action(int x, int y);
	void EndTurn();
	void UnitMove(int id, int x, int y);
	void Damage(int x, int y, int value, int type);
	void EraseFog();
	bool Eliminate();
	void GetHero(int id);

	//����, �ҷ�����
	void SaveNow();
	void Save(int file_no);
	bool Load(int file_no);
	void LoadInfo();

	//��Ÿ ��ɾ��
	void Function(int func_no);
	void SelectBGM();

public:
	//�� ���� ����
	int fx_i, fy_i;
	char click_mode;	//-1: ���� 0: ���� 1: �̵� 2: �̵��� 3: ���ݼ��� 4: ���ݹ������� 5: �������� 99: AI
	bool sw_focus;
	bool sw_move;
	bool sw_ai;
	char escape;

	/////////////////////////////
	//Public �޼ҵ�
	CGame(CHungLib* hung);
	void Init();
	void Start();
	void End();
	void SetStartPoint(int id, int x, int y);
	bool LoadMap();
	void Process();
	void Event(int type, int id=0, int x=0, int y=0);
	void Scroll(int MouseX, int MouseY);
	void LeftClick(int MouseX, int MouseY);
	void RightClick(int MouseX, int MouseY);
	void DrawMap(int MouseX, int MouseY, char d_tile, char r_tile);
	void Cheat(int code);
	void PartySort();
};

CGame::CGame(CHungLib* hung)
{
	lib=hung;

	unit_id = 0;

	//���� �ʱ�ȭ
	n_hp.Set(lib, imgno.number, 5);
	n_ammor.Set(lib, imgno.number, 5);
	n_shield.Set(lib, imgno.number, 5);
	n_arms.Set(lib, imgno.number, 5);
	n_mizi.Set(lib, imgno.number, 5);
	n_move.Set(lib, imgno.number, 5);
	n_wait.Set(lib, imgno.number, 5);
}

//�������� �� �ʱ�ȭ
void CGame::Init()
{
	pre_battle=false;

	//��Ƽ �ʱ�ȭ
	party.money=1000;
	party.unit[0].SetUnit(0, 0, HERO1, 0);
	for(int i=1; i<PARTY_MAX; i++)party.unit[i].life=false;
	for(int i=0; i<EQUIP_MAX; i++)party.item[i]=0;

	//����ġ �ʱ�ȭ
	for(int i=0; i<SWTICH_MAX; i++)event_switch[i]=false;
}

void CGame::Start()
{
	//���ó�� �ʱ�ȭ
	que.Init();
	sw_focus=false;
	sw_move=false;

	//��Ÿ �ʱ�ȭ
	click_mode=0;
	state_id=-1;
	fx_i=0, fy_i=0;
	escape=0;
	dialog=question=false;
}

void CGame::End()
{
	que.Destroy();
}

//���� ����
void CGame::SetUnit(int id, int vx, int vy, char vcaracter, char vteam)
{
	unit[id].SetUnit(vx, vy, vcaracter, vteam);
	unit_map[vx][vy]=id;
}

//��� ������ ���� ��ȣ �˷��ֱ�
int CGame::GetUnitID()
{
	for(int i=2; i<UNIT_MAX; i++)
		if(!unit[i].life)return i;
	return -1;
}

//���� �߰�
void CGame::AddUnit(int vx, int vy, char vcaracter, char vteam)
{
	SetUnit(GetUnitID(), vx, vy, vcaracter, vteam);
}

void CGame::SelectBGM()
{
	if(map_id==0)
	{
		_MidiPlay("field.mid");
	}
	else if(map_id==13)
	{
		_MidiStop();
	}
	else if(map_id<13)
	{
		if(battle)
		{
			_MidiPlay("battle.mid");
		}
		else
		{
			_MidiPlay("village.mid");
		}
	}
}

bool CGame::LoadMap()
{
	FILE* fp;

	//���� ������ ���������� ����
	if(map_id==0 || map_id==1 || map_id==13 || map_id==14)battle=false;
		else
		{
			if(map_id==7 && event_switch[SW_GAMIUL_DEFEAT])battle=false;
				else if(map_id==8 && event_switch[SW_MIBUGA_DEFEAT])battle=false;
				else if(map_id==9 && event_switch[SW_SIKUBEL_DEFEAT])battle=false;
				else if(map_id==10 && event_switch[SW_RUBELBU_DEFEAT])battle=false;
				else battle=true;
		}
	boss=false;

	//�� ������ �о� ���̱�
	char file_name[20];
	sprintf(file_name,"map\\map%03d.map",map_id);

	if(fp=fopen(file_name,"rb"))
	{
		//������ ������ ���� Ȯ��
		char data_type[11], ver;
		fread(&data_type,sizeof(char),10,fp);
		data_type[10]=NULL;

		if(strcmp(data_type,"HYUGARUMAP")==0)
		{
			fread(&ver,sizeof(char),1,fp);
			fread(&x_size,sizeof(int),1,fp);
			fread(&y_size,sizeof(int),1,fp);
			
			for(int k=0; k<3; k++)
				for(int j=0; j<y_size; j++)
					for(int i=0; i<x_size; i++)
						fread(&map[i][j][k],sizeof(char),1,fp);
		}
		else return false;

		//�ʱ�ȭ
		for(int i=0; i<UNIT_MAX; i++)unit[i].life=unit[i].soul=false;
		for(int j=0; j<48; j++)
			for(int i=0; i<48; i++)
			{
				unit_map[i][j]=-1;
				if(battle && map_id>=7)fog_map[i][j]=true;
					else fog_map[i][j]=false;
			}

		//���͸�, ���� �ʱ���ġ ����
		int party_no=1;		//��ġ�� ��Ƽ�� ��ȣ
		for(int j=0; j<y_size; j++)
			for(int i=0; i<x_size; i++)
			{
				if(Between(map[i][j][1],9,14))map[i][j][1]-=9;
					else map[i][j][1]=-1;
				if(Between(map[i][j][2],16,16))map[i][j][2]-=16;
					else
					{						
						//�Ʊ� ��ġ
						if(map[i][j][2]==17)
						{
							if(battle && party.unit[party_no].life)
							{
								int set_unit_id=GetUnitID();
								SetUnit(set_unit_id, i, j, party.unit[party_no].caracter, 0);
								unit[set_unit_id]=party.unit[party_no];
								unit[set_unit_id].party_id=party_no;
								party_no++;
							}
						}
						//���� ��ġ
						else if(map[i][j][2]==18)
						{
							if(battle)
							{
								if(map_id<7)AddUnit(i, j, rand()%6+BANDIT, 1);
									else if(map_id==7)AddUnit(i, j, MAEQO, 1);
									else if(map_id==8)AddUnit(i, j, BIGROCK, 1);
									else if(map_id==9)AddUnit(i, j, BANG, 1);
									else if(map_id==10)AddUnit(i, j, KUOT, 1);
									else if(map_id==12)AddUnit(i, j, NEYUM, 1);
									else AddUnit(i, j, rand()%5+BANG, 1);
							}
						}
						//���ΰ� ��ġ
						else if(map[i][j][2]==19)
						{
							SetUnit(0, i, j, party.unit[0].caracter, 0);
							unit[0]=party.unit[0];
							unit[0].party_id=0;
						}
						//���� ��ġ
						else if(map[i][j][2]==20)
						{
							if(battle)
							{
								if(map_id==7)SetUnit(1, i, j, HERO2, 1);
								else if(map_id==8)SetUnit(1, i, j, HERO5, 1);
								else if(map_id==9)SetUnit(1, i, j, HERO3, 1);
								else if(map_id==10)SetUnit(1, i, j, HERO4, 1);
								else if(map_id==11)SetUnit(1, i, j, HERO6, 1);
								else if(map_id==12)SetUnit(1, i, j, DARKQUIZ, 1);
								boss=true;
							}
						}

						map[i][j][2]=-1;
					}
			}
		//������� ���
		if(map_id==0)
		{
			//SetUnit(0, world_x, world_y, party.unit[0].caracter, 0);
			unit_map[unit[0].x][unit[0].y]=-1;
			unit_map[world_x][world_y]=0;
			unit[0].x=world_x;
			unit[0].y=world_y;
		}
		//���θ޴��� ���
		else if(map_id==13)
		{
			//����
			if(main_menu)
			{
				map[4][7][0]=7;
				map[5][7][0]=7;
				map[6][7][0]=7;
			}
			//���&��Ƽ����
			if(main_menu || pre_battle)
			{
				map[5][8][0]=7;
				map[5][9][0]=7;

				map[8][9][0]=7;
				map[9][9][0]=7;
			}
			//��� �˼���
			if(clear_count<2 || main_menu || pre_battle)
			{
				map[5][5][0]=7;
				map[6][5][0]=7;
			}
			//��� ����
			if(clear_count<1 || main_menu || pre_battle)
			{
				map[9][5][0]=7;
				map[9][6][0]=7;
			}
		}		

		MakeMoveMap();
		InitCheckMap(ANIMATION_MAP);
		InitCheckMap(DELAY_MAP, 0);
		EraseFog();

		//��Ŀ���� ���ΰ����Է�
		Focus(unit[0].x, unit[0].y);
		Event(EV_START);

		SelectBGM();

		fclose(fp);
		return true;
	}

	return false;
}

//üũ �� �ʱ�ȭ
void CGame::InitCheckMap(int id, unsigned char value)
{
	for(int i=0; i<x_size; i++)
		for(int j=0; j<y_size; j++)
			check_map[id][i][j]=value;
}

void CGame::SetCheckMap(int id, int x, int y, unsigned char value)
{
	if(x<0 || x>=x_size || y<0 || y>=y_size)return;
	check_map[id][x][y]=value;
}

//�̵��� �� �ִ� ĭ �˻�
void CGame::Search(int id, int x, int y, int dist, int type, char team)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return;
	if(type==0)//����
	{
		if(unit_map[x][y]>=0 && unit[unit_map[x][y]].team!=team)dist=99;	//���� ������ �̵� �Ұ���
			else dist+=(int)move_map[x][y];
	}
	else if(type==-1)	//����
	{
		if(move_map[x][y]==99)dist=99;	//������ �� ���� ���̸� ���൵ �Ұ�
			else dist++;
	}
	else if(type==-2)	//����
	{
		dist++;
	}

	if((int)check_map[id][x][y]<=dist)return;	//�̵����� �����ϰų� ������ �� ������ �˻� ����

	//�����¿� �˻�
	check_map[id][x][y]=(unsigned char)dist;	
	Search(id, x-1, y, dist, type, team);
	Search(id, x+1, y, dist, type, team);
	Search(id, x, y-1, dist, type, team);
	Search(id, x, y+1, dist, type, team);
}

//�� ã��
void CGame::SearchRoad(int id, int x, int y, int type, char team)
{
	InitCheckMap(id);

	check_map[id][x][y]=0;
	Search(id, x-1, y, 0, type, team);
	Search(id, x+1, y, 0, type, team);
	Search(id, x, y-1, 0, type, team);
	Search(id, x, y+1, 0, type, team);
}

//������ ���� ����� ĭ�� �Ÿ�
unsigned char CGame::SearchNear(int id, int x, int y)
{
	unsigned char min=0xff;

	if(x>0 && check_map[id][x-1][y]<min)min=check_map[id][x-1][y];
	if(x<x_size-1 && check_map[id][x+1][y]<min)min=check_map[id][x+1][y];
	if(y>0 && check_map[id][x][y-1]<min)min=check_map[id][x][y-1];
	if(y<y_size-1 && check_map[id][x][y+1]<min)min=check_map[id][x][y+1];

	return min;
}

//���� ǥ��(��������)
void CGame::SetDiamondArea(int id, int x, int y, int dist_min, int dist_max, int value)
{
	for(int i=dist_min; i<=dist_max; i++)
	{
		if(i==0)
			SetCheckMap(id, x, y, value);
		else
		{
			for(int j=0; j<i; j++)SetCheckMap(id, x-(i-j), y+j, value);	//��
			for(int j=0; j<i; j++)SetCheckMap(id, x+(i-j), y-j, value);		//��
			for(int j=0; j<i; j++)SetCheckMap(id, x-j, y-(i-j), value);		//��
			for(int j=0; j<i; j++)SetCheckMap(id, x+j, y+(i-j), value);		//��
		}
	}
}

//���� ǥ��(���簢��)
void CGame::SetSquareArea(int id, int x, int y, int dist_min, int dist_max, int value)
{
	for(int i=dist_min; i<=dist_max; i++)
	{
		if(i==0)
			SetCheckMap(id, x, y, value);
		else
		{
			for(int j=-i; j<i; j++)
			{
				SetCheckMap(id, x-i, y+j, value);	//��
				SetCheckMap(id, x+i, y-j, value);	//��
				SetCheckMap(id, x-j, y-i, value);	//��
				SetCheckMap(id, x+j, y+i, value);	//��
			}
		}
	}
}

//����
void CGame::TransMission(int id, int x, int y, int value, bool water, bool human)
{
	if(x<0 || x>=x_size || y<0 || y>=y_size || check_map[id][x][y]==value)return;

	//���->���, ��->�� �θ� �带 �� ����
	if((human && unit_map[x][y]>=0) || (water && map[x][y][0]==3))
	{
		check_map[id][x][y]=value;
		if(unit_map[x][y]>=0)human=true;
			else human=false;
		if(map[x][y][0]==3)water=true;
			else water=false;
		TransMission(id, x-1, y, value, water, human);
		TransMission(id, x+1, y, value, water, human);
		TransMission(id, x, y-1, value, water, human);
		TransMission(id, x, y+1, value, water, human);
	}
}

//��ũ��
void CGame::Scroll(int MouseX, int MouseY)
{
	if(!sw_focus)
	{
		if(MouseY==479)fy_i=Min(fy_i+8, (y_size*40)-480);
			else if(MouseY==0)fy_i=Max(fy_i-8, 0);
		if(MouseX==0)fx_i=Max(fx_i-8, 0);
			else if(MouseX==639)fx_i=Min(fx_i+8, (x_size*40)-640);
	}
}

//���콺 ���ʹ�ư Ŭ���� �̺�Ʈ
void CGame::LeftClick(int MouseX, int MouseY)
{
	//��ȭ�� ���� �̺�Ʈ �߿��� ��ȭ �ѱ�⸸ ����
	if(question)
	{
		if(selected==0)
		{
			answer=true;
			que.Insert(CM_ANSWER);
		}
		else if(selected==1)
		{
			answer=false;
			que.Insert(CM_ANSWER);
		}
		return;
	}
	else if(dialog)
	{
		que.Insert(CM_NEXT_DIALOG);
		return;
	}

	int px=(MouseX+fx_i)/40;
	int py=(MouseY+fy_i)/40;
	
	//���� ����
	if(click_mode==-1)
	{
		if(state_id>=0)state_id=-1;	//���� ���� ���
			else if(!fog_map[px][py])
			{
				if(unit_map[px][py]>=0)state_id=unit_map[px][py];		//���� ����
					else que.Insert(CM_QUESTION, 53, FUNC_MAINMENU);	//�޴�
			}
	}
	//�̵��� �� ����
	else if(click_mode==1)
	{
		if(check_map[MOVE_TEST_MAP][px][py]<=unit[unit_id].move && (unit_map[px][py]==-1 || unit_map[px][py]==unit_id))UnitMove(unit_id, px, py);
	}
	//�ൿ�� ����
	else if(click_mode==3)
	{
		if(selected>=0)
		{
			//���� ���� ���ϱ�
			if(selected==0)
			{
				SetAttackArea();
			}
			//����
			else if(selected==1)
			{
				click_mode=5;
			}
			//���
			else if(selected==5)
			{						
				EndTurn();
			}
			else
			{
				att_code=2000+selected;
				//���� ���� ���ϱ�
				first_check=false;
				InitCheckMap(ATTACK_TEST_MAP);
				SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);

				click_mode=4;
			}
		}
	}
	//���� ����
	else if(click_mode==5)
	{
		if(selected>=0 && selected2>=0)
		{
			first_check=false;
			InitCheckMap(ATTACK_TEST_MAP);
			att_code=1000+selected*10+selected2;

			//��ǳ&��
			if(selected==0 || selected==10)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);
			//�߹ٶ�
			else if(selected==1)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 2, 0);
			//�ҵ���
			else if(selected==2)
			{
				if(selected2==3)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 5, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
			}
			//�ҹٴ�&��
			else if(selected==3 || selected==5)
			{
				if(selected2<2)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 3, 0);
			}
			//ġ��
			else if(selected==4)
			{
				if(selected2==3)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 5, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 2, 0);
			}
			//����
			else if(selected==6)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 0, 0);
			//��ȭ
			else if(selected==7)
			{
				if(selected2<2)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 2, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 3, 0);
			}
			//����
			else if(selected==8)
			{
				if(selected2==0)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
					else if(selected2==1)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 3, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 4, 0);;
			}
			//�ູ
			else if(selected==9)
			{
				if(selected2==0)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 0, 0);
					else if(selected2==1)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 1, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 2, 0);
			}
			//����&�͵���
			else if(selected==11 || selected==12)
			{
				if(selected2==0)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);
					else if(selected2==1 || selected2==2)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
					else if(selected2==3)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 0, 0);
			}

			click_mode=4;
		}
	}
	//�ൿ ����
	else if(click_mode==4)
	{
		if(check_map[ATTACK_TEST_MAP][px][py]!=99)Action(px, py);
	}
	//���
	else if(click_mode==80)
	{
		//����� ��� ����
		for(int j=0; j<PARTY_MAX; j++)
		{
			if(party.unit[j].life)
			{
				if(party.unit[j].caracter<10 && party.unit[j].caracter!=HERO4 && party.unit[j].caracter!=SOLDIER2 && party.unit[j].caracter!=SOLDIER3)
				{
					int cx, cy;
					cx=35+(j%10)*40;
					cy=113+(j/10)*40;
					if(MouseX>cx && MouseX<=cx+40 && MouseY>cy && MouseY<=cy+40)selected=j;
				}
			}
		}

		//���⸦ �����
		if(selected>=0 && selected2>=0)
		{
			bool equip_able=true;
			//���� ��� ���� �������
			if(selected2<13)	//������ ���
			{
				if(party.unit[selected].arms>0)party.item[party.unit[selected].arms]++;
			}
			else	//������ ���
			{
				if(party.unit[selected].ammor!=-1)
				{
					if(party.unit[selected].dp==equipment[party.unit[selected].ammor].pow)
					{
						party.item[party.unit[selected].ammor]++;
					}
					else	//�μ��� ������ ���� �Ұ���
					{
						equip_able=false;
					}
				}
			}

			if(equip_able)
			{
				//���
				party.unit[selected].Equip(selected2);
				//����� ���� ���� ��Ͽ��� ����
				party.item[selected2]--;
			}
		}
	}
	//��Ƽ ����
	else if(click_mode==81)
	{
		//��� ����
		int ty=37;
		for(int i=0; i<3; i++)
		{
			if(MouseX>445 && MouseX<=605 && MouseY>ty && MouseY<=ty+20)
			{
				selected2=i;

				//��ɿ� ���� �ʱ� ����
				if(i==0)	//���� �ٲٱ�
				{
					selected3=-1;

					for(int j=0; j<PARTY_MAX; j++)
					{
						if(j==0)able_unit[j]=false;
							else able_unit[j]=true;
					}
				}
				else if(i==1)	//�ذ�
				{
					for(int j=0; j<PARTY_MAX; j++)
					{
						if(party.unit[j].life)
						{
							if(party.unit[j].caracter<5)able_unit[j]=false;
								else able_unit[j]=true;
						}
						else able_unit[j]=false;
					}
				}
				else if(i==2)	//�����ü
				{
					for(int j=0; j<PARTY_MAX; j++)
					{
						if(party.unit[j].life)
						{
							if(party.unit[j].caracter<10 && party.unit[j].caracter!=HERO4 && party.unit[j].caracter!=SOLDIER2 && party.unit[j].caracter!=SOLDIER3)able_unit[j]=true;
								else able_unit[j]=false;
						}
						else able_unit[j]=false;
					}
				}
			}
			ty+=20;
		}

		if(selected>=0)
		{
			//��� ����
			if(selected2==0)	//���� �ٲٱ�
			{
				if(selected3==-1)selected3=selected;
				else
				{
					CUnit tmp;
					memcpy(&tmp, &party.unit[selected], sizeof(CUnit));
					memcpy(&party.unit[selected], &party.unit[selected3], sizeof(CUnit));
					memcpy(&party.unit[selected3], &tmp, sizeof(CUnit));
					selected3=-1;
				}
			}
			else if(selected2==1)	//�ذ�
			{
				party.unit[selected].life=false;
				PartySort();

				//����, �Ұ��� �ٽ� ����
				for(int j=0; j<PARTY_MAX; j++)
				{
					if(party.unit[j].life)
					{
						if(party.unit[j].caracter<5)able_unit[j]=false;
							else able_unit[j]=true;
					}
					else able_unit[j]=false;
				}
			}
			else if(selected2==2)	//�����ü
			{
				//���� ��� ���� �������
				if(party.unit[selected].arms>0)
				{
					party.item[party.unit[selected].arms]++;
					party.unit[selected].TakeOff(EQ_ARMS);
				}
				if(party.unit[selected].ammor!=-1)
				{
					if(party.unit[selected].dp==equipment[party.unit[selected].ammor].pow)
					{
						party.item[party.unit[selected].ammor]++;
						party.unit[selected].TakeOff(EQ_AMMOR);
					}
				}
			}
		}
	}
	//��ǰ ����
	else if(click_mode==90)
	{
		if(selected>=0)
		{
			if(party.money>=equipment[selected].price)
			{
				party.item[selected]++;
				party.money-=equipment[selected].price;
			}
		}
	}
	//�б�
	else if(click_mode==100)
	{
		if(selected>=0 && party.money>=select_price)
		{
			party.unit[selected].mizi_level++;
			if(party.unit[selected].mizi_level>=4)able_unit[selected]=false;
			party.money-=select_price;
		}
	}
	//�뺴 ���
	else if(click_mode==110)
	{
		if(selected>=0)
		{
			if(party.money>=select_price && !party.unit[PARTY_MAX-1].life)	//���� ����ϰ� �� �ڸ��� ������
			{
				for(int i=1; i<PARTY_MAX; i++)
					if(!party.unit[i].life)	//��Ƽ�� �߰�
					{
						party.unit[i].SetUnit(0, 0, selected, 0);
						party.money-=select_price;
						break;
					}
			}
		}
	}
	//��Ƽ���� ���� �ٸ� ���� �ֱ�
	else if(click_mode==111)
	{
		if(selected>=0)
		{
			party.unit[selected].SetUnit(0, 0, selected2, 0);

			escape=1;
			map_id=0;
		}
	}
	//������
	else if(click_mode==120)
	{
		if(selected>=0 && party.money>=select_price)
		{
			party.unit[selected].dp=equipment[party.unit[selected].ammor].pow;
			able_unit[selected]=false;
			party.money-=select_price;
		}
	}
}

//���콺 �����ʹ�ư Ŭ���� �̺�Ʈ
void CGame::RightClick(int MouseX, int MouseY)
{	
	if(question)			//���� ���¿��� '����'
	{
		answer=false;
		que.Insert(CM_ANSWER);
		return;
	}
	else if(dialog)return;	//��ȭ ���¿��� ��� �Ұ���

	if(click_mode==-1)
	{
		if(state_id>=0)state_id=-1;
		else
		{
			click_mode=1;
			Focus(unit_x, unit_y);
		}
	}
	else if(click_mode==1)
	{
		if(map_id<13)click_mode=-1;
	}
	else if(click_mode==3)
	{
		fx_i=MaxMin(unit_x*40-300, 0, (x_size*40)-640);
		fy_i=MaxMin(unit_y*40-220,0,(y_size*40)-480);
		//������ ����ġ��
		unit_map[unit[unit_id].x][unit[unit_id].y]=-1;
		unit[unit_id].x=unit_x;
		unit[unit_id].y=unit_y;
		unit_map[unit_x][unit_y]=unit_id;
		unit[unit_id].wait=100;
		
		click_mode=1;
	}
	else if(click_mode==4 || click_mode==5)
	{
		click_mode=3;
	}
	else if(click_mode==80 || click_mode==81 || click_mode==90 || click_mode==100 || click_mode==110 || click_mode==120)
	{
		click_mode=0;
	}
}

void CGame::Process()
{
	//AI�� ����
	static int target_id;
	static int target_x, target_y;
	static int act_3;		//click_mode 3�� �� �� �ൿ

	if(que.GetMax()==0)
	{
		if(click_mode==0)
		{
			//���� �̺�Ʈ
			bool moveable=false;
			for(int i=0; i<UNIT_MAX; i++)
				if(unit[i].life && unit[i].wait>=100)
				{
					//���� ����
					unit_id=i;
					unit_x=unit[unit_id].x;
					unit_y=unit[unit_id].y;
					SearchRoad(MOVE_TEST_MAP, unit_x, unit_y, unit[unit_id].move_type, unit[unit_id].team);

					//�÷��̾� �������� AI���� �Ǻ�
					if(unit[unit_id].team==0)
					{
						sw_ai=false;
					}
					else
					{
						AI_thinking=0;	//AI ���� �ð�
						sw_ai=true;
					}

					Focus(unit_x, unit_y, que.que_time+1);
					moveable=true;
					
					//��&�ູ�� ȿ��					
					if(unit[unit_id].state==1)		//��
					{
						if(unit[unit_id].hp>1)Damage(unit_x, unit_y, 1, 7);
					}
					else if(unit[unit_id].state==2)	//�͵�
					{
						Damage(unit_x, unit_y, 1, 7);
						if(unit[unit_id].hp==0)break;	//�����
					}
					else if(unit[unit_id].state==3)	//�ູ
					{
						Damage(unit_x, unit_y, -1, 3);
					}
					else if(unit[unit_id].state==4)	//���� ���� �ູ
					{
						Damage(unit_x, unit_y, -2, 3);
					}

					click_mode=1;
					break;
				}

			if(!moveable)
				for(int i=0; i<UNIT_MAX; i++)
					if(unit[i].life)unit[i].wait=Min(100, unit[i].wait+(unit[i].move/2));
		}

		/////////////////////////////////////////////////////////////////
		//AI�� ����
		////////////////////////////////////////////////////////////////
		if(sw_ai)
		{
			static bool do_anything;

			///////////////////////////////////////////////////////
			//AI�� �̵�
			if(click_mode==1)
			{
				if(AI_thinking==0)
				{
					//�Ʊ��� ��ġ Ž��
					unsigned char best=0xff;	//���� ����� �Ÿ�
					int target_id1=-1;			//���� ����� ����
					int death=999;				//���� ���� HP
					int target_id2=-1;			//���� HP�� ���� ����

					//���� ����� ���ְ� ���� ���� ��ģ ���� ã��
					for(int i=0; i<UNIT_MAX; i++)
						if(unit[i].life && unit[i].team!=unit[unit_id].team)
						{						
							if(SearchNear(MOVE_TEST_MAP, unit[i].x, unit[i].y)<best)
							{
								best=SearchNear(MOVE_TEST_MAP, unit[i].x, unit[i].y);
								target_id1=i;
							}
							if(unit[i].hp<death)
							{
								death=unit[i].hp;
								target_id2=i;
							}
						}

					do_anything=false;
					if(target_id2>=0)
					{
						//���� ���� ��ģ ������ �ѹ��� �̵� ������ ���� ������ ����
						if(SearchNear(MOVE_TEST_MAP, unit[target_id2].x, unit[target_id2].y)<=unit[unit_id].move)
						{
							target_id=target_id2;
							do_anything=true;
						}
						//�ƴϸ� ���� ����� ���� �ִ� �� ���� �̵�
						else if(target_id1>=0)
						{
							int sight[]={0,6,6,6,6,6,0,0,0,99,99,15,15,10,20,0,99};	//�þ�: ���� �� ���� �ȿ� ������ ����
							int my_sight;
							if(map_id<6)my_sight=99;	//����� �������� ������ ����
								else my_sight=sight[unit[unit_id].caracter];
							if(SearchNear(MOVE_TEST_MAP, unit[target_id1].x, unit[target_id1].y)<=my_sight)
							{
								target_id=target_id1;
								do_anything=true;
							}
						}
					}

					//���� �������� �̵�
					if(do_anything)
					{
						test=target_id;
						SearchRoad(DISTANCE_MAP, unit[target_id].x, unit[target_id].y, -2, unit[unit_id].team);

						int move_range, least_range;
						bool move_end=false;

						//���⿡ ���� ���� ����
						if(unit[unit_id].arms==6)
						{
							move_range=3;
							least_range=1;
						}
						else if(unit[unit_id].arms==9)
						{
							move_range=3;
							least_range=2;
						}
						else if(unit[unit_id].arms==5 || unit[unit_id].arms==10)
						{
							move_range=2;
							least_range=1;
						}
						else if(unit[unit_id].arms==7 || unit[unit_id].arms==8)
						{
							move_range=2;
							least_range=2;				
						}
						else
						{
							move_range=1;
						}
						int old_move_range=move_range;

						//���� ������ ��ҷ� �̵�
						while(!move_end)
						{
							for(int j=0; j<y_size && !move_end; j++)
								for(int i=0; i<x_size && !move_end; i++)
								{
									if(check_map[DISTANCE_MAP][i][j]==move_range && (check_map[MOVE_TEST_MAP][i][j]<=(unsigned char)unit[unit_id].move && (unit_map[i][j]==-1 || unit_map[i][j]==unit_id)))
									{
										target_x=i;
										target_y=j;
										move_end=true;
									}
								}

							//�� �´� ���ڰ� ������ ���ڸ� �÷� ��
							if(!move_end)
							{
								//2ĭ �̻� ���� �� �� �ִ� ���� ���� �հŸ��� �븮�� �� �Ǹ� �Ÿ��� ������
								if(old_move_range>1)
								{
									move_range--;
									//�� �˻��� ���� ���� ��
									if(move_range<least_range)
									{
										move_range=old_move_range+1;
										old_move_range=1;
									}
								}
								else
								{
									move_range++;
								}

								//100ĭ �Ÿ��� ������ �׸� ��
								if(move_range>=100)move_end=true;
							}
						}
					}
					else target_x=-1;
				}

				if(++AI_thinking==5 || fog_map[unit_x][unit_y])
				{
					if(target_x==-1)click_mode=3;
						else UnitMove(unit_id, target_x, target_y);
					AI_thinking=0;
				}
			}
			///////////////////////////////////////////////////
			//AI�� �ൿ
			else if(click_mode==3)
			{
				if(AI_thinking==0)
				{
					//������ �õ�
					InitCheckMap(ATTACK_TEST_MAP);

					if(unit[unit_id].arms==3 || unit[unit_id].arms==4)	//���&ȸ����
					{
						SetSquareArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);
					}
					else if(unit[unit_id].arms==5 || unit[unit_id].arms==6)	//â
					{
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x-1, unit[unit_id].y, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x-2, unit[unit_id].y, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x+1, unit[unit_id].y, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x+2, unit[unit_id].y, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y-1, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y-2, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y+1, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y+2, 0);
						//�� â
						if(unit[unit_id].arms==6)
						{
							SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x-3, unit[unit_id].y, 0);
							SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x+3, unit[unit_id].y, 0);
							SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y-3, 0);
							SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y+3, 0);
						}
					}
					else if(unit[unit_id].arms==7 || unit[unit_id].arms==8)//Ȱ
					{
						SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 2, 2, 0);
					}
					else if(unit[unit_id].arms==9)//�� Ȱ
					{
						SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 2, 3, 0);
					}
					else if(unit[unit_id].arms==10)	//�氥
					{
						SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
					}
					//��Ÿ 1ĭ ¥��
					else
					{
						SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);
					}

					if(do_anything)
					{
						//��ǥ���� ���� ���� ������ ������ ����
						if(check_map[ATTACK_TEST_MAP][unit[target_id].x][unit[target_id].y]==0)
						{
							act_3=0;
						}
						//�ƴϸ� �ٸ� ��ǥ�� ã��
						else
						{
							bool att_end=false;
							for(int j=0; j<y_size && !att_end; j++)
								for(int i=0; i<x_size && !att_end; i++)
								{
									if(check_map[ATTACK_TEST_MAP][i][j]==0 && unit_map[i][j]>=0 && unit[unit_map[i][j]].team!=unit[unit_id].team)
									{
										target_id=unit_map[i][j];
										act_3=0;
										att_end=true;
									}
								}

							if(!att_end)act_3=5;
						}
					}
					else
					{
						//��ũ����&�޷��
						if(unit[unit_id].caracter==DARKQUIZ || unit[unit_id].caracter==HERO6)act_3=1;
							else act_3=5;
					}
				}

				if(++AI_thinking>=5 || fog_map[unit[unit_id].x][unit[unit_id].y])
				{
					//�� �� ������ ���
					if(act_3==5)
					{
						EndTurn();
					}
					//����
					else if(act_3==1)
					{
						//��ũ����
						if(unit[unit_id].caracter==DARKQUIZ)
						{
							AI_thinking=0;
							att_code=1123;
							first_check=false;
							InitCheckMap(ATTACK_TEST_MAP);
							SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 0, 0);
							click_mode=4;
						}
						//�޷��
						else if(unit[unit_id].caracter==HERO6)
						{
							AI_thinking=0;
							att_code=1103;
							first_check=false;
							InitCheckMap(ATTACK_TEST_MAP);
							SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);
							click_mode=4;
						}
					}
					//���� �����ϸ� ����
					else if(act_3==0)
					{
						AI_thinking=0;
						SetAttackArea();
					}
				}
			}
			///////////////////////////////////////////////////
			//AI�� ����
			else if(click_mode==4)
			{
				if(act_3==1)
				{
					//�͵��� ����
					if(unit[unit_id].caracter==DARKQUIZ)
					{
						target_x=unit[unit_id].x;
						target_y=unit[unit_id].y;
						if(++AI_thinking>=20)
						{
							SetDamageArea(target_x, target_y);
							Action(target_x, target_y);
						}
					}
					//�� ����
					else if(unit[unit_id].caracter==HERO6)
					{
						target_x=unit[unit_id].x;
						target_y=unit[unit_id].y+1;
						if(++AI_thinking>=20)
						{
							SetDamageArea(target_x, target_y);
							Action(target_x, target_y);
						}
					}
				}
				else
				{
					if(AI_thinking==0)
					{
						bool att_end=false;
						for(int j=0; j<y_size && !att_end; j++)
							for(int i=0; i<x_size && !att_end; i++)
							{
								if(check_map[ATTACK_TEST_MAP][i][j]==0)
								{
									SetDamageArea(i, j);
									if(check_map[DAMAGE_TEST_MAP][unit[target_id].x][unit[target_id].y]==0)
									{
										target_x=i;
										target_y=j;
										att_end=true;
									}
								}
							}
					}

					if(++AI_thinking>=20)
					{
						Action(target_x, target_y);
					}
				}
			}
		}
	}

	//AI�� ������ �� ���콺 ��ġ
	if(sw_ai)
	{
		if(click_mode==1 || click_mode==2 || click_mode==4)
		{
			AI_MouseX=target_x*40-fx_i+20;
			AI_MouseY=target_y*40-fy_i+20;
		}
		else if(click_mode==3)
		{
			int ai_ty=0;
			for(int i=0; i<4; i++)
				if(cara[unit[unit_id].caracter].command[i]==act_3)
				{
					ai_ty=i;
					break;
				}

			AI_MouseX=panel_x+30;
			AI_MouseY=panel_y+10+ai_ty*20;
		}
	}

	//ť�� ��� ��ɾ� ó��
	bool empty=true;	//ť�� ����� ������� �˻�
	QueData* first_focus=NULL;	//�ΰ��� ��Ŀ�� ����� �浹���� �ʰ� �ϴ� ����ġ
	QueData* qp;	//���� ó���Ǵ� ť �����͸� ����Ű�� ������
	for(int i=0; i<que.GetMax(); i++)
	{
		qp=que.GetQue(i);

		if(qp->time==que.que_time)
		{
			empty=false;

			////////////////////////////////////
			//���� ���߱�
			if(qp->command==CM_FOCUS)
			{
				if(first_focus==NULL)
				{
					first_focus=qp;
					sw_focus=true;
					//X��ǥ ���߱�
					int distance=qp->x-fx_i;
					if(abs(distance)<8)fx_i=qp->x;
						else
						{
							bool minus;
							if(qp->x-fx_i<0)minus=true;
								else minus=false;
							distance=Max(Min(abs(distance)/10,16),8);
							if(minus)fx_i-=distance;
								else fx_i+=distance;
						}
					//Y��ǥ ���߱�
					distance=qp->y-fy_i;
					if(abs(distance)<8)fy_i=qp->y;
						else
						{
							bool minus;
							if(qp->y-fy_i<0)minus=true;
								else minus=false;
							distance=Max(Min(abs(distance)/10,16),8);
							if(minus)fy_i-=distance;
								else fy_i+=distance;
						}
					//��Ŀ�� ��
					if(fx_i==qp->x && fy_i==qp->y)
					{							
						que.Delete(i);
						sw_focus=false;
					}
				}
				//����� ���̸� ��Ŀ�� ����
				else if(abs((first_focus->x)-(qp->x))<320 && abs((first_focus->y)-(qp->y))<240)que.Delete(i);
			}
			////////////////////////////////////
			//�̵�
			else if(qp->command==CM_MOVE)
			{
				sw_move=true;
				if(!fog_map[unit[qp->id].x][unit[qp->id].y] && unit[qp->id].sx==0 && unit[qp->id].sy==0 && unit[qp->id].caracter!=HANAZUM &&unit[qp->id].caracter!=NEYUM)lib->PlaySound(0);
				//��
				if(qp->x==4)
				{
					//��� �ӿ��� �̵��� ������
					if(fog_map[unit[qp->id].x][unit[qp->id].y] && fog_map[unit[qp->id].x-1][unit[qp->id].y])
					{
						unit[qp->id].x--;
						que.Delete(i);
					}
					//���� �̵�
					else
					{
						unit[qp->id].sx-=4;
						if(unit[qp->id].sx<=-40)
						{
							unit[qp->id].x--;
							unit[qp->id].sx=0;
							que.Delete(i);
						}
					}
				}
				//��
				else if(qp->x==6)
				{
					//��� �ӿ��� �̵��� ������
					if(fog_map[unit[qp->id].x][unit[qp->id].y] && fog_map[unit[qp->id].x+1][unit[qp->id].y])
					{
						unit[qp->id].x++;
						que.Delete(i);
					}
					//���� �̵�
					else
					{
						unit[qp->id].sx+=4;
						if(unit[qp->id].sx>=40)
						{
							unit[qp->id].x++;
							unit[qp->id].sx=0;
							que.Delete(i);
						}
					}
				}
				//��
				else if(qp->x==8)
				{
					//��� �ӿ��� �̵��� ������
					if(fog_map[unit[qp->id].x][unit[qp->id].y] && fog_map[unit[qp->id].x][unit[qp->id].y-1])
					{
						unit[qp->id].y--;
						que.Delete(i);
					}
					//���� �̵�
					else
					{
						unit[qp->id].sy-=4;
						if(unit[qp->id].sy<=-40)
						{
							unit[qp->id].y--;
							unit[qp->id].sy=0;
							que.Delete(i);
						}
					}
				}
				//��
				else if(qp->x==2)
				{
					//��� �ӿ��� �̵��� ������
					if(fog_map[unit[qp->id].x][unit[qp->id].y] && fog_map[unit[qp->id].x][unit[qp->id].y+1])
					{
						unit[qp->id].y++;
						que.Delete(i);
					}
					//���� �̵�
					else
					{
						unit[qp->id].sy+=4;
						if(unit[qp->id].sy>=40)
						{
							unit[qp->id].y++;
							unit[qp->id].sy=0;
							que.Delete(i);
						}
					}
				}
			}
			////////////////////////////////////
			//�̵� ��
			else if(qp->command==CM_MOVEEND)
			{
				//���� �� ����
				unit_map[qp->x][qp->y]=-1;
				unit_map[unit[qp->id].x][unit[qp->id].y]=qp->id;

				if(battle)click_mode=3;
					else click_mode=0;

				Event(EV_MOVE, qp->id, unit[qp->id].x, unit[qp->id].y);
				//���� �̺�Ʈ
				unit[unit_id].wait-=check_map[MOVE_TEST_MAP][unit[unit_id].x][unit[unit_id].y]*10;

				//�۾� ����
				sw_move=false;
				que.Delete(i);
			}
			////////////////////////////////////
			//���� �Ǵ� ȸ��
			else if(qp->command==CM_DAMAGE)
			{				
				if(qp->x>0)
				{
					//����
					int tremble=(qp->y)%4;
					if(tremble==0 || tremble==3)unit[qp->id].sx-=4;
						else if(tremble==1 || tremble==2)unit[qp->id].sx+=4;
				}

				//�ð� �帣��
				qp->y++;
				if(qp->y>=20)
				{
					//���
					if(unit[qp->id].hp==0 && unit[qp->id].caracter!=NEYUM/*�һ� ĳ����*/)
					{
						//�����̸� �� ȹ��
						if(unit[qp->id].team!=0)party.money+=have_money[unit[qp->id].caracter];
						//�Ʊ��̸� ��Ƽ���� ����
						else party.unit[unit[qp->id].party_id].life=false;

						if(qp->id==1)	//������ �̺�Ʈ
						{
							if(map_id==7)
							{
								que.Insert(CM_DIALOG, 65, 70, FUNC_BOSS);
								event_switch[SW_GAMIUL_DEFEAT]=true;
							}
							else if(map_id==8)
							{
								que.Insert(CM_DIALOG, 84, 90, FUNC_BOSS);
								event_switch[SW_MIBUGA_DEFEAT]=true;
							}
							else if(map_id==9)
							{
								que.Insert(CM_DIALOG, 57, 64, FUNC_BOSS);
								event_switch[SW_SIKUBEL_DEFEAT]=true;
							}
							else if(map_id==10)
							{
								que.Insert(CM_DIALOG, 71, 83, FUNC_BOSS);
								event_switch[SW_RUBELBU_DEFEAT]=true;
							}
							else if(map_id==11)
							{
								que.Insert(CM_DIALOG, 91, 102, FUNC_BOSS);
							}
							else if(map_id==12)
							{
								que.Insert(CM_DIALOG, 115, 120, FUNC_BOSS);
							}
						}
						else	//�Ϲ�
						{
							unit[qp->id].life=false;
							unit_map[unit[qp->id].x][unit[qp->id].y]=-1;
							Focus(unit[qp->id].x, unit[qp->id].y);
							que.Insert(CM_DIE, qp->id, 0, 0, que.que_time+1);
						}
					}

					unit[qp->id].sx=0;
					que.Delete(i);
				}
			}
			////////////////////////////////////
			//���
			else if(qp->command==CM_DIE)
			{
				qp->y++;
				//��
				if(qp->y>=50)
				{
					unit[qp->id].sy-=10;
					if(qp->y>=100)
					{
						unit[qp->id].soul=false;

						if(qp->id==0)escape=2;	//���� ����
							else if((qp->id==1 && boss) || Eliminate())	//Ŭ����
							{
								//�Ϲ� ����
								if(map_id<7)
								{
									que.Insert(CM_DIALOG, 56, 56, FUNC_WORLDMAP);
									party.money+=50;
								}
							}
						que.Delete(i);
					}
				}
			}
			////////////////////////////////////
			//����
			else if(qp->command==CM_WARP)
			{
				//����
				if(qp->y==0)
				{
					unit[qp->id].life=false;
					unit[qp->id].sy=-400;
					qp->y=1;
				}
				unit[qp->id].sy+=8;
				//��
				if(unit[qp->id].sy>=0)
				{
					unit[qp->id].life=true;
					que.Delete(i);
				}
			}
			////////////////////////////////////
			//��ȭ
			else if(qp->command==CM_DIALOG)
			{
				//��ȭ ���¸� �Ѱ� ��ȭ ���۰� �� ����
				dialog=true;
				dialog_no=qp->id;
				dialog_end=qp->x;
				answer_func=qp->y;				
				click_mode=-1;

				que.Delete(i);
			}
			////////////////////////////////////
			//���� ��ȭ
			else if(qp->command==CM_NEXT_DIALOG)
			{
				dialog_no++;
				if(dialog_no==45)Focus(4,3);
					else if(dialog_no==46)Focus(25,4);
					else if(dialog_no==47)Focus(21,16);
					else if(dialog_no==48)Focus(27,18);
				//��ȭ ������
				if(dialog_no>dialog_end)
				{
					dialog=false;
					click_mode=0;
					Function(answer_func);
				}

				que.Delete(i);
			}
			////////////////////////////////////
			//����
			else if(qp->command==CM_QUESTION)
			{
				//��ȭ ���¸� �Ѱ� ��ȭ ���۰� �� ����
				question=true;
				dialog_no=qp->id;
				answer_func=qp->x;
				answer_func2=qp->y;

				que.Delete(i);
			}
			////////////////////////////////////
			//���
			else if(qp->command==CM_ANSWER)
			{
				question=false;
				if(answer)Function(answer_func);
					else Function(answer_func2);
				que.Delete(i);
			}
			////////////////////////////////////
			//ȭ�� ����
			else if(qp->command==CM_SHAKE)
			{
				int tmp=(qp->y)%4;
				if(tmp==0 || tmp==3)fx_i-=4;
					else if(tmp==1 || tmp==2)fx_i+=4;
				if(++(qp->y)>=20)que.Delete(i);
			}

			//�����
			/*char* command_name[]={"��Ŀ��","�̵�","�̵���","����","ġ��","���"};
			for(int i=0; i<que.GetMax(); i++)
				fprintf(fp2, "%s %d %d %d %d\n", command_name[que.GetQue(i)->command], que.GetQue(i)->id, que.GetQue(i)->x, que.GetQue(i)->y, que.GetQue(i)->time);
			fprintf(fp2, "------------------------------------------------------\n");*/
			//Sleep(100);
		}
	}

	//���� ó���� ��ɾ ���� �� ���μ����� ����
	if(empty)
	{
		for(int i=0; i<que.GetMax(); i++)
			if(qp->time>que.que_time)
			{
				que.que_time++;
				break;
			}
	}
}

void CGame::Event(int type, int id, int x, int y)
{
	////////////////////////////////////////
	//���� �̺�Ʈ
	if(type==EV_START)
	{
		//���ѷα�
		if(map_id==0)
		{
			if(!event_switch[SW_PROLOGUE])
			{
				que.Insert(CM_DIALOG, 0, 6);
				event_switch[SW_PROLOGUE]=true;
			}
		}
		//�ް��� ����
		else if(map_id==1)
		{
			if(!event_switch[SW_HYUGARU])
			{
				que.Insert(CM_DIALOG, 44, 51);
				event_switch[SW_HYUGARU]=true;
			}
		}
		//���̾� ����
		else if(map_id==7)
		{
			if(!event_switch[SW_GAMIUL])
			{
				que.Insert(CM_DIALOG, 13, 18);
				event_switch[SW_GAMIUL]=true;
			}
		}
		//�̹��� ����
		else if(map_id==8)
		{
			if(!event_switch[SW_MIBUGA])
			{
				que.Insert(CM_DIALOG, 24, 28);
				event_switch[SW_MIBUGA]=true;
			}
		}
		//��ũ�� ����
		else if(map_id==9)
		{
			if(!event_switch[SW_SIKUBEL])
			{
				que.Insert(CM_DIALOG, 19, 23);
				event_switch[SW_SIKUBEL]=true;
			}
		}
		//�级�� ����
		else if(map_id==10)
		{
			if(!event_switch[SW_RUBELBU])
			{
				que.Insert(CM_DIALOG, 29, 34);
				event_switch[SW_RUBELBU]=true;
			}
		}
		//��ũ���� �� 1��
		else if(map_id==11)
		{
			if(!event_switch[SW_HYURUSI])
			{
				que.Insert(CM_DIALOG, 35, 43);
				event_switch[SW_HYURUSI]=true;
			}
		}
		//��ũ���� �� 2��
		else if(map_id==12)
		{
			que.Insert(CM_DIALOG, 103, 105, FUNC_QUIZ1);
		}
		//���� �ִϸ��̼�
		else if(map_id==13 || map_id==14)
		{
			que.Insert(CM_WARP, 0);
		}
	}
	////////////////////////////////////////
	//�̵� �� �̺�Ʈ
	else if(type==EV_MOVE)
	{
		//�̺�Ʈ
		if(map_id==0)	//�����
		{
			bool move_event=false;
			//Ư�� ��ҷ� �̵�
			int point[][3]={{0,4,8},{8,0,7},{13,10,9},{15,2,10},{8,5,1},{9,5,11}};
			for(int i=0; i<6; i++)
				if(x==point[i][0] && y==point[i][1])
				{
					map_id=point[i][2];
					escape=1;
					world_x=unit[id].x;
					world_y=unit[id].y;
					move_event=true;
				}

			if(!move_event && !no_enemy && (rand()%2)==0)que.Insert(CM_DIALOG, 54, 55, FUNC_ENEMY);
		}		
		else if(map_id==13)	//���� �޴�
		{
			//�� ����
			if(x==7 && y==4)
			{
				escape=3;
				map_id=0;
			}
			//����
			else if(x==4 && y==7)
			{
				save_mode=true;
				escape=1;
				map_id=14;
				LoadInfo();
			}
			//���
			else if(x==5 && y==9)
			{
				selected=-1;
				click_mode=80;
			}
			//��Ƽ ����
			else if(x==9 && y==9)
			{
				selected2=-1;
				for(int i=0; i<PARTY_MAX; i++)able_unit[i]=true;
				click_mode=81;
			}
			//���ư���
			else if(x==7 && y==7)
			{
				if(!main_menu)Load(0);
			}
			//�ҷ�����
			else if(x==10 && y==7)
			{
				save_mode=false;				
				escape=1;
				map_id=14;
				LoadInfo();
			}
			//������
			else if(x==7 && y==10)
			{
				escape=99;
			}
			//��� �˼���
			else if(x==5 && y==5)
			{
				shop[0]=BANDIT;
				shop[1]=NEYUM;
				shop[2]=HERO6;
				shop[3]=DARKQUIZ;
				click_mode=110;
			}
			//��� ����
			else if(x==9 && y==5)
			{
				shop[0]=10;
				shop[1]=11;
				shop[2]=12;
				shop[3]=16;
				click_mode=90;
			}
		}
		else if(map_id==14)	//���� �޴�
		{
			//���ư���
			if(x==3 && y==6)
			{
				escape=1;
				map_id=13;
			}
			//����
			else if(x==6)
			{
				for(int i=0; i<=3; i++)
				{
					if(y==4+(i*2))
					{
						if(save_mode)
						{
							Save(i+1);
							LoadInfo();
						}
						else Load(i+1);
					}
				}
			}
		}
		else
		{
			//�ް��� ����
			if(map_id==1)
			{
				//�ΰ�
				if(x==9 && y==9)
				{
					que.Insert(CM_DIALOG, 7, 7);
				}
				else if(x==12 && y==9)
				{
					que.Insert(CM_DIALOG, 8, 8);
				}
				else if(x==15 && y==9)
				{
					que.Insert(CM_DIALOG, 9, 9);
				}
				else if(x==22 && y==9)
				{
					que.Insert(CM_DIALOG, 10, 10);
				}
				else if(x==25 && y==9)
				{
					que.Insert(CM_DIALOG, 11, 11);
				}
				else if(x==28 && y==9)
				{
					que.Insert(CM_DIALOG, 12, 12);
				}
				else if(x==7 && y==17)
				{
					que.Insert(CM_DIALOG, 125, 125);
				}
				else if(x==10 && y==16)
				{
					que.Insert(CM_DIALOG, 126, 126);
				}
				else if(x==25 && y==21)
				{
					que.Insert(CM_DIALOG, 127, 127);
				}
				//������ ��
				else if(x==13 && y==21)
				{
					if(clear_count==3)
					{
						shop[0]=HERO2;
						shop[1]=HERO3;
						shop[2]=HERO4;
						shop[3]=HERO5;
						click_mode=110;
					}
					else que.Insert(CM_DIALOG, 128, 128);
				}
				//������
				else if(x==4 && y==3)
				{
					shop[0]=1;
					shop[1]=2;
					shop[2]=shop[3]=-1;
					click_mode=90;
				}
				//������
				else if(x==25 && y==4)
				{
					for(int i=0; i<PARTY_MAX; i++)
						if(party.unit[i].ammor>=0 && equipment[party.unit[i].ammor].pow>party.unit[i].dp)able_unit[i]=true;
							else able_unit[i]=false;

					click_mode=120;
				}
				//�б�
				else if(x==21 && y==16)
				{
					for(int i=0; i<PARTY_MAX; i++)
						if(party.unit[i].life && party.unit[i].mizi_type>=0 && party.unit[i].mizi_level<4)able_unit[i]=true;
							else able_unit[i]=false;
						
					click_mode=100;
				}
				//�˼���
				else if(x==27 && y==18)
				{
					shop[0]=SOLDIER1;
					shop[1]=SOLDIER2;
					shop[2]=SOLDIER3;
					shop[3]=HANAZUM;
					click_mode=110;
				}
			}
			//���̾� ����
			else if(map_id==7)
			{
				if(!battle)
				{
					//������
					if(x==24 && y==8)
					{
						shop[0]=3;
						shop[1]=4;
						shop[2]=shop[3]=-1;
						click_mode=90;
					}
					//������
					else if(x==7 && y==16)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].ammor>=0 && equipment[party.unit[i].ammor].pow>party.unit[i].dp)able_unit[i]=true;
								else able_unit[i]=false;

						click_mode=120;
					}
					//�б�
					else if(x==4 && y==11)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].life && party.unit[i].mizi_type>=0 && party.unit[i].mizi_level<4)able_unit[i]=true;
								else able_unit[i]=false;
							
						click_mode=100;
					}
					//�˼���
					else if(x==27 && y==16)
					{
						shop[0]=SOLDIER1;
						shop[1]=SOLDIER2;
						shop[2]=SOLDIER3;
						shop[3]=MAEQO;
						click_mode=110;
					}
					//�ΰ�
					else if(x==7 && y==4)
					{
						que.Insert(CM_DIALOG, 129, 129);
					}
					else if(x==27 && y==3)
					{
						que.Insert(CM_DIALOG, 130, 130);
					}
				}
			}
			//�̹��� ����
			else if(map_id==8)
			{
				if(!battle)
				{
					//������
					if(x==13 && y==5)
					{
						shop[0]=5;
						shop[1]=6;
						shop[2]=shop[3]=-1;
						click_mode=90;
					}
					//������
					else if(x==20 && y==6)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].ammor>=0 && equipment[party.unit[i].ammor].pow>party.unit[i].dp)able_unit[i]=true;
								else able_unit[i]=false;

						click_mode=120;
					}
					//�б�
					else if(x==4 && y==7)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].life && party.unit[i].mizi_type>=0 && party.unit[i].mizi_level<4)able_unit[i]=true;
								else able_unit[i]=false;
							
						click_mode=100;
					}
					//�˼���
					else if(x==8 && y==5)
					{
						shop[0]=SOLDIER1;
						shop[1]=SOLDIER2;
						shop[2]=SOLDIER3;
						shop[3]=BIGROCK;
						click_mode=110;
					}
					//�ΰ�
					else if(x==11 && y==5)
					{
						que.Insert(CM_DIALOG, 131, 133);
					}
					else if(x==17 && y==5)
					{
						que.Insert(CM_DIALOG, 134, 136);
					}
					else if(x==17 && y==8)
					{
						que.Insert(CM_DIALOG, 137, 139);
					}
					else if(x==9 && y==9)
					{
						que.Insert(CM_DIALOG, 140, 140);
					}
				}
			}
			//��ũ�� ����
			else if(map_id==9)
			{
				if(!battle)
				{
					//������
					if(x==2 && y==8)
					{
						shop[0]=7;
						shop[1]=8;
						shop[2]=9;
						shop[3]=-1;
						click_mode=90;
					}
					//������
					else if(x==8 && y==15)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].ammor>=0 && equipment[party.unit[i].ammor].pow>party.unit[i].dp)able_unit[i]=true;
								else able_unit[i]=false;

						click_mode=120;
					}
					//�б�
					else if(x==21 && y==6)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].life && party.unit[i].mizi_type>=0 && party.unit[i].mizi_level<4)able_unit[i]=true;
								else able_unit[i]=false;
							
						click_mode=100;
					}
					//�˼���
					else if(x==10 && y==6)
					{
						shop[0]=SOLDIER1;
						shop[1]=SOLDIER2;
						shop[2]=SOLDIER3;
						shop[3]=BANG;
						click_mode=110;
					}
					//�ΰ�
					else if(x==7 && y==2)
					{
						que.Insert(CM_DIALOG, 141, 141);
					}
					else if(x==15 && y==2)
					{
						que.Insert(CM_DIALOG, 142, 142);
					}
					else if(x==21 && y==13)
					{
						que.Insert(CM_DIALOG, 143, 143);
					}
					else if(x==25 && y==12)
					{
						que.Insert(CM_DIALOG, 144, 144);
					}
					else if(x==25 && y==15)
					{
						que.Insert(CM_DIALOG, 145, 145);
					}
				}
			}
			//�级�� ����
			else if(map_id==10)
			{
				if(!battle)
				{
					//������
					if(x==20 && y==17)
					{
						shop[0]=13;
						shop[1]=14;
						shop[2]=15;
						shop[3]=-1;
						click_mode=90;
					}
					//������
					else if(x==30 && y==17)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].ammor>=0 && equipment[party.unit[i].ammor].pow>party.unit[i].dp)able_unit[i]=true;
								else able_unit[i]=false;

						click_mode=120;
					}
					//�б�
					else if(x==18 && y==3)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].life && party.unit[i].mizi_type>=0 && party.unit[i].mizi_level<4)able_unit[i]=true;
								else able_unit[i]=false;
							
						click_mode=100;
					}
					//�˼���
					else if(x==3 && y==26)
					{
						shop[0]=SOLDIER1;
						shop[1]=SOLDIER2;
						shop[2]=SOLDIER3;
						shop[3]=KUOT;
						click_mode=110;
					}
					//�ΰ�
					else if(x==11 && y==24)
					{
						que.Insert(CM_DIALOG, 146, 146);
					}
					else if(x==27 && y==32)
					{
						que.Insert(CM_DIALOG, 147, 147);
					}
					else if(x==28 && y==32)
					{
						que.Insert(CM_DIALOG, 148, 148);
					}
				}
			}

			//������
			if(id==0)
			{				
				if(x==0 || y==0 || x==(x_size-1) || y==(y_size-1))
				{
					if(battle)que.Insert(CM_QUESTION, 52, FUNC_WORLDMAP);
						else Function(FUNC_WORLDMAP);
				}
			}
		}

		if(escape)return;	//Ż���� ��� �˻� �� ��
	}
	////////////////////////////////////////
	//�ൿ ���� �� �̺�Ʈ
	else if(type==EV_ACTION_END)
	{
		EraseFog();

		if(map_id==10)		//�� ����
		{
			//�������� ������
			if(battle)
			{
				int tree=1;
				for(int j=0; j<y_size; j++)
					for(int i=0; i<x_size; i++)
						if(map[i][j][1]==tree)
						{
							int ran=rand()%4;
							if(ran==0)
							{
								if(i<x_size-1 && map[i+1][j][1]==-1 && unit_map[i+1][j]==-1)
								{
									map[i][j][1]=-1;
									map[i+1][j][1]=tree;
								}
							}
							else if(ran==1)
							{
								if(i>0 && map[i-1][j][1]==-1 && unit_map[i-1][j]==-1)
								{
									map[i][j][1]=-1;
									map[i-1][j][1]=tree;
								}
							}
							else if(ran==2)
							{
								if(j<y_size-1 && map[i][j+1][1]==-1 && unit_map[i][j+1]==-1)
								{
									map[i][j][1]=-1;
									map[i][j+1][1]=tree;
								}
							}
							else if(ran==3)
							{
								if(j>0 && map[i][j-1][1]==-1 && unit_map[i][j-1]==-1)
								{
									map[i][j][1]=-1;
									map[i][j-1][1]=tree;
								}
							}
						}
				MakeMoveMap();
			}
		}

		//������ ������
		for(int j=0; j<y_size; j++)
			for(int i=0; i<x_size; i++)
				if(map[i][j][2]==0)
				{
					int ran=rand()%4;
					if(ran==0)
					{
						if(i<x_size-1 && map[i+1][j][2]==-1)
						{
							map[i][j][2]=-1;
							map[i+1][j][2]=0;
						}
					}
					else if(ran==1)
					{
						if(i>0 && map[i-1][j][2]==-1)
						{
							map[i][j][2]=-1;
							map[i-1][j][2]=0;
						}
					}
					else if(ran==2)
					{
						if(j<y_size-1 && map[i][j+1][2]==-1)
						{
							map[i][j][2]=-1;
							map[i][j+1][2]=0;
						}
					}
					else if(ran==3)
					{
						if(j>0 && map[i][j-1][2]==-1)
						{
							map[i][j][2]=-1;
							map[i][j-1][2]=0;
						}
					}
				}
	}
	////////////////////////////////////////
	//�� ���� �� �̺�Ʈ
	else if(type==EV_END)
	{
		//������ ��Ƽ ������ ����
		if(battle)
		{
			for(int i=0; i<UNIT_MAX; i++)
			{
				if(unit[i].life && unit[i].team==0)
				{
					//���� ������ �ű��
					party.unit[unit[i].party_id].ammor=unit[i].ammor;
					party.unit[unit[i].party_id].dp=unit[i].dp;
				}
			}
		}
	}
}

void CGame::SaveNow()
{
	FILE* fp;

	char* title="HUGARU01SAVE";

	if(fp=fopen("hugaru0.sav","wb"))
	{
		fwrite(title, sizeof(char), 12, fp);
		fwrite(this, sizeof(CGame), 1, fp);
		fclose(fp);
	}
}

void CGame::Save(int file_no)
{
	FILE* fp;

	int file_size=0;
	//ũ�� �˾Ƴ���
	if(fp=fopen("hugaru0.sav","rb"))
	{
		char buffer;
		while(fread(&buffer, sizeof(char), 1, fp))file_size++;
		fclose(fp);
	}
	//������ �б�
	char* game_data=new char[file_size];
	if(fp=fopen("hugaru0.sav","rb"))
	{
		fread(game_data, sizeof(char), file_size, fp);
		fclose(fp);
	}
	//������ ����
	char file_name[20];
	sprintf(file_name, "hugaru%d.sav", file_no);
	if(fp=fopen(file_name,"wb"))
	{
		fwrite(game_data, sizeof(char), file_size, fp);
		fclose(fp);
	}
	delete [] game_data;

	sprintf(file_name, "party%d.sav", file_no);
	if(fp=fopen(file_name,"wb"))
	{
		fwrite(&party, sizeof(CParty), 1, fp);
		fclose(fp);
	}
}

bool CGame::Load(int file_no)
{
	FILE* fp;

	char file_name[20];
	sprintf(file_name, "hugaru%d.sav", file_no);
	if(fp=fopen(file_name,"rb"))
	{
		//���̺� ���� �ĺ�
		char title[13];
		fread(title, sizeof(char), 12, fp);
		title[12]=NULL;
		if(strcmp(title, "HUGARU01SAVE")==0)
		{
			//�ҷ����� ���� ����
			que.Destroy();

			//������ �о� ���̱�
			fread(this, sizeof(CGame), 1, fp);
			fclose(fp);

			//�ҷ��� �� ����
			que.Init();
			click_mode=1;

			sprintf(file_name, "party%d.sav", file_no);
			if(fp=fopen(file_name,"rb"))
			{
				fread(&party, sizeof(CParty), 1, fp);
				fclose(fp);
			}

			SelectBGM();
			return true;
		}
	}

	return false;
}

void CGame::LoadInfo()
{
	FILE* fp;

	char file_name[20];
	for(int i=0; i<3; i++)
	{
		sprintf(file_name, "hugaru%d.sav", i+1);
		if(fp=fopen(file_name,"rb"))
		{
			file_data[i].read_able=true;
			//���̺� ���� �ĺ�
			char title[13];
			fread(title, sizeof(char), 12, fp);
			title[12]=NULL;
			if(strcmp(title, "HUGARU01SAVE")==0)
			{
				fread(&file_data[i].map_id, sizeof(int), 1, fp);
				fclose(fp);
			}
			else file_data[i].read_able=false;
		}
		else file_data[i].read_able=false;
	}
}

void CGame::DrawMap(int MouseX, int MouseY, char d_tile, char r_tile)
{
	//AI�� ���콺 Ŀ��
	if(sw_ai)
	{
		MouseX=AI_MouseX;
		MouseY=AI_MouseY;
	}

	int fx=fx_i/40, fy=fy_i/40;
	int mx=(MouseX+fx_i)/40;
	int my=(MouseY+fy_i)/40;

	//1,2��
	for(int j=fy; j<Min(y_size, fy+13); j++)
		for(int i=fx; i<Min(x_size, fx+17); i++)
		{
			int tile_id1;
			int tile_x=i*40-fx_i;
			int tile_y=j*40-fy_i;

			//1�� ǥ��(�����̳� �̵��� ���)
			if(click_mode==1)	//�̵��� �̵��Ҵ� ������ ��Ӱ� ǥ��
			{				
				if(check_map[MOVE_TEST_MAP][i][j]>unit[unit_id].move)tile_id1=d_tile;
					else tile_id1=0;
			}
			else if(click_mode==4)	//���ݽ� ���� ������ ������ �Ӱ� ǥ��
			{
				if(check_map[ATTACK_TEST_MAP][i][j]!=99)tile_id1=r_tile;
					else tile_id1=0;
			}
			else tile_id1=0;

			if(fog_map[i][j])lib->Image(tile_x, tile_y, tile_id1, 7);
				else lib->Image(tile_x, tile_y, tile_id1, map[i][j][0]);
			//2��
			if(map[i][j][1]>=0 && !fog_map[i][j])lib->Image(tile_x, tile_y, 1, map[i][j][1]);
		}

	//��������Ʈ
	for(int i=0; i<UNIT_MAX; i++)
		if(unit[i].life)
		{
			//���� �Ȱ�
			if(fog_map[unit[i].x][unit[i].y])continue;
			//���� ǥ��
			if(click_mode==1 && unit_id==i)lib->Image(unit[i].x*40+unit[i].sx-fx_i, unit[i].y*40+unit[i].sy-fy_i, imgno.cursor+2);
			//���� ���
			int unit_img;
			if(unit[i].state==0 || unit[i].state>2)
			{
				if(unit[i].team==0)unit_img=imgno.unit;
					else unit_img=imgno.unit+1;
			}
			else	//�ߵ�
			{
				if(unit[i].team==0)unit_img=imgno.unit+2;
					else unit_img=imgno.unit+3;
			}
			lib->Image(unit[i].x*40+unit[i].sx-fx_i, unit[i].y*40+unit[i].sy-fy_i, unit_img, unit[i].caracter);
		}
		//ȥ
		else if(unit[i].soul)lib->Image(unit[i].x*40+unit[i].sx-fx_i, unit[i].y*40+unit[i].sy-fy_i, imgno.animation+2);

	//3��&�ִϸ��̼�
	for(int j=fy; j<Min(y_size, fy+13); j++)
		for(int i=fx; i<Min(x_size, fx+17); i++)
		{			
			int tile_x=i*40-fx_i;
			int tile_y=j*40-fy_i;

			//3�� Ÿ��
			if(map[i][j][2]>=0 && !fog_map[i][j])lib->Image(tile_x, tile_y, 1, map[i][j][2]+6);
			//�ִϸ��̼�
			if(check_map[ANIMATION_MAP][i][j]!=99)
			{
				if(!fog_map[i][j])
				{
					//��&��
					if(check_map[ANIMATION_MAP][i][j]==5 || check_map[ANIMATION_MAP][i][j]==7)
					{
						lib->Image(tile_x, tile_y, imgno.animation+check_map[ANIMATION_MAP][i][j], (20-check_map[DELAY_MAP][i][j])/4);
					}
					//���
					else if(check_map[ANIMATION_MAP][i][j]==1)
					{
						if(check_map[DELAY_MAP][i][j]>15)lib->Image(tile_x, tile_y, imgno.animation+check_map[ANIMATION_MAP][i][j], 20-check_map[DELAY_MAP][i][j]);
					}
					//��
					else if(check_map[ANIMATION_MAP][i][j]==8)
					{
						if(check_map[DELAY_MAP][i][j]>10)lib->Image(tile_x, tile_y, imgno.animation+check_map[ANIMATION_MAP][i][j], 20-check_map[DELAY_MAP][i][j]);
							else lib->Image(tile_x, tile_y, imgno.animation+check_map[ANIMATION_MAP][i][j], 10);
					}
					//�ڵ� �ִϸ��̼�
					else
					{
						lib->Image(tile_x, tile_y, imgno.animation+check_map[ANIMATION_MAP][i][j]);
					}
				}
			}
			//���� ���� ǥ��
			if(click_mode==4 && check_map[ATTACK_TEST_MAP][mx][my]!=99 && check_map[DAMAGE_TEST_MAP][i][j]==0 && first_check)lib->Image(tile_x, tile_y, imgno.cursor+2);
		}
	//�ִϸ��̼� ���� �ð� ����
	for(int ay=0; ay<y_size; ay++)
		for(int ax=0; ax<x_size; ax++)
			if(check_map[DELAY_MAP][ax][ay]>0)
			{
				check_map[DELAY_MAP][ax][ay]--;
				if(check_map[DELAY_MAP][ax][ay]<=0)check_map[ANIMATION_MAP][ax][ay]=99;
			}

	//��������
	if((click_mode==1 || click_mode==3) && !sw_move)
	{		
		int px=(mx)*40-fx_i;
		int py=(my)*40-fy_i;
		if(!fog_map[mx][my])lib->Image(px, py, imgno.cursor+1);
	}
	//���������� ���ݹ���
	else if(click_mode==4)
	{
		SetDamageArea(mx, my);
	}

	//����
	if(battle)
	{
		int px=(MouseX+fx_i)/40;
		int py=(MouseY+fy_i)/40;
		if(!fog_map[px][py] && unit_map[px][py]>=0)
		{
			//����â
			CUnit* unitp=&unit[unit_map[px][py]];
			int state_x, state_y;
			if(MouseX>560)state_x=MouseX-65;
				else state_x=MouseX+15;
			if(MouseY>440)state_y=MouseY-45;
				else state_y=MouseY;
			lib->Image(state_x-2, state_y-2, imgno.panel+1);
			//ü��
			lib->Image(state_x, state_y, imgno.state_icon, 0);
			n_hp.PutNumber(state_x+12, state_y+2, unitp->hp);
			//����
			if(unitp->ammor>=0)
			{
				lib->Image(state_x+30, state_y, imgno.state_icon, 1);
				n_ammor.PutNumber(state_x+42, state_y+2, unitp->dp);
			}
			//����
			lib->Image(state_x, state_y+10, imgno.state_icon, 3+equipment[unitp->arms].type);
			n_arms.PutNumber(state_x+12, state_y+12, unitp->str);
			//����
			if(unitp->mizi_type>=0)
			{
				lib->Image(state_x+30, state_y+10, imgno.state_icon, 7+unitp->mizi_type);
				n_mizi.PutNumber(state_x+42, state_y+12, unitp->mizi_level);				
			}
			//�̵�
			lib->Image(state_x, state_y+20, imgno.state_icon, 14);
			n_move.PutNumber(state_x+12, state_y+22, unitp->move);
			//����
			if(unitp->state!=0)
			{
				if(unitp->state==1 || unitp->state==2)lib->Image(state_x+30, state_y+20, imgno.state_icon, 13);
					else if(unitp->state==3 || unitp->state==4)lib->Image(state_x+40, state_y+30, imgno.state_icon, 12);
			}
			//���
			lib->Image(state_x, state_y+30, imgno.state_icon, 15);
			n_wait.PutNumber(state_x+12, state_y+32, unitp->wait);
			//�Ҽ�
			lib->Image(state_x+50, state_y+30, imgno.state_icon, 16+(int)unitp->team);
		}
	}

	//�ڼ��� ����
	if(click_mode==-1)
	{
		if(state_id>=0)
		{
			CUnit* unit_p;
			if(battle)unit_p=&unit[state_id];
				else unit_p=&party.unit[state_id];
			//���� ���
			lib->Image(200, 200, imgno.panel+3);
			if(unit_p->team==0)lib->Image(220, 220, imgno.unit, unit_p->caracter);
				else lib->Image(220, 220, imgno.unit+1, unit_p->caracter);
			//����
			lib->Image(280, 160, imgno.panel+2);
			char info_text[512], tmp_text[128];
			RECT info_rect;
			SetRect(&info_rect, 282, 162, 438, 318);
			//�̸�, ü��, ����
			sprintf(info_text, "%s\nü��: %d/%d\n����: %s(��: %d)", cara[unit_p->caracter].name, unit_p->hp, unit_p->hp_max, equipment[unit_p->arms].name, unit_p->str);
			//����
			if(unit_p->ammor==-1)strcat(info_text, "\n����: ����");
			else
			{
				sprintf(tmp_text, "\n����: %s(%d/%d)", equipment[unit_p->ammor].name, unit_p->dp, equipment[unit_p->ammor].pow);
				strcat(info_text, tmp_text);
			}
			//����
			if(unit_p->mizi_type==-1)strcat(info_text, "\n����: ��� �� ��");
			else
			{
				sprintf(tmp_text, "\n����: %s(����: %d)", mizi_name[unit_p->mizi_type], unit_p->mizi_level);
				strcat(info_text, tmp_text);
			}
			//�̵���, ����, ���, ������
			char* state_name[]={"����","��","�͵�","�ູ","���� �ູ"};
			sprintf(tmp_text, "\n�̵���: %d\n����: %s\n���: %d\n������: ",unit_p->move, state_name[unit_p->state], unit_p->wait);
			strcat(info_text, tmp_text);
			if(unit_p->team==0)sprintf(tmp_text, "%d", party.money);
				else sprintf(tmp_text, "%d", have_money[unit_p->caracter]);
			strcat(info_text,tmp_text);
			//���
			lib->PutText(&info_rect, info_text, JColor(0,0,0));
		}
	}
	//�����
	else if(click_mode==3)
	{
		if(!fog_map[unit[unit_id].x][unit[unit_id].y] && !question)
		{
			panel_x=(unit[unit_id].x+1)*40-fx_i;
			panel_y=unit[unit_id].y*40-fy_i;
			if(unit[unit_id].x>=(x_size-2))panel_x-=100;
			if(unit[unit_id].y>=(y_size-1))panel_y-=40;

			lib->Image(panel_x, panel_y, imgno.panel);
			char* str_command[]={"����","����","����","�ٸ�","ġ��","���"};
			//������ ��� ǥ��
			selected=-1;
			for(int i=0; i<4; i++)
				if(cara[unit[unit_id].caracter].command[i]>=0)
				{
					if(MouseX>=panel_x && MouseX<panel_x+60 && MouseY>=panel_y+(i*20) && MouseY<panel_y+(i*20)+20)
					{
						lib->PutText(panel_x+2, panel_y+2+i*20, str_command[cara[unit[unit_id].caracter].command[i]], YELLOW);
						selected=cara[unit[unit_id].caracter].command[i];
					}
					else
						lib->PutText(panel_x+2, panel_y+2+i*20, str_command[cara[unit[unit_id].caracter].command[i]], JColor(255,255,255));
				}
		}
	}
	//�������
	else if(click_mode==5)
	{
		panel_x=240;
		panel_y=160;

		lib->Image(panel_x, panel_y, imgno.panel+2);
		char* mizi_list[]={"��ǳ","�߹ٶ�","�ҵ���","�ҹٴ�","ġ��","��","����","��ȭ","����","�ູ","��","����","�͵���"};
		//������ ��� ǥ��
		selected=selected2=-1;
		for(int i=0; i<4; i++)
			if(unit[unit_id].skill[i]>=0)
			{
				//���� �̸�
				if(MouseX>=panel_x && MouseX<panel_x+160 && MouseY>=panel_y+(i*20) && MouseY<panel_y+(i*20)+20)
				{
					lib->PutText(panel_x+2, panel_y+2+i*20, mizi_list[unit[unit_id].skill[i]], YELLOW);
					selected=unit[unit_id].skill[i];
				}
				else
					lib->PutText(panel_x+2, panel_y+2+i*20, mizi_list[unit[unit_id].skill[i]], CUSTOM_BLUE);
				//���� ����
				for(int j=0; j<unit[unit_id].mizi_level; j++)
				{
					char lv_buffer[2];
					itoa(j+1, lv_buffer, 10);
					if(MouseX>=panel_x+80+(j*20) && MouseX<panel_x+100+(j*20) && MouseY>=panel_y+(i*20) && MouseY<panel_y+(i*20)+20)
					{
						lib->PutText(panel_x+80+(j*20), panel_y+2+i*20, lv_buffer, YELLOW);
						selected2=j;
					}
					else
						lib->PutText(panel_x+80+(j*20), panel_y+2+i*20, lv_buffer, CUSTOM_BLUE);
				}
			}
		//���� ����
		if(selected>=0)
		{
			char* mizi_help_text[]=
			{
				"����� �Ȱ��� ����",
				"�̵����� ����",
				"�Ѹ��� �ҷ� ����",
				"�������� �ҷ� ����",
				"ü���� ȸ��",
				"���� ���� �ٲ�",
				"��������  ����",
				"���� ���·� ����\n(�ູ�� ����)",
				"�������� ���� ����\n�������� ����\n���� ����",
				"ü���� ���ݾ� ȸ��",
				"������ ���� ����",
				"��븦 �ߵ� ��Ŵ",
				"�͵��� �ߵ� ��Ŵ\n������� �̸��� ��"
			};
			RECT mizi_help_rect;
			SetRect(&mizi_help_rect, 242, 260, 398, 318);
			lib->PutText(&mizi_help_rect, mizi_help_text[selected], JColor(0,0,0));
		}
	}
	//���
	else if(click_mode==80)
	{
		panel_x=160;
		panel_y=120;

		lib->Image(445, 35, imgno.panel+7);
		lib->Image(33, 111, imgno.panel+6);
		lib->Image(33, 195, imgno.panel+6);

		//������ ĳ����
		for(int j=0; j<PARTY_MAX; j++)
		{
			if(party.unit[j].life)
			{
				int able;
				if(party.unit[j].caracter>=10 || party.unit[j].caracter==HERO4 || party.unit[j].caracter==SOLDIER2 || party.unit[j].caracter==SOLDIER3)able=4;
					else able=0;

				int cx, cy;
				cx=35+(j%10)*40;
				cy=113+(j/10)*40;
				lib->Image(cx, cy, imgno.unit+able, party.unit[j].caracter);				
				if(selected==j)lib->Image(cx, cy, imgno.cursor+2);
			}
		}
		//���� ���
		int ty=37;
		selected2=-1;
		for(int i=0; i<EQUIP_MAX; i++)
		{
			if(party.item[i]>0)
			{				
				char item_number[10];
				sprintf(item_number, "%d", party.item[i]);				
				if(MouseX>445 && MouseX<=605 && MouseY>ty && MouseY<=ty+20)
				{
					lib->PutText(447, ty, equipment[i].name, YELLOW);
					lib->PutText(565, ty, item_number, YELLOW);					
					selected2=i;
				}
				else
				{
					lib->PutText(447, ty, equipment[i].name, CUSTOM_BLUE);
					lib->PutText(565, ty, item_number, CUSTOM_BLUE);					
				}
				ty+=20;
			}
		}
		//���� ����
		if(selected>=0)
		{
			char help_text[256];
			if(party.unit[selected].ammor==-1)sprintf(help_text, "����: %s\n����: ����\n���ݷ�: %d", equipment[party.unit[selected].arms].name, party.unit[selected].str);
			else
			{
				//�ջ�� ������ �԰�� ���ü �Ұ���
				if(party.unit[selected].dp<equipment[party.unit[selected].ammor].pow) sprintf(help_text, "����: %s\n����: %s(�ջ�� �����̶� ��� ��ü �Ұ���)\n���ݷ�: %d\n����: %d/%d", equipment[party.unit[selected].arms].name, equipment[party.unit[selected].ammor].name, party.unit[selected].str, party.unit[selected].dp, equipment[party.unit[selected].ammor].pow);
					else sprintf(help_text, "����: %s\n����: %s\n���ݷ�: %d\n����: %d/%d", equipment[party.unit[selected].arms].name, equipment[party.unit[selected].ammor].name, party.unit[selected].str, party.unit[selected].dp, equipment[party.unit[selected].ammor].pow);
			}

			RECT help_rect;
			SetRect(&help_rect, 35, 197, 435, 273);
			lib->PutText(&help_rect, help_text, JColor(0,0,0));
		}
	}
	//��Ƽ ����
	else if(click_mode==81)
	{
		panel_x=160;
		panel_y=120;

		lib->Image(445, 35, imgno.panel+7);
		lib->Image(33, 111, imgno.panel+6);
		lib->Image(33, 195, imgno.panel+6);

		//��� ĳ����
		selected=-1;
		for(int j=0; j<PARTY_MAX; j++)
		{
			if(party.unit[j].life)
			{
				int cx, cy;
				cx=35+(j%10)*40;
				cy=113+(j/10)*40;
				if(able_unit[j])
				{
					lib->Image(cx, cy, imgno.unit, party.unit[j].caracter);
					if(MouseX>=cx && MouseX<cx+40 && MouseY>=cy && MouseY<cy+40)
					{
						lib->Image(cx, cy, imgno.cursor+2);
						selected=j;
					}
				}
				else lib->Image(cx, cy, imgno.unit+4, party.unit[j].caracter);

				//�����ٲٱ� ���
				if(selected2==0 &&selected3==j)lib->Image(cx, cy, imgno.cursor+2);
			}
		}
		//��ɵ�
		int ty=37;
		char* party_command[3]={"���� �ٲٱ�", "�ذ�", "��� ��ü"};
		for(int i=0; i<3; i++)
		{
			if(selected2==i)
			{
				lib->PutText(447, ty, party_command[i], YELLOW);
			}
			else
			{
				lib->PutText(447, ty, party_command[i], CUSTOM_BLUE);
			}
			ty+=20;
		}
		//���� ����
		if(selected>=0)
		{
			char help_text[256];
			if(party.unit[selected].ammor==-1)sprintf(help_text, "����: %s\n����: ����\n���ݷ�: %d", equipment[party.unit[selected].arms].name, party.unit[selected].str);
				else sprintf(help_text, "����: %s\n����: %s\n���ݷ�: %d\n����: %d/%d", equipment[party.unit[selected].arms].name, equipment[party.unit[selected].ammor].name, party.unit[selected].str, party.unit[selected].dp, equipment[party.unit[selected].ammor].pow);

			RECT help_rect;
			SetRect(&help_rect, 35, 197, 435, 273);
			lib->PutText(&help_rect, help_text, JColor(0,0,0));
		}
	}
	//��ǰ ���
	else if(click_mode==90)
	{
		panel_x=160;
		panel_y=120;

		lib->Image(panel_x, panel_y, imgno.panel+2);
		lib->Image(325, 35, imgno.panel+7);
		lib->Image(118, 358, imgno.panel+6);

		//���ڵ�
		selected=-1;
		for(int i=0; i<4; i++)
			if(shop[i]>=0)
			{
				//�̸��� ���� �˾Ƴ���
				char price[10];
				sprintf(price, "%dī��", equipment[shop[i]].price);
				JColor goods_color;				
				if(MouseX>=panel_x && MouseX<panel_x+160 && MouseY>=panel_y+(i*20) && MouseY<panel_y+(i*20)+20)
				{
					goods_color=YELLOW;
					selected=shop[i];
				}
				else
				{
					goods_color=CUSTOM_BLUE;
				}
				//��ǰ �̸�
				lib->PutText(panel_x+2, panel_y+2+i*20, equipment[shop[i]].name, goods_color);
				lib->PutText(panel_x+80, panel_y+2+i*20, price, goods_color);
			}
		//���� ���
		int ty=37;
		for(int i=0; i<EQUIP_MAX; i++)
		{
			if(party.item[i]>0)
			{
				lib->PutText(327, ty, equipment[i].name, CUSTOM_BLUE);
				char item_number[10];
				sprintf(item_number, "%d", party.item[i]);
				lib->PutText(445, ty, item_number, CUSTOM_BLUE);
				ty+=20;
			}
		}
		//����
		if(selected>=0)
		{
			char* help_text[]=
			{
				"",
				"���ݷ�+1",
				"���ݷ�+2",
				"���ݷ�+1\n���� 3ĭ ����",
				"���ݷ�+1\n���� 8ĭ ����",
				"���ݷ�+1\n������ 2ĭ ����",
				"���ݷ�+1\n������ 3ĭ ����",
				"���ݷ�+1\n�Ÿ� 2",
				"���ݷ�+2\n�Ÿ� 2",
				"���ݷ�+1\n�Ÿ� 2-3",
				"���ݷ�+0\n�Ÿ� 1-2",
				"���ݷ�+1\n�� ȿ��",
				"���ݷ�+0\n���� �ߵ���Ŵ",
				"������ 5\n����ȿ��",
				"������ 10",
				"������ 20",
				"������ 99\n����ȿ��",
			};

			RECT help_rect;
			SetRect(&help_rect, panel_x+2, panel_y+100, panel_x+158, panel_y+140);
			lib->PutText(&help_rect, help_text[selected], JColor(0,0,0));
		}
		//������
		char my_money[30];
		sprintf(my_money, "������: %dī��", party.money);
		lib->PutText(panel_x+2, panel_y+140, my_money, CUSTOM_BLUE);
		//��Ƽ�鿡�� ȿ��
		if(selected>=0)
		{
			//���� ������ ĳ����
			for(int j=0; j<PARTY_MAX; j++)
			{
				if(party.unit[j].life)
				{
					int able;
					if(party.unit[j].caracter>=10 || party.unit[j].caracter==HERO4 || party.unit[j].caracter==SOLDIER2 || party.unit[j].caracter==SOLDIER3)able=4;
						else able=0;

					int cx, cy;
					cx=120+(j%10)*40;
					cy=360+(j/10)*40;
					lib->Image(cx, cy, imgno.unit+able, party.unit[j].caracter);
				}
			}
		}
	}
	//�б�
	else if(click_mode==100)
	{
		int mizi_price[]={0, 200, 500, 1000};

		lib->Image(118, 116, imgno.panel+6);
		lib->Image(118, 200, imgno.panel+6);
		selected=-1;
		for(int i=0; i<PARTY_MAX; i++)
		{
			if(party.unit[i].life)
			{
				int px, py;
				px=120+(i%10)*40;
				py=118+(i/10)*40;
				if(able_unit[i])lib->Image(px, py, imgno.unit, party.unit[i].caracter);
					else lib->Image(px, py, imgno.unit+4, party.unit[i].caracter);
				if(able_unit[i] && MouseX>px && MouseX<=px+40 && MouseY>py && MouseY<=py+40)
				{
					lib->Image(px, py, imgno.cursor+2);
					selected=i;
					select_price=mizi_price[party.unit[selected].mizi_level];
				}
			}
			else break;
		}

		//����
		RECT help_rect;
		SetRect(&help_rect, 120, 202, 520, 282);
		if(selected>=0)
		{
			char help_text[128];

			sprintf(help_text, "���� �Ӽ�: %s / ���� ����: %d\n���� ������ �Ǳ� ���� �ʿ��� �н���: %d\n������: %dī��", mizi_name[party.unit[selected].mizi_type], party.unit[selected].mizi_level, mizi_price[party.unit[selected].mizi_level], party.money);
			lib->PutText(&help_rect, help_text, JColor(0, 0, 0));
		}
		else lib->PutText(&help_rect, "���� ����� ���� ĳ���͸� �������ּ���.", JColor(0, 0, 0));
	}
	//�˼���
	else if(click_mode==110)
	{
		panel_x=240;
		panel_y=160;

		lib->Image(panel_x, panel_y, imgno.panel+2);
		selected=-1;
		for(int i=0; i<4; i++)
			if(shop[i]>=0)
			{
				//�̸��� ����
				int pay[]={0,200,200,200,200,300,100,80,80,120,50,100,80,60,120,300,100};
				char price[10];
				sprintf(price, "%dī��", pay[shop[i]]);
				JColor goods_color;				
				if(MouseX>=panel_x && MouseX<panel_x+160 && MouseY>=panel_y+(i*20) && MouseY<panel_y+(i*20)+20)
				{
					goods_color=YELLOW;
					selected=shop[i];
					select_price=pay[shop[i]];
				}
				else
				{
					goods_color=CUSTOM_BLUE;
				}

				lib->PutText(panel_x+2, panel_y+2+i*20, cara[shop[i]].name, goods_color);
				lib->PutText(panel_x+80, panel_y+2+i*20, price, goods_color);
			}
		//���� ����
		lib->Image(118, 348, imgno.panel+6);
		for(int i=0; i<PARTY_MAX; i++)
		{
			if(party.unit[i].life)
			{
				int px, py;
				px=120+(i%10)*40;
				py=350+(i/10)*40;
				lib->Image(px, py, imgno.unit, party.unit[i].caracter);
			}
			else break;
		}
		//������
		char my_money[30];
		sprintf(my_money, "������: %dī��", party.money);
		lib->PutText(242, 300, my_money, CUSTOM_BLUE);
	}
	//��Ƽ���� ����
	else if(click_mode==111)
	{
		lib->Image(118, 116, imgno.panel+6);
		lib->Image(118, 200, imgno.panel+6);
		selected=-1;
		for(int i=0; i<PARTY_MAX; i++)
		{
			if(party.unit[i].life)
			{
				int px, py;
				px=120+(i%10)*40;
				py=118+(i/10)*40;
				if(able_unit[i])lib->Image(px, py, imgno.unit, party.unit[i].caracter);
					else lib->Image(px, py, imgno.unit+4, party.unit[i].caracter);
				if(able_unit[i] && MouseX>px && MouseX<=px+40 && MouseY>py && MouseY<=py+40)
				{
					lib->Image(px, py, imgno.cursor+2);
					selected=i;
				}
			}
		}

		//����
		char help_text[256];
		if(selected>=0)
		{
			if(party.unit[selected].ammor==-1)sprintf(help_text, "����: %s\n����: ����\n���ݷ�: %d", equipment[party.unit[selected].arms].name, party.unit[selected].str);
				else sprintf(help_text, "����: %s\n����: %s\n���ݷ�: %d\n����: %d/%d", equipment[party.unit[selected].arms].name, equipment[party.unit[selected].ammor].name, party.unit[selected].str, party.unit[selected].dp, equipment[party.unit[selected].ammor].pow);
		}
		else strcpy(help_text, "��Ƽ���� �� ������ ��� �ֽʽÿ�.");

		RECT help_rect;
		SetRect(&help_rect, 120, 202, 516, 258);
		lib->PutText(&help_rect, help_text, JColor(0,0,0));
	}
	//������
	else if(click_mode==120)
	{
		lib->Image(118, 116, imgno.panel+6);
		lib->Image(118, 200, imgno.panel+6);
		selected=-1;
		for(int i=0; i<PARTY_MAX; i++)
		{
			if(party.unit[i].life)
			{
				int px, py;
				px=120+(i%10)*40;
				py=118+(i/10)*40;
				if(able_unit[i])lib->Image(px, py, imgno.unit, party.unit[i].caracter);
					else lib->Image(px, py, imgno.unit+4, party.unit[i].caracter);
				if(able_unit[i] && MouseX>px && MouseX<=px+40 && MouseY>py && MouseY<=py+40)
				{
					lib->Image(px, py, imgno.cursor+2);
					selected=i;
					select_price=equipment[party.unit[i].ammor].pow-party.unit[i].dp;
				}
			}
			else break;
		}

		//����
		RECT help_rect;
		SetRect(&help_rect, 120, 202, 520, 282);
		if(selected>=0)
		{
			char help_text[128];

			sprintf(help_text, "%s: %d/%d\n������: %d\n������: %dī��", equipment[party.unit[selected].ammor].name, party.unit[selected].dp, equipment[party.unit[selected].ammor].pow, select_price, party.money);
			lib->PutText(&help_rect, help_text, JColor(0, 0, 0));
		}
		else lib->PutText(&help_rect, "������ ������ ĳ���͸� �������ּ���.", JColor(0, 0, 0));
	}

	//���� ���� �˷��ִ� �׸�
	if(sw_ai)lib->Image(0, 0, imgno.panel+4);

	//��ȭâ
	if(dialog || question)
	{
		//������ ���� ���� ǥ��
		if(question)
		{
			selected=-1;
			//����
			if(MouseX>260 && MouseX<=310 && MouseY>175 && MouseY<=225)
			{
				lib->Image(260, 175, imgno.cursor+3, 0);
				selected=0;
			}
			else lib->Image(260, 175, imgno.cursor+3, 1);
			//����
			if(MouseX>330 && MouseX<=380 && MouseY>175 && MouseY<=225)
			{
				lib->Image(330, 175, imgno.cursor+3, 2);
				selected=1;
			}
			else lib->Image(330, 175, imgno.cursor+3, 3);
		}

		//��, ���� ǥ��
		lib->Image(0, 400, imgno.panel+3);
		int dialog_face=atoi(dialog_text[dialog_no*2]);
		if(dialog_face>=16)lib->Image(20, 420, imgno.unit+1, dialog_face-16);
			else if(dialog_face>=0)lib->Image(20, 420, imgno.unit, dialog_face);
		lib->Image(80, 400, imgno.panel+5);
		RECT dialog_rect;
		SetRect(&dialog_rect, 85, 408, 635, 480);
		lib->PutText(&dialog_rect, dialog_text[dialog_no*2+1], JColor(0,0,0));
	}

	//Ŀ��
	if(!fog_map[unit[unit_id].x][unit[unit_id].y])lib->Image(MouseX, MouseY, imgno.cursor);

	//���� �޴��� ���̺�&�ε�
	if(map_id==13)
	{
		if(click_mode<10)
		{
			if(unit[0].life)lib->Image(150, 10, "Ÿ��Ʋ");

			lib->PutText(275, 140, "�� ����", JColor(255,255,255));
			if(!main_menu)	//�߰� �޴��� ��
			{
				lib->PutText(120, 290, "����", JColor(255,255,255));
				lib->PutText(325, 325, "���ư���", JColor(255,255,255));				
				if(!pre_battle)
				{
					lib->PutText(200, 400, "���", JColor(255,255,255));
					lib->PutText(410, 370, "��Ƽ����", JColor(255,255,255));
				}
			}
			lib->PutText(450, 290, "�ҷ�����", JColor(255,255,255));
			lib->PutText(275, 450, "������", JColor(255,255,255));
		}
	}
	else if(map_id==14)
	{
		char* map_name[]={"��������", "�ް��� ����", "�ʿ�", "�縷", "������", "��", "�ٸ�", "���̾� ����",
			"�̹��� ����", "��ũ�� ����", "�级�� ����",  "��ũ���� �� 1��", "��ũ���� �� 2��"};
		//������ ȿ��
		for(int i=0; i<3; i++)
		{
			if(file_data[i].read_able)
			{
				lib->PutText(290, 170+(i*80), map_name[file_data[i].map_id], JColor(255,255,255));				
				check_map[ANIMATION_MAP][6][4+(i*2)]=3;
				check_map[DELAY_MAP][6][4+(i*2)]=1;
			}
		}
	}
}

void CGame::Focus(int x, int y, WORD time)
{
	if(fog_map[x][y])return;	//���� �Ȱ��� �ִ� �κ��� ��Ŀ���� �ȵ�
	x=MaxMin(x*40-300, 0, (x_size*40)-640);
	y=MaxMin(y*40-220,0,(y_size*40)-480);
	que.Insert(CM_FOCUS, 0, x, y, time);
}

void CGame::SetStartPoint(int id, int x, int y)
{
	map_id=id;
	world_x=x;
	world_y=y;
}

void CGame::MakeMoveMap()
{
	//�ʿ� �ҿ�Ǵ� �̵��� ���
	for(int j=0; j<y_size; j++)
		for(int i=0; i<x_size; i++)
		{
			unsigned char move_dec_1F[]={1,1,1,2,1,1,99,99};
			unsigned char move_dec_2F[]={98,98,98,98,1,1};
			if(map[i][j][1]>=0)move_map[i][j]=move_dec_2F[map[i][j][1]];
				else move_map[i][j]=move_dec_1F[map[i][j][0]];
		}
}

void CGame::Damage(int x, int y, int value, int type)
{
	//�ִϸ��̼�
	check_map[ANIMATION_MAP][x][y]=type;
	check_map[DELAY_MAP][x][y]=20;

	//�ҷ� ����&������ �¿��
	if(type==0 &&(map[x][y][1]==1 || map[x][y][1]==2))
	{
		map[x][y][1]=-1;
		MakeMoveMap();
	}

	if(unit_map[x][y]>=0)
	{
		int damage_unit_id=unit_map[x][y];
		//������ �Ծ��� ���
		if(unit[damage_unit_id].dp>0)
		{
			//���� ������
			if(type==6)
			{
				//ö ������ �Ծ��� ���� ������
				if(unit[damage_unit_id].ammor==14 || unit[damage_unit_id].ammor==15)
				{
					unit[damage_unit_id].hp=MaxMin(unit[damage_unit_id].hp-value, 0, unit[damage_unit_id].hp_max);					
				}
				else value=0;
			}
			//�� ������
			else if(type==7)
			{
				unit[damage_unit_id].hp=MaxMin(unit[damage_unit_id].hp-value, 0, unit[damage_unit_id].hp_max);
			}
			else
			{
				unit[damage_unit_id].dp-=value;
				//���� �ı�
				if(unit[damage_unit_id].dp<=0)
				{
					unit[damage_unit_id].hp=MaxMin(unit[damage_unit_id].hp+unit[damage_unit_id].dp, 0, unit[damage_unit_id].hp_max);
					value=-unit[damage_unit_id].dp;
					unit[damage_unit_id].dp=0;
					unit[damage_unit_id].ammor=-1;
				}
				else
				{
					value=0;
				}
			}
		}
		//�������� ������
		else
		{
			unit[damage_unit_id].hp=MaxMin(unit[damage_unit_id].hp-value, 0, unit[damage_unit_id].hp_max);
		}
		
		que.Insert(CM_DAMAGE, damage_unit_id, value);
	}
}

void CGame::UnitMove(int id, int x, int y)
{
	click_mode=2;
	SearchRoad(MOVE_BACK_MAP, x, y, unit[id].move_type, unit[id].team);	//�������� ���� ���� ����

	//�ӽ÷� �� X,Y�� TIME
	int tx, ty;
	tx=unit[id].x;
	ty=unit[id].y;
	int tt=que.que_time;
	
	Focus(tx, ty);
	//�̵��ϴ� ĭ�� �¿���Ϸ� ť�� ������� ���� ����
	while(check_map[MOVE_BACK_MAP][tx][ty]!=0)
	{								
		unsigned char min=check_map[MOVE_BACK_MAP][tx][ty];
		int dir=999;
		//�·� �̵� ���ɼ� �˻�
		if(tx>0 && check_map[MOVE_BACK_MAP][tx-1][ty]<min)
		{
			dir=4;
			min=check_map[MOVE_BACK_MAP][tx-1][ty];
		}
		//��� �̵� ���ɼ� �˻�
		if(tx<x_size-1 && check_map[MOVE_BACK_MAP][tx+1][ty]<min)
		{
			dir=6;
			min=check_map[MOVE_BACK_MAP][tx+1][ty];
		}
		//���� �̵� ���ɼ� �˻�
		if(ty>0 && check_map[MOVE_BACK_MAP][tx][ty-1]<min)
		{
			dir=8;
			min=check_map[MOVE_BACK_MAP][tx][ty-1];
		}
		//�Ʒ��� �̵� ���ɼ� �˻�
		if(ty<y_size-1 && check_map[MOVE_BACK_MAP][tx][ty+1]<min)
		{
			dir=2;
			min=check_map[MOVE_BACK_MAP][tx][ty+1];
		}

		//�ӽ� ��ǥ�� �̵� ��Ŵ
		if(dir==4)tx--;
			else if(dir==6)tx++;
			else if(dir==8)ty--;
			else if(dir==2)ty++;
			//����
			else
			{
				click_mode=-1;
				break;
			}

		que.Insert(CM_MOVE, id, dir, 0, tt);
		tt++;								
	}
	que.Insert(CM_MOVEEND, id, unit[id].x, unit[id].y, tt);
	Focus(tx, ty, tt);
}

void CGame::EndTurn()
{
	if(unit[unit_id].wait==100)unit[unit_id].wait=75;
	Event(EV_ACTION_END, unit_id, unit[unit_id].x, unit[unit_id].y);
	click_mode=0;
}

void CGame::SetAttackArea()
{
	first_check=false;
	InitCheckMap(ATTACK_TEST_MAP);
	att_code=unit[unit_id].arms;
	//ȸ�� ��
	if(att_code==4)SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0);
	//2ĭ �Ÿ� Ȱ
	else if(att_code==7 || att_code==8)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 2, 2, 0);
	//�氥 �ֵ���
	else if(att_code==10)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
	//3ĭ �Ÿ� Ȱ
	else if(att_code==9)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 2, 3, 0);
	//�Ϲ� ��
	else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);

	click_mode=4;
}

void CGame::SetDamageArea(int x, int y)
{
	if(check_map[ATTACK_TEST_MAP][x][y]!=99 && (prev_x!=x || prev_y!=y || !first_check))
	{
		//////////////////////////////////////////////
		//���� ����
		if(att_code<1000)
		{
			if(att_code==3)	//���
			{
				//�¿�
				if(x!=unit[unit_id].x)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y-1, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x, y+1, 0);
				}
				//����
				else
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x-1, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x+1, y, 0);
				}
			}
			else if(att_code==4)	//ȸ����
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				SetSquareArea(DAMAGE_TEST_MAP, x, y, 1, 1, 0);
			}
			else if(att_code==5 || att_code==6)	//â
			{
				//��
				if(x<unit[unit_id].x)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x-1, y, 0);
					if(att_code==6)SetCheckMap(DAMAGE_TEST_MAP, x-2, y, 0);
				}
				//��
				else if(x>unit[unit_id].x)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x+1, y, 0);
					if(att_code==6)SetCheckMap(DAMAGE_TEST_MAP, x+2, y, 0);
				}
				//��
				else if(y<unit[unit_id].y)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x, y-1, 0);
					if(att_code==6)SetCheckMap(DAMAGE_TEST_MAP, x, y-2, 0);
				}
				//��
				else if(y>unit[unit_id].y)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x, y+1, 0);
					if(att_code==6)SetCheckMap(DAMAGE_TEST_MAP, x, y+2, 0);
				}
			}
			//��Ÿ 1ĭ ¥��
			else
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
			}
		}
		//////////////////////////////////////////////
		//����
		else if(att_code<2000)
		{				
			int mizi_code=(att_code-1000)/10;
			int lv_code=att_code%10;
			//��ǳ
			if(mizi_code==0)
			{
				//��
				if(x<unit[unit_id].x)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					for(int i=0; i<unit[unit_id].x; i++)
					{
						SetCheckMap(DAMAGE_TEST_MAP, i, y, 0);
						if(lv_code>0)
						{
							SetCheckMap(DAMAGE_TEST_MAP, i, y+1, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-1, 0);
						}
						if(lv_code>1)
						{
							SetCheckMap(DAMAGE_TEST_MAP, i, y+2, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-2, 0);
						}
						if(lv_code>2)
						{
							SetCheckMap(DAMAGE_TEST_MAP, i, y+3, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-3, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y+4, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-4, 0);
						}
					}
					att_dir=4;
				}
				//��
				else if(x>unit[unit_id].x)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					for(int i=x; i<x_size; i++)
					{
						SetCheckMap(DAMAGE_TEST_MAP, i, y, 0);
						if(lv_code>0)
						{
							SetCheckMap(DAMAGE_TEST_MAP, i, y+1, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-1, 0);
						}
						if(lv_code>1)
						{
							SetCheckMap(DAMAGE_TEST_MAP, i, y+2, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-2, 0);
						}
						if(lv_code>2)
						{
							SetCheckMap(DAMAGE_TEST_MAP, i, y+3, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-3, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y+4, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-4, 0);
						}
					}
					att_dir=6;
				}
				//��
				else if(y<unit[unit_id].y)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					for(int i=0; i<unit[unit_id].y; i++)
					{
						SetCheckMap(DAMAGE_TEST_MAP, x, i, 0);
						if(lv_code>0)
						{
							SetCheckMap(DAMAGE_TEST_MAP, x+1, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-1, i, 0);
						}
						if(lv_code>1)
						{
							SetCheckMap(DAMAGE_TEST_MAP, x+2, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-2, i, 0);
						}
						if(lv_code>2)
						{
							SetCheckMap(DAMAGE_TEST_MAP, x+3, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-3, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x+4, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-4, i, 0);
						}
					}
					att_dir=8;
				}
				//��
				else if(y>unit[unit_id].y)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					for(int i=y; i<y_size; i++)
					{
						SetCheckMap(DAMAGE_TEST_MAP, x, i, 0);
						if(lv_code>0)
						{
							SetCheckMap(DAMAGE_TEST_MAP, x+1, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-1, i, 0);
						}
						if(lv_code>1)
						{
							SetCheckMap(DAMAGE_TEST_MAP, x+2, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-2, i, 0);
						}
						if(lv_code>2)
						{
							SetCheckMap(DAMAGE_TEST_MAP, x+3, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-3, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x+4, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-4, i, 0);
						}
					}
					att_dir=2;
				}
			}
			//�߹ٶ�
			else if(mizi_code==1)
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				if(lv_code==3)SetDiamondArea(DAMAGE_TEST_MAP, x, y, 0, 1, 0);
					else SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
			}
			//�ҹٴ�
			else if(mizi_code==3)
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				if(lv_code<2)SetDiamondArea(DAMAGE_TEST_MAP, x, y, 0, 1, 0);
					else SetDiamondArea(DAMAGE_TEST_MAP, x, y, 0, 2, 0);
			}
			//�� || ��ȭ
			else if(mizi_code==5 || mizi_code==7)
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				SetDiamondArea(DAMAGE_TEST_MAP, x, y, 0, lv_code, 0);
			}
			//����
			else if(mizi_code==6)
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				if(lv_code==0)SetSquareArea(DAMAGE_TEST_MAP, x, y, 1, 1, 0);
					else if(lv_code==3)SetSquareArea(DAMAGE_TEST_MAP, x, y, 1, 3, 0);
					else SetSquareArea(DAMAGE_TEST_MAP, x, y, 1, 2, 0);
			}
			//����
			else if(mizi_code==8)
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				TransMission(DAMAGE_TEST_MAP, x, y, 0);					
				SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
				//�Ա����� ���� ��
				for(int j=0; j<y_size; j++)
					for(int i=0; i<x_size; i++)
						if(map[i][j][2]>=0)
						{
							TransMission(DAMAGE_TEST_MAP, i, j, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, j, 0);
						}
			}
			//��
			else if(mizi_code==10)
			{
				//��
				if(x<unit[unit_id].x)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					for(int i=0; i<unit[unit_id].x; i++)
					{
						SetCheckMap(DAMAGE_TEST_MAP, i, y, 0);
						if(lv_code>0)
						{
							SetCheckMap(DAMAGE_TEST_MAP, i, y+1, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-1, 0);
						}
						if(lv_code>2)
						{
							SetCheckMap(DAMAGE_TEST_MAP, i, y+2, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-2, 0);
						}
					}
					att_dir=4;
				}
				//��
				else if(x>unit[unit_id].x)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					for(int i=x; i<x_size; i++)
					{
						SetCheckMap(DAMAGE_TEST_MAP, i, y, 0);
						if(lv_code>0)
						{
							SetCheckMap(DAMAGE_TEST_MAP, i, y+1, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-1, 0);
						}
						if(lv_code>2)
						{
							SetCheckMap(DAMAGE_TEST_MAP, i, y+2, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, y-2, 0);
						}
					}
					att_dir=6;
				}
				//��
				else if(y<unit[unit_id].y)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					for(int i=0; i<unit[unit_id].y; i++)
					{
						SetCheckMap(DAMAGE_TEST_MAP, x, i, 0);
						if(lv_code>0)
						{
							SetCheckMap(DAMAGE_TEST_MAP, x+1, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-1, i, 0);
						}
						if(lv_code>2)
						{
							SetCheckMap(DAMAGE_TEST_MAP, x+2, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-2, i, 0);
						}
					}
					att_dir=8;
				}
				//��
				else if(y>unit[unit_id].y)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					for(int i=y; i<y_size; i++)
					{
						SetCheckMap(DAMAGE_TEST_MAP, x, i, 0);
						if(lv_code>0)
						{
							SetCheckMap(DAMAGE_TEST_MAP, x+1, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-1, i, 0);
						}
						if(lv_code>2)
						{
							SetCheckMap(DAMAGE_TEST_MAP, x+2, i, 0);
							SetCheckMap(DAMAGE_TEST_MAP, x-2, i, 0);
						}
					}
					att_dir=2;
				}
			}
			//����&�͵���
			else if(mizi_code==11 || mizi_code==12)
			{					
				if(lv_code==3)
				{
					InitCheckMap(DAMAGE_TEST_MAP, 0);
					SetCheckMap(DAMAGE_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 99);
				}
				else if(lv_code==2)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetDiamondArea(DAMAGE_TEST_MAP, x, y, 0, 1, 0);
				}
				else
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
				}
			}
			//��Ÿ 1ĭ ¥�� ����
			else
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
			}
		}
		else
		{
			InitCheckMap(DAMAGE_TEST_MAP);
			SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
		}

		prev_x=x;
		prev_y=y;
		first_check=true;
	}
}

void CGame::Action(int x, int y)
{
	//������ ����
	if(att_code<1000)
	{
		for(int j=0; j<y_size; j++)
			for(int i=0; i<x_size; i++)
				if(check_map[DAMAGE_TEST_MAP][i][j]!=99)
				{
					if(unit[unit_id].arms==10)
					{
						lib->PlaySound(4);
						Damage(i, j, unit[unit_id].str, 5);	//��
					}
					else if(unit[unit_id].arms==11)
					{
						lib->PlaySound(3);
						Damage(i, j, unit[unit_id].str, 0);	//��
					}
					else if(unit[unit_id].arms==12)		//��
					{
						lib->PlaySound(2);
						Damage(i, j, unit[unit_id].str, 7);
						if(unit_map[i][j]>=0 && unit[unit_map[i][j]].caracter!=NEYUM)unit[unit_map[i][j]].state=1;
					}
					else
					{
						lib->PlaySound(1);
						Damage(i, j, unit[unit_id].str, 1);
					}
				}
		unit[unit_id].wait-=25;
	}
	//����
	else if(att_code<2000)
	{
		int mizi_code=(att_code-1000)/10;
		int mizi_level=att_code%10;

		//������ ������� ���� �ߵ��� ó���ؾ� �� �͵�
		if(mizi_code==6)que.Insert(CM_SHAKE);
		//��� ������ �Ͼ�� ��
		for(int j=0; j<y_size; j++)
			for(int i=0; i<x_size; i++)
				if(check_map[DAMAGE_TEST_MAP][i][j]!=99)
				{
					//��ǳ
					if(mizi_code==0)
					{
						lib->PlaySound(5);
						fog_map[i][j]=false;
						Damage(i, j, 0, 4);
						//ȿ���� �������� ��Ŀ��
						if(att_dir==4)Focus(0, unit[unit_id].y);
							else if(att_dir==6)Focus(x_size-1, unit[unit_id].y);
							else if(att_dir==8)Focus(unit[unit_id].x, 0);
							else if(att_dir==2)Focus(unit[unit_id].x, y_size-1);
					}
					//�߹ٶ�
					else if(mizi_code==1)
					{
						lib->PlaySound(5);
						//�̵��� ����
						if(unit_map[i][j]>=0 && unit[unit_map[i][j]].move<=cara[unit[unit_map[i][j]].caracter].move)
						{
							if(mizi_level==3)unit[unit_map[i][j]].move+=3;
								else unit[unit_map[i][j]].move+=(mizi_level+1);
						}
						Damage(i, j, 0, 4);
					}
					//�ҵ���
					else if(mizi_code==2)
					{
						lib->PlaySound(3);
						if(mizi_level==0)Damage(i, j, 2, 0);
							else if(mizi_level==1)Damage(i, j, 3, 0);
							else Damage(i, j, 4, 0);
					}
					//�ҹٴ�
					else if(mizi_code==3)
					{
						lib->PlaySound(3);
						if(mizi_level==0)Damage(i, j, 1, 0);
							else if(mizi_level==3)Damage(i, j, 3, 0);
							else Damage(i, j, 2, 0);
					}
					//ġ��
					else if(mizi_code==4)
					{
						lib->PlaySound(6);
						if(mizi_level==0)Damage(i, j, -3, 3);
							else if(mizi_level==1)Damage(i, j, -5, 3);
							else Damage(i, j, -10, 3);
					}
					//��
					else if(mizi_code==5)
					{
						lib->PlaySound(4);
						if(move_map[i][j]!=99)map[i][j][0]=3;
						Damage(i, j, 0, 5);
					}
					//����
					else if(mizi_code==6)
					{
						lib->PlaySound(7);
						if(mizi_level<2)Damage(i, j, 1, 99);
							else Damage(i, j, 2, 99);						
					}
					//��ȭ
					else if(mizi_code==7)
					{
						lib->PlaySound(6);
						if(unit_map[i][j]>=0)unit[unit_map[i][j]].state=0;
						Damage(i, j, 0, 3);
					}
					//����
					else if(mizi_code==8)
					{
						lib->PlaySound(8);
						if(mizi_level==3)Damage(i, j, 3, 6);
							else Damage(i, j, 2, 6);
					}
					//�ູ
					else if(mizi_code==9)
					{
						lib->PlaySound(6);
						if(unit_map[i][j]>=0)
						{
							if(mizi_level==3)
							{
								if(unit[unit_map[i][j]].caracter!=NEYUM)unit[unit_map[i][j]].state=4;
							}
							else
							{
								if(unit[unit_map[i][j]].caracter!=NEYUM)unit[unit_map[i][j]].state=3;
							}
						}
						Damage(i, j, 0, 3);
					}
					//��
					else if(mizi_code==10)
					{
						lib->PlaySound(9);
						if(mizi_level<2)Damage(i, j, 1, 8);
							else Damage(i, j, 2, 8);
						//ȿ���� �������� ��Ŀ��
						if(att_dir==4)Focus(0, unit[unit_id].y);
							else if(att_dir==6)Focus(x_size-1, unit[unit_id].y);
							else if(att_dir==8)Focus(unit[unit_id].x, 0);
							else if(att_dir==2)Focus(unit[unit_id].x, y_size-1);
					}
					//����
					else if(mizi_code==11)
					{
						lib->PlaySound(2);
						if(unit_map[i][j]>=0 && unit[unit_map[i][j]].caracter!=NEYUM)unit[unit_map[i][j]].state=1;
						Damage(i, j, 0, 7);
					}
					//�͵���
					else if(mizi_code==12)
					{
						lib->PlaySound(2);
						if(unit_map[i][j]>=0 && unit[unit_map[i][j]].caracter!=NEYUM)unit[unit_map[i][j]].state=2;
						Damage(i, j, 0, 7);
					}
				}

		if(mizi_code==5)MakeMoveMap();	//���� ������ ���� ���
		unit[unit_id].wait-=(mizi_level+1)*10;
	}
	//��Ÿ �ൿ
	else
	{
		if(att_code==2002)	//����
		{
			lib->PlaySound(1);

			Damage(x, y, 0, 1);

			if(map[x][y][1]==1)			//����
			{
				map[x][y][1]=-1;
				party.money++;
			}
			else if(map[x][y][1]==2)	//������
			{
				map[x][y][1]=-1;
				map[x][y][0]=3;
			}

			MakeMoveMap();
			unit[unit_id].wait--;
		}
		else if(att_code==2003)	//�ٸ�
		{
			lib->PlaySound(10);

			if(move_map[x][y]!=99)
			{
				map[x][y][0]=4;
			}

			MakeMoveMap();
			unit[unit_id].wait--;
		}
		else if(att_code==2004)	//ġ��
		{
			lib->PlaySound(6);

			Damage(x, y, -5, 3);
			unit[unit_id].wait-=40;
		}		
	}

	EndTurn();
}

void CGame::EraseFog()
{
	for(int i=0; i<UNIT_MAX; i++)
	{
		//�츮�� ������ �Ȱ��� ����
		if(unit[i].life && unit[i].team==0)
		{
			for(int y=0; y<y_size; y++)
				for(int x=0; x<x_size; x++)
					if(fog_map[x][y] && (unit[i].move)>=(abs(x-unit[i].x)+abs(y-unit[i].y)))fog_map[x][y]=false;
		}
	}
}

void CGame::Cheat(int code)
{
	if(map_id==13 || map_id==14)return;	//�޴��ʿ��� �� ����

	//�Ȱ� ���ֱ�
	if(code==0)
	{
		for(int j=0; j<y_size; j++)
			for(int i=0; i<x_size; i++)
				fog_map[i][j]=false;
	}
	//���ΰ� ����
	else if(code==1)
	{
		unit[0].str=99;
		unit[0].hp_max=unit[0].hp=99;
		unit[0].mizi_level=4;
	}
	//�������� ���� �̵�
	else if(code==2)
	{
		if(_GetKeyState('0'))
		{
			map_id=0;
			escape=1;
		}		
		else if(_GetKeyState('1'))
		{
			map_id=1;
			escape=1;
		}		
		else if(_GetKeyState('2'))
		{
			map_id=2;
			escape=1;
		}		
		else if(_GetKeyState('3'))
		{
			map_id=3;
			escape=1;
		}		
		else if(_GetKeyState('4'))
		{
			map_id=4;
			escape=1;
		}		
		else if(_GetKeyState('5'))
		{
			map_id=5;
			escape=1;
		}		
		else if(_GetKeyState('6'))
		{
			map_id=6;
			escape=1;
		}		
		else if(_GetKeyState('7'))
		{
			map_id=7;
			escape=1;
		}		
		else if(_GetKeyState('8'))
		{
			map_id=8;
			escape=1;
		}		
		else if(_GetKeyState('9'))
		{
			map_id=9;
			escape=1;
		}		
		else if(_GetKeyState('A'))
		{
			map_id=10;
			escape=1;
		}		
		else if(_GetKeyState('B'))
		{
			map_id=11;
			escape=1;
		}		
		else if(_GetKeyState('C'))
		{
			map_id=12;
			escape=1;
		}		
	}
	//ġƮ ����(������ ġƮ ����)
	else if(code==3)
	{
		cheat_able=false;
		escape=99;
	}
	//��ȭ ���� ����
	else if(code==4)
	{
		if(dialog)
		{
			dialog=false;
			click_mode=0;
		}
	}
	//������������ ���� ���� �� ��
	else if(code==5)
	{
		no_enemy=true;
	}
}

void CGame::Function(int func_no)
{
	//���θ޴���
	if(func_no==FUNC_MAINMENU)
	{
		pre_battle=battle;
		SaveNow();
		map_id=13;
		main_menu=false;
		escape=1;
	}
	//����������
	else if(func_no==FUNC_WORLDMAP)
	{
		escape=1;
		map_id=0;
	}
	//���� ����
	else if(func_no==FUNC_ENEMY)
	{
		map_id=map[unit[0].x][unit[0].y][0]+2;
		world_x=unit[0].x;
		world_y=unit[0].y;
		escape=1;
	}
	//������ �̺�Ʈ ��
	else if(func_no==FUNC_BOSS)
	{
		if(map_id==7)GetHero(1);
			else if(map_id==8)GetHero(4);
			else if(map_id==9)GetHero(2);
			else if(map_id==10)GetHero(3);

		if(map_id==11)
		{
			escape=1;
			map_id=12;
		}
		else if(map_id==12)
		{
			unit[1].life=false;
			que.Insert(CM_DIE, 1);
			que.Insert(CM_DIALOG, 121, 124, FUNC_ENDING);
		}
		else if(click_mode!=111)
		{			
			escape=1;
			map_id=0;
		}
	}
	//��ũ������ ����
	else if(func_no==FUNC_QUIZ1)
	{
		que.Insert(CM_QUESTION, 106, FUNC_ANSWER1, FUNC_FAILED);
	}
	//����
	else if(func_no==FUNC_ANSWER1)
	{
		que.Insert(CM_DIALOG, 110, 110, FUNC_QUIZ2);
	}
	//��ũ������ ����2
	else if(func_no==FUNC_QUIZ2)
	{
		que.Insert(CM_QUESTION, 111, FUNC_FAILED, FUNC_ANSWER2);
	}
	//����
	else if(func_no==FUNC_ANSWER2)
	{
		que.Insert(CM_DIALOG, 112, 113, FUNC_QUIZ3);
	}
	//��ũ������ ����3
	else if(func_no==FUNC_QUIZ3)
	{
		que.Insert(CM_QUESTION, 114, FUNC_FAILED);
	}
	//����
	else if(func_no==FUNC_FAILED)
	{
		que.Insert(CM_DIALOG, 107, 109, FUNC_GAMEOVER);
	}
	//���� ����
	else if(func_no==FUNC_GAMEOVER)
	{
		escape=2;
	}
	//����
	else if(func_no==FUNC_ENDING)
	{
		escape=5;
	}
}

bool CGame::Eliminate()
{
	for(int i=0; i<UNIT_MAX; i++)
		if(unit[i].life && unit[i].team!=0)return false;

	return true;
}

void CGame::GetHero(int id)
{
	bool blank=false;

	//���ڸ��� ������ �׳� ����
	for(int i=0; i<PARTY_MAX; i++)
		if(!party.unit[i].life)
		{
			party.unit[i].SetUnit(0, 0, id, 0);
			blank=true;
			break;
		}

	//������ ��Ƽ���� �� ���� ���� ����
	if(!blank)
	{
		for(int i=0; i<PARTY_MAX; i++)
			if(party.unit[i].life && party.unit[i].caracter<5)able_unit[i]=false;
				else able_unit[i]=true;
		
		selected2=id;
		click_mode=111;
	}
}

void CGame::PartySort()
{
	for(int i=1; i<PARTY_MAX; i++)
	{
		if(!party.unit[i].life)
		{
			//�ű� ĳ���� ã��
			int change_target=-1;
			for(int j=i+1; j<PARTY_MAX; j++)
				if(party.unit[j].life)
				{
					change_target=j;
					break;
				}

			if(change_target==-1)break;	//�������̸� ������

			//�ű��
			party.unit[i].SetUnit(0, 0, party.unit[change_target].caracter, 0);
			party.unit[i]=party.unit[change_target];
			party.unit[change_target].life=false;
		}
	}
}