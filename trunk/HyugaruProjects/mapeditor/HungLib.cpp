#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

#include "ddutil.h"
#include "HungLib.h"

LRESULT CALLBACK WindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

int custom_key[KEY_MAX]={VK_DOWN, VK_LEFT, VK_UP, VK_RIGHT, VK_RETURN, VK_ESCAPE, VK_SPACE};
bool key_push[KEY_MAX];

//////////////////////////////////////////////////////////////////
//CHungLib �޼ҵ�
CHungLib::CHungLib(HWND hwnd, HINSTANCE hInstance, int nCmdShow, char* class_name, char* window_name, int x, int y, bool usm)
{
	//���� �ʱ�ȭ
	use_system_memory=usm;
	screen_x=x;
	screen_y=y;
	surface_count=0;
	left_button_push=false;
	right_button_push=false;

	//������ ����
	WNDCLASS wc;

	wc.style=CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc=WindowProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor=LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground=GetStockBrush(BLACK_BRUSH);
	wc.lpszMenuName=NULL;
	wc.lpszClassName=class_name;
	RegisterClass(&wc);

	hwnd=CreateWindowEx(0, class_name, window_name, WS_POPUP, 0, 0,
						GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
						NULL, NULL, hInstance, NULL);

	if(!hwnd)Fail(hwnd);

	SetFocus(hwnd);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	ShowCursor(FALSE);

	//ȭ�� ����
	HRESULT result;

	//DirectDraw��ü ����
	result=DirectDrawCreate(NULL, &lpDD, NULL);
	if(result!=DD_OK)Fail(hwnd);

	//Ǯ��ũ��
	result=lpDD->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
	if(result!=DD_OK)Fail(hwnd);

	//���÷��� ��� ����
	result=lpDD->SetDisplayMode(screen_x, screen_y, 16);
	if(result!=DD_OK)Fail(hwnd);

	//Primary Surface ��ü ������ ���� ����ü ���� ����
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount=1;
	result=lpDD->CreateSurface(&ddsd, &RealScreen, NULL);
	if(result!=DD_OK)Fail(hwnd);

	//BackSurface��ü�� �����Ͽ� Primary Surface ��ü�� �����Ѵ�.
	memset(&ddscaps, 0, sizeof(ddscaps));
	ddscaps.dwCaps=DDSCAPS_BACKBUFFER;
	result=RealScreen->GetAttachedSurface(&ddscaps, &BackScreen);
	if(result!=DD_OK)Fail(hwnd);

	//��Ÿ �ʱ�ȭ
	SetClip(CLIP_NORMAL);
	dc_mode=false;
	for(int i=0; i<KEY_MAX; i++)key_push[i]=false;
}

CHungLib::~CHungLib()
{
	if(lpDD != NULL)
	{
		//RealScreen ����
		if(RealScreen != NULL)
		{
			RealScreen->Release();
			RealScreen=NULL;
		}

		//�����̽��� ����
		for(int i=0; i<SURFACE_MAX; i++)
			if(ddsurface[i] != NULL)
			{
				ddsurface[i]->Release();
				ddsurface[i]=NULL;

				delete[] surface_name[i];
			}
		
		lpDD->Release();
		lpDD=NULL;
	}
}

void CHungLib::BeginDC()
{
	dc_mode=true;
	BackScreen->GetDC(&hdc);
	SetBkMode(hdc,TRANSPARENT);
}

void CHungLib::EndDC()
{
	dc_mode=false;
	BackScreen->ReleaseDC(hdc);
}

void CHungLib::PutText(RECT* tarRECT, char* text)
{
	if(!dc_mode)BeginDC();
	if(tarRECT->left>screen_x || tarRECT->top>screen_y)return;

	DrawText(hdc,text,strlen(text),tarRECT,NULL);
}

void CHungLib::PutText(int x, int y, char* text)
{
	if(!dc_mode)BeginDC();
	if(x>screen_x || y>screen_y)return;

	TextOut(hdc,x,y,text,strlen(text));
}

void CHungLib::Image(int x, int y, int surface_id)
{
	if(dc_mode)EndDC();
	if(surface_id<0 || surface_id>=surface_count)return;	//�����̽� ��Ͽ� ���� ���� ��

	RECT rect;
	memcpy(&rect, &surface_rect[surface_id], sizeof(RECT));

	//���� Ŭ����
	if(x<clip.left)
	{
		if(x+rect.right<clip.left)return;	//ȭ�� ������ ������ �� ����
		rect.left+=(clip.left-x);
		x=clip.left;
	}
	//���� Ŭ����
	if(y<clip.top)
	{
		if(y+rect.bottom<clip.top)return;	//ȭ�� ������ ������ �� ����
		rect.top+=(clip.top-y);
		y=clip.top;
	}
	//������ Ŭ����
	if(x>=clip.right)return;	//ȭ�� ������ ������ �� ����
	if(x+(rect.right-rect.left)>=clip.right)
	{
		rect.right-=((x+(rect.right-rect.left))-clip.right);
	}
	//�Ʒ��� Ŭ����
	if(y>=clip.bottom)return;	//ȭ�� ������ ������ �� ����
	if(y+(rect.bottom-rect.top)>=clip.bottom)
	{
		rect.bottom-=((y+(rect.bottom-rect.top))-clip.bottom);
	}

	DWORD flag;
	if(surface_trans[surface_id])flag=DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY;
		else flag=DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY;

	BackScreen->BltFast(x, y, ddsurface[surface_id], &rect, flag);
}

