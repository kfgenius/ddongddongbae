#define BASE 400	//지면의 높이
#define RECTMAX 18	//한 캐릭터가 가지는 영역의 한계
#define BALLMAX 20	//공 숫자 한계
#define PLAYERS 4	//플레이어 숫자
#define ALL_PLAYER 7	//만들어 놓은 모든 선수
#define ALL_GROUND 5	//만들어 놓은 경기장

#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey )) && activate

FILE *fp;
int frameskip = 0;
bool keydown[2];
bool key_enter, key_esc;
double music_volume=1.0f;
double effect_volume=1.0f;
bool sound_on;

bool activate = true;

HWND hwnd;

//사운드
HSNDOBJ Sound[100];

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

BOOL SoundCard;
BOOL ReplayFlag;

//미디 연주
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

void _Play( int num )
{
    if ( SoundCard ) SndObjPlay( Sound[num], NULL );
}

//효과음 연주
void Play(char* name)
{
}

//최대값 최소값
int Max(int x, int y)
{
	if(x>y)return x;
		else return y;
}

int Min(int x, int y)
{
	if(x<y)return x;
		else return y;
}

bool collision(RECT* rect1, RECT* rect2)
{
	if(rect2->left < rect1->right && rect2->right>rect1->left &&
		rect2->top < rect1->bottom && rect2->bottom > rect1->top)return true;
	else return false;
}

void AddRect(RECT* org_rect, RECT* add_rect, int x, int y)	//RECT 크기 더하기
{
	org_rect->left=x+add_rect->left;
	org_rect->right=x+add_rect->right;
	org_rect->top=y+add_rect->top;
	org_rect->bottom=y+add_rect->bottom;
}

char global_buffer[80];

//선수 ID
#define LDH		0
#define GUNDAL	1
#define VIRUS	2
#define SKULL	3
#define DRAGUN	4
#define HWARANG	5
#define AMOR	6

//경기장 ID
#define SOCCER	0
#define JAIL	1
#define BASKET	2
#define HELL	3
#define STAR	4

////////////////////////////////////////////
//경기장 클래스(전역 클래스)
////////////////////////////////////////////
class CHall
{
public:
	int id;				//경기장 ID
	int players;		//참가 선수수
	int scroll;			//화면 스크롤
	int grav;			//경기장 중력(높을 수록 중력이 낮음)
	int bound;			//공이 튀는 정도
	int width;			//경기장의 넓이
	int score[2];		//점수
	RECT srect;			//화면에 보이는 영역
	int time;
	char filename[20];

	CHall();
	void SetHall(int type);
	void SetScroll(int x);
	bool AutoScroll(int x);
	void SetSrect();
	void Event();
	void Score(int t);
};

////////////////////////////////////////////
//플레이어 클래스
////////////////////////////////////////////
class CPlayer
{
private:
	bool jump, pressJ;		//점프 가능 여부, 점프키 누르기 가능 여부
	int gravity;			//중력
	int next[4];			//애니메이션 할 장면들

public:
	bool life;							//생존 여부
	bool ai;							//컴퓨터 조종여부
	int id;								//캐릭터 고유넘버
	char name[8];						//캐릭터 이름(불러올 파일)
	int team;							//소속 팀
	int x, y;							//좌표
	int hp;								//체력
	int power, speed;					//힘, 속도
	int defense, jump_pow;				//방어력, 점프력
	int spring;							//도약력
	int keyL, keyR, keyJ, keyD, keyA;	//키 설정
	DWORD dir;							//방향
	int now;							//지금 보여지는 장면
	int delay;							//장면의 지연시간
	bool ani_loop;						//애니메이션 루프 여부
	int count, cp;						//임의로 사용
	int cx[14], cy[7];					//중심 좌표
	RECT rects[RECTMAX*2];				//가진 영역들
	int n_rect[RECTMAX];				//각 장면에 해당하는 영역을 가르키는 변수:6장면*3(발, 몸통, 공격)
	RECT frect, brect, arect;			//발, 몸, 공격
	bool attack_on;						//공격 여부
	bool up_on, up_pre, down_on, left_on, right_on, att_on; //키를 눌렀는지 여부
	bool attack;	//공격 가능 여부

	void SetPlayer(int cid, int tm);
	void SetKey(int left, int right, int jump, int down, int attack);
	void Move();
	void Jump();
	void Hurt(int damage);
	void SetAni(int f1, int f2, int f3, int f4, int delay, bool loop = false);
	void Gravity();
	void KeyIn();
	void Action();
	void Common();
	void SetRegion(int px, int py);
};

/*//////////////////////////////////////////
공 클래스
(공은 중심좌표를 안 쓰고 사각영역으로 처리)
//////////////////////////////////////////*/
class CBall
{
private:
	int force_x, force_y;		//x,y로 전해진 힘

