/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddutil.cpp
 *  Content:    Routines for loading bitmap and palettes from resources
 *
 ***************************************************************************/
#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <ddraw.h>

#include "ddutil.h"

int BmpXsize, BmpYsize;


FILE               *bmpFile;
BITMAPFILEHEADER    bmpHeader;              //비트맵 화일 헤더
LPBITMAPINFOHEADER  bmpInfoHeader;          //비트맵 정보 헤더
LPBITMAPINFO        bmpInfo;                //비트맵 정보

int                 colorNum;               //사용된 칼라수
int                 widthX,widthY;          //이미지의  가로,세로 크기
LPSTR               Data, Temp;
int                 Fsize;

extern HWND MainHwnd;
extern PALETTEENTRY pPe[];
extern int BPP;

struct pcx_header
{
    char Maker;
    char Ver;
    char Code;
    char Bit;
    short X1, Y1, X2, Y2, Mx, My;
    char Palette[48], Mode, Plane;
    short Nubi, Info, Hres, Vres;
    char Extra[54];

} PcxHeader;

extern WORD RGB2(WORD r, WORD g, WORD b );
extern BYTE Palette[768];

extern "C" IDirectDrawSurface7 *_LoadPcx( IDirectDraw7 *pdd, LPCTSTR name, BOOL flag )
{
    FILE *file;
    int i, x = 0, y = 0, ScreenX, ScreenY;
    char ch, data;
    BYTE *PcxData;

    if ( (file = fopen(name, "rb")) == NULL ) return NULL;

    fread( &PcxHeader, 128, 1, file );

    fseek( file, -768, SEEK_END );
    fread( Palette, 768, 1, file);
    for ( i = 0; i < 768; i++ ) Palette[i] >>= 2;
    fseek( file, 128, SEEK_SET );

    ScreenX = PcxHeader.X2 + 1;
    ScreenY = PcxHeader.Y2 + 1;

    PcxData = (BYTE *)malloc( ScreenX * ScreenY );

    while (1)
    {
        ch = fgetc( file );

        if ( (ch & 0xC0) == 0xC0 )
        {
            data = fgetc( file );

            for ( i = 0; i < (ch & 0x3F); i++ ) PcxData[(y * ScreenX) + x++] = data;
        }
        else PcxData[(y * ScreenX) + x++] = ch;

        if ( x > PcxHeader.X2 ) { x = 0; y++; }
        if ( y > PcxHeader.Y2 ) break;
    }
    fclose( file );

    DDSURFACEDESC2       ddsd;
    IDirectDrawSurface7 *pdds;
    WORD                *Ptr1;
    BYTE                *Ptr2;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    if ( flag ) ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth  = BmpXsize = ScreenX;
    ddsd.dwHeight = BmpYsize = ScreenY;
    if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK) return NULL;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    pdds -> Lock( NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL );

    if ( BPP == 8 )
    {
        Ptr2 = (BYTE *)ddsd.lpSurface;
        memmove( Ptr2, PcxData, ScreenX * ScreenY );
    }
    if ( BPP == 16 )
    {
        Ptr1 = (WORD *)ddsd.lpSurface;
        for ( i = 0; i < (ScreenX * ScreenY); i++ ) Ptr1[i] = RGB2( ((Palette[(PcxData[i] * 3)]+1) * 4), ((Palette[(PcxData[i] * 3) + 1]+1) * 4), ((Palette[(PcxData[i] * 3) + 2]+1) * 4) );

        //for ( i = 0; i < (ScreenX * ScreenY); i++ ) Ptr1[i] = RGB2( ((Palette[(PcxData[i] * 3)]+1) * 4)-1, ((Palette[(PcxData[i] * 3) + 1]+1) * 4)-1, ((Palette[(PcxData[i] * 3) + 2]+1) * 4)-1 );
    }
    pdds -> Unlock( NULL );
    free( PcxData );

    return pdds;
}

extern "C" IDirectDrawSurface7 *_LoadImage( IDirectDraw7 *pdd, LPCTSTR strFilename, BOOL flag )
{
    if ( BPP == 8 ) return NULL;

    FILE *file;
    int x, y;
    long Size;

    file = fopen( (LPSTR)strFilename, "rb" );
    fread( &x, 4, 1, file );
    fread( &y, 4, 1, file );
    Size = (x * y * 2);
    Data = (LPSTR)malloc( Size );
    fread( Data, Size, 1, file );
    fclose( file );

    DDSURFACEDESC2       ddsd;
    IDirectDrawSurface7 *pdds;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    if ( flag ) ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth  = BmpXsize = x;
    ddsd.dwHeight = BmpYsize = y;
    if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK) return NULL;

    WORD *buffer;
    HRESULT result;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    result = pdds->Lock( NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL );
    if ( result != DD_OK ) return NULL;
    buffer = (WORD *)ddsd.lpSurface;
    memmove( buffer, Data, Size );
    pdds->Unlock( NULL );
    free( Data );

    return pdds;
}

static void _Decode( void )
{
    int i;
    unsigned char ch, data;
    int x = 0, Size = 0;

    while ( 1 )
    {
        ch = Temp[x++];

        if ( (ch & 0xC0) == 0xC0 )
        {
            data = Temp[x++];

            for ( i = 0; i < (ch & 0x3F); i++ ) Data[Size++] = data;
        }
        else Data[Size++] = ch;

        if ( x >= Fsize ) break;
    }
}

extern "C" IDirectDrawSurface7 * DDLoadBitmap(IDirectDraw7 *pdd, LPCTSTR szBitmap, int dx, int dy, BOOL flag )
{
    HBITMAP              hbm;
    BITMAP               bm;
    DDSURFACEDESC2       ddsd;
    IDirectDrawSurface7 *pdds;

    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx, dy, LR_CREATEDIBSECTION);

    if (hbm == NULL)
    hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
    return NULL;

    GetObject(hbm, sizeof(bm), &bm);

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    if ( flag ) ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth  = bm.bmWidth;
    ddsd.dwHeight = bm.bmHeight;
    BmpXsize = bm.bmWidth;
    BmpYsize = bm.bmHeight;

    if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)
    return NULL;

    DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);

    DeleteObject(hbm);

    return pdds;
}

HRESULT DDReLoadBitmap(IDirectDrawSurface7 * pdds, LPCSTR szBitmap)
{
    HBITMAP                 hbm;
    HRESULT                 hr;

    hbm = (HBITMAP) LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

    if (hbm == NULL) hbm = (HBITMAP) LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (hbm == NULL)
    {
        OutputDebugString("handle is null\n");
        return E_FAIL;
    }
    hr = DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);

    if (hr != DD_OK)
    {
        OutputDebugString("ddcopybitmap failed\n");
    }
    DeleteObject(hbm);
    return hr;
}

extern "C" HRESULT DDCopyBitmap(IDirectDrawSurface7 *pdds, HBITMAP hbm, int x, int y, int dx, int dy)
{
    HDC                 hdcImage;
    HDC                 hdc;
    BITMAP              bm;
    DDSURFACEDESC2      ddsd;
    HRESULT             hr;

    if (hbm == NULL || pdds == NULL) return E_FAIL;

    pdds->Restore();

    hdcImage = CreateCompatibleDC(NULL);

    if ( !hdcImage )
    {
        OutputDebugString(TEXT("createcompatible dc failed\n"));
    }
    SelectObject(hdcImage, hbm);
    GetObject(hbm, sizeof(bm), &bm);
    
    dx = dx == 0 ? bm.bmWidth  : dx;
    dy = dy == 0 ? bm.bmHeight : dy;
 
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);

    if ((hr = pdds->GetDC(&hdc)) == DD_OK)
    {
        StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y, dx, dy, SRCCOPY);
        pdds->ReleaseDC(hdc);
    }
    DeleteDC(hdcImage);

    return hr;
}

extern "C" IDirectDrawPalette *DDLoadPalette(IDirectDraw7 * pdd, LPCSTR szBitmap)
{
    IDirectDrawPalette     *ddpal;
    int                     i;
    int                     n;
    int                     fh;
    HRSRC                   h;
    LPBITMAPINFOHEADER      lpbi;
    PALETTEENTRY            ape[256];
    RGBQUAD                *prgb;

    for (i = 0; i < 256; i++)
    {
        ape[i].peRed = (BYTE) (((i >> 5) & 0x07) * 255 / 7);
        ape[i].peGreen = (BYTE) (((i >> 2) & 0x07) * 255 / 7);
        ape[i].peBlue = (BYTE) (((i >> 0) & 0x03) * 255 / 3);
        ape[i].peFlags = (BYTE) 0;
    }

    if (szBitmap && (h = FindResource(NULL, szBitmap, RT_BITMAP)))
    {
        lpbi = (LPBITMAPINFOHEADER) LockResource(LoadResource(NULL, h));
        if (!lpbi)
            OutputDebugString("lock resource failed\n");
        prgb = (RGBQUAD *) ((BYTE *) lpbi + lpbi->biSize);
        if (lpbi == NULL || lpbi->biSize < sizeof(BITMAPINFOHEADER))
            n = 0;
        else if (lpbi->biBitCount > 8)
            n = 0;
        else if (lpbi->biClrUsed == 0)
            n = 1 << lpbi->biBitCount;
        else
            n = lpbi->biClrUsed;

        for (i = 0; i < n; i++)
        {
            ape[i].peRed = prgb[i].rgbRed;
            ape[i].peGreen = prgb[i].rgbGreen;
            ape[i].peBlue = prgb[i].rgbBlue;
            ape[i].peFlags = 0;
        }
    }
    else if (szBitmap && (fh = _lopen(szBitmap, OF_READ)) != -1)
    {
        BITMAPFILEHEADER        bf;
        BITMAPINFOHEADER        bi;

        _lread(fh, &bf, sizeof(bf));
        _lread(fh, &bi, sizeof(bi));
        _lread(fh, ape, sizeof(ape));
        _lclose(fh);

        if (bi.biSize != sizeof(BITMAPINFOHEADER))
            n = 0;
        else if (bi.biBitCount > 8)
            n = 0;
        else if (bi.biClrUsed == 0)
            n = 1 << bi.biBitCount;
        else
            n = bi.biClrUsed;

        for (i = 0; i < n; i++)
        {
            BYTE        r = ape[i].peRed;

            ape[i].peRed = ape[i].peBlue;
            ape[i].peBlue = r;
        }
    }
    pdd->CreatePalette(DDPCAPS_8BIT, ape, &ddpal, NULL);
    return ddpal;
}

extern "C" DWORD DDColorMatch(IDirectDrawSurface7 *pdds, COLORREF rgb)
{
    COLORREF rgbT;
    HDC hdc;
    DWORD dw = CLR_INVALID;
    DDSURFACEDESC2 ddsd;
    HRESULT hres;

    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);
        SetPixel(hdc, 0, 0, rgb);
        pdds->ReleaseDC(hdc);
    }

    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING);

    if (hres == DD_OK)
    {
        dw  = *(DWORD *)ddsd.lpSurface;
        dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1;
        pdds->Unlock(NULL);
    }

    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }
    return dw;
}

extern "C" HRESULT DDSetColorKey(IDirectDrawSurface7 *pdds, COLORREF rgb)
{
    DDCOLORKEY          ddck;

    ddck.dwColorSpaceLowValue  = DDColorMatch(pdds, rgb);
    ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
    return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}