void CHungLib::Image(int x, int y, char* name)
{
	int id=GetImageID(name);
	if(id>=0)Image(x,y,GetImageID(name));
}

int CHungLib::GetImageID(char* name)
{
	for(int i=0; i<surface_count; i++)
		if(strcmp(name, surface_name[i])==0)
			return i;
	return -1;
}

void CHungLib::Render()
{
	if(dc_mode)EndDC();

	old_time=new_time;
	new_time=GetCurTime();

	HRESULT hr=RealScreen->Flip(NULL, DDFLIP_WAIT);
	if(hr==DDERR_SURFACELOST)
	{
		if(!use_system_memory)RestoreAll();
		RealScreen->Restore();
	}
}

WORD* CHungLib::GetSurfaceAddress(int surface_id)
{
	if(surface_id<0 || surface_id>=SURFACE_MAX)return NULL;

	//�����̽� �ּ� ���
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsurface[surface_id]->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	WORD* screen_address=(WORD*)ddsd.lpSurface;
	ddsurface[surface_id]->Unlock(NULL);

	return screen_address;
}

void CHungLib::SetClip(int left, int top, int right, int bottom)
{
	SetRect(&clip, left, top, right, bottom);
}

void CHungLib::SetClip(int mode)
{
	if(mode==CLIP_NORMAL)SetRect(&clip, 0, 0, screen_x, screen_y);
}

//�ʱ�ȭ ����
void CHungLib::Fail(HWND hwnd)
{
	ShowWindow(hwnd, SW_HIDE);
	MessageBox(hwnd, "DIRECT X �ʱ�ȭ�� �����߽��ϴ�.", window_name, MB_OK);
	DestroyWindow(hwnd);
	exit(1);
}

long CHungLib::GetCurTime()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	
	return (long)((DWORD)time.LowPart>>2);
}

long CHungLib::GetFPS()
{
	if(new_time-old_time<=0)return 0;
	return 1000000L/(new_time-old_time);
}

void CHungLib::AddSurface(char* name, char* src)
{
	AddSurfaceUnion(0, name, src, NULL);
}

void CHungLib::AddSurface(char* name, char* src, COLORREF key)
{
	AddSurfaceUnion(1, name, src, key);
}

void CHungLib::AddSurfaceUnion(int mode, char* name, char* src, COLORREF key)
{
	//���ο� ���� �迭�� �߰�
	ddsurface[surface_count]=DDLoadBitmap(lpDD, src, 0, 0, use_system_memory);
	surface_name[surface_count]=new char[strlen(name+1)];
	strcpy(surface_name[surface_count],name);
	file_name[surface_count]=new char[strlen(src+1)];
	strcpy(file_name[surface_count],src);

	//����Ű ����
	if(mode==0)
		surface_trans[surface_count]=false;
	else if(mode==1)
	{
		surface_colorkey[surface_count]=key;
		surface_trans[surface_count]=true;
		DDSetColorKey(ddsurface[surface_count], key);
	}

	//�����̽� ũ�� ���
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsurface[surface_count]->GetSurfaceDesc(&ddsd);
	SetRect(&surface_rect[surface_count],0,0,ddsd.dwWidth,ddsd.dwHeight);

	surface_count++;
}

bool CHungLib::GetKey(int key)
{
	if(_GetKeyState(custom_key[key]))
	{
		if(!key_push[key])
		{
			key_push[key]=true;
			return true;
		}
		else return false;
	}
	else
	{
		key_push[key]=false;
		return false;
	}
}


bool CHungLib::LeftClick()
{
	if(LeftButton)
	{
		if(!left_button_push)
		{
			left_button_push=true;
			return true;			
		}
		else return false;
	}
	else
	{
		left_button_push=false;
		return false;
	}
}

bool CHungLib::RightClick()
{
	if(RightButton)
	{
		if(!right_button_push)
		{
			right_button_push=true;
			return true;			
		}
		else return false;
	}
	else
	{
		right_button_push=false;
		return false;
	}
}

void CHungLib::RestoreAll()
{
	int max=surface_count;
	surface_count=0;
	for(int i=0; i<max; i++)
	{
		if(surface_trans[i])
			AddSurface(surface_name[i], file_name[i], surface_colorkey[i]);
		else
			AddSurface(surface_name[i], file_name[i]);
	}
}
