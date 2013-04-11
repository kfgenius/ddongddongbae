#include <windows.h>
#include <ddraw.h>
#include <dsound.h>

#include "JDirectDraw.h"
#include "Dsutil.h"
#include "JResourceManager.h"
#include "resource.h"

#define random(num) (int)(((long)rand() * (num)) / (RAND_MAX + 1))
#define randomize() srand((unsigned)time(NULL))

#define MAX_SOUND       50
#define VIDEO           FALSE
#define SYSTEM          TRUE

#define WHITE		JColor(255,255,255)
#define BLACK		JColor(0,0,0)
#define RED			JColor(255,0,0)
#define GREEN		JColor(0,255,0)
#define BLUE		JColor(0,0,255)
#define YELLOW		JColor(255,255,0)

#define SCREEN_X	800
#define SCREEN_Y	600

#define MAX_SOUND       50
#define VIDEO           FALSE
#define SYSTEM          TRUE

#define TRANS      -1
#define COLORDODGE  0
#define SCREEN      1
#define DODGEBURN   2
#define DIFFERENT   3
#define LIGHTEN     4

extern BOOL SoundCard;

extern LPDIRECTSOUND       SoundOBJ;
extern LPDIRECTSOUNDBUFFER SoundDSB;
extern DSBUFFERDESC        DSB_desc;

extern int  MouseX, MouseY;
extern BOOL LeftButton, RightButton;

extern JDirectDraw* jdd;
extern JResourceManager* jre;
extern char* backbuffer;

extern JFont font20;
extern char* BmpScreen[10];

extern HWND MainHwnd;
extern HINSTANCE MainInstance;

extern bool activate;
extern bool window_mode;

extern char DirectOBJ;

extern HSNDOBJ Sound[MAX_SOUND];

extern MSG msg;

#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey )) && activate
#define _GetKeyPush( vkey )  LOBYTE(GetAsyncKeyState( vkey )) && activate

void Init();
void Term();
void PutFontOutline(int x, int y, JColor jc,char *fmt,... );
void PutFont(int x, int y, JColor jc, char* text);
void _DrawBmp(RECT rect, int x, int y, char* surf_name, int flag);
void _DrawBmp2(RECT rect, int x, int y, char* surf_name, int flag, RECT clip, int x2);
void _Pixel( int x, int y, JColor jc);
void _DrawBox( int x1, int y1, int x2, int y2, JColor jc);
void _DrawBar( int x1, int y1, int x2, int y2, JColor jc);
void _DrawBarAlpha( int x1, int y1, int x2, int y2, JColor jc);

void _LockScreen();
void _UnlockScreen();

void _CopyScreen(bool mode);

char* DDLoadBitmap(char direct_obj, char* path, int x, int y, bool system);
void DDSetColorKey(char* surf_name, JColor color);

BOOL _GameMode( HINSTANCE hInstance, int nCmdShow, int x, int y, int bpp, BOOL mode );

int Max(int x, int y);
int Min(int x, int y);
int MaxMin(int x, int max, int min);

extern void _Delay( DWORD count );