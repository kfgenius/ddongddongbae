#define WHITE JColor(255,255,255)
#define BLACK JColor(0,0,0)
#define RED JColor(255,0,0)
#define GREEN JColor(0,255,0)
#define BLUE JColor(0,0,255)
#define YELLOW JColor(255,255,0)

#define SMAX 25
#define BATTLE_STORY  Click(); DrawMap();

JFont font20;
HSNDOBJ Sound[18];

char *BattleBack[]={
	"DATA//back6.jpg","DATA//back5.jpg","DATA//back4.jpg",
	"DATA//back3.jpg","DATA//back2.jpg","DATA//back1.jpg",
	"DATA//back1.jpg","DATA//back1.jpg","DATA//back7.jpg"};
char *MapBMP[]={
	"DATA//map1.jpg","DATA//map2.jpg","DATA//map3.jpg","DATA//map4.jpg",
	"DATA//map5.jpg","DATA//map6.jpg","DATA//map7.jpg","DATA//map8.jpg"};
char *Carac[]={
	"DATA//Cbang.png","DATA//Cbate.png","DATA//Cbow.png",
	"DATA//Cdark.png","DATA//Cdeath.png","DATA//Cdhb.png",
	"DATA//Cknight.png","DATA//Cgold.png","DATA//Cmage.png",
	"DATA//Ccatapult.png","DATA//Carmor.png","DATA//Cmonk.png"};
int Matt[]={2,5,3,4,0,0,0,0,0,0,0,0,0,0,0,0,2,4,5,4,3,0};	//공격
int Mdef[]={1,0,3,2,0,0,0,0,0,0,0,0,0,0,0,0,1,2,0,1,5,0};	//방어
int Mmov[]={4,3,2,5,4,5,0,0,0,0,0,0,0,0,0,0,3,5,5,0,2,4};	//이동
int Magl[]={1,5,1,3,5,5,0,0,0,0,0,0,0,0,0,0,1,3,3,1,1,5};	//민첩

int Smov[]={2,3,4,5,2,2,0,0,0,0,0,0,0,0,0,0,5,5,2,2,4,2};	//걷는 소리

char map_tile[16][16];	//지형
char map_spr[16][16];	//말들의 위치
char map_move[16][16];	//이동 검사 판
char map_move2[16][16];	//역 이동 검사 판

int tile_expend[]={1,99,2,1,1,2,1,2,2,1,99,2,99,99};	//지형에 소비되는 이동력
char tile_high[]={0,3,1,2,0,1,0,1,1,0,1,1,0,0};	//높이

char* cGeneral[]={"쿠오오3세","과묵","초쨉실","100프로","왕미모","은마녀","자머","뚜엑","슈하","안콥","칼","아들내미"};
char* cName[]={"방갈","파마배추","큰바위","암흑기사","에야디야","DHB","","","","","","","","","","","병사","기사","미지인","발석차","중장보병","승려"};
char* cStage[]={"쏘갈요새","칼날계곡","죽음의 늪","마왕성","카베사막","헤멤의 숲","해물산맥","성"};

