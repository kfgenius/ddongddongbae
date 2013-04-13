#define _GetKeyState(vkey) HIBYTE(GetAsyncKeyState(vkey))

//Ŭ���� ���
#define CLIP_NORMAL	0	//ȭ�� ��ü ���

#define SURFACE_MAX 100

#define KEY_MAX 7

extern int MouseX, MouseY;							//���콺 ��ǥ
extern BOOL LeftButton, RightButton;	//���콺 Ŭ�� ����

//////////////////////////////////////////////////////////////////
//�׷��� �۾� Ŭ����
class CHungLib
{
private:
	//DirectDraw ��ü
	LPDIRECTDRAW		lpDD;				//���̷�Ʈ ��ο� ��ü
	LPDIRECTDRAWSURFACE	RealScreen;			//Primary Surface ��ü
	LPDIRECTDRAWSURFACE	BackScreen;			//Back Surface ��ü
	DDSURFACEDESC ddsd;
	DDSCAPS ddscaps;

	//�����̽�
	int surface_count;
	LPDIRECTDRAWSURFACE	ddsurface[SURFACE_MAX];
	char* surface_name[SURFACE_MAX];
	char* file_name[SURFACE_MAX];
	bool surface_trans[SURFACE_MAX];
	COLORREF surface_colorkey[SURFACE_MAX];
	RECT surface_rect[SURFACE_MAX];

	//DirectDraw���� ����
	bool use_system_memory;
	int screen_x, screen_y;
	RECT clip;

	//DC���� ����
	HDC hdc;
	bool dc_mode;	//DC�� ���� �ִ��� �˱� ���� ��. DC�� ���µ� BLT�� ��ٰ� �ٿ�

	//������ ��Ű�� ����
	long new_time;
	long old_time;

	//Ŭ���� �̸�, ������ �̸�
	char* class_name;
	char* window_name;

	//���콺 ���� ����
	bool left_button_push;
	bool right_button_push;

	//*** Private �޼ҵ� ***//
	void RestoreAll();

	//DC ���� �޼ҵ�
	void BeginDC();
	void EndDC();

public:
	//DirectDraw ���� �޼ҵ�
	void Image(int x, int y, int surface_id, RECT rect);
	void Image(int x, int y, int surface_id);
	void Image(int x, int y, char* name);
	int GetImageID(char* name);
	void Render();
	WORD* GetSurfaceAddress(int surface_id);
	void SetClip(int left, int top, int right, int bottom);
	void SetClip(int mode);

	//�����̽� ���� �޼ҵ�
	void AddSurfaceUnion(int mode, char* name, char* src, COLORREF key);
	void AddSurface(char* name, char* src);
	void AddSurface(char* name, char* src, COLORREF key);

	//DC ���� �޼ҵ�
	void PutText(RECT* tarRECT, char* text);
	void PutText(int x, int y, char* text);

	//������ ��Ű�� �޼ҵ�
	long GetCurTime();
	long GetFPS();

	//�Է� �޼ҵ�
	bool GetKey(int key);
	bool LeftClick();
	bool RightClick();

	//��Ÿ �޼ҵ�
	void Fail(HWND hwnd);	//�������� �� ����������

	//�����ڿ� �Ҹ���
	CHungLib(HWND hwnd, HINSTANCE hInstance, int nCmdShow, char* class_name, char* window_name, int x, int y, bool usm);
	~CHungLib();
};