	void SetBallRect();
public:
	int type;					//공의 타입(0:축구공 1:철구 2:농구공 3:슬라임 4:달)
	bool life;					//존재여부
	int x, y;					//x,y좌표
	int width, height;			//너비, 높이
	int master;					//공 찬 사람
	DWORD dir;					//방향
	bool weapon;				//무기인지 공인지 구분
	int heavy;					//무게
	RECT brect;					//존재 영역
	bool ani;					//애니메이션
	int ani_now, ani_count;		//애니메이션 상태, 카운트

	void SetBall(int sx, int sy, int stype);
	void SetWeapon(int sx, int sy, int stype, DWORD sdir, int fx, int fy, int ms);
	void Move();
	bool Force(RECT* frect, int dir, int power, int ms);
	void Force2(int fx, int fy);
};

////////////////////////////////////////////
//녹화 클래스
////////////////////////////////////////////
class CVideo
{
private:
	int players;
	int player_id[PLAYERS];
	int hall_id;
	int cnt;
	char pattern[PLAYERS][9900];

public:
	void RecordStart();
	void PlayStart();
	void Record();
	void Play();
	bool LoadFile(char* filename);
	void RecordFile(char* filename);
};

////////////////////////////////////////////
//AI 클래스
////////////////////////////////////////////
class CComputer
{
	//공격 관련
	int shoot[ALL_PLAYER*2][8], attack[ALL_PLAYER*2][8];	//슛하는 거리와 공격 거리
	int shoot_count[ALL_PLAYER*2][8], attack_count[ALL_PLAYER*2][8];
	//점프 관련
	int jump[ALL_PLAYER*2][8];	//점프하는 거리와 공격 거리
	int jump_count[ALL_PLAYER*2][8];
public:
	void learn(int self, int target);
	void run(int self, int target);
	CComputer();
	~CComputer();
};

CHall hall;
CBall ball[BALLMAX];
CPlayer player[PLAYERS];					

//CHall 메소드들
CHall::CHall()
{
	srect.top=0;
	srect.bottom=600;
}

void CHall::SetHall(int type)
{
	int gravs[]={5,5,5,1,9};		//숫자가 클수록 중력이 낮음
	int bounds[]={3,5,9,4,1};		//숫자가 클수록 탄성이 높음
	int widths[]={1440,700,1800,940,2300};
	char* files[]={"unite.mlc","jail.mlc","basket.mlc","hell.mlc","star.mlc"};
	id=type;
	grav = gravs[id];
	bound = bounds[id];
	width = widths[id];
	strcpy(filename,files[id]);
	SetScroll(0);
	time=99;
	for(int i=0; i<2; i++)score[i]=0;

	//플레이어 배치
	player[0].x=200;
	player[1].x=width-200;
	player[2].x=100;
	player[3].x=width-100;
	//공 초기화
	for(int i=0; i<BALLMAX; i++)ball[i].life=false;
	ball[0].SetBall(width/2,999,type);
}

void CHall::SetScroll(int x)
{
	scroll=x;
	SetSrect();
}

bool CHall::AutoScroll(int x)
{
	int scroll_max=width-640;
	int between=x-scroll-320;
	int move_scroll=Min(abs(between)/3,20);
	if(scroll>0 && between < -6)//좌로 스크롤
		scroll=Max(scroll-move_scroll, 0);
	else if(scroll<scroll_max && between > 6)	//우로 스크롤
		scroll=Min(scroll+move_scroll/2,scroll_max);
	else return false;	//스크롤이 없었으면 이 시점에서 false를 리턴
	SetSrect();
	return true;
}

void CHall::SetSrect()
{
	srect.left=scroll;
	srect.right=scroll+640;
}

void CHall::Event()
{
	time--;
	int random=((100+time)*123);
	if(id==HELL)
	{
		int count=3;
		for(int w_id=0; w_id<BALLMAX; w_id++)
			if(!ball[w_id].life)
			{
				ball[w_id].SetWeapon((random+w_id*743)%(width-60),0,1,0,0,-40,-1);
				count--;
				if(count==0)break;
			}
	}

	for(int i=0; i<BALLMAX; i++)
	{
		if(!ball[i].life)continue;
		//슬라임의 자유의지와 번식
		if(!ball[i].weapon && ball[i].type==3)
		{
			//자유의지
			ball[i].Force2((random%10-5)*30,(random%10-5)*30);
			//번식
			if(i==0 && (time%20)==0)
				for(int w_id=0; w_id<BALLMAX; w_id++)
					if(!ball[w_id].life)
					{
						ball[w_id].SetBall(ball[i].x,ball[i].y,ball[i].type);
						break;
					}
		}
	}
}

