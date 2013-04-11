#include "Extern.h"

#include <stdio.h>
#include <crtdbg.h>
#include <time.h>

JDirectDraw* jdd;
JResourceManager* jre;
char* backbuffer;

JFont font20;

char* BmpScreen[10];

int  MouseX, MouseY;
int  ClipX1 = 0, ClipY1 = 0, ClipX2 = SCREEN_X - 1, ClipY2 = SCREEN_Y - 1, ClipXsize = SCREEN_X, ClipYsize = SCREEN_Y;

HWND MainHwnd;
HINSTANCE MainInstance;

bool activate = true;
bool window_mode = true;

char DirectOBJ = NULL;	//사용 안 함

BOOL SoundCard;

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

HSNDOBJ Sound[MAX_SOUND];

MSG msg;

void Init()
{
	backbuffer = jdd->GetBackBuffer();
	font20=jdd->CreateFont("굴림체",14,true,false,false,false,false);
	srand( (unsigned)time( NULL ) );
}

void Term()
{
	jdd->DeleteFont(font20);

	delete jdd;

	_CrtDumpMemoryLeaks();
}

void PutFontOutline(int x, int y, JColor jc,char *fmt,... )
{
    char text[256];

    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(text,fmt,argptr);
    va_end(argptr);

	jdd->DrawText(backbuffer,text,font20,x+1,y,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font20,x-1,y,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font20,x,y+1,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font20,x,y-1,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font20,x,y,jc);
}

void PutFont(int x, int y, JColor jc, char* text)
{
	jdd->DrawText(backbuffer,text,font20,x,y,jc);
}

void _DrawBmp(RECT rect, int x, int y, char* surf_name, int flag)
{
	jdd->DrawPicture(backbuffer,surf_name,x,y,&rect);
}

void _DrawBmp2(RECT rect, int x, int y, char* surf_name, int flag, RECT clip, int x2)
{
	RECT Orect;
	SetRect(&Orect, rect.left, rect.top, rect.right, rect.bottom);
	rect.right=rect.left+(rect.right-rect.left) * x2;
	rect.bottom=rect.top+(rect.bottom-rect.top) * x2;
    int xx = rect.right - rect.left, yy = rect.bottom - rect.top;

    if ( x + (xx*x2) - 1 < clip.left || y + (yy*x2) - 1 < clip.top || x > clip.right || y > clip.bottom ) return;

	RECT Drect;
	Drect.left=x; Drect.right=x+xx;
	Drect.top=y; Drect.bottom=y+yy;

	_DrawBmp(Drect, x, y, surf_name, flag);
}

void _Pixel( int x, int y, JColor jc)
{
	JBrush tmp_brush;
	tmp_brush=jdd->CreateBrush(jc);
	jdd->DrawLine(backbuffer,tmp_brush,x,y,x,y,1);
	jdd->DeleteBrush(tmp_brush);
}

void _DrawBox( int x1, int y1, int x2, int y2, JColor jc)
{
	JBrush tmp_brush;
	tmp_brush=jdd->CreateBrush(jc);
	RECT tmp_rect;
	SetRect(&tmp_rect,x1,y1,x1 + x2,y1 + y2);
	jdd->DrawRect(backbuffer,tmp_brush,&tmp_rect,1);
	jdd->DeleteBrush(tmp_brush);
}

void _DrawBar( int x1, int y1, int x2, int y2, JColor jc)
{
	JBrush tmp_brush;
	tmp_brush=jdd->CreateBrush(jc);
	RECT tmp_rect;
	SetRect(&tmp_rect,x1,y1,x2,y2);
	jdd->DrawRect(backbuffer,tmp_brush,&tmp_rect);
	jdd->DeleteBrush(tmp_brush);
}

void _DrawBarAlpha( int x1, int y1, int x2, int y2, JColor jc)
{
	JBrush tmp_brush;
	tmp_brush=jdd->CreateBrush(jc,0.5f);
	RECT tmp_rect;
	SetRect(&tmp_rect,x1,y1,x2,y2);
	jdd->DrawRect(backbuffer,tmp_brush,&tmp_rect);
	jdd->DeleteBrush(tmp_brush);
}

void _LockScreen()
{
}

void _UnlockScreen()
{
}

void _CopyScreen(bool mode)
{
	jdd->Render();
}

char* DDLoadBitmap(char direct_obj, char* path, int x, int y, bool system)
{
	jdd->DeleteSurface(path);
	jdd->LoadPicture(path, path, NULL, true);

	return path;
}

void DDSetColorKey(char* surf_name, JColor color)
{
	JPictureInfo pi;
	jdd->GetPictureInfo(surf_name, &pi);
	pi.SetColorKey(color);
	jdd->SetPictureInfo(surf_name, &pi);
}

int Max(int x, int y)
{
	if(x>y)return x;
		else return y;
}

int Min(int x, int y)
{
	if(x<y)return x;
		else return y;
}

int MaxMin(int x, int max, int min)
{
	if(x>max) return max;
		else if(x<min) return min;
		else return x;
}

void _Delay( DWORD count )
{
    DWORD cur;
    
    cur = GetTickCount();

    while ( (GetTickCount() - cur) < count )
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