char *snr1[]={
	"0","용서할 수 없다! 인간 놈들!","감히 날 봉인하고 마족을 괴롭히다니!","나같은 평화주의자로 하여금","전쟁을 하게 하는군!","",//0
	"1","용사의 침입으로 인해","우리의 영토의 저주가 풀렸습니다.","다시 땅에 저주를 걸어야 합니다.","그렇게 하면 다시 각 영토에서","마물이 번성케 될 겁니다.",
	"2","헤헤헷, 인간놈들 이 기회에","다 죽여 버리자고요!","","","",
	"2","(앗, 내가 이런 끔찍한 말을...","에잇, 이놈의 입!!!)","","","",
	"1","인간들이 잘못한 것은 사실이나","몰살은 안 돼.","나쁜 인간은 상위계층의 몇 명 뿐이다.","일반인에게 피해가 가선 안 돼.","",
	"0","그래, 그 말이 맞다!","나는 평화주의자,","전쟁은 별로 원하지 않는다.","헤멤의 숲까지 차지하는 것이","우리의 목표다!",//5
	"0","우선 가장 가까운 쏘갈요새다.","쏘갈요새를 되찾는 것이다.","","","",
	"0","흠, 병기라는 것이 어딨자?","흠, 저건가 보군.","","","",
	"3","위잉~ 위이잉~","마족의 마력이 느껴진다.","동면상태 해체.","","",
	"0","네가 이 요새의 고대병기냐?","","","","",
	"3","그렇다. 그런데 당신은 누군가?","","","","",//10
	"0","나는 마왕, 쿠오오 3세다.","","","","",
	"3","쿠오오 3세...","쿠오오2세의 아들인가?","좋습니다.","이제부터 마왕님을 따르겠습니다.","",
	"0","그런데 너는 언제부터 있었느냐?","","","","",
	"3","자세한 기억은 없습니다.","하지만 쿠오오 1세님 이전부터","전 있었습니다.","제 안에 시간기억장치에 의해면","약 3000년 전부터 군요.",
	"0","3000년?","그렇다면 마왕시대가 열리기","이전일지도 모르겠군.","기억이 없다니 소용은 없지만.","", //15
	"0","좋아, 그럼 우리 마왕군이 되어","인간군과 싸우는 거다!","자, 다음은 칼날 계곡이다.","","",
	"8","크윽... 역부족이었나...","","","","",
	"0","이제 어둠의 대륙을 지났군.","다음은 죽음의 늪으로 출발이다!","","","",
	"6","잠깐만요. 호호홋!","","","","",
	"0","웃! 뭐냐!!!","","","","",//20
	"6","이 근처 사는 마녀입니다.","마왕님의 인간군 정벌을 도와","같이가고자 합니다.","","",
	"0","후후... 좋다. 허락하지!","","","","",
	"2","마왕님, 이런 정체를 알 수 없는","여자를...","(그런데 되게 예쁘다.)","","",
	"0","예쁘면 모든 것이 허락된다!","크하하핫!","","","",
	"0","응, 누구지? 저들은...","인간의 미지인들이군.","아직 남아 있었다니...","","",//25
	"4","아니, 아니, 공격말아요!","우리가 인간이긴 하지만","마녀로 이곳에 추방당한","신세라고요.","",
	"4","오히려 우릴 추방한 인간들에게","복수하기 위해 마왕님에게","힘이 되어 드리겠습니다.","","",
	"0","좋다. 허락하지.","그 옆에 여자는...?","","","",
	"5","...은마녀라고 합니다...","","","","",
	"4","오호호호호홋!","저와 같이 추방당했죠.","원래 말이 없는 성격이예요.","오호호홋!","",//30
	"2","재수없어. 아줌마!","(너무 무례했나?)","마왕님, 저들은 인간입니다.","정말 받아들이시는 겁니까?","",
	"0","후후, 내 가슴은 바다같이 넓어","모든 것을 수용하지.","마왕군의 특징이라면","많은 장수들 아니겠나?","오히려 더 많이 받아들여야지.",
	"2","앗! 원소정찰자가 돌아왔습니다.","뭐라고?","마왕님 큰일입니다!","마왕성에 인간군이 기습부대를","보낸 모양입니다.",
	"0","흥! 그런 속셈이었군.","멍청한! 우리의 공간미지를 모르는가?","바로 마왕성으로 공간이동이다!","","",
	"9","역시... 무리였나...","","","","",//35
	"0","그럼 다시 혼돈대륙으로 갈까?","","","","",
	"7","잠깐 기다리십쇼.","쿠오오 3세님이시군요.","","","",
	"0","누구냐? 넌...","","","","",
	"7","어둠의 구멍 탐색 업무를","마치고 돌아왔습니다.","쿠오오 2세님은 물러나셨나보군요.","","",
	"0","혹시... 뚜엑? 100년만이군.","마침 잘 됐군.","힘을 빌려주게.","지금 헤멤의 숲을 점령하러","가는 중이다.",//40
	"7","혼돈대륙에 가신다는 겁니까?","그건 쿠오오 1세님께서","금지하신 일일 텐데...","","",
	"0","하지만 용서할 수 없다.","어차피 인간을 멸망시키는 건 아니다.","우리의 영토를 되찾는 것이다.","","",
	"7","그렇군요.","4대 미지지역은 혼돈이지만","우리의 영역.","그럼 저도 돕겠습니다.","",
	"0","헉헉... 더웠다.","다음은 숲이니 좀 괜찮겠지?","","","",
	"10","이런 젠장...","두고보자!!!","","","",//45
	"1","이것으로 목적은 달성했군요.","헤멤의 숲도 점령했으니","마왕성으로 돌아갑시다.","","",
	"0","무슨 소리냐! 저 놈을 잡아야지!","후환을 남겨둬선 안 돼!","","","",
	"1","하지만 저주만 걸면 이 숲은","미알이 넘치는 땅이 되어","스스로를 지킬 겁니다.","","",
	"0","어쨌든 후환은 놔두면 안돼!","칼의 후손이야!","아주 옛날 대반란을 방해한","그 썩을 놈의 후손이란 말이다!","",
	"1","알겠습니다.","(마왕님이 조금 변한 것 같군.","왠지 불길해...)","","",//50
	"1","이것으로 모든게 끝났군요.","","","","",
	"0","후후후... 끝?","아니, 아직 안 끝났어.","성을 공격해서 인간 모두를","점령해야 겠다!","",
	"1","무슨 소리입니까?","처음한 말과 틀리잖습니까?","헤멤의 숲까지만...","","",
	"0","닥쳐라!","이제야 난 마왕의 본분을 깨달았다.","인간들을 지배하는 이 기쁨.","이것이 진정한 마왕이야.","",
	"0","평화주의자?","무슨 헛소리...","자! 싸우자! 마족 답게!!!","","",//55
	"7","하지만 쿠오오 1세님께선","인간과의 전쟁을 금지하셨습니다.","","","",
	"0","상관없어!","어차피 친 할아버지도 아니잖아.","그가 인간이었기에","그런 명령을 한 거다!","",
	"1","쿠오오 마왕가이신 분께서","쿠오오 1세님을 그런식으로","함부로 말씀하시다니!!!","","",
	"0","어쨌든 중요한 것은 이거다!","지금은...","나의 시대다!!!","","",
	"1","...","좋습니다. 그렇다면...","따를 수 밖에...","무사로서 충성을 맹세했으니","배신할 순 없죠.",//60
	"2","으헤헤헷!","인간들을 다 죽입시다!","(다 죽이는 건 너무한데...","설마 진짜 그러진 않겠지?)","",
	"11","크흑... 강하군...","","","","",
	"0","후훗, 용사로서 목숨을 걸고","끝까지 저항할 생각인가?","","","",
	"11","후후후... 당연히...","","","","",
	"11","마왕군에 붙겠습니다!","잘 봐주세요!","열심히 하겠습니다.","","",//65
	"0","헛소리 하지마!","","","","",
	"11","젠장, 안 통하는군!","도망치자!!!","","","",
	"0","으하하하핫!","겁쟁이...","저런 놈은 쫓을 필요도 없지.","","",
	"0","이것으로 이제 혼둠은 우리 세상이다!","","","","",
	};

