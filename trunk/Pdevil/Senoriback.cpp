#include <windows.h>
#include <windowsx.h>
#include <winbase.h>
#include <stdio.h>
#include <ddraw.h>
#include <dsound.h>
#include <vfw.h>
#include <digitalv.h>

#include "extern.h"
#include "dsutil.h"
#include "ddutil.h"

#define _GetKeyState( vkey ) HIBYTE(GetAsyncKeyState( vkey ))
#define _GetKeyPush( vkey )  LOBYTE(GetAsyncKeyState( vkey ))

#define random(num) (int)(((long)rand() * (num)) / (RAND_MAX + 1))
#define randomize() srand((unsigned)time(NULL))

LPDIRECTDRAW7        DirectOBJ;
LPDIRECTDRAWSURFACE7 RealScreen;
LPDIRECTDRAWSURFACE7 BackScreen;
LPDIRECTDRAWSURFACE7 BmpScreen[10];
DDSURFACEDESC2       ddsd;
RECT                 WinRect, Rect;
WORD                *Screen;
int                  SCREEN_X, SCREEN_Y;

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

BOOL SoundCard;

int  MouseX, MouseY;
int  ClipX1 = 0, ClipY1 = 0, ClipX2 = 799, ClipY2 = 599, ClipXsize = 800, ClipYsize = 600;

int  BPP;
BOOL FULL_SCREEN, PixelMode, ScanLine = FALSE;

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

HRESULT _DrawBmp2( RECT rect, int x, int y, LPDIRECTDRAWSURFACE7 target, long flag, RECT clip, int x2)
{
	int xx = rect.right - rect.left, yy = rect.bottom - rect.top;

    if ( x + xx - 1 < clip.left || y + yy - 1 < clip.top || x > clip.right || y > clip.bottom ) return DD_OK;

	RECT Drect;
	Drect.left=x; Drect.right=x+xx;
	Drect.top=y; Drect.bottom=y+yy;
	DDBLTFX fx;
	fx.dwSize=sizeof(fx);
	fx.dwDDFX=DDBLTFX_MIRRORLEFTRIGHT;
	
    if ( x < clip.left )
    { 
		if(flag&DDBLT_DDFX) rect.right -= abs(clip.left - x);
			else rect.left += abs(clip.left - x);
        Drect.left += abs(clip.left - x);
        xx = Drect.right - Drect.left;

        x += (clip.left - x);
    }
    if ( y < clip.top )
    { 
        rect.top += abs(clip.top - y);
        Drect.top += abs(clip.top - y);
        yy = Drect.bottom - Drect.top;

        y += (clip.top - y);
    }
    if ( (x + xx) > clip.right ){
		if(flag&DDBLT_DDFX) rect.left += ((x + xx) - clip.right - 1);
			else rect.right  -= ((x + xx) - clip.right - 1);
		Drect.right -= ((x + xx) - clip.right - 1);
	}
    if ( (y + yy) > clip.bottom ){
		rect.bottom -= ((y + yy) - clip.bottom - 1);
		Drect.bottom -= ((y + yy) - clip.bottom - 1);
	}
	return BackScreen -> Blt(&Drect, target, &rect, flag, &fx);
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

void _WindowMode( void )
{
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
    wc.lpszClassName = "용사탄생";
    RegisterClass( &wc );

    if ( mode )
    {
        MainHwnd = CreateWindowEx (
                0, "용사탄생", "용사탄생", WS_POPUP, 0, 0,
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

    int i;

    for ( i = 0; i < MAX_SOUND; i++ ) Sample[i] = NULL;

    DirectOBJ  = NULL;
    RealScreen = NULL;
    BackScreen = NULL;

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

    BPP = bpp;
    FULL_SCREEN = mode;

    SCREEN_X = x;
    SCREEN_Y = y;

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

static void Put_Eng(WORD *v,int nWidth,COLOR color,char *pData)
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

static void Put_Han(WORD *v,int nWidth,COLOR color,char *pData)
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

    px = xPos;

    ptr1 = Screen + (yPos * (ddsd.lPitch / 2)) + xPos;
    
    while(*pStr)
    {
        fstByte = *pStr++;

        if ( fstByte & 0x80 )   // Hangul
        {           
	        if ( xPos >= 0 && xPos <= 784 )
            {
                ptr1 = Screen + (yPos * (ddsd.lPitch / 2)) + xPos;
                Put_Han(ptr1,(ddsd.lPitch / 2),color,HANKS[GetKsIndex(fstByte,*pStr)]);
            }
            ptr1 += HANGUL_WIDTH;
            pStr++;
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

                        ptr1 = Screen + (yPos * (ddsd.lPitch / 2)) + xPos;
                    }
                    else pStr--;
                }
                else
                {
	               if ( xPos >= 0 && xPos <= 784 )
                   {
                       ptr1 = Screen + (yPos * (ddsd.lPitch / 2)) + xPos;
                       Put_Eng(ptr1,(ddsd.lPitch / 2),color,*(EF+fstByte));
                   }
                   ptr1 += ENGLISH_WIDTH;
                   xPos += ENGLISH_WIDTH;
                }
            } 
            else
            {
                ptr1 += ENGLISH_WIDTH;
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
        mciRect.rc.right  = 799;
        mciRect.rc.bottom = 599;
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

    for ( i = y1; i <= y2; i++ ) memset( &tar[(i * 800) + x1], color, mx );
}
