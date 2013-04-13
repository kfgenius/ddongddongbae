#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdio.h>
#include <time.h>

#include "HungLib.h"

#define X_MAX		48
#define Y_MAX		48
#define LAYER_MAX	3
#define FILE_MAX	15
#define UNDO_MAX	20

HWND hwnd;

int MouseX, MouseY;
BOOL LeftButton=FALSE, RightButton=FALSE;	//마우스 클릭 여부
BOOL active;

//전역 변수
FILE *fp;											//파일 포인터

char map[X_MAX][Y_MAX][LAYER_MAX];					//맵 데이터
int x_size, y_size;									//맵의 크기
int file_no=0;										//편집 중인 파일 번호

char history[UNDO_MAX][X_MAX][Y_MAX][LAYER_MAX];	//히스토리 목록
int undo_sp;										//UNDO 포인터
int undo_count;										//UNDO 사용 가능 횟수
bool undo_sw=false;									//UNDO를 위한 스위치

//최대값
int Max(int x, int y)
{
	if(x>y)return x;
		else return y;
}

//최소값
int Min(int x, int y)
{
	if(x>y)return y;
		else return x;
}

//이전 작업 데이터 저장
void SaveHistory()
{
	if(undo_sp>0)undo_count=undo_sp=0;

	int max=Min(undo_count, UNDO_MAX-1);
	for(int i=max; i>0; i--)
		memcpy(history[i],history[i-1],sizeof(map));
	memcpy(history[0],map,sizeof(map));

	if(undo_count<UNDO_MAX)undo_count++;
}

//초기화
void Init(int tile_no=0, int x=32, int y=24)
{
	//1층을 정해진 타일로
	for(int j=0; j<Y_MAX; j++)
		for(int i=0; i<X_MAX; i++)
				map[i][j][0]=tile_no;
	//2,3층 비우기
	for(int k=1; k<LAYER_MAX; k++)
		for(int j=0; j<Y_MAX; j++)
			for(int i=0; i<X_MAX; i++)
					map[i][j][k]=0;

	x_size=x;
	y_size=y;
}

//맵 데이터 읽어 들이기
void Load()
{
	char file_name[20];
	sprintf(file_name,"map\\map%03d.map",file_no);
	if(fp=fopen(file_name,"rb"))
	{
		//데이터 종류와 버전 확인
		char data_type[11], ver;
		fread(&data_type,sizeof(char),10,fp);
		data_type[10]=NULL;

		if(strcmp(data_type,"HYUGARUMAP")==0)
		{
			fread(&ver,sizeof(char),1,fp);
			fread(&x_size,sizeof(int),1,fp);
			fread(&y_size,sizeof(int),1,fp);
			for(int k=0; k<3; k++)
				for(int j=0; j<y_size; j++)
					for(int i=0; i<x_size; i++)
						fread(&map[i][j][k],sizeof(char),1,fp);
		}

		fclose(fp);
	}

	undo_sp=undo_count=0;
	SaveHistory();
}

//맵데이터 저장
void Save()
{
	char file_name[20];
	sprintf(file_name,"map\\map%03d.map",file_no);
	if(fp=fopen(file_name,"wb"))
	{
		char ver=10;
		fwrite("HYUGARUMAP",sizeof(char),10,fp);
		fwrite(&ver,sizeof(char),1,fp);
		fwrite(&x_size,sizeof(int),1,fp);
		fwrite(&y_size,sizeof(int),1,fp);
		for(int k=0; k<3; k++)
			for(int j=0; j<y_size; j++)
				for(int i=0; i<x_size; i++)
					fwrite(&map[i][j][k],sizeof(char),1,fp);
		fclose(fp);
	}
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	//초기화
	CHungLib m_game(hwnd, hInstance, nCmdShow, "Hyugaru MapEditor", "휴가루 맵에디터", 640, 480, false);
	ZeroMemory(history, sizeof(history));

	//이미지 불러오기
	m_game.AddSurface("풀","grass.bmp");
	m_game.AddSurface("모래","sand.bmp");
	m_game.AddSurface("흙","dirt.bmp");
	m_game.AddSurface("물","water.bmp");
	m_game.AddSurface("다리","bridge.bmp");
	m_game.AddSurface("타일","tile.bmp");
	m_game.AddSurface("벽","wall.bmp");
	m_game.AddSurface("공간","blank.bmp");
	m_game.AddSurface("돌","stone.bmp",RGB(0,0,255));
	m_game.AddSurface("나무","tree.bmp",RGB(0,0,255));
	m_game.AddSurface("선인장","cactus.bmp",RGB(0,0,255));
	m_game.AddSurface("바위","rock.bmp",RGB(0,0,255));
	m_game.AddSurface("집","house.bmp",RGB(0,0,255));
	m_game.AddSurface("탑","tower.bmp",RGB(0,0,255));
	m_game.AddSurface("구름","cloud.bmp",RGB(0,0,255));
	m_game.AddSurface("시작","start.bmp",RGB(0,0,255));
	m_game.AddSurface("적","enemy.bmp",RGB(0,0,255));
	m_game.AddSurface("영웅","hero.bmp",RGB(0,0,255));
	m_game.AddSurface("보스","boss.bmp",RGB(0,0,255));

	m_game.AddSurface("풀10x10","grass2.bmp");
	m_game.AddSurface("모래10x10","sand2.bmp");
	m_game.AddSurface("흙10x10","dirt2.bmp");
	m_game.AddSurface("물10x10","water2.bmp");
	m_game.AddSurface("다리10x10","bridge2.bmp");
	m_game.AddSurface("타일10x10","tile2.bmp");
	m_game.AddSurface("벽10x10","wall2.bmp");
	m_game.AddSurface("공간10x10","blank2.bmp");
	m_game.AddSurface("돌10x10","stone2.bmp",RGB(0,0,255));
	m_game.AddSurface("나무10x10","tree2.bmp",RGB(0,0,255));
	m_game.AddSurface("선인장10x10","cactus2.bmp",RGB(0,0,255));
	m_game.AddSurface("바위10x10","rock2.bmp",RGB(0,0,255));
	m_game.AddSurface("집10x10","house2.bmp",RGB(0,0,255));
	m_game.AddSurface("탑10x10","tower2.bmp",RGB(0,0,255));
	m_game.AddSurface("구름10x10","cloud2.bmp",RGB(0,0,255));
	m_game.AddSurface("시작10x10","start2.bmp",RGB(0,0,255));
	m_game.AddSurface("적10x10","enemy2.bmp",RGB(0,0,255));
	m_game.AddSurface("영웅10x10","hero2.bmp",RGB(0,0,255));
	m_game.AddSurface("보스10x10","boss2.bmp",RGB(0,0,255));

	m_game.AddSurface("배경","back.bmp");
	m_game.AddSurface("<-","arrow_left.bmp",RGB(0,0,255));
	m_game.AddSurface("->","arrow_right.bmp",RGB(0,0,255));
	m_game.AddSurface("커서","cursor.bmp",RGB(0,0,255));
	m_game.AddSurface("미리보기","preview.bmp",RGB(0,0,255));
	m_game.AddSurface("지우기","clear.bmp",RGB(0,0,255));

	//각 그림들의 시작 위치
	int layer_sp[3];
	layer_sp[0]=m_game.GetImageID("풀");
	layer_sp[1]=m_game.GetImageID("돌");
	layer_sp[2]=m_game.GetImageID("구름");
	int tile_sp=m_game.GetImageID("풀10x10");

	//기타 변수들 초기화
	int fps_sec=0;
	int fps_value=0;
	char now[2]={0,0};
	int mouse_push=0;

	Init();
	Load();
	
	//메인 루프
	while(!m_game.GetKey(5))
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
			/////////////////////////////////////////////
			//제어 부분
			if(LeftButton || RightButton)
			{
				int now_pt;
				if(LeftButton)now_pt=0;
					else now_pt=1;
				//맵 수정
				if(MouseX<480)
				{
					if(MouseX<x_size*10 && MouseY<y_size*10)
					{
						undo_sw=true;

						if(now[now_pt]<layer_sp[1])map[MouseX/10][MouseY/10][0]=now[now_pt];
							else if(now[now_pt]<layer_sp[2]+1)map[MouseX/10][MouseY/10][1]=now[now_pt];
							else map[MouseX/10][MouseY/10][2]=now[now_pt];
					}
				}
				//메뉴
				else
				{
					//타일 선택
					if(MouseY<240)
					{
						int tile_no=((MouseX-480)/40)+(MouseY/40*4);
						if(tile_no<tile_sp+2)now[now_pt]=tile_no;
					}
					//맵 바꿈
					else if(MouseY>=320 && MouseY<340)
					{
						if(!mouse_push)
						{
							mouse_push=999;

							//데이터 바꿈
							if(MouseX>=580 && MouseX<600)
							{
								Save();
								file_no--;
								if(file_no<0)file_no=FILE_MAX-1;
								Init();
								Load();
							}
							else if(MouseX>=600 && MouseX<620)
							{
								Save();
								file_no++;
								if(file_no>=FILE_MAX)file_no=0;
								Init();
								Load();
							}
						}
						else if(mouse_push<999)mouse_push--;
					}
					//크기 조절
					else if(MouseY>=340 && MouseY<360)
					{
						if(!mouse_push)
						{
							mouse_push=10;

							if(MouseX>=580 && MouseX<600)
							{
								if(x_size>16)x_size--;
							}
							else if(MouseX>=600 && MouseX<620)
							{
								if(x_size<X_MAX)x_size++;
							}
						}
						else if(mouse_push<999)mouse_push--;
					}
					else if(MouseY>=360 && MouseY<380)
					{
						if(!mouse_push)
						{
							mouse_push=10;

							if(MouseX>=580 && MouseX<600)
							{
								if(y_size>12)y_size--;
							}
							else if(MouseX>=600 && MouseX<620)
							{
								if(y_size<Y_MAX)y_size++;
							}
						}
						else if(mouse_push<999)mouse_push--;
					}
					//UNDO, REDO
					else if(MouseY>=380 && MouseY<400)
					{
						if(!mouse_push)
						{
							mouse_push=10;

							if(MouseX>=580 && MouseX<600)
							{
								if(undo_sp>0)
								{
									undo_sp--;
									memcpy(map, history[undo_sp], sizeof(map));
								}
							}
							else if(MouseX>=600 && MouseX<620)
							{
								if(undo_sp<undo_count-1)
								{
									undo_sp++;
									memcpy(map, history[undo_sp], sizeof(map));
								}
							}
						}
						else if(mouse_push<999)mouse_push--;
					}
					//미리보기, 지우기
					else if(MouseY>=400 && MouseY<420)
					{
						if(!mouse_push)
						{
							mouse_push=999;

							if(MouseX>=480 && MouseX<550)
							{
								int fx=0, fy=0;
								while(!m_game.GetKey(5))
								{
									if(m_game.GetKey(0))fy=Min(fy+1, y_size-12);
									if(m_game.GetKey(1))fx=Max(fx-1, 0);
									if(m_game.GetKey(2))fy=Max(fy-1, 0);
									if(m_game.GetKey(3))fx=Min(fx+1, x_size-16);

									for(int k=0; k<3; k++)
										for(int j=fy; j<Min(y_size, fy+12); j++)
											for(int i=fx; i<Min(x_size, fx+16); i++)
												if(map[i][j][k]>=layer_sp[k]+k)m_game.Image((i-fx)*40, (j-fy)*40, map[i][j][k]-k);
									if(active)m_game.Render();
								}
							}
							else if(MouseX>=550 && MouseX<620)
							{
								if(now[0]<layer_sp[1])
								{
									Init(now[0],x_size,y_size);
									SaveHistory();
								}
							}
						}
						else if(mouse_push<999)mouse_push--;
					}
				}
			}

			//마우스를 뗐을 때 일어나는 이벤트
			if(!LeftButton)
			{
				mouse_push=0;
				if(undo_sw)
				{
					undo_sw=false;
					SaveHistory();
				}
			}

			/////////////////////////////////////////////
			//출력 부분
			m_game.Image(0, 0, "배경");

			//맵
			for(int k=0; k<3; k++)
				for(int j=0; j<y_size; j++)
					for(int i=0; i<x_size; i++)
						if(map[i][j][k]>=layer_sp[k]+k)m_game.Image(i*10, j*10, map[i][j][k]+(tile_sp-k));

			//타일
			for(int i=layer_sp[0]; i<layer_sp[1]; i++)m_game.Image(480+((i%4)*40),(i/4)*40,i);
			for(i=layer_sp[1]; i<layer_sp[2]; i++)m_game.Image(480+(((i+1)%4)*40),((i+1)/4)*40,i);
			for(i=layer_sp[2]; i<tile_sp; i++)m_game.Image(480+(((i+2)%4)*40),((i+2)/4)*40,i);

			//화살표들
			for(i=0; i<4; i++)
			{
				m_game.Image(580,320+i*20,"<-");
				m_game.Image(600,320+i*20,"->");
			}
			//미리보기, 지우기
			m_game.Image(480,402,"미리보기");
			m_game.Image(550,402,"지우기");

			//DC를 쓰는 부분
			char buffer[10], buffer2[20];
			//선택된 타일
			m_game.PutText(490+((now[0]%4)*40),(now[0]/4)*40+14,"L");
			m_game.PutText(504+((now[1]%4)*40),(now[1]/4)*40+14,"R");
			//파일번호
			sprintf(buffer2,"파일 번호: %d",file_no);
			m_game.PutText(480,322,buffer2);
			//X 사이즈
			sprintf(buffer2,"X 사이즈: %d",x_size);
			m_game.PutText(480,342,buffer2);
			//Y 사이즈
			sprintf(buffer2,"Y 사이즈: %d",y_size);
			m_game.PutText(480,362,buffer2);
			//UNDO 정보
			sprintf(buffer2,"UNDO: %d/%d", undo_sp+1, undo_count);
			m_game.PutText(480,382,buffer2);
			//초당 프레임 수(FPS) 출력
			if(++fps_sec>10)
			{
				fps_value=m_game.GetFPS();				
				fps_sec=0;
			}
			sprintf(buffer,"속도: %d fps",fps_value);
			m_game.PutText(480,422,buffer);

			//커서
			m_game.Image(MouseX, MouseY, "커서");

			//렌더링
			if(active)m_game.Render();
		}
	}

	Save();

	return TRUE;
}

//윈도우 메세지 처리 부분
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
			PostQuitMessage( 0 );
			break;
	}

    return DefWindowProc(hwnd, message, wParam, lParam);
}