BOOL Quit=false, key, to_menu=false;
int mode=0, Frame=0, RFrame=0, tab=0, Select=0;
int focusX, focusY;
RECT BackRect;

typedef struct{
	BOOL agree[7];
	int monster[6];
	int mp , turn, score;
	int stage, idea, fear;
}Devil;
Devil devil;

typedef struct{
	//입력
	int hieght[2];
	int frame[2];
	int width[2][5];
	int center[2][5];

	//연산 후 나오는 결과
	int x[2][5];
	int y[2];
}CaraData;
CaraData cara_data[12];

typedef struct{
	int x, state, kind, frz;		//표시될 중심위치, 상태, 종류, 멈춤
	int unit;						//몇 번 부대, 데미지 지연 시간
	bool end_action, attack, fight;	//동작의 끝을 알리는 플래그, 공격 횟수 제한, 전급전 가능
	int frame, real_frame, tempo;	//표시되는 프레임과 기타 거기에 필요한 변수
	int soundtempo;
	int dam;						//공격에 의한 충격
}Cara;
Cara cara[2];

typedef struct
{
	BOOL life;
	int state, delay, lv, idea, bear;
}Follower;
Follower foll[7];

typedef struct
{
	bool life, able, turn;			//존재여부, 출전가능(아군부대만 해당), 턴 종료
	int x, y;						//부대위치
	int force, kind, general, hp;	//부대 수, 종류, 지휘관, 체력
	int att, def, mov, agl;		//공격, 방어, 이동, 민첩
	int poison, death, laser, recovery, monster;	//특수 능력
	bool poisoned;	//상태이상
}Units;
Units unit[SMAX];