void CHall::Score(int t)
{
	score[t]++;
}

//CPlayer 메소드들
void CPlayer::SetPlayer(int cid, int tm)	//키 설정
{
	char* names[]={"dh","gd","vr","sp","dr","hr","la"};
	int pow[]={2,4,1,7,6,1,5};
	int def[]={6,7,1,6,2,1,9};
	int spd[]={4,3,6,1,3,3,1};
	int spr[]={20,15,30,0,0,10,0};

	id = cid;
	strcpy(name,names[id]);
	power = pow[id];
	defense = (10 - def[id]);
	speed = spd[id];
	jump_pow = spr[id];
	team = tm;
	if(team==1)dir=DPX_HFLIP;
		else dir=0;
	jump=true;
	hp=1000;
	spring=gravity=count=0;
	
	y=BASE;
	up_on=down_on=left_on=right_on=att_on=false;

	SetAni(0,1,2,1,40,true);
}

void CPlayer::SetKey(int left, int right, int jump, int down, int attack)
{
	keyL = left;
	keyR = right;
	keyJ = jump;
	keyD = down;
	keyA = attack;
}

void CPlayer::Move()
{
	if(!dir)x+=speed;
		else x-=speed;
	if(id == AMOR)	//생갑의 경우 스스로 골인 가능
	{
		//골인
		if(x < 0)
		{
			hall.Score(1);
			x=hall.width/2;
			y=0;
		}
		else if(x > hall.width)
		{
			hall.Score(0);
			x=hall.width/2;
			y=0;
		}
	}
	else x=Min(Max(x,0),hall.width);
	count++;
}

void CPlayer::Jump()	//점프
{
	if(jump)
	{
		spring = jump_pow;
		jump=false;
	}
	if(spring>0)
	{
		y-=spring;
		spring--;
	}
}

void CPlayer::Gravity()	//중력
{
	if(id == DRAGUN)return;	//비행 가능한 캐릭터

	if(y < BASE)
	{
		if(jump)jump=false;
		y+=(gravity/10);
		gravity+=(10-hall.grav);
		if(y>=BASE)
		{
			y=BASE;
			jump=true;
			gravity=0;
		}
	}
}

void CPlayer::Hurt(int damage)
{
	if(now >= 5)return;
	damage = damage * defense / 10;
	hp=Max(0,hp-damage);
	if(!hp)	//사망
	{
		SetAni(6,6,6,6,100,true);
	}
	else	//패닉
	{
		SetAni(5,5,5,5,damage);
	}
}

void CPlayer::SetAni(int f1, int f2, int f3, int f4, int d, bool loop)
{
	next[0]=f1;
	next[1]=f2;
	next[2]=f3;
	next[3]=f4;

	delay=d;
	ani_loop=loop;
	now=f1;
	cp=0;
}

void CPlayer::KeyIn()
{
	down_on=left_on=right_on=att_on=false;
	
	if(_GetKeyState(keyL))left_on=true;
	if(_GetKeyState(keyR))right_on=true;
	if(_GetKeyState(keyD))down_on=true;
	if(_GetKeyState(keyA))att_on=true;

	up_pre=up_on;
	if(_GetKeyState(keyJ))up_on=true;
		else up_on=false;
}

void CPlayer::Action()
{
	count++;
	if(count > delay)
	{

		count = 0;
		cp++;
		if(cp > 3)
		{
			cp=0;
			//화살쏘기
			if(id == HWARANG && next[3]==4)
			{
				attack_on=true;
			}

			if(!ani_loop)SetAni(0,1,2,1,40,true);
		}
		now=next[cp];
	}
	Common();
	if(now >= 5)return;

	//좌우 이동
	if(left_on)
	{
		dir=DPX_HFLIP;
		Move();
	}
	else if(right_on)
	{
		dir=0;
		Move();
	}
	if(id != DRAGUN)
	{
		//점프
		if(up_on)
		{
			if(!pressJ)
			{
				Jump();
				if(!spring)pressJ=true;
			}
		}
		else
		{
			pressJ=false;
			spring=0;
		}
	}
	else
	{
		//비행
		if(up_on)y = Max(y-speed,0);
		else if(down_on)y = Min(y+speed,BASE);
	}
	//공격
	if(att_on)
	{
		if(attack)
		{
			if(id != HWARANG)SetAni(0,3,4,3,10);
				else SetAni(1,3,3,4,10);
			attack=false;
		}
	}
	else attack=true;
}

