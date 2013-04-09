#include <dsound.h>

#include "JDirectDraw.h"
#include "Dsutil.h"
#include "JResourceManager.h"
#include "resource.h"

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <crtdbg.h>
#include <gdiplus.h>

using namespace Gdiplus;

JDirectDraw* jdd;
JResourceManager* jre;

char* backbuffer;
JFont font;
JFont font2;
MSG msg;

#include "data.h"

bool window_mode = true;

LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam);

//게임 모드
#define PLAYMODE	0
#define READYMODE	1
#define WINMODE		2
#define SELECTMODE	3
#define DEMOMODE	4
#define QUIT		99

int gamemode;		//게임 모드
char surfname[PLAYERS][8][15];	//서페이스 이름들

char* StrAdd(char* s1, char* s2)		//문자+문자
{
	strcpy(global_buffer,s1);
	strcat(global_buffer,s2);

	return global_buffer;
}

char* StrAdd(int n1, char* s2)			//숫자+문자
{
	char s1[10];
	itoa(n1,s1,10);
	strcpy(global_buffer,s1);
	strcat(global_buffer,s2);

	return global_buffer;
}

char* StrAdd(char* s2, int n1)			//문자+숫자
{
	char s1[10];
	itoa(n1,s1,10);
	strcpy(global_buffer,s2);
	strcat(global_buffer,s1);

	return global_buffer;
}

void prepare_global()	//전체적으로 쓰는 것 준비
{
	jre->LoadResource("data\\system.mlc");
	jre->LoadResource("data\\ball.mlc");

	//옵션 읽어오기
	if(fp=fopen("setup.sav","rb"))
	{
		for(int i=0; i<4; i++)fread(&player[i].keyL,sizeof(int),5,fp);
		fread(&music_volume,sizeof(double),1,fp);
		fread(&effect_volume,sizeof(double),1,fp);
		fread(&sound_on,sizeof(bool),1,fp);
		fclose(fp);
	}
}

void prepare_mode0()	//게임에 필요한 것들 준비
{
	JPictureInfo pi, pi2;
	JColor blue(0,0,255);
	pi.SetColorKey(blue);
	pi2.SetColorKey(blue);
	pi2.SetOpacity(0.5f);
	jre->LoadResource(StrAdd("data\\",hall.filename));

	//캐릭터에 해당하는 폴더에서 그림 가져오기
	for(int i=0; i<hall.players; i++)
	{
		strcpy(surfname[i][0],StrAdd(i,"walk1"));
		strcpy(surfname[i][1],StrAdd(i,"walk2"));
		strcpy(surfname[i][2],StrAdd(i,"walk3"));
		strcpy(surfname[i][3],StrAdd(i,"attack1"));
		strcpy(surfname[i][4],StrAdd(i,"attack2"));
		strcpy(surfname[i][5],StrAdd(i,"damage"));
		strcpy(surfname[i][6],StrAdd(i,"die"));
		strcpy(surfname[i][7],StrAdd(i,"face"));
		jdd->LoadPicture(surfname[i][0],StrAdd(player[i].name,"\\w1.gif"),&pi,true);
		jdd->LoadPicture(surfname[i][1],StrAdd(player[i].name,"\\w2.gif"),&pi,true);
		jdd->LoadPicture(surfname[i][2],StrAdd(player[i].name,"\\w3.gif"),&pi,true);
		jdd->LoadPicture(surfname[i][3],StrAdd(player[i].name,"\\p1.gif"),&pi,true);
		jdd->LoadPicture(surfname[i][4],StrAdd(player[i].name,"\\p2.gif"),&pi,true);
		jdd->LoadPicture(surfname[i][5],StrAdd(player[i].name,"\\dm.gif"),&pi,true);
		jdd->LoadPicture(surfname[i][6],StrAdd(player[i].name,"\\dt.gif"),&pi2,true);
		jdd->LoadPicture(surfname[i][7],StrAdd(player[i].name,"\\face.gif"),&pi,true);
	}
}

//#include <iostream.h>