void PutFontOutline(int x, int y, JColor jc, char* text)
{
	jdd->DrawText(backbuffer,text,font20,x+1,y,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font20,x-1,y,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font20,x,y+1,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font20,x,y-1,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font20,x,y,jc);
}

void LoadPic(int num, char* stream, bool transparent=true)
{
	jdd->DeleteSurface(StrAdd("P%d", num));

	if(transparent)
	{
		JPictureInfo pi;
		pi.SetColorKey(JColor(0,0,255));
		jdd->LoadPicture(StrAdd("P%d", num),stream,&pi,true);
	}
	else jdd->LoadPicture(StrAdd("P%d", num),stream, NULL, true);
}

void _DrawBmp(RECT* rect, int x, int y, int num)
{
	if(num < 0 || num >9)return;

	if(rect)jdd->DrawPicture(backbuffer,StrAdd("P%d", num),x,y,rect);
	else jdd->DrawPicture(backbuffer,StrAdd("P%d", num),x,y,NULL);
}

void _Pixel( int x, int y, JColor jc)
{
	JBrush tmp_brush;
	tmp_brush=jdd->CreateBrush(jc);
	jdd->DrawLine(backbuffer,tmp_brush,x,y,x,y,1);
	jdd->DeleteBrush(tmp_brush);
}

void _DrawBox( int x1, int y1, int x2, int y2, JColor jc)
{
	JBrush tmp_brush;
	tmp_brush=jdd->CreateBrush(jc);
	RECT tmp_rect;
	SetRect(&tmp_rect,x1,y1,x2,y2);
	jdd->DrawRect(backbuffer,tmp_brush,&tmp_rect,1);
	jdd->DeleteBrush(tmp_brush);
}

void _DrawBar( int x1, int y1, int x2, int y2, JColor jc)
{
	JBrush tmp_brush;
	tmp_brush=jdd->CreateBrush(jc);
	RECT tmp_rect;
	SetRect(&tmp_rect,x1,y1,x2,y2);
	jdd->DrawRect(backbuffer,tmp_brush,&tmp_rect);
	jdd->DeleteBrush(tmp_brush);
}

void _DrawBarAlpha( int x1, int y1, int x2, int y2, JColor jc)
{
	JBrush tmp_brush;
	tmp_brush=jdd->CreateBrush(jc,0.5f);
	RECT tmp_rect;
	SetRect(&tmp_rect,x1,y1,x2,y2);
	jdd->DrawRect(backbuffer,tmp_brush,&tmp_rect);
	jdd->DeleteBrush(tmp_brush);
}