void CPlayer::Common()
{
	if(now!=6)Gravity();
	if(now==5 && id != AMOR)
	{
		if(!dir)x-=speed;
			else x+=speed;
		//골인
		if(x < 0)
		{
			hall.Score(1);
			x=hall.width/2;
			y=0;
		}
		else if(x > hall.width)
		{
			hall.Score(0);
			x=hall.width/2;
			y=0;
		}
	}
	if(now==6)
	{
		y-=speed*2;
		if(y<0)life=false;
	}
}
void CPlayer::SetRegion(int px, int py)
{
	int no;
	if(!dir)
	{
		no=n_rect[now*3];
		if(no==-1)SetRect(&frect,999,999,999,999);
			else AddRect(&frect,&rects[no],px,py);
		no=n_rect[now*3+1];
		AddRect(&brect,&rects[no],px,py);
		no=n_rect[now*3+2];
		if(no==-1)SetRect(&arect,999,999,999,999);
			else AddRect(&arect,&rects[no],px,py);
	}
	else
	{
		no=n_rect[now*3]+RECTMAX;
		AddRect(&frect,&rects[no],px,py);
		no=n_rect[now*3+1]+RECTMAX;
		AddRect(&brect,&rects[no],px,py);
		no=n_rect[now*3+2];
		if(no==-1)SetRect(&arect,999,999,999,999);
			else AddRect(&arect,&rects[no+RECTMAX],px,py);
	}
}

//CBall 메소드들
void CBall::SetBall(int sx, int sy, int stype)
{
	//공 타입에 따른 크기와 무게
	int ball_w[]={40,80,40,80,50};
	int ball_h[]={40,71,42,59,50};
	int ball_hv[]={10,100,10,10,80};	//100이 한계
	bool animation[]={true,false,true,true,false};

	type=stype;
	width=ball_w[type];
	height=ball_h[type];
	heavy=ball_hv[type];
	x=sx;
	if(sy==999)y=BASE-height;	//999일때는 지면에
		else y=sy;
	dir=0;
	ani=animation[stype];
	ani_now=ani_count=0;
	master=-1;

	force_x=0;
	force_y=0;
	life=true;
	weapon=false;
	SetBallRect();
}

void CBall::SetWeapon(int sx, int sy, int stype, DWORD sdir, int fx, int fy, int ms)
{
	//무기 타입에 따른 크기와 무게
	int ball_w[]={85,60};
	int ball_h[]={17,100};
	int ball_damage[]={1,2};
	bool animation[]={false,true};

	type=stype;
	width=ball_w[type];
	height=ball_h[type];
	heavy=ball_damage[type];
	x=sx;
	y=sy;
	force_x=fx;
	force_y=fy;
	dir=sdir;
	master=ms;
	ani=animation[stype];
	ani_now=ani_count=0;

	life=true;
	weapon=true;
	SetBallRect();
}

void CBall::SetBallRect()		//존재영역 확인
{
	brect.left=x-hall.scroll;
	brect.top=y;
	brect.right=x+width-hall.scroll;
	brect.bottom=y+height;
}

void CBall::Move()		//힘에 의한 공의 이동
{
	if(weapon && type==0)	//화살이 공중에서 안 멈추게...
	{
		if(force_x>0)force_x=Max(50,force_x);
		else if(force_x<=0)force_x=Min(-50,force_x);	
	}

	x+=force_x/10;
	y+=force_y/10;
	//애니메이션을 할 때는 힘에 의해 카운트 증가
	if(ani)
	{
		ani_count+=abs(force_x)+abs(force_y);
		if(ani_count > 200)
		{
			ani_now++;
			ani_count=0;
		}
		if(ani_now > 2)ani_now=0;
	}
	//좌우로 가해진 힘 처리
	if(!weapon )
	{
		if(force_x > 0)
		{
			dir=0;
			force_x=Max(0,force_x-(10-hall.grav)*heavy/20);
		}
		else if(force_x < 0)
		{
			dir=DPX_HFLIP;
			force_x=Min(0,force_x+(10-hall.grav)*heavy/20);
		}
	}
	else
	{
		if(force_x > 0)dir=0;
			else if(force_x < 0)dir=DPX_HFLIP;
	}

	//공이 바닥에 닿았을 때
	if((y+height)>=BASE)
	{
		if(weapon && type==1)	//불의 경우 바닥에 닿으면 사라짐
		{
			life=false;	
		}
		else
		{
			y=BASE-height;
			force_y=-(force_y*hall.bound/10*(100-heavy)/100);
		}
	}
	else force_y+=(10-hall.grav)*heavy/10;
	//골인
	if(x < -width)
	{
		hall.Score(1);
		if(!weapon)SetBall(hall.width/2-width,0,type);
		 else life=false;
	}
	else if(x > hall.width)
	{
		hall.Score(0);
		if(!weapon)SetBall(hall.width/2-width,0,type);
			else life=false;
	}
	SetBallRect();
}
bool CBall::Force(RECT* frect, int dir, int power, int ms)
//힘의 방향: 0 - 우상 1 - 우하 2 - 좌하 3 - 좌상
{
	bool result = false;
	int distance1, distance2, fx, fy;
	int power2=power*hall.grav;		//힘과 중력의 곱

	if(collision(&brect,frect))
	{
		result = true;
		//겹친 영역 너비
		if(frect->left >= brect.left && frect->right <= brect.right ||
			frect->left <= brect.left && frect->right >= brect.right)
		//정통으로 맞았을 때
		{
			fx=0;
		}
		//비스듬히 맞았을 때
		else
		{
			distance1 = frect->right - brect.left;
			distance2 =  brect.right - frect->left;
			if(distance1 < distance2)
			{
				if(dir == 0 || dir == 1)fx=Max(0, distance1*2+power2-heavy);	//정방향일때 힘이 2배
					else fx=Max(0, distance1+power2-heavy);
			}
			else
			{
				if(dir == 0 || dir == 1)fx=-Max(0, distance2+power2-heavy);
					else fx=-Max(0, distance2*2+power2-heavy);
			}
		}
		//겹친 영역 높이
		//겹친 영역 너비
		if(frect->top >= brect.top && frect->bottom <= brect.bottom ||
			frect->top <= brect.top && frect->bottom >= brect.bottom)
		//정통으로 맞았을 때
		{
			fy=0;
		}
		//비스듬히 맞았을 때
		else
		{
			distance1 = frect->bottom - brect.top;
			distance2 =  brect.bottom - frect->top;
			if(distance1 < distance2)
			{
				if(dir == 1 || dir == 2)fy=distance1*2;	//정방향일때 힘이 2배
					else fy=distance1;
			}
			else
			{
				if(dir == 1 || dir == 2)fy=-distance2;
					else fy=-distance2*2;
			}
		}
		force_y+=fy-power2+heavy;
		force_x+=fx;

		master=ms;	//공 찬 주인
	}

	return result;
}

