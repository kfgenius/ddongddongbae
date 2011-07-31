//�����̺� 080519 : ���������� ���� �����ӿ�ũ

#pragma once

#include <dsound.h>
#include <time.h>
#include <gdiplus.h>
#include <vfw.h>
#include <digitalv.h>

#include "JDirectDraw.h"
#include "Dsutil.h"
#include "Ogg.h"

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

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

//���Ǵ� Ű������ Ű ����
#define ALL_KEYS 94

//Ű ���� �ޱ�
#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))

//����
#define BLACK	JColor(0, 0, 0)
#define RED		JColor(255, 0, 0)
#define GREEN	JColor(0, 255, 0)
#define BLUE	JColor(0, 0, 255)

/*--------------------------------
		���� ���� ����κ�
--------------------------------*/

//������ ����
extern HWND hwnd;
extern BOOL activate;		//������ Ȱ��ȭ ����
extern BOOL gameover;		//���� ���� �÷���

//�����̺� ���� ����
extern JDirectDraw* jdd;
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
BOOL MainInitialize(char* window_name, BOOL use_keyboard, BOOL use_mouse, bool window_mode=FALSE);

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