#include <windows.h>
#include <ddraw.h>
#include <dsound.h>

#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))
#define _GetKeyPush( vkey )  LOBYTE(GetAsyncKeyState( vkey ))

#define random(num) (int)(((long)rand() * (num)) / (RAND_MAX + 1))
#define randomize() srand((unsigned)time(NULL))

#define MAX_SOUND       50
#define VIDEO           FALSE
#define SYSTEM          TRUE

#define TRANS      -1
#define COLORDODGE  0
#define SCREEN      1
#define DODGEBURN   2
#define DIFFERENT   3
#define LIGHTEN     4

#define WHITE RGB2(255,255,255)
#define BLACK RGB2(0,0,0)
#define RED RGB2(255,0,0)
#define GREEN RGB2(0,255,0)
#define BLUE RGB2(0,0,255)
#define YELLOW RGB2(255,255,0)

extern LPDIRECTDRAW7        DirectOBJ;
extern LPDIRECTDRAWSURFACE7 RealScreen;
extern LPDIRECTDRAWSURFACE7 BackScreen;
extern LPDIRECTDRAWSURFACE7 BmpScreen[10];
extern LPDIRECTSOUND        SoundOBJ;
extern DDSURFACEDESC2       ddsd;
extern RECT                 WinRect, Rect;
extern WORD                 *Screen;
extern int                  SCREEN_X, SCREEN_Y;

extern BOOL SoundCard;

extern int  MouseX, MouseY;
extern BOOL LeftButton, RightButton;
extern BOOL FULL_SCREEN, BPP, ScanLine;

extern int TotalObject, ProjectCount;

extern HWND MainHwnd;
extern HINSTANCE MainInstance;

extern WORD RGB2(WORD r, WORD g, WORD b );
extern BOOL  Fail( HWND hwnd );

extern void _Delay( DWORD count );
extern void _Pixel( int x, int y, WORD color );
extern void _DrawBox( int x1, int y1, int x2, int y2, WORD color );
extern void _DrawBar( int x1, int y1, int x2, int y2, WORD color );
extern void _DrawBarAlpha( int x1, int y1, int x2, int y2, WORD color, int alpha );

extern void _CopyScreen( BOOL mode );
extern BOOL _LockScreen( void );
extern void _UnlockScreen( void );
extern void _ClearScreen( int color );

extern HRESULT _DrawBmp( RECT rect, int x, int y, LPDIRECTDRAWSURFACE7 target, long flag );
extern HRESULT _DrawBmp2( RECT rect, int x, int y, LPDIRECTDRAWSURFACE7 target, long flag, RECT clip, int x2=1);
extern BOOL _LoadBitmap( LPDIRECTDRAWSURFACE7 *page, char *name, BOOL flag );

extern void _WindowMode( void );
extern BOOL _ScreenMode( int x, int y, int bpp, BOOL mode );
extern void _DeleteDraw( void );
extern BOOL _GameMode( HINSTANCE hInstance, int nCmdShow, int x, int y, int bpp, BOOL mode );

extern DWORD _PlayAVI( char name[] );

enum FONT_PATTERN
{
    NORMAL_PATTERN = 0,
    INC16_PATTERN,
    DEC16_PATTERN,
    INC8_PATTERN,
    DEC8_PATTERN,
    DECINC_PATTERN,
    INCDEC_PATTERN,
};

BOOL InitXddFont(char *pHan,char *pEng);
BOOL LoadFont(char *pFile,WORD wType);

void SetFontPattern(int nPattern);
void SetUserFontPattern(char *Pattern);
void SetFontWidth(int nHangul,int nEnglish,int nSpace);

int  PutFont( int xPos,int yPos,WORD color,char *pStr );
int  PutFontf( int xPos,int yPos,WORD color,char *fmt,... );
int PutFontOutline( int xPos,int yPos,WORD color,char *fmt,... );

int Max(int x, int y);
int Min(int x, int y);
int MaxMin(int x, int max, int min);
