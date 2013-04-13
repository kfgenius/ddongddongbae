#include <dsound.h>

#include "dsutil.h"

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
	//DirectDraw ��ü
	LPDIRECTDRAW		lpDD;				//���̷�Ʈ ��ο� ��ü
	LPDIRECTDRAWSURFACE	RealScreen;			//Primary Surface ��ü
	LPDIRECTDRAWSURFACE	BackScreen;			//Back Surface ��ü
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
	LPDIRECTDRAWSURFACE	ddsurface[SURFACE_MAX];
	char* surface_name[SURFACE_MAX];
	char* file_name[SURFACE_MAX];
	bool surface_trans[SURFACE_MAX];
	COLORREF surface_colorkey[SURFACE_MAX];
	RECT surface_rect[SURFACE_MAX];
	CAnimation* animation[SURFACE_MAX];

	//DirectDraw���� ����
	bool use_system_memory;
	int screen_x, screen_y;
	RECT clip;

	//DC���� ����
	HDC hdc;
	bool dc_mode;	//DC�� ���� �ִ��� �˱� ���� ��. DC�� ���µ� BLT�� ��ٰ� �ٿ�

	//Ŭ���� �̸�, ������ �̸�
	char* class_name;
	char* window_name;

	//���콺 ���� ����
	bool left_button_push;
	bool right_button_push;

	//*** Private �޼ҵ� ***//
	//ddutil
	DWORD DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb);
	HRESULT DDSetColorKey(IDirectDrawSurface *pdds, COLORREF rgb);

	//DC ���� �޼ҵ�
	void BeginDC();
	void EndDC();

public:
	//DirectDraw ���� �޼ҵ�
	void Image(DrawData ddata);
	void Image(int x, int y, int surface_id, int frame=0);
	void Image(int x, int y, char* name, int frame=0);
	int NextImage(int surface_id);
	int GetImageID(char* name);
	bool Render();
	WORD* GetSurfaceAddress(int surface_id);
	void SetClip(int left, int top, int right, int bottom);
	void SetClip(int mode);
	void SaveImage(int id);

	//DirectSound ���� �޼ҵ�
	void LoadSound();
	void PlaySound(int num);

	//�����̽� ���� �޼ҵ�
	bool LoadDGF(char* name, char* src, COLORREF key=NULL, CAnimation* ani=NULL);
	void SurfaceCopy(char* name, char* src);
	void ColorChange(int surface_id, WORD from, WORD to, WORD from2, WORD to2);
	void Colorize(int surface_id, int mode);
	void RestoreAll();

	//DC ���� �޼ҵ�
	void PutText(RECT* tarRECT, char* text, COLORREF rgb);
	void PutText(int x, int y, char* text, COLORREF rgb);

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