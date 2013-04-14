#include <dsound.h>
#include <stdio.h>
#include <time.h>
#include <tchar.h>

#include "HungLib.h"
#include "useful.h"
#include "data.h"

#define IMG(x)		gm.GetImageID(x)

BOOL active;
int MouseX, MouseY;
BOOL LeftButton=FALSE, RightButton=FALSE;	//마우스 클릭 여부

//애니메이션 클래스
CAnimation ani_tile(40, 40);
CAnimation ani_icon(10, 10);
CAnimation ani_number(5, 6);
CAnimation ani_yesno(50, 50);
CAnimation ani_select(40, 40, 2, 3, 5);
CAnimation ani_fire(40, 40, 2, 3, 2);
CAnimation ani_heel(40, 40, 2, 5, 1);
CAnimation ani_turn(100, 50, 2, 3, 5);

//메인
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	//초기화
	CHungLib gm(hwnd, hInstance, nCmdShow, "Hyugaru", "휴가루 웍휴", 640, 480, false);

	srand((unsigned int)time(NULL));

	//이미지, 소리 불러오기
	gm.RestoreAll();
	gm.LoadSound();

	int d_tile=IMG("지형D");
	int r_tile=IMG("지형R");

	//캐릭터 초기화				 이름        HP  MV  MZ  FL 가능한 행동들
	cara[HERO1].SetCaracter		("휴가루",   10,  6,  0,  0,  0,  1,  5);
	cara[HERO2].SetCaracter		("가미얼",   10,  6,  1,  0,  0,  1,  5);
	cara[HERO3].SetCaracter		("시크벨",   10,  6,  2,  0,  0,  1,  5);
	cara[HERO4].SetCaracter		("루벨버",   10,  6,  3,  0,  0,  1,  5);
	cara[HERO5].SetCaracter		("미버가",   10,  6,  4,  0,  0,  1,  5);
	cara[HERO6].SetCaracter		("휴루시",   10,  6,  5,  0,  0,  1,  5);
	cara[SOLDIER1].SetCaracter	("용병",     10,  6, -1,  0,  0,  5);
	cara[SOLDIER2].SetCaracter	("공병",     10,  7, -1,  0,  2,  3,  5);
	cara[SOLDIER3].SetCaracter	("위생병",   10,  5, -1,  0,  4,  5);
	cara[BANDIT].SetCaracter	("산적",     10,  7, -1,  0,  0,  5);
	cara[BANG].SetCaracter		("방갈",      5,  6, -1,  0,  0,  5);
	cara[BIGROCK].SetCaracter	("큰바위",   20,  5, -1,  0,  0,  5);
	cara[HANAZUM].SetCaracter	("하나접",   10,  5, -1, -1,  0,  5);
	cara[KUOT].SetCaracter		("꾸옷",     10,  4, -1,  0,  0,  5);
	cara[MAEQO].SetCaracter		("매코",     10,  5, -1,  0,  0,  5);
	cara[DARKQUIZ].SetCaracter	("다크퀴즈", 10,  8,  6,  0,  0,  1,  5);
	cara[NEYUM].SetCaracter		("네염",	  0,  4, -1, -2,  0,  5);

	//장비 초기화(   이름			위력 값 타입  부위  )
	equipment[0].Set ("없음",		 0,   0,  0, EQ_ARMS);
	equipment[1].Set ("단검",		 1,  50,  1, EQ_ARMS);
	equipment[2].Set ("철검",		 2, 100,  1, EQ_ARMS);
	equipment[3].Set ("장검",		 1, 125,  1, EQ_ARMS);
	equipment[4].Set ("회전검",		 1, 150,  1, EQ_ARMS);
	equipment[5].Set ("창",			 1,  75,  2, EQ_ARMS);
	equipment[6].Set ("긴 창",		 1, 125,  2, EQ_ARMS);
	equipment[7].Set ("활",			 1, 100,  3, EQ_ARMS);
	equipment[8].Set ("강화 활",	 2, 200,  3, EQ_ARMS);
	equipment[9].Set ("긴 활",		 1, 250,  3, EQ_ARMS);
	equipment[10].Set("물펌프",		 0,  50,  6, EQ_ARMS);
	equipment[11].Set("불펌프",		 1, 100,  5, EQ_ARMS);
	equipment[12].Set("독덩쿨",		 0, 100, 10, EQ_ARMS);
	equipment[13].Set("가죽 갑옷",	 5,  50,  0, EQ_AMMOR);
	equipment[14].Set("철 갑옷",	10, 110,  0, EQ_AMMOR);
	equipment[15].Set("강철 갑옷",	20, 300,  0, EQ_AMMOR);
	equipment[16].Set("빛의 갑옷",	99, 999,  0, EQ_AMMOR);

	//게임 클래스에서 사용할 그림들 ID
	imgno.panel=IMG("명령창");
	imgno.state_icon=IMG("상태 아이콘");
	imgno.number=IMG("숫자");
	imgno.unit=IMG("유닛1");
	imgno.animation=IMG("불");
	imgno.cursor=IMG("커서");

	CNumber n_fps(&gm, IMG("숫자"), 5);

	//게임 초기화
	CGame main(&gm);
	bool game_end=false;
	main_menu=true;
	main.SetStartPoint(13, 0, 0);
	main.Init();

	//클리어 횟수 알아내기
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
		//엔딩
		if(ending)
		{
			_MidiPlay("end.mid");

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
					//스크롤
					if(++count>2)
					{
						scroll_time++;
						count=0;
					}
					//검은 바탕
					for(int j=0; j<12; j++)
						for(int i=0; i<16; i++)
							lib->Image(i*40, j*40, 0, 7);
					//스텝롤
					if(clear_count==0)
					{
						char* steproll[]={
							"제작: 혼돈과 어둠",
							"팀장: 똥똥배",
							"팀원: 똥똥배",
							"에헤야~ 나홀로 팀~",
							"특히 이번 게임은 라이브러리까지 직접 만들었다구.",
							"그래픽 파일도 DGF라고 내가 직접 만든 포맷이라구.",
							"그래봐야 JPG나 GIF가 압축률 더 좋으니 은폐기능밖에 없어.",
							"엔딩이래야 적을 것도 없으니 치트키 가르쳐 줄께.",
							"F, O, G   : 안개 없애기",
							"H, E, R, O: 주인공 무적 만들기",
							"E, N, D   : 대사가 길 때 그냥 끝내기",
							"N, O, E   : 세계지도에서 적 출현 안 하게 하기",
							"두개 더 있지만 하나는 귀찮고, 하나는 필요 없으니 생략",
							"처음 본 엔딩이니까 한 판 더해봐.",
							"아마 재밌는게 추가되어 있을지도... 크크크..."
						};
						for(int i=0; i<15; i++)
							gm.PutText(100, 480+(i*40)-scroll_time, steproll[i], JColor(255,255,255));
					}
					//두번째 엔딩
					else if(clear_count==1)
					{
						char* steproll[]={
							"제작: 혼돈과 어둠",
							"팀장: 똥똥배",
							"팀원: 똥똥배",
							"에헤야~ 나홀로 팀~",
							"두번이나 깼으니 대단하구마.",
							"대단~ 대단~",
							"어쩌면 대단하지 않을지도 모르지만 일단 대단.",
							"그럼 저번에 가르쳐 주기 귀찮았던거 가르쳐주지.",
							"S, O : 치트 불능 만들기",
							"치트 알아내려고 마구 누르는 거 방지용이었어.",
							"S, T, G + ? : 스테이지 이동",
							"0~C까지 같이 누르면 발동되지.",
							"0이 월드맵, 아무튼 숫자나 영어는 맵의 번호야.",
							"또 한 판 더해봐도 괜찮아.",
							"또 재밌는게 추가되어 있을걸... 크크크..."
						};
						for(int i=0; i<15; i++)
							gm.PutText(100, 480+(i*40)-scroll_time, steproll[i], JColor(255,255,255));
					}
					//세번째 엔딩
					else if(clear_count==2)
					{
						char* steproll[]={
							"제작: 혼돈과 어둠",
							"팀장: 똥똥배",
							"팀원: 똥똥배",
							"에헤야~ 나홀로 팀~",
							"이 짓도 3번째로구만.",
							"그나저나 자네도 징하구만",
							"무슨 재미가 있어 이렇게 계속하는 겐가?",
							"아니면 혹시 깨달았을지도 모르지.",
							"끝판 깨기 바로 직전 세이브를 이용하면 되는 걸.",
							"그래, 그걸 이용하면 클리어 횟수를 확확 늘리지.",
							"몰랐다면 자넨 정말 대단한 걸세.",
							"으어어어~",
							"이제 할 말도 없어.",
							"더 이상 엔딩을 보지 말아줘.",
							"그럼 이만... 쉬리리릭~"
						};
						for(int i=0; i<15; i++)
							gm.PutText(100, 480+(i*40)-scroll_time, steproll[i], JColor(255,255,255));
					}
					//네번째 엔딩
					else if(clear_count==3)
					{
						char* steproll[]={
							"제작: 혼돈과 어둠",
							"팀장: 똥똥배",
							"팀원: 똥똥배",
							"혼돈과 어둠의 땅: http://sico.wo.to",
							"http://kfgenius.ilen.net으로도 접속 가능",
							"혼돈성: http://hondoom.org",
							"그냥 좀 스크롤을 늘려봤어.",
							"왜냐면 이게 마지막 엔딩이거든.",
							"그러니 더 이상 괴롭히지 말아줘.",
							"딩가 딩가 딩가~",
							"추쿠추쿠추~",
							"으허허헝~ 귀찮다.",
							"만사가 귀찮다.",
							"인생 귀차니즘",
							"끝"
						};
						for(int i=0; i<15; i++)
							gm.PutText(100, 480+(i*40)-scroll_time, steproll[i], JColor(255,255,255));
					}

					gm.Render();
				}
			}

			//클리어 코드 저장
			FILE* fp;
			if(fp=fopen("game.sav", "wb"))
			{
				int code=clear_code[Min(clear_count, 2)];
				fwrite(&code, sizeof(int), 1, fp);
				fclose(fp);
			}

			game_end=true;
		}
		//하나의 맵 처리
		else
		{
			main.Start();

			//프레임 스킵 & FPS
			DWORD tick, tick2;
			int tf=0, fps=0;
			tick=GetTickCount();

			//메인 루프
			main.LoadMap();
			while(main.escape==0)
			{
				if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
				{
					if(!GetMessage(&msg, NULL, 0, 0))return msg.wParam;

					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				//프로그램 처리 부분
				else
				{
					//대기 상태
					if(!main.sw_move && !main.sw_ai)
					{
						if(!main.sw_focus)main.Scroll(MouseX, MouseY);
						if(gm.LeftClick())main.LeftClick(MouseX, MouseY);
						if(gm.RightClick())main.RightClick(MouseX, MouseY);
					}
					if(cheat_able)
					{
						//치트
						if(_GetKeyState('F') && _GetKeyState('O') && _GetKeyState('G'))main.Cheat(0);
							else if(_GetKeyState('H') && _GetKeyState('E') && _GetKeyState('R') && _GetKeyState('O'))main.Cheat(1);
							else if(_GetKeyState('S') && _GetKeyState('T') && _GetKeyState('G'))main.Cheat(2);
							else if(_GetKeyState('S') && _GetKeyState('O'))main.Cheat(3);
							else if(_GetKeyState('E') && _GetKeyState('N') && _GetKeyState('D'))main.Cheat(4);
							else if(_GetKeyState('N') && _GetKeyState('O') && _GetKeyState('E'))main.Cheat(5);
					}

					main.Process();

					//프레임 계산
					tick2=GetTickCount();
					tf++;
					if(tick2-tick>1000)
					{
						tick=tick2;
						fps=tf;
						tf=0;
					}

					/////////////////////////////////////////
					//출력부
					//////////////////////////////////////////
					//애니메이션
					ani_select.Process();
					ani_fire.Process();
					ani_heel.Process();
					ani_turn.Process();

					//맵 출력
					if(active)
					{
						main.DrawMap(MouseX, MouseY, d_tile, r_tile);
						n_fps.PutNumber(0, 0, fps);

						gm.Render();
					}			
				}
			}	//한 맵의 루프
			main.Event(EV_END);

			main.PartySort();
			
			//게임 오버
			if(main.escape==2)
			{
				main.SetStartPoint(13, 0, 0);
				main_menu=true;
			}
			//새 게임
			else if(main.escape==3)
			{
				main.SetStartPoint(0, 8, 5);
				main.Init();
			}
			//엔딩
			else if(main.escape==5)
			{
				ending=true;
			}
			//종료
			else if(main.escape==99)
			{			
				game_end=true;
			}

			main.End();
		}	//하나의 맵 루프
	}	//전체 게임의 루프
	
	return TRUE;
}

//윈도우 메세지 처리 부분
LRESULT CALLBACK WindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
		case WM_SIZE		 :	if(wParam == SIZE_MINIMIZED)active=false;
								else active=true;
								break;
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
	   case    MM_MCINOTIFY:
			if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL ) _MidiReplay();
			break;
		case WM_MOVE:
			if(jdd)jdd->OnMove(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_DESTROY:			
			PostQuitMessage( 0 );
			break;
	}

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void CHungLib::RestoreAll()
{
	surface_count=0;

	//타일
	LoadDGF("지형","dgf\\tile_1F.bmp", JColor(0, 0, 0), &ani_tile);
	LoadDGF("지물","dgf\\tile_2F.bmp", JColor(0,0,255), &ani_tile);
	LoadDGF("지형D","dgf\\tile_1F_Dark.bmp", JColor(0, 0, 0), &ani_tile);
	LoadDGF("지형R","dgf\\tile_1F_Red.bmp", JColor(0,0,255), &ani_tile);

	//캐릭터
	LoadDGF("유닛1","dgf\\unit1.png",JColor(0,0,255), &ani_tile);
	LoadDGF("유닛2","dgf\\unit2.png",JColor(0,0,255), &ani_tile);
	LoadDGF("유닛1P","dgf\\unit1_poison.png",JColor(0,0,255), &ani_tile);
	LoadDGF("유닛2P","dgf\\unit2_poison.png",JColor(0,0,255), &ani_tile);
	LoadDGF("유닛1D","dgf\\unit1_dark.png",JColor(0,0,255), &ani_tile);

	//상태창 용
	LoadDGF("상태 아이콘","dgf\\state_icon.png",JColor(0,0,255), &ani_icon);

	//숫자
	LoadDGF("숫자","dgf\\number.png",JColor(0,0,255), &ani_number);

	//애니메이션
	LoadDGF("불","dgf\\ani_fire.bmp",JColor(0,0,255), &ani_fire);
	LoadDGF("충격","dgf\\ani_damage.bmp",JColor(0,0,255), &ani_tile);
	LoadDGF("혼","dgf\\ani_soul.bmp",JColor(0,0,255), &ani_fire);
	LoadDGF("치료","dgf\\ani_heel.bmp",JColor(0,0,255), &ani_heel);
	LoadDGF("바람","dgf\\ani_wind.bmp",JColor(0,0,255), &ani_fire);
	LoadDGF("물","dgf\\ani_water.bmp",JColor(0,0,255), &ani_tile);
	LoadDGF("벼락","dgf\\ani_thunder.bmp",JColor(0,0,255), &ani_fire);
	LoadDGF("독","dgf\\ani_poison.bmp",JColor(0,0,255), &ani_tile);
	LoadDGF("빛","dgf\\ani_light.bmp",JColor(0,0,255), &ani_tile);

	//타이틀
	LoadDGF("타이틀","dgf\\title.bmp");

	//패널
	LoadDGF("명령창","dgf\\panel.bmp");
	LoadDGF("상태창","dgf\\state.bmp");
	LoadDGF("자세히","dgf\\detail.bmp");
	LoadDGF("얼굴","dgf\\detail2.bmp");
	LoadDGF("적의 턴","dgf\\enemy_turn.bmp", JColor(0, 0, 0), &ani_turn);
	LoadDGF("대화창","dgf\\dialog.bmp");
	LoadDGF("파티원","dgf\\party.bmp");
	LoadDGF("소지목록","dgf\\itemlist.bmp");
	
	//기타
	LoadDGF("커서","dgf\\cursor.bmp",JColor(0,0,255));
	LoadDGF("여기","dgf\\here.bmp",JColor(0,0,255));
	LoadDGF("선택","dgf\\select.bmp", JColor(0, 0, 255), &ani_select);
	LoadDGF("예아니오","dgf\\yesno.bmp",JColor(0,0,255), &ani_yesno);
}

void CHungLib::LoadSound()
{
	//사운드 소스
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