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

//���� ���
#define PLAYMODE	0
#define READYMODE	1
#define WINMODE		2
#define SELECTMODE	3
#define DEMOMODE	4
#define QUIT		99

int gamemode;		//���� ���
char surfname[PLAYERS][8][15];	//�����̽� �̸���

char* StrAdd(char* s1, char* s2)		//����+����
{
	strcpy(global_buffer,s1);
	strcat(global_buffer,s2);

	return global_buffer;
}

char* StrAdd(int n1, char* s2)			//����+����
{
	char s1[10];
	itoa(n1,s1,10);
	strcpy(global_buffer,s1);
	strcat(global_buffer,s2);

	return global_buffer;
}

char* StrAdd(char* s2, int n1)			//����+����
{
	char s1[10];
	itoa(n1,s1,10);
	strcpy(global_buffer,s2);
	strcat(global_buffer,s1);

	return global_buffer;
}

void prepare_global()	//��ü������ ���� �� �غ�
{
	jre->LoadResource("data\\system.mlc");
	jre->LoadResource("data\\ball.mlc");

	//�ɼ� �о����
	if(fp=fopen("setup.sav","rb"))
	{
		for(int i=0; i<4; i++)fread(&player[i].keyL,sizeof(int),5,fp);
		fread(&music_volume,sizeof(double),1,fp);
		fread(&effect_volume,sizeof(double),1,fp);
		fread(&sound_on,sizeof(bool),1,fp);
		fclose(fp);
	}
}

