#include <dsound.h>

#include "dsutil.h"

#define _GetKeyState(vkey) HIBYTE(GetAsyncKeyState(vkey))

//클립핑 모드
#define CLIP_NORMAL	0	//화면 전체 사용

#define SURFACE_MAX 50

#define KEY_MAX 7

#define TO_RED	 	1
#define TO_GREEN 	2
#define TO_BLUE 	3
#define DARK		4

extern int MouseX, MouseY;							//마우스 좌표
extern BOOL LeftButton, RightButton;	//마우스 클릭 여부

//그림을 그릴 정보를 보내주는 클래스
class DrawData
{
public:
	int x;
	int y;
	int id;
	int frame;

	void Set(int vx, int vy, int vid, int vframe=0);
};

//애니메이션 클래스
class CAnimation
{
private:
	int x_size, y_size;
	bool walk;							//보행 여부(기본값: false)
	int top, bottom, left, right;		//보행 그래픽을 위한 상하좌우 애니메이션 그림의 위치(Y좌표를 증가)
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
//게임 작업 클래스
class CHungLib
{
private:
	//DirectDraw 객체
	LPDIRECTDRAW		lpDD;				//다이렉트 드로우 객체
	LPDIRECTDRAWSURFACE	RealScreen;			//Primary Surface 객체
	LPDIRECTDRAWSURFACE	BackScreen;			//Back Surface 객체
	DDSURFACEDESC ddsd;
	DDSCAPS ddscaps;

	//DirectSound 객체
	LPDIRECTSOUND       SoundOBJ;
	LPDIRECTSOUNDBUFFER SoundDSB;
	DSBUFFERDESC        DSB_desc;

	HSNDOBJ Sound[100];
	BOOL SoundCard;

	//서페이스
	int surface_count;
	LPDIRECTDRAWSURFACE	ddsurface[SURFACE_MAX];
	char* surface_name[SURFACE_MAX];
	char* file_name[SURFACE_MAX];
	bool surface_trans[SURFACE_MAX];
	COLORREF surface_colorkey[SURFACE_MAX];
	RECT surface_rect[SURFACE_MAX];
	CAnimation* animation[SURFACE_MAX];

	//DirectDraw관련 변수
	bool use_system_memory;
	int screen_x, screen_y;
	RECT clip;

	//DC관련 변수
	HDC hdc;
	bool dc_mode;	//DC를 쓰고 있는지 알기 위한 것. DC를 쓰는데 BLT를 썼다간 다운

	//클래스 이름, 윈도우 이름
	char* class_name;
	char* window_name;

	//마우스 관련 변수
	bool left_button_push;
	bool right_button_push;

	//*** Private 메소드 ***//
	//ddutil
	DWORD DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb);
	HRESULT DDSetColorKey(IDirectDrawSurface *pdds, COLORREF rgb);

	//DC 관련 메소드
	void BeginDC();
	void EndDC();

public:
	//DirectDraw 관련 메소드
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

	//DirectSound 관련 메소드
	void LoadSound();
	void PlaySound(int num);

	//서페이스 제어 메소드
	bool LoadDGF(char* name, char* src, COLORREF key=NULL, CAnimation* ani=NULL);
	void SurfaceCopy(char* name, char* src);
	void ColorChange(int surface_id, WORD from, WORD to, WORD from2, WORD to2);
	void Colorize(int surface_id, int mode);
	void RestoreAll();

	//DC 관련 메소드
	void PutText(RECT* tarRECT, char* text, COLORREF rgb);
	void PutText(int x, int y, char* text, COLORREF rgb);

	//입력 메소드
	bool GetKey(int key, int delay=999);
	bool LeftClick();
	bool RightClick();

	//기타 메소드
	void Fail(HWND hwnd);	//실패했을 때 빠져나가기

	//생성자와 소멸자
	CHungLib(HWND hwnd, HINSTANCE hInstance, int nCmdShow, char* class_name, char* window_name, int x, int y, bool usm);
	~CHungLib();
};