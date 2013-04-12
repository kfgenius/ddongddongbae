#include <windows.h>
#include <windowsx.h>
#include <winbase.h>
#include <math.h>
#include <stdio.h>
#include <ddraw.h>
#include <dsound.h>
#include <vfw.h>
#include <digitalv.h>
#include <mmsystem.h>

#include "extern.h"
#include "dsutil.h"
#include "ddutil.h"

#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))
#define _GetKeyPush( vkey )  LOBYTE(GetAsyncKeyState( vkey ))

#define randomize() srand((unsigned)time(NULL))

#define RATIO 4

struct pattern_header Pattern[MAX_PATTERN];
struct ani_header Ani[MAX_PATTERN][MAX_ANIFRAME];
struct motion_header Motion[MAX_PATTERN][100];
struct map_header Map[5];
struct tile_header TileData;
struct sprmap_header SpriteMap[MAX_PATTERN][MAX_FRAME];

LPDIRECTDRAW7        DirectOBJ;
LPDIRECTDRAWSURFACE7 RealScreen;
LPDIRECTDRAWSURFACE7 BackScreen;
LPDIRECTDRAWSURFACE7 TileScreen;
LPDIRECTDRAWCLIPPER  ClipScreen;
LPDIRECTDRAWSURFACE7 BmpScreen[10];
LPDIRECTDRAWPALETTE  Plt;
PALETTEENTRY         pPe[256];
DDSURFACEDESC2       ddsd;
RECT                 WinRect, Rect;
WORD                *Screen;
int                  SCREEN_X, SCREEN_Y;
unsigned short       Effect1[5][64][64];
unsigned short       Effect2[5][32][32];
unsigned char       *CompTile;

int TileOffset[20000];

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

char ProjectName[MAX_PATTERN][13];

BOOL SoundCard;

unsigned char CrashMap[76800];
unsigned char Palette[768], ColorTable[256][256];

int  MouseX, MouseY;
int  TotalObject = 0, ProjectCount;
int  ScrollX, ScrollY, ScrollMaxX, ScrollMaxY, OldX, OldY;
int  ScreenX[MAX_PATTERN], ScreenY[MAX_PATTERN];

int  WinX1 = 0, WinY1 = 0, WinX2 = 639, WinY2 = 479, WinXsize = 640, WinYsize = 480;
int  ClipX1 = 0, ClipY1 = 0, ClipX2 = 639, ClipY2 = 479, ClipXsize = 640, ClipYsize = 480;
int  MotionCount[MAX_PATTERN], MapOffset[240], Offset[480], Blank[500], BlankCount;
int  TileScreenX, TileScreenY, TileCount, TileSize, MapYoff[500];

BOOL PatternMode[MAX_PATTERN];
int  BPP;
BOOL FULL_SCREEN, PixelMode, ScanLine = FALSE;

struct {
    WORD skipbytes;
    WORD putsbytes;
} Comp;


BYTE *Compile;




HSNDOBJ Sound[MAX_SOUND], Sample[MAX_SOUND];

HWND MainHwnd;
HINSTANCE MainInstance;



typedef WORD COLOR;

enum FONT_FILE_TYPE
{
    HANGUL_FNT,
    ENGLISH_FNT,
};

typedef struct {
    BYTE sndchar;
    BYTE fstchar;
} hanchar;

typedef struct {
    WORD jongsung : 5;
    WORD jungsung : 5;
    WORD chosung  : 5;
    WORD det_han_or_eng : 1;
} hancode;

typedef union {
     hanchar hchar;
     hancode hcode;
} hangul;

static char HANKS[2350][32];
static char EF[256][16];

static int ENGLISH_WIDTH = 0;
static int HANGUL_WIDTH  = 0;
static int SPACE_WIDTH   = 0;