int Menu(char* background, int menus, int x, int y, char* menu1, char* menu2, char* menu3, char* menu4, char* menu5, char* menu6, char* menu7)
{
	bool next=false;

	//메뉴
	char menu_content[7][20];
	strcpy(menu_content[0],menu1);
	strcpy(menu_content[1],menu2);
	strcpy(menu_content[2],menu3);
	strcpy(menu_content[3],menu4);
	strcpy(menu_content[4],menu5);
	strcpy(menu_content[5],menu6);
	strcpy(menu_content[6],menu7);
	
	int select_menu=0;
	bool key_up, key_down;
	while(!next)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//위 아래 이동
		if(_GetKeyState(player[0].keyJ))
		{
			if(!key_up)
			{
				select_menu=Max(0,select_menu-1);
				key_up=true;
			}
		}
		else key_up=false;

		if(_GetKeyState(player[0].keyD))
		{
			if(!key_down)
			{
				select_menu=Min(menus-1,select_menu+1);
				key_down=true;
			}
		}
		else key_down=false;

		//타이틀과 메뉴표시
		jdd->DrawPicture(backbuffer,background,0,0,NULL);
		for(int i=0;i<menus;i++)
		{
			jdd->DrawPicture(backbuffer,"Menu",x+55,i*35+y,NULL);
			jdd->DrawText(backbuffer,menu_content[i],font,x+80,i*35+y+5,JColor(0,0,0));
		}
		jdd->DrawPicture(backbuffer,"MyFace",x,select_menu*35+y-20,NULL);
		jdd->Render();
		if(_GetKeyState(player[0].keyA))
		{
			if(!key_enter)
			{
				next=true;
				key_enter=true;
			}
		}
		else key_enter=false;

		if(_GetKeyState(VK_ESCAPE))
		{
			if(!key_esc)
			{
				next=true;
				select_menu=-1;
				key_esc=true;
			}
		}
		else key_esc=false;
	}

	return select_menu;
}