void prepare_mode0()	//���ӿ� �ʿ��� �͵� �غ�
{
	JPictureInfo pi, pi2;
	JColor blue(0,0,255);
	pi.SetColorKey(blue);
	pi2.SetColorKey(blue);
	pi2.SetOpacity(0.5f);
	jre->LoadResource(StrAdd("data\\",hall.filename));

	//ĳ���Ϳ� �ش��ϴ� �������� �׸� ��������
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

	//�޴�
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
		//�� �Ʒ� �̵�
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

		//Ÿ��Ʋ�� �޴�ǥ��
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
//����, ��, ����� ����
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
		//��������
		SetRect(&next_r,544,458,632,476);
		//���� ���� ����
		SetRect(&chr_r[0],3,26,127,143);
		SetRect(&chr_r[1],2,146,127,248);
		SetRect(&chr_r[2],3,252,152,347);
		SetRect(&chr_r[3],3,351,236,463);
		SetRect(&chr_r[4],131,25,250,141);
		SetRect(&chr_r[5],132,148,254,247);
		SetRect(&chr_r[6],156,251,256,346);
		//����� ���� ����
		SetRect(&grd_r[0],435,22,614,81);
		SetRect(&grd_r[1],409,153,495,212);
		SetRect(&grd_r[2],409,87,633,146);
		SetRect(&grd_r[3],517,153,633,212);
		SetRect(&grd_r[4],402,219,633,278);
		
		if(select_ground>=0)jdd->DrawPicture(backbuffer,StrAdd("Ball",select_ground*10),grd_r[select_ground].left,grd_r[select_ground].top,NULL);
		for(int i=0; i<hall.players; i++)
		{
			//����
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
			//���� �Ǵ� ����� ����
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
	//������ ���� �� �ߴٸ� �ڵ� ����
	if(select_ground==-1)select_ground=rand()%ALL_GROUND;
	for(int i=0; i<hall.players; i++)
		if(select_pi[i]==-1)select_pi[i]=rand()%ALL_PLAYER;

	//������ �÷��̾� �ʱ�ȭ
	hall.SetHall(select_ground);
	for(int i=0; i<hall.players; i++)player[i].SetPlayer(select_pi[i],i%2);
}

//////////////////////////////////////////////////////
//Ű ����
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

	//Ÿ��Ʋ�� �޴�ǥ��
	jdd->DrawPicture(backbuffer,"TitleDark",0,0,NULL);
	                          
	jdd->DrawText(backbuffer,"��Ű�� �Է��ϼ���.",font,260,160,JColor(255,32,32));
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

	jdd->DrawText(backbuffer,"��Ű�� �Է��ϼ���.",font,260,180,JColor(255,32,32));
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

	jdd->DrawText(backbuffer,"��Ű�� �Է��ϼ���.",font,260,200,JColor(255,32,32));
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

	jdd->DrawText(backbuffer,"��Ű�� �Է��ϼ���.",font,260,220,JColor(255,32,32));
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

	jdd->DrawText(backbuffer,"����Ű�� �Է��ϼ���.",font,260,240,JColor(255,32,32));
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

		hwnd = CreateWindow("Demo","����Ŀ", ws, 100, 100, crt.right - crt.left, crt.bottom - crt.top, NULL, NULL, hInstance, NULL);
		ShowCursor( TRUE );
	}
	else
	{
		hwnd=CreateWindow("Demo","����Ŀ",WS_POPUP|WS_VISIBLE,0,0,640,480,NULL,NULL,hInstance,NULL);
		ShowCursor( FALSE );
	}

	jdd->Initialize(NULL/*jdd->GetDeviceCount()-1*/,hwnd,640,480,16,true,true);
	
	
	//������â �̵�
	if(window_mode)
	{
		jdd->OnMove(100, 100);
		SetCursor(LoadCursor(0, IDC_ARROW));
	}

	//���� �ʱ�ȭ
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

	//���� �ʱ�ȭ
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

	//�ʱ�ȭ
	backbuffer=jdd->GetBackBuffer();
	font=jdd->CreateFont("����",20,true,false,false,false,true);
	font2=jdd->CreateFont("HY�߰��",40,true,false,false,false,true);
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

		//�׸� �غ�
		JPictureInfo pi;
		JColor blue(0,0,255);
		pi.SetColorKey(blue);

		jre->LoadResource("data\\title.mlc");
		do{
			ai_fight=ready=false;
			//Ÿ��Ʋ �޴�
			int selected=Menu("Title",6,210,270,"1 �� 1 ���", "�±� ��ġ", "AI ���", "��ȭ��� ����","�ɼ�","������","");

			if(selected==5 || selected==-1)	//������
			{
				gamemode=QUIT;
				break;
			}
			else if(selected==0)	//1��1
			{
				demoplay=false;
				ready=true;
				hall.players=2;
			}
			else if(selected==1)	//��ũ��ġ
			{
				demoplay=false;
				ready=true;
				hall.players=4;
			}
			else if(selected==2)	//AI���
			{
				demoplay=false;
				ready=true;
				ai_fight=true;
				hall.players=4;
			}
			else if(selected==3)	//��ȭ��⺸��
			{
				int tape=Menu("TitleDark",7,210,200,"������1","������2","������3","������4","������5","������6","������7");
				if(tape>=0)
				{
					if(m_rec.LoadFile(StrAdd(StrAdd("Play",tape),".rec")))
					{
						demoplay=true;
						ready=true;
					}
					else
					{
						jdd->DrawText(backbuffer,"�������� �����ϴ�.",font,260,160,JColor(255,32,32));
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
					if(sound_on)strcpy(on_off,"ȿ���� ����");
						else strcpy(on_off,"ȿ���� �ѱ�");
					int what=Menu("TitleDark",5,210,200,"1P Ű����","2P Ű����","3P Ű����","4P Ű����",on_off,"","");
					
					if(what==-1)break;
					//���� ����
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
							//�۰�
							if(_GetKeyState(player[0].keyL))
							{
								music_volume-=0.01f;
								if(music_volume<0.0f)music_volume=0.0f;
								//music_info.SetVolume(music_volume);
								//jds->SetSoundInfo("Test",&music_info);
							}							
							//ũ��
							else if(_GetKeyState(player[0].keyR))
							{
								music_volume+=0.01f;
								if(music_volume>1.0f)music_volume=1.0f;
								//music_info.SetVolume(music_volume);
								//jds->SetSoundInfo("Test",&music_info);
							}
							else if(_GetKeyState(VK_ESCAPE))break;
							
							jdd->DrawPicture(backbuffer,"TitleDark",0,0,NULL);
							jdd->DrawText(backbuffer,StrAdd("����ũ��:",int(music_volume*100)),font,260,160,JColor(255,32,32));
							jdd->Render();
						}
						//jds->Stop("Test");
						//jds->DeleteSound("Test");
					}
					//ȿ���� ����
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
							//��� ������ �߻�
							test_count++;
							if(test_count>100)
							{
								test_count=0;
							}

							//�۰�
							if(_GetKeyState(player[0].keyL))
							{
								effect_volume-=0.01f;
								if(effect_volume<0.0f)effect_volume=0.0f;
								//effect_info.SetVolume(effect_volume);
								//jds->SetSoundInfo("Test",&effect_info);
							}							
							//ũ��
							else if(_GetKeyState(player[0].keyR))
							{
								effect_volume+=0.01f;
								if(effect_volume>1.0f)effect_volume=1.0f;
								//effect_info.SetVolume(effect_volume);
								//jds->SetSoundInfo("Test",&effect_info);
							}
							else if(_GetKeyState(VK_ESCAPE))break;
							
							jdd->DrawPicture(backbuffer,"TitleDark",0,0,NULL);
							jdd->DrawText(backbuffer,StrAdd("ȿ���� ũ��:",int(effect_volume*100)),font,260,160,JColor(255,32,32));
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
		//�׸� ����
		jre->UnloadResource("data\\title.mlc");

		//��� �غ�
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
		else	//��ȭ��� ����
		{
			m_rec.PlayStart();
		}

		gamemode=READYMODE;

		for(int i=0; i<hall.players; i++)
		{
			char tmp_file[40];
			//��������Ʈ ������
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
			//ĳ���� ������
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

		int infox[]={0,540,100,440};					//�÷��̾���� ����ġ
		int stop=-200, win=0;							//���ս��� ���, �¸�����
		prepare_mode0();

		if(!demoplay)m_rec.RecordStart();

		//music_info.SetVolume(music_volume);
		//effect_info.SetVolume(effect_volume);

		//�౸�� �Ҹ�
		if(hall.id==0)
		{
			_MidiPlay("bgm\\soccer.mid");
			bound_sound_id = 3;

		}
		//���� �Ҹ�
		else if(hall.id==1)
		{
			_MidiPlay("bgm\\jail.mid");
			bound_sound_id = 0;
		}
		//���� �Ҹ�
		else if(hall.id==2)
		{
			_MidiPlay("bgm\\slamdunk.mid");
			bound_sound_id = 0;
		}
		//���� �Ҹ�
		else if(hall.id==3)
		{
			_MidiPlay("bgm\\hell.mid");
			bound_sound_id = 2;
		}
		//B-612 �Ҹ�
		else if(hall.id==4)
		{
			_MidiPlay("bgm\\star.mid");
			bound_sound_id = 5;
		}

		//���� �÷���
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
			if(_GetKeyState(VK_ESCAPE))	//���� �ߴ�
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
			//      �÷��� ���         
			if(gamemode == PLAYMODE)
			{
				//-----------�� ����-----------//
				for(int i=0; i<BALLMAX; i++)
				{
					if(!ball[i].life)continue;
					ball[i].Move();
					//���� ���� �浹
					for(int j=0; j<BALLMAX; j++)
						if(ball[j].life && i!=j)
						{
							int direction;
							if(!ball[j].dir)direction=0;
								else direction=4;
							ball[i].Force(&ball[j].brect,direction,1,-1);
						}
				}

				if(demoplay)m_rec.Play();	//���� �÷���
				bool teamlife[]={false,false};	//���� ������Ȳ
				//-----------�÷��̾� ����-----------//
				for(int i=0; i<hall.players; i++)
				{
					if(!player[i].life)continue;
					if(player[i].now==6)//�׾��� ��� ����Ҵ�
					{
						player[i].Action();
						continue;
					}
					
					if(!demoplay)
					{
						int j;	//���
						if((i%2)==0)j=i+1;
							else j=i-1;
						//��밡 �׾��� ���
						if(!player[j].life)
						{
							if(j>=2)j-=2;
								else j+=2;
						}

						if(!player[i].ai)
						{
							player[i].KeyIn();	//����� ��� Ű�Է� �ޱ�
							computer.learn(i,j);
						}
						else computer.run(i,j);
					}

					//�Է¹��� ���� ó��
					player[i].Action();
					//��Ÿ� ����ó��
					if(player[i].attack_on)
					{
						for(int w_id=0; w_id<BALLMAX; w_id++)
							if(!ball[w_id].life)
							{
								if(player[i].id==HWARANG)	//ȭ���� ȭ�� ����
								{
									if(!player[i].dir)ball[w_id].SetWeapon(player[i].arect.left+hall.scroll,player[i].arect.top,0,player[i].dir,50,0,i);
										else ball[w_id].SetWeapon(player[i].arect.left+hall.scroll,player[i].arect.top,0,player[i].dir,-50,0,i);
								}
								player[i].attack_on=false;
								break;
							}
					}
			
					teamlife[player[i].team]=true;
					//ĳ����
					int px, py;
					if(!player[i].dir)
						px = player[i].x-hall.scroll-player[i].cx[player[i].now];
					else
						px = player[i].x-hall.scroll-player[i].cx[player[i].now+7];
					py = player[i].y-player[i].cy[player[i].now];
					
					//������ ����
					player[i].SetRegion(px, py);
					//������ �浹ó��
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
						//���� ���� �浹
						int power=1;
						if(player[i].id==LDH && player[i].now==4)power=30;	//�̵����� ��
						if(player[i].frect.left!=999)ball[j].Force(&player[i].frect,direction,power,i);
						//���� ���� �浹
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

						if(player[i].arect.left==999)continue;	//���� ���°� �ƴ� �� �Ѿ
						//���� ���ݹ����� �浹
						if(collision(&ball[j].brect, &player[i].arect))
							ball[j].Force(&player[i].arect,direction,10,i);
						//���ݹ����� ���� �浹
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
								else	//����ũ�� ��� �ݴ������� �ĳ�
								{
									if(!player[i].dir)player[k].dir=0;
										else player[k].dir=DPX_HFLIP;
									player[k].Hurt(player[i].power*10);
								}
							}
						}
					}
				}
				if(!demoplay)m_rec.Record();		//���� ���

				//�ӵ� ��
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
				//�ӵ� �ٿ�
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
				//�ð� ����
				if(cnt>100)
				{
					hall.Event();
					cnt=0;
				}
				//�ð� ����
				if(hall.time==0)
				{
					if(hall.score[0]>hall.score[1])win=1;
						else if(hall.score[0]>hall.score[1])win=2;
						else win=0;
					stop=400;
					gamemode=READYMODE;
				}
				//�� ���� ���
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
			//��� �κ�, �Լ��� ����� ����ؼ� �׳� ó�� �߼���.
			if(gamemode == PLAYMODE || gamemode == READYMODE)
			{
				//ȭ�� ���߱�
				hall.AutoScroll(ball[0].x);//player[0].x);
				cnt++;
				//------------���-----------------//
				if(cnt%(frameskip+1)==0)
				{
					//���
					jdd->DrawPicture(backbuffer,"Back",0,0,&hall.srect);
					//��
					for(int i=0; i<BALLMAX; i++)
					{
						if(!ball[i].life)continue;
						int x=ball[i].x-hall.scroll;
						if(!ball[i].weapon)jdd->DrawPictureEx(backbuffer,StrAdd("Ball",ball[i].type*10+ball[i].ani_now),x,ball[i].y,NULL,ball[i].dir);
							else jdd->DrawPictureEx(backbuffer,StrAdd("Weapon",ball[i].type*10+ball[i].ani_now),x,ball[i].y,NULL,ball[i].dir);
					}
					//ĳ����
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
						//OP���� ǥ��
						int signx = player[i].x-hall.scroll;
						if(signx < -30)
							jdd->DrawPicture(backbuffer,StrAdd(i+1,"P"),0, i*30+200,NULL);
						else if(signx > 670)
							jdd->DrawPicture(backbuffer,StrAdd(i+1,"P"),580, i*30+200,NULL);
						else
							jdd->DrawPicture(backbuffer,StrAdd(i+1,"P"),player[i].x-hall.scroll-30, py-25,NULL);
						
						//�󱼰� ü��
						int hp_ceil = ((player[i].hp%10)==0)?player[i].hp/10:player[i].hp/10+1;
						jdd->DrawPicture(backbuffer,surfname[i][7], infox[i], 0,NULL);
						jdd->DrawText(backbuffer,StrAdd(hp_ceil,"%"),font,infox[i]+42,62,JColor(0,0,0));
						jdd->DrawText(backbuffer,StrAdd(hp_ceil,"%"),font,infox[i]+40,60,JColor(255,255,255));
					}

					//�ð�
					if(hall.time>=0 && hall.time<10000)
					{
						int time10, time1;
						time10 = hall.time/10;
						time1 = hall.time%10;
						jdd->DrawPicture(backbuffer,StrAdd(time10,""), 240, 10,NULL);
						jdd->DrawPicture(backbuffer,StrAdd(time1,""), 320, 10,NULL);
					}
					//����
					jdd->DrawText(backbuffer,StrAdd(hall.score[0],"��"),font2,72,82,JColor(0,0,0));
					jdd->DrawText(backbuffer,StrAdd(hall.score[0],"��"),font2,70,80,JColor(255,255,255));
					jdd->DrawText(backbuffer,StrAdd(hall.score[1],"��"),font2,542,82,JColor(0,0,0));
					jdd->DrawText(backbuffer,StrAdd(hall.score[1],"��"),font2,540,80,JColor(255,255,255));
					//�޼���
					if(message>0)
					{
						jdd->DrawText(backbuffer,StrAdd("Frame skip:",frameskip),font,10,460,JColor(255,255,255));
						message--;
					}
				}
			}
			//////////////////////////////
			//      ��� ���         
			if(gamemode == READYMODE)
			{
				//���� ����, ����
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
		//���� ����
		//jds->DeleteSound("Puck");
		//jds->DeleteSound("Bound");
		//jds->DeleteSound("Whistle");
		//jds->DeleteSound("Fire");
		//jds->DeleteSound("Wind");
		//jds->DeleteSound("Scream");

		//���� �߿��� ���� �����̽��� ����
		jre->UnloadResource(StrAdd("data\\",hall.filename));
		for(int i=0; i<hall.players; i++)
			for(int j=0; j<8; j++)
				jdd->DeleteSurface(surfname[i][j]);
		//����ȭ
		if(win!=999 && !demoplay)
		{
			jdd->LoadPicture("Record","data\\record.jpg",&pi,true);
			int tape=Menu("Record",7,360,200,"������1","������2","������3","������4","������5","������6","������7");
			if(tape>=0)m_rec.RecordFile(StrAdd(StrAdd("Play",tape),".rec"));
			jdd->DeleteSurface("Record");
		}
	}
	//���� ��, ����
	jdd->DeleteFont(font);
	jdd->DeleteFont(font2);
	delete jdd;

	_CrtDumpMemoryLeaks();

	//�ɼ� ������ ����
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

		case WM_SYSCOMMAND	 :  //�ݱ� �޽��� ����ä��
								if(wParam==SC_CLOSE)
								{
									wParam=0;
									exit(0);
								}
								break;

	}

	return DefWindowProc(wnd,msg,wParam,lParam);
}