#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <dsound.h>
#include <stdio.h>
#include <time.h>
#include <dmusicc.h>
#include <tchar.h>

#include "HungLib.h"
#include "useful.h"
#include "data.h"

#define IMG(x)		gm.GetImageID(x)

BOOL active;
int MouseX, MouseY;
BOOL LeftButton=FALSE, RightButton=FALSE;	//���콺 Ŭ�� ����

//�ִϸ��̼� Ŭ����
CAnimation ani_tile(40, 40);
CAnimation ani_icon(10, 10);
CAnimation ani_number(5, 6);
CAnimation ani_yesno(50, 50);
CAnimation ani_select(40, 40, 2, 3, 5);
CAnimation ani_fire(40, 40, 2, 3, 2);
CAnimation ani_heel(40, 40, 2, 5, 1);
CAnimation ani_turn(100, 50, 2, 3, 5);

//����
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	//�ʱ�ȭ
	CHungLib gm(hwnd, hInstance, nCmdShow, "Hyugaru", "�ް��� ����", 640, 480, false);

	srand((unsigned int)time(NULL));

	//�̹���, �Ҹ� �ҷ�����
	gm.RestoreAll();
	gm.LoadSound();

	int d_tile=IMG("����D");
	int r_tile=IMG("����R");

	//ĳ���� �ʱ�ȭ				 �̸�        HP  MV  MZ  FL ������ �ൿ��
	cara[HERO1].SetCaracter		("�ް���",   10,  6,  0,  0,  0,  1,  5);
	cara[HERO2].SetCaracter		("���̾�",   10,  6,  1,  0,  0,  1,  5);
	cara[HERO3].SetCaracter		("��ũ��",   10,  6,  2,  0,  0,  1,  5);
	cara[HERO4].SetCaracter		("�级��",   10,  6,  3,  0,  0,  1,  5);
	cara[HERO5].SetCaracter		("�̹���",   10,  6,  4,  0,  0,  1,  5);
	cara[HERO6].SetCaracter		("�޷��",   10,  6,  5,  0,  0,  1,  5);
	cara[SOLDIER1].SetCaracter	("�뺴",     10,  6, -1,  0,  0,  5);
	cara[SOLDIER2].SetCaracter	("����",     10,  7, -1,  0,  2,  3,  5);
	cara[SOLDIER3].SetCaracter	("������",   10,  5, -1,  0,  4,  5);
	cara[BANDIT].SetCaracter	("����",     10,  7, -1,  0,  0,  5);
	cara[BANG].SetCaracter		("�氥",      5,  6, -1,  0,  0,  5);
	cara[BIGROCK].SetCaracter	("ū����",   20,  5, -1,  0,  0,  5);
	cara[HANAZUM].SetCaracter	("�ϳ���",   10,  5, -1, -1,  0,  5);
	cara[KUOT].SetCaracter		("�ٿ�",     10,  4, -1,  0,  0,  5);
	cara[MAEQO].SetCaracter		("����",     10,  5, -1,  0,  0,  5);
	cara[DARKQUIZ].SetCaracter	("��ũ����", 10,  8,  6,  0,  0,  1,  5);
	cara[NEYUM].SetCaracter		("�׿�",	  0,  4, -1, -2,  0,  5);

	//��� �ʱ�ȭ(   �̸�			���� �� Ÿ��  ����  )
	equipment[0].Set ("����",		 0,   0,  0, EQ_ARMS);
	equipment[1].Set ("�ܰ�",		 1,  50,  1, EQ_ARMS);
	equipment[2].Set ("ö��",		 2, 100,  1, EQ_ARMS);
	equipment[3].Set ("���",		 1, 125,  1, EQ_ARMS);
	equipment[4].Set ("ȸ����",		 1, 150,  1, EQ_ARMS);
	equipment[5].Set ("â",			 1,  75,  2, EQ_ARMS);
	equipment[6].Set ("�� â",		 1, 125,  2, EQ_ARMS);
	equipment[7].Set ("Ȱ",			 1, 100,  3, EQ_ARMS);
	equipment[8].Set ("��ȭ Ȱ",	 2, 200,  3, EQ_ARMS);
	equipment[9].Set ("�� Ȱ",		 1, 250,  3, EQ_ARMS);
	equipment[10].Set("������",		 0,  50,  6, EQ_ARMS);
	equipment[11].Set("������",		 1, 100,  5, EQ_ARMS);
	equipment[12].Set("������",		 0, 100, 10, EQ_ARMS);
	equipment[13].Set("���� ����",	 5,  50,  0, EQ_AMMOR);
	equipment[14].Set("ö ����",	10, 110,  0, EQ_AMMOR);
	equipment[15].Set("��ö ����",	20, 300,  0, EQ_AMMOR);
	equipment[16].Set("���� ����",	99, 999,  0, EQ_AMMOR);

	//���� Ŭ�������� ����� �׸��� ID
	imgno.panel=IMG("���â");
	imgno.state_icon=IMG("���� ������");
	imgno.number=IMG("����");
	imgno.unit=IMG("����1");
	imgno.animation=IMG("��");
	imgno.cursor=IMG("Ŀ��");

	CNumber n_fps(&gm, IMG("����"), 5);

	//���� �ʱ�ȭ
	CGame main(&gm);
	bool game_end=false;
	main_menu=true;
	main.SetStartPoint(13, 0, 0);
	main.Init();

	//���� �ʱ�ȭ
	InitDirectMusic();
	
	//Ŭ���� Ƚ�� �˾Ƴ���
	bool ending=false;
	int clear_code[]={0x2c5f138a, 0xd70ebb04, 0xaa6b5ce9};
	FILE* fp;
	if(fp=fopen("game.sav", "rb"))
	{
		int code;
		fread(&code, sizeof(int), 1, fp);
		for(int i=0; i<3; i++)
			if(code==clear_code[i])
			{
				clear_count=i+1;
				break;
			}
		fclose(fp);
	}

	while(!game_end)
	{
		//����
		if(ending)
		{
			LoadSegmentFile("end.mid");
			PlayAudio();
			int scroll_time=0, count=0;
			while(scroll_time<1100 && !gm.RightClick())
			{
				if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
				{
					if(!GetMessage(&msg, NULL, 0, 0))return msg.wParam;

					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					//��ũ��
					if(++count>2)
					{
						scroll_time++;
						count=0;
					}
					//���� ����
					for(int j=0; j<12; j++)
						for(int i=0; i<16; i++)
							lib->Image(i*40, j*40, 0, 7);
					//���ܷ�
					if(clear_count==0)
					{
						char* steproll[]={
							"����: ȥ���� ���",
							"����: �˶˹�",
							"����: �˶˹�",
							"�����~ ��Ȧ�� ��~",
							"Ư�� �̹� ������ ���̺귯������ ���� ������ٱ�.",
							"�׷��� ���ϵ� DGF��� ���� ���� ���� �����̶�.",
							"�׷����� JPG�� GIF�� ����� �� ������ �����ɹۿ� ����.",
							"�����̷��� ���� �͵� ������ ġƮŰ ������ �ٲ�.",
							"F, O, G   : �Ȱ� ���ֱ�",
							"H, E, R, O: ���ΰ� ���� �����",
							"E, N, D   : ��簡 �� �� �׳� ������",
							"N, O, E   : ������������ �� ���� �� �ϰ� �ϱ�",
							"�ΰ� �� ������ �ϳ��� ������, �ϳ��� �ʿ� ������ ����",
							"ó�� �� �����̴ϱ� �� �� ���غ�.",
							"�Ƹ� ��մ°� �߰��Ǿ� ��������... ũũũ..."
						};
						for(int i=0; i<15; i++)
							gm.PutText(100, 480+(i*40)-scroll_time, steproll[i], RGB(255,255,255));
					}
					//�ι�° ����
					else if(clear_count==1)
					{
						char* steproll[]={
							"����: ȥ���� ���",
							"����: �˶˹�",
							"����: �˶˹�",
							"�����~ ��Ȧ�� ��~",
							"�ι��̳� ������ ����ϱ���.",
							"���~ ���~",
							"��¼�� ������� �������� ������ �ϴ� ���.",
							"�׷� ������ ������ �ֱ� �����Ҵ��� ����������.",
							"S, O : ġƮ �Ҵ� �����",
							"ġƮ �˾Ƴ����� ���� ������ �� �������̾���.",
							"S, T, G + ? : �������� �̵�",
							"0~C���� ���� ������ �ߵ�����.",
							"0�� �����, �ƹ�ư ���ڳ� ����� ���� ��ȣ��.",
							"�� �� �� ���غ��� ������.",
							"�� ��մ°� �߰��Ǿ� ������... ũũũ..."
						};
						for(int i=0; i<15; i++)
							gm.PutText(100, 480+(i*40)-scroll_time, steproll[i], RGB(255,255,255));
					}
					//����° ����
					else if(clear_count==2)
					{
						char* steproll[]={
							"����: ȥ���� ���",
							"����: �˶˹�",
							"����: �˶˹�",
							"�����~ ��Ȧ�� ��~",
							"�� ���� 3��°�α���.",
							"�׳����� �ڳ׵� ¡�ϱ���",
							"���� ��̰� �־� �̷��� ����ϴ� �հ�?",
							"�ƴϸ� Ȥ�� ���޾������� ����.",
							"���� ���� �ٷ� ���� ���̺긦 �̿��ϸ� �Ǵ� ��.",
							"�׷�, �װ� �̿��ϸ� Ŭ���� Ƚ���� ȮȮ �ø���.",
							"�����ٸ� �ڳ� ���� ����� �ɼ�.",
							"������~",
							"���� �� ���� ����.",
							"�� �̻� ������ ���� ������.",
							"�׷� �̸�... ��������~"
						};
						for(int i=0; i<15; i++)
							gm.PutText(100, 480+(i*40)-scroll_time, steproll[i], RGB(255,255,255));
					}
					//�׹�° ����
					else if(clear_count==3)
					{
						char* steproll[]={
							"����: ȥ���� ���",
							"����: �˶˹�",
							"����: �˶˹�",
							"ȥ���� ����� ��: http://sico.wo.to",
							"http://kfgenius.ilen.net���ε� ���� ����",
							"ȥ����: http://hondoom.org",
							"�׳� �� ��ũ���� �÷��þ�.",
							"�ֳĸ� �̰� ������ �����̰ŵ�.",
							"�׷��� �� �̻� �������� ������.",
							"���� ���� ����~",
							"����������~",
							"��������~ ������.",
							"���簡 ������.",
							"�λ� ��������",
							"��"
						};
						for(int i=0; i<15; i++)
							gm.PutText(100, 480+(i*40)-scroll_time, steproll[i], RGB(255,255,255));
					}

					gm.Render();
				}
			}

			//Ŭ���� �ڵ� ����
			FILE* fp;
			if(fp=fopen("game.sav", "wb"))
			{
				int code=clear_code[Min(clear_count, 2)];
				fwrite(&code, sizeof(int), 1, fp);
				fclose(fp);
			}

			game_end=true;
		}
		//�ϳ��� �� ó��
		else
		{
			main.Start();

			//������ ��ŵ & FPS
			DWORD tick, tick2;
			int tf=0, fps=0;
			tick=GetTickCount();

			//���� ����
			main.LoadMap();
			while(main.escape==0)
			{
				if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
				{
					if(!GetMessage(&msg, NULL, 0, 0))return msg.wParam;

					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				//���α׷� ó�� �κ�
				else
				{
					//DirectMusic ���μ���
					HRESULT hr;
					if(FAILED(hr = ProcessDirectMusicMessages())) 
					{
						DXTRACE_ERR_MSGBOX( TEXT("ProcessDirectMusicMessages"), hr );
						return FALSE;
					}

					//��� ����
					if(!main.sw_move && !main.sw_ai)
					{
						if(!main.sw_focus)main.Scroll(MouseX, MouseY);
						if(gm.LeftClick())main.LeftClick(MouseX, MouseY);
						if(gm.RightClick())main.RightClick(MouseX, MouseY);
					}
					if(cheat_able)
					{
						//ġƮ
						if(_GetKeyState('F') && _GetKeyState('O') && _GetKeyState('G'))main.Cheat(0);
							else if(_GetKeyState('H') && _GetKeyState('E') && _GetKeyState('R') && _GetKeyState('O'))main.Cheat(1);
							else if(_GetKeyState('S') && _GetKeyState('T') && _GetKeyState('G'))main.Cheat(2);
							else if(_GetKeyState('S') && _GetKeyState('O'))main.Cheat(3);
							else if(_GetKeyState('E') && _GetKeyState('N') && _GetKeyState('D'))main.Cheat(4);
							else if(_GetKeyState('N') && _GetKeyState('O') && _GetKeyState('E'))main.Cheat(5);
					}

					main.Process();

					//������ ���
					tick2=GetTickCount();
					tf++;
					if(tick2-tick>1000)
					{
						tick=tick2;
						fps=tf;
						tf=0;
					}

					/////////////////////////////////////////
					//��º�
					//////////////////////////////////////////
					//�ִϸ��̼�
					ani_select.Process();
					ani_fire.Process();
					ani_heel.Process();
					ani_turn.Process();

					//�� ���
					if(active)
					{
						main.DrawMap(MouseX, MouseY, d_tile, r_tile);
						n_fps.PutNumber(0, 0, fps);

						gm.Render();
					}			
				}
			}	//�� ���� ����
			main.Event(EV_END);

			main.PartySort();
			
			//���� ����
			if(main.escape==2)
			{
				main.SetStartPoint(13, 0, 0);
				main_menu=true;
			}
			//�� ����
			else if(main.escape==3)
			{
				main.SetStartPoint(0, 8, 5);
				main.Init();
			}
			//����
			else if(main.escape==5)
			{
				ending=true;
			}
			//����
			else if(main.escape==99)
			{			
				game_end=true;
			}

			main.End();
		}	//�ϳ��� �� ����
	}	//��ü ������ ����
	
	return TRUE;
}

//������ �޼��� ó�� �κ�
LRESULT CALLBACK WindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
		case WM_ACTIVATEAPP:
			active = wParam;
			break;
		case WM_LBUTTONDOWN:
			LeftButton=true;
			break;
		case WM_RBUTTONDOWN:
			RightButton=true;
			break;
		case WM_LBUTTONUP:
			LeftButton=false;
			break;
		case WM_RBUTTONUP:
			RightButton=false;
			break;
		case WM_MOUSEMOVE:
			MouseX=LOWORD(lParam);
			MouseY=HIWORD(lParam);
			break;
		case WM_DESTROY:			
            SAFE_DELETE( g_pMusicSegment );
            SAFE_DELETE( g_pMusicManager );
            CloseHandle( g_hDMusicMessageEvent );
			PostQuitMessage( 0 );
			break;
	}

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void CHungLib::RestoreAll()
{
	surface_count=0;

	//Ÿ��
	LoadDGF("����","dgf\\tile_1F.dgf", NULL, &ani_tile);
	LoadDGF("����","dgf\\tile_2F.dgf", RGB(0,0,255), &ani_tile);

	//�̵��� ���� ����� ���� ����
	SurfaceCopy("����D","����");
	Colorize(GetImageID("����D"),DARK);
	//���� ���� ����� ���� ����
	SurfaceCopy("����R","����");
	Colorize(GetImageID("����R"),TO_RED);

	//ĳ����
	LoadDGF("����1","dgf\\unit.dgf",RGB(0,0,255), &ani_tile);
	ColorChange(GetImageID("����1"), RGB2(255,0,255), RGB2(184,185,244), RGB2(192,0,192), RGB2(128,128,255));
	LoadDGF("����2","dgf\\unit.dgf",RGB(0,0,255), &ani_tile);
	ColorChange(GetImageID("����2"), RGB2(255,0,255), RGB2(247,140,81), RGB2(192,0,192), RGB2(200,52,28));
	//�ߵ� �̹���
	SurfaceCopy("����1P","����1");
	Colorize(GetImageID("����1P"),TO_GREEN);
	SurfaceCopy("����2P","����2");
	Colorize(GetImageID("����2P"),TO_GREEN);
	//��ο� �̹���
	SurfaceCopy("����1D","����1");
	Colorize(GetImageID("����1D"),DARK);

	//����â ��
	LoadDGF("���� ������","dgf\\state_icon.dgf",RGB(0,0,255), &ani_icon);

	//����
	LoadDGF("����","dgf\\number.dgf",RGB(0,0,255), &ani_number);

	//�ִϸ��̼�
	LoadDGF("��","dgf\\ani_fire.dgf",RGB(0,0,255), &ani_fire);
	LoadDGF("���","dgf\\ani_damage.dgf",RGB(0,0,255), &ani_tile);
	LoadDGF("ȥ","dgf\\ani_soul.dgf",RGB(0,0,255), &ani_fire);
	LoadDGF("ġ��","dgf\\ani_heel.dgf",RGB(0,0,255), &ani_heel);
	LoadDGF("�ٶ�","dgf\\ani_wind.dgf",RGB(0,0,255), &ani_fire);
	LoadDGF("��","dgf\\ani_water.dgf",RGB(0,0,255), &ani_tile);
	LoadDGF("����","dgf\\ani_thunder.dgf",RGB(0,0,255), &ani_fire);
	LoadDGF("��","dgf\\ani_poison.dgf",RGB(0,0,255), &ani_tile);
	LoadDGF("��","dgf\\ani_light.dgf",RGB(0,0,255), &ani_tile);

	//Ÿ��Ʋ
	LoadDGF("Ÿ��Ʋ","dgf\\title.dgf");

	//�г�
	LoadDGF("���â","dgf\\panel.dgf");
	LoadDGF("����â","dgf\\state.dgf");
	LoadDGF("�ڼ���","dgf\\detail.dgf");
	LoadDGF("��","dgf\\detail2.dgf");
	LoadDGF("���� ��","dgf\\enemy_turn.dgf", NULL, &ani_turn);
	LoadDGF("��ȭâ","dgf\\dialog.dgf");
	LoadDGF("��Ƽ��","dgf\\party.dgf");
	LoadDGF("�������","dgf\\itemlist.dgf");
	
	//��Ÿ
	LoadDGF("Ŀ��","dgf\\cursor.dgf",RGB(0,0,255));
	LoadDGF("����","dgf\\here.dgf",RGB(0,0,255));
	LoadDGF("����","dgf\\select.dgf", RGB(0, 0, 255), &ani_select);
	LoadDGF("���ƴϿ�","dgf\\yesno.dgf",RGB(0,0,255), &ani_yesno);
}

void CHungLib::LoadSound()
{
	//���� �ҽ�
	if(SoundCard)
	{
		Sound[0] = SndObjCreate(SoundOBJ,"sound\\move.wav",2);
		Sound[1] = SndObjCreate(SoundOBJ,"sound\\puck.wav",2);
		Sound[2] = SndObjCreate(SoundOBJ,"sound\\poison.wav",2);
		Sound[3] = SndObjCreate(SoundOBJ,"sound\\fire.wav",2);
		Sound[4] = SndObjCreate(SoundOBJ,"sound\\bang.wav",2);
		Sound[5] = SndObjCreate(SoundOBJ,"sound\\tornado.wav",2);
		Sound[6] = SndObjCreate(SoundOBJ,"sound\\heal.wav",2);
		Sound[7] = SndObjCreate(SoundOBJ,"sound\\earth.wav",2);
		Sound[8] = SndObjCreate(SoundOBJ,"sound\\thunder.wav",2);
		Sound[9] = SndObjCreate(SoundOBJ,"sound\\beam.wav",2);
		Sound[10] = SndObjCreate(SoundOBJ,"sound\\upgrade.wav",2);
	}
}