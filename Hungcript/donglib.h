//�����̺� 080519 : ���������� ���� �����ӿ�ũ

#ifndef _DONGLIB_
#define _DONGLIB_

#include <dsound.h>
#include <time.h>
#include <gdiplus.h>
#include <vfw.h>
#include <digitalv.h>

#include "JDirectDraw.h"
#include "Dsutil.h"
#include "JResourceManager.h"
#include "resource.h"
#include "Ogg.h"

#include <vector>
#include <map>

using namespace std;

/*--------------------------------
			���� �κ�
--------------------------------*/

//���� ������ �ʿ��� �� ���� ����
#define N_ZERO		0
#define N_ONE		1
#define N_TWO		2
#define N_THREE		3
#define N_FOUR		4
#define N_FIVE		5

#define SCREEN_X	640
#define SCREEN_Y	480

//���Ǵ� Ű������ Ű ����
#define ALL_KEYS 94

//�ѱ� �Է� ���� ����
#define STR_MAX	80

//Ű ���� �ޱ�
#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))

/*--------------------------------
		���� ���� ����κ�
--------------------------------*/

//������ ����
extern HWND hwnd;
extern BOOL activate;		//������ Ȱ��ȭ ����
extern BOOL gameover;		//���� ���� �÷���

//�����̺� ���� ����
extern JDirectDraw* jdd;
extern JResourceManager* jre;
extern char* backbuffer;
extern JFont global_font;

//���� ���� ����
extern LPDIRECTSOUND       SoundOBJ;
extern LPDIRECTSOUNDBUFFER SoundDSB;
extern DSBUFFERDESC        DSB_desc;

extern BOOL ReplayFlag;

//���� ���� ����
extern BOOL keyboard_control;	//Ű���� ��� ����
extern BOOL mouse_control;	//���콺 ��� ����
extern int MouseX, MouseY;	//���콺 ��ǥ
extern BOOL LButton, RButton;	//���콺 Ŭ������
extern BOOL mouse_move;	//���콺 �̵� ����

/*--------------------------------
		���� �Լ� ����κ�
--------------------------------*/

//�ʱ�ȭ �۾�
BOOL MainInitialize(char* window_name, BOOL use_keyboard, BOOL use_mouse, bool window_mode=false);

//������ �޼��� ó��
LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL ProcessMessage();
int GetFPS();

//���� ���� �Լ�
BOOL _MidiPlay(char* pszMidiFN, BOOL bReplayFlag = TRUE);
BOOL _MidiStop();
BOOL _MidiReplay();
void _Play( HSNDOBJ sound );
DWORD _PlayAVI( char* name );

//�ִ� �ּҰ�
int Max(int x, int y);
int Min(int x, int y);
int MaxMin(int x, int min, int max);
BOOL Between(int value, int min, int max);

//��ȣȭ�� ���� ���ڿ��� ��ȣ�� �˾Ƴ��� �Լ�
int get_char_num(char chr);

//ȭ�� �����
void ClearScreen();

//���̵� �ƿ� / ��
void FadeOut(int delay=0);
void FadeIn(int delay=0);
void WhiteOut(int delay=0);
void WhiteIn(int delay=0);

//Ű ó��
enum ValidKey	//���Ǵ� Ű
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

//���콺 ��ư ó��
BOOL LeftDown();
BOOL RightDown();

//HY�Ÿ����϶� �� ĳ���͵��� ���̸� �����ִ� �Լ�
int GetCharLength(char tmp_char);

//�ѱ� �Է� Ŭ����
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

extern CHan m_han;	//�ٸ� Ŭ�������� ����ϱ� ���� ����

//�⺻ ��ȭ ó�� Ŭ����
class CDlg
{
protected:
	//�ڽ��� ID
	int id;

	//�ʱ�ȭ �Ǿ������� Ȯ��
	BOOL setting, setting2;

