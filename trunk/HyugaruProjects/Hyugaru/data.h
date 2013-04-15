#define CUSTOM_BLUE	JColor(149,190,209)
#define YELLOW		JColor(255,255,0)

HWND hwnd;

//미디 연주
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
//장비 클래스
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

//장비 종류와 수
#define EQUIP_MAX 17
CEquip equipment[EQUIP_MAX];

//////////////////////////////////////////////////////
//캐릭터 클래스

//캐릭터 ID
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

//장비 명령
#define EQ_ARMS		0
#define EQ_AMMOR	1

class CCaracter
{
public:
	//같은 캐릭터의 공통되는 정보
	char* name;
	int hp_max;
	int move;
	int mizi_type;
	int move_type;

	//표시할 그림
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
//유닛 클래스
class CUnit
{
public:
	//기본 정보
	bool life, soul;
	char team;

	char caracter;
	int x,y;
	int sx, sy;			//애니메이션을 위한 특수 좌표
	int hp, hp_max;
	int move;
	int mizi_type;
	int move_type;

	//장비, 능력
	int arms;
	int str;
	int ammor;
	int dp;
	int mizi_level;
	int skill[4];

	//상태
	int wait;
	char state;			//0: 정상	1: 독 	2: 맹독   3: 축복   4: 강한 축복

	//파티에서의 번호(아군만 해당)
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
	
	//캐릭터 고유 속성 받기
	hp=cara[caracter].hp_max;
	hp_max=cara[caracter].hp_max;
	move=cara[caracter].move;
	mizi_type=cara[caracter].mizi_type;
	move_type=cara[caracter].move_type;

	wait=100;
	state=0;

	//캐릭터의 초기 무기(아군의 경우 설정하지 않았다가 설정)
	str=1;
	arms=0;
	ammor=-1;
	dp=0;
	mizi_level=1;
	for(int i=0; i<4; i++)skill[i]=-1;

	//공용 설정
	if(caracter==HERO1)			//휴가루
	{
		skill[0]=0;
		skill[1]=1;
	}
	else if(caracter==HERO2)	//가미얼
	{
		skill[0]=2;
		skill[1]=3;
	}
	else if(caracter==HERO3)	//시크벨
	{
		skill[0]=4;
		skill[1]=5;
	}
	else if(caracter==HERO4)	//루벨버
	{
		str=3;
		skill[0]=6;
		skill[1]=7;
	}
	else if(caracter==HERO5)	//미버가
	{
		skill[0]=8;
	}
	else if(caracter==HERO6)	//휴루시
	{
		mizi_level=4;
		skill[0]=9;
		skill[1]=10;
	}
	else if(caracter==DARKQUIZ)	//다크퀴즈
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
	//적일 경우에 캐릭터 설정
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
//큐 클래스
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

	//생성, 소멸, 삽입, 삭제
	void Init();
	void Destroy();
	void Insert(unsigned char command, int id=0, int x=0, int y=0, WORD time=0xffff);
	bool Delete(int id);

	//값 얻기
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
	//빈 큐에 삽입
	for(int i=0; i<que_max; i++)
	{
		if(que[i].time<que_time)
		{
			que[i].Insert(command, id, x, y, time);
			return;
		}
	}

	//큐 용량 확장
	if(que_max>=limit)
	{
		//데이터를 백업하고 기존의 큐배열 삭제
		QueData* tmp_que=new QueData[que_max];
		for(int i=0; i<que_max; i++)tmp_que[i]=que[i];
		delete [] que;
		//확장하고 데이터를 옮김
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
//숫자 클래스
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
	//초기값
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
		int cmp=10;		//자릿수를 알기 위해 비교하는 수
		width=1;
		int tmp=abs(value);

		//음수, 양수 구분
		if(value<0)minus=true;
			else minus=false;

		//필요한 공간 계산
		while(tmp>=cmp)
		{
			cmp*=10;
			width++;
			if(width>=10)break;
		}

		//각 자리수 마다 출력할 숫자 정하기
		for(int i=0; i<width; i++)
		{
			buffer[(width-1)-i]=tmp%10;
			tmp/=10;
		}

		pre_number=value;
	}

