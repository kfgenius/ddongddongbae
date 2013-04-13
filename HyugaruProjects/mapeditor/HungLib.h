#define _GetKeyState(vkey) HIBYTE(GetAsyncKeyState(vkey))

//클립핑 모드
#define CLIP_NORMAL	0	//화면 전체 사용

#define SURFACE_MAX 100

#define KEY_MAX 7

extern int MouseX, MouseY;							//마우스 좌표
extern BOOL LeftButton, RightButton;	//마우스 클릭 여부

//////////////////////////////////////////////////////////////////
//그래픽 작업 클래스
class CHungLib
{
private:
	//DirectDraw 객체
	LPDIRECTDRAW		lpDD;				//다이렉트 드로우 객체
	LPDIRECTDRAWSURFACE	RealScreen;			//Primary Surface 객체
	LPDIRECTDRAWSURFACE	BackScreen;			//Back Surface 객체
	DDSURFACEDESC ddsd;
	DDSCAPS ddscaps;

	//서페이스
	int surface_count;
	LPDIRECTDRAWSURFACE	ddsurface[SURFACE_MAX];
	char* surface_name[SURFACE_MAX];
	char* file_name[SURFACE_MAX];
	bool surface_trans[SURFACE_MAX];
	COLORREF surface_colorkey[SURFACE_MAX];
	RECT surface_rect[SURFACE_MAX];

	//DirectDraw관련 변수
	bool use_system_memory;
	int screen_x, screen_y;
	RECT clip;

	//DC관련 변수
	HDC hdc;
	bool dc_mode;	//DC를 쓰고 있는지 알기 위한 것. DC를 쓰는데 BLT를 썼다간 다운

	//프레임 스키핑 변수
	long new_time;
	long old_time;

	//클래스 이름, 윈도우 이름
	char* class_name;
	char* window_name;

	//마우스 관련 변수
	bool left_button_push;
	bool right_button_push;

	//*** Private 메소드 ***//
	void RestoreAll();

	//DC 관련 메소드
	void BeginDC();
	void EndDC();

public:
	//DirectDraw 관련 메소드
	void Image(int x, int y, int surface_id, RECT rect);
	void Image(int x, int y, int surface_id);
	void Image(int x, int y, char* name);
	int GetImageID(char* name);
	void Render();
	WORD* GetSurfaceAddress(int surface_id);
	void SetClip(int left, int top, int right, int bottom);
	void SetClip(int mode);

	//서페이스 제어 메소드
	void AddSurfaceUnion(int mode, char* name, char* src, COLORREF key);
	void AddSurface(char* name, char* src);
	void AddSurface(char* name, char* src, COLORREF key);

	//DC 관련 메소드
	void PutText(RECT* tarRECT, char* text);
	void PutText(int x, int y, char* text);

	//프레임 스키핑 메소드
	long GetCurTime();
	long GetFPS();

	//입력 메소드
	bool GetKey(int key);
	bool LeftClick();
	bool RightClick();

	//기타 메소드
	void Fail(HWND hwnd);	//실패했을 때 빠져나가기

	//생성자와 소멸자
	CHungLib(HWND hwnd, HINSTANCE hInstance, int nCmdShow, char* class_name, char* window_name, int x, int y, bool usm);
	~CHungLib();
};