//////////////////////////////////////////////////////
//선수, 공, 경기장 선택
void SelectPlayer()
{
	bool next=false;
	jdd->LoadPicture("Select","data\\select.jpg",NULL,true);
	int select_px[PLAYERS];
	int select_py[PLAYERS];
	int select_pi[PLAYERS];
	int select_ground;
	bool key_sel[PLAYERS];
	for(int i=0; i<PLAYERS; i++)
	{
		select_px[i]=0;
		select_py[i]=0;
		select_pi[i]=-1;
		player[i].ai=true;
		key_sel[i]=true;
	}
	select_ground=-1;
	while(!next)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if(!GetMessage(&msg,NULL,0,0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		jdd->DrawPicture(backbuffer,"Select",0,0,NULL);

		RECT next_r, chr_r[ALL_PLAYER], grd_r[ALL_GROUND];
		//다음으로
		SetRect(&next_r,544,458,632,476);
		//선수 선택 영역
		SetRect(&chr_r[0],3,26,127,143);
		SetRect(&chr_r[1],2,146,127,248);
		SetRect(&chr_r[2],3,252,152,347);
		SetRect(&chr_r[3],3,351,236,463);
		SetRect(&chr_r[4],131,25,250,141);
		SetRect(&chr_r[5],132,148,254,247);
		SetRect(&chr_r[6],156,251,256,346);
		//경기장 선택 영역
		SetRect(&grd_r[0],435,22,614,81);
		SetRect(&grd_r[1],409,153,495,212);
		SetRect(&grd_r[2],409,87,633,146);
		SetRect(&grd_r[3],517,153,633,212);
		SetRect(&grd_r[4],402,219,633,278);
		
		if(select_ground>=0)jdd->DrawPicture(backbuffer,StrAdd("Ball",select_ground*10),grd_r[select_ground].left,grd_r[select_ground].top,NULL);
		for(int i=0; i<hall.players; i++)
		{
			//등장
			if(player[i].ai)
			{
				if(_GetKeyState(player[i].keyA))
				{
					if(!key_sel[i])player[i].ai=false;
				}
				else key_sel[i]=false;
			}			

			if(player[i].ai)continue;
			RECT player_r;
			SetRect(&player_r,select_px[i],select_py[i],select_px[i]+60,select_py[i]+20);
			if(_GetKeyState(player[i].keyL))select_px[i]=Max(select_px[i]-5,0);
			if(_GetKeyState(player[i].keyR))select_px[i]=Min(select_px[i]+5,600);
			if(_GetKeyState(player[i].keyJ))select_py[i]=Max(select_py[i]-5,0);
			if(_GetKeyState(player[i].keyD))select_py[i]=Min(select_py[i]+5,460);
			//선수 또는 경기장 선택
			if(_GetKeyState(player[i].keyA))
			{
				if(!key_sel[i])
				{
					if(collision(&next_r,&player_r))next=true;
					for(int j=0; j<ALL_PLAYER; j++)
					{
						if(collision(&chr_r[j],&player_r))
						{
							select_pi[i]=j;
							break;
						}
					}
					for(int j=0; j<ALL_GROUND; j++)
					{
						if(collision(&grd_r[j],&player_r))
						{
							select_ground=j;
							break;
						}
					}
					key_sel[i]=true;
				}
			}
			else key_sel[i]=false;

			if(select_pi[i]>=0)jdd->DrawPicture(backbuffer,StrAdd(i+1,"P"),chr_r[select_pi[i]].left, chr_r[select_pi[i]].top+i*20,NULL);
			jdd->DrawPicture(backbuffer,StrAdd(i+1,"P"),select_px[i], select_py[i],NULL);
		}
		jdd->Render();
	}
	jdd->DeleteSurface("Select");
	//선수를 선택 안 했다면 자동 선택
	if(select_ground==-1)select_ground=rand()%ALL_GROUND;
	for(int i=0; i<hall.players; i++)
		if(select_pi[i]==-1)select_pi[i]=rand()%ALL_PLAYER;

	//경기장과 플레이어 초기화
	hall.SetHall(select_ground);
	for(int i=0; i<hall.players; i++)player[i].SetPlayer(select_pi[i],i%2);
}

//////////////////////////////////////////////////////
//키 설정
#define ALL_KEYS 81
void KeyDefine(int player_no)
{
	bool next=false;

	int defined_key[4];
	int keys[ALL_KEYS]={
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'F',
		'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		'Y', 'Z', VK_ADD, '[', ']', ';', ',', '.', '/', '-', '=', '\\', '\'', VK_BACK,
		'`', VK_CONTROL, VK_MENU, VK_DECIMAL, VK_DELETE, VK_DIVIDE, VK_DOWN, VK_END,  
		VK_RETURN, VK_HELP, VK_HOME, VK_INSERT,	VK_LEFT, VK_MULTIPLY, VK_NUMPAD0,  
		VK_NUMPAD1,	VK_NUMPAD2,	VK_NUMPAD3, VK_NUMPAD4,	VK_NUMPAD5,	VK_NUMPAD6,	VK_NUMPAD7,  
		VK_NUMPAD8,	VK_NUMPAD9,	VK_NEXT, VK_PRIOR, VK_PAUSE, VK_RIGHT, VK_SHIFT, VK_SPACE,
		VK_SUBTRACT, VK_TAB, VK_UP
	};

	//타이틀과 메뉴표시
	jdd->DrawPicture(backbuffer,"TitleDark",0,0,NULL);
	                          
	jdd->DrawText(backbuffer,"↑키를 입력하세요.",font,260,160,JColor(255,32,32));
	jdd->Render();
	bool keyset=false;
	Sleep(500);
	while(!keyset)
	{
		for(int i=0; i<ALL_KEYS; i++)
			if(_GetKeyState(keys[i]))
			{
				defined_key[0]=player[player_no].keyJ=keys[i];
				keyset=true;
				break;
			}
	}

	jdd->DrawText(backbuffer,"↓키를 입력하세요.",font,260,180,JColor(255,32,32));
	jdd->Render();
	keyset=false;
	while(!keyset)
	{
		for(int i=0; i<ALL_KEYS; i++)
			if(_GetKeyState(keys[i]) && defined_key[0]!=keys[i])
			{
				defined_key[1]=player[player_no].keyD=keys[i];
				keyset=true;
				break;
			}
	}

	jdd->DrawText(backbuffer,"←키를 입력하세요.",font,260,200,JColor(255,32,32));
	jdd->Render();
	keyset=false;
	while(!keyset)
	{
		for(int i=0; i<ALL_KEYS; i++)
			if(_GetKeyState(keys[i]) && defined_key[0]!=keys[i]  && defined_key[1]!=keys[i])
			{
				defined_key[2]=player[player_no].keyL=keys[i];
				keyset=true;
				break;
			}
	}

	jdd->DrawText(backbuffer,"→키를 입력하세요.",font,260,220,JColor(255,32,32));
	jdd->Render();
	keyset=false;
	while(!keyset)
	{
		for(int i=0; i<ALL_KEYS; i++)
			if(_GetKeyState(keys[i]) && defined_key[0]!=keys[i]  && defined_key[1]!=keys[i] && defined_key[2]!=keys[i])
			{
				defined_key[3]=player[player_no].keyR=keys[i];
				keyset=true;
				break;
			}
	}

	jdd->DrawText(backbuffer,"선택키를 입력하세요.",font,260,240,JColor(255,32,32));
	jdd->Render();
	keyset=false;
	while(!keyset)
	{
		for(int i=0; i<ALL_KEYS; i++)
			if(_GetKeyState(keys[i]) && defined_key[0]!=keys[i]  && defined_key[1]!=keys[i] && defined_key[2]!=keys[i] && defined_key[3]!=keys[i])
			{
				player[player_no].keyA=keys[i];
				keyset=true;
				break;
			}
	}
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstancem, LPSTR lpCmdLine, int nShowCmd)
{

	jdd=CreateDirectDraw();
	jre=CreateDXResourceManager(jdd);

	WNDCLASS wc={0};
	wc.hIcon=LoadIcon(hInstance,"PSYCCER.ico");
	wc.hCursor=LoadCursor(hInstance,IDC_ARROW);
	wc.lpfnWndProc=WndProc;
	wc.hInstance=hInstance;
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName="Demo";
	RegisterClass(&wc);

	if(window_mode)
	{
		LONG ws=WS_OVERLAPPEDWINDOW|WS_VISIBLE;
		ws &= ~WS_THICKFRAME;
		ws &= ~WS_MAXIMIZEBOX;

		RECT crt;
		SetRect(&crt, 0, 0, 640, 480);
		AdjustWindowRect(&crt, ws, FALSE);

		hwnd = CreateWindow("Demo","싸이커", ws, 100, 100, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);
		ShowCursor( TRUE );
	}
	else
	{
		hwnd=CreateWindow("Demo","싸이커",WS_POPUP|WS_VISIBLE,0,0,640,480,NULL,NULL,hInstance,NULL);
		ShowCursor( FALSE );
	}

	jdd->Initialize(NULL/*jdd->GetDeviceCount()-1*/,hwnd,640,480,16,true,true);
	
	
	//윈도우창 이동
	if(window_mode)
	{
		jdd->OnMove(100, 100);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}

	//사운드 초기화
	if ( DirectSoundCreate(NULL,&SoundOBJ,NULL) == DS_OK )
	{
		SoundCard = TRUE;
		if (SoundOBJ->SetCooperativeLevel(hwnd,DSSCL_PRIORITY)!=DS_OK) return 0;

		memset(&DSB_desc,0,sizeof(DSBUFFERDESC));
		DSB_desc.dwSize = sizeof(DSBUFFERDESC);
		DSB_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

		if (SoundOBJ->CreateSoundBuffer(&DSB_desc,&SoundDSB,NULL)!=DS_OK) return 0;
		SoundDSB -> SetVolume(0);
		SoundDSB -> SetPan(0);
	}
	else SoundCard = FALSE;

	//사운드 초기화
	if(SoundCard)
	{
		Sound[0] = SndObjCreate(SoundOBJ,"sound\\bound.wav",2);
		Sound[1] = SndObjCreate(SoundOBJ,"sound\\fire.wav",2);
		Sound[2] = SndObjCreate(SoundOBJ,"sound\\jump.wav",2);
		Sound[3] = SndObjCreate(SoundOBJ,"sound\\leaf.wav",2);
		Sound[4] = SndObjCreate(SoundOBJ,"sound\\puck.wav",2);
		Sound[5] = SndObjCreate(SoundOBJ,"sound\\sand.wav",2);
		Sound[6] = SndObjCreate(SoundOBJ,"sound\\scream.wav",2);
		Sound[7] = SndObjCreate(SoundOBJ,"sound\\whistle.wav",2);
		Sound[8] = SndObjCreate(SoundOBJ,"sound\\wind.wav",2);
	}

 	JBrush red=jdd->CreateBrush(JColor(255,0,0));

	//초기화
	backbuffer=jdd->GetBackBuffer();
	font=jdd->CreateFont("굴림",20,true,false,false,false,true);
	font2=jdd->CreateFont("HY견고딕",40,true,false,false,false,true);
	int cnt=0;
	for(int i=0; i<PLAYERS; i++)player[i].SetPlayer(0,0);
	prepare_global();
	srand( (unsigned)time( NULL ) );
	
	jdd->SetFrameRate(100,true);
	jdd->SetVerticalSync(false);

	CComputer computer;

	while(gamemode!=QUIT)
	{
		CVideo m_rec;
		bool demoplay, ready, ai_fight;

		//그림 준비
		JPictureInfo pi;
		JColor blue(0,0,255);
		pi.SetColorKey(blue);

		jre->LoadResource("data\\title.mlc");
		do{
			ai_fight=ready=false;
			//타이틀 메뉴
			int selected=Menu("Title",6,210,270,"1 대 1 대결", "태그 매치", "AI 대결", "녹화경기 보기","옵션","나가기","");

			if(selected==5 || selected==-1)	//나가기
			{
				gamemode=QUIT;
				break;
			}
			else if(selected==0)	//1대1
			{
				demoplay=false;
				ready=true;
				hall.players=2;
			}
			else if(selected==1)	//태크매치
			{
				demoplay=false;
				ready=true;
				hall.players=4;
			}
			else if(selected==2)	//AI대결
			{
				demoplay=false;
				ready=true;
				ai_fight=true;
				hall.players=4;
			}
			else if(selected==3)	//녹화경기보기
			{
				int tape=Menu("TitleDark",7,210,200,"테이프1","테이프2","테이프3","테이프4","테이프5","테이프6","테이프7");
				if(tape>=0)
				{
					if(m_rec.LoadFile(StrAdd(StrAdd("Play",tape),".rec")))
					{
						demoplay=true;
						ready=true;
					}
					else
					{
						jdd->DrawText(backbuffer,"테이프가 없습니다.",font,260,160,JColor(255,32,32));
						jdd->Render();
						Sleep(1000);
					}
				}
			}
			else if(selected==4)
			{
				while(1)
				{
					int test_count=0;
					char on_off[20];
					if(sound_on)strcpy(on_off,"효과음 끄기");
						else strcpy(on_off,"효과음 켜기");
					int what=Menu("TitleDark",5,210,200,"1P 키설정","2P 키설정","3P 키설정","4P 키설정",on_off,"","");
					
					if(what==-1)break;
					//음악 조절
					/*else if(what==0)
					{
						while(1)
						{
							if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
							{
								if(!GetMessage(&msg,NULL,0,0)) break;
								TranslateMessage(&msg);
								DispatchMessage(&msg);
							}
							//작게
							if(_GetKeyState(player[0].keyL))
							{
								music_volume-=0.01f;
								if(music_volume<0.0f)music_volume=0.0f;
								//music_info.SetVolume(music_volume);
								//jds->SetSoundInfo("Test",&music_info);
							}							
							//크게
							else if(_GetKeyState(player[0].keyR))
							{
								music_volume+=0.01f;
								if(music_volume>1.0f)music_volume=1.0f;
								//music_info.SetVolume(music_volume);
								//jds->SetSoundInfo("Test",&music_info);
							}
							else if(_GetKeyState(VK_ESCAPE))break;
							
							jdd->DrawPicture(backbuffer,"TitleDark",0,0,NULL);
							jdd->DrawText(backbuffer,StrAdd("음악크기:",int(music_volume*100)),font,260,160,JColor(255,32,32));
							jdd->Render();
						}
						//jds->Stop("Test");
						//jds->DeleteSound("Test");
					}
					//효과음 조절
					else if(what==1)
					{
						while(1)
						{
							if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
							{
								if(!GetMessage(&msg,NULL,0,0)) break;
								TranslateMessage(&msg);
								DispatchMessage(&msg);
							}
							//계속 적으로 발생
							test_count++;
							if(test_count>100)
							{
								test_count=0;
							}

							//작게
							if(_GetKeyState(player[0].keyL))
							{
								effect_volume-=0.01f;
								if(effect_volume<0.0f)effect_volume=0.0f;
								//effect_info.SetVolume(effect_volume);
								//jds->SetSoundInfo("Test",&effect_info);
							}							
							//크게
							else if(_GetKeyState(player[0].keyR))
							{
								effect_volume+=0.01f;
								if(effect_volume>1.0f)effect_volume=1.0f;
								//effect_info.SetVolume(effect_volume);
								//jds->SetSoundInfo("Test",&effect_info);
							}
							else if(_GetKeyState(VK_ESCAPE))break;
							
							jdd->DrawPicture(backbuffer,"TitleDark",0,0,NULL);
							jdd->DrawText(backbuffer,StrAdd("효과음 크기:",int(effect_volume*100)),font,260,160,JColor(255,32,32));
							jdd->Render();
						}
						//jds->Stop("Test");
						//jds->DeleteSound("Test");
					}*/
					else if(what==4)sound_on=(sound_on)?false:true;
					else KeyDefine(what);
				}
			}
		}while(!ready);
		if(gamemode==QUIT)break;
		//그림 삭제
		jre->UnloadResource("data\\title.mlc");

		//경기 준비
		player[0].life=player[1].life=true;
		if(hall.players>2)player[2].life=player[3].life=true;
			else player[2].life=player[3].life=false;

		if(!demoplay)
		{
			SelectPlayer();
			if(ai_fight)
			{
				if(player[2].ai && player[3].ai)hall.players=2;
				for(int i=0; i<PLAYERS; i++)player[i].ai=true;
			}			
		}
		else	//녹화경기 보기
		{
			m_rec.PlayStart();
		}

		gamemode=READYMODE;

		for(int i=0; i<hall.players; i++)
		{
			char tmp_file[40];
			//스프라이트 데이터
			strcpy(tmp_file, StrAdd(player[i].name,"\\"));
			strcat(tmp_file, StrAdd(player[i].name,".spr"));
			if(!(fp=fopen(tmp_file,"rb")))
			{
				printf("%s not found!",StrAdd(player[i].name,".spr"));
				return 0;
			}
			fread(player[i].cx,sizeof(int),14,fp);
			fread(player[i].cy,sizeof(int),7,fp);
			fread(player[i].rects,sizeof(RECT),RECTMAX*2,fp);
			fclose(fp);
			//캐릭터 데이터
			strcpy(tmp_file, StrAdd(player[i].name,"\\"));
			strcat(tmp_file, StrAdd(player[i].name,".chr"));
			if(!(fp=fopen(tmp_file,"rb")))
			{
				printf("%s not found!",StrAdd(player[i].name,".chr"));
				return 0;
			}
			fread(player[i].n_rect,sizeof(RECT),RECTMAX,fp);
			fclose(fp);
		}

		int infox[]={0,540,100,440};					//플레이어들의 얼굴위치
		int stop=-200, win=0;							//시합시작 대기, 승리여부
		prepare_mode0();

		if(!demoplay)m_rec.RecordStart();

		//music_info.SetVolume(music_volume);
		//effect_info.SetVolume(effect_volume);

		//축구장 소리
		if(hall.id==0)
		{
			_MidiPlay("bgm\\soccer.mid");
			bound_sound_id = 3;

		}
		//감옥 소리
		else if(hall.id==1)
		{
			_MidiPlay("bgm\\jail.mid");
			bound_sound_id = 0;
		}
		//농구장 소리
		else if(hall.id==2)
		{
			_MidiPlay("bgm\\slamdunk.mid");
			bound_sound_id = 0;
		}
		//지옥 소리
		else if(hall.id==3)
		{
			_MidiPlay("bgm\\hell.mid");
			bound_sound_id = 2;
		}
		//B-612 소리
		else if(hall.id==4)
		{
			_MidiPlay("bgm\\star.mid");
			bound_sound_id = 5;
		}

		//게임 플레이
		int message=0;
		bool game_over=false;
		while(!game_over)
		{
			if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
			{
				if(!GetMessage(&msg,NULL,0,0)) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if(_GetKeyState(VK_ESCAPE))	//시합 중단
			{
				if(!key_esc)
				{
					_MidiStop();
					key_esc=true;
					game_over=true;
					win=999;
					continue;
				}
			}
			else key_esc=false;
			//////////////////////////////
			//      플레이 모드         
			if(gamemode == PLAYMODE)
			{
				//-----------공 제어-----------//
				for(int i=0; i<BALLMAX; i++)
				{
					if(!ball[i].life)continue;
					ball[i].Move();
					//공과 공의 충돌
					for(int j=0; j<BALLMAX; j++)
						if(ball[j].life && i!=j)
						{
							int direction;
							if(!ball[j].dir)direction=0;
								else direction=4;
							ball[i].Force(&ball[j].brect,direction,1,-1);
						}
				}

				if(demoplay)m_rec.Play();	//데모 플레이
				bool teamlife[]={false,false};	//팀의 생존현황
				//-----------플레이어 제어-----------//
				for(int i=0; i<hall.players; i++)
				{
					if(!player[i].life)continue;
					if(player[i].now==6)//죽었을 경우 제어불능
					{
						player[i].Action();
						continue;
					}
					
					if(!demoplay)
					{
						int j;	//상대
						if((i%2)==0)j=i+1;
							else j=i-1;
						//상대가 죽었을 경우
						if(!player[j].life)
						{
							if(j>=2)j-=2;
								else j+=2;
						}

						if(!player[i].ai)
						{
							player[i].KeyIn();	//사람일 경우 키입력 받기
							computer.learn(i,j);
						}
						else computer.run(i,j);
					}

					//입력받은 동작 처리
					player[i].Action();
					//장거리 공격처리
					if(player[i].attack_on)
					{
						for(int w_id=0; w_id<BALLMAX; w_id++)
							if(!ball[w_id].life)
							{
								if(player[i].id==HWARANG)	//화랑의 화살 공격
								{
									if(!player[i].dir)ball[w_id].SetWeapon(player[i].arect.left+hall.scroll,player[i].arect.top,0,player[i].dir,50,0,i);
										else ball[w_id].SetWeapon(player[i].arect.left+hall.scroll,player[i].arect.top,0,player[i].dir,-50,0,i);
								}
								player[i].attack_on=false;
								break;
							}
					}
			
					teamlife[player[i].team]=true;
					//캐릭터
					int px, py;
					if(!player[i].dir)
						px = player[i].x-hall.scroll-player[i].cx[player[i].now];
					else
						px = player[i].x-hall.scroll-player[i].cx[player[i].now+7];
					py = player[i].y-player[i].cy[player[i].now];
					
					//영역들 설정
					player[i].SetRegion(px, py);
					//공과의 충돌처리
					int direction, direction2;
					if(!player[i].dir)
					{
						direction=0;
						direction2=1;
					}
					else
					{
						direction=3;
						direction2=2;
					}
					for(int j=0; j<BALLMAX; j++)
					{
						if(!ball[j].life)continue;
						//공과 발의 충돌
						int power=1;
						if(player[i].id==LDH && player[i].now==4)power=30;	//이동헌의 슛
						if(player[i].frect.left!=999)ball[j].Force(&player[i].frect,direction,power,i);
						//공과 몸의 충돌
						if(!ball[j].weapon)
						{
							if(ball[j].master!=i && collision(&ball[j].brect, &player[i].brect))
							{
								player[i].Hurt(ball[j].heavy);
								ball[j].Force(&player[i].brect,direction2,1,i);
							}
						}
						else
						{
							if(ball[j].master!=i && collision(&ball[j].brect, &player[i].brect))
							{
								player[i].Hurt(ball[j].heavy*10);
								ball[j].life=false;
							}							
						}

						if(player[i].arect.left==999)continue;	//공격 상태가 아닐 땐 넘어감
						//공과 공격범위의 충돌
						if(collision(&ball[j].brect, &player[i].arect))
							ball[j].Force(&player[i].arect,direction,10,i);
						//공격범위와 몸의 충돌
						for(int k=0; k<hall.players; k++)
						{
							if(player[k].team==player[i].team || !player[k].life || player[k].now>=5)continue;
							if(collision(&player[k].brect, &player[i].arect))
							{
								if(player[i].id != SKULL)
								{
									if(!player[i].dir)player[k].dir=DPX_HFLIP;
										else player[k].dir=0;
									player[k].Hurt(player[i].power*10);
								}
								else	//스쿨크스 경우 반대쪽으로 쳐냄
								{
									if(!player[i].dir)player[k].dir=0;
										else player[k].dir=DPX_HFLIP;
									player[k].Hurt(player[i].power*10);
								}
							}
						}
					}
				}
				if(!demoplay)m_rec.Record();		//패턴 기록

				//속도 업
				if(_GetKeyState(VK_ADD) && frameskip<20)
				{
					if(!keydown[0])
					{
						frameskip++;
						message=50;
					}
					keydown[0]=true;
				}
				else keydown[0]=false;
				//속도 다운
				if(_GetKeyState(VK_SUBTRACT) && frameskip>0)
				{
					if(!keydown[1])
					{
						frameskip--;
						message=50;
					}
					keydown[1]=true;
				}
				else keydown[1]=false;
				//시간 감소
				if(cnt>100)
				{
					hall.Event();
					cnt=0;
				}
				//시간 종료
				if(hall.time==0)
				{
					if(hall.score[0]>hall.score[1])win=1;
						else if(hall.score[0]>hall.score[1])win=2;
						else win=0;
					stop=400;
					gamemode=READYMODE;
				}
				//팀 전원 사망
				else
				{
					for(int i=0; i<2; i++)
					{
						if(!teamlife[i])
						{
							win+=int(pow((double)2,(double)i));
							stop=400;
							gamemode=READYMODE;
						}
					}
					win=3-win;
				}
			}
			///////////////////////////////////////////////////////
			//출력 부분, 함수로 만들기 곤란해서 그냥 처리 했수다.
			if(gamemode == PLAYMODE || gamemode == READYMODE)
			{
				//화면 맞추기
				hall.AutoScroll(ball[0].x);//player[0].x);
				cnt++;
				//------------출력-----------------//
				if(cnt%(frameskip+1)==0)
				{
					//배경
					jdd->DrawPicture(backbuffer,"Back",0,0,&hall.srect);
					//공
					for(int i=0; i<BALLMAX; i++)
					{
						if(!ball[i].life)continue;
						int x=ball[i].x-hall.scroll;
						if(!ball[i].weapon)jdd->DrawPictureEx(backbuffer,StrAdd("Ball",ball[i].type*10+ball[i].ani_now),x,ball[i].y,NULL,ball[i].dir);
							else jdd->DrawPictureEx(backbuffer,StrAdd("Weapon",ball[i].type*10+ball[i].ani_now),x,ball[i].y,NULL,ball[i].dir);
					}
					//캐릭터
					for(int i=0; i<hall.players; i++)
					{
						if(!player[i].life)continue;

						int px, py;
						if(!player[i].dir)
							px = player[i].x-hall.scroll-player[i].cx[player[i].now];
						else
							px = player[i].x-hall.scroll-player[i].cx[player[i].now+7];
						py = player[i].y-player[i].cy[player[i].now];

						jdd->DrawPictureEx(backbuffer,surfname[i][player[i].now],px, py,NULL,player[i].dir);
						if(player[i].now==6)continue;
						//OP인지 표시
						int signx = player[i].x-hall.scroll;
						if(signx < -30)
							jdd->DrawPicture(backbuffer,StrAdd(i+1,"P"),0, i*30+200,NULL);
						else if(signx > 670)
							jdd->DrawPicture(backbuffer,StrAdd(i+1,"P"),580, i*30+200,NULL);
						else
							jdd->DrawPicture(backbuffer,StrAdd(i+1,"P"),player[i].x-hall.scroll-30, py-25,NULL);
						
						//얼굴과 체력
						int hp_ceil = ((player[i].hp%10)==0)?player[i].hp/10:player[i].hp/10+1;
						jdd->DrawPicture(backbuffer,surfname[i][7], infox[i], 0,NULL);
						jdd->DrawText(backbuffer,StrAdd(hp_ceil,"%"),font,infox[i]+42,62,JColor(0,0,0));
						jdd->DrawText(backbuffer,StrAdd(hp_ceil,"%"),font,infox[i]+40,60,JColor(255,255,255));
					}

					//시간
					if(hall.time>=0 && hall.time<10000)
					{
						int time10, time1;
						time10 = hall.time/10;
						time1 = hall.time%10;
						jdd->DrawPicture(backbuffer,StrAdd(time10,""), 240, 10,NULL);
						jdd->DrawPicture(backbuffer,StrAdd(time1,""), 320, 10,NULL);
					}
					//점수
					jdd->DrawText(backbuffer,StrAdd(hall.score[0],"점"),font2,72,82,JColor(0,0,0));
					jdd->DrawText(backbuffer,StrAdd(hall.score[0],"점"),font2,70,80,JColor(255,255,255));
					jdd->DrawText(backbuffer,StrAdd(hall.score[1],"점"),font2,542,82,JColor(0,0,0));
					jdd->DrawText(backbuffer,StrAdd(hall.score[1],"점"),font2,540,80,JColor(255,255,255));
					//메세지
					if(message>0)
					{
						jdd->DrawText(backbuffer,StrAdd("Frame skip:",frameskip),font,10,460,JColor(255,255,255));
						message--;
					}
				}
			}
			//////////////////////////////
			//      대기 모드         
			if(gamemode == READYMODE)
			{
				//시합 종료, 시작
				if(stop > 0)
				{
					if(stop==400)
					{
						_Play(7);
						_MidiStop();
					}
					jdd->DrawPicture(backbuffer,"End",114,161,NULL);
					stop--;
					if(stop==0)game_over=true;
				}
				else
				{
					if(stop==-200)
					{
						_Play(7);
					}

					jdd->DrawPicture(backbuffer,"Start",126,171,NULL);
					stop++;
					if(stop==0)
					{
						gamemode=PLAYMODE;
					}
				}
			}
			jdd->Render();
		}
		//음악 삭제
		//jds->DeleteSound("Puck");
		//jds->DeleteSound("Bound");
		//jds->DeleteSound("Whistle");
		//jds->DeleteSound("Fire");
		//jds->DeleteSound("Wind");
		//jds->DeleteSound("Scream");

		//게임 중에만 쓰는 서페이스들 삭제
		jre->UnloadResource(StrAdd("data\\",hall.filename));
		for(int i=0; i<hall.players; i++)
			for(int j=0; j<8; j++)
				jdd->DeleteSurface(surfname[i][j]);
		//경기녹화
		if(win!=999 && !demoplay)
		{
			jdd->LoadPicture("Record","data\\record.jpg",&pi,true);
			int tape=Menu("Record",7,360,200,"테이프1","테이프2","테이프3","테이프4","테이프5","테이프6","테이프7");
			if(tape>=0)m_rec.RecordFile(StrAdd(StrAdd("Play",tape),".rec"));
			jdd->DeleteSurface("Record");
		}
	}
	//게임 끝, 정리
	jdd->DeleteFont(font);
	jdd->DeleteFont(font2);
	delete jdd;

	_CrtDumpMemoryLeaks();

	//옵션 정보를 저장
	if(fp=fopen("setup.sav","wb"))
	{
		for(int i=0; i<4; i++)fwrite(&player[i].keyL,sizeof(int),5,fp);
		fwrite(&music_volume,sizeof(double),1,fp);
		fwrite(&effect_volume,sizeof(double),1,fp);
		fwrite(&sound_on,sizeof(bool),1,fp);
		fclose(fp);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch ( msg )
    {
		case WM_CHAR		:	break;

		case MM_MCINOTIFY    :  if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL ) _MidiReplay();
								break;

		case WM_MOVE		 :	if(jdd)jdd->OnMove(LOWORD(lParam), HIWORD(lParam));
								break;
		
		case WM_SIZE		 :	if(wParam == SIZE_MINIMIZED)activate=false;
								else activate=true;
								break;
		
		case WM_ACTIVATE	 : if(LOWORD(wParam))activate=true;
								else activate=false;
							   break;

		case WM_SYSCOMMAND	 :  //닫기 메시지 가로채기
								if(wParam==SC_CLOSE)
								{
									wParam=0;
									exit(0);
								}
								break;

	}

	return DefWindowProc(wnd,msg,wParam,lParam);
}