void CBall::Force2(int fx, int fy)
{
	force_x+=fx;
	force_y+=fy;
}

//CVideo 메소드들
void CVideo::RecordStart()
{
	hall_id=hall.id;
	players=hall.players;
	for(int i=0; i<players; i++)player_id[i]=player[i].id;
	cnt=0;
}

void CVideo::PlayStart()
{
	hall.SetHall(hall_id);
	for(int i=0; i<players; i++)player[i].SetPlayer(player_id[i],i%2);
	cnt=0;
}

void CVideo::Record()
{
	for(int i=0; i<players; i++)
	{
		pattern[i][cnt]=0;
		if(player[i].up_on)pattern[i][cnt]|=0x01;
		if(player[i].down_on)pattern[i][cnt]|=0x02;
		if(player[i].left_on)pattern[i][cnt]|=0x04;
		if(player[i].right_on)pattern[i][cnt]|=0x08;
		if(player[i].att_on)pattern[i][cnt]|=0x10;
	}
	cnt++;
}

void CVideo::Play()
{
	for(int i=0; i<players; i++)
	{
		player[i].up_on=player[i].down_on=player[i].left_on=player[i].right_on=player[i].att_on=false;
		
		if(pattern[i][cnt] & 0x01)player[i].up_on=true;
		if(pattern[i][cnt] & 0x02)player[i].down_on=true;
		if(pattern[i][cnt] & 0x04)player[i].left_on=true;
		if(pattern[i][cnt] & 0x08)player[i].right_on=true;
		if(pattern[i][cnt] & 0x10)player[i].att_on=true;
	}
	cnt++;
}

void CVideo::RecordFile(char* filename)
{
	if(!(fp = fopen(filename,"wb")))return;
	fwrite(&players, sizeof(int), 1, fp);
	fwrite(player_id, sizeof(int), players, fp);
	fwrite(&hall_id, sizeof(int), 1, fp);
	fwrite(&pattern, sizeof(bool), players*9900, fp);
	fclose(fp);
}

bool CVideo::LoadFile(char* filename)
{
	if(!(fp = fopen(filename,"rb")))return false;
	fread(&players, sizeof(int), 1, fp);
	fread(player_id, sizeof(int), players, fp);
	fread(&hall_id, sizeof(int), 1, fp);
	fread(&pattern, sizeof(bool), players*9900, fp);
	fclose(fp);

	hall.players=players;

	return true;
}

