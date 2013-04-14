#include <stdio.h>
#include <time.h>

#include "HungLib.h"
#include "useful.h"

LRESULT CALLBACK WindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

int custom_key[KEY_MAX]={VK_DOWN, VK_LEFT, VK_UP, VK_RIGHT, VK_RETURN, VK_ESCAPE, VK_SPACE};
int key_push[KEY_MAX];

JDirectDraw* jdd;
JResourceManager* jre;

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
	jdd=CreateDirectDraw();
	jre=CreateDXResourceManager(jdd);

	//DirectSound 초기값
	SoundOBJ = NULL;
	SoundDSB = NULL;

	//변수 초기화
	this->window_name = window_name;
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
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName=NULL;
	wc.lpszClassName=class_name;
	RegisterClass(&wc);

//	hwnd=CreateWindowEx(0, class_name, window_name, WS_POPUP, 0, 0,
//						GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
//						NULL, NULL, hInstance, NULL);

//	if(!hwnd)Fail(hwnd);

	LONG ws=WS_OVERLAPPEDWINDOW|WS_VISIBLE;
	ws &= ~WS_THICKFRAME;
	ws &= ~WS_MAXIMIZEBOX;

	RECT crt;
	SetRect(&crt, 0, 0, x, y);
	AdjustWindowRect(&crt, ws, FALSE);

	hwnd = CreateWindow(class_name, window_name, ws, 100, 100, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);

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

	jdd->Initialize(NULL,hwnd,x,y,16,true,true);
	jdd->SetFrameRate(100);
	
	//윈도우창 이동
	jdd->OnMove(100, 100);
	SetCursor(LoadCursor(0, IDC_ARROW));

	backbuffer = jdd->GetBackBuffer();
	font = jdd->CreateFont("굴림체",14,true,false,false,false,false);
	srand( (unsigned)time( NULL ) );

	//기타 초기화
	for(int i=0; i<KEY_MAX; i++)key_push[i]=false;
}

CHungLib::~CHungLib()
{
	jdd->Cleanup();

	/*for(int i = 0; i < SURFACE_MAX; i++)
	{
		delete [] surface_name[i];
		delete []  file_name[i];
	}*/
}

void CHungLib::PutText(RECT* tarRECT, char* text, JColor rgb)
{
	jdd->DrawText(backbuffer, text, font, tarRECT, rgb);
}

void CHungLib::PutText(int x, int y, char* text, JColor rgb)
{
	jdd->DrawText(backbuffer, text, font, x + 1, y + 1, JColor(0, 0, 0));
	jdd->DrawText(backbuffer, text, font, x, y, rgb);
}

void CHungLib::Image(int x, int y, int surface_id, int frame)
{
	if(surface_id<0 || surface_id>=surface_count)return;	//서페이스 목록에 없는 것일 때

	//Image(x, y, surface_name[surface_id], frame);

	char* name = surface_name[surface_id];

	if(animation[surface_id]==NULL)
	{
		jdd->DrawPicture(backbuffer, name, x, y, NULL);
	}
	else
	{
		RECT rect;
		if((animation[surface_id]->type)==0)animation[surface_id]->SetFrame(frame);
		memcpy(&rect, &(animation[surface_id]->ani_rect), sizeof(RECT));	//애니메이션
		jdd->DrawPicture(backbuffer, name, x, y, &rect);
	}
}

void CHungLib::Image(int x, int y, char* name, int frame)
{
	Image(x, y, GetImageID(name), frame);
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
	jdd->Render();

	return true;
}

//초기화 실패
void CHungLib::Fail(HWND hwnd)
{
	ShowWindow(hwnd, SW_HIDE);
	MessageBox(hwnd, "DIRECT X 초기화에 실패했습니다.", window_name, MB_OK);
	DestroyWindow(hwnd);
	exit(1);
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

bool CHungLib::LoadDGF(char* name, char* src, JColor key, CAnimation* ani)
{
	if(key != JColor(0, 0, 0))
	{
		JPictureInfo pi;
		pi.SetColorKey(key);

		jdd->LoadPicture(name, src, &pi, true);
	}
	else
	{
		jdd->LoadPicture(name, src, NULL, true);
	}

	//surface_name[surface_count]=new char[strlen(name+1)];
	//file_name[surface_count]=new char[strlen(src+1)];

	//strcpy(surface_name[surface_count],name);
	//strcpy(file_name[surface_count],src);

	surface_name[surface_count] = name;
	file_name[surface_count] = src;

	animation[surface_count]=ani;

	/*FILE *fp;
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

			//서페이스 크기 얻기
			fread(&dgf_x_size, sizeof(WORD), 1, fp);
			fread(&dgf_y_size, sizeof(WORD), 1, fp);
			
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
	else return false;*/

	//생성 성공
	surface_count++;
	return true;
}

void CHungLib::PlaySound(int num)
{
	if(SoundCard)SndObjPlay(Sound[num], NULL);
};