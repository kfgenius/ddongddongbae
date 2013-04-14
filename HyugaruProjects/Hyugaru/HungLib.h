#include <ddraw.h>
#include <dsound.h>

#include "dsutil.h"
#include "JDirectDraw.h"
#include "JResourceManager.h"

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

//DirectDraw 객체
extern JDirectDraw* jdd;
extern JResourceManager* jre;

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
	char* backbuffer;

	JFont font;

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
	char* surface_name[SURFACE_MAX];
	char* file_name[SURFACE_MAX];
	CAnimation* animation[SURFACE_MAX];

	//DirectDraw관련 변수
	int screen_x, screen_y;

	//클래스 이름, 윈도우 이름
	char* class_name;
	char* window_name;

	//마우스 관련 변수
	bool left_button_push;
	bool right_button_push;

public:
	//DirectDraw 관련 메소드
	void Image(DrawData ddata);
	void Image(int x, int y, int surface_id, int frame=0);
	void Image(int x, int y, char* name, int frame=0);
	int NextImage(int surface_id);
	int GetImageID(char* name);
	bool Render();

	//DirectSound 관련 메소드
	void LoadSound();
	void PlaySound(int num);

	//서페이스 제어 메소드
	bool LoadDGF(char* name, char* src, JColor key = JColor(0, 0, 0), CAnimation* ani=NULL);
	void RestoreAll();

	//DC 관련 메소드
	void PutText(RECT* tarRECT, char* text, JColor rgb);
	void PutText(int x, int y, char* text, JColor rgb);

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