//AI 메소드
void CComputer::learn(int self, int target)
{
	int position;
	int distance[3];
	int tmp;
	int me=player[self].id+player[self].team*ALL_PLAYER;

	//공과 적사이의 거리 차
	distance[0]=player[target].x-player[self].x;
	if(ball[0].x>player[self].x)distance[1]=ball[0].x-player[self].x;
		else distance[1]=ball[0].x+ball[0].width-player[self].x;
	if(ball[0].y>player[self].y)distance[2]=ball[0].y-player[self].y;
		else distance[2]=ball[0].y+ball[0].height-player[self].y;
	if(distance[0]<0)tmp=4;
		else tmp=0;
	//위치 분석
	if((distance[0]*distance[1])>=0)
	{
		if(abs(distance[0])>abs(distance[1]))position=tmp;
			else position=tmp+1;
	}
	else
	{
		if(abs(distance[0])>abs(distance[1]))position=tmp+2;
			else position=tmp+3;
	}

	//공격&슛 분석
	if(player[self].attack && player[self].att_on)
	{
		if(abs(distance[0])>abs(distance[1]))	//공차는 거리로 생각
		{
			shoot_count[me][position]++;
			shoot[me][position]=(shoot[me][position]*(shoot_count[me][position]-1)+distance[1])/shoot_count[me][position];
		}
		else	//공격 하는 거리로 생각
		{
			attack_count[me][position]++;
			attack[me][position]=(attack[me][position]*(attack_count[me][position]-1)+distance[0])/attack_count[me][position];
		}
	}

	//점프 분석
	if(!player[self].up_pre && player[self].up_on)
	{
		jump_count[me][position]++;
		jump[me][position]=(jump[me][position]*(jump_count[me][position]-1)+distance[2])/jump_count[me][position];
	}
	
}

void CComputer::run(int self, int target)
{
	int position;
	int distance[3];
	int tmp;
	int me=player[self].id+player[self].team*ALL_PLAYER;

	//공과 적사이의 거리 차
	distance[0]=player[target].x-player[self].x;
	if(ball[0].x>player[self].x)distance[1]=ball[0].x-player[self].x;
		else distance[1]=ball[0].x+ball[0].width-player[self].x;
	if(ball[0].y>player[self].y)distance[2]=ball[0].y-player[self].y;
		else distance[2]=ball[0].y+ball[0].height-player[self].y;
	if(distance[0]<0)tmp=4;
		else tmp=0;
	//위치 분석
	if((distance[0]*distance[1])>=0)
	{
		if(abs(distance[0])>abs(distance[1]))position=tmp;
			else position=tmp+1;
	}
	else
	{
		if(abs(distance[0])>abs(distance[1]))position=tmp+2;
			else position=tmp+3;
	}

	player[self].left_on=player[self].right_on=player[self].up_on=player[self].down_on=player[self].att_on=false;
	if(player[self].team==0)
	{
		if(distance[1]<0 || player[self].id==HWARANG || player[self].id==GUNDAL)
		{
			if(distance[0]<-attack[me][position])player[self].left_on=true;
				else if(distance[0]>attack[me][position])player[self].right_on=true;
			if(abs(distance[0])<abs(attack[me][position])*5 && player[self].id!=DRAGUN)player[self].up_on=true;
		}
		else player[self].right_on=true;
	}
	else
	{
		if(distance[1]<0 && player[self].id!=HWARANG && player[self].id!=GUNDAL)player[self].left_on=true;
		else
		{
			if(distance[0]<-attack[me][position])player[self].left_on=true;
				else if(distance[0]>attack[me][position])player[self].right_on=true;
			if(abs(distance[0])<abs(attack[me][position])*5 && player[self].id!=DRAGUN)player[self].up_on=true;
		}
	}
	
	//공격
	if(abs(distance[0]-attack[me][position])<player[self].speed)player[self].att_on=true;
	//슛
	if(abs(distance[1]-shoot[me][position])<player[self].speed)player[self].att_on=true;
	//점프
	if(abs(distance[2]-jump[me][position])<player[self].jump_pow)player[self].up_on=true;
		else player[self].down_on=true;
}