	//출력
	if(minus)lib->Image(x, y, img_id, 10);
	for(int i=width-1; i>=0; i--)
	{
		if(minus)lib->Image(x+(i+1)*fontsize, y, img_id, buffer[i]);
			else lib->Image(x+i*fontsize, y, img_id, buffer[i]);
	}
}

//////////////////////////////////////////////////////
//이미지 번호 클래스(게임 클래스와 같이 쓰기 위해서)
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
//파티 클래스
#define PARTY_MAX	20		//파티원 한계

class CParty
{
public:
	CUnit unit[PARTY_MAX];
	int money;
	int item[EQUIP_MAX];
};

//////////////////////////////////////////////////////
//게임 클래스
#define UNIT_MAX	50		//유닛 한계

//체크맵 종류
#define MOVE_TEST_MAP	0
#define MOVE_BACK_MAP	1
#define ATTACK_TEST_MAP	2
#define DAMAGE_TEST_MAP	3
#define ANIMATION_MAP	4
#define DELAY_MAP		5
#define DISTANCE_MAP	6

//명령어
#define CM_FOCUS		0	//포커스
#define CM_MOVE			1	//이동중인 상태
#define CM_MOVEEND		2	//이동 끝
#define CM_DAMAGE		3	//데미지
#define CM_HEEL			4	//치료
#define CM_DIE			5	//사망
#define CM_WARP			6	//워프
#define CM_DIALOG		7	//대화
#define CM_NEXT_DIALOG	8	//다음 대화
#define CM_QUESTION		9	//질문
#define CM_ANSWER		10	//대답
#define CM_SHAKE	11	//지진

//함수
#define FUNC_NONE		0	//아무일 없음(반드시 0)
#define FUNC_MAINMENU	1	//메인 메뉴로 감
#define FUNC_WORLDMAP	2	//세계지도로 감
#define FUNC_ENEMY		3	//적의 출현
#define FUNC_BOSS		4	//보스급 전투후
#define FUNC_QUIZ1		5	//다크퀴즈의 퀴즈
#define FUNC_ANSWER1	6
#define FUNC_QUIZ2		7
#define FUNC_ANSWER2	8
#define FUNC_QUIZ3		9
#define FUNC_ENDING		10
#define FUNC_FAILED		13
#define FUNC_GAMEOVER	14

//이벤트
#define EV_START		0
#define EV_MOVE			1
#define EV_ACTION_END	2
#define EV_END			3

//이벤트 스위치
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
//저장할 필요 없는 전역 변수
CHungLib* lib;	//라이브러리 포인터

int panel_x;	//판의 위치
int panel_y;
CNumber n_hp, n_ammor, n_shield, n_arms, n_mizi, n_move, n_wait;	//숫자 클래스

//데미지 지역을 체크하기 위해 쓰이는 변수
bool first_check;
int prev_x, prev_y;
char att_dir;

int AI_MouseX, AI_MouseY;	//AI가 사용하는 마우스
int AI_thinking;

bool main_menu;	//시작 메뉴인지 중간 메뉴인지 판별
bool save_mode;	//세이브인지 로드인지 판별

//세이브 데이터 정보 표시
class CFileData
{
public:
	bool read_able;
	int map_id;
};
CFileData file_data[3];

//대화&질문 관련 변수
bool dialog, question, answer;
int answer_func, answer_func2;
int dialog_no;
int dialog_end;

//상수들
int have_money[]={0, 200, 200, 200, 200, 0, 0, 0, 0, 60, 20, 40, 30, 45, 25, 0, 0};	//적 해치우고 얻는 돈
char* mizi_name[]={"바람", "불", "물", "땅", "번개", "빛", "독"};					//미지 이름

//상점
int shop[4];
int select_price;
bool able_unit[20];
int selected, selected2, selected3;

//치트
bool no_enemy=false;
bool cheat_able=true;

int clear_count=0;	//클리어 횟수

//대화 내용들
char* dialog_text[]={
/*000:*/	"-1",	"휴가루가 사는 세계에는 개나 소나 마왕한다고 난리였다.\n다크 퀴즈도 그 중 하나였다.\n아무것도 아닐 것 같던 다크퀴즈는 놀랍게도 세계를 장악한다.\n그래서 귀찮지만 휴가루는 다크퀴즈를 없애기 위해 여행길에 오른 것이다.",
/*001:*/	"-1",	"휴가루가 사는 세계에는 5개의 마을이 있다.\n중앙이 자유로운 바람의 마을 휴가루,\n아직 마왕에게 정복 당하지 않은 유일한 마을이자 휴가루의 고향이다.\n그 바로 옆에 마왕성이 있다.",
/*002:*/	"-1",	"북쪽은 사막과 불의 마을, 가미얼 마을.\n서쪽은 구름과 번개의 마을, 미버가 마을.\n동쪽은 숲과 땅의 마을, 루벨버 마을\n남쪽은 물의 마을, 시크벨 마을.",
/*003:*/	"-1",	"나머지 4개의 마을은 마왕의 4명의 장군이 지키고 있다.",
/*004:*/	"-1",	"설명끝, 세계를 구하든지... 말든지...\n살든지... 죽든지...\n어허랑 어허야 디야~♬",
/*005:*/	"0",	"<휴가루>\n뭐 저딴 설명이 다 있어?\n아무튼 세계를 구해볼까? 딱히 할 일도 없으니...\n죽고 싶으면 아무데나 쳐들어가도 되겠지만 일단 마을에서 정비를 해야지.",
/*006:*/	"0",	"<휴가루>\n돈은 많이 있으니까!\n으케케케케!",
/*007:*/	"-1",	"<주민>\n휴가루님, 저희도 더 이상 돈 없습니다.\n서랍을 뒤지지 말아 주십시오.",
/*008:*/	"-1",	"<주민>\n많은 건 안 바라니까 그냥 대충 세계만 구해주세요.",
/*009:*/	"-1",	"<주민>\n다크 퀴즈는 처음에 별 거 아니었는데,\n언제 부턴가 4명의 장군을 얻고 강해졌더군요.\n그러고 보니 다크퀴즈가 직접 싸우는 건 못 봤어요.",
/*010:*/	"-1",	"<주민>\n마왕의 영토에는 어둠의 안개가 깔려 있으니 앞이 잘 안 보일 겁니다.\n휴가루님의 돌풍미지라면 안개를 걷을 수 있겠죠.",
/*011:*/	"-1",	"<주민>\n미지 중에 벼락이 최고라고 생각해요. 물에도 흐르고, 생물끼리 감전시키고,\n구름 밑에도 내려치고 공격 범위가 엄청나죠. 쇠갑옷을 파고들어 피해도 주죠.\n가죽갑옷에 방전되는 게 약점이지만...",
/*012:*/	"-1",	"<주민>\n다크퀴즈의 4명의 장군은 마족이 아니라 인간인 것 같아요.",
/*013:*/	"17",	"<가미얼>\n우하하핫! 잘 왔다.\n여기는 후끈후끈 사막 마을.\n이 가미얼 님이 널 해치운 후 말려서 건어물로 만들어 주지.",
/*014:*/	"0",	"<휴가루>\n난 포유류라서 건어물은 무리일 듯 싶다.",
/*015:*/	"17",	"<가미얼>\n흐음, 그건 그렇군.\n그럼 어떻게 한다?",
/*016:*/	"0",	"<휴가루>\n내가 널 이기면 돼.",
/*017:*/	"17",	"<가미얼>\n오호, 그거 좋은 생각이군.\n한 번 할 수 있으면 해봐라.\n가자! 매코들아!\n불 싸질러보자꾸나.",
/*018:*/	"30",	"<매코>\n매코!!!",
/*019:*/	"18",	"<시크벨>어서 오십시오.\n이 마을을 해방시키고 싶으면 절 쓰러뜨려야 합니다.",
/*020:*/	"0",	"<휴가루>\n너 남자냐? 여자냐?",
/*021:*/	"18",	"<시크벨>\n남자입니다.",
/*022:*/	"0",	"<휴가루>\n......",
/*023:*/	"0",	"<휴가루>\n죽여주마.",
/*024:*/	"0",	"<휴가루>\n구름이 많군.",
/*025:*/	"20",	"<미버가>\n그 구름이 나의 힘을 더해주지.\n여긴 나의 영역, 맛깔스럽게 찌져주마.",
/*026:*/	"0",	"<휴가루>\n흥!",
/*027:*/	"0",	"<휴가루>\n한번만 봐주세요.",
/*028:*/	"20",	"<미버가>\n싫다.",
/*029:*/	"0",	"<휴가루>\n울창한 숲이군.",
/*030:*/	"19",	"<루벨버>\n울창한 것 뿐만이 아니지...\n이 숲의 공포는 곧 알게 될거다.\n마왕님에게 대적하는 자에게는 죽음을...",
/*031:*/	"0",	"<휴가루>\n죽음을 어쩐다고?",
/*032:*/	"19",	"<루벨버>\n어쩔까?",
/*033:*/	"0",	"<휴가루>\n죽음을...\n...넘어선 사랑을 하게 해줘.",
/*034:*/	"19",	"<루벨버>\n넌 좀 맞아야 겠다.",
/*035:*/	"21",	"<휴루시>\n하하핫! 겁을 아침마다 우유에 말아먹은 녀석!\n감히 여기가 어디라고 발을 들여놓느냐!",
/*036:*/	"0",	"<휴가루>\n아니! 당신은 빛의 용사, 휴루시!\n어째서 다크 퀴즈의 부하가 되었지?",
/*037:*/	"21",	"<휴루시>\n다크퀴즈님이야 말로 이 세계를 정복하기에 올바른 분이시기 때문이다.\n다크퀴즈님의 부하가 되거라!",
/*038:*/	"0",	"<휴가루>\n싫다! 난 인간들을 위해 싸울 것이다.",
/*039:*/	"0",	"<휴가루>\n인간이 있어야 돈을 벌어먹지!\n미알 놈들은 너무 욕심이 없어.\n자신의 배만 채울 음식만 있으면 만족하고!",
/*040:*/	"0",	"<휴가루>\n재산도 안 가지고 싸우기만 열심히 싸우고...\n와! 이거 이상사회다.",
/*041:*/	"21",	"<휴루시>\n그렇지?",
/*042:*/	"0",	"<휴가루>\n그래도 난 싫어!\n난 자유로운 바람의 용사니까!",
/*043:*/	"21",	"<휴루시>\n후후후, 그럼 처단하는 수 밖에...",
/*044:*/	"0",	"<휴가루>\n오, 나의 고향, 나의 마을.",
/*045:*/	"0",	"<휴가루>\n바위에 둘러싸인 집은 무기점.\n무기와 갑옷을 살 수 있지.",
/*046:*/	"0",	"<휴가루>\n돌에 둘러싸인 집은 수리점.\n갑옷을 고칠 수 있지.",
/*047:*/	"0",	"<휴가루>\n나무로 둘러싸인 곳은 학교. 미지등급을 높일 수 있지.\n미지란 물리적이 아닌 뭔가 미지의 힘같은 거지.\n다른 세계에선 마법이라고도 하던데, 약간 틀려.",
/*048:*/	"0",	"<휴가루>\n선인장으로 둘러싸인 곳은 알선소.\n용병을 고용할 수 있지. 최고 15명까지.\n용병은 전투에서 쓰러지면 없어진다구.",
/*049:*/	"0",	"<휴가루>\n여담으로 몇가지 더 가르쳐 주자면 이동상태에서\n마우스 오른쪽 버튼을 클릭하면 캐릭터 정보를 볼 수 있는데,\n캐릭터가 없는 곳을 클릭하면 최초의 방으로 갈 수 있지.",
/*050:*/	"0",	"<휴가루>\n음하하하핫! 이 친절한 설명!",
/*051:*/	"0",	"<휴가루>\n좋았어. 이걸로 '아잉, 우리 고향 최공~♡'에 나갈 준비는 다 됐다.\n 다크퀴즈 없애고 출연해야지.",
/*052:*/	"-1",	"도망치시겠습니까?",
/*053:*/	"-1",	"최초의 방으로 이동하겠습니까?",
/*054:*/	"-1",	"부스럭...",
/*055:*/	"0",	"<휴가루>\n응? 적인가?",
/*056:*/	"-1",	"전투에서 승리하였습니다.\n보너스: 50카오",
/*057:*/	"2",	"<시크벨>\n으... 머리가... 어라?",
/*058:*/	"0",	"<휴가루>\n최후의 일격!!!",
/*059:*/	"2",	"<시크벨>\n잠시만 기다려 주십시오.\n혹시 당신은 그 유명한 바람의 용사 휴가루?",
/*060:*/	"0",	"<휴가루>\n음하하, 내가 좀 유명하지.",
/*061:*/	"2",	"<시크벨>\n전 사실 다크퀴즈를 쓰러뜨리려고 했습니다.\n그 다음 어찌된지 몰라도 조종당한듯 싶군요.",
/*062:*/	"0",	"<휴가루>\n음하하, 그럼 유명한 나를 도와 다크퀴즈를 무찌르자고...",
/*063:*/	"2",	"<시크벨>\n예, 알겠습니다.",
/*064:*/	"-1",	"시크벨이 동료가 되었다!",
/*065:*/	"1",	"<가미얼>\n크앗! 죽어라! 다크퀴즈! 엇?",
/*066:*/	"0",	"<휴가루>\n맞더니 맛이 갔나? 왜 이래?",
/*067:*/	"1",	"<가미얼>\n난 다크퀴즈와 싸우고 있었는데...",
/*068:*/	"0",	"<휴가루>\n나는 다크퀴즈의 장군과 싸우고 있었는데?",
/*069:*/	"1",	"<가미얼>\n크흑, 이 가미얼님이 다크퀴즈에게 조종당하신 듯 싶다.\n두고보자, 다크퀴즈.",
/*070:*/	"-1",	"가미얼이 동료가 되었다!",
/*071:*/	"3",	"<루벨버>\n......",
/*072:*/	"0",	"<휴가루>\n갑자기 왜 그래?",
/*073:*/	"3",	"<루벨버>\n미안하다. 내가 아무래도 다크퀴즈에게 조종당한 듯 싶군.",
/*074:*/	"0",	"<휴가루>\n미안하면 다야? 미안하면 다면 경찰이 왜 있겠어?",
/*075:*/	"3",	"<루벨버>\n이 죄, 목숨으로 씻겠다.",
/*076:*/	"0",	"<휴가루>\n잠까안! 농담이야!",
/*077:*/	"3",	"<루벨버>\n아냐, 마왕에게 협력하다니.\n살 면목이 없다.",
/*078:*/	"0",	"<휴가루>\n아니, 있어.",
/*079:*/	"3",	"<루벨버>\n도대체 무슨...",
/*080:*/	"0",	"<휴가루>\n아, 뭐. 귀찮고... 대충... 그...\n함께 다크퀴즈나 물리치자고.",
/*081:*/	"3",	"<루벨버>\n다크퀴즈를 물리쳐 죄를 씻으라는 건가?\n관대한 배려 고맙다.",
/*082:*/	"0",	"<휴가루>\n에휴~ 피곤한 타입이군.",
/*083:*/	"-1",	"루벨버가 동료가 되었다!",
/*084:*/	"4",	"<미버가>\n얏호!",
/*085:*/	"0",	"<휴가루>\n뭐 이래? 이거 미쳤나?",
/*086:*/	"4",	"<미버가>\n아니, 난 다크퀴즈의 조종을 받았는데, 좀 맞고 나니까 그게 풀린거지.\n그래서 고맙다고 하려는 거다.",
/*087:*/	"0",	"<휴가루>\n아, 그런거야.",
/*088:*/	"4",	"<미버가>\n좋아, 형제여! 다크퀴즈 없애러 감세다.",
/*089:*/	"0",	"<휴가루>\n(이... 이놈, 제 정신이 아니다.)",
/*090:*/	"-1",	"미버가가 동료가 되었다!",
/*091:*/	"5",	"<휴루시>\n쿨럭, 윽... 여기는?",
/*092:*/	"0",	"<휴가루>\n이제 정신이 든 건가?",
/*093:*/	"5",	"<휴루시>\n후후... 그렇군. 고맙다. 나를 다크퀴즈로부터 해방시켜 줘서.",
/*094:*/	"0",	"<휴가루>\n도대체 어떻게 된 거야?",
/*095:*/	"5",	"<휴루시>\n다크퀴즈... 녀석은 별로 강하지 않아.\n비겁하게 싸울 뿐이지.\n특히 그의 퀴즈상자를 조심해라.",
/*096:*/	"0",	"<휴가루>\n퀴즈 상자?",
/*097:*/	"5",	"<휴루시>\n녀석이 묻는 질문에 당연히 해야 할 대답을 하지 마라.\n그것이 녀석의 퀴즈상자를 피하는 방법이다.",
/*098:*/	"0",	"<휴가루>\n만약 당연한 대답을 하게 된다면...?",
/*099:*/	"5",	"<휴루시>\n나처럼 녀석의 부하가 되어 버리지.\n쿨럭! 으윽... 그럼 마지막으로...\n난 사실... 네 형이다.",
/*100:*/	"0",	"<휴가루>\n아, 그러셔.",
/*101:*/	"5",	"<휴루시>\n으억, 쿨럭~",
/*102:*/	"0",	"<휴가루>\n아무튼 드디어 다크퀴즈의 앞까지 도달했다!\n해치우자!",
/*103:*/	"15",	"<다크퀴즈>\n용케도 여기까지 왔구나. 훌륭하다 용사여.",
/*104:*/	"0",	"<휴가루>\n랄 하네~.",
/*105:*/	"15",	"<다크퀴즈>\n후후후... 그 기백이 마음에 드는군.",
/*106:*/	"15",	"<다크퀴즈>\n나의 부하가 되겠느냐?",
/*107:*/	"0",	"<휴가루>\n으윽... 머리가...",
/*108:*/	"15",	"<다크퀴즈>\n크크크, 걸렸군. 이제 넌 나의 부하다!",
/*109:*/	"-1",	"그렇게 휴가루를 부하로 삼은 다크퀴즈는 세계를 정복했습니다.\n-게임오버-",
/*110:*/	"15",	"<다크퀴즈>\n이봐, 그딴 용사가 어딨어!\n",
/*111:*/	"15",	"<다크퀴즈>\n좋아, 그럼 나의 부하가 되기 싫다면 나의 주인이 되어 다오.",
/*112:*/	"15",	"<다크퀴즈>\n크흑, 그런가... 이런 요청을 거부하다니...",
/*113:*/	"15",	"<다크퀴즈>\n나의 퀴즈상자에 대해 아는가 보군.",
/*114:*/	"15",	"<다크퀴즈>\n좋아! 싸우자!",
/*115:*/	"15",	"<다크퀴즈>\n으어억... 너무 아파...\n약골로 태어나 퀴즈상자를 얻어 세계 한 번 정복하려 했건만...\n역시 난 안 되나...",
/*116:*/	"0",	"<휴가루>\n그래, 이 찌질아.",
/*117:*/	"15",	"<다크퀴즈>\n크크, 끝까지 동정없는 녀석이로군.\n마지막으로 용사여...\n퀴즈 좋아하나...?",
/*118:*/	"0",	"<휴가루>\n싫어하진 않아.",
/*119:*/	"15",	"<다크퀴즈>\n그래...\n다행이다.",
/*120:*/	"0",	"<휴가루>\n빨랑 죽어.",
/*121:*/	"-1",	"이렇게 휴가루는 다크퀴즈를 물리쳤습니다.\n그리고 그날 저녁 <아잉, 우리 고향 최공~♡>에 출현하여 퀴즈상자를 사용,\n관객을 모두 자기편으로 만들어 우승했다고 합니다.",
/*122:*/	"-1",	"아무튼 이것으로 자유로운 바람의 용사 휴가루의 이야기는 끝이 났습니다.",
/*123:*/	"-1",	"다음 번엔 자유로운 바람의 마왕의 이야기 <휴가루 웍휴2>로 찾아뵙겠습니다.",
/*124:*/	"-1",	"진심으로 감사 받아야 겠습니다.\n아, 정말 만드느라 힘들었죠.",
/*125:*/	"-1",	"<주민>\n미알들은 의지를 가진 인간외의 생물을 말합니다.\n마왕의 부하가 되기도 하고, 잘 길들이면 인간의 편이 되어 주기도 하죠.",
/*126:*/	"-1",	"<주민>\n빛의 갑옷이라고 들어보셨습니까?\n내구력 99의 말도 안 되는 갑옷이라고 하더군요.",
/*127:*/	"-1",	"<주민>\n예전에 많은 마왕을 물리치셨던 빛의 용사 휴루시님이라고 계시죠.\n요즘은 어디 가셨는지 모르겠군요.",
/*128:*/	"-1",	"<의문의 주민>\n아직은 때가 아냐...",
/*129:*/	"-1",	"<주민>\n가미얼님을 정상으로 돌려주셔서 감사합니다.",
/*130:*/	"-1",	"<주민>\n마을 중앙의 오아시스는 매우 중요한 곳이죠.\n시크벨님께서 만들어 주셨죠.\n아, 그리고 선인장도 베면 물이 나와죠.",
/*131:*/	"-1",	"<주민>\n미버가 님을 제 정신으로 만들어 주셔서 감사합니다.",
/*132:*/	"0",	"<휴가루>\n아니, 저 상태가 제 정신이라고요?",
/*133:*/	"-1",	"<주민>\n불행히도...",
/*134:*/	"-1",	"<주민>\n저희 마을에는 벼락이 많이 쳐서 집을 방전이 되게 지었죠.",
/*135:*/	"0",	"<휴가루>\n다른데랑 똑같은데?",
/*136:*/	"-1",	"<주민>\n내부가 다른 것이죠. 하하핫!",
/*137:*/	"-1",	"<주민>\n여긴 척박해서 참 살기가 어려워요.",
/*138:*/	"0",	"<휴가루>\n그럼 왜 여기 계속 사는데?",
/*139:*/	"-1",	"<주민>\n야이 자식아! 누가 살고 싶어서 사나?\n그렇게 된 걸 어쩌라고!\n우리도 좋은데 살고 싶어. 그런데 돈이 없어!",
/*140:*/	"-1",	"<주민>\n인간은 재밌어. 크크크...",
/*141:*/	"-1",	"<주민>\n시크벨님은 참 좋으신 분입니다.\n다른 마을에도 도움을 주셨죠.",
/*142:*/	"-1",	"<주민>\n시크벨님 너무 잘 생기셨어.\n멋져♡",
/*143:*/	"-1",	"<주민>\n물의 마을이라 이동이 참 불편합니다.",
/*144:*/	"-1",	"<주민>\n물의 마을. 멋있죠?",
/*145:*/	"-1",	"<주민>\n우리도 배를 만들면 좋을 텐데...",
/*146:*/	"-1",	"<주민>\n저주가 풀려서 다행입니다.\n나무가 움직여 대서 길을 못 찾았다고요...",
/*147:*/	"-1",	"<주민>\n음... 오히려 마을이 불편해 진 것 같습니다.\n나무들이 움직여 댈 땐 그냥 가만히 있어도 길이 생겼는데...\n애초부터 우리 마을은 잘못 만들어 졌어.",
/*148:*/	"-1",	"<주민>\n시크벨님의 도움으로 나무들이 무성하게 자랐습니다.",
};

//----------------------------------------------//

CParty party;	//파티 정보

class CGame
{
private:
	//맵과 월드맵에서의 위치
	int map_id;
	int world_x;
	int world_y;

