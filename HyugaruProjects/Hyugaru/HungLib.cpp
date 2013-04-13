#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdio.h>
#include <time.h>

#include "HungLib.h"
#include "useful.h"

LRESULT CALLBACK WindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

int custom_key[KEY_MAX]={VK_DOWN, VK_LEFT, VK_UP, VK_RIGHT, VK_RETURN, VK_ESCAPE, VK_SPACE};
int key_push[KEY_MAX];

//DrawData 메소드
void DrawData::Set(int vx, int vy, int vid, int vframe)
{
	x=vx;
	y=vy;
	id=vid;
	frame=vframe;
}

//CAnimation 메소드
CAnimation::CAnimation()
{
	frame=delay=0;
	walk=false;
}

CAnimation::CAnimation(int x, int y, char vtype, int vframe_max, int vdelay_max)
{
	Set(x, y, vtype, vframe_max, vdelay_max);
	CAnimation();
}

void CAnimation::Set(int x, int y, char vtype, int vframe_max, int vdelay_max)
{
	x_size=x;
	y_size=y;
	SetRect(&ani_rect, 0, 0, x, y);
	type=vtype;
	if(type==2)frame_max=vframe_max*2-1;
		else frame_max=vframe_max;
	delay_max=vdelay_max;
}

void CAnimation::Process()
{
	if(type==1)	//일직선 방향 애니메이션
	{
		delay++;
		if(delay>=delay_max)
		{
			delay=0;
			frame++;
			if(frame>=frame_max)frame=0;
		}
		
		SetRect(&ani_rect, x_size*frame, 0, x_size*(frame+1), y_size);
	}
	else if(type==2)	//반복 애니메이션
	{
		delay++;
		if(delay>=delay_max)
		{
			delay=0;
			frame++;
			if(frame>=frame_max)frame=0;
		}
		int frame2=abs((frame_max/2)-frame);
		SetRect(&ani_rect, x_size*frame2, 0, x_size*(frame2+1), y_size);
	}
}

void CAnimation::SetWalk(int t, int b, int l, int r)
{
	top=t;
	bottom=b;
	left=l;
	right=r;
}

void CAnimation::SetFrame(int vframe)
{
	SetRect(&ani_rect, x_size*vframe, 0, x_size*(vframe+1), y_size);
}

//////////////////////////////////////////////////////////////////
//CHungLib 메소드
CHungLib::CHungLib(HWND hwnd, HINSTANCE hInstance, int nCmdShow, char* class_name, char* window_name, int x, int y, bool usm)
{
	//DirectSound 초기값
	SoundOBJ = NULL;
	SoundDSB = NULL;

	//변수 초기화
	use_system_memory=usm;
	screen_x=x;
	screen_y=y;
	surface_count=0;
	left_button_push=false;
	right_button_push=false;
	ZeroMemory(key_push, sizeof(int)*KEY_MAX);

	//윈도우 생성
	WNDCLASS wc;

	wc.hIcon=LoadIcon(hInstance,"icon.ico");
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

	//사운드 초기화
	if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
	{
		SoundCard = TRUE;
		if (SoundOBJ->SetCooperativeLevel(hwnd,DSSCL_PRIORITY)!=DS_OK)Fail(hwnd);

		memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
		DSB_desc.dwSize = sizeof(DSBUFFERDESC);
		DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

		if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK)Fail(hwnd);
		SoundDSB -> SetVolume(0);
		SoundDSB -> SetPan(0);
	}
	else SoundCard = FALSE;

	//화면 생성
	HRESULT result;

	//DirectDraw객체 생성
	result=DirectDrawCreate(NULL, &lpDD, NULL);
	if(result!=DD_OK)Fail(hwnd);

	//풀스크린
	result=lpDD->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
	if(result!=DD_OK)Fail(hwnd);

	//디스플레이 모드 변경
	result=lpDD->SetDisplayMode(screen_x, screen_y, 16);
	if(result!=DD_OK)Fail(hwnd);

	//Primary Surface 객체 생성을 위한 구조체 변수 설정
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount=1;
	result=lpDD->CreateSurface(&ddsd, &RealScreen, NULL);
	if(result!=DD_OK)Fail(hwnd);

	//BackSurface객체를 생성하여 Primary Surface 객체에 연결한다.
	memset(&ddscaps, 0, sizeof(ddscaps));
	ddscaps.dwCaps=DDSCAPS_BACKBUFFER;
	result=RealScreen->GetAttachedSurface(&ddscaps, &BackScreen);
	if(result!=DD_OK)Fail(hwnd);

	//기타 초기화
	SetClip(CLIP_NORMAL);
	dc_mode=false;
	for(int i=0; i<KEY_MAX; i++)key_push[i]=false;
}