CComputer::CComputer()
{
	char ver;	//AI 버전
	if(fp==fopen("AI\\donghun.ai1","rb"))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[0],sizeof(int),8,fp);
		fread(shoot_count[0],sizeof(int),8,fp);
		fread(attack[0],sizeof(int),8,fp);
		fread(attack_count[0],sizeof(int),8,fp);
		fread(jump[0],sizeof(int),8,fp);
		fread(jump_count[0],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\gundal.ai1","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[1],sizeof(int),8,fp);
		fread(shoot_count[1],sizeof(int),8,fp);
		fread(attack[1],sizeof(int),8,fp);
		fread(attack_count[1],sizeof(int),8,fp);
		fread(jump[1],sizeof(int),8,fp);
		fread(jump_count[1],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\DH_virus.ai1","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[2],sizeof(int),8,fp);
		fread(shoot_count[2],sizeof(int),8,fp);
		fread(attack[2],sizeof(int),8,fp);
		fread(attack_count[2],sizeof(int),8,fp);
		fread(jump[2],sizeof(int),8,fp);
		fread(jump_count[2],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\skullkus.ai1","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[3],sizeof(int),8,fp);
		fread(shoot_count[3],sizeof(int),8,fp);
		fread(attack[3],sizeof(int),8,fp);
		fread(attack_count[3],sizeof(int),8,fp);
		fread(jump[3],sizeof(int),8,fp);
		fread(jump_count[3],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\dragundal.ai1","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[4],sizeof(int),8,fp);
		fread(shoot_count[4],sizeof(int),8,fp);
		fread(attack[4],sizeof(int),8,fp);
		fread(attack_count[4],sizeof(int),8,fp);
		fread(jump[4],sizeof(int),8,fp);
		fread(jump_count[4],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\hwarang.ai1","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[5],sizeof(int),8,fp);
		fread(shoot_count[5],sizeof(int),8,fp);
		fread(attack[5],sizeof(int),8,fp);
		fread(attack_count[5],sizeof(int),8,fp);
		fread(jump[5],sizeof(int),8,fp);
		fread(jump_count[5],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\livingamor.ai1","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[6],sizeof(int),8,fp);
		fread(shoot_count[6],sizeof(int),8,fp);
		fread(attack[6],sizeof(int),8,fp);
		fread(attack_count[6],sizeof(int),8,fp);
		fread(jump[6],sizeof(int),8,fp);
		fread(jump_count[6],sizeof(int),8,fp);
		fclose(fp);
	}

	//2P AI
	if(fp==fopen("AI\\donghun.ai2","rb"))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[7],sizeof(int),8,fp);
		fread(shoot_count[7],sizeof(int),8,fp);
		fread(attack[7],sizeof(int),8,fp);
		fread(attack_count[7],sizeof(int),8,fp);
		fread(jump[7],sizeof(int),8,fp);
		fread(jump_count[7],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\gundal.ai2","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[8],sizeof(int),8,fp);
		fread(shoot_count[8],sizeof(int),8,fp);
		fread(attack[8],sizeof(int),8,fp);
		fread(attack_count[8],sizeof(int),8,fp);
		fread(jump[8],sizeof(int),8,fp);
		fread(jump_count[8],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\DH_virus.ai2","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[9],sizeof(int),8,fp);
		fread(shoot_count[9],sizeof(int),8,fp);
		fread(attack[9],sizeof(int),8,fp);
		fread(attack_count[9],sizeof(int),8,fp);
		fread(jump[9],sizeof(int),8,fp);
		fread(jump_count[9],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\skullkus.ai2","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[10],sizeof(int),8,fp);
		fread(shoot_count[10],sizeof(int),8,fp);
		fread(attack[10],sizeof(int),8,fp);
		fread(attack_count[10],sizeof(int),8,fp);
		fread(jump[10],sizeof(int),8,fp);
		fread(jump_count[10],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\dragundal.ai2","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[11],sizeof(int),8,fp);
		fread(shoot_count[11],sizeof(int),8,fp);
		fread(attack[11],sizeof(int),8,fp);
		fread(attack_count[11],sizeof(int),8,fp);
		fread(jump[11],sizeof(int),8,fp);
		fread(jump_count[11],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\hwarang.ai2","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[12],sizeof(int),8,fp);
		fread(shoot_count[12],sizeof(int),8,fp);
		fread(attack[12],sizeof(int),8,fp);
		fread(attack_count[12],sizeof(int),8,fp);
		fread(jump[12],sizeof(int),8,fp);
		fread(jump_count[12],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\livingamor.ai2","rb")))
	{
		fread(&ver,sizeof(char),1,fp);
		if(ver==0)fseek(fp,32,SEEK_CUR);
		fread(shoot[13],sizeof(int),8,fp);
		fread(shoot_count[13],sizeof(int),8,fp);
		fread(attack[13],sizeof(int),8,fp);
		fread(attack_count[13],sizeof(int),8,fp);
		fread(jump[13],sizeof(int),8,fp);
		fread(jump_count[13],sizeof(int),8,fp);
		fclose(fp);
	}
}

