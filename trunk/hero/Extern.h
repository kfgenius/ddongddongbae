#include <windows.h>
#include <ddraw.h>
#include <dsound.h>

#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))
#define _GetKeyPush( vkey )  LOBYTE(GetAsyncKeyState( vkey ))


#define Pie2 0.0174532925
#define ALL -1

#define random(num) (int)(((long)rand() * (num)) / (RAND_MAX + 1))
#define randomize() srand((unsigned)time(NULL))

#define MAX_PATTERN     100
#define MAX_FRAME       1000
#define MAX_ANIFRAME    1000
#define MAX_OBJECT      1000
#define MAX_SOUND       50
#define VIDEO           FALSE
#define SYSTEM          TRUE

#define TRANS      -1
#define COLORDODGE  0
#define SCREEN      1
#define DODGEBURN   2
#define DIFFERENT   3
#define LIGHTEN     4


struct pattern_header
{
    int  Code;
    char Name[14];
    int  Count;
    RECT Rect[MAX_FRAME];
    int  Xsize[MAX_FRAME], Ysize[MAX_FRAME];
    LPDIRECTDRAWSURFACE7 PageScreen;

    int Size;
    int Offset[MAX_FRAME];
    BYTE *Data;
};

struct sprite_header
{
    int     Type, Pattern, Pframe;
    double  Px, Py;
    int     Rx, Ry;
    int     X1, Y1, X2, Y2;
    int     Mx1, My1, Mx2, My2;
    int     Motion, Frame;
    double  Count, Speed, Jump, Fly, Energy;
    int     Attack, Crash;
    int     Location, Move, Show;
    int     Demage, Death;
    int     Weapon, Hit;
    double  Level, HP, MP, EXP;
    BOOL    Flip, Strike;
    int     Gravity, Layer;
    int     Extra[10];
};

struct ani_header
{
    int Num;
    int Sx;
    int Sy;
};

struct motion_header
{
    int Count;
    int Start;
};

struct map_header
{
    int Xsize, Ysize, Size, Xpos, Ypos, MaxX, MaxY;

    WORD *Data;
};

struct tile_header
{
    char Bmp[100];
    int Xsize, Ysize, Width;
    unsigned char Attr[20000], Skip[20000];
};

struct sprmap_header
{
    int Mx, My;
    unsigned char *Cell;
};

extern LPDIRECTDRAW7        DirectOBJ;
extern LPDIRECTDRAWSURFACE7 RealScreen;
extern LPDIRECTDRAWSURFACE7 BackScreen;
extern LPDIRECTDRAWSURFACE7 TileScreen;
extern LPDIRECTDRAWSURFACE7 BmpScreen[10];
extern LPDIRECTDRAWCLIPPER  ClipScreen;
extern LPDIRECTDRAWPALETTE  Plt;
extern PALETTEENTRY         pPe[256];
extern LPDIRECTSOUND        SoundOBJ;
extern DDSURFACEDESC2       ddsd;
extern RECT                 WinRect, Rect;
extern WORD                 *Screen;
extern int                  SCREEN_X, SCREEN_Y;
extern int                  Offset[];

extern unsigned char       *CompTile;
extern int                  TileOffset[];

extern struct pattern_header Pattern[MAX_PATTERN];
extern struct ani_header Ani[MAX_PATTERN][MAX_ANIFRAME];
extern struct motion_header Motion[MAX_PATTERN][100];
extern struct map_header Map[];
extern struct tile_header TileData;
extern struct sprmap_header SpriteMap[MAX_PATTERN][MAX_FRAME];

extern BOOL SoundCard;
extern char ProjectName[MAX_PATTERN][13];
extern unsigned char CrashMap[76800];
extern unsigned char Palette[768], ColorTable[256][256];

extern int  MouseX, MouseY;
extern BOOL LeftButton, RightButton;
extern BOOL FULL_SCREEN, BPP, ScanLine;