static char *pPattern;
static char pattNormal[16]   ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static char pattIncress16[16]={0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
static char pattDecress16[16]={15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
static char pattIncress8[16] ={0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
static char pattDecress8[16] ={7,-1,0,-1,0,-1,0,-1,0,-1,0,-1,0,-1,0,-1};
static char pattIncDec[16]   ={0,1,1,1,1,1,1,1,0,-1,-1,-1,-1,-1,-1,-1};
static char pattDecInc[16]   ={7,-1,-1,-1,-1,-1,-1,0,1,1,1,1,1,1,1,1};


extern long FAR PASCAL WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );


//////////////////////////////////////////////////////////////////////////////////////////
///
/// 사운드 데이터를 저장하기 위한 Sound[]에 할당된 내용을 소멸시켜주는 함수
///
//////////////////////////////////////////////////////////////////////////////////////////

void _RemoveSND( void )
{
    for ( int i = 0; i < MAX_SOUND; i++ )
    {
        if ( Sound[i] != NULL )
        {
            SndObjDestroy( Sound[i] );
            Sound[i] = NULL;
        }
        if ( Sample[i] != NULL )
        {
            SndObjDestroy( Sample[i] );
            Sample[i] = NULL;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 16비트 모드에서 RGB값을 리턴해주는 함수, r,g,b는 0~255까지의 값을 지정할 수 있다.
///
//////////////////////////////////////////////////////////////////////////////////////////

WORD RGB2(WORD r, WORD g, WORD b )
{
    WORD rgb;

    if ( PixelMode )
    {
        rgb = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }
    else
    {
        rgb = ((r >> 3) << 10) | ((g >> 2) << 5) | (b >> 3);
    }
    return rgb;
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// Noriter 라이브러리의 자체 에러 발생시 메시지를 출력해주는 함수
///
//////////////////////////////////////////////////////////////////////////////////////////

BOOL Fail( HWND hwnd )
{
    ShowWindow( hwnd, SW_HIDE );
    MessageBox( hwnd, "DIRECT X init failed !!!", "Noriter LIB 3.0", MB_OK );
    DestroyWindow( hwnd );

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 1/1000 단위로 지정한 시간만큼 지연시켜주는 함수, count가 1000이면 1초간 지연된다.
///
//////////////////////////////////////////////////////////////////////////////////////////

void _Delay( DWORD count )
{
    DWORD cur;
    
    cur = GetTickCount();

    while ( (GetTickCount() - cur) < count );
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 화면을 페이지 플리핑 해준다. mode가 TRUE이면 Flip() 함수로 수직동기화하여 플리핑하고,
/// FALSE이면 BltFast() 함수로 BackScreen의 내용을 RealScreen으로 단순 복사한다.
/// 그러나 설정된 모드가 윈도우 모드이면 mode 값에 상관없이 Blt() 함수가 사용된다.
///
//////////////////////////////////////////////////////////////////////////////////////////

void _CopyScreen( BOOL mode )
{
	HRESULT hResult ;

    if ( FULL_SCREEN )
    {
		while (1)
		{
			if ( mode )
			{
				hResult = RealScreen -> Flip( NULL, DDFLIP_WAIT );
			}
			else
			{
				hResult = RealScreen -> BltFast( 0, 0, BackScreen, &Rect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
			}
			
			if (hResult == DD_OK)
			{
				break ;
			}

			if (hResult == DDERR_SURFACELOST)
			{
				hResult = RealScreen->Restore();

				if (hResult != DD_OK)
				{
					break ;
				}
			}

			if (hResult != DDERR_WASSTILLDRAWING)
			{
				break ;
			}

		}
    }
    else RealScreen -> Blt( &WinRect, BackScreen, &Rect, DDBLT_WAIT, NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// BackScreen에 대한 접근을 허용하고 메모리 포인터를 Screen으로 리턴한다.
///
//////////////////////////////////////////////////////////////////////////////////////////

BOOL _LockScreen( void )
{
    HRESULT result;

    memset( &ddsd, 0, sizeof(ddsd) );
    ddsd.dwSize = sizeof(ddsd);

    result = BackScreen->Lock( NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL );
    if ( result != DD_OK ) return FALSE;

    Screen = (WORD *)ddsd.lpSurface;
    return TRUE;
}

void _UnlockScreen( void )
{
    BackScreen -> Unlock( NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 화면을 지정한 색상으로 지운다. color에는 RGB2() 함수로 지정한 값을 사용한다.
///
//////////////////////////////////////////////////////////////////////////////////////////

void _ClearScreen( int color )
{
    _DDBLTFX BltFx;

    ZeroMemory( &BltFx, sizeof(BltFx) );
    BltFx.dwSize = sizeof( BltFx );
    BltFx.dwFillColor = color;
    BackScreen -> Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &BltFx );
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 지정한 값에 따라 오브젝트를 생성한다.
///
//////////////////////////////////////////////////////////////////////////////////////////

void _CreateOBJECT( int type, int layer, double x, double y, int pattern, int motion, int frame, BOOL flip, sprite_header *Sprite )
{
    Sprite -> Type = type;
    Sprite -> Pattern = pattern;
    Sprite -> Px = x;
    Sprite -> Py = y;
    Sprite -> Motion = motion;
    Sprite -> Frame = frame;
    Sprite -> Count = Sprite -> Speed = Sprite -> Jump = Sprite -> Fly = 0;
    Sprite -> Attack = Sprite -> Crash = Sprite -> Location = Sprite -> Move = 0;
    Sprite -> Energy = 100;
    Sprite -> Show = 1;
    Sprite -> Demage = Sprite -> Death = 0;
    Sprite -> Weapon = Sprite -> Hit = 0;
    Sprite -> Level = Sprite -> HP = Sprite -> MP = Sprite -> EXP = 0;
    Sprite -> Strike = FALSE;
    Sprite -> Flip = flip;
    Sprite -> Gravity = 40;
    Sprite -> Layer = layer;

    memset( Sprite -> Extra, 0, 40 );
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 점, 상자, 채워진 상자, 반투명 상자 등을 그려주는 함수
///
//////////////////////////////////////////////////////////////////////////////////////////

void _Pixel( int x, int y, WORD color )
{
    if ( x < ClipX1 || x > ClipX2 || y < ClipY1 || y > ClipY2 ) return;
    Screen[(y * (ddsd.lPitch / 2)) + x] = color;
}

void _DrawBox( int x1, int y1, int x2, int y2, WORD color )
{
    for ( int i = y1; i <= y2; i++ )
    {
        _Pixel( x1, i, color );
        _Pixel( x2, i, color );
    }
    for ( int k = x1+1; k < x2; k++ )
    {
        _Pixel( k, y1, color );
        _Pixel( k, y2, color );
    }
}

void _DrawBar( int x1, int y1, int x2, int y2, WORD color )
{
    for ( int i = y1; i < y2; i++ )
    for ( int k = x1; k < x2; k++ ) _Pixel( k, i, color );
}

void _DrawBarAlpha( int x1, int y1, int x2, int y2, WORD color, int alpha )
{
    WORD tar, r1, g1, b1, r2, g2, b2, r, g, b, aa = 256 - alpha;

    if ( PixelMode )
    {
        r2 = ((color & 0xF800) >> 11) * alpha;
        g2 = ((color & 0x07E0) >> 5 ) * alpha;
        b2 = ((color & 0x001F)      ) * alpha;

        for ( int i = y1; i <= y2; i++ )
        {
            if ( i < ClipY1 || i > ClipY2 ) continue;

            for ( int k = x1; k <= x2; k++ )
            {
                if ( k >= ClipX1 && k <= ClipX2 )
                {
                    tar = Screen[(i*(ddsd.lPitch/2))+k];

                    r1 = (tar & 0xF800) >> 11;
                    g1 = (tar & 0x07E0) >> 5;
                    b1 = (tar & 0x001F);
        
                    r = (r2 + (r1 * aa)) >> 8;
                    g = (g2 + (g1 * aa)) >> 8;
                    b = (b2 + (b1 * aa)) >> 8;

                    Screen[(i*(ddsd.lPitch/2))+k] = ((r << 11) | (g << 5) | b);
                }
            }
        }
    }
    else
    {
        r2 = ((color & 0x7C00) >> 10) * alpha;
        g2 = ((color & 0x03E0) >> 5 ) * alpha;
        b2 = ((color & 0x001F)      ) * alpha;

        for ( int i = y1; i <= y2; i++ )
        {
            if ( i < ClipY1 || i > ClipY2 ) continue;

            for ( int k = x1; k <= x2; k++ )
            {
                if ( k >= ClipX1 && k <= ClipX2 )
                {
                    tar = Screen[Offset[i]+k];

                    r1 = (tar & 0x7C00) >> 10;
                    g1 = (tar & 0x03E0) >> 5;
                    b1 = (tar & 0x001F);
        
                    r = (r2 + (r1 * aa)) >> 8;
                    g = (g2 + (g1 * aa)) >> 8;
                    b = (b2 + (b1 * aa)) >> 8;

                    Screen[Offset[i]+k] = ((r << 10) | (g << 5) | b);
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 압축된 스프라이트 이미지를 출력해주는 함수
///
//////////////////////////////////////////////////////////////////////////////////////////

void _PutCompress( int x, int y, BYTE *buf, BYTE *off, int mx, int my, BOOL flag )
{
    char index;
    int  i, k, size = (x + mx), length, loop;
    int  skipbyte, putsbyte, skip, puts, xx, cx = SCREEN_X, gx = ClipX2 + 1;


    if ( BPP == 16 )
    {
        x <<= 1;
        cx = (SCREEN_X << 1);
        gx <<= 1;
    }
    length = (*(WORD*)off);
    off += 2;

    if ( !flag )
    {
        buf += ((y * ddsd.lPitch) + x);

        for ( loop = y; loop < (length + y); loop++, buf += ddsd.lPitch )
        {
            index = *off++;
            skipbyte = 0;

            if ( loop < ClipY1 || loop > ClipY2 || ((loop % 2)== 0 && ScanLine) )
            {
                while ( index-- )
                {
                    skipbyte += (*(WORD*)off);
                    off += 2;

                    putsbyte  = (*(WORD*)off);
                    off += 2;

                    skipbyte += putsbyte;
                    off      += putsbyte;
                }
                continue;
            }

            while ( index-- )
            {
                skipbyte += (*(WORD*)off);
                off += 2;

                putsbyte  = (*(WORD*)off);
                off += 2;

                if ( putsbyte )
                {
                    if ( (x + skipbyte) < ClipX1 )
                    {
                        xx = abs(x + skipbyte);
                        if ( xx > putsbyte ) xx = putsbyte;
                        memmove( buf + skipbyte + xx, off + xx, putsbyte - xx );
                    }
                    else if ( (x + skipbyte + putsbyte) > gx )
                    {
                        xx = (x + skipbyte + putsbyte) - gx;
                        if ( xx > putsbyte ) xx = putsbyte;
                        memmove( buf + skipbyte, off, putsbyte - xx );
                    }
                    else memmove( buf + skipbyte, off, putsbyte );
                }
                skipbyte += putsbyte;
                off      += putsbyte;
            }
        }
    }
    else
    {
        buf += ((y * ddsd.lPitch) + x + (mx << 1));

        for ( loop = y; loop < (length + y); loop++, buf += ddsd.lPitch )
        {
            index = *off++;
            skipbyte = 0;

            if ( loop < ClipY1 || loop > ClipY2 || ((loop % 2)== 0 && ScanLine) )
            {
                while ( index-- )
                {
                    skipbyte += (*(WORD*)off);
                    off += 2;

                    putsbyte  = (*(WORD*)off);
                    off += 2;

                    skipbyte += putsbyte;
                    off      += putsbyte;
                }
                continue;
            }
            // 16비트 모드 전용 루틴이다. 8비트 모드는 수정 필요

            while ( index-- )
            {
                skipbyte += (*(WORD*)off);
                off += 2;

                putsbyte  = (*(WORD*)off);
                off += 2;

                k = (size - (skipbyte >> 1));

                if ( putsbyte )
                {
                    puts = putsbyte >> 1;
                    skip = skipbyte >> 1;

                    for ( i = 0; i < puts; i++ )
                    {
                         if ( k >= ClipX1 && k <= ClipX2 ) *((WORD *)buf - skip - i) = *((WORD *)off + i);
                         k--;
                    }
                }
                skipbyte += putsbyte;
                off      += putsbyte;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// sour의 내용을 length의 길이 만큼 dest에 복사한다.
/// 이때 sour의 내용과 dest의 내용을 alpha 값에 따라 반투명으로 처리한다.
/// 16비트 모드 전용이다.
///
//////////////////////////////////////////////////////////////////////////////////////////

void effect( WORD *dest, WORD *sour, int length, int style, int alpha )
{
    WORD r, g, b, e, d;
    WORD r1, g1, b1, r2, g2, b2, aa = 256 - alpha;

    if ( PixelMode )
    {
        for ( int i = 0; i < length; i++ )
        {
            e = sour[i];
            d = dest[i];

            if ( style == TRANS )
            {
                r1 = (e & 0xF800) >> 11;
                g1 = (e & 0x07E0) >> 5;
                b1 = (e & 0x001F);

                r2 = ((d & 0xF800) >> 11) * alpha;
                g2 = ((d & 0x07E0) >> 5 ) * alpha;
                b2 = ((d & 0x001F)      ) * alpha;

                r = (r2 + (r1 * aa)) >> 8;
                g = (g2 + (g1 * aa)) >> 8;
                b = (b2 + (b1 * aa)) >> 8;
            }
            else
            {
                r = Effect2[style][(d&0xF800)>>11][(e&0xF800)>>11];
                g = Effect1[style][(d&0x07E0)>> 5][(e&0x07E0)>> 5];
                b = Effect2[style][(d&0x001F)    ][(e&0x001F)    ];
            }
            dest[i] = (unsigned short)( (r << 11) | (g << 5) | (b) );
        }
    }
    else
    {
        for ( int i = 0; i < length; i++ )
        {
            e = sour[i];
            d = dest[i];

            if ( style == TRANS )
            {
                r1 = (e & 0x7C00) >> 10;
                g1 = (e & 0x03E0) >> 5;
                b1 = (e & 0x001F);

                r2 = ((d & 0x7C00) >> 10) * alpha;
                g2 = ((d & 0x03E0) >> 5 ) * alpha;
                b2 = ((d & 0x001F)      ) * alpha;

                r = (r2 + (r1 * aa)) >> 8;
                g = (g2 + (g1 * aa)) >> 8;
                b = (b2 + (b1 * aa)) >> 8;
            }
            else
            {
                r = Effect2[style][(d&0x7C00)>>10][(e&0x7C00)>>10];
                g = Effect2[style][(d&0x03E0)>> 5][(e&0x03E0)>> 5];
                b = Effect2[style][(d&0x001F)    ][(e&0x001F)    ];
            }
            dest[i] = (unsigned short)( (r << 10) | (g << 5) | (b) );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// effect() 함수를 사용하여 압축된 스프라이트를 반투명으로 출력한다.
///
//////////////////////////////////////////////////////////////////////////////////////////

void _PutEffect( int x, int y, BYTE *buf, BYTE *off, int style, int alpha )
{
    int  length, loop;
    int  skipbyte, putsbyte, xx;
    char index;

    x <<= 1;
    buf += ((y * ddsd.lPitch) + x);

    length = (*(WORD*)off);
    off += 2;

    for ( loop = y; loop < (length + y); loop++, buf += ddsd.lPitch )
    {
        index = *off++;
        skipbyte = 0;

        if ( loop < 0 || loop >= 480 )
        {
            while ( index-- )
            {
                skipbyte += (*(WORD*)off);
                off += 2;

                putsbyte  = (*(WORD*)off);
                off += 2;

                skipbyte += putsbyte;
                off      += putsbyte;
            }
            continue;
        }

        while ( index-- )
        {
            skipbyte += (*(WORD*)off);
            off += 2;

            putsbyte  = (*(WORD*)off);
            off += 2;

            if ( putsbyte )
            {
                if ( (x + skipbyte) < 0 )
                {
                    xx = abs(x + skipbyte);
                    if ( xx > putsbyte ) xx = putsbyte;
                    effect( (WORD *)(buf + skipbyte + xx), (WORD *)(off + xx), (putsbyte - xx) / 2, style, alpha );
                }
                else if ( (x + skipbyte + putsbyte) > 1280 )
                {
                    xx = (x + skipbyte + putsbyte) - 1280;
                    if ( xx > putsbyte ) xx = putsbyte;
                    effect( (WORD *)(buf + skipbyte), (WORD *)off, (putsbyte - xx) / 2, style, alpha );
                }
                else effect( (WORD *)(buf + skipbyte), (WORD *)off, putsbyte / 2, style, alpha );
            }
            skipbyte += putsbyte;
            off      += putsbyte;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 스프라이트 파일의 압축 여부에 따라 BltFast() 혹은 _PutCompress() 함수를 사용하여,
/// 스프라이트 이미지를 출력한다.
///
//////////////////////////////////////////////////////////////////////////////////////////

BOOL _PutPattern( int num, int x, int y, int frame, BOOL flip, LPDIRECTDRAWSURFACE7 sor, LPDIRECTDRAWSURFACE7 tar )
{
    DDBLTFX BltFx;
    RECT Rect, Rect2;
    int mx, my;

    if ( Pattern[num].Code == 0 )
    {
        Rect2 = Pattern[num].Rect[frame];
    
        mx = Pattern[num].Xsize[frame];
        my = Pattern[num].Ysize[frame];

        if ( x + mx - 1 < 0 || y + my - 1 < 0 || x > ClipX2 || y > ClipY2 ) return FALSE;

        if ( x < ClipX1 || (x + mx - 1) >= ClipX2 || y < ClipY1 || (y + my - 1) >= ClipY2 )
        {
            if ( !flip )
            {
                if ( x < ClipX1 )
                { 
                    Rect2.left += abs(ClipX1 - x);

                    mx -= (ClipX1 - x);
                    x  += (ClipX1 - x);
                }
                if ( y < ClipY1 )
                { 
                    Rect2.top += abs(ClipY1 - y);

                    my -= (ClipY1 - y);
                    y  += (ClipY1 - y);
                }
                if ( (x + mx) > ClipX2 ) Rect2.right  -= ((x + mx) - ClipX2 - 1);
                if ( (y + my) > ClipY2 ) Rect2.bottom -= ((y + my) - ClipY2 - 1);

                tar -> BltFast( x, y, sor, &Rect2, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
            }
            else
            {
                if ( x < ClipX1 )
                { 
                    Rect2.right -= abs(ClipX1 - x);

                    mx -= (ClipX1 - x);
                    x  += (ClipX1 - x);
                }
                if ( y < ClipY1 )
                { 
                    Rect2.top += abs(ClipY1 - y);

                    my -= (ClipY1 - y);
                    y  += (ClipY1 - y);
                }
                if ( (x + mx) > ClipX2 ) Rect2.left   += ((x + mx) - ClipX2 - 1);
                if ( (y + my) > ClipY2 ) Rect2.bottom -= ((y + my) - ClipY2 - 1);

                BltFx.dwSize = sizeof( BltFx );
                BltFx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;

                Rect.left   = x;
                Rect.top    = y;
                Rect.right  = x + (Rect2.right - Rect2.left);
                Rect.bottom = y + (Rect2.bottom - Rect2.top);

                tar -> Blt( &Rect, sor, &Rect2, DDBLT_DDFX | DDBLT_WAIT | DDBLT_KEYSRC, &BltFx );
            }
        }
        else
        {
            if ( !flip )
            {
                tar -> BltFast( x, y, sor, &Rect2, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
            }
            else
            {       
                BltFx.dwSize = sizeof( BltFx );
                BltFx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;

                Rect.left   = x;
                Rect.top    = y;
                Rect.right  = x + mx;
                Rect.bottom = y + my;

                tar -> Blt( &Rect, sor, &Rect2, DDBLT_DDFX | DDBLT_WAIT | DDBLT_KEYSRC, &BltFx );
            }
        }
    }
    if ( Pattern[num].Code == 1 )
    {
        mx = Pattern[num].Xsize[frame];
        my = Pattern[num].Ysize[frame];

        if ( x + mx - 1 < 0 || y + my - 1 < 0 || x > ClipX2 || y > ClipY2 ) return FALSE;

       _PutCompress( x, y, (BYTE *)Screen, &Pattern[num].Data[Pattern[num].Offset[frame]], mx, my, flip );
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 압축된 스프라이트 이미지를 반투명으로 출력한다.
///
//////////////////////////////////////////////////////////////////////////////////////////

BOOL _PutPatternEffect( int num, int x, int y, int frame, LPDIRECTDRAWSURFACE7 tar, int style, int alpha )
{
    int mx, my;

    if ( Pattern[num].Code == 1 )
    {
        mx = Pattern[num].Xsize[frame];
        my = Pattern[num].Ysize[frame];

        if ( x + mx - 1 < 0 || y + my - 1 < 0 || x > ClipX2 || y > ClipY2 ) return FALSE;

        _PutEffect( x, y, (BYTE *)Screen, &Pattern[num].Data[Pattern[num].Offset[frame]], style, alpha );
    }
    return TRUE;
}

void _PutAni( int x, int y, int pattern, int motion, int frame, int trans, LPDIRECTDRAWSURFACE7 sor, LPDIRECTDRAWSURFACE7 tar, BOOL flip )
{
    int num = Ani[pattern][Motion[pattern][motion].Start + frame].Num;
    
    if ( !trans )
    {
        if ( !flip )
        {
            _PutPattern( pattern, x + Ani[pattern][Motion[pattern][motion].Start + frame].Sx, y + Ani[pattern][Motion[pattern][motion].Start + frame].Sy, num, flip, sor, tar );
        }
        else
        {
            _PutPattern( pattern, x - (Pattern[pattern].Xsize[num] + Ani[pattern][Motion[pattern][motion].Start + frame].Sx), y + Ani[pattern][Motion[pattern][motion].Start + frame].Sy, num, flip, sor, tar );
        }
    }
    else
    {
        if ( !flip )
        {
            _PutPatternEffect( pattern, x + Ani[pattern][Motion[pattern][motion].Start + frame].Sx, y + Ani[pattern][Motion[pattern][motion].Start + frame].Sy, num, tar, TRANS, trans );
        }
        else
        {
            _PutPatternEffect( pattern, x - (Pattern[pattern].Xsize[num] + Ani[pattern][Motion[pattern][motion].Start + frame].Sx), y + Ani[pattern][Motion[pattern][motion].Start + frame].Sy, num, tar, TRANS, trans );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 충돌비트맵 데이터를 이용하여 오브젝트끼리의 충돌을 체크한다.
/// 만일 해당 스프라이트에 충돌비트맵 파일이 없으면 정상적인 결과를 얻을 수 없다.
///
//////////////////////////////////////////////////////////////////////////////////////////

int _CrashCheck( sprite_header *LeftSpr, sprite_header *RightSpr, int ic, int kc )
{
    int i, k, m, px, py, mx, my;

    if ( LeftSpr->Death || RightSpr->Death ) return 0;

    if ( LeftSpr->X1 < RightSpr->X2 && LeftSpr->X2 > RightSpr->X1 && LeftSpr->Y1 < RightSpr->Y2 && LeftSpr->Y2 > RightSpr->Y1 )
    {
        memset( CrashMap, 0, 76800 );

        px = (640-(Pattern[LeftSpr->Pattern].Xsize[LeftSpr->Pframe]/2)) / RATIO;
        py = (480-(Pattern[LeftSpr->Pattern].Ysize[LeftSpr->Pframe]/2)) / RATIO;

        if ( !LeftSpr->Flip )
        {
            for ( i = 0; i < SpriteMap[LeftSpr->Pattern][LeftSpr->Pframe].My; i++ )
            {
                memmove( &CrashMap[MapOffset[py+i] + px], &SpriteMap[LeftSpr->Pattern][LeftSpr->Pframe].Cell[i * SpriteMap[LeftSpr->Pattern][LeftSpr->Pframe].Mx], SpriteMap[LeftSpr->Pattern][LeftSpr->Pframe].Mx );
            }
        }
        else
        {
            for ( i = 0; i < SpriteMap[LeftSpr->Pattern][LeftSpr->Pframe].My; i++ )
            {
                for ( k = 0, m = SpriteMap[LeftSpr->Pattern][LeftSpr->Pframe].Mx + 1; m >= 0; k++, m-- ) CrashMap[MapOffset[py+i] + px + k] = SpriteMap[LeftSpr->Pattern][LeftSpr->Pframe].Cell[(i * SpriteMap[LeftSpr->Pattern][LeftSpr->Pframe].Mx) + m];
            }
        }
        mx = px - ((LeftSpr->X1-RightSpr->X1) / RATIO);
        my = py - ((LeftSpr->Y1-RightSpr->Y1) / RATIO);

        if ( !RightSpr->Flip )
        {
            for ( i = 0; i < SpriteMap[RightSpr->Pattern][RightSpr->Pframe].My; i++ )
            for ( k = 0; k < SpriteMap[RightSpr->Pattern][RightSpr->Pframe].Mx; k++ )
            {
                if ( CrashMap[MapOffset[my+i] + mx + k] == ic && SpriteMap[RightSpr->Pattern][RightSpr->Pframe].Cell[(i * SpriteMap[RightSpr->Pattern][RightSpr->Pframe].Mx) + k] == kc) return 1;
            }
        }
        else
        {
            for ( i = 0; i < SpriteMap[RightSpr->Pattern][RightSpr->Pframe].My; i++ )
            for ( k = 0, m = SpriteMap[RightSpr->Pattern][RightSpr->Pframe].Mx + 1; m >= 0; k++, m-- )
            {
                if ( CrashMap[MapOffset[my+i] + mx + k] == ic && SpriteMap[RightSpr->Pattern][RightSpr->Pframe].Cell[(i * SpriteMap[RightSpr->Pattern][RightSpr->Pframe].Mx) + m] == kc ) return 1;
            }
        }
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 오브젝트 데이터가 가지고 있는 내용을 논리적으로 출력한다.
/// 다만 이 함수는 오브젝트 구조체인 sprite_header의 각 필드값을 설정할 뿐,
/// 화면으로는 아무것도 출력되지 않는다.
///
//////////////////////////////////////////////////////////////////////////////////////////

BOOL _PutSprite( sprite_header *Sprite )
{
    int xsize, ysize;
    int frame = Sprite->Frame + Motion[Sprite->Pattern][Sprite->Motion].Start;

    Sprite->Pframe = Ani[Sprite->Pattern][frame].Num;

    xsize = Pattern[Sprite->Pattern].Xsize[Sprite->Pframe];
    ysize = Pattern[Sprite->Pattern].Ysize[Sprite->Pframe];

    if ( !Sprite->Flip )
    {
        Sprite->X1 = (int)Sprite->Px + Ani[Sprite->Pattern][frame].Sx - ScrollX + ClipX1;
        Sprite->Y1 = (int)Sprite->Py + Ani[Sprite->Pattern][frame].Sy - ScrollY + ClipY1;
        Sprite->X2 = Sprite->X1 + xsize - 1;
        Sprite->Y2 = Sprite->Y1 + ysize - 1;
    }
    else
    {
        Sprite->X1 = ((int)Sprite->Px - (xsize + Ani[Sprite->Pattern][frame].Sx)) - ScrollX + ClipX1;
        Sprite->Y1 = (int)Sprite->Py + Ani[Sprite->Pattern][frame].Sy - ScrollY + ClipY1;
        Sprite->X2 = Sprite->X1 + xsize - 1;
        Sprite->Y2 = Sprite->Y1 + ysize - 1;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 애니메이션 파일을 불러온다.
///
//////////////////////////////////////////////////////////////////////////////////////////

void _LoadANI( int pnum, char *name )
{
    FILE *file;
    int anix, aniy, framecount;
    char pname[80];

    strcpy( pname, "DATA//" );
    strcat( pname, name );

    if ( (file = fopen( pname, "rb" )) != NULL )
    {
        fread( &framecount, 4, 1, file );
        fread( &anix, 4, 1, file );
        fread( &aniy, 4, 1, file );
        fread( Ani[pnum], 12, framecount, file );
        fread( &MotionCount[pnum], 4, 1, file );
        fread( Motion[pnum], 8, MotionCount[pnum], file );
        fclose( file );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 충돌비트맵 파일을 불러온다.
///
//////////////////////////////////////////////////////////////////////////////////////////

void _LoadCBM( int num, char *name )
{
    FILE *file;
    int i, count = Pattern[num].Count;
    char pname[80];

    strcpy( pname, "DATA//" );
    strcat( pname, name );

    if ( (file = fopen( pname, "rb" )) != NULL )
    {
        for ( i = 0; i < count; i++ )
        {
            if ( fread(&SpriteMap[num][i].Mx, 4, 1, file) == NULL ) return;
            if ( fread(&SpriteMap[num][i].My, 4, 1, file) == NULL ) return;

            SpriteMap[num][i].Cell = (unsigned char *)malloc( SpriteMap[num][i].Mx * SpriteMap[num][i].My );

            if ( fread(SpriteMap[num][i].Cell, SpriteMap[num][i].Mx * SpriteMap[num][i].My, 1, file) == NULL ) return;
        }
        fclose( file );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// target에서 rect가 나타내는 영역을 BackScreen의 좌표 x,y에 출력한다.
/// flag는 BltFast() 함수의 플래그와 같다.
///
//////////////////////////////////////////////////////////////////////////////////////////

HRESULT _DrawBmp( RECT rect, int x, int y, LPDIRECTDRAWSURFACE7 target, long flag )
{
    int xx = rect.right - rect.left, yy = rect.bottom - rect.top;

    if ( x + xx - 1 < 0 || y + yy - 1 < 0 || x > ClipX2 || y > ClipY2 ) return DD_OK;

    if ( x < ClipX1 )
    { 
        rect.left += abs(ClipX1 - x);
        xx = rect.right - rect.left;

        x += (ClipX1 - x);
    }
    if ( y < ClipY1 )
    { 
        rect.top += abs(ClipY1 - y);
        yy = rect.bottom - rect.top;

        y += (ClipY1 - y);
    }
    if ( (x + xx) > ClipX2 ) rect.right  -= ((x + xx) - ClipX2 - 1);
    if ( (y + yy) > ClipY2 ) rect.bottom -= ((y + yy) - ClipY2 - 1);

    return BackScreen->BltFast( x, y, target, &rect, flag );
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// DDUTIL.CPP의 DDLoadBitmap() 함수를 사용하여 비트맵 파일을 불러온다.
/// flag의 값이 TRUE,SYSTEM이면 시스템 메모리에 FALSE,VIDEO이면 비디오 메모리에 불러온다.
///
//////////////////////////////////////////////////////////////////////////////////////////

BOOL _LoadBitmap( LPDIRECTDRAWSURFACE7 *page, char *name, BOOL flag )
{
    char pname[80];

    strcpy( pname, "DATA//" );
    strcat( pname, name );

    *page = DDLoadBitmap( DirectOBJ, pname, 0, 0, flag );
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
///
/// 비트맵 파일을 불러오기 위해 미리 마련된 배열 BmpScreen에 할당된 메모리를 소멸시킨다.
///
//////////////////////////////////////////////////////////////////////////////////////////

void _RemoveBMP( void )
{
    for ( int i = 0; i < 10; i++ )
    {
        if ( BmpScreen[i] != NULL ) BmpScreen[i] -> Release();
        BmpScreen[i] = NULL;
    }
}

void _Convert( BYTE *off )
{
    int  length, loop;
    int  skipbyte, putsbyte, i;
    char index;
    WORD *off2, data, r, g, b;

    length = (*(WORD*)off);
    off += 2;

    for ( loop = 0; loop < length; loop++ )
    {
        index = *off++;
        skipbyte = 0;

        while ( index-- )
        {
            skipbyte += (*(WORD*)off);
            off += 2;

            putsbyte  = (*(WORD*)off);
            off += 2;

            if ( putsbyte )
            {
                off2 = (WORD *)off;

                for ( i = 0; i < (putsbyte / 2); i++ )
                {
                    data = off2[i];

                    r = (data & 0xF800) >> 11;
                    g = (data & 0x07E0) >> 5;
                    b = (data & 0x001F);

                    data = (r << 10) | ((g / 2) << 5) | b;

                    off2[i] = data;
                }
            }
            skipbyte += putsbyte;
            off      += putsbyte;
        }
    }
}

BOOL _InstallMAP( char *name )
{
    FILE *file;
    char pname[80];

    int i;


    strcpy( pname, "DATA//" );
    strcat( pname, name );

    if ( (file = fopen(pname, "rb")) == NULL ) return FALSE;

    _RemoveMAP();

    fread( &Map[0].Xsize, 4, 1, file );
    fread( &Map[0].Ysize, 4, 1, file );

    for ( i = 0; i < Map[0].Ysize; i++ ) MapYoff[i] = (i * Map[0].Xsize);

    for ( i = 0; i < 5; i++ )
    {
        Map[i].Size = (Map[0].Xsize * Map[0].Ysize) * 2;
        Map[i].Data = (WORD *)malloc( Map[i].Size );

        fread( Map[i].Data, Map[i].Size, 1, file );

        Map[i].MaxX = Map[0].Xsize * TileData.Xsize;
        Map[i].MaxY = Map[0].Ysize * TileData.Ysize;
    }
    fclose( file );

    ScrollMaxX = Map[0].MaxX - ClipXsize;
    ScrollMaxY = Map[0].MaxY - ClipYsize;

    ScrollX = ScrollY = OldX = OldY = 0;
    return TRUE;
}

void _RemoveMAP( void )
{
    for ( int i = 0; i < 5; i++ )
    {
        if ( Map[i].Data != NULL )
        {
            free( Map[i].Data );
            Map[i].Data = NULL;
        }
    }
}

BOOL _InstallTIL( char *name, BOOL flag )
{
    FILE *file;
    char pname[80];

    strcpy( pname, "DATA//" );
    strcat( pname, name );

    if ( (file = fopen(pname, "rb")) == NULL ) return FALSE;

    fread( &TileData.Bmp, 100, 1, file );
    fread( &TileData.Xsize, 4, 1, file );
    fread( &TileData.Ysize, 4, 1, file );
    fread( TileData.Attr, 20000, 1, file );
    fread( TileData.Skip, 20000, 1, file );

   _LoadBitmap( &TileScreen, TileData.Bmp, flag );
    DDSetColorKey( TileScreen, RGB(0, 0, 0) );
    TileScreenX = BmpXsize;
    TileScreenY = BmpYsize;
    TileData.Width = (BmpXsize / TileData.Xsize);
    fclose( file );

    return TRUE;
}

void _RemoveTIL( void )
{
    if ( TileScreen != NULL )
    {
        TileScreen -> Release();
        TileScreen = NULL;
    }
    if ( CompTile != NULL ) free( CompTile );
}

BOOL _InstallPAT( int num, char *name )
{
    FILE *file;
    char sname[80], aname[80];
    char pname[80], kname[10];
    int i;

    strcpy( pname, "DATA//" );
    strcat( pname, name );

    if ( (file = fopen(pname, "rb")) == NULL ) return FALSE;

    fread( &Pattern[num].Code, 4, 1, file );

    if ( Pattern[num].Code < 0 || Pattern[num].Code > 2 )
    {
        Pattern[num].Code = 0;
        fseek( file, -4, SEEK_CUR );
    }
    fread( Pattern[num].Name, 14, 1, file );
    fread( &Pattern[num].Count, 4, 1, file );

    fread( Pattern[num].Rect, sizeof(RECT), Pattern[num].Count, file );
    fread( Pattern[num].Xsize, 4, Pattern[num].Count, file );
    fread( Pattern[num].Ysize, 4, Pattern[num].Count, file );

    PatternMode[num] = TRUE;

    if ( Pattern[num].Code == 0 )
    {
        for ( i = 0; i < lstrlen(Pattern[num].Name); i++ )
        {
            if ( Pattern[num].Name[i] == '.' )
            {
                kname[0] = Pattern[num].Name[i+1];
                kname[1] = Pattern[num].Name[i+2];
                kname[2] = Pattern[num].Name[i+3];
                kname[3] = NULL;
                break;
            }
        }
        if ( strnicmp(kname, "bmp", 3) == 0 )
        {
            _LoadBitmap( &Pattern[num].PageScreen, Pattern[num].Name, SYSTEM );
        }
        if ( strnicmp(kname, "pcx", 3) == 0 )
        {
            strcpy( pname, "DATA//" );
            strcat( pname, Pattern[num].Name );
            Pattern[num].PageScreen = _LoadPcx( DirectOBJ, pname, SYSTEM );
        }
        if ( strnicmp(kname, "img", 3) == 0 )
        {
            if ( BPP == 8 ) MessageBox( MainHwnd, "This file is 16bit mode only !!!", "NORITER", MB_OK );
            strcpy( pname, "DATA//" );
            strcat( pname, Pattern[num].Name );
            Pattern[num].PageScreen = _LoadImage( DirectOBJ, pname, SYSTEM );
        }
        DDSetColorKey( Pattern[num].PageScreen, RGB(0, 0, 0) );
    }
    if ( Pattern[num].Code == 1 || Pattern[num].Code == 2 )
    {
        fread( &Pattern[num].Size, 4, 1, file );
        fread( Pattern[num].Offset, 4, Pattern[num].Count, file );
        Pattern[num].Data = (BYTE *)malloc( Pattern[num].Size );
        fread( Pattern[num].Data, Pattern[num].Size, 1, file );

        if ( !PixelMode && BPP == 16 )
        {
            for ( i = 0; i < Pattern[num].Count; i++ ) _Convert( &Pattern[num].Data[Pattern[num].Offset[i]] );
        }
    }
    fclose( file );

    ScreenX[num] = BmpXsize;
    ScreenY[num] = BmpYsize;

    for ( i = 0; i < lstrlen(name); i++ )
    {
        if ( name[i] == '.' )
        {
            sname[i] = 0;
            aname[i] = 0;
            break;
        }
        sname[i] = name[i];
        aname[i] = name[i];
    }
    strcat( aname, ".ANI" );
    strcat( sname, ".CBM" );

    _LoadANI( num, aname );
    _LoadCBM( num, sname );

    return TRUE;
}

void _RemovePAT( void )
{
    for ( int i = 0; i < MAX_PATTERN; i++ )
    {
        if ( PatternMode[i] )
        {
            if ( !Pattern[i].Code ) Pattern[i].PageScreen -> Release(); else free( Pattern[i].Data );

            for ( int k = 0; k < MAX_FRAME; k++ )
            {
                if ( SpriteMap[i][k].Cell != NULL )
                { 
                    free( SpriteMap[i][k].Cell );
                    SpriteMap[i][k].Cell = NULL;
                }
            }
        }
        PatternMode[i] = FALSE;
    }
}

void _DrawMap( int num, int x, int y, BOOL flag )
{
    RECT rect;

    int i, k, xx, yy, xp, yp;
    int da;

    int wx = (ClipXsize / TileData.Xsize);
    int wy = (ClipYsize / TileData.Ysize);

    if ( (ClipXsize % TileData.Xsize) != 0 ) wx++;
    if ( (ClipYsize % TileData.Ysize) != 0 ) wy++;

    for ( k = 0; k <= wy; k++ )
    for ( i = 0; i <= wx; i++ )
    {
        xx = Map[0].Xpos + i;
        yy = Map[0].Ypos + k;

        if ( xx >= Map[0].Xsize || yy >= Map[0].Ysize ) continue;

        da = (int)Map[num].Data[(yy * Map[0].Xsize) + xx] - 1;

        xp = x + (i * TileData.Xsize);
        yp = y + (k * TileData.Ysize);

        rect.left   = (da % TileData.Width) * TileData.Xsize;
        rect.top    = (da / TileData.Width) * TileData.Ysize;

        rect.right  = rect.left + TileData.Xsize;
        rect.bottom = rect.top  + TileData.Ysize;

        if ( xp + TileData.Xsize <= ClipX1 || yp + TileData.Ysize <= ClipY1 || xp > ClipX2 || yp > ClipY2 ) continue;

        if ( xp < ClipX1 ) 
        { 
            rect.left += abs(ClipX1 - xp);

            xp += (ClipX1 - xp);
        }
        if ( yp < ClipY1 )
        { 
            rect.top += abs(ClipY1 - yp);

            yp += (ClipY1 - yp);
        }
        if ( (xp + TileData.Xsize) > ClipX2 ) rect.right  -= ((xp + TileData.Xsize) - ClipX2 - 1);
        if ( (yp + TileData.Ysize) > ClipY2 ) rect.bottom -= ((yp + TileData.Ysize) - ClipY2 - 1);

        if ( da < 0 ) continue;

        if ( flag )
        {
            if ( TileData.Skip[da] )
            {
                BackScreen->BltFast( xp, yp, TileScreen, &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
            }
            else
            {
                BackScreen->BltFast( xp, yp, TileScreen, &rect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
            }
        }
        else BackScreen->BltFast( xp, yp, TileScreen, &rect, DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
    }
}

void _SetClipArea( int x1, int y1, int x2, int y2 )
{
    ClipX1 = x1;
    ClipY1 = y1;
    ClipX2 = x2;
    ClipY2 = y2;

    ClipXsize = ClipX2 - ClipX1 + 1;
    ClipYsize = ClipY2 - ClipY1 + 1;

    ScrollMaxX = Map[0].MaxX - ClipXsize;
    ScrollMaxY = Map[0].MaxY - ClipYsize;
}

void _SetScrollArea( int x1, int y1, int x2, int y2 )
{
    WinX1 = x1;
    WinY1 = y1;
    WinX2 = x2;
    WinY2 = y2;

    WinXsize = WinX2 - WinX1 + 1;
    WinYsize = WinY2 - WinY1 + 1;
}

void _Scroll( int num, BOOL flag )
{
    if ( ScrollX < 0 ) ScrollX = 0;
    if ( ScrollY < 0 ) ScrollY = 0;
    if ( ScrollX > ScrollMaxX ) ScrollX = ScrollMaxX;
    if ( ScrollY > ScrollMaxY ) ScrollY = ScrollMaxY;

    Map[0].Xpos = ScrollX / TileData.Xsize;
    Map[0].Ypos = ScrollY / TileData.Ysize;

    _DrawMap( num, ClipX1 - (ScrollX % TileData.Xsize), ClipY1 - (ScrollY % TileData.Ysize), flag );

    OldX = ScrollX;
    OldY = ScrollY;
}

void _DeleteDraw( void )
{
    if ( DirectOBJ != NULL )
    {
        if ( RealScreen != NULL )
        {
            RealScreen->Release();
            RealScreen = NULL;
        }
        DirectOBJ->Release();
        DirectOBJ = NULL;
    }
}

void _SetPalette( unsigned char *data, int s, int e )
{
    int i, f = s;

    s *= 3;
    e *= 3;

    for ( i = s; i <= e; i += 3, f++ )
    {
        pPe[f].peRed   = (int)data[i  ] * 4;
        pPe[f].peGreen = (int)data[i+1] * 4;
        pPe[f].peBlue  = (int)data[i+2] * 4;
        pPe[f].peFlags = 0;
    }
    Plt->SetEntries( 0, 0, 256, pPe );
}

void _SetFade( int num )
{
    BYTE data[768];

    for ( int i = 0; i < 768; i++ ) data[i] = (Palette[i] * num) / 100;

    _SetPalette( data, 0, 255 );
}

void _RotatePalette( int a, int b, BOOL flag )
{
    unsigned char temp[3];
    int s = a * 3, e = b * 3;

    if ( flag )
    {
        memmove( temp, &Palette[e], 3 );

        for ( int i = b; i > a; i-- )
        {
            Palette[(i * 3)  ] = Palette[((i-1) * 3)  ];
            Palette[(i * 3)+1] = Palette[((i-1) * 3)+1];
            Palette[(i * 3)+2] = Palette[((i-1) * 3)+2];
        }
        memmove( &Palette[s], temp, 3 );
    }
    else
    {
        memmove( temp, &Palette[s], 3 );
        memmove( &Palette[s], &Palette[s+3], e - s );
        memmove( &Palette[e], temp, 3 );
    }
    _SetPalette( Palette, a, b );
}

void _LoadPalette( char *name )
{
    FILE *file;
    char pname[80];

    strcpy( pname, "DATA//" );
    strcat( pname, name );

    file = fopen( pname, "rb" );

    if ( file == NULL ) return;

    fread( Palette, 768, 1, file );
    fread( ColorTable, 65536, 1, file );
    fclose( file );

    _SetPalette( Palette, 0, 255 );
}

void _WindowMode( void )
{
    _RemoveMAP();
    _RemoveTIL();
    _RemovePAT();
    _RemoveBMP();
    _RemoveSND();

    if ( SoundOBJ != NULL )
    {
        SoundOBJ->Release();
        SoundOBJ=NULL;
    }
    _DeleteDraw();
}

BOOL _ScreenMode( int x, int y, int bpp, BOOL mode )
{
    DDSURFACEDESC2  ddsd;
    HRESULT         result;

    if ( DirectOBJ != NULL )
    {
        if ( RealScreen != NULL )
        {
            RealScreen->Release();
            RealScreen = NULL;
        }
    }

    if ( mode )
    {
        result = DirectOBJ->SetDisplayMode( x, y, bpp, 0, 0 );
        if ( result != DD_OK ) return Fail( MainHwnd );

        memset( &ddsd, 0, sizeof(ddsd) );
        ddsd.dwSize = sizeof( ddsd );
        ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
        ddsd.dwBackBufferCount = 1;
        result = DirectOBJ -> CreateSurface( &ddsd, &RealScreen, NULL );
        if ( result != DD_OK ) return Fail( MainHwnd );

        DDSCAPS2 ddscaps;

        ZeroMemory(&ddscaps, sizeof(ddscaps));
        ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
        result = RealScreen -> GetAttachedSurface( &ddscaps, &BackScreen );
        if ( result != DD_OK ) return Fail( MainHwnd );
    }
    else
    {
        memset( &ddsd, 0, sizeof(ddsd) );
        ddsd.dwSize = sizeof( ddsd );
        ddsd.dwFlags = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
        ddsd.dwBackBufferCount = 0;
        result = DirectOBJ -> CreateSurface( &ddsd, &RealScreen, NULL );
        if ( result != DD_OK ) return Fail( MainHwnd );

        memset( &ddsd, 0, sizeof(ddsd) );
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        ddsd.dwWidth = x;
        ddsd.dwHeight = y;
        result = DirectOBJ->CreateSurface( &ddsd, &BackScreen, NULL );
        if ( result != DD_OK ) return Fail( MainHwnd );

        result = DirectOBJ->CreateClipper( 0, &ClipScreen, NULL);
        if ( result != DD_OK ) return Fail( MainHwnd );

        result = ClipScreen->SetHWnd( 0, MainHwnd );
        if ( result != DD_OK ) return Fail( MainHwnd );

        result = RealScreen->SetClipper( ClipScreen );
        if ( result != DD_OK ) return Fail( MainHwnd );

        RECT rc;
        SetRect( &rc, 0, 0, x, y );
        AdjustWindowRectEx(&rc, GetWindowStyle(MainHwnd), GetMenu(MainHwnd) != NULL, GetWindowExStyle(MainHwnd));

        int xx, yy;

        xx = (GetSystemMetrics(SM_CXSCREEN) - x) / 2;
        yy = (GetSystemMetrics(SM_CYSCREEN) - y) / 2;

        SetWindowPos(MainHwnd, NULL, xx, yy, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
        ShowWindow( MainHwnd, SW_RESTORE );
        SetFocus( MainHwnd );
    }

    if ( bpp == 16 )
    {
        HDC hdc;

        BackScreen->GetDC(&hdc);
        SetPixel( hdc, 0, 0, RGB(255,255,255) );
        BackScreen->ReleaseDC(hdc);

        if ( _LockScreen() ) _UnlockScreen();
        if ( Screen[0] == 0xFFFF ) PixelMode = TRUE; else PixelMode = FALSE;
    }

    for ( int i = 0; i < 256; i++ )
    {
        pPe[i].peRed   = i;
        pPe[i].peGreen = i;
        pPe[i].peBlue  = i;
        pPe[i].peFlags = 0;
    }
    DirectOBJ -> CreatePalette( DDPCAPS_8BIT, pPe, &Plt, NULL );
    RealScreen -> SetPalette( Plt );
    BackScreen -> SetPalette( Plt );

    return TRUE;
}

BOOL _GameMode( HINSTANCE hInstance, int nCmdShow, int x, int y, int bpp, BOOL mode )
{
    WNDCLASS wc;
    HRESULT result;
    LPDIRECTDRAW pdd;

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockBrush(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "마왕놀이2";
    RegisterClass( &wc );

    if ( mode )
    {
        MainHwnd = CreateWindowEx (
                0, "마왕놀이2", "마왕놀이2", WS_POPUP, 0, 0,
                GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                NULL, NULL, hInstance, NULL );
    }
    else
    {
        int xx, yy;

        xx = (GetSystemMetrics(SM_CXSCREEN) - x) / 2;
        yy = (GetSystemMetrics(SM_CYSCREEN) - y) / 2;

        MainHwnd = CreateWindow( "NORITER", "Noriter LIB 3.0", WS_OVERLAPPEDWINDOW, xx, yy, x, y, NULL, NULL, hInstance, NULL );
    }
    if ( !MainHwnd ) return FALSE;

    SetFocus( MainHwnd );
    ShowWindow( MainHwnd, nCmdShow );
    UpdateWindow( MainHwnd );
    ShowCursor( FALSE );

    MainInstance = hInstance;

    int i, k;

    for ( i = 0; i < MAX_SOUND; i++ ) Sample[i] = NULL;

    for ( i = 0; i < 240; i++ ) MapOffset[i] = (i * (640 / RATIO));
    for ( i = 0; i < 480; i++ )    Offset[i] = (i * 640);

    DirectOBJ  = NULL;
    RealScreen = NULL;
    BackScreen = NULL;
    TileScreen = NULL;

    for ( i = 0; i < 5; i++ )
    {
        Map[i].Xsize = Map[i].Ysize = 100;
        Map[i].Data = NULL;
    }
    for ( i = 0; i < 10; i++ ) BmpScreen[i] = NULL;

    for ( i = 0; i < MAX_PATTERN; i++ )
    {
        PatternMode[i] = FALSE;
        for ( k = 0; k < MAX_FRAME; k++ ) SpriteMap[i][k].Cell = NULL;
    }
    TileData.Xsize = TileData.Ysize = 16;

    result = DirectDrawCreate( NULL, &pdd, NULL );
    if ( result != DD_OK ) return Fail( MainHwnd );

    result = pdd->QueryInterface(IID_IDirectDraw7, (LPVOID *) &DirectOBJ);

    if ( mode )
    {
        result = DirectOBJ->SetCooperativeLevel( MainHwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
        if ( result != DD_OK ) return Fail( MainHwnd );
    }
    else
    {
        result = DirectOBJ->SetCooperativeLevel( MainHwnd, DDSCL_NORMAL );
        if ( result != DD_OK ) return Fail( MainHwnd );
    }
    _ScreenMode( x, y, bpp, mode );

    SetRect( &WinRect, 0, 0, x, y );
    SetRect( &Rect, 0, 0, x, y );

    _SetClipArea( 0, 0, x - 1, y - 1 );
    _SetScrollArea( 0, 0, x - 1, y - 1 );

    BPP = bpp;
    FULL_SCREEN = mode;

    SCREEN_X = x;
    SCREEN_Y = y;


    int bg, sp;

    for ( bg = 0; bg < 64; bg++ )
    {
        for ( sp = 0; sp < 64; sp++ )
        {
            // Color Dodge
            Effect1[0][bg][sp] = (unsigned short)( (bg * 64) / (64 - sp) );
            // Screen
            Effect1[1][bg][sp] = (unsigned short)max(bg, sp) + ((64 - max(bg, sp)) / 64 * min(bg, sp) );
            // Dodge-Burn
            Effect1[2][bg][sp] = (unsigned short)( (bg * (64 - sp)) / 64 );
            // Different
            Effect1[3][bg][sp] = (unsigned short)( max(bg, sp)-min(bg, sp) );
            // Lighten
            Effect1[4][bg][sp] = (unsigned short)( max(bg, sp) );
        }
    }
    for ( bg = 0; bg < 32; bg++ )
    {
        for ( sp = 0; sp < 32; sp++ )
        {
            // Color Dodge
            Effect2[0][bg][sp] = (unsigned short)( (bg * 32) / (32 - sp) );
            // Screen
            Effect2[1][bg][sp] = (unsigned short)max(bg, sp) + ((32 - max(bg, sp)) / 32 * min(bg, sp) );
            // Dodge-Burn
            Effect2[2][bg][sp] = (unsigned short)( (bg * (32 - sp)) / 32 );
            // Different
            Effect2[3][bg][sp] = (unsigned short)( max(bg, sp)-min(bg, sp) );
            // Lighten
            Effect2[4][bg][sp] = (unsigned short)( max(bg, sp) );
        }
    }

    if ( !mode )
    {
        GetClientRect( MainHwnd, &WinRect );
        ClientToScreen(MainHwnd, (LPPOINT)&WinRect);
        ClientToScreen(MainHwnd, (LPPOINT)&WinRect+1);
    }

    if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
    {
        SoundCard = TRUE;
        if (SoundOBJ->SetCooperativeLevel(MainHwnd,DSSCL_PRIORITY)!=DS_OK) return Fail( MainHwnd );

        memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
        DSB_desc.dwSize = sizeof(DSBUFFERDESC);
        DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

        if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK) return Fail( MainHwnd );
        SoundDSB -> SetVolume(0);
        SoundDSB -> SetPan(0);
    }
    else SoundCard = FALSE;

    return TRUE;
}





//////////////////////////////////////////////////////////////////////////////////////////
///
/// 한글 출력에 관련된 함수들......
///
//////////////////////////////////////////////////////////////////////////////////////////




static void Put_Eng16(WORD *v,int nWidth,COLOR color,char *pData)
{
    char  *p = pPattern;
    for(int i=0;i<16;i++)
    {
        color += *p++;
        if(*pData)
        {
            if(*pData & 0X80) *(v)   = color;
            if(*pData & 0X40) *(v+1) = color;
            if(*pData & 0X20) *(v+2) = color;
            if(*pData & 0X10) *(v+3) = color;
            if(*pData & 0X08) *(v+4) = color;
            if(*pData & 0X04) *(v+5) = color;
            if(*pData & 0X02) *(v+6) = color;
            if(*pData & 0X01) *(v+7) = color;
        }
        v += nWidth;
        pData++;
    }   
}

static void Put_Han16(WORD *v,int nWidth,COLOR color,char *pData)
{
    char  *p = pPattern;

    for(int i=0;i<16;i++)
    {
        color += *p++;
        if(*pData)
        {
            if (*pData & 0X80) *(v  )=color;
            if (*pData & 0X40) *(v+1)=color;
            if (*pData & 0X20) *(v+2)=color;
            if (*pData & 0X10) *(v+3)=color;
            if (*pData & 0X08) *(v+4)=color;
            if (*pData & 0X04) *(v+5)=color;
            if (*pData & 0X02) *(v+6)=color;
            if (*pData & 0X01) *(v+7)=color;
        }
        pData++;
        if(*pData)
        {
            if (*pData & 0X80) *(v+ 8)=color;
            if (*pData & 0X40) *(v+ 9)=color;
            if (*pData & 0X20) *(v+10)=color;
            if (*pData & 0X10) *(v+11)=color;
            if (*pData & 0X08) *(v+12)=color;
            if (*pData & 0X04) *(v+13)=color;
            if (*pData & 0X02) *(v+14)=color;
            if (*pData & 0X01) *(v+15)=color;
        }
        v += nWidth;
        pData++;
    }
}

static void Put_Eng8( BYTE *v,int nWidth,char color,char *pData)
{
    char  *p = pPattern;
    for(int i=0;i<16;i++)
    {
        color += *p++;
        if(*pData)
        {
            if(*pData & 0X80) *(v)   = color;
            if(*pData & 0X40) *(v+1) = color;
            if(*pData & 0X20) *(v+2) = color;
            if(*pData & 0X10) *(v+3) = color;
            if(*pData & 0X08) *(v+4) = color;
            if(*pData & 0X04) *(v+5) = color;
            if(*pData & 0X02) *(v+6) = color;
            if(*pData & 0X01) *(v+7) = color;
        }
        v += nWidth;
        pData++;
    }   
}

static void Put_Han8(BYTE *v,int nWidth,char color,char *pData)
{
    char  *p = pPattern;

    for(int i=0;i<16;i++)
    {
        color += *p++;
        if(*pData)
        {
            if (*pData & 0X80) *(v  )=color;
            if (*pData & 0X40) *(v+1)=color;
            if (*pData & 0X20) *(v+2)=color;
            if (*pData & 0X10) *(v+3)=color;
            if (*pData & 0X08) *(v+4)=color;
            if (*pData & 0X04) *(v+5)=color;
            if (*pData & 0X02) *(v+6)=color;
            if (*pData & 0X01) *(v+7)=color;
        }
        pData++;
        if(*pData)
        {
            if (*pData & 0X80) *(v+ 8)=color;
            if (*pData & 0X40) *(v+ 9)=color;
            if (*pData & 0X20) *(v+10)=color;
            if (*pData & 0X10) *(v+11)=color;
            if (*pData & 0X08) *(v+12)=color;
            if (*pData & 0X04) *(v+13)=color;
            if (*pData & 0X02) *(v+14)=color;
            if (*pData & 0X01) *(v+15)=color;
        }
        v += nWidth;
        pData++;
    }
}

BOOL LoadFont(char *name,WORD wType)
{
    FILE *fp;
    char pname[80];

    strcpy( pname, "DATA//" );
    strcat( pname, name );

    fp = fopen(pname,"rb");

    if(fp==NULL) return FALSE;

    if(wType == ENGLISH_FNT)
    {
        fread(EF, 4096, 1, fp);
    } else
    {
        fread(HANKS,75200, 1, fp);
    }

    fclose(fp);
    return TRUE;
}

void SetFontWidth(int nHangul,int nEnglish,int nSpace)
{
    HANGUL_WIDTH  = nHangul;
    ENGLISH_WIDTH = nEnglish;
    SPACE_WIDTH   = nSpace;
}

void SetFontPattern(int nPattern)
{
    switch(nPattern)
    {
        case NORMAL_PATTERN: pPattern = &pattNormal[0];    break;
        case INC16_PATTERN : pPattern = &pattIncress16[0]; break;
        case DEC16_PATTERN : pPattern = &pattDecress16[0]; break;
        case INC8_PATTERN  : pPattern = &pattIncress8[0];  break;
        case DEC8_PATTERN  : pPattern = &pattDecress8[0];  break;
        case DECINC_PATTERN: pPattern = &pattDecInc[0];    break;
        case INCDEC_PATTERN: pPattern = &pattIncDec[0];    break;
        default: pPattern = &pattNormal[0];
    }
}

void SetUserFontPattern(char *Pattern)
{
    pPattern = Pattern;
}

BOOL InitXddFont(char *pHan,char *pEng)
{
    BOOL ret = FALSE;

    SetFontPattern(NORMAL_PATTERN);

    if(pHan)
    {
        ret = LoadFont(pHan,HANGUL_FNT);
        if(!ret) return FALSE;
        SetFontWidth(16,ENGLISH_WIDTH,8);
    }

    if(pEng)
    {
        ret = LoadFont(pEng,ENGLISH_FNT);
        SetFontWidth(HANGUL_WIDTH,8,8);
    }

    return ret;
}

int GetKsIndex(BYTE fstByte,BYTE sndByte)
{
    return ( (int)fstByte  - 0xb0) * 94 + (int)sndByte - 0xa1;
}

int PutFont( int xPos,int yPos,WORD color,char *pStr )
{
    int px;
    BYTE fstByte, secByte;
    WORD *ptr1;
    BYTE *ptr2;

    px = xPos;

    if ( BPP == 16 )
    {
        ptr1 = Screen + (yPos * (ddsd.lPitch / 2)) + xPos;
    }
    else
    {
        ptr2 = (BYTE *)Screen + (yPos * ddsd.lPitch) + xPos;
    }
    
    while(*pStr)
    {
        fstByte = *pStr++;

        if ( fstByte & 0x80 )   // Hangul
        {           
            if ( BPP == 16 )
            {
                if ( xPos >= 0 && xPos <= 624 )
                {
                    ptr1 = Screen + (yPos * (ddsd.lPitch / 2)) + xPos;
                    Put_Han16(ptr1,(ddsd.lPitch / 2),color,HANKS[GetKsIndex(fstByte,*pStr)]);
                }
                ptr1 += HANGUL_WIDTH;
                pStr++;
            }
            else
            {
                Put_Han8(ptr2,ddsd.lPitch,(char)color,HANKS[GetKsIndex(fstByte,*pStr++)]);
                ptr2 += HANGUL_WIDTH;
            }
            xPos += HANGUL_WIDTH;
        } 
        else
        {
            if ( fstByte != ' ' ) // English
            {
                if ( fstByte == '\\' )
                {
                    secByte = *pStr++;

                    if ( secByte == 'n' || secByte == 'N' )
                    {
                        xPos  = px;
                        yPos += 17;

                        if ( BPP == 16 )
                        {
                            ptr1 = Screen + (yPos * (ddsd.lPitch / 2)) + xPos;
                        }
                        else
                        {
                            ptr2 = (BYTE *)Screen + (yPos * ddsd.lPitch) + xPos;
                        }
                    }
                    else pStr--;
                }
                else
                {
                    if ( BPP == 16 )
                    {
                        if ( xPos >= 0 && xPos <= 624 )
                        {
                            ptr1 = Screen + (yPos * (ddsd.lPitch / 2)) + xPos;
                            Put_Eng16(ptr1,(ddsd.lPitch / 2),color,*(EF+fstByte));
                        }
                        ptr1 += ENGLISH_WIDTH;
                    }
                    else
                    {
                        Put_Eng8(ptr2,ddsd.lPitch,(char)color,*(EF+fstByte));
                        ptr2 += ENGLISH_WIDTH;
                    }
                    xPos += ENGLISH_WIDTH;
                }
            } 
            else
            {
                if ( BPP == 16 )
                {
                    ptr1 += ENGLISH_WIDTH;
                }
                else
                {
                    ptr2 += ENGLISH_WIDTH;
                }
                xPos += ENGLISH_WIDTH;
            }
        }
    }
    return xPos;
}

int PutFontf( int xPos,int yPos,WORD color,char *fmt,... )
{
    char buffer[256];

    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(buffer,fmt,argptr);
    va_end(argptr);

    return PutFont(xPos,yPos,color,buffer);
}

int PutFontOutline( int xPos,int yPos,WORD color,char *fmt,... )
{
    char buffer[256], str[256];

    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(buffer,fmt,argptr);
    va_end(argptr);

    strcpy( str, buffer );
    PutFont(xPos,yPos-1,0,str);
    strcpy( str, buffer );
    PutFont(xPos,yPos+1,0,str);
    strcpy( str, buffer );
    PutFont(xPos-1,yPos,0,str);
    strcpy( str, buffer );
    PutFont(xPos+1,yPos,0,str);

    return PutFont(xPos,yPos,color,buffer);
}


DWORD AviCount;

BOOL MciOpen( HWND hwnd, MCIDEVICEID *wDeviceID, char *filename )
{
    DWORD dwError;
    MCI_DGV_OPEN_PARMS mciOpen;
    MCI_STATUS_PARMS mciStatus;
    MCI_DGV_RECT_PARMS mciRect;

    mciOpen.lpstrDeviceType="avivideo";//MPEGVideo";//"avivideo";//"ActiveMovie";
    mciOpen.hWndParent=hwnd;
    mciOpen.dwStyle=WS_CHILD;
    mciOpen.lpstrElementName=filename;

    dwError=mciSendCommand (
            (WORD)NULL, 
            (WORD)MCI_OPEN,
            (DWORD)(MCI_DGV_OPEN_PARENT|MCI_DGV_OPEN_WS|MCI_OPEN_TYPE|MCI_OPEN_ELEMENT),
            (DWORD)(LPVOID)&mciOpen );

    if (dwError)
    {
        char lpszErrorText[80];
        mciGetErrorString( dwError, lpszErrorText, 79 );
    }
    *wDeviceID=mciOpen.wDeviceID;

    if ( FULL_SCREEN )
    {
        mciRect.rc.left   = 0;
        mciRect.rc.top    = 0;
        mciRect.rc.right  = 319;
        mciRect.rc.bottom = 239;
    }
    else
    {
        mciRect.rc.left   = 0;
        mciRect.rc.top    = 0;
        mciRect.rc.right  = 639;
        mciRect.rc.bottom = 479;
    }
    mciSendCommand( *wDeviceID, MCI_PUT, MCI_DGV_PUT_WINDOW | MCI_DGV_RECT, (DWORD)(LPVOID)&mciRect );

    mciStatus.dwItem = MCI_STATUS_LENGTH;
    mciSendCommand( *wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatus );
    AviCount = mciStatus.dwReturn;

    return TRUE;
}

DWORD PlayMovie( WORD wDevID, DWORD dwFrom, DWORD dwTo ) 
{ 
    DWORD dwFlags = 0; 
    MCI_DGV_PLAY_PARMS mciPlay;
 
    if (dwFrom)
    {
        mciPlay.dwFrom = dwFrom;
        dwFlags |= MCI_FROM;
    } 
 
    if (dwTo)
    {
        mciPlay.dwTo = dwTo;
        dwFlags |= MCI_TO;
    } 
    dwFlags |= MCI_NOTIFY;

    return mciSendCommand(wDevID, MCI_PLAY, dwFlags, (DWORD)(LPVOID)&mciPlay);
} 

DWORD CloseMovie( WORD wDevID )
{
    return mciSendCommand(wDevID, MCI_CLOSE, 0, NULL);
}

DWORD _PlayAVI( char name[] )
{
    MCIDEVICEID mcidec;
    DSBUFFERDESC DSB_desc;
    MCI_STATUS_PARMS mciStatus;
    FILE *file;

    file = fopen( name, "rb" ); if ( file == NULL ) return FALSE;
    fclose( file );

    if ( SoundOBJ != NULL )
    {
        SoundOBJ->Release();
        SoundOBJ=NULL;
    }
    if ( FULL_SCREEN ) _ScreenMode( 320, 240, 16, FULL_SCREEN );

    MciOpen( MainHwnd, &mcidec, name );
    PlayMovie( mcidec, 0, 0 );

    mciStatus.dwItem = MCI_STATUS_POSITION;

    while ( !_GetKeyState(VK_ESCAPE) )
    {
        mciSendCommand( mcidec, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatus );
        if ( mciStatus.dwReturn == AviCount ) break;
    }
    CloseMovie( mcidec );

    if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
    {
        SoundCard = TRUE;
        if (SoundOBJ->SetCooperativeLevel(MainHwnd,DSSCL_PRIORITY)!=DS_OK) return Fail( MainHwnd );

        memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
        DSB_desc.dwSize = sizeof(DSBUFFERDESC);
        DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

        if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK) return Fail( MainHwnd );
        SoundDSB -> SetVolume(0);
        SoundDSB -> SetPan(0);
    }
    else SoundCard = FALSE;

    if ( FULL_SCREEN ) _ScreenMode( SCREEN_X, SCREEN_Y, BPP, FULL_SCREEN );
    return TRUE;
}

static void _DrawBar2( LPBYTE tar, int x1, int y1, int x2, int y2, unsigned char color )
{
    int i, mx = x2 - x1 + 1;

    for ( i = y1; i <= y2; i++ ) memset( &tar[(i * 640) + x1], color, mx );
}

void _Mozaik( int start, int end, int delay )
{
    int i, k, x2, y2;
    unsigned char color, *Buffer1, *Buffer2;

    if ( BPP != 8 ) return;

    Buffer1 = (unsigned char *)malloc( 307200 );
    Buffer2 = (unsigned char *)malloc( 307200 );

    if ( _LockScreen() ) _UnlockScreen();

    for ( i = 0; i < 480; i++ ) memmove( &Buffer1[i*640], &Screen[Offset[i]], 640 );

    while ( TRUE )
    {
        for ( i = 0; i < 480; i += start )
        for ( k = 0; k < 640; k += start )
        {
            x2 = k + start - 1;
            y2 = i + start - 1;

            if ( x2 > 639 ) x2 = 639;
            if ( y2 > 479 ) y2 = 479;

            color = Buffer1[(i * 640) + k];

            _DrawBar2( Buffer2, k, i, x2, y2, color );
        }
        for ( i = 0; i < 480; i++ ) memmove( &Screen[Offset[i]], &Buffer2[i * 640], 640 );

        _CopyScreen( TRUE );
        _Delay( delay );

        if ( start > end ) start--;
        if ( start < end ) start++;

        if ( start == end ) break;
    }
    _ClearScreen( RGB2(0,0,0) );

    free( Buffer1 );
    free( Buffer2 );
}

void _WaveScreen( int count, double range )
{   
    int data;
    double sdata, m;
    RECT rect = { 0, 0, 640, 480 };
    static int f = 0;

    _ClearScreen( RGB2(0,0,0) );

    if ( ++f > count ) f = 0;
    m = f;

    for ( int i = 0; i < 480; i++ )
    {
        sdata = sin((m * (360 / (double)count)) * Pie2) * range;
        data = (int)sdata;
    
        rect.top    = i;
        rect.bottom = i + 1;

        _DrawBmp( rect, data, i, BmpScreen[0], DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT );
    
        if ( ++m > count ) m = 0;
    }   
}