CComputer::~CComputer()
{
	char ver=1;	//AI 버전
	if(fp==fopen("AI\\donghun.ai1","wb"))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[0],sizeof(int),8,fp);
		fwrite(shoot_count[0],sizeof(int),8,fp);
		fwrite(attack[0],sizeof(int),8,fp);
		fwrite(attack_count[0],sizeof(int),8,fp);
		fwrite(jump[0],sizeof(int),8,fp);
		fwrite(jump_count[0],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\gundal.ai1","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[1],sizeof(int),8,fp);
		fwrite(shoot_count[1],sizeof(int),8,fp);
		fwrite(attack[1],sizeof(int),8,fp);
		fwrite(attack_count[1],sizeof(int),8,fp);
		fwrite(jump[1],sizeof(int),8,fp);
		fwrite(jump_count[1],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\DH_virus.ai1","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[2],sizeof(int),8,fp);
		fwrite(shoot_count[2],sizeof(int),8,fp);
		fwrite(attack[2],sizeof(int),8,fp);
		fwrite(attack_count[2],sizeof(int),8,fp);
		fwrite(jump[2],sizeof(int),8,fp);
		fwrite(jump_count[2],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\skullkus.ai1","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[3],sizeof(int),8,fp);
		fwrite(shoot_count[3],sizeof(int),8,fp);
		fwrite(attack[3],sizeof(int),8,fp);
		fwrite(attack_count[3],sizeof(int),8,fp);
		fwrite(jump[3],sizeof(int),8,fp);
		fwrite(jump_count[3],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\dragundal.ai1","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[4],sizeof(int),8,fp);
		fwrite(shoot_count[4],sizeof(int),8,fp);
		fwrite(attack[4],sizeof(int),8,fp);
		fwrite(attack_count[4],sizeof(int),8,fp);
		fwrite(jump[4],sizeof(int),8,fp);
		fwrite(jump_count[4],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\hwarang.ai1","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[5],sizeof(int),8,fp);
		fwrite(shoot_count[5],sizeof(int),8,fp);
		fwrite(attack[5],sizeof(int),8,fp);
		fwrite(attack_count[5],sizeof(int),8,fp);
		fwrite(jump[5],sizeof(int),8,fp);
		fwrite(jump_count[5],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\livingamor.ai1","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[6],sizeof(int),8,fp);
		fwrite(shoot_count[6],sizeof(int),8,fp);
		fwrite(attack[6],sizeof(int),8,fp);
		fwrite(attack_count[6],sizeof(int),8,fp);
		fwrite(jump[6],sizeof(int),8,fp);
		fwrite(jump_count[6],sizeof(int),8,fp);
		fclose(fp);
	}

	//2P AI
	if(fp==fopen("AI\\donghun.ai2","wb"))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[7],sizeof(int),8,fp);
		fwrite(shoot_count[7],sizeof(int),8,fp);
		fwrite(attack[7],sizeof(int),8,fp);
		fwrite(attack_count[7],sizeof(int),8,fp);
		fwrite(jump[7],sizeof(int),8,fp);
		fwrite(jump_count[7],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\gundal.ai2","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[8],sizeof(int),8,fp);
		fwrite(shoot_count[8],sizeof(int),8,fp);
		fwrite(attack[8],sizeof(int),8,fp);
		fwrite(attack_count[8],sizeof(int),8,fp);
		fwrite(jump[8],sizeof(int),8,fp);
		fwrite(jump_count[8],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\DH_virus.ai2","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[9],sizeof(int),8,fp);
		fwrite(shoot_count[9],sizeof(int),8,fp);
		fwrite(attack[9],sizeof(int),8,fp);
		fwrite(attack_count[9],sizeof(int),8,fp);
		fwrite(jump[9],sizeof(int),8,fp);
		fwrite(jump_count[9],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\skullkus.ai2","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[10],sizeof(int),8,fp);
		fwrite(shoot_count[10],sizeof(int),8,fp);
		fwrite(attack[10],sizeof(int),8,fp);
		fwrite(attack_count[10],sizeof(int),8,fp);
		fwrite(jump[10],sizeof(int),8,fp);
		fwrite(jump_count[10],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\dragundal.ai2","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[11],sizeof(int),8,fp);
		fwrite(shoot_count[11],sizeof(int),8,fp);
		fwrite(attack[11],sizeof(int),8,fp);
		fwrite(attack_count[11],sizeof(int),8,fp);
		fwrite(jump[11],sizeof(int),8,fp);
		fwrite(jump_count[11],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\hwarang.ai2","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[12],sizeof(int),8,fp);
		fwrite(shoot_count[12],sizeof(int),8,fp);
		fwrite(attack[12],sizeof(int),8,fp);
		fwrite(attack_count[12],sizeof(int),8,fp);
		fwrite(jump[12],sizeof(int),8,fp);
		fwrite(jump_count[12],sizeof(int),8,fp);
		fclose(fp);
	}
	if((fp==fopen("AI\\livingamor.ai2","wb")))
	{
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(shoot[13],sizeof(int),8,fp);
		fwrite(shoot_count[13],sizeof(int),8,fp);
		fwrite(attack[13],sizeof(int),8,fp);
		fwrite(attack_count[13],sizeof(int),8,fp);
		fwrite(jump[13],sizeof(int),8,fp);
		fwrite(jump_count[13],sizeof(int),8,fp);
		fclose(fp);
	}
}