CHungLib::~CHungLib()
{
	if(lpDD != NULL)
	{
		//RealScreen 제거
		if(RealScreen != NULL)
		{
			RealScreen->Release();
			RealScreen=NULL;
		}

		//서페이스들 제거
		for(int i=0; i<SURFACE_MAX; i++)
			if(ddsurface[i] != NULL)
			{
				ddsurface[i]->Release();
				ddsurface[i]=NULL;

				delete[] surface_name[i];
				surface_name[i]=NULL;

				delete[] file_name[i];
				file_name[i]=NULL;
			}
		
		lpDD->Release();
		lpDD=NULL;
	}
}

DWORD CHungLib::DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb)
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

HRESULT CHungLib::DDSetColorKey(IDirectDrawSurface *pdds, COLORREF rgb)
{
	DDCOLORKEY	ddck;

	ddck.dwColorSpaceLowValue=DDColorMatch(pdds, rgb);
	ddck.dwColorSpaceHighValue=ddck.dwColorSpaceLowValue;
	return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
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

void CHungLib::PutText(RECT* tarRECT, char* text, COLORREF rgb)
{
	if(!dc_mode)BeginDC();
	if(tarRECT->left>screen_x || tarRECT->top>screen_y)return;

	SetTextColor(hdc,rgb);
	DrawText(hdc,text,strlen(text),tarRECT,NULL);
}

void CHungLib::PutText(int x, int y, char* text, COLORREF rgb)
{
	if(!dc_mode)BeginDC();
	if(x>screen_x || y>screen_y)return;

	SetTextColor(hdc,RGB(0,0,0));
	TextOut(hdc,x+1,y+1,text,strlen(text));
	SetTextColor(hdc,rgb);
	TextOut(hdc,x,y,text,strlen(text));
}

void CHungLib::Image(int x, int y, int surface_id, int frame)
{
	if(dc_mode)EndDC();
	if(surface_id<0 || surface_id>=surface_count)return;	//서페이스 목록에 없는 것일 때

	RECT rect;
	if(animation[surface_id]==NULL)memcpy(&rect, &surface_rect[surface_id], sizeof(RECT));
		else
		{
			if((animation[surface_id]->type)==0)animation[surface_id]->SetFrame(frame);
			memcpy(&rect, &(animation[surface_id]->ani_rect), sizeof(RECT));	//애니메이션
		}

	//왼쪽 클리핑
	if(x<clip.left)
	{
		if(x+rect.right<clip.left)return;	//화면 밖으로 나갔을 때 무시
		rect.left+=(clip.left-x);
		x=clip.left;
	}
	//위쪽 클리핑
	if(y<clip.top)
	{
		if(y+rect.bottom<clip.top)return;	//화면 밖으로 나갔을 때 무시
		rect.top+=(clip.top-y);
		y=clip.top;
	}
	//오른쪽 클리핑
	if(x>=clip.right)return;	//화면 밖으로 나갔을 때 무시
	if(x+(rect.right-rect.left)>=clip.right)
	{
		rect.right-=((x+(rect.right-rect.left))-clip.right);
	}
	//아래쪽 클리핑
	if(y>=clip.bottom)return;	//화면 밖으로 나갔을 때 무시
	if(y+(rect.bottom-rect.top)>=clip.bottom)
	{
		rect.bottom-=((y+(rect.bottom-rect.top))-clip.bottom);
	}

	DWORD flag;
	if(surface_trans[surface_id])flag=DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY;
		else flag=DDBLTFAST_WAIT | DDBLTFAST_NOCOLORKEY;

	BackScreen->BltFast(x, y, ddsurface[surface_id], &rect, flag);
}

void CHungLib::Image(int x, int y, char* name, int frame)
{
	int id=GetImageID(name);
	if(id>=0)Image(x,y,GetImageID(name), frame);
}

void CHungLib::Image(DrawData ddata)
{
	Image(ddata.x, ddata.y, ddata.id, ddata.frame);
}

int CHungLib::GetImageID(char* name)
{
	for(int i=0; i<surface_count; i++)
		if(strcmp(name, surface_name[i])==0)
			return i;
	return -1;
}

bool CHungLib::Render()
{
	if(dc_mode)EndDC();

	HRESULT hr=RealScreen->Flip(NULL, DDFLIP_WAIT);
	if(hr==DDERR_SURFACELOST)
	{
		if(!use_system_memory)RestoreAll();
		RealScreen->Restore();
	}

	return true;
}

WORD* CHungLib::GetSurfaceAddress(int surface_id)
{
	if(surface_id<0 || surface_id>=SURFACE_MAX)return NULL;

	//서페이스 주소 얻기
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

//초기화 실패
void CHungLib::Fail(HWND hwnd)
{
	ShowWindow(hwnd, SW_HIDE);
	MessageBox(hwnd, "DIRECT X 초기화에 실패했습니다.", window_name, MB_OK);
	DestroyWindow(hwnd);
	exit(1);
}

void CHungLib::SurfaceCopy(char* name, char* src)
{
	int src_id=GetImageID(src);
	if(src_id<0)return;

	//빈 서페이스 생성
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN;
	if(use_system_memory)ddsd.ddsCaps.dwCaps|=DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth=(unsigned long)surface_rect[src_id].right;
	ddsd.dwHeight=(unsigned long)surface_rect[src_id].bottom;
	if(lpDD->CreateSurface(&ddsd, &ddsurface[surface_count], NULL) != DD_OK)return;

	//내용 복사
	WORD* ptr1=GetSurfaceAddress(surface_count);
	WORD* ptr2=GetSurfaceAddress(src_id);
	for(unsigned int i=0; i<(ddsd.lPitch/2*ddsd.dwHeight); i++)ptr1[i]=ptr2[i];

	//정보 복사
	surface_name[surface_count]=new char[strlen(name+1)];
	strcpy(surface_name[surface_count],name);
	file_name[surface_count]=new char[strlen(file_name[src_id]+1)];
	strcpy(file_name[surface_count],file_name[src_id]);

	memcpy(&surface_rect[surface_count], &surface_rect[src_id], sizeof(RECT));
	animation[surface_count]=animation[src_id];

	//투명키 지정
	if(surface_trans[src_id])
	{
		surface_colorkey[surface_count]=surface_colorkey[src_id];
		surface_trans[surface_count]=true;
		DDSetColorKey(ddsurface[surface_count], surface_colorkey[surface_count]);
	}		
	else surface_trans[surface_count]=false;

	surface_count++;
}

void CHungLib::ColorChange(int surface_id, WORD from, WORD to, WORD from2, WORD to2)
{
	if(surface_id<0 || surface_id>=surface_count)return;

	WORD* ptr=GetSurfaceAddress(surface_id);
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsurface[surface_id]->GetSurfaceDesc(&ddsd);

	for(unsigned int i=0; i<(ddsd.lPitch/2*ddsd.dwHeight); i++)
	{
		if(ptr[i]==from)ptr[i]=to;
			else if(ptr[i]==from2)ptr[i]=to2;
	}
}

void CHungLib::Colorize(int surface_id, int mode)
{
	WORD* ptr=GetSurfaceAddress(surface_id);
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsurface[surface_id]->GetSurfaceDesc(&ddsd);

	WORD i;
	switch(mode)
	{
		case TO_RED:
			for(int i=0; i<(ddsd.lPitch/2*ddsd.dwHeight); i++)if(ptr[i]!=0x001f)ptr[i]|=0xf800;
			break;
		case TO_GREEN:
			for(int i=0; i<(ddsd.lPitch/2*ddsd.dwHeight); i++)if(ptr[i]!=0x001f)ptr[i]|=0x07e0;
			break;
		case TO_BLUE:
			for(int i=0; i<(ddsd.lPitch/2*ddsd.dwHeight); i++)ptr[i]|=0x001e;
			break;
		case DARK:
			for(int i=0; i<(ddsd.lPitch/2*ddsd.dwHeight); i++)
			{
				if(ptr[i]!=0x001f)
				{
					WORD r,g,b;
					r=(ptr[i]&0xf800)>>11;
					g=(ptr[i]&0x07e0)>>5;
					b=ptr[i]&0x001f;

					r=r*3/10;
					g=g*3/10;
					b=b*3/10;

					ptr[i]=(r<<11) | (g<<5) | b;
				}
			}
			break;
	}
}

bool CHungLib::GetKey(int key, int delay)
{
	if(_GetKeyState(custom_key[key]))
	{
		if(key_push[key]==0)
		{
			key_push[key]=delay;
			return true;
		}
		else
		{
			if(delay<999)key_push[key]=Max(key_push[key]-1, 0);
			return false;
		}
	}
	else
	{
		key_push[key]=0;
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

void CHungLib::SaveImage(int id)
{
	if(id<0 || id>=SURFACE_MAX)return;

	//서페이스 주소 얻기
	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsurface[id]->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
	WORD* screen_address=(WORD*)ddsd.lpSurface;
	ddsurface[id]->Unlock(NULL);

	FILE *fp;

	char dgf_file_name[40];
	strcpy(dgf_file_name, file_name[id]);
	dgf_file_name[strlen(dgf_file_name)-4]=NULL;
	strcat(dgf_file_name, ".dgf");
	if(fp=fopen(dgf_file_name,"wb"))
	{
		//사용될 변수들
		WORD x_size=(WORD)ddsd.dwWidth;
		WORD y_size=(WORD)ddsd.dwHeight;
		int size=(int)(x_size*y_size);
		WORD* img_data=new WORD[size*2];
		int img_cp=0;
		WORD list[65536];
		WORD list_count=0;
		WORD count_max=0;

		//이미지 파악하기
		int tmp_count=1;
		WORD rgb_p;
		for(WORD y=0; y<y_size; y++)
			for(WORD x=0; x<x_size; x++)
			{
				WORD rgb;
				//이미지 데이터 저장
				long pt=(ddsd.lPitch/2)*y+x;
				rgb=screen_address[pt];
				//최대 반복 횟수 구하기
				if(x>0 || y>0)
				{
					if(rgb_p==rgb && tmp_count<0xffff)
					{
						tmp_count++;
					}
					else	//지금까지 색의 반복된 횟수 조사
					{
						if(count_max<tmp_count)count_max=tmp_count;
						img_data[img_cp]=rgb_p;
						img_data[img_cp+1]=tmp_count;
						img_cp+=2;
						tmp_count=1;
					}

					if(x==x_size-1 && y==y_size-1)	//이미지 마지막
					{
						if(count_max<tmp_count)count_max=tmp_count;
						img_data[img_cp]=rgb;
						img_data[img_cp+1]=tmp_count;
						img_cp+=2;
					}
				}
				rgb_p=rgb;

				//사용되는 색상수 조사
				bool dup=false;
				for(int i=0; i<list_count; i++)
					if(list[i]==rgb)dup=true;
				if(!dup)
				{
					list[list_count]=rgb;
					if(list_count<0xffff)list_count++;
				}
			}

		//사용되는 반바이트수 
		char use_hbyte1, use_hbyte2;
		if(list_count>=4096)use_hbyte1=4;
			else if(list_count>=256)use_hbyte1=3;
			else if(list_count>=16)use_hbyte1=2;
			else use_hbyte1=1;
		if(count_max>=4096)use_hbyte2=4;
			else if(count_max>=256)use_hbyte2=3;
			else if(count_max>=16)use_hbyte2=2;
			else use_hbyte2=1;

		//헤더 정보
		fprintf(fp,"DGF5");
		fwrite(&x_size, sizeof(WORD), 1, fp);
		fwrite(&y_size, sizeof(WORD), 1, fp);
		fwrite(&list_count, sizeof(WORD), 1, fp);
		fwrite(list, sizeof(WORD), list_count, fp);
		fwrite(&use_hbyte1, sizeof(char), 1, fp);
		fwrite(&use_hbyte2, sizeof(char), 1, fp);

		int dgf_max=img_cp*(use_hbyte1+use_hbyte2);
		if((dgf_max%2)==1)dgf_max++;
		unsigned char* dgf_data=new unsigned char[dgf_max];
		ZeroMemory(dgf_data, dgf_max);
		int dgf_cp=0;

		for(int i=0; i<img_cp; i+=2)
		{			
			//색상 데이터를 리스트 값으로 변환
			for(WORD j=0; j<list_count; j++)
				if(img_data[i]==list[j])
				{
					img_data[i]=j;
					break;
				}

			//색상 정보 저장
			if(use_hbyte1>=4)dgf_data[dgf_cp+3]=(unsigned char)(img_data[i]>>12);
			if(use_hbyte1>=3)dgf_data[dgf_cp+2]=(unsigned char)((img_data[i]&0x0f00)>>8);
			if(use_hbyte1>=2)dgf_data[dgf_cp+1]=(unsigned char)((img_data[i]&0x00f0)>>4);
			dgf_data[dgf_cp]=(unsigned char)(img_data[i]&0x000f);
			dgf_cp+=use_hbyte1;
			
			//반복 횟수 저장
			if(use_hbyte2>=4)dgf_data[dgf_cp+3]=(unsigned char)(img_data[i+1]>>12);
			if(use_hbyte2>=3)dgf_data[dgf_cp+2]=(unsigned char)((img_data[i+1]&0x0f00)>>8);
			if(use_hbyte2>=2)dgf_data[dgf_cp+1]=(unsigned char)((img_data[i+1]&0x00f0)>>4);
			dgf_data[dgf_cp]=(unsigned char)(img_data[i+1]&0x000f);
			dgf_cp+=use_hbyte2;
		}

		//데이터 1/2압축
		dgf_cp=(dgf_cp>>1)+(dgf_cp%2);
		for(int i=0; i<dgf_cp; i++)
		{
			dgf_data[i]=(dgf_data[i*2]<<4);
			dgf_data[i]|=dgf_data[i*2+1];
		}
		
		//이미지 저장
		fwrite(&dgf_cp, sizeof(int), 1, fp);
		fwrite(dgf_data, sizeof(char), dgf_cp, fp);
		fclose(fp);

		//메모리 비우기
		delete [] img_data;
		delete [] dgf_data;
		img_data=NULL;
		dgf_data=NULL;
	}
}

bool CHungLib::LoadDGF(char* name, char* src, COLORREF key, CAnimation* ani)
{
	FILE *fp;
	WORD dgf_x_size;
	WORD dgf_y_size;

	if(fp=fopen(src,"rb"))
	{
		//포맷 확인
		char format[5];
		fread(&format, sizeof(char), 4, fp);
		format[4]=NULL;
		if(!strcmp(format,"DGF5"))
		{
			//새로운 정보 배열에 추가
			surface_name[surface_count]=new char[strlen(name+1)];
			strcpy(surface_name[surface_count],name);
			file_name[surface_count]=new char[strlen(src+1)];
			strcpy(file_name[surface_count],src);

			//서페이스 크기 얻기
			fread(&dgf_x_size, sizeof(WORD), 1, fp);
			fread(&dgf_y_size, sizeof(WORD), 1, fp);
			SetRect(&surface_rect[surface_count], 0, 0, dgf_x_size, dgf_y_size);
			animation[surface_count]=ani;
			
			//서페이스 생성
			DDSURFACEDESC		ddsd;

			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize=sizeof(ddsd);
			ddsd.dwFlags=DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			ddsd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN;
			if(use_system_memory)ddsd.ddsCaps.dwCaps|=DDSCAPS_SYSTEMMEMORY;
			ddsd.dwWidth=dgf_x_size;
			ddsd.dwHeight=dgf_y_size;
			if(lpDD->CreateSurface(&ddsd, &ddsurface[surface_count], NULL) != DD_OK)
			{
				fclose(fp);
				return false;
			}

			//서페이스 주소 얻기
			memset(&ddsd,0,sizeof(ddsd));
			ddsd.dwSize=sizeof(ddsd);
			ddsurface[surface_count]->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);
			WORD* screen_address=(WORD*)ddsd.lpSurface;
			ddsurface[surface_count]->Unlock(NULL);

			//데이터 읽어 들이기
			//사용될 변수들
			WORD list[65536];
			WORD list_count;
			WORD count=0;
			char use_hbyte1, use_hbyte2;

			//색상 정보 얻기
			fread(&list_count, sizeof(WORD), 1, fp);
			fread(list, sizeof(WORD), list_count, fp);
			fread(&use_hbyte1, sizeof(char), 1, fp);
			fread(&use_hbyte2, sizeof(char), 1, fp);

			//이미지 데이터 읽은 후 디코딩
			int dgf_cp;
			fread(&dgf_cp, sizeof(int), 1, fp);
			unsigned char* tmp_data=new unsigned char[dgf_cp];
			unsigned char* dgf_data=new unsigned char[dgf_cp*2];
			fread(tmp_data, sizeof(char), dgf_cp, fp);
			fclose(fp);

			for(int i=0; i<dgf_cp; i++)
			{
				dgf_data[i*2]=tmp_data[i]>>4;
				dgf_data[i*2+1]=tmp_data[i]%16;
			}

			delete [] tmp_data;
			tmp_data=NULL;

			//이미지 복원
			dgf_cp=0;
			for(WORD y=0; y<dgf_y_size; y++)
				for(WORD x=0; x<dgf_x_size; x++)
				{
					WORD rgb;
					long pt=(ddsd.lPitch/2)*y+x;

					if(count<=0)
					{
						//이미지 데이터 읽기
						rgb=0;
						for(int i=use_hbyte1-1; i>=0; i--)
						{
							rgb+=(WORD)dgf_data[dgf_cp+i];
							if(i>0)rgb<<=4;
						}
						rgb=list[rgb];
						dgf_cp+=use_hbyte1;

						//반복 횟수 읽기
						count=0;
						for(int i=use_hbyte2-1; i>=0; i--)
						{
							count|=(WORD)dgf_data[dgf_cp+i];
							if(i>0)count<<=4;
						}
						dgf_cp+=use_hbyte2;
					}
					
					screen_address[pt]=rgb;
					count--;
				}
			
			//메모리 비우기
			delete [] dgf_data;
			dgf_data=NULL;

			//투명키 지정
			if(key==NULL)
				surface_trans[surface_count]=false;
			else
			{
				surface_colorkey[surface_count]=key;
				surface_trans[surface_count]=true;
				DDSetColorKey(ddsurface[surface_count], key);
			}
		}
		else return false;
	}
	else return false;

	//생성 성공
	surface_count++;
	return true;
}

void CHungLib::PlaySound(int num)
{
	if(SoundCard)SndObjPlay(Sound[num], NULL);
};