//동라이브 071215 : 게임제작을 위한 프레임워크

#ifndef _DONGLIB_
#define _DONGLIB_

#include <dsound.h>
#include <time.h>
#include <gdiplus.h>
#include <dxerr9.h>
#include <dmusicc.h>
#include <vfw.h>
#include <digitalv.h>

#include "JDirectDraw.h"
#include "Dsutil.h"
#include "JResourceManager.h"
#include "resource.h"
#include "Ogg.h"
#include "dxutil.h"
#include "dmutil.h"

#include <vector>
#include <map>

using namespace std;

/*--------------------------------
			정의 부분
--------------------------------*/

//뭔가 구분이 필요할 때 쓰는 숫자
#define N_ZERO		0
#define N_ONE		1
#define N_TWO		2
#define N_THREE		3
#define N_FOUR		4
#define N_FIVE		5

#define SCREEN_X	800
#define SCREEN_Y	600

//사용되는 키보드의 키 숫자
#define ALL_KEYS 94

//한글 입력 관련 정의
#define STR_MAX	80

//키 상태 받기
#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))

/*--------------------------------
		전역 변수 선언부분
--------------------------------*/

//윈도우 변수
extern HWND hwnd;
extern MSG msg;
extern BOOL activate;		//윈도우 활성화 여부
extern BOOL gameover;		//강제 종료 플래그

//문라이브 관련 변수
extern JDirectDraw* jdd;
extern JResourceManager* jre;
extern char* backbuffer;
extern JFont global_font;

//사운드 관련 변수
extern LPDIRECTSOUND       SoundOBJ;
extern LPDIRECTSOUNDBUFFER SoundDSB;
extern DSBUFFERDESC        DSB_desc;

extern BOOL SoundCard;	//사운드 카드 존재 여부
extern BOOL ReplayFlag;

//조작 관련 변수
extern BOOL keyboard_control;	//키보드 사용 여부
extern BOOL mouse_control;	//마우스 사용 여부
extern int MouseX, MouseY;	//마우스 좌표
extern BOOL LButton, RButton;	//마우스 클릭여부
extern BOOL left_click, right_click;	//마우스 클릭은 한번만 검사하기 위한 변수

/*--------------------------------
		전역 함수 선언부분
--------------------------------*/

//초기화 작업
BOOL MainInitialize(char* window_name, BOOL use_keyboard, BOOL use_mouse, BOOL window_mode=FALSE);

//윈도우 메세지 처리
LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL ProcessMessage();
int GetFPS();

//사운드 관련 함수
BOOL _MidiPlay(char* pszMidiFN, BOOL bReplayFlag = TRUE);
BOOL _MidiStop();
BOOL _MidiReplay();
void _Play( HSNDOBJ sound );
DWORD _PlayAVI( char* name );

//최대 최소값
int Max(int x, int y);
int Min(int x, int y);
int MaxMin(int x, int min, int max);
BOOL Between(int value, int min, int max);

//문자열 처리 함수
char* StrAdd(char* msg, ...);
//숫자 선택지 만들기
void MakeNumSelect(int from, int to, int step=1);

//암호화를 위한 문자열의 번호를 알아내는 함수
int get_char_num(char chr);

//잠시 정지
void Pause();

//화면 지우기
void ClearScreen();

//페이드 아웃 / 인
void FadeOut(int delay=0);
void FadeIn(int delay=0);
void WhiteOut(int delay=0);
void WhiteIn(int delay=0);