	//맵 데이터
	char map[48][48][3];
	char unit_map[48][48];
	bool fog_map[48][48];
	unsigned char check_map[7][48][48];
	unsigned char move_map[48][48];

	int x_size, y_size;
	bool battle;
	bool pre_battle;
	bool boss;

	//유닛 정보
	CUnit unit[UNIT_MAX];
	char unit_id;
	int unit_x;
	int unit_y;
	int state_id;
	int att_code;

	//이벤트 처리용 스위치
	bool event_switch[SWTICH_MAX];
	int test;

	//큐 클래스
	Que que;

	/////////////////////////////
	//Private 메소드

	//유닛 생성 관련
	void SetUnit(int id, int vx, int vy, char vcaracter, char vteam);
	int GetUnitID();
	void AddUnit(int vx, int vy, char vcaracter, char vteam);

	//검사용 맵 관련
	void InitCheckMap(int id, unsigned char value=99);
	void SetCheckMap(int id, int x, int y, unsigned char value);
	void Search(int id, int x, int y, int dist, int type, char team);
	void SearchRoad(int id, int x, int y, int type, char team);
	unsigned char SearchNear(int id, int x, int y);
	void SetDiamondArea(int id, int x, int y, int dist_min, int dist_max, int value);
	void SetSquareArea(int id, int x, int y, int dist_min, int dist_max, int value);
	void TransMission(int id, int x, int y, int value, bool water=true, bool human=true);
	void MakeMoveMap();

	//맵 관련
	void Focus(int x, int y, WORD time=0xffff);

	//유닛 관련
	void SetAttackArea();
	void SetDamageArea(int x, int y);
	void Action(int x, int y);
	void EndTurn();
	void UnitMove(int id, int x, int y);
	void Damage(int x, int y, int value, int type);
	void EraseFog();
	bool Eliminate();
	void GetHero(int id);

	//저장, 불러오기
	void SaveNow();
	void Save(int file_no);
	bool Load(int file_no);
	void LoadInfo();

	//기타 명령어들
	void Function(int func_no);
	void SelectBGM();

public:
	//맵 관련 변수
	int fx_i, fy_i;
	char click_mode;	//-1: 정보 0: 제어 1: 이동 2: 이동중 3: 공격선택 4: 공격범위선택 5: 미지선택 99: AI
	bool sw_focus;
	bool sw_move;
	bool sw_ai;
	char escape;

	/////////////////////////////
	//Public 메소드
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

	//숫자 초기화
	n_hp.Set(lib, imgno.number, 5);
	n_ammor.Set(lib, imgno.number, 5);
	n_shield.Set(lib, imgno.number, 5);
	n_arms.Set(lib, imgno.number, 5);
	n_mizi.Set(lib, imgno.number, 5);
	n_move.Set(lib, imgno.number, 5);
	n_wait.Set(lib, imgno.number, 5);
}

//새게임일 때 초기화
void CGame::Init()
{
	pre_battle=false;

	//파티 초기화
	party.money=1000;
	party.unit[0].SetUnit(0, 0, HERO1, 0);
	for(int i=1; i<PARTY_MAX; i++)party.unit[i].life=false;
	for(int i=0; i<EQUIP_MAX; i++)party.item[i]=0;

	//스위치 초기화
	for(int i=0; i<SWTICH_MAX; i++)event_switch[i]=false;
}

void CGame::Start()
{
	//명령처리 초기화
	que.Init();
	sw_focus=false;
	sw_move=false;

	//기타 초기화
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

//유닛 설정
void CGame::SetUnit(int id, int vx, int vy, char vcaracter, char vteam)
{
	unit[id].SetUnit(vx, vy, vcaracter, vteam);
	unit_map[vx][vy]=id;
}

//사용 가능한 유닛 번호 알려주기
int CGame::GetUnitID()
{
	for(int i=2; i<UNIT_MAX; i++)
		if(!unit[i].life)return i;
	return -1;
}

//유닛 추가
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

	//전투 맵인지 마을맵인지 구분
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

	//맵 데이터 읽어 들이기
	char file_name[20];
	sprintf(file_name,"map\\map%03d.map",map_id);

	if(fp=fopen(file_name,"rb"))
	{
		//데이터 종류와 버전 확인
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

		//초기화
		for(int i=0; i<UNIT_MAX; i++)unit[i].life=unit[i].soul=false;
		for(int j=0; j<48; j++)
			for(int i=0; i<48; i++)
			{
				unit_map[i][j]=-1;
				if(battle && map_id>=7)fog_map[i][j]=true;
					else fog_map[i][j]=false;
			}

		//필터링, 유닛 초기위치 설정
		int party_no=1;		//배치할 파티원 번호
		for(int j=0; j<y_size; j++)
			for(int i=0; i<x_size; i++)
			{
				if(Between(map[i][j][1],9,14))map[i][j][1]-=9;
					else map[i][j][1]=-1;
				if(Between(map[i][j][2],16,16))map[i][j][2]-=16;
					else
					{						
						//아군 배치
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
						//적군 배치
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
						//주인공 배치
						else if(map[i][j][2]==19)
						{
							SetUnit(0, i, j, party.unit[0].caracter, 0);
							unit[0]=party.unit[0];
							unit[0].party_id=0;
						}
						//보스 배치
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
		//월드맵인 경우
		if(map_id==0)
		{
			//SetUnit(0, world_x, world_y, party.unit[0].caracter, 0);
			unit_map[unit[0].x][unit[0].y]=-1;
			unit_map[world_x][world_y]=0;
			unit[0].x=world_x;
			unit[0].y=world_y;
		}
		//메인메뉴의 경우
		else if(map_id==13)
		{
			//저장
			if(main_menu)
			{
				map[4][7][0]=7;
				map[5][7][0]=7;
				map[6][7][0]=7;
			}
			//장비&파티설정
			if(main_menu || pre_battle)
			{
				map[5][8][0]=7;
				map[5][9][0]=7;

				map[8][9][0]=7;
				map[9][9][0]=7;
			}
			//비밀 알선소
			if(clear_count<2 || main_menu || pre_battle)
			{
				map[5][5][0]=7;
				map[6][5][0]=7;
			}
			//비밀 상점
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

		//포커스를 주인공에게로
		Focus(unit[0].x, unit[0].y);
		Event(EV_START);

		SelectBGM();

		fclose(fp);
		return true;
	}

	return false;
}

//체크 맵 초기화
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

//이동할 수 있는 칸 검색
void CGame::Search(int id, int x, int y, int dist, int type, char team)
{
	if(x<0 || y<0 || x>=x_size || y>=y_size)return;
	if(type==0)//보행
	{
		if(unit_map[x][y]>=0 && unit[unit_map[x][y]].team!=team)dist=99;	//적이 막으면 이동 불가능
			else dist+=(int)move_map[x][y];
	}
	else if(type==-1)	//비행
	{
		if(move_map[x][y]==99)dist=99;	//지나갈 수 없는 벽이면 비행도 불가
			else dist++;
	}
	else if(type==-2)	//공격
	{
		dist++;
	}

	if((int)check_map[id][x][y]<=dist)return;	//이동력이 부족하거나 지나갈 수 없으면 검색 끝냄

	//상하좌우 검사
	check_map[id][x][y]=(unsigned char)dist;	
	Search(id, x-1, y, dist, type, team);
	Search(id, x+1, y, dist, type, team);
	Search(id, x, y-1, dist, type, team);
	Search(id, x, y+1, dist, type, team);
}

//길 찾기
void CGame::SearchRoad(int id, int x, int y, int type, char team)
{
	InitCheckMap(id);

	check_map[id][x][y]=0;
	Search(id, x-1, y, 0, type, team);
	Search(id, x+1, y, 0, type, team);
	Search(id, x, y-1, 0, type, team);
	Search(id, x, y+1, 0, type, team);
}

//주위에 제일 가까운 칸의 거리
unsigned char CGame::SearchNear(int id, int x, int y)
{
	unsigned char min=0xff;

	if(x>0 && check_map[id][x-1][y]<min)min=check_map[id][x-1][y];
	if(x<x_size-1 && check_map[id][x+1][y]<min)min=check_map[id][x+1][y];
	if(y>0 && check_map[id][x][y-1]<min)min=check_map[id][x][y-1];
	if(y<y_size-1 && check_map[id][x][y+1]<min)min=check_map[id][x][y+1];

	return min;
}

//범위 표시(마름모형)
void CGame::SetDiamondArea(int id, int x, int y, int dist_min, int dist_max, int value)
{
	for(int i=dist_min; i<=dist_max; i++)
	{
		if(i==0)
			SetCheckMap(id, x, y, value);
		else
		{
			for(int j=0; j<i; j++)SetCheckMap(id, x-(i-j), y+j, value);	//좌
			for(int j=0; j<i; j++)SetCheckMap(id, x+(i-j), y-j, value);		//우
			for(int j=0; j<i; j++)SetCheckMap(id, x-j, y-(i-j), value);		//상
			for(int j=0; j<i; j++)SetCheckMap(id, x+j, y+(i-j), value);		//하
		}
	}
}

//범위 표시(정사각형)
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
				SetCheckMap(id, x-i, y+j, value);	//좌
				SetCheckMap(id, x+i, y-j, value);	//우
				SetCheckMap(id, x-j, y-i, value);	//상
				SetCheckMap(id, x+j, y+i, value);	//하
			}
		}
	}
}

