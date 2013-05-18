#include "Extern.h"

#include <stdio.h>
#include <crtdbg.h>
#include <time.h>

JDirectDraw* jdd;
JResourceManager* jre;
char* backbuffer;

JFont font20;
JFont font22;

char* BmpScreen[10];

int  MouseX, MouseY;
int  ClipX1 = 0, ClipY1 = 0, ClipX2 = SCREEN_X - 1, ClipY2 = SCREEN_Y - 1, ClipXsize = SCREEN_X, ClipYsize = SCREEN_Y;

HWND MainHwnd;
HINSTANCE MainInstance;

bool activate = true;
bool window_mode = true;
bool fafa_sound = true;
bool elf_sound = true;

char DirectOBJ = NULL;	//»ç¿ë ¾È ÇÔ

BOOL SoundCard;

LPDIRECTSOUND       SoundOBJ = NULL;
LPDIRECTSOUNDBUFFER SoundDSB = NULL;
DSBUFFERDESC        DSB_desc;

HSNDOBJ Sound[MAX_SOUND];

MSG msg;

void Init()
{
	backbuffer = jdd->GetBackBuffer();
	font20=jdd->CreateFont("±¼¸²Ã¼",16,true,false,false,false,false);
	font22=jdd->CreateFont("±¼¸²Ã¼",22,true,false,false,false,false);
	srand( (unsigned)time( NULL ) );
}

void Term()
{
	jdd->DeleteFont(font20);
	jdd->DeleteFont(font22);

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

void PutFontOutlineBig(int x, int y, JColor jc,char *fmt,... )
{
    char text[256];

    va_list argptr;
    va_start(argptr, fmt);
    vsprintf(text,fmt,argptr);
    va_end(argptr);

	jdd->DrawText(backbuffer,text,font22,x+1,y,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font22,x-1,y,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font22,x,y+1,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font22,x,y-1,JColor(0,0,0));
	jdd->DrawText(backbuffer,text,font22,x,y,jc);
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
	rect.right=Orect.left+(rect.right-Orect.left)/x2;
	rect.bottom=Orect.top+(rect.bottom-Orect.top)/x2;
	rect.left=Orect.left+(rect.left-Orect.left)/x2;
	rect.top=Orect.top+(rect.top-Orect.top)/x2;

	if(x2 > 1)
	{
		jdd->DrawStretchedPicture(backbuffer, surf_name, &Drect, &rect);
	}
	else
	{
		if(flag & DDBLT_DDFX)
		{
			jdd->DrawPictureEx(backbuffer, surf_name, x, y, &rect, DPX_HFLIP);
		}
		else
		{
			jdd->DrawPicture(backbuffer, surf_name, x, y, &rect);
		}
	}	 
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
	SetRect(&tmp_rect,x1,y1,x2,y2);
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
	_Delay(1);
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
