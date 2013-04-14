#include <ddraw.h>
#include <dsound.h>

#include "dsutil.h"
#include "JDirectDraw.h"
#include "JResourceManager.h"

#define _GetKeyState(vkey) HIBYTE(GetAsyncKeyState(vkey))

//Ŭ���� ���
#define CLIP_NORMAL	0	//ȭ�� ��ü ���

#define SURFACE_MAX 50

#define KEY_MAX 7

#define TO_RED	 	1
#define TO_GREEN 	2
#define TO_BLUE 	3
#define DARK		4

extern int MouseX, MouseY;							//���콺 ��ǥ
extern BOOL LeftButton, RightButton;	//���콺 Ŭ�� ����

//DirectDraw ��ü
extern JDirectDraw* jdd;
extern JResourceManager* jre;

//�׸��� �׸� ������ �����ִ� Ŭ����
class DrawData
{
public:
	int x;
	int y;
	int id;
	int frame;

	void Set(int vx, int vy, int vid, int vframe=0);
};

//�ִϸ��̼� Ŭ����
class CAnimation
{
private:
	int x_size, y_size;
	bool walk;							//���� ����(�⺻��: false)
	int top, bottom, left, right;		//���� �׷����� ���� �����¿� �ִϸ��̼� �׸��� ��ġ(Y��ǥ�� ����)
	int frame, frame_max;
	int delay, delay_max;

public:
	char type;
	RECT ani_rect;

	CAnimation();
	CAnimation(int x, int y, char vtype=0, int vframe_max=0, int vdelay_max=0);
	void Set(int x, int y, char vtype=0, int vframe_max=0, int vdelay_max=0);
	void Process();
	void SetWalk(int t, int b, int l, int r);
	void SetFrame(int vframe);
};

//////////////////////////////////////////////////////////////////
//���� �۾� Ŭ����
class CHungLib
{
private:
	char* backbuffer;

	JFont font;

	DDSURFACEDESC ddsd;
	DDSCAPS ddscaps;

	//DirectSound ��ü
	LPDIRECTSOUND       SoundOBJ;
	LPDIRECTSOUNDBUFFER SoundDSB;
	DSBUFFERDESC        DSB_desc;

	HSNDOBJ Sound[100];
	BOOL SoundCard;

	//�����̽�
	int surface_count;
	char* surface_name[SURFACE_MAX];
	char* file_name[SURFACE_MAX];
	CAnimation* animation[SURFACE_MAX];

	//DirectDraw���� ����
	int screen_x, screen_y;

	//Ŭ���� �̸�, ������ �̸�
	char* class_name;
	char* window_name;

	//���콺 ���� ����
	bool left_button_push;
	bool right_button_push;

public:
	//DirectDraw ���� �޼ҵ�
	void Image(DrawData ddata);
	void Image(int x, int y, int surface_id, int frame=0);
	void Image(int x, int y, char* name, int frame=0);
	int NextImage(int surface_id);
	int GetImageID(char* name);
	bool Render();

	//DirectSound ���� �޼ҵ�
	void LoadSound();
	void PlaySound(int num);

	//�����̽� ���� �޼ҵ�
	bool LoadDGF(char* name, char* src, JColor key = JColor(0, 0, 0), CAnimation* ani=NULL);
	void RestoreAll();

	//DC ���� �޼ҵ�
	void PutText(RECT* tarRECT, char* text, JColor rgb);
	void PutText(int x, int y, char* text, JColor rgb);

	//�Է� �޼ҵ�
	bool GetKey(int key, int delay=999);
	bool LeftClick();
	bool RightClick();

	//��Ÿ �޼ҵ�
	void Fail(HWND hwnd);	//�������� �� ����������

	//�����ڿ� �Ҹ���
	CHungLib(HWND hwnd, HINSTANCE hInstance, int nCmdShow, char* class_name, char* window_name, int x, int y, bool usm);
	~CHungLib();
};