//키 처리
enum ValidKey	//사용되는 키
{
	vkey_up, vkey_left, vkey_right, vkey_down, vkey_enter,
	vkey_esc, vkey_space, vkey_ctrl, vkey_alt, vkey_tab,
	vkey_num0, vkey_num1, vkey_num2, vkey_num3, vkey_num4,
	vkey_num5, vkey_num6, vkey_num7, vkey_num8, vkey_num9,
	vkey_0, vkey_1, vkey_2, vkey_3, vkey_4,	vkey_5, vkey_6, vkey_7, vkey_8, vkey_9,
	vkey_a, vkey_b, vkey_c, vkey_d, vkey_e, vkey_f, vkey_g, vkey_h, vkey_i,	vkey_j,
	vkey_k, vkey_l, vkey_m, vkey_n, vkey_o, vkey_p, vkey_q, vkey_r,	vkey_s, vkey_t,
	vkey_u, vkey_v, vkey_w, vkey_x, vkey_y, vkey_z,
	vkey_shift, vkey_back, vkey_pause,
	vkey_insert, vkey_delete, vkey_home, vkey_end, vkey_pageup, vkey_pagedown,
	vkey_add, vkey_subtract, vkey_multiply, vkey_divide, vk_decimal,
	vkey_f1, vkey_f2, vkey_f3, vkey_f4, vkey_f5, vkey_f6,
	vkey_f7, vkey_f8, vkey_f9, vkey_f10, vkey_f11, vkey_f12,
	vkey_colon, vkey_plus, vkey_comma, vkey_minus, vkey_period,
	vkey_question, vkey_wave, vkey_bracketopen,
	vkey_won, vkey_bracketclose, vkey_quotation
};

const int keys[ALL_KEYS]={
	/*0*/	VK_UP, VK_LEFT, VK_RIGHT, VK_DOWN, VK_RETURN,
	/*5*/	VK_ESCAPE, VK_SPACE, VK_CONTROL, VK_MENU, VK_TAB,
	/*10*/	VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4,
	/*15*/	VK_NUMPAD5, VK_NUMPAD6,	VK_NUMPAD7, VK_NUMPAD8,	VK_NUMPAD9,
	/*20*/	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	/*30*/	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	/*40*/	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	/*50*/	'U', 'V', 'W', 'X', 'Y', 'Z',
	/*56*/	VK_SHIFT, VK_BACK, VK_PAUSE,
	/*59*/	VK_INSERT, VK_DELETE, VK_HOME, VK_END, VK_PRIOR, VK_NEXT,
	/*65*/	VK_ADD, VK_SUBTRACT,VK_MULTIPLY, VK_DIVIDE, VK_DECIMAL,
	/*70*/	VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6,
	/*76*/	VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,
	/*82*/	VK_OEM_1, VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PERIOD,
	/*87*/	VK_OEM_2, VK_OEM_3, VK_OEM_4,
	/*90*/	VK_OEM_5, VK_OEM_6,VK_OEM_7, VK_OEM_8
};

BOOL GetKey(ValidKey key, int delay=-1);

//마우스 버튼 처리
BOOL LeftDown();
BOOL RightDown();

//HY신명조일때 각 캐릭터들의 길이를 돌려주는 함수
int GetCharLength(char tmp_char);

//한글 입력 클래스
class CHan
{
private:
	int mark[20];
	char han[STR_MAX];

	unsigned int FindMark(char eng);

public:
	char* EngToHan(char* text, char* han_area=NULL);

	CHan();
};

extern CHan m_han;	//다른 클래스에서 사용하기 위해 선언

//기본 대화 처리 클래스
class CDlg
{
protected:
	//초기화 되었는지를 확인
	BOOL setting, setting2;

	//대화창 정보
	unsigned int buffer_size;
	int n_of_e;						//줄 바꿈 횟수
	int x, y, width, height, line;
	int x_size, y_size;
	BOOL b_shadow;					//그림자를 넣는지 여부
	char* dlgbox;					//대화창 그림
	BOOL show_dlgbox;
	JColor color;					//글자색
	int frame_x, frame_y;			//대화창 틀의 가로,세로 크기(기본 5, 임의의 대화창을 쓸때 조절)

	//대사 출력을 위한 변수
	int tp, sp;			//한글자씩 출력하기 위한 포인터, tp는 지금 출력되는 대화창의 포인터, sp는 전체 내용의 포인터
	BOOL ani_end;
	int length;