//감전
void CGame::TransMission(int id, int x, int y, int value, bool water, bool human)
{
	if(x<0 || x>=x_size || y<0 || y>=y_size || check_map[id][x][y]==value)return;

	//사람->사람, 물->물 로만 흐를 수 있음
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

//스크롤
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

//마우스 왼쪽버튼 클릭시 이벤트
void CGame::LeftClick(int MouseX, int MouseY)
{
	//대화나 질문 이벤트 중에는 대화 넘기기만 가능
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
	
	//정보 보기
	if(click_mode==-1)
	{
		if(state_id>=0)state_id=-1;	//정보 보기 취소
			else if(!fog_map[px][py])
			{
				if(unit_map[px][py]>=0)state_id=unit_map[px][py];		//정보 보기
					else que.Insert(CM_QUESTION, 53, FUNC_MAINMENU);	//메뉴
			}
	}
	//이동할 곳 고르기
	else if(click_mode==1)
	{
		if(check_map[MOVE_TEST_MAP][px][py]<=unit[unit_id].move && (unit_map[px][py]==-1 || unit_map[px][py]==unit_id))UnitMove(unit_id, px, py);
	}
	//행동을 선택
	else if(click_mode==3)
	{
		if(selected>=0)
		{
			//공격 범위 정하기
			if(selected==0)
			{
				SetAttackArea();
			}
			//미지
			else if(selected==1)
			{
				click_mode=5;
			}
			//대기
			else if(selected==5)
			{						
				EndTurn();
			}
			else
			{
				att_code=2000+selected;
				//공격 범위 정하기
				first_check=false;
				InitCheckMap(ATTACK_TEST_MAP);
				SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);

				click_mode=4;
			}
		}
	}
	//미지 선택
	else if(click_mode==5)
	{
		if(selected>=0 && selected2>=0)
		{
			first_check=false;
			InitCheckMap(ATTACK_TEST_MAP);
			att_code=1000+selected*10+selected2;

			//돌풍&빔
			if(selected==0 || selected==10)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);
			//발바람
			else if(selected==1)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 2, 0);
			//불덩이
			else if(selected==2)
			{
				if(selected2==3)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 5, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
			}
			//불바다&강
			else if(selected==3 || selected==5)
			{
				if(selected2<2)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 3, 0);
			}
			//치유
			else if(selected==4)
			{
				if(selected2==3)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 5, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 2, 0);
			}
			//지진
			else if(selected==6)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 0, 0);
			//정화
			else if(selected==7)
			{
				if(selected2<2)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 2, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 3, 0);
			}
			//벼락
			else if(selected==8)
			{
				if(selected2==0)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
					else if(selected2==1)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 3, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 4, 0);;
			}
			//축복
			else if(selected==9)
			{
				if(selected2==0)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 0, 0);
					else if(selected2==1)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 1, 0);
					else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 2, 0);
			}
			//독김&맹독김
			else if(selected==11 || selected==12)
			{
				if(selected2==0)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);
					else if(selected2==1 || selected2==2)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
					else if(selected2==3)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 0, 0);
			}

			click_mode=4;
		}
	}
	//행동 실행
	else if(click_mode==4)
	{
		if(check_map[ATTACK_TEST_MAP][px][py]!=99)Action(px, py);
	}
	//장비
	else if(click_mode==80)
	{
		//장비할 대상 선택
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

		//무기를 장비함
		if(selected>=0 && selected2>=0)
		{
			bool equip_able=true;
			//이전 장비가 소지 목록으로
			if(selected2<13)	//무기의 경우
			{
				if(party.unit[selected].arms>0)party.item[party.unit[selected].arms]++;
			}
			else	//갑옷의 경우
			{
				if(party.unit[selected].ammor!=-1)
				{
					if(party.unit[selected].dp==equipment[party.unit[selected].ammor].pow)
					{
						party.item[party.unit[selected].ammor]++;
					}
					else	//부서진 갑옷은 벗기 불가능
					{
						equip_able=false;
					}
				}
			}

			if(equip_able)
			{
				//장비
				party.unit[selected].Equip(selected2);
				//장비한 것은 소지 목록에서 빠짐
				party.item[selected2]--;
			}
		}
	}
	//파티 설정
	else if(click_mode==81)
	{
		//명령 선택
		int ty=37;
		for(int i=0; i<3; i++)
		{
			if(MouseX>445 && MouseX<=605 && MouseY>ty && MouseY<=ty+20)
			{
				selected2=i;

				//명령에 따른 초기 설정
				if(i==0)	//순서 바꾸기
				{
					selected3=-1;

					for(int j=0; j<PARTY_MAX; j++)
					{
						if(j==0)able_unit[j]=false;
							else able_unit[j]=true;
					}
				}
				else if(i==1)	//해고
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
				else if(i==2)	//장비해체
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
			//명령 실행
			if(selected2==0)	//순서 바꾸기
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
			else if(selected2==1)	//해고
			{
				party.unit[selected].life=false;
				PartySort();

				//가능, 불가능 다시 설정
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
			else if(selected2==2)	//장비해체
			{
				//이전 장비가 소지 목록으로
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
	//상품 구입
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
	//학교
	else if(click_mode==100)
	{
		if(selected>=0 && party.money>=select_price)
		{
			party.unit[selected].mizi_level++;
			if(party.unit[selected].mizi_level>=4)able_unit[selected]=false;
			party.money-=select_price;
		}
	}
	//용병 고용
	else if(click_mode==110)
	{
		if(selected>=0)
		{
			if(party.money>=select_price && !party.unit[PARTY_MAX-1].life)	//돈이 충분하고 빈 자리가 있으면
			{
				for(int i=1; i<PARTY_MAX; i++)
					if(!party.unit[i].life)	//파티에 추가
					{
						party.unit[i].SetUnit(0, 0, selected, 0);
						party.money-=select_price;
						break;
					}
			}
		}
	}
	//파티에서 빼고 다른 유닛 넣기
	else if(click_mode==111)
	{
		if(selected>=0)
		{
			party.unit[selected].SetUnit(0, 0, selected2, 0);

			escape=1;
			map_id=0;
		}
	}
	//수리점
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

//마우스 오른쪽버튼 클릭시 이벤트
void CGame::RightClick(int MouseX, int MouseY)
{	
	if(question)			//질문 상태에선 '부정'
	{
		answer=false;
		que.Insert(CM_ANSWER);
		return;
	}
	else if(dialog)return;	//대화 상태에선 사용 불가능

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
		//유닛을 원위치로
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
	//AI용 변수
	static int target_id;
	static int target_x, target_y;
	static int act_3;		//click_mode 3일 때 할 행동

	if(que.GetMax()==0)
	{
		if(click_mode==0)
		{
			//시작 이벤트
			bool moveable=false;
			for(int i=0; i<UNIT_MAX; i++)
				if(unit[i].life && unit[i].wait>=100)
				{
					//유닛 선택
					unit_id=i;
					unit_x=unit[unit_id].x;
					unit_y=unit[unit_id].y;
					SearchRoad(MOVE_TEST_MAP, unit_x, unit_y, unit[unit_id].move_type, unit[unit_id].team);

					//플레이어 조종인지 AI인지 판별
					if(unit[unit_id].team==0)
					{
						sw_ai=false;
					}
					else
					{
						AI_thinking=0;	//AI 생각 시간
						sw_ai=true;
					}

					Focus(unit_x, unit_y, que.que_time+1);
					moveable=true;
					
					//독&축복의 효과					
					if(unit[unit_id].state==1)		//독
					{
						if(unit[unit_id].hp>1)Damage(unit_x, unit_y, 1, 7);
					}
					else if(unit[unit_id].state==2)	//맹독
					{
						Damage(unit_x, unit_y, 1, 7);
						if(unit[unit_id].hp==0)break;	//사망시
					}
					else if(unit[unit_id].state==3)	//축복
					{
						Damage(unit_x, unit_y, -1, 3);
					}
					else if(unit[unit_id].state==4)	//레벨 높은 축복
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
		//AI의 조종
		////////////////////////////////////////////////////////////////
		if(sw_ai)
		{
			static bool do_anything;

			///////////////////////////////////////////////////////
			//AI의 이동
			if(click_mode==1)
			{
				if(AI_thinking==0)
				{
					//아군의 위치 탐색
					unsigned char best=0xff;	//가장 가까운 거리
					int target_id1=-1;			//가장 가까운 유닛
					int death=999;				//가장 적은 HP
					int target_id2=-1;			//가장 HP가 적은 유닛

					//가장 가까운 유닛과 가장 많이 다친 유닛 찾기
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
						//가장 많이 다친 유닛이 한번에 이동 가능한 곳에 있으면 공격
						if(SearchNear(MOVE_TEST_MAP, unit[target_id2].x, unit[target_id2].y)<=unit[unit_id].move)
						{
							target_id=target_id2;
							do_anything=true;
						}
						//아니면 가장 가까운 적이 있는 곳 까지 이동
						else if(target_id1>=0)
						{
							int sight[]={0,6,6,6,6,6,0,0,0,99,99,15,15,10,20,0,99};	//시야: 적이 이 범위 안에 들어오면 접근
							int my_sight;
							if(map_id<6)my_sight=99;	//월드맵 전투에선 무조건 접근
								else my_sight=sight[unit[unit_id].caracter];
							if(SearchNear(MOVE_TEST_MAP, unit[target_id1].x, unit[target_id1].y)<=my_sight)
							{
								target_id=target_id1;
								do_anything=true;
							}
						}
					}

					//목적 유닛으로 이동
					if(do_anything)
					{
						test=target_id;
						SearchRoad(DISTANCE_MAP, unit[target_id].x, unit[target_id].y, -2, unit[unit_id].team);

						int move_range, least_range;
						bool move_end=false;

						//무기에 따른 공격 범위
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

						//가장 최적의 장소로 이동
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

							//딱 맞는 숫자가 없으면 숫자를 늘려 봄
							if(!move_end)
							{
								//2칸 이상 공격 할 수 있는 적은 가장 먼거리를 노리다 안 되면 거리를 좁혀봄
								if(old_move_range>1)
								{
									move_range--;
									//다 검색해 봐도 없을 때
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

								//100칸 거리도 없으면 그만 둠
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
			//AI의 행동
			else if(click_mode==3)
			{
				if(AI_thinking==0)
				{
					//공격을 시도
					InitCheckMap(ATTACK_TEST_MAP);

					if(unit[unit_id].arms==3 || unit[unit_id].arms==4)	//장검&회전검
					{
						SetSquareArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);
					}
					else if(unit[unit_id].arms==5 || unit[unit_id].arms==6)	//창
					{
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x-1, unit[unit_id].y, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x-2, unit[unit_id].y, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x+1, unit[unit_id].y, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x+2, unit[unit_id].y, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y-1, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y-2, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y+1, 0);
						SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y+2, 0);
						//긴 창
						if(unit[unit_id].arms==6)
						{
							SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x-3, unit[unit_id].y, 0);
							SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x+3, unit[unit_id].y, 0);
							SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y-3, 0);
							SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y+3, 0);
						}
					}
					else if(unit[unit_id].arms==7 || unit[unit_id].arms==8)//활
					{
						SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 2, 2, 0);
					}
					else if(unit[unit_id].arms==9)//긴 활
					{
						SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 2, 3, 0);
					}
					else if(unit[unit_id].arms==10)	//방갈
					{
						SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
					}
					//기타 1칸 짜리
					else
					{
						SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);
					}

					if(do_anything)
					{
						//목표물이 공격 가능 범위에 있으면 공격
						if(check_map[ATTACK_TEST_MAP][unit[target_id].x][unit[target_id].y]==0)
						{
							act_3=0;
						}
						//아니면 다른 목표를 찾음
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
						//다크퀴즈&휴루시
						if(unit[unit_id].caracter==DARKQUIZ || unit[unit_id].caracter==HERO6)act_3=1;
							else act_3=5;
					}
				}

				if(++AI_thinking>=5 || fog_map[unit[unit_id].x][unit[unit_id].y])
				{
					//할 것 없으면 대기
					if(act_3==5)
					{
						EndTurn();
					}
					//미지
					else if(act_3==1)
					{
						//다크퀴즈
						if(unit[unit_id].caracter==DARKQUIZ)
						{
							AI_thinking=0;
							att_code=1123;
							first_check=false;
							InitCheckMap(ATTACK_TEST_MAP);
							SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0, 0, 0);
							click_mode=4;
						}
						//휴루시
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
					//공격 가능하면 공격
					else if(act_3==0)
					{
						AI_thinking=0;
						SetAttackArea();
					}
				}
			}
			///////////////////////////////////////////////////
			//AI의 공격
			else if(click_mode==4)
			{
				if(act_3==1)
				{
					//맹독김 공격
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
					//빔 공격
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

	//AI가 조종할 때 마우스 위치
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

	//큐에 담긴 명령어 처리
	bool empty=true;	//큐에 명령이 비었는지 검사
	QueData* first_focus=NULL;	//두개의 포커스 명령이 충돌하지 않게 하는 스위치
	QueData* qp;	//지금 처리되는 큐 데이터를 가르키는 포인터
	for(int i=0; i<que.GetMax(); i++)
	{
		qp=que.GetQue(i);

		if(qp->time==que.que_time)
		{
			empty=false;

			////////////////////////////////////
			//촛점 맞추기
			if(qp->command==CM_FOCUS)
			{
				if(first_focus==NULL)
				{
					first_focus=qp;
					sw_focus=true;
					//X좌표 맞추기
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
					//Y좌표 맞추기
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
					//포커스 끝
					if(fx_i==qp->x && fy_i==qp->y)
					{							
						que.Delete(i);
						sw_focus=false;
					}
				}
				//가까운 곳이면 포커스 무시
				else if(abs((first_focus->x)-(qp->x))<320 && abs((first_focus->y)-(qp->y))<240)que.Delete(i);
			}
			////////////////////////////////////
			//이동
			else if(qp->command==CM_MOVE)
			{
				sw_move=true;
				if(!fog_map[unit[qp->id].x][unit[qp->id].y] && unit[qp->id].sx==0 && unit[qp->id].sy==0 && unit[qp->id].caracter!=HANAZUM &&unit[qp->id].caracter!=NEYUM)lib->PlaySound(0);
				//좌
				if(qp->x==4)
				{
					//어둠 속에서 이동은 빠르게
					if(fog_map[unit[qp->id].x][unit[qp->id].y] && fog_map[unit[qp->id].x-1][unit[qp->id].y])
					{
						unit[qp->id].x--;
						que.Delete(i);
					}
					//보통 이동
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
				//우
				else if(qp->x==6)
				{
					//어둠 속에서 이동은 빠르게
					if(fog_map[unit[qp->id].x][unit[qp->id].y] && fog_map[unit[qp->id].x+1][unit[qp->id].y])
					{
						unit[qp->id].x++;
						que.Delete(i);
					}
					//보통 이동
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
				//상
				else if(qp->x==8)
				{
					//어둠 속에서 이동은 빠르게
					if(fog_map[unit[qp->id].x][unit[qp->id].y] && fog_map[unit[qp->id].x][unit[qp->id].y-1])
					{
						unit[qp->id].y--;
						que.Delete(i);
					}
					//보통 이동
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
				//하
				else if(qp->x==2)
				{
					//어둠 속에서 이동은 빠르게
					if(fog_map[unit[qp->id].x][unit[qp->id].y] && fog_map[unit[qp->id].x][unit[qp->id].y+1])
					{
						unit[qp->id].y++;
						que.Delete(i);
					}
					//보통 이동
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
			//이동 끝
			else if(qp->command==CM_MOVEEND)
			{
				//유닛 맵 수정
				unit_map[qp->x][qp->y]=-1;
				unit_map[unit[qp->id].x][unit[qp->id].y]=qp->id;

				if(battle)click_mode=3;
					else click_mode=0;

				Event(EV_MOVE, qp->id, unit[qp->id].x, unit[qp->id].y);
				//종료 이벤트
				unit[unit_id].wait-=check_map[MOVE_TEST_MAP][unit[unit_id].x][unit[unit_id].y]*10;

				//작업 삭제
				sw_move=false;
				que.Delete(i);
			}
			////////////////////////////////////
			//피해 또는 회복
			else if(qp->command==CM_DAMAGE)
			{				
				if(qp->x>0)
				{
					//떨림
					int tremble=(qp->y)%4;
					if(tremble==0 || tremble==3)unit[qp->id].sx-=4;
						else if(tremble==1 || tremble==2)unit[qp->id].sx+=4;
				}

				//시간 흐르기
				qp->y++;
				if(qp->y>=20)
				{
					//사망
					if(unit[qp->id].hp==0 && unit[qp->id].caracter!=NEYUM/*불사 캐릭터*/)
					{
						//적군이면 돈 획득
						if(unit[qp->id].team!=0)party.money+=have_money[unit[qp->id].caracter];
						//아군이면 파티에서 빠짐
						else party.unit[unit[qp->id].party_id].life=false;

						if(qp->id==1)	//보스급 이벤트
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
						else	//일반
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
			//사망
			else if(qp->command==CM_DIE)
			{
				qp->y++;
				//끝
				if(qp->y>=50)
				{
					unit[qp->id].sy-=10;
					if(qp->y>=100)
					{
						unit[qp->id].soul=false;

						if(qp->id==0)escape=2;	//게임 오버
							else if((qp->id==1 && boss) || Eliminate())	//클리어
							{
								//일반 전투
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
			//워프
			else if(qp->command==CM_WARP)
			{
				//시작
				if(qp->y==0)
				{
					unit[qp->id].life=false;
					unit[qp->id].sy=-400;
					qp->y=1;
				}
				unit[qp->id].sy+=8;
				//끝
				if(unit[qp->id].sy>=0)
				{
					unit[qp->id].life=true;
					que.Delete(i);
				}
			}
			////////////////////////////////////
			//대화
			else if(qp->command==CM_DIALOG)
			{
				//대화 상태를 켜고 대화 시작과 얼굴 설정
				dialog=true;
				dialog_no=qp->id;
				dialog_end=qp->x;
				answer_func=qp->y;				
				click_mode=-1;

				que.Delete(i);
			}
			////////////////////////////////////
			//다음 대화
			else if(qp->command==CM_NEXT_DIALOG)
			{
				dialog_no++;
				if(dialog_no==45)Focus(4,3);
					else if(dialog_no==46)Focus(25,4);
					else if(dialog_no==47)Focus(21,16);
					else if(dialog_no==48)Focus(27,18);
				//대화 끝내기
				if(dialog_no>dialog_end)
				{
					dialog=false;
					click_mode=0;
					Function(answer_func);
				}

				que.Delete(i);
			}
			////////////////////////////////////
			//질문
			else if(qp->command==CM_QUESTION)
			{
				//대화 상태를 켜고 대화 시작과 얼굴 설정
				question=true;
				dialog_no=qp->id;
				answer_func=qp->x;
				answer_func2=qp->y;

				que.Delete(i);
			}
			////////////////////////////////////
			//대답
			else if(qp->command==CM_ANSWER)
			{
				question=false;
				if(answer)Function(answer_func);
					else Function(answer_func2);
				que.Delete(i);
			}
			////////////////////////////////////
			//화면 흔들기
			else if(qp->command==CM_SHAKE)
			{
				int tmp=(qp->y)%4;
				if(tmp==0 || tmp==3)fx_i-=4;
					else if(tmp==1 || tmp==2)fx_i+=4;
				if(++(qp->y)>=20)que.Delete(i);
			}

			//디버그
			/*char* command_name[]={"포커스","이동","이동끝","피해","치료","사망"};
			for(int i=0; i<que.GetMax(); i++)
				fprintf(fp2, "%s %d %d %d %d\n", command_name[que.GetQue(i)->command], que.GetQue(i)->id, que.GetQue(i)->x, que.GetQue(i)->y, que.GetQue(i)->time);
			fprintf(fp2, "------------------------------------------------------\n");*/
			//Sleep(100);
		}
	}

	//지금 처리할 명령어가 없어 한 프로세스를 끝냄
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
	//시작 이벤트
	if(type==EV_START)
	{
		//프롤로그
		if(map_id==0)
		{
			if(!event_switch[SW_PROLOGUE])
			{
				que.Insert(CM_DIALOG, 0, 6);
				event_switch[SW_PROLOGUE]=true;
			}
		}
		//휴가루 마을
		else if(map_id==1)
		{
			if(!event_switch[SW_HYUGARU])
			{
				que.Insert(CM_DIALOG, 44, 51);
				event_switch[SW_HYUGARU]=true;
			}
		}
		//가미얼 마을
		else if(map_id==7)
		{
			if(!event_switch[SW_GAMIUL])
			{
				que.Insert(CM_DIALOG, 13, 18);
				event_switch[SW_GAMIUL]=true;
			}
		}
		//미버가 마을
		else if(map_id==8)
		{
			if(!event_switch[SW_MIBUGA])
			{
				que.Insert(CM_DIALOG, 24, 28);
				event_switch[SW_MIBUGA]=true;
			}
		}
		//시크벨 마을
		else if(map_id==9)
		{
			if(!event_switch[SW_SIKUBEL])
			{
				que.Insert(CM_DIALOG, 19, 23);
				event_switch[SW_SIKUBEL]=true;
			}
		}
		//루벨버 마을
		else if(map_id==10)
		{
			if(!event_switch[SW_RUBELBU])
			{
				que.Insert(CM_DIALOG, 29, 34);
				event_switch[SW_RUBELBU]=true;
			}
		}
		//다크퀴즈 성 1층
		else if(map_id==11)
		{
			if(!event_switch[SW_HYURUSI])
			{
				que.Insert(CM_DIALOG, 35, 43);
				event_switch[SW_HYURUSI]=true;
			}
		}
		//다크퀴즈 성 2층
		else if(map_id==12)
		{
			que.Insert(CM_DIALOG, 103, 105, FUNC_QUIZ1);
		}
		//워프 애니메이션
		else if(map_id==13 || map_id==14)
		{
			que.Insert(CM_WARP, 0);
		}
	}
	////////////////////////////////////////
	//이동 후 이벤트
	else if(type==EV_MOVE)
	{
		//이벤트
		if(map_id==0)	//월드맵
		{
			bool move_event=false;
			//특정 장소로 이동
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
		else if(map_id==13)	//메인 메뉴
		{
			//새 게임
			if(x==7 && y==4)
			{
				escape=3;
				map_id=0;
			}
			//저장
			else if(x==4 && y==7)
			{
				save_mode=true;
				escape=1;
				map_id=14;
				LoadInfo();
			}
			//장비
			else if(x==5 && y==9)
			{
				selected=-1;
				click_mode=80;
			}
			//파티 설정
			else if(x==9 && y==9)
			{
				selected2=-1;
				for(int i=0; i<PARTY_MAX; i++)able_unit[i]=true;
				click_mode=81;
			}
			//돌아가기
			else if(x==7 && y==7)
			{
				if(!main_menu)Load(0);
			}
			//불러오기
			else if(x==10 && y==7)
			{
				save_mode=false;				
				escape=1;
				map_id=14;
				LoadInfo();
			}
			//나가기
			else if(x==7 && y==10)
			{
				escape=99;
			}
			//비밀 알선소
			else if(x==5 && y==5)
			{
				shop[0]=BANDIT;
				shop[1]=NEYUM;
				shop[2]=HERO6;
				shop[3]=DARKQUIZ;
				click_mode=110;
			}
			//비밀 상점
			else if(x==9 && y==5)
			{
				shop[0]=10;
				shop[1]=11;
				shop[2]=12;
				shop[3]=16;
				click_mode=90;
			}
		}
		else if(map_id==14)	//메인 메뉴
		{
			//돌아가기
			if(x==3 && y==6)
			{
				escape=1;
				map_id=13;
			}
			//파일
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
			//휴가루 마을
			if(map_id==1)
			{
				//민가
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
				//연못의 집
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
				//무기점
				else if(x==4 && y==3)
				{
					shop[0]=1;
					shop[1]=2;
					shop[2]=shop[3]=-1;
					click_mode=90;
				}
				//수리점
				else if(x==25 && y==4)
				{
					for(int i=0; i<PARTY_MAX; i++)
						if(party.unit[i].ammor>=0 && equipment[party.unit[i].ammor].pow>party.unit[i].dp)able_unit[i]=true;
							else able_unit[i]=false;

					click_mode=120;
				}
				//학교
				else if(x==21 && y==16)
				{
					for(int i=0; i<PARTY_MAX; i++)
						if(party.unit[i].life && party.unit[i].mizi_type>=0 && party.unit[i].mizi_level<4)able_unit[i]=true;
							else able_unit[i]=false;
						
					click_mode=100;
				}
				//알선소
				else if(x==27 && y==18)
				{
					shop[0]=SOLDIER1;
					shop[1]=SOLDIER2;
					shop[2]=SOLDIER3;
					shop[3]=HANAZUM;
					click_mode=110;
				}
			}
			//가미얼 마을
			else if(map_id==7)
			{
				if(!battle)
				{
					//무기점
					if(x==24 && y==8)
					{
						shop[0]=3;
						shop[1]=4;
						shop[2]=shop[3]=-1;
						click_mode=90;
					}
					//수리점
					else if(x==7 && y==16)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].ammor>=0 && equipment[party.unit[i].ammor].pow>party.unit[i].dp)able_unit[i]=true;
								else able_unit[i]=false;

						click_mode=120;
					}
					//학교
					else if(x==4 && y==11)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].life && party.unit[i].mizi_type>=0 && party.unit[i].mizi_level<4)able_unit[i]=true;
								else able_unit[i]=false;
							
						click_mode=100;
					}
					//알선소
					else if(x==27 && y==16)
					{
						shop[0]=SOLDIER1;
						shop[1]=SOLDIER2;
						shop[2]=SOLDIER3;
						shop[3]=MAEQO;
						click_mode=110;
					}
					//민가
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
			//미버가 마을
			else if(map_id==8)
			{
				if(!battle)
				{
					//무기점
					if(x==13 && y==5)
					{
						shop[0]=5;
						shop[1]=6;
						shop[2]=shop[3]=-1;
						click_mode=90;
					}
					//수리점
					else if(x==20 && y==6)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].ammor>=0 && equipment[party.unit[i].ammor].pow>party.unit[i].dp)able_unit[i]=true;
								else able_unit[i]=false;

						click_mode=120;
					}
					//학교
					else if(x==4 && y==7)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].life && party.unit[i].mizi_type>=0 && party.unit[i].mizi_level<4)able_unit[i]=true;
								else able_unit[i]=false;
							
						click_mode=100;
					}
					//알선소
					else if(x==8 && y==5)
					{
						shop[0]=SOLDIER1;
						shop[1]=SOLDIER2;
						shop[2]=SOLDIER3;
						shop[3]=BIGROCK;
						click_mode=110;
					}
					//민가
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
			//시크벨 마을
			else if(map_id==9)
			{
				if(!battle)
				{
					//무기점
					if(x==2 && y==8)
					{
						shop[0]=7;
						shop[1]=8;
						shop[2]=9;
						shop[3]=-1;
						click_mode=90;
					}
					//수리점
					else if(x==8 && y==15)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].ammor>=0 && equipment[party.unit[i].ammor].pow>party.unit[i].dp)able_unit[i]=true;
								else able_unit[i]=false;

						click_mode=120;
					}
					//학교
					else if(x==21 && y==6)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].life && party.unit[i].mizi_type>=0 && party.unit[i].mizi_level<4)able_unit[i]=true;
								else able_unit[i]=false;
							
						click_mode=100;
					}
					//알선소
					else if(x==10 && y==6)
					{
						shop[0]=SOLDIER1;
						shop[1]=SOLDIER2;
						shop[2]=SOLDIER3;
						shop[3]=BANG;
						click_mode=110;
					}
					//민가
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
			//루벨버 마을
			else if(map_id==10)
			{
				if(!battle)
				{
					//무기점
					if(x==20 && y==17)
					{
						shop[0]=13;
						shop[1]=14;
						shop[2]=15;
						shop[3]=-1;
						click_mode=90;
					}
					//수리점
					else if(x==30 && y==17)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].ammor>=0 && equipment[party.unit[i].ammor].pow>party.unit[i].dp)able_unit[i]=true;
								else able_unit[i]=false;

						click_mode=120;
					}
					//학교
					else if(x==18 && y==3)
					{
						for(int i=0; i<PARTY_MAX; i++)
							if(party.unit[i].life && party.unit[i].mizi_type>=0 && party.unit[i].mizi_level<4)able_unit[i]=true;
								else able_unit[i]=false;
							
						click_mode=100;
					}
					//알선소
					else if(x==3 && y==26)
					{
						shop[0]=SOLDIER1;
						shop[1]=SOLDIER2;
						shop[2]=SOLDIER3;
						shop[3]=KUOT;
						click_mode=110;
					}
					//민가
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

			//나가기
			if(id==0)
			{				
				if(x==0 || y==0 || x==(x_size-1) || y==(y_size-1))
				{
					if(battle)que.Insert(CM_QUESTION, 52, FUNC_WORLDMAP);
						else Function(FUNC_WORLDMAP);
				}
			}
		}

		if(escape)return;	//탈출한 경우 검사 안 함
	}
	////////////////////////////////////////
	//행동 종료 후 이벤트
	else if(type==EV_ACTION_END)
	{
		EraseFog();

		if(map_id==10)		//숲 마을
		{
			//나무들의 움직임
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

		//구름의 움직임
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
	//맵 종료 후 이벤트
	else if(type==EV_END)
	{
		//전투후 파티 데이터 갱신
		if(battle)
		{
			for(int i=0; i<UNIT_MAX; i++)
			{
				if(unit[i].life && unit[i].team==0)
				{
					//갑옷 데이터 옮기기
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
	//크기 알아내기
	if(fp=fopen("hugaru0.sav","rb"))
	{
		char buffer;
		while(fread(&buffer, sizeof(char), 1, fp))file_size++;
		fclose(fp);
	}
	//데이터 읽기
	char* game_data=new char[file_size];
	if(fp=fopen("hugaru0.sav","rb"))
	{
		fread(game_data, sizeof(char), file_size, fp);
		fclose(fp);
	}
	//데이터 쓰기
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
		//세이브 파일 식별
		char title[13];
		fread(title, sizeof(char), 12, fp);
		title[12]=NULL;
		if(strcmp(title, "HUGARU01SAVE")==0)
		{
			//불러오기 전에 정리
			que.Destroy();

			//데이터 읽어 들이기
			fread(this, sizeof(CGame), 1, fp);
			fclose(fp);

			//불러온 후 설정
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
			//세이브 파일 식별
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
	//AI의 마우스 커서
	if(sw_ai)
	{
		MouseX=AI_MouseX;
		MouseY=AI_MouseY;
	}

	int fx=fx_i/40, fy=fy_i/40;
	int mx=(MouseX+fx_i)/40;
	int my=(MouseY+fy_i)/40;

	//1,2층
	for(int j=fy; j<Min(y_size, fy+13); j++)
		for(int i=fx; i<Min(x_size, fx+17); i++)
		{
			int tile_id1;
			int tile_x=i*40-fx_i;
			int tile_y=j*40-fy_i;

			//1층 표현(공격이나 이동시 고려)
			if(click_mode==1)	//이동시 이동불능 지역은 어둡게 표현
			{				
				if(check_map[MOVE_TEST_MAP][i][j]>unit[unit_id].move)tile_id1=d_tile;
					else tile_id1=0;
			}
			else if(click_mode==4)	//공격시 공격 가능한 지역은 붉게 표현
			{
				if(check_map[ATTACK_TEST_MAP][i][j]!=99)tile_id1=r_tile;
					else tile_id1=0;
			}
			else tile_id1=0;

			if(fog_map[i][j])lib->Image(tile_x, tile_y, tile_id1, 7);
				else lib->Image(tile_x, tile_y, tile_id1, map[i][j][0]);
			//2층
			if(map[i][j][1]>=0 && !fog_map[i][j])lib->Image(tile_x, tile_y, 1, map[i][j][1]);
		}

	//스프라이트
	for(int i=0; i<UNIT_MAX; i++)
		if(unit[i].life)
		{
			//검은 안개
			if(fog_map[unit[i].x][unit[i].y])continue;
			//선택 표시
			if(click_mode==1 && unit_id==i)lib->Image(unit[i].x*40+unit[i].sx-fx_i, unit[i].y*40+unit[i].sy-fy_i, imgno.cursor+2);
			//유닛 출력
			int unit_img;
			if(unit[i].state==0 || unit[i].state>2)
			{
				if(unit[i].team==0)unit_img=imgno.unit;
					else unit_img=imgno.unit+1;
			}
			else	//중독
			{
				if(unit[i].team==0)unit_img=imgno.unit+2;
					else unit_img=imgno.unit+3;
			}
			lib->Image(unit[i].x*40+unit[i].sx-fx_i, unit[i].y*40+unit[i].sy-fy_i, unit_img, unit[i].caracter);
		}
		//혼
		else if(unit[i].soul)lib->Image(unit[i].x*40+unit[i].sx-fx_i, unit[i].y*40+unit[i].sy-fy_i, imgno.animation+2);

	//3층&애니메이션
	for(int j=fy; j<Min(y_size, fy+13); j++)
		for(int i=fx; i<Min(x_size, fx+17); i++)
		{			
			int tile_x=i*40-fx_i;
			int tile_y=j*40-fy_i;

			//3층 타일
			if(map[i][j][2]>=0 && !fog_map[i][j])lib->Image(tile_x, tile_y, 1, map[i][j][2]+6);
			//애니메이션
			if(check_map[ANIMATION_MAP][i][j]!=99)
			{
				if(!fog_map[i][j])
				{
					//물&독
					if(check_map[ANIMATION_MAP][i][j]==5 || check_map[ANIMATION_MAP][i][j]==7)
					{
						lib->Image(tile_x, tile_y, imgno.animation+check_map[ANIMATION_MAP][i][j], (20-check_map[DELAY_MAP][i][j])/4);
					}
					//충격
					else if(check_map[ANIMATION_MAP][i][j]==1)
					{
						if(check_map[DELAY_MAP][i][j]>15)lib->Image(tile_x, tile_y, imgno.animation+check_map[ANIMATION_MAP][i][j], 20-check_map[DELAY_MAP][i][j]);
					}
					//빛
					else if(check_map[ANIMATION_MAP][i][j]==8)
					{
						if(check_map[DELAY_MAP][i][j]>10)lib->Image(tile_x, tile_y, imgno.animation+check_map[ANIMATION_MAP][i][j], 20-check_map[DELAY_MAP][i][j]);
							else lib->Image(tile_x, tile_y, imgno.animation+check_map[ANIMATION_MAP][i][j], 10);
					}
					//자동 애니메이션
					else
					{
						lib->Image(tile_x, tile_y, imgno.animation+check_map[ANIMATION_MAP][i][j]);
					}
				}
			}
			//공격 범위 표시
			if(click_mode==4 && check_map[ATTACK_TEST_MAP][mx][my]!=99 && check_map[DAMAGE_TEST_MAP][i][j]==0 && first_check)lib->Image(tile_x, tile_y, imgno.cursor+2);
		}
	//애니메이션 지연 시간 감소
	for(int ay=0; ay<y_size; ay++)
		for(int ax=0; ax<x_size; ax++)
			if(check_map[DELAY_MAP][ax][ay]>0)
			{
				check_map[DELAY_MAP][ax][ay]--;
				if(check_map[DELAY_MAP][ax][ay]<=0)check_map[ANIMATION_MAP][ax][ay]=99;
			}

	//선택지점
	if((click_mode==1 || click_mode==3) && !sw_move)
	{		
		int px=(mx)*40-fx_i;
		int py=(my)*40-fy_i;
		if(!fog_map[mx][my])lib->Image(px, py, imgno.cursor+1);
	}
	//공격지점과 공격범위
	else if(click_mode==4)
	{
		SetDamageArea(mx, my);
	}

	//상태
	if(battle)
	{
		int px=(MouseX+fx_i)/40;
		int py=(MouseY+fy_i)/40;
		if(!fog_map[px][py] && unit_map[px][py]>=0)
		{
			//상태창
			CUnit* unitp=&unit[unit_map[px][py]];
			int state_x, state_y;
			if(MouseX>560)state_x=MouseX-65;
				else state_x=MouseX+15;
			if(MouseY>440)state_y=MouseY-45;
				else state_y=MouseY;
			lib->Image(state_x-2, state_y-2, imgno.panel+1);
			//체력
			lib->Image(state_x, state_y, imgno.state_icon, 0);
			n_hp.PutNumber(state_x+12, state_y+2, unitp->hp);
			//갑옷
			if(unitp->ammor>=0)
			{
				lib->Image(state_x+30, state_y, imgno.state_icon, 1);
				n_ammor.PutNumber(state_x+42, state_y+2, unitp->dp);
			}
			//무기
			lib->Image(state_x, state_y+10, imgno.state_icon, 3+equipment[unitp->arms].type);
			n_arms.PutNumber(state_x+12, state_y+12, unitp->str);
			//미지
			if(unitp->mizi_type>=0)
			{
				lib->Image(state_x+30, state_y+10, imgno.state_icon, 7+unitp->mizi_type);
				n_mizi.PutNumber(state_x+42, state_y+12, unitp->mizi_level);				
			}
			//이동
			lib->Image(state_x, state_y+20, imgno.state_icon, 14);
			n_move.PutNumber(state_x+12, state_y+22, unitp->move);
			//상태
			if(unitp->state!=0)
			{
				if(unitp->state==1 || unitp->state==2)lib->Image(state_x+30, state_y+20, imgno.state_icon, 13);
					else if(unitp->state==3 || unitp->state==4)lib->Image(state_x+40, state_y+30, imgno.state_icon, 12);
			}
			//대기
			lib->Image(state_x, state_y+30, imgno.state_icon, 15);
			n_wait.PutNumber(state_x+12, state_y+32, unitp->wait);
			//소속
			lib->Image(state_x+50, state_y+30, imgno.state_icon, 16+(int)unitp->team);
		}
	}

	//자세한 정보
	if(click_mode==-1)
	{
		if(state_id>=0)
		{
			CUnit* unit_p;
			if(battle)unit_p=&unit[state_id];
				else unit_p=&party.unit[state_id];
			//유닛 모습
			lib->Image(200, 200, imgno.panel+3);
			if(unit_p->team==0)lib->Image(220, 220, imgno.unit, unit_p->caracter);
				else lib->Image(220, 220, imgno.unit+1, unit_p->caracter);
			//정보
			lib->Image(280, 160, imgno.panel+2);
			char info_text[512], tmp_text[128];
			RECT info_rect;
			SetRect(&info_rect, 282, 162, 438, 318);
			//이름, 체력, 무기
			sprintf(info_text, "%s\n체력: %d/%d\n무기: %s(힘: %d)", cara[unit_p->caracter].name, unit_p->hp, unit_p->hp_max, equipment[unit_p->arms].name, unit_p->str);
			//갑옷
			if(unit_p->ammor==-1)strcat(info_text, "\n갑옷: 없음");
			else
			{
				sprintf(tmp_text, "\n갑옷: %s(%d/%d)", equipment[unit_p->ammor].name, unit_p->dp, equipment[unit_p->ammor].pow);
				strcat(info_text, tmp_text);
			}
			//미지
			if(unit_p->mizi_type==-1)strcat(info_text, "\n미지: 사용 못 함");
			else
			{
				sprintf(tmp_text, "\n미지: %s(레벨: %d)", mizi_name[unit_p->mizi_type], unit_p->mizi_level);
				strcat(info_text, tmp_text);
			}
			//이동력, 상태, 대기, 소지금
			char* state_name[]={"정상","독","맹독","축복","강한 축복"};
			sprintf(tmp_text, "\n이동력: %d\n상태: %s\n대기: %d\n소지금: ",unit_p->move, state_name[unit_p->state], unit_p->wait);
			strcat(info_text, tmp_text);
			if(unit_p->team==0)sprintf(tmp_text, "%d", party.money);
				else sprintf(tmp_text, "%d", have_money[unit_p->caracter]);
			strcat(info_text,tmp_text);
			//출력
			lib->PutText(&info_rect, info_text, JColor(0,0,0));
		}
	}
	//명령판
	else if(click_mode==3)
	{
		if(!fog_map[unit[unit_id].x][unit[unit_id].y] && !question)
		{
			panel_x=(unit[unit_id].x+1)*40-fx_i;
			panel_y=unit[unit_id].y*40-fy_i;
			if(unit[unit_id].x>=(x_size-2))panel_x-=100;
			if(unit[unit_id].y>=(y_size-1))panel_y-=40;

			lib->Image(panel_x, panel_y, imgno.panel);
			char* str_command[]={"공격","미지","벌목","다리","치료","대기"};
			//가능한 명령 표시
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
	//미지목록
	else if(click_mode==5)
	{
		panel_x=240;
		panel_y=160;

		lib->Image(panel_x, panel_y, imgno.panel+2);
		char* mizi_list[]={"돌풍","발바람","불덩이","불바다","치유","강","지진","정화","벼락","축복","빔","독김","맹독김"};
		//가능한 명령 표시
		selected=selected2=-1;
		for(int i=0; i<4; i++)
			if(unit[unit_id].skill[i]>=0)
			{
				//미지 이름
				if(MouseX>=panel_x && MouseX<panel_x+160 && MouseY>=panel_y+(i*20) && MouseY<panel_y+(i*20)+20)
				{
					lib->PutText(panel_x+2, panel_y+2+i*20, mizi_list[unit[unit_id].skill[i]], YELLOW);
					selected=unit[unit_id].skill[i];
				}
				else
					lib->PutText(panel_x+2, panel_y+2+i*20, mizi_list[unit[unit_id].skill[i]], CUSTOM_BLUE);
				//미지 레벨
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
		//미지 도움말
		if(selected>=0)
		{
			char* mizi_help_text[]=
			{
				"어둠의 안개를 없앰",
				"이동력을 증가",
				"한명을 불로 공격",
				"여러명을 불로 공격",
				"체력을 회복",
				"땅을 물로 바꿈",
				"지진으로  공격",
				"정상 상태로 만듦\n(축복도 없앰)",
				"벼락으로 적을 공격\n생물끼리 감전\n물에 누전",
				"체력을 조금씩 회복",
				"빛으로 적을 공격",
				"상대를 중독 시킴",
				"맹독에 중독 시킴\n사망까지 이르는 독"
			};
			RECT mizi_help_rect;
			SetRect(&mizi_help_rect, 242, 260, 398, 318);
			lib->PutText(&mizi_help_rect, mizi_help_text[selected], JColor(0,0,0));
		}
	}
	//장비
	else if(click_mode==80)
	{
		panel_x=160;
		panel_y=120;

		lib->Image(445, 35, imgno.panel+7);
		lib->Image(33, 111, imgno.panel+6);
		lib->Image(33, 195, imgno.panel+6);

		//장착할 캐릭터
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
		//소지 목록
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
		//현재 상태
		if(selected>=0)
		{
			char help_text[256];
			if(party.unit[selected].ammor==-1)sprintf(help_text, "무기: %s\n갑옷: 없음\n공격력: %d", equipment[party.unit[selected].arms].name, party.unit[selected].str);
			else
			{
				//손상된 갑옷을 입고는 장비교체 불가능
				if(party.unit[selected].dp<equipment[party.unit[selected].ammor].pow) sprintf(help_text, "무기: %s\n갑옷: %s(손상된 갑옷이라 장비 교체 불가능)\n공격력: %d\n방어력: %d/%d", equipment[party.unit[selected].arms].name, equipment[party.unit[selected].ammor].name, party.unit[selected].str, party.unit[selected].dp, equipment[party.unit[selected].ammor].pow);
					else sprintf(help_text, "무기: %s\n갑옷: %s\n공격력: %d\n방어력: %d/%d", equipment[party.unit[selected].arms].name, equipment[party.unit[selected].ammor].name, party.unit[selected].str, party.unit[selected].dp, equipment[party.unit[selected].ammor].pow);
			}

			RECT help_rect;
			SetRect(&help_rect, 35, 197, 435, 273);
			lib->PutText(&help_rect, help_text, JColor(0,0,0));
		}
	}
	//파티 설정
	else if(click_mode==81)
	{
		panel_x=160;
		panel_y=120;

		lib->Image(445, 35, imgno.panel+7);
		lib->Image(33, 111, imgno.panel+6);
		lib->Image(33, 195, imgno.panel+6);

		//대상 캐릭터
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

				//순서바꾸기 대상
				if(selected2==0 &&selected3==j)lib->Image(cx, cy, imgno.cursor+2);
			}
		}
		//명령들
		int ty=37;
		char* party_command[3]={"순서 바꾸기", "해고", "장비 해체"};
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
		//현재 상태
		if(selected>=0)
		{
			char help_text[256];
			if(party.unit[selected].ammor==-1)sprintf(help_text, "무기: %s\n갑옷: 없음\n공격력: %d", equipment[party.unit[selected].arms].name, party.unit[selected].str);
				else sprintf(help_text, "무기: %s\n갑옷: %s\n공격력: %d\n방어력: %d/%d", equipment[party.unit[selected].arms].name, equipment[party.unit[selected].ammor].name, party.unit[selected].str, party.unit[selected].dp, equipment[party.unit[selected].ammor].pow);

			RECT help_rect;
			SetRect(&help_rect, 35, 197, 435, 273);
			lib->PutText(&help_rect, help_text, JColor(0,0,0));
		}
	}
	//상품 목록
	else if(click_mode==90)
	{
		panel_x=160;
		panel_y=120;

		lib->Image(panel_x, panel_y, imgno.panel+2);
		lib->Image(325, 35, imgno.panel+7);
		lib->Image(118, 358, imgno.panel+6);

		//글자들
		selected=-1;
		for(int i=0; i<4; i++)
			if(shop[i]>=0)
			{
				//이름과 가격 알아내기
				char price[10];
				sprintf(price, "%d카오", equipment[shop[i]].price);
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
				//상품 이름
				lib->PutText(panel_x+2, panel_y+2+i*20, equipment[shop[i]].name, goods_color);
				lib->PutText(panel_x+80, panel_y+2+i*20, price, goods_color);
			}
		//소지 목록
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
		//도움말
		if(selected>=0)
		{
			char* help_text[]=
			{
				"",
				"공격력+1",
				"공격력+2",
				"공격력+1\n가로 3칸 공격",
				"공격력+1\n주위 8칸 공격",
				"공격력+1\n앞으로 2칸 공격",
				"공격력+1\n앞으로 3칸 공격",
				"공격력+1\n거리 2",
				"공격력+2\n거리 2",
				"공격력+1\n거리 2-3",
				"공격력+0\n거리 1-2",
				"공격력+1\n불 효과",
				"공격력+0\n적을 중독시킴",
				"내구력 5\n방전효과",
				"내구력 10",
				"내구력 20",
				"내구력 99\n방전효과",
			};

			RECT help_rect;
			SetRect(&help_rect, panel_x+2, panel_y+100, panel_x+158, panel_y+140);
			lib->PutText(&help_rect, help_text[selected], JColor(0,0,0));
		}
		//소지금
		char my_money[30];
		sprintf(my_money, "소지금: %d카오", party.money);
		lib->PutText(panel_x+2, panel_y+140, my_money, CUSTOM_BLUE);
		//파티들에게 효과
		if(selected>=0)
		{
			//장착 가능한 캐릭터
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
	//학교
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

		//도움말
		RECT help_rect;
		SetRect(&help_rect, 120, 202, 520, 282);
		if(selected>=0)
		{
			char help_text[128];

			sprintf(help_text, "미지 속성: %s / 미지 레벨: %d\n다음 레벨이 되기 위해 필요한 학습료: %d\n소지금: %d카오", mizi_name[party.unit[selected].mizi_type], party.unit[selected].mizi_level, mizi_price[party.unit[selected].mizi_level], party.money);
			lib->PutText(&help_rect, help_text, JColor(0, 0, 0));
		}
		else lib->PutText(&help_rect, "미지 등급을 높일 캐릭터를 선택해주세요.", JColor(0, 0, 0));
	}
	//알선소
	else if(click_mode==110)
	{
		panel_x=240;
		panel_y=160;

		lib->Image(panel_x, panel_y, imgno.panel+2);
		selected=-1;
		for(int i=0; i<4; i++)
			if(shop[i]>=0)
			{
				//이름과 고용료
				int pay[]={0,200,200,200,200,300,100,80,80,120,50,100,80,60,120,300,100};
				char price[10];
				sprintf(price, "%d카오", pay[shop[i]]);
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
		//현재 용사단
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
		//소지금
		char my_money[30];
		sprintf(my_money, "소지금: %d카오", party.money);
		lib->PutText(242, 300, my_money, CUSTOM_BLUE);
	}
	//파티에서 빼기
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

		//도움말
		char help_text[256];
		if(selected>=0)
		{
			if(party.unit[selected].ammor==-1)sprintf(help_text, "무기: %s\n갑옷: 없음\n공격력: %d", equipment[party.unit[selected].arms].name, party.unit[selected].str);
				else sprintf(help_text, "무기: %s\n갑옷: %s\n공격력: %d\n방어력: %d/%d", equipment[party.unit[selected].arms].name, equipment[party.unit[selected].ammor].name, party.unit[selected].str, party.unit[selected].dp, equipment[party.unit[selected].ammor].pow);
		}
		else strcpy(help_text, "파티에서 뺄 유닛을 골라 주십시오.");

		RECT help_rect;
		SetRect(&help_rect, 120, 202, 516, 258);
		lib->PutText(&help_rect, help_text, JColor(0,0,0));
	}
	//수리점
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

		//도움말
		RECT help_rect;
		SetRect(&help_rect, 120, 202, 520, 282);
		if(selected>=0)
		{
			char help_text[128];

			sprintf(help_text, "%s: %d/%d\n수리비: %d\n소지금: %d카오", equipment[party.unit[selected].ammor].name, party.unit[selected].dp, equipment[party.unit[selected].ammor].pow, select_price, party.money);
			lib->PutText(&help_rect, help_text, JColor(0, 0, 0));
		}
		else lib->PutText(&help_rect, "갑옷을 수리할 캐릭터를 선택해주세요.", JColor(0, 0, 0));
	}

	//적의 턴을 알려주는 그림
	if(sw_ai)lib->Image(0, 0, imgno.panel+4);

	//대화창
	if(dialog || question)
	{
		//질문일 때는 선택 표시
		if(question)
		{
			selected=-1;
			//긍정
			if(MouseX>260 && MouseX<=310 && MouseY>175 && MouseY<=225)
			{
				lib->Image(260, 175, imgno.cursor+3, 0);
				selected=0;
			}
			else lib->Image(260, 175, imgno.cursor+3, 1);
			//부정
			if(MouseX>330 && MouseX<=380 && MouseY>175 && MouseY<=225)
			{
				lib->Image(330, 175, imgno.cursor+3, 2);
				selected=1;
			}
			else lib->Image(330, 175, imgno.cursor+3, 3);
		}

		//얼굴, 내용 표시
		lib->Image(0, 400, imgno.panel+3);
		int dialog_face=atoi(dialog_text[dialog_no*2]);
		if(dialog_face>=16)lib->Image(20, 420, imgno.unit+1, dialog_face-16);
			else if(dialog_face>=0)lib->Image(20, 420, imgno.unit, dialog_face);
		lib->Image(80, 400, imgno.panel+5);
		RECT dialog_rect;
		SetRect(&dialog_rect, 85, 408, 635, 480);
		lib->PutText(&dialog_rect, dialog_text[dialog_no*2+1], JColor(0,0,0));
	}

	//커서
	if(!fog_map[unit[unit_id].x][unit[unit_id].y])lib->Image(MouseX, MouseY, imgno.cursor);

	//메인 메뉴나 세이브&로드
	if(map_id==13)
	{
		if(click_mode<10)
		{
			if(unit[0].life)lib->Image(150, 10, "타이틀");

			lib->PutText(275, 140, "새 게임", JColor(255,255,255));
			if(!main_menu)	//중간 메뉴일 때
			{
				lib->PutText(120, 290, "저장", JColor(255,255,255));
				lib->PutText(325, 325, "돌아가기", JColor(255,255,255));				
				if(!pre_battle)
				{
					lib->PutText(200, 400, "장비", JColor(255,255,255));
					lib->PutText(410, 370, "파티설정", JColor(255,255,255));
				}
			}
			lib->PutText(450, 290, "불러오기", JColor(255,255,255));
			lib->PutText(275, 450, "나가기", JColor(255,255,255));
		}
	}
	else if(map_id==14)
	{
		char* map_name[]={"세계지도", "휴가루 마을", "초원", "사막", "바위산", "물", "다리", "가미얼 마을",
			"미버가 마을", "시크벨 마을", "루벨버 마을",  "다크퀴즈 성 1층", "다크퀴즈 성 2층"};
		//빛나는 효과
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
	if(fog_map[x][y])return;	//검은 안개가 있는 부분은 포커스가 안됨
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
	//맵에 소요되는 이동력 계산
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
	//애니메이션
	check_map[ANIMATION_MAP][x][y]=type;
	check_map[DELAY_MAP][x][y]=20;

	//불로 나무&선인장 태우기
	if(type==0 &&(map[x][y][1]==1 || map[x][y][1]==2))
	{
		map[x][y][1]=-1;
		MakeMoveMap();
	}

	if(unit_map[x][y]>=0)
	{
		int damage_unit_id=unit_map[x][y];
		//갑옷을 입었을 경우
		if(unit[damage_unit_id].dp>0)
		{
			//전기 데미지
			if(type==6)
			{
				//철 갑옷을 입었을 때만 데미지
				if(unit[damage_unit_id].ammor==14 || unit[damage_unit_id].ammor==15)
				{
					unit[damage_unit_id].hp=MaxMin(unit[damage_unit_id].hp-value, 0, unit[damage_unit_id].hp_max);					
				}
				else value=0;
			}
			//독 데미지
			else if(type==7)
			{
				unit[damage_unit_id].hp=MaxMin(unit[damage_unit_id].hp-value, 0, unit[damage_unit_id].hp_max);
			}
			else
			{
				unit[damage_unit_id].dp-=value;
				//갑옷 파괴
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
		//직접적인 데미지
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
	SearchRoad(MOVE_BACK_MAP, x, y, unit[id].move_type, unit[id].team);	//목적지로 부터 길을 만듦

	//임시로 쓸 X,Y와 TIME
	int tx, ty;
	tx=unit[id].x;
	ty=unit[id].y;
	int tt=que.que_time;
	
	Focus(tx, ty);
	//이동하는 칸을 좌우상하로 큐에 명령으로 집어 넣음
	while(check_map[MOVE_BACK_MAP][tx][ty]!=0)
	{								
		unsigned char min=check_map[MOVE_BACK_MAP][tx][ty];
		int dir=999;
		//좌로 이동 가능성 검사
		if(tx>0 && check_map[MOVE_BACK_MAP][tx-1][ty]<min)
		{
			dir=4;
			min=check_map[MOVE_BACK_MAP][tx-1][ty];
		}
		//우로 이동 가능성 검사
		if(tx<x_size-1 && check_map[MOVE_BACK_MAP][tx+1][ty]<min)
		{
			dir=6;
			min=check_map[MOVE_BACK_MAP][tx+1][ty];
		}
		//위로 이동 가능성 검사
		if(ty>0 && check_map[MOVE_BACK_MAP][tx][ty-1]<min)
		{
			dir=8;
			min=check_map[MOVE_BACK_MAP][tx][ty-1];
		}
		//아래로 이동 가능성 검사
		if(ty<y_size-1 && check_map[MOVE_BACK_MAP][tx][ty+1]<min)
		{
			dir=2;
			min=check_map[MOVE_BACK_MAP][tx][ty+1];
		}

		//임시 좌표를 이동 시킴
		if(dir==4)tx--;
			else if(dir==6)tx++;
			else if(dir==8)ty--;
			else if(dir==2)ty++;
			//버그
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
	//회전 검
	if(att_code==4)SetCheckMap(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 0);
	//2칸 거리 활
	else if(att_code==7 || att_code==8)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 2, 2, 0);
	//방갈 주둥이
	else if(att_code==10)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 2, 0);
	//3칸 거리 활
	else if(att_code==9)SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 2, 3, 0);
	//일반 검
	else SetDiamondArea(ATTACK_TEST_MAP, unit[unit_id].x, unit[unit_id].y, 1, 1, 0);

	click_mode=4;
}

void CGame::SetDamageArea(int x, int y)
{
	if(check_map[ATTACK_TEST_MAP][x][y]!=99 && (prev_x!=x || prev_y!=y || !first_check))
	{
		//////////////////////////////////////////////
		//무기 공격
		if(att_code<1000)
		{
			if(att_code==3)	//장검
			{
				//좌우
				if(x!=unit[unit_id].x)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y-1, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x, y+1, 0);
				}
				//상하
				else
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x-1, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x+1, y, 0);
				}
			}
			else if(att_code==4)	//회전검
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				SetSquareArea(DAMAGE_TEST_MAP, x, y, 1, 1, 0);
			}
			else if(att_code==5 || att_code==6)	//창
			{
				//좌
				if(x<unit[unit_id].x)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x-1, y, 0);
					if(att_code==6)SetCheckMap(DAMAGE_TEST_MAP, x-2, y, 0);
				}
				//우
				else if(x>unit[unit_id].x)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x+1, y, 0);
					if(att_code==6)SetCheckMap(DAMAGE_TEST_MAP, x+2, y, 0);
				}
				//상
				else if(y<unit[unit_id].y)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x, y-1, 0);
					if(att_code==6)SetCheckMap(DAMAGE_TEST_MAP, x, y-2, 0);
				}
				//하
				else if(y>unit[unit_id].y)
				{
					InitCheckMap(DAMAGE_TEST_MAP);
					SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
					SetCheckMap(DAMAGE_TEST_MAP, x, y+1, 0);
					if(att_code==6)SetCheckMap(DAMAGE_TEST_MAP, x, y+2, 0);
				}
			}
			//기타 1칸 짜리
			else
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
			}
		}
		//////////////////////////////////////////////
		//미지
		else if(att_code<2000)
		{				
			int mizi_code=(att_code-1000)/10;
			int lv_code=att_code%10;
			//돌풍
			if(mizi_code==0)
			{
				//좌
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
				//우
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
				//상
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
				//하
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
			//발바람
			else if(mizi_code==1)
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				if(lv_code==3)SetDiamondArea(DAMAGE_TEST_MAP, x, y, 0, 1, 0);
					else SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
			}
			//불바다
			else if(mizi_code==3)
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				if(lv_code<2)SetDiamondArea(DAMAGE_TEST_MAP, x, y, 0, 1, 0);
					else SetDiamondArea(DAMAGE_TEST_MAP, x, y, 0, 2, 0);
			}
			//강 || 정화
			else if(mizi_code==5 || mizi_code==7)
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				SetDiamondArea(DAMAGE_TEST_MAP, x, y, 0, lv_code, 0);
			}
			//지진
			else if(mizi_code==6)
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				if(lv_code==0)SetSquareArea(DAMAGE_TEST_MAP, x, y, 1, 1, 0);
					else if(lv_code==3)SetSquareArea(DAMAGE_TEST_MAP, x, y, 1, 3, 0);
					else SetSquareArea(DAMAGE_TEST_MAP, x, y, 1, 2, 0);
			}
			//벼락
			else if(mizi_code==8)
			{
				InitCheckMap(DAMAGE_TEST_MAP);
				TransMission(DAMAGE_TEST_MAP, x, y, 0);					
				SetCheckMap(DAMAGE_TEST_MAP, x, y, 0);
				//먹구름이 있을 때
				for(int j=0; j<y_size; j++)
					for(int i=0; i<x_size; i++)
						if(map[i][j][2]>=0)
						{
							TransMission(DAMAGE_TEST_MAP, i, j, 0);
							SetCheckMap(DAMAGE_TEST_MAP, i, j, 0);
						}
			}
			//빔
			else if(mizi_code==10)
			{
				//좌
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
				//우
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
				//상
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
				//하
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
			//독김&맹독김
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
			//기타 1칸 짜리 공격
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
	//물리적 공격
	if(att_code<1000)
	{
		for(int j=0; j<y_size; j++)
			for(int i=0; i<x_size; i++)
				if(check_map[DAMAGE_TEST_MAP][i][j]!=99)
				{
					if(unit[unit_id].arms==10)
					{
						lib->PlaySound(4);
						Damage(i, j, unit[unit_id].str, 5);	//물
					}
					else if(unit[unit_id].arms==11)
					{
						lib->PlaySound(3);
						Damage(i, j, unit[unit_id].str, 0);	//불
					}
					else if(unit[unit_id].arms==12)		//독
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
	//미지
	else if(att_code<2000)
	{
		int mizi_code=(att_code-1000)/10;
		int mizi_level=att_code%10;

		//지역과 상관없이 미지 발동시 처리해야 할 것들
		if(mizi_code==6)que.Insert(CM_SHAKE);
		//대상 지역에 일어나는 것
		for(int j=0; j<y_size; j++)
			for(int i=0; i<x_size; i++)
				if(check_map[DAMAGE_TEST_MAP][i][j]!=99)
				{
					//돌풍
					if(mizi_code==0)
					{
						lib->PlaySound(5);
						fog_map[i][j]=false;
						Damage(i, j, 0, 4);
						//효과의 방향으로 포커스
						if(att_dir==4)Focus(0, unit[unit_id].y);
							else if(att_dir==6)Focus(x_size-1, unit[unit_id].y);
							else if(att_dir==8)Focus(unit[unit_id].x, 0);
							else if(att_dir==2)Focus(unit[unit_id].x, y_size-1);
					}
					//발바람
					else if(mizi_code==1)
					{
						lib->PlaySound(5);
						//이동력 증가
						if(unit_map[i][j]>=0 && unit[unit_map[i][j]].move<=cara[unit[unit_map[i][j]].caracter].move)
						{
							if(mizi_level==3)unit[unit_map[i][j]].move+=3;
								else unit[unit_map[i][j]].move+=(mizi_level+1);
						}
						Damage(i, j, 0, 4);
					}
					//불덩이
					else if(mizi_code==2)
					{
						lib->PlaySound(3);
						if(mizi_level==0)Damage(i, j, 2, 0);
							else if(mizi_level==1)Damage(i, j, 3, 0);
							else Damage(i, j, 4, 0);
					}
					//불바다
					else if(mizi_code==3)
					{
						lib->PlaySound(3);
						if(mizi_level==0)Damage(i, j, 1, 0);
							else if(mizi_level==3)Damage(i, j, 3, 0);
							else Damage(i, j, 2, 0);
					}
					//치유
					else if(mizi_code==4)
					{
						lib->PlaySound(6);
						if(mizi_level==0)Damage(i, j, -3, 3);
							else if(mizi_level==1)Damage(i, j, -5, 3);
							else Damage(i, j, -10, 3);
					}
					//강
					else if(mizi_code==5)
					{
						lib->PlaySound(4);
						if(move_map[i][j]!=99)map[i][j][0]=3;
						Damage(i, j, 0, 5);
					}
					//지진
					else if(mizi_code==6)
					{
						lib->PlaySound(7);
						if(mizi_level<2)Damage(i, j, 1, 99);
							else Damage(i, j, 2, 99);						
					}
					//정화
					else if(mizi_code==7)
					{
						lib->PlaySound(6);
						if(unit_map[i][j]>=0)unit[unit_map[i][j]].state=0;
						Damage(i, j, 0, 3);
					}
					//벼락
					else if(mizi_code==8)
					{
						lib->PlaySound(8);
						if(mizi_level==3)Damage(i, j, 3, 6);
							else Damage(i, j, 2, 6);
					}
					//축복
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
					//빔
					else if(mizi_code==10)
					{
						lib->PlaySound(9);
						if(mizi_level<2)Damage(i, j, 1, 8);
							else Damage(i, j, 2, 8);
						//효과의 방향으로 포커스
						if(att_dir==4)Focus(0, unit[unit_id].y);
							else if(att_dir==6)Focus(x_size-1, unit[unit_id].y);
							else if(att_dir==8)Focus(unit[unit_id].x, 0);
							else if(att_dir==2)Focus(unit[unit_id].x, y_size-1);
					}
					//독김
					else if(mizi_code==11)
					{
						lib->PlaySound(2);
						if(unit_map[i][j]>=0 && unit[unit_map[i][j]].caracter!=NEYUM)unit[unit_map[i][j]].state=1;
						Damage(i, j, 0, 7);
					}
					//맹독김
					else if(mizi_code==12)
					{
						lib->PlaySound(2);
						if(unit_map[i][j]>=0 && unit[unit_map[i][j]].caracter!=NEYUM)unit[unit_map[i][j]].state=2;
						Damage(i, j, 0, 7);
					}
				}

		if(mizi_code==5)MakeMoveMap();	//지형 변경이 생긴 경우
		unit[unit_id].wait-=(mizi_level+1)*10;
	}
	//기타 행동
	else
	{
		if(att_code==2002)	//벌목
		{
			lib->PlaySound(1);

			Damage(x, y, 0, 1);

			if(map[x][y][1]==1)			//나무
			{
				map[x][y][1]=-1;
				party.money++;
			}
			else if(map[x][y][1]==2)	//선인장
			{
				map[x][y][1]=-1;
				map[x][y][0]=3;
			}

			MakeMoveMap();
			unit[unit_id].wait--;
		}
		else if(att_code==2003)	//다리
		{
			lib->PlaySound(10);

			if(move_map[x][y]!=99)
			{
				map[x][y][0]=4;
			}

			MakeMoveMap();
			unit[unit_id].wait--;
		}
		else if(att_code==2004)	//치료
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
		//우리편 주위의 안개를 지움
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
	if(map_id==13 || map_id==14)return;	//메뉴맵에선 안 먹힘

	//안개 없애기
	if(code==0)
	{
		for(int j=0; j<y_size; j++)
			for(int i=0; i<x_size; i++)
				fog_map[i][j]=false;
	}
	//주인공 무적
	else if(code==1)
	{
		unit[0].str=99;
		unit[0].hp_max=unit[0].hp=99;
		unit[0].mizi_level=4;
	}
	//스테이지 강제 이동
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
	//치트 끄기(무차별 치트 방지)
	else if(code==3)
	{
		cheat_able=false;
		escape=99;
	}
	//대화 강제 종료
	else if(code==4)
	{
		if(dialog)
		{
			dialog=false;
			click_mode=0;
		}
	}
	//세계지도에서 적이 출현 안 함
	else if(code==5)
	{
		no_enemy=true;
	}
}

void CGame::Function(int func_no)
{
	//메인메뉴로
	if(func_no==FUNC_MAINMENU)
	{
		pre_battle=battle;
		SaveNow();
		map_id=13;
		main_menu=false;
		escape=1;
	}
	//세계지도로
	else if(func_no==FUNC_WORLDMAP)
	{
		escape=1;
		map_id=0;
	}
	//적의 출현
	else if(func_no==FUNC_ENEMY)
	{
		map_id=map[unit[0].x][unit[0].y][0]+2;
		world_x=unit[0].x;
		world_y=unit[0].y;
		escape=1;
	}
	//보스급 이벤트 끝
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
	//다크퀴즈의 퀴즈
	else if(func_no==FUNC_QUIZ1)
	{
		que.Insert(CM_QUESTION, 106, FUNC_ANSWER1, FUNC_FAILED);
	}
	//정답
	else if(func_no==FUNC_ANSWER1)
	{
		que.Insert(CM_DIALOG, 110, 110, FUNC_QUIZ2);
	}
	//다크퀴즈의 퀴즈2
	else if(func_no==FUNC_QUIZ2)
	{
		que.Insert(CM_QUESTION, 111, FUNC_FAILED, FUNC_ANSWER2);
	}
	//정답
	else if(func_no==FUNC_ANSWER2)
	{
		que.Insert(CM_DIALOG, 112, 113, FUNC_QUIZ3);
	}
	//다크퀴즈의 퀴즈3
	else if(func_no==FUNC_QUIZ3)
	{
		que.Insert(CM_QUESTION, 114, FUNC_FAILED);
	}
	//오답
	else if(func_no==FUNC_FAILED)
	{
		que.Insert(CM_DIALOG, 107, 109, FUNC_GAMEOVER);
	}
	//게임 오버
	else if(func_no==FUNC_GAMEOVER)
	{
		escape=2;
	}
	//엔딩
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

	//빈자리가 있으면 그냥 넣음
	for(int i=0; i<PARTY_MAX; i++)
		if(!party.unit[i].life)
		{
			party.unit[i].SetUnit(0, 0, id, 0);
			blank=true;
			break;
		}

	//없으면 파티에서 한 명을 빼고 넣음
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
			//옮길 캐릭터 찾기
			int change_target=-1;
			for(int j=i+1; j<PARTY_MAX; j++)
				if(party.unit[j].life)
				{
					change_target=j;
					break;
				}

			if(change_target==-1)break;	//마지막이면 끝내기

			//옮기기
			party.unit[i].SetUnit(0, 0, party.unit[change_target].caracter, 0);
			party.unit[i]=party.unit[change_target];
			party.unit[change_target].life=false;
		}
	}
}