extern int TotalObject, ProjectCount;
extern int ScrollX, ScrollY, ScrollMaxX, ScrollMaxY, OldX, OldY;
extern int WinX1, WinY1, WinX2, WinY2, WinXsize, WinYsize;
extern int ClipX1, ClipY1, ClipX2, ClipY2, ClipXsize, ClipYsize;
extern int TileScreenX, TileScreenY, MapYoff[];

extern int MotionCount[MAX_PATTERN];

extern HWND MainHwnd;
extern HINSTANCE MainInstance;



extern WORD RGB2(WORD r, WORD g, WORD b );
extern BOOL  Fail( HWND hwnd );

extern void _Delay( DWORD count );
extern void _Pixel( int x, int y, WORD color );
extern void _DrawBox( int x1, int y1, int x2, int y2, WORD color );
extern void _DrawBar( int x1, int y1, int x2, int y2, WORD color );
extern void _DrawBarAlpha( int x1, int y1, int x2, int y2, WORD color, int alpha );

extern void _CreateOBJECT( int type, int layer, double x, double y, int pattern, int motion, int frame, BOOL flip, sprite_header *Sprite );

extern void _PutCompress( int x, int y, BYTE *buf, BYTE *off, int mx, int my, BOOL flag );
extern BOOL _PutPattern( int num, int x, int y, int frame, BOOL flip, LPDIRECTDRAWSURFACE7 sor, LPDIRECTDRAWSURFACE7 tar );
extern BOOL _PutPatternEffect( int num, int x, int y, int frame, LPDIRECTDRAWSURFACE7 tar, int style, int alpha );
extern void _PutAni( int x, int y, int pattern, int motion, int frame, int trans, LPDIRECTDRAWSURFACE7 sor, LPDIRECTDRAWSURFACE7 tar, BOOL flip );
extern BOOL _PutSprite( sprite_header *Sprite );

extern void _CopyScreen( BOOL mode );
extern BOOL _LockScreen( void );
extern void _UnlockScreen( void );
extern void _ClearScreen( int color );

extern int  _CrashCheck( sprite_header *LeftSpr, sprite_header *RightSpr, int ic, int kc );

extern void _LoadANI( int pnum, char *name );
extern void _LoadCBM( int num, char *name );

extern HRESULT _DrawBmp( RECT rect, int x, int y, LPDIRECTDRAWSURFACE7 target, long flag );
extern BOOL _LoadBitmap( LPDIRECTDRAWSURFACE7 *page, char *name, BOOL flag );

extern BOOL _InstallMAP( char *name );
extern BOOL _InstallTIL( char *name, BOOL flag );
extern BOOL _InstallPAT( int num, char *name );

extern void _RemoveMAP( void );
extern void _RemoveTIL( void );
extern void _RemovePAT( void );
extern void _RemoveSND( void );
extern void _RemoveBMP( void );

extern void _DrawMap( int num, int x, int y, BOOL flag );
extern void _SetClipArea( int x1, int y1, int x2, int y2 );
extern void _SetScrollArea( int x1, int y1, int x2, int y2 );
extern void _Scroll( int num, BOOL flag );

extern void _SetPalette( unsigned char *data, int s, int e );
extern void _SetFade( int num );
extern void _RotatePalette( int start, int end, BOOL flag );
extern void _LoadPalette( char *name );

extern void _WindowMode( void );
extern BOOL _ScreenMode( int x, int y, int bpp, BOOL mode );
extern void _DeleteDraw( void );
extern BOOL _GameMode( HINSTANCE hInstance, int nCmdShow, int x, int y, int bpp, BOOL mode );


extern long _GetCurTime( void );
extern void _InitFrame( int frame );
extern BOOL _FrameSkip( void );

extern DWORD _PlayAVI( char name[] );
extern void  _Mozaik( int start, int end, int delay );
extern void  _WaveScreen( int count, double range );

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