	JFont* text_font;

	void DrawDlg();		//대화창 보이기
	BOOL MouseInBox();	//마우스가 대화상자에 있는가를 검사

public:
	//생성자
	CDlg();

	void SetDlg();											//대화창 설정
	void Move(int vx, int vy);								//대화창 이동
	void ShowShadow(BOOL show_or_hide);						//그림자 사용
	void SetColor(BYTE r, BYTE g, BYTE b);					//글자색 설정
	void SetDlgBox(char* box_name);							//글상자 설정
	void ShowDlg(BOOL show_or_hide);						//글 상자 보이기 or 숨기기
	void SetFont(JFont* font);								//폰트 설정
	void SetOpacity(float opacity);							//대화창 투명도 설정
	void SetFrameSize(int vx, int vy);						//대화창 틀 크기 설정
	int GetX(){return x;}									//현재 X좌표 돌려주기
	int GetY(){return y;}									//현재 Y좌표 돌려주기
	void SetX(int vx){x = vx;}								//현재 X좌표 돌려주기
	void SetY(int vy){y = vy;}								//현재 Y좌표 돌려주기
};

//출력을 위한 대화상자
class CTextDlg:public CDlg
{
private:
	char* dlg_buffer;
	char* text_buffer;
	BOOL lock;

public:
	//생성자 소멸자
	CTextDlg(unsigned int vbuffer_size=1024);
	~CTextDlg();

	void SetDlg(int vx, int vy, int vwidth, int vline);
	void MakeText(char* content);
	void Print(char* content);
	int Printing();

	void Lock();		//창을 못 닫게 함
	void UnLock();		//창을 닫을 수 있게 함
};

//선택을 위한 대화상자
class CSelectDlg:public CDlg
{
private:
	int select;
	int gap, col;
	int sx, sy;
	char* dlg_buffer;
	char** select_point;
	BOOL row;

public:
	//생성자 소멸자
    CSelectDlg(int select_max=64, unsigned int vbuffer_size=1024);
	~CSelectDlg();

	//설정 오버라이드
	void SetDlg(int vx, int vy, int vgap, int vline, BOOL vrow=FALSE, int start=0);

	//선택 관련 메소드
	void MakeSelection(char* input_dlg);
	int Selecting();
	int Select(char* input_dlg, BOOL cancel=FALSE);
	void SetSelect(int select);
};

//입력을 위한 대화상자(단문 정도를 입력하기 위한 작은 버퍼를 가지고 있음)
#define FLAG_TEXT	0x01
#define FLAG_NUMBER	0x02
#define FLAG_MARK	0x04

class CInputDlg:public CDlg
{
private:
	int old_len;
	int text_length;
	int str_max;
	char is_eng_buffer[81];
	BOOL flag;

public:
	//생성자
	CInputDlg(int vx, int vy, int max);
	CInputDlg();

	void SetDlg(int vx, int vy, int max);
	void SetFlag(BOOL vflag);

	char eng_buffer[81];
	char input_text[161];

	void Clear();
	char* Input();
	int Inputing();
};

//명령 처리 클래스
class CCommand
{
	int com_id[64];
	int command_max;
	BOOL row;
	int x, y, gap, line_max;
	char** source;
	char commands[1024];

	int GetComID(int count);

public:
	CSelectDlg comdlg;

	int count;

	void Init(char* vsource[], int max=64);
	void AddComs(int n, ...);
	void AddCom(int id);
	void AddComStr(int id, char* str);
	int CommandSelect();
	int CommandSelecting();
	BOOL IsFull();
	BOOL Empty();

	//생성자
	CCommand(char* vsource[], int vx, int vy, int vgap, int vline, BOOL vrow=FALSE, int max=64);
};

//파일 처리 클래스
class CFiles
{
public:
	vector<char*> filename;
	void AddFile(char* name);
	void SearchFile(char* dir, char* filename);

	//생성자 소멸자
	CFiles();
	~CFiles();
};

//애니메이션 클래스
enum AniType	//애니메이션 타입
{
	ani_normal,
	ani_loop,
	ani_exchange,
	ani_once
};

enum WalkDirection	//걷는 방향
{
	dir_up,
	dir_left,
	dir_down,
	dir_right,
	dir_turn,
	dir_turn_left,
	dir_turn_right,
	dir_random
};

class CAnimationData
{
private:
	int x_size, y_size;
	BOOL walk;					//보행 여부(기본값: FALSE)
	int up, down, left, right;	//보행 그래픽을 위한 상하좌우 애니메이션 그림의 위치(Y좌표를 증가)
	int frame, frame_max;
	int delay, delay_max;
	int y_point;

	char type;

public:
	RECT ani_rect;

	void Set(int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
	void Process();
	void SetFrame(int vframe);
	void Reset();		//애니메이션을 시작점으로
	int GetXSize();
	int GetYSize();

	//상하좌우 걷기 관련
	void SetWalk(int up_ypos, int down_ypos, int left_ypos, int right_ypos);
	void SetDirection(WalkDirection dir);

	//그리기
	void Draw(char* dest, char* src, int x, int y, int frame=-1);
	void Draw(int dest, int src, int x, int y, int frame=-1);
	void DrawEx(char* dest, char* src, int x, int y, DWORD dwFlags, int frame=-1);
	void DrawEx(int dest, int src, int x, int y, DWORD dwFlags, int frame=-1);

	//생성자
	CAnimationData(int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
};

class CAnimation
{
private:
	map<int, CAnimationData*> anidata;

public:
	void CreateAnimation(int id, int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
	void Process();	//애니메이션을 진행하는 함수(이것을 실행하지 않으면 움직이지 않음)
	CAnimationData* GetAni(int id);

	//생성자 소멸자
	CAnimation();
	~CAnimation();
};

//길찾기 클래스
#define MOVE_DISABLE	9999

typedef struct
{
	int x;
	int y;
}MapPoint;

class CSearchMap
{
private:
	int* map;			//검색할 맵
	int** roadmap;		//길을 검색하는 맵
	int** distancemap;	//거리를 나타내는 맵
	MapPoint point;		//지금 사물이 있는 위치

	int x_size;			//맵의 크기
	int y_size;

	void FindRoad(int** vmap, int x, int y, int expense, int move_max=-1);

public:
	CSearchMap(int* p_map, int x, int y);
	~CSearchMap();

	void SetStartPoint(MapPoint p, int move_max);	//시작위치 지정
	void SetStartPoint(int x, int y, int move_max);
	void SetEndPoint(MapPoint p);					//도착지점 지정
	void SetEndPoint(int x, int y);

	BOOL MoveNextPointFast(int move);			//가장 가까운 다음 위치로 이동
	BOOL MoveNextPoint(int move);				//가장 가까운 다음 위치로 자연스럽게 이동
	BOOL MoveNextPointStupid(int move, int iq);	//약간 멍청하게 길을 찾아감
										//iq : 1~10까지, 낮을 수록 멍청함

	int GetRoadMap(int x, int y);				//RoadMap값 가져오기
	int GetDistanceMap(int x, int y);			//DistanceMap값 가져오기
	int GetPointX();
	int GetPointY();
};

/////////////////////////////////////////////////
//DirectMusic
#define MUSIC_VOLUME_RANGE      ( 0-(DMUS_VOLUME_MIN/4) )

class CDirectMusic
{
private:
	CMusicManager*     g_pMusicManager;
	CMusicSegment*     g_pMusicSegment;
	HINSTANCE          g_hInst;
	HANDLE             g_hDMusicMessageEvent;

public:
	HRESULT InitDirectMusic();
	HRESULT LoadSegmentFile(TCHAR* strFileName);
	HRESULT ProcessDirectMusicMessages();
	HRESULT PlayAudio();
	void StopAudio();

	CDirectMusic();
	~CDirectMusic();
};

#endif