	//��ȭâ ����
	unsigned int buffer_size;
	int n_of_e;						//�� �ٲ� Ƚ��
	int x, y, width, line;			//X,Y ��ġ, ���� ũ��, ���� �� ��
	BOOL b_shadow;					//�׸��ڸ� �ִ��� ����
	char* dlgbox;					//��ȭâ �׸�
	BOOL show_dlgbox;				//��ȭâ�� ǥ���ϴ��� ����
	JColor color;					//���ڻ�
	int frame_x, frame_y;			//��ȭâ Ʋ�� ����,���� ũ��(�⺻ 5, ������ ��ȭâ�� ���� ����)
	float opacity;					//â ����

	//��� ����� ���� ����
	int length;

	JFont* text_font;

	void DrawDlg();					//��ȭâ ���̱�
	BOOL MouseInBox();				//���콺�� ��ȭ���ڿ� �ִ°��� �˻�

public:
	//������
	CDlg();
	~CDlg();

	void SetDlg();											//��ȭâ ����
	void Move(int mov_x, int mov_y);						//��ȭâ �̵�
	void ShowDlg()					;						//�� ���� ���̱�
	void HideDlg();											//�� ���� �����
	
	void ShowShadow(bool shadow);							//�׸��� ���O
	void ShowShadow();										//�׸��� ���O
	void HideShadow();										//�׸��� ���X
	
	int GetX(){return x;}									//X��ǥ ��������
	int GetY(){return y;}									//Y��ǥ ��������
	int GetWidth(){return width*10;}						//���� ũ�� ��������
	int GetLine(){return line;}								//���� ũ��(�� ��) ��������
	float GetOpacity(){return opacity;}						//������ ��������

	void SetX(int vx){x = vx;}								//X��ǥ ����
	void SetY(int vy){y = vy;}								//Y��ǥ ����
	void SetFont(JFont* font);								//��Ʈ ����
	void SetColor(BYTE r, BYTE g, BYTE b);					//���ڻ� ����
	void SetOpacity(float opacity);							//��ȭâ ������ ����
	void SetFrameSize(int vx, int vy);						//��ȭâ Ʋ ũ�� ����

	void SetDlgBox(char* box_name);							//�ۻ��� ����
	void MakeDlgBox(char* pic_dlg);							//�ۻ��� �����
};

//����� ���� ��ȭ����
class CTextDlg:public CDlg
{
private:
	char* dlg_buffer;
	char* text_buffer;
	BOOL lock;
	int tp, sp;			//�ѱ��ھ� ����ϱ� ���� ������, tp�� ���� ��µǴ� ��ȭâ�� ������, sp�� ��ü ������ ������
	BOOL ani_end;		//���ڰ� ��µǴ� �ִϸ��̼��� �������� ����

public:
	//������ �Ҹ���
	CTextDlg(unsigned int vbuffer_size=1024);
	~CTextDlg();

	void SetDlg(int vx, int vy, int vwidth, int vline);
	void MakeText(char* content);
	void Print(char* content);
	int Printing();

	void Lock();		//â�� �� �ݰ� ��
	void UnLock();		//â�� ���� �� �ְ� ��
};

//������ ���� ��ȭ����
class CSelectDlg:public CDlg
{
private:
	int select;
	int gap;
	int sx, sy;
	char* dlg_buffer;
	char** select_point;
	BOOL row;

public:
	//������ �Ҹ���
    CSelectDlg(int select_max=64, unsigned int vbuffer_size=1024);
	~CSelectDlg();

	//���� �������̵�
	void SetDlg(int x, int y, int width, int line, BOOL row=FALSE, int start=0);

	//���� ���� �޼ҵ�
	void MakeSelection(char* input_dlg);
	int Selecting();
	int Select(char* input_dlg, BOOL cancel=FALSE);
	void SetSelect(int select);
};

//�Է��� ���� ��ȭ����(�ܹ� ������ �Է��ϱ� ���� ���� ���۸� ������ ����)
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
	//������
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

//��� ó�� Ŭ����
class CCommand
{
	char** source;
	char commands[1024];

	int command_max;
	int count;
	int com_id[64];

	int GetComID(int no);

public:
	CSelectDlg comdlg;

	void Init(char* source[], int max=64);
	void AddComs(int n, ...);
	void AddCom(int id);
	void AddComStr(int id, char* str);
	int CommandSelect();
	int CommandSelecting();
	BOOL IsFull();
	BOOL Empty();

	int GetCount();

	//������
	CCommand(char* source[], int x, int y, int width, int line, BOOL row=FALSE, int max=64);
};

//���� ó�� Ŭ����
class CFiles
{
public:
	vector<char*> filename;
	void AddFile(char* name);
	void SearchFile(char* dir, char* filename);

	//������ �Ҹ���
	CFiles();
	~CFiles();
};

//�ִϸ��̼� ������ Ŭ����
enum AniType	//�ִϸ��̼� Ÿ��
{
	ani_normal,
	ani_loop,
	ani_exchange,
	ani_once
};

enum WalkDirection	//�ȴ� ����
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
	//���� ��
	int x_size, y_size;
	BOOL walk;					//���� ����(�⺻��: FALSE)
	int up, down, left, right;	//���� �׷����� ���� �����¿� �ִϸ��̼� �׸��� ��ġ(Y��ǥ�� ����)
	char type;
	int frame_max;
	int delay_max;

	//�ڽ� ���� ��
	int frame;
	int delay;
	int y_point;

public:
	RECT ani_rect;

	void Set(int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
	void Process();
	void SetFrame(int vframe);
	void Reset();		//�ִϸ��̼��� ����������
	int GetXSize();
	int GetYSize();

	//�����¿� �ȱ� ����
	void SetWalk(int up_ypos, int down_ypos, int left_ypos, int right_ypos);
	void SetDirection(WalkDirection dir);

	//�׸���
	void Draw(char* dest, char* src, int x, int y, int frame=-1);
	void Draw(int dest, int src, int x, int y, int frame=-1);
	void DrawEx(char* dest, char* src, int x, int y, DWORD dwFlags, int frame=-1);
	void DrawEx(int dest, int src, int x, int y, DWORD dwFlags, int frame=-1);

	//������
	CAnimationData(int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
};

//�ִϸ��̼� ���� Ŭ����
class CAnimation
{
private:
	map<int, CAnimationData*> anidata;

public:
	void CreateAnimation(int id, int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
	void Process();	//�ִϸ��̼��� �����ϴ� �Լ�(�̰��� �������� ������ �������� ����)
	CAnimationData* GetAni(int id);

	//������ �Ҹ���
	CAnimation();
	~CAnimation();
};

//��ã�� Ŭ����
#define MOVE_DISABLE	9999

typedef struct
{
	int x;
	int y;
}MapPoint;

class CSearchMap
{
private:
	int* map;			//�˻��� ��
	int** roadmap;		//���� �˻��ϴ� ��
	int** distancemap;	//�Ÿ��� ��Ÿ���� ��
	MapPoint point;		//���� �繰�� �ִ� ��ġ

	int x_size;			//���� ũ��
	int y_size;

	void FindRoad(int** vmap, int x, int y, int expense, int move_max=-1);

public:
	CSearchMap(int* p_map, int x, int y);
	~CSearchMap();

	void SetStartPoint(MapPoint p, int move_max);	//������ġ ����
	void SetStartPoint(int x, int y, int move_max);
	void SetEndPoint(MapPoint p);					//�������� ����
	void SetEndPoint(int x, int y);

	BOOL MoveNextPointFast(int move);			//���� ����� ���� ��ġ�� �̵�
	BOOL MoveNextPoint(int move);				//���� ����� ���� ��ġ�� �ڿ������� �̵�
	BOOL MoveNextPointStupid(int move, int iq);	//�ణ ��û�ϰ� ���� ã�ư�
										//iq : 1~10����, ���� ���� ��û��

	int GetRoadMap(int x, int y);				//RoadMap�� ��������
	int GetDistanceMap(int x, int y);			//DistanceMap�� ��������
	int GetPointX();
	int GetPointY();
};

#endif