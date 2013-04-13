#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <ddraw.h>

#include "ddutil.h"

int BmpXsize, BmpYsize;

FILE				*bmpFile;
BITMAPFILEHEADER	bmpHeader;
LPBITMAPINFOHEADER	bmpInfoHeader;
LPBITMAPINFO		bmpInfo;

int		colorNum;
int		widthX, widthY;
LPSTR	Data, Temp;
int		Fsize;

extern HWND hwnd;
extern WORD RGB2(WORD r, WORD g, WORD b);

extern "C" IDirectDrawSurface *_LoadImage(IDirectDraw *pdd, LPCTSTR strFilename, BOOL flag)
{
	FILE *file;
	int x,y;
	long Size;

	file=fopen((LPSTR)strFilename, "rb");
	fread(&x, 4, 1, file);
	fread(&y, 4, 1, file);
	Size=(x*y*2);
	Data=(LPSTR)malloc(Size);
	fread(Data, Size, 1, file);
	fclose(file);

	DDSURFACEDESC		ddsd;
	IDirectDrawSurface	*pdds;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN;
	if(flag)ddsd.ddsCaps.dwCaps|=DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth=BmpXsize=x;
	ddsd.dwHeight=BmpYsize=y;
	if(pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)return NULL;

	WORD *buffer;
	HRESULT result;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	result=pdds->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	if(result != DD_OK)return NULL;
	buffer=(WORD*)ddsd.lpSurface;
	memmove(buffer, Data, Size);
	pdds->Unlock(NULL);
	free(Data);

	return pdds;
}

static void _Decode()
{
	int i;
	unsigned char ch, data;
	int x=0, Size=0;

	while(1)
	{
		ch=Temp[x++];

		if((ch & 0xC0) == 0xC0)
		{
			data=Temp[x++];

			for(i=0; i<(ch & 0x3F); i++)Data[Size++]=data;
		}
		else Data[Size++]=ch;

		if(x>=Fsize)break;
	}
}

extern "C" IDirectDrawSurface *DDLoadBitmap(IDirectDraw *pdd, LPCTSTR szBitmap, int dx, int dy, BOOL flag)
{
	HBITMAP				hbm;
	BITMAP				bmp;
	DDSURFACEDESC		ddsd;
	IDirectDrawSurface	*pdds;

	hbm=(HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx, dy, LR_CREATEDIBSECTION);

	if(hbm==NULL)
		hbm=(HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if(hbm==NULL)
		return NULL;

	GetObject(hbm, sizeof(bmp), &bmp);

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN;
	if(flag)ddsd.ddsCaps.dwCaps|=DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth=bmp.bmWidth;
	ddsd.dwHeight=bmp.bmHeight;
	BmpXsize=bmp.bmWidth;
	BmpYsize=bmp.bmHeight;

	if(pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)return NULL;

	DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);

	DeleteObject(hbm);

	return pdds;
}

extern "C" HRESULT DDCopyBitmap(IDirectDrawSurface *pdds, HBITMAP hbm, int x, int y, int dx, int dy)
{
	HDC				hdcImage;
	HDC				hdc;
	BITMAP			bm;
	DDSURFACEDESC	ddsd;
	HRESULT			hr;

	if(hbm==NULL || pdds==NULL)return E_FAIL;

	pdds->Restore();

	hdcImage=CreateCompatibleDC(NULL);
	
	if(!hdcImage)
	{
		OutputDebugString(TEXT("createcompatible dc failed\n"));
	}
	SelectObject(hdcImage, hbm);
	GetObject(hbm, sizeof(bm), &bm);

	dx=dx==0? bm.bmWidth : dx;
	dy=dy==0? bm.bmHeight: dy;

	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_HEIGHT | DDSD_WIDTH;
	pdds->GetSurfaceDesc(&ddsd);

	if((hr=pdds->GetDC(&hdc))==DD_OK)
	{
		StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y, dx, dy, SRCCOPY);
		pdds->ReleaseDC(hdc);
	}
	DeleteDC(hdcImage);

	return hr;
}

extern "C" DWORD DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb)
{
	COLORREF rgbT;
	HDC hdc;
	DWORD dw=CLR_INVALID;
	DDSURFACEDESC ddsd;
	HRESULT hres;

	if(rgb!=CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
	{
		rgbT=GetPixel(hdc, 0, 0);
		SetPixel(hdc, 0, 0, rgb);
		pdds->ReleaseDC(hdc);
	}

	ddsd.dwSize=sizeof(ddsd);
	while((hres=pdds->Lock(NULL, &ddsd, 0, NULL))==DDERR_WASSTILLDRAWING);

	if(hres==DD_OK)
	{
		dw=*(DWORD*)ddsd.lpSurface;
		dw&=(1<<ddsd.ddpfPixelFormat.dwRGBBitCount)-1;
		pdds->Unlock(NULL);
	}

	if(rgb!=CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
	{
		SetPixel(hdc, 0, 0, rgbT);
		pdds->ReleaseDC(hdc);
	}

	return dw;
}

extern "C" HRESULT DDSetColorKey(IDirectDrawSurface *pdds, COLORREF rgb)
{
	DDCOLORKEY	ddck;

	ddck.dwColorSpaceLowValue=DDColorMatch(pdds, rgb);
	ddck.dwColorSpaceHighValue=ddck.dwColorSpaceLowValue;
	return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}