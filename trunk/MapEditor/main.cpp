#include <stdio.h>
#include <math.h>

#include "donglib.h"
#include "resource2.h"

BOOL window_mode=TRUE;
char dest_surface[]="FrameBuffer";
CAnimation m_ani;

JFont font20, font10, font5;
JBrush red_brush;

char edit_file_name[80] = "NoName";
bool open_dialog = false;

//int undo_count=0;

//다이얼로그 핸들
HWND hNewMap;
HWND hLoadMap;
HWND hAttr;
HWND hUnitAttr;
HWND hEventID;

int tile_no, layer_no;
int selected_event_x, selected_event_y;

/////////////////////////////
// 맵 에디터
#define MAP_MAX			100
#define MAP_XMAX		80
#define MAP_YMAX		60
#define LAYER_MAX		3
#define TILE_MAX		120
#define EVENT_NO_MAX	100

#define MODE_COMMON		0
#define MODE_CUSTOM		1

//메뉴 위치
#define NEW_ICON_X		660
#define NEW_ICON_Y		5
#define OPEN_ICON_X		700
#define OPEN_ICON_Y		5
#define FILE_ICON_WIDTH		40
#define FILE_ICON_HEIGHT	40

//윈도우 이름
#define BASE_WINDOW_NAME	"혼둠맵Editor v0.9a+"

unsigned char x_size, y_size;
unsigned char land_map[LAYER_MAX][MAP_XMAX][MAP_YMAX];	//지형 정보
unsigned char eventland_map[MAP_XMAX][MAP_YMAX];		//이벤트
unsigned char passable_map[MAP_XMAX][MAP_YMAX];			//통행여부
unsigned char selected[LAYER_MAX]={0,0,0};
bool save_recent;	//세이브 여부
bool redraw;	//다시 그리는 여부

int edit_layer=0;
int edit_tool=0;
int zoomin=4;
int fx, fy;	//스크롤
int tile_size, unit_ysize;
bool enable_undo;
int unit_edit_mode;

//타일 그림
char* tile1F[] = {"", "tile1", "tile1m", "", "tile1s"}; // 1배, 2배, 4배
char* tile2F[] = {"", "tile2", "tile2m", "", "tile2s"};
char* unit2F[] = {"", "UnitTile", "UnitTilem", "", "UnitTiles"};

//타일 속성
int tile_mov[2][TILE_MAX];
int tile_attr[2][TILE_MAX];
int tile_damage[2][TILE_MAX];

struct CUnitData
{
	int hp;
	int mp;

	int pow;
	int mpow;
	int def;
	int mdef;
	int agl;
	int mov;
	int jump;
	int swim;
	int see;
	int act;

	int attr;
};

CUnitData unitdata[TILE_MAX];

//이벤트 수정 관련
char* event_content[]={"없음", "시나리오", "변수", "스위치", "장소이동", "돈", "동료", "장비", "적", "무기점", "도구점", "알선소", "미지학교"};
char* require_content[]={"충돌","대화","무조건"};

#define PIC_TOOLS	"Tools"
#define PIC_MOVEMAP	"MoveMap"
#define PIC_SAVE	"ClearSave"
#define PIC_UNDO	"Undo"
#define PIC_SIZE	"Size"
#define PIC_MOVE	"Move"

#define DELAY_MAX	5

#define SAVE_UNDO	if(!action)gp_undo = CreateUndo(gp_undo)

struct UndoData
{
	unsigned char x_size, y_size;
	unsigned char land_map[LAYER_MAX][MAP_XMAX][MAP_YMAX];
	unsigned char eventland_map[MAP_XMAX][MAP_YMAX];

	UndoData* undo;
	//UndoData* redo;
};
UndoData* gp_undo = NULL;

bool CheckDataChange(UndoData* original)
{
	if(original->x_size != x_size)return true;
	if(original->y_size != y_size)return true;

	//맵 데이터 복사
	for(int k=0; k<3; ++k)
	for(int y=0; y<MAP_YMAX; ++y)
	for(int x=0; x<MAP_XMAX; ++x)
		if(original->land_map[k][x][y] != land_map[k][x][y])return true;

	//이벤트 맵
	for(int y=0; y<MAP_YMAX; ++y)
	for(int x=0; x<MAP_XMAX; ++x)
		if(original->eventland_map[x][y] != eventland_map[x][y])return true;

	return false;
}

UndoData* CreateUndo(UndoData* p_undo)
{
	//Undo데이터 생성
	UndoData* old_undo = p_undo;
	p_undo = new UndoData();

	p_undo->x_size = x_size;
	p_undo->y_size = y_size;

	//맵 데이터 복사
	for(int k=0; k<3; ++k)
	for(int y=0; y<MAP_YMAX; ++y)
	for(int x=0; x<MAP_XMAX; ++x)
		p_undo->land_map[k][x][y] = land_map[k][x][y];

	//이벤트 맵
	for(int y=0; y<MAP_YMAX; ++y)
	for(int x=0; x<MAP_XMAX; ++x)
		p_undo->eventland_map[x][y] = eventland_map[x][y];

	p_undo->undo = old_undo;

	//undo_count++;

	return p_undo;
}

UndoData* ReadUndo(UndoData* p_undo)
{
	UndoData* next_undo = p_undo->undo;

	x_size = p_undo->x_size;
	y_size = p_undo->y_size;

	//맵 데이터 복사
	for(int k=0; k<3; ++k)
	for(int y=0; y<MAP_YMAX; ++y)
	for(int x=0; x<MAP_XMAX; ++x)
		land_map[k][x][y] = p_undo->land_map[k][x][y];

	//이벤트 맵
	for(int y=0; y<MAP_YMAX; ++y)
	for(int x=0; x<MAP_XMAX; ++x)
		eventland_map[x][y] = p_undo->eventland_map[x][y];

	delete p_undo;
	p_undo = NULL;

	//undo_count--;

	return next_undo;
}

//Undo데이터 하나 제거
UndoData* DeleteUndoData(UndoData* p_undo)
{
	UndoData* tmp_undo = p_undo->undo;
	delete p_undo;
	p_undo = NULL;

	return tmp_undo;
}

//모든 Undo 제거
void ClearUndo()
{
	//undo 데이터 모두 지우기
	while(gp_undo != NULL)
	{
		gp_undo=DeleteUndoData(gp_undo);
	}
	enable_undo = false;
}

//맵 초기화
void MapDataInit()
{
	x_size=20;
	y_size=15;

	for(int j=0; j<60; j++)
		for(int i=0; i<80; i++)
		{
			land_map[0][i][j]=0;
			land_map[1][i][j]=0xff;
			land_map[2][i][j]=0xff;
			eventland_map[i][j]=0xff;
		}

	//undo 데이터 모두 지우기
	ClearUndo();
}

//세이브
void Save()
{
	FILE *fp;
	char map_name[80];
	sprintf(map_name, "MAP\\%s.map", edit_file_name);
	if(fp=fopen(map_name, "wb"))
	{
		//휴가루웍휴2 맵임을 표시
		char data_type[12]="HYUGARU2MAP";
		fwrite(data_type, sizeof(char), 11, fp);
		//맵 크기
		fwrite(&x_size, sizeof(char), 1, fp);
		fwrite(&y_size, sizeof(char), 1, fp);
		//맵 데이터
		for(int k=0; k<3; k++)
			for(int j=0; j<y_size; j++)
				for(int i=0; i<x_size; i++)
					fwrite(&land_map[k][i][j], sizeof(char), 1, fp);
		//이벤트 맵 데이터
		for(int j=0; j<y_size; j++)
			for(int i=0; i<x_size; i++)
					fwrite(&eventland_map[i][j], sizeof(char), 1, fp);

		fclose(fp);
	}
}

//로드
bool Load()
{
	FILE *fp;
	char map_name[80];
	sprintf(map_name, "MAP\\%s.map", edit_file_name);
	if(fp=fopen(map_name, "rb"))
	{
		char data_type[12];
		fread(&data_type,sizeof(char),11,fp);
		data_type[11]=NULL;

		//휴가루 웍휴2 맵(~ 혼둠맵 V0.9a)
		if(strcmp(data_type,"HYUGARU2MAP")==0)
		{
			MapDataInit();

			//맵 크기
			fread(&x_size, sizeof(char), 1, fp);
			fread(&y_size, sizeof(char), 1, fp);
			//맵 데이터
			for(int k=0; k<3; k++)
				for(int j=0; j<y_size; j++)
					for(int i=0; i<x_size; i++)
						fread(&land_map[k][i][j], sizeof(char), 1, fp);
			//이벤트 맵 데이터
			for(int j=0; j<y_size; j++)
				for(int i=0; i<x_size; i++)
					fread(&eventland_map[i][j], sizeof(char), 1, fp);
			
			fclose(fp);
			return true;
		}

		fclose(fp);
		return false;
	}

	return false;
}

//페인트
void PaintMap(int x, int y, int value)
{
	if(x<0 || x>=x_size || y<0 || y>=y_size)return;
	if(land_map[edit_layer][x][y]!=value)return;

	value=land_map[edit_layer][x][y];
	
	land_map[edit_layer][x][y]=selected[edit_layer];
	if(edit_layer == 2)eventland_map[x][y]=selected[edit_layer];

	PaintMap(x-1, y, value);
	PaintMap(x+1, y, value);
	PaintMap(x, y-1, value);
	PaintMap(x, y+1, value);
}

void Paint(int x, int y)
{
	if(x<0 || x>=x_size || y<0 || y>=y_size)return;
	if(land_map[edit_layer][x][y]==selected[edit_layer])return;

	PaintMap(x, y, land_map[edit_layer][x][y]);
}

//축소된 타일 생성
void CreateSmallTile(char* tile1_name, char* tile2_name, char* unit_name, int denominator)
{
	JPictureInfo jpi;
	RECT src_rect, dest_rect;
	JBrush blue_brush = jdd->CreateBrush(JColor(0,0,255));

	//1F 타일 복사
	jpi.SetWidth(800/denominator);
	jpi.SetHeight(32/denominator);
	SetRect(&src_rect, 0, 0, 800, 32);
	SetRect(&dest_rect, 0, 0, 800/denominator, 32/denominator);
	jdd->CreateSurface(tile1_name, &jpi, true);
	jdd->DrawStretchedPicture(tile1_name, tile1F[1], &dest_rect, &src_rect); 

	//2F 타일 복사
	jpi.SetHeight(40/denominator);
	jpi.SetColorKey(JColor(0,0,255));
	SetRect(&src_rect, 0, 0, 800, 40);
	SetRect(&dest_rect, 0, 0, 800/denominator, 40/denominator);
	jdd->CreateSurface(tile2_name, &jpi, true);
	jdd->DrawRect(tile2_name, blue_brush, &dest_rect);
	jdd->DrawStretchedPicture(tile2_name, tile2F[1], &dest_rect, &src_rect); 

	//유닛 복사
	SetRect(&src_rect, 0, 0, 800, 40);
	SetRect(&dest_rect, 0, 0, 800/denominator, 40/denominator);
	jdd->CreateSurface(unit_name, &jpi, true);
	jdd->DrawRect(unit_name, blue_brush, &dest_rect);
	jdd->DrawStretchedPicture(unit_name, unit2F[1], &dest_rect, &src_rect); 

	jdd->DeleteBrush(blue_brush);
}

//타일 생성
void CreateTile()
{
	//타일용 서페이스 생성
	JPictureInfo jpi;

	jpi.SetWidth(800);
	jpi.SetHeight(32);
	jpi.SetColorKey(JColor(0,0,255));

	jdd->CreateSurface(tile1F[1], &jpi, true);
	jdd->LoadPicture("RealTile", "User\\tile1.gif", NULL, true);

	//타일 중심부만 표시
	for(int i=0; i<25; i++)
	{
		RECT src_rect;
		SetRect(&src_rect, i*48+8, 8, i*48+40, 40);
		jdd->DrawPicture(tile1F[1], "RealTile", i<<5, 0, &src_rect);
	}

	//실제 타일에 칼라키 적용
	JPictureInfo jpi2;
	jpi2.SetColorKey(JColor(0,0,255));
	jdd->SetPictureInfo("RealTile", &jpi2);
}

//유닛 생성
void CreateUnit()
{
	//유닛용 서페이스 생성
	JPictureInfo jpi;
	RECT src_rect, dest_rect;
	JBrush blue_brush = jdd->CreateBrush(JColor(0,0,255));

	SetRect(&dest_rect, 0, 0, 800, 40);
	SetRect(&src_rect, 0, 120, 32, 160);	//유닛을 대표하는 부분

	jpi.SetWidth(800);
	jpi.SetHeight(40);
	jpi.SetColorKey(JColor(0, 0, 255));

	jdd->CreateSurface(unit2F[1], &jpi, true);
	jdd->DrawRect(unit2F[1], blue_brush, &dest_rect);

	jdd->DeleteBrush(blue_brush);

	//파일에서 읽기
	int file_no = 0;

	FILE *fp;
	while(1)
	{
		if(file_no>=25)break;	//25개가 한계

		char src_file[80];
		strcpy(src_file, StrAdd("User\\Unit%d.png", file_no));

		if(fp = fopen(src_file, "rb"))
		{
			fclose(fp);

			//해당 유닛 파일에서 정면 모습 하나를 가져다 찍음
			jdd->LoadPicture("Temp", src_file, NULL, true);
			jdd->DrawPicture(unit2F[1], "Temp", file_no<<5, 0, &src_rect);
			jdd->DeleteSurface("Temp");

			++file_no;
		}
		else break;
	}
}

//화면을 다시 그려주는 함수
void RedrawAll()
{
	//배경
	RECT back_rect;
	SetRect(&back_rect, 0, 0, 640, 480);
	jdd->DrawPicture(dest_surface, "Back", 0, 0, &back_rect);

	int xmax=Min(x_size, fx+(20*zoomin));
	int ymax=Min(y_size, fy+(15*zoomin));
	//타일
	if(zoomin==1)
	{
		for(int k=0; k<25; k++)
		for(int j=fy; j<ymax; j++)
		for(int i=fx; i<xmax; i++)
			if(land_map[0][i][j]==k)m_ani.GetAni(0)->Draw(dest_surface, "RealTile", (i-fx)*tile_size-8, (j-fy)*tile_size-8, land_map[0][i][j]);
	}
	else
	{
		for(int j=fy; j<ymax; j++)
		for(int i=fx; i<xmax; i++)
			m_ani.GetAni(zoomin*2)->Draw(dest_surface, tile1F[zoomin], (i-fx)*tile_size, (j-fy)*tile_size, land_map[0][i][j]);
	}
	//다른 부분 배경(맵의 튀어나온 부분 지우기)
	SetRect(&back_rect, 640, 0, 800, 480);
	jdd->DrawPicture(dest_surface, "Back", 640, 0, &back_rect);
	SetRect(&back_rect, 0, 480, 800, 600);
	jdd->DrawPicture(dest_surface, "Back", 0, 480, &back_rect);

	//오브젝트
	for(int j=fy; j<ymax; j++)
	for(int i=fx; i<xmax; i++)
	{
		if(land_map[1][i][j]!=0xff)m_ani.GetAni(zoomin*2+1)->Draw(dest_surface, tile2F[zoomin], (i-fx)*tile_size, (j-fy)*tile_size-(unit_ysize-tile_size), land_map[1][i][j]);
		if(land_map[2][i][j]!=0xff)m_ani.GetAni(zoomin*2+1)->Draw(dest_surface, unit2F[zoomin], (i-fx)*tile_size, (j-fy)*tile_size-(unit_ysize-tile_size), land_map[2][i][j]);
	}

	//이벤트
	for(int j=fy; j<ymax; j++)
	for(int i=fx; i<xmax; i++)
	{
		if(eventland_map[i][j]==0xff)continue;

		int tx=(i-fx)*tile_size;
		int ty=(j-fy)*tile_size;
		char* str_no = StrAdd("%d", eventland_map[i][j]);
		if(zoomin==1)
		{
			jdd->DrawText(dest_surface, str_no, font20, tx+1, ty+1, JColor(0, 0, 0));
			jdd->DrawText(dest_surface, str_no, font20, tx, ty, JColor(255, 0, 0));
		}
		else if(zoomin==2)
		{
			jdd->DrawText(dest_surface, str_no, font10, tx+1, ty+1, JColor(0, 0, 0));
			jdd->DrawText(dest_surface, str_no, font10, tx, ty, JColor(255, 0, 0));
		}
		else
		{
			jdd->DrawText(dest_surface, str_no, font5, tx+1, ty+1, JColor(0, 0, 0));
			jdd->DrawText(dest_surface, str_no, font5, tx, ty, JColor(255, 0, 0));
		}
	}

	//소스들
	jdd->DrawPicture(dest_surface, tile1F[1], 0, 480, NULL);
	jdd->DrawPicture(dest_surface, tile2F[1], 0, 512, NULL);
	jdd->DrawPicture(dest_surface, unit2F[1], 0, 552, NULL);

	//도구
	RECT src_rect;
	SetRect(&src_rect, 96, 0, 192, 96);
	jdd->DrawPicture(dest_surface, PIC_TOOLS, 640, 384, &src_rect);

	//확대율
	if(zoomin==1)
	{
		SetRect(&src_rect, 0, 0, 32, 32);
		jdd->DrawPicture(dest_surface, PIC_TOOLS, 640, 384, &src_rect);
	}
	else if(zoomin==2)
	{
		SetRect(&src_rect, 32, 0, 64, 32);
		jdd->DrawPicture(dest_surface, PIC_TOOLS, 672, 384, &src_rect);
	}
	else
	{
		SetRect(&src_rect, 64, 0, 96, 32);
		jdd->DrawPicture(dest_surface, PIC_TOOLS, 704, 384, &src_rect);
	}

	//에디트 레이어
	SetRect(&src_rect, edit_layer<<5, 32, (edit_layer<<5)+32, 64);
	jdd->DrawPicture(dest_surface, PIC_TOOLS, 640+(edit_layer<<5), 416, &src_rect);

	//도구
	SetRect(&src_rect, edit_tool<<5, 64, (edit_tool<<5)+32, 96);
	jdd->DrawPicture(dest_surface, PIC_TOOLS, 640+(edit_tool<<5), 448, &src_rect);
	SetRect(&src_rect, 0, 0, 40, (save_recent)?40:80);
	jdd->DrawPicture(dest_surface, PIC_SAVE, 760, 400, &src_rect);
	if(enable_undo)jdd->DrawPicture(dest_surface, PIC_UNDO, 672, 352, NULL);

	jdd->DrawPicture(dest_surface, PIC_MOVE, 640, 60, NULL);
	jdd->DrawPicture(dest_surface, PIC_SIZE, 725, 60, NULL);

	//선택한 타일
	if(selected[edit_layer]!=0xff)
	{
		if(edit_layer==0)
			jdd->DrawText(dest_surface, "L", font20, selected[0]<<5, 480, JColor(0,0,255));
		else
			jdd->DrawText(dest_surface, "L", font20, selected[edit_layer]<<5, 512+((edit_layer-1)*40), JColor(0,0,255));
	}

	//맵 테두리
	RECT map_rect;
	SetRect(&map_rect, 0, 0, Min(640, (x_size-fx)*tile_size), Min(480, (y_size-fy)*tile_size));
	int right=Min(640, (x_size-fx)*tile_size);
	int bottom=Min(480, (y_size-fy)*tile_size);

	if(fx==0)jdd->DrawLine(dest_surface, red_brush, 0, 0, 0, bottom);
	if(fy==0)jdd->DrawLine(dest_surface, red_brush, 0, 0, right, 0);

	if(fx+(20*zoomin)>=x_size)jdd->DrawLine(dest_surface, red_brush, right, 0, right, bottom);
	if(fy+(15*zoomin)>=y_size)jdd->DrawLine(dest_surface, red_brush, 0, bottom, right, bottom);

	//파일 관련 메뉴
	jdd->DrawPicture(dest_surface, "New", NEW_ICON_X, NEW_ICON_Y, NULL);
	jdd->DrawPicture(dest_surface, "Open", OPEN_ICON_X, OPEN_ICON_Y, NULL);
}

//맵 이동
void LoadMap()
{
	//맵 초기화
	if(!Load())
	{
		MapDataInit();
	}

	fx=fy=0;
	save_recent=true;
}

void ClearAll()
{
	//뒷정리
	ClearUndo();
	
	jdd->DeleteFont(global_font);
	jdd->DeleteFont(font20);
	jdd->DeleteFont(font10);
	jdd->DeleteFont(font5);
	jdd->DeleteBrush(red_brush);

	jdd->Cleanup();
	delete jdd;
	jdd=NULL;
}

//다이얼로그 창 닫는 처리
void CloseDialog(HWND hDlgWnd)
{
	ShowWindow(hDlgWnd, SW_HIDE);
	open_dialog = false;
	LButton = RButton = false;
	redraw = true;
}

//새로운 맵 만들기 속성창 메시지 처리
BOOL CALLBACK NewMapProc(HWND hNewMapwnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	char buffer[80];	//문자 버퍼
	HWND h_ok_button;		//완료 버튼 핸들

	switch (iMessage)
	{
		case WM_INITDIALOG:
		return TRUE;

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case IDOK:
			//맵 초기화
			MapDataInit();

			//값 가져오기
			GetDlgItemText(hNewMap, IDC_FILENAME, edit_file_name, 80);
			x_size = MaxMin(GetDlgItemInt(hNewMap, IDC_WIDTH, NULL, TRUE), 20, 80);
			y_size = MaxMin(GetDlgItemInt(hNewMap, IDC_HEIGHT, NULL, TRUE), 15, 60);
			fx = fy = 0;

			//윈도우 이름 변경
			sprintf(buffer, "%s - %s", BASE_WINDOW_NAME, edit_file_name);
			SetWindowText(hwnd, buffer);
			
			CloseDialog(hNewMapwnd);
			break;

			case IDCANCEL:
			CloseDialog(hNewMapwnd);
			break;

			case IDC_FILENAME:
			switch (HIWORD(wParam))
			{
				case EN_CHANGE :	//파일명이 없으면 OK버튼 비활성화, 아니면 활성화
				h_ok_button = GetDlgItem(hNewMap, IDOK);
				GetDlgItemText(hNewMap, IDC_FILENAME, buffer, 80);
				if(strcmp(buffer, "") == 0)
				{
					EnableWindow(h_ok_button, FALSE);
				}
				else
				{
					EnableWindow(h_ok_button, TRUE);
				}
				
				break;

			}
		}
		return TRUE;

		//창을 움직이면 밑의 그림을 다시 그림
		case WM_MOVE:
		redraw=true;
		return TRUE;

		//창 닫기
		case WM_CLOSE:
		CloseDialog(hNewMapwnd);
		return TRUE;
	}

	return FALSE;
}

//맵 불러오기 속성창 메시지 처리
BOOL CALLBACK LoadMapProc(HWND hLoadMapwnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	char window_name[80];	//윈도우 이름
	HWND hList;				//리스트 핸들
	HWND h_ok_button;		//완료 버튼 핸들
	int cursor;				//현재 선택된 파일 인덱스

	switch (iMessage)
	{
		case WM_INITDIALOG:
		return TRUE;

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//값 가져오기
			case IDOK:

			hList = GetDlgItem(hLoadMap, IDC_MAP_LIST);
			cursor = SendMessage(hList, LB_GETCURSEL, 0, 0);
			SendMessage(hList, LB_GETTEXT, cursor, (LPARAM)edit_file_name);

			LoadMap();

			sprintf(window_name, "%s - %s", BASE_WINDOW_NAME, edit_file_name);
			SetWindowText(hwnd, window_name);

			CloseDialog(hLoadMapwnd);
			break;

			case IDCANCEL:
			CloseDialog(hLoadMapwnd);
			break;

			case IDC_MAP_LIST:
			switch (HIWORD(wParam))
			{
				case LBN_SELCHANGE :
				h_ok_button = GetDlgItem(hLoadMap, IDOK);
				EnableWindow(h_ok_button, TRUE);
				break;

			}
			break;
		}
		return TRUE;

		//창을 움직이면 밑의 그림을 다시 그림
		case WM_MOVE:
		redraw=true;
		return TRUE;

		//창 닫기
		case WM_CLOSE:
		CloseDialog(hLoadMapwnd);
		return TRUE;
	}

	return FALSE;
}

//타일 속성창 메시지 처리
BOOL CALLBACK AttrDlgProc(HWND hAttrwnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hCombo;

	switch (iMessage)
	{
		case WM_INITDIALOG:
		return TRUE;

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//값 가져오기
			case IDOK:
			hCombo = GetDlgItem(hAttr, IDC_MOVE);
			tile_mov[layer_no][tile_no] = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			hCombo = GetDlgItem(hAttr, IDC_ATTR);
			tile_attr[layer_no][tile_no] = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			tile_damage[layer_no][tile_no] = GetDlgItemInt(hAttr, IDC_DAMAGE, NULL, TRUE);

			CloseDialog(hAttrwnd);
			break;

			case IDCANCEL:
			CloseDialog(hAttrwnd);
			break;
		}
		return TRUE;

		//창을 움직이면 밑의 그림을 다시 그림
		case WM_MOVE:
		redraw=true;
		return TRUE;

		//창 닫기
		case WM_CLOSE:
		CloseDialog(hAttrwnd);
		return TRUE;
	}

	return FALSE;
}

//유닛 속성창 메시지 처리
BOOL CALLBACK UnitAttrDlgProc(HWND hAttrwnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HWND hCombo;

	switch (iMessage)
	{
		case WM_INITDIALOG:
		return TRUE;

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//값 가져오기
			case IDOK:
			CUnitData* punit;
			punit = &unitdata[tile_no];

			punit->hp = GetDlgItemInt(hUnitAttr, IDC_HP, NULL, TRUE);
			punit->mp = GetDlgItemInt(hUnitAttr, IDC_MP, NULL, TRUE);
			punit->pow = GetDlgItemInt(hUnitAttr, IDC_POW, NULL, TRUE);
			punit->mpow = GetDlgItemInt(hUnitAttr, IDC_MPOW, NULL, TRUE);
			punit->def = GetDlgItemInt(hUnitAttr, IDC_DEF, NULL, TRUE);
			punit->mdef = GetDlgItemInt(hUnitAttr, IDC_MDEF, NULL, TRUE);
			punit->agl = GetDlgItemInt(hUnitAttr, IDC_AGL, NULL, TRUE);
			punit->mov = GetDlgItemInt(hUnitAttr, IDC_MOV, NULL, TRUE);
			punit->jump = GetDlgItemInt(hUnitAttr, IDC_JUMP, NULL, TRUE);
			punit->swim = GetDlgItemInt(hUnitAttr, IDC_SWIM, NULL, TRUE);
			punit->see = GetDlgItemInt(hUnitAttr, IDC_SEE, NULL, TRUE);
			punit->act = GetDlgItemInt(hUnitAttr, IDC_ACT, NULL, TRUE);
			
			hCombo = GetDlgItem(hUnitAttr, IDC_ATTR);
			SendMessage(hCombo, CB_GETCURSEL, punit->attr, 0);

			CloseDialog(hAttrwnd);
			break;

			case IDCANCEL:
			CloseDialog(hAttrwnd);
			break;
		}
		return TRUE;

		//창을 움직이면 밑의 그림을 다시 그림
		case WM_MOVE:
		redraw=true;
		return TRUE;

		//창 닫기
		case WM_CLOSE:
		CloseDialog(hAttrwnd);
		return TRUE;
	}

	return FALSE;
}

//유닛 속성창 메시지 처리
BOOL CALLBACK EventIDDlgProc(HWND hEventIDwnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch (iMessage)
	{
		case WM_INITDIALOG:
		return TRUE;

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//값 가져오기
			case IDOK:
			eventland_map[selected_event_x][selected_event_y] =  MaxMin(GetDlgItemInt(hEventID, IDC_EVENT_ID, NULL, TRUE), 0, 255);

			CloseDialog(hEventIDwnd);
			break;

			case IDCANCEL:
			CloseDialog(hEventIDwnd);
			break;
		}
		return TRUE;

		//창을 움직이면 밑의 그림을 다시 그림
		case WM_MOVE:
		redraw=true;
		return TRUE;

		//창 닫기
		case WM_CLOSE:
		CloseDialog(hEventIDwnd);
		return TRUE;
	}

	return FALSE;
}

void LoadResource()
{
	//투명색
	JPictureInfo jpi;
	jpi.SetColorKey(JColor(0,0,255));

	//Common
	jdd->LoadPicture("CommonSelect", "DATA\\CommonSelect.gif", NULL, TRUE);
	jdd->LoadPicture("CommonDlg1", "DATA\\CommonDlg1.gif", NULL, TRUE);
	jdd->LoadPicture("CommonDlg2", "DATA\\CommonDlg2.gif", NULL, TRUE);
	jdd->LoadPicture("CommonDlg3", "DATA\\CommonDlg3.gif", NULL, TRUE);
	jdd->LoadPicture("CommonDlg4", "DATA\\CommonDlg4.gif", NULL, TRUE);
	jdd->LoadPicture("CommonDlg5", "DATA\\CommonDlg5.gif", NULL, TRUE);
	jdd->LoadPicture("CommonDlg6", "DATA\\CommonDlg6.gif", NULL, TRUE);
	jdd->LoadPicture("CommonDlg7", "DATA\\CommonDlg7.gif", NULL, TRUE);
	jdd->LoadPicture("CommonDlg8", "DATA\\CommonDlg8.gif", NULL, TRUE);
	jdd->LoadPicture("CommonDlg9", "DATA\\CommonDlg9.gif", NULL, TRUE);

	//MapEditor
	jdd->LoadPicture("Undo", "DATA\\Undo.jpg", NULL, TRUE);
	jdd->LoadPicture("Back", "DATA\\Back.jpg", NULL, TRUE);
	jdd->LoadPicture("ClearSave", "DATA\\ClearSave.jpg", NULL, TRUE);
	jdd->LoadPicture("Move", "DATA\\Move.gif", &jpi, TRUE);
	jdd->LoadPicture("Size", "DATA\\Size.gif", &jpi, TRUE);
	jdd->LoadPicture("Tools", "DATA\\Tools.jpg", NULL, TRUE);
	
	//파일 메뉴
	jdd->LoadPicture("New", "DATA\\New.gif", NULL, TRUE);
	jdd->LoadPicture("Open", "DATA\\Open.gif", NULL, TRUE);

	//타일
	CreateTile();	//1층 타일
	jdd->LoadPicture(tile2F[1], "User\\tile2.gif", &jpi, TRUE);	//2층 타일
	CreateUnit();	//유닛

	CreateSmallTile(tile1F[2], tile2F[2], unit2F[2], 2);	// 1/2 타일
	CreateSmallTile(tile1F[4], tile2F[4], unit2F[4], 4);	// 1/4 타일

	//타일 애니메이션
	m_ani.CreateAnimation(0, 48, 48);

	m_ani.CreateAnimation(2, 32, 32);
	m_ani.CreateAnimation(3, 32, 40);

	m_ani.CreateAnimation(4, 16, 16);
	m_ani.CreateAnimation(5, 16, 20);

	m_ani.CreateAnimation(8, 8, 8);
	m_ani.CreateAnimation(9, 8, 10);

	//프레임 버퍼
	JPictureInfo fpi;
	fpi.SetWidth(800);
	fpi.SetHeight(600);
	strcpy(dest_surface, "FrameBuffer");
	jdd->CreateSurface(dest_surface, &fpi, true);
}

//새로운 맵 만들기 다이얼로그
void InitNewMapDialog(HINSTANCE hInstance)
{
	hNewMap = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_NEW_MAP), hwnd, NewMapProc);
	RECT m_rcClient;
	GetWindowRect(hwnd, &m_rcClient);
	SetWindowPos(hNewMap,NULL,m_rcClient.left+150,m_rcClient.top+50,0,0,SWP_NOZORDER|SWP_NOSIZE);
	ShowWindow(hNewMap,SW_HIDE);
}

//맵 불러오기 다이얼로그
void InitLoadMapDialog(HINSTANCE hInstance)
{
	hLoadMap = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_OPEN_MAP), hwnd, LoadMapProc);
	RECT m_rcClient;
	GetWindowRect(hwnd, &m_rcClient);
	SetWindowPos(hLoadMap,NULL,m_rcClient.left+150,m_rcClient.top+50,0,0,SWP_NOZORDER|SWP_NOSIZE);
	ShowWindow(hLoadMap,SW_HIDE);
}

//타일 속성 다이얼로그
void InitTileDialog(HINSTANCE hInstance)
{
	hAttr = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_TILEATTR), hwnd, AttrDlgProc);
	RECT m_rcClient;
	GetWindowRect(hwnd, &m_rcClient);
	SetWindowPos(hAttr,NULL,m_rcClient.left+50,m_rcClient.top+50,0,0,SWP_NOZORDER|SWP_NOSIZE);
	ShowWindow(hAttr,SW_HIDE);

	TCHAR *moves[]={TEXT("이동불가"),TEXT("빙판"),TEXT("길"),TEXT("진흙"),TEXT("늪")};
	TCHAR *attrs[]={TEXT("없음"),TEXT("물"),TEXT("함정")};
	HWND hCombo = GetDlgItem(hAttr, IDC_MOVE);
	for(int i=0; i<5; ++i)SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)moves[i]);
	hCombo = GetDlgItem(hAttr, IDC_ATTR);
	for(int i=0; i<3; ++i)SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)attrs[i]);
}

//유닛 속성 다이얼로그
void InitUnitDialog(HINSTANCE hInstance)
{
	hUnitAttr = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_UNITATTR), hwnd, UnitAttrDlgProc);
	RECT m_rcClient;
	GetWindowRect(hwnd, &m_rcClient);
	SetWindowPos(hUnitAttr,NULL,m_rcClient.left+150,m_rcClient.top+50,0,0,SWP_NOZORDER|SWP_NOSIZE);
	ShowWindow(hUnitAttr,SW_HIDE);

	TCHAR *unitattrs[]={TEXT("보통"),TEXT("유격대"),TEXT("수영"),TEXT("헤엄"),TEXT("유령")};
	HWND hCombo = GetDlgItem(hUnitAttr, IDC_ATTR);
	for(int i=0; i<5; ++i)SendMessage(hCombo,CB_ADDSTRING,0,(LPARAM)unitattrs[i]);
}

//이벤트ID 다이얼로그
void InitEventIDDialog(HINSTANCE hInstance)
{
	hEventID = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_EVENT_ID), hwnd, EventIDDlgProc);
	RECT m_rcClient;
	GetWindowRect(hwnd, &m_rcClient);
	SetWindowPos(hEventID,NULL,m_rcClient.left + 150,m_rcClient.top + 50, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
	ShowWindow(hEventID,SW_HIDE);
}

//속성값 불러오기
void LoadAttribute()
{
	FILE* fp;
	BOOL read_success = FALSE;
	if(fp=fopen("MAP\\map_attr.sav", "rb"))
	{
		//식별자
		char head[9];
		fread(head, sizeof(char), 8, fp);
		head[8] = NULL;
		if(strcmp(head, "MAPATTR0")==0)
		{
			//타일 속성값
			for(int i=0; i<2; ++i)
			{
				fread(tile_mov[i], sizeof(int), TILE_MAX, fp);
				fread(tile_attr[i], sizeof(int), TILE_MAX, fp);
				fread(tile_damage[i], sizeof(int), TILE_MAX, fp);
			}
			//유닛 속성값
			fread(unitdata, sizeof(CUnitData), TILE_MAX, fp);

			read_success = TRUE;
		}

		fclose(fp);
	}

	//읽기 실패하면 초기화
	if(!read_success)
	{
		for(int j=0; j<2; ++j)
		for(int i=0; i<TILE_MAX; ++i)
			tile_mov[j][i]=tile_attr[j][i]=tile_damage[j][i]=0;
	}
}

//속성값 저장
void SaveAttribute()
{
	//속성값 저장
	FILE *fp;
	if(fp=fopen("MAP\\map_attr.sav", "wb"))
	{
		//식별자
		char* head = "MAPATTR0";
		fwrite(head, sizeof(char), 8, fp);
		//타일 속성값
		for(int i=0; i<2; ++i)
		{
			fwrite(tile_mov[i], sizeof(int), TILE_MAX, fp);
			fwrite(tile_attr[i], sizeof(int), TILE_MAX, fp);
			fwrite(tile_damage[i], sizeof(int), TILE_MAX, fp);
		}
		//유닛 속성값
		fwrite(unitdata, sizeof(CUnitData), TILE_MAX, fp);

		fclose(fp);
	}
}

//////////////////////////////////////////////////
//메인 코드
//////////////////////////////////////////////////
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{	
	//창 생성
	char window_name[80];
	sprintf(window_name, "%s - %s", BASE_WINDOW_NAME, edit_file_name);
	if(!MainInitialize(window_name, TRUE, TRUE, window_mode))return 0;
	
	//다이얼로그창 초기화
	InitNewMapDialog(hInstance);
	InitLoadMapDialog(hInstance);
	InitTileDialog(hInstance);
	InitUnitDialog(hInstance);
	InitEventIDDialog(hInstance);

	//속성 정보 불러오기
	LoadAttribute();

	//윈도우창 이동
	if(window_mode)jdd->OnMove(CW_USEDEFAULT, CW_USEDEFAULT);

	//초기화
	font20=jdd->CreateFont("굴림체",20,true,false,false,false,false);
	font10=jdd->CreateFont("굴림체",10,true);
	font5=jdd->CreateFont("굴림체",5);
	
	//Undo
	bool mouse_click = false;
	bool rmouse_click = false;
	enable_undo = false;

	//그림 불러오기
	LoadResource();

	//맵 불러오기 또는 초기화
	int click_delay=DELAY_MAX;
	LoadMap();

	tile_size = 32 / zoomin;
	unit_ysize = 40 / zoomin;

	//화면 그리기
	redraw = true;
	bool action = false;
	red_brush = jdd->CreateBrush(JColor(255, 0, 0));

	SetCursor(LoadCursor(0 , IDC_ARROW));

	while(!GetKey(vkey_esc))
	{
		if(!ProcessMessage())break;

		//크기 조절
		if(zoomin == 4)
		{
			if(GetKey(vkey_up, 10))
			{
				SAVE_UNDO;
				y_size=Max(y_size-1, 15);
				action = redraw = true;
			}
			else if(GetKey(vkey_down, 10))
			{
				SAVE_UNDO;
				y_size=Min(y_size+1, 60);
				action = redraw = true;
			}
			if(GetKey(vkey_left, 10))
			{
				SAVE_UNDO;
				x_size=Max(x_size-1, 20);
				action = redraw = true;
			}
			else if(GetKey(vkey_right, 10))
			{
				SAVE_UNDO;
				x_size=Min(x_size+1, 80);
				action = redraw = true;
			}
		}
		//이동
		else
		{
			if(GetKey(vkey_up, 1))
			{
				fy=Max(fy-1, 0);
				redraw = true;
			}
			else if(GetKey(vkey_down, 1))
			{
				if(fy+1<=y_size-15*zoomin)++fy;
				redraw = true;
			}
			if(GetKey(vkey_left, 1))
			{
				fx=Max(fx-1, 0);
				redraw = true;
			}
			else if(GetKey(vkey_right, 1))
			{
				if(fx+1<=x_size-20*zoomin)++fx;
				redraw = true;
			}
		}

		//마우스 왼쪽 버튼 이벤트
		if(LButton && !open_dialog)
		{
			int tx=MouseX/tile_size+fx;
			int ty=MouseY/tile_size+fy;

			//그리기
			if(MouseX<640 && MouseY<480)
			{
				if(tx < x_size && ty < y_size)
				{
					SAVE_UNDO;

					//하나 그리기
					if(edit_tool==0)
					{
						land_map[edit_layer][tx][ty]=selected[edit_layer];
						if(edit_layer==2)eventland_map[tx][ty]=selected[edit_layer];
					}
					//페인트(이벤트 경우에는 편집)
					else if(edit_tool==1)
					{
						if(!mouse_click)
						{
							Paint(tx, ty);
						}
					}
					//지우기
					else
					{
						if(edit_layer==0)land_map[edit_layer][tx][ty]=0;
							else land_map[edit_layer][tx][ty]=0xff;

						//이벤트까지 지우기
						if(edit_layer==2)eventland_map[tx][ty]=0xff;
					}

					action = redraw = true;
				}
			}
			//소스 선택
			else if(MouseY>=480 && MouseY<592)
			{
				if(!mouse_click)
				{
					if(MouseY<512)edit_layer=0;
						else if(MouseY<552)edit_layer=1;
						else edit_layer=2;

					selected[edit_layer]=MouseX>>5;

					redraw = true;
				}
			}
			//편집 메뉴
			else if(MouseX>=640 && MouseX<736 && MouseY>=384 && MouseY<480)
			{
				if(!mouse_click)
				{
					//확대, 축소
					if(MouseY<416)
					{
						int old_zoomin=zoomin;

						if(MouseX<672)zoomin=1;
							else if(MouseX<704)zoomin=2;
							else zoomin=4;

						if(old_zoomin != zoomin)
						{
							//이전의 중심점을 지금의 배율에 맞게 이동
							int cx = fx+(10*old_zoomin);
							int cy = fy+(7*old_zoomin);
							fx=MaxMin(cx-(10*zoomin), 0, Max(x_size-20*zoomin, 0));
							fy=MaxMin(cy-(7*zoomin), 0, Max(y_size-15*zoomin, 0));
						
							tile_size=32/zoomin;
							unit_ysize=40/zoomin;
						}
					}
					//편집 레이어
					else if(MouseY<448)
					{
						edit_layer=(MouseX-640)>>5;
					}
					//도구선택
					else
					{
						edit_tool=(MouseX-640)>>5;
					}

					redraw=true;
				}
			}
			//Clear
			else if(MouseX>=760 && MouseY>=400 && MouseY<440)
			{
				if(!mouse_click)
				{
					SAVE_UNDO;

					int clear_tile;
					if(edit_layer > 0)clear_tile = 0xff;
						else clear_tile = selected[edit_layer];

					for(int j=0; j<60; j++)
					for(int i=0; i<80; i++)
					{
						land_map[edit_layer][i][j]=clear_tile;
						if(edit_layer==2)eventland_map[i][j]=0xff;
					}

					action = redraw = true;
				}
			}
			//Save
			else if(MouseX>=760 && MouseY>=440 && MouseY<480)
			{
				if(!save_recent && !mouse_click)
				{
					Save();

					save_recent=true;
					redraw=true;
				}
			}
			//Undo
			else if(MouseX>=672 && MouseX<704 && MouseY>=352 && MouseY<384)
			{
				//Undo데이터 읽기
				if(!mouse_click && gp_undo != NULL)
				{
					gp_undo = ReadUndo(gp_undo);
					if(gp_undo == NULL)enable_undo=false;
					redraw=true;
				}
			}
			//새로운 맵
			else if(MouseX >= NEW_ICON_X && MouseX < NEW_ICON_X + FILE_ICON_WIDTH 
				&& MouseY >= NEW_ICON_Y && MouseY < NEW_ICON_Y + FILE_ICON_HEIGHT)
			{
				//저장하지 않고 새로운 맵을 만들 때
				if(!save_recent)
				{
					if(MessageBox(hwnd, "작업 중인 맵을 저장하지 않았습니다.\n저장하시겠습니까?", "경고", MB_YESNO) == IDYES)
					{
						Save();
						save_recent=true;
					}
				}

				//OK버튼은 처음에는 비활성화
				HWND h_ok_button = GetDlgItem(hNewMap, IDOK);
				EnableWindow(h_ok_button, FALSE);

				//파일이름 비워두기
				SetDlgItemText(hNewMap, IDC_FILENAME, "");

				//윈도우 보이기
				ShowWindow(hNewMap,SW_SHOW);

				redraw = true;
				open_dialog = true;
			}
			//맵 불러오기
			else if(MouseX >= OPEN_ICON_X && MouseX < OPEN_ICON_X + FILE_ICON_WIDTH 
				&& MouseY >= OPEN_ICON_Y && MouseY < OPEN_ICON_Y + FILE_ICON_HEIGHT)
			{
				//저장하지 않고 불러 올 때
				if(!save_recent)
				{
					if(MessageBox(hwnd, "작업 중인 맵을 저장하지 않았습니다.\n저장하시겠습니까?", "경고", MB_YESNO) == IDYES)
					{
						Save();
						save_recent=true;
					}
				}

				HWND hList = GetDlgItem(hLoadMap, IDC_MAP_LIST);
				CFiles* files = new CFiles();
				files->SearchFile("MAP", "map");

				//기존의 스트링 모두 삭제
				while(SendMessage(hList, LB_GETCOUNT, 0, 0) > 0)
				{
					SendMessage(hList, LB_DELETESTRING, 0, 0);
				}

				//파일 추가
				std::vector<char*>::iterator it;
				for(it=files->filename.begin(); it!=files->filename.end(); ++it)
				{
					char filename[80];
					strcpy(filename, (*it));

					filename[strlen(filename) - 4] = NULL;
					SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)filename);
				}

				//OK버튼은 처음에는 비활성화
				HWND h_ok_button = GetDlgItem(hLoadMap, IDOK);
				EnableWindow(h_ok_button, FALSE);

				//윈도우 보이기
				ShowWindow(hLoadMap, SW_SHOW);

				redraw = true;
				
				//파일 찾기 클래스 릴리즈
				delete files;
				files = NULL;
				
				open_dialog = true;
			}
			//맵 이동
			else if(MouseY>=60 && MouseY<85 && MouseX>=665 && MouseX<690)
			{
				fy=Max(fy-1, 0);
				redraw = true;
			}
			else if(MouseY>=85 && MouseY<110 && MouseX>=640 && MouseX<665)
			{
				fx=Max(fx-1, 0);
				redraw = true;
			}
			else if(MouseY>=85 && MouseY<110 && MouseX>=690 && MouseX<715)
			{
				if(fx+1<=x_size-20*zoomin)++fx;
				redraw = true;
			}
			else if(MouseY>=110 && MouseY<135 && MouseX>=665 && MouseX<690)
			{
				if(fy+1<=y_size-15*zoomin)++fy;
				redraw = true;
			}
			//사이즈 조절
			else if(MouseY>=60 && MouseY<85 && MouseX>=750 && MouseX<775)
			{
				SAVE_UNDO;
				++click_delay;
				if(click_delay>DELAY_MAX)
				{
					y_size=Max(y_size-1, 15);
					redraw = true;
					click_delay=0;
				}
				action=true;
			}
			else if(MouseY>=85 && MouseY<110 && MouseX>=725 && MouseX<750)
			{
				SAVE_UNDO;
				++click_delay;
				if(click_delay>DELAY_MAX)
				{
					x_size=Max(x_size-1, 20);
					redraw = true;
					click_delay=0;
				}
				action=true;
			}
			else if(MouseY>=85 && MouseY<110 && MouseX>=775)
			{
				SAVE_UNDO;
				++click_delay;
				if(click_delay>DELAY_MAX)
				{
					x_size=Min(x_size+1, 80);
					redraw = true;
					click_delay=0;
				}
				action=true;
			}
			else if(MouseY>=110 && MouseY<135 && MouseX>=750 && MouseX<775)
			{
				SAVE_UNDO;
				++click_delay;
				if(click_delay>DELAY_MAX)
				{
					y_size=Min(y_size+1, 60);
					redraw = true;
					click_delay=0;
				}
				action=true;
			}

			//누른 순간 이벤트
			if(!mouse_click)
			{
				SetCapture(hwnd);
				mouse_click=true;
			}
		}
		//누르지 않을 때 이벤트
		else
		{
			//액션
			if(action)
			{
				//Undo데이터 중복 확인
				if(gp_undo != NULL && !CheckDataChange(gp_undo))
				{
					gp_undo = DeleteUndoData(gp_undo);
				}
				else
				{
					save_recent=false;
				}
				
				if(gp_undo!=NULL)
				{
					enable_undo=true;
				}

				action=false;
				redraw=true;
			}

			//놓는 순간 이벤트
			if(mouse_click)
			{
				ReleaseCapture();
				mouse_click = false;
			}
		}

		//마우스 오른쪽 버튼
		if(RButton && !open_dialog)
		{
			if(!rmouse_click)
			{
				rmouse_click = true;

				//타일 속성 정의
				if(MouseY>=480 && MouseY<552)
				{
					if(MouseY<512)layer_no=0;
						else if(MouseY<552)layer_no=1;

					tile_no=MouseX>>5;

					//타일 종류
					if(layer_no==0)SetDlgItemText(hAttr, IDC_LAYER, "바닥");
					else if(layer_no==1)SetDlgItemText(hAttr, IDC_LAYER, "물체");

					//타일 번호
					SetDlgItemInt(hAttr, IDC_TILENO, tile_no, TRUE);

					//속성값 넘기기
					HWND hCombo = GetDlgItem(hAttr, IDC_MOVE);
					SendMessage(hCombo, CB_SETCURSEL, tile_mov[layer_no][tile_no], 0);
					hCombo = GetDlgItem(hAttr, IDC_ATTR);
					SendMessage(hCombo, CB_SETCURSEL, tile_attr[layer_no][tile_no], 0);
					SetDlgItemInt(hAttr, IDC_DAMAGE, tile_damage[layer_no][tile_no], TRUE);

					ShowWindow(hAttr,SW_SHOW);

					redraw = true;
					open_dialog = true;
				}
				//유닛 속성 정의
				else if(MouseY>=552 && MouseY<592)
				{
					tile_no=MouseX>>5;

					//타일 번호
					SetDlgItemInt(hUnitAttr, IDC_TILENO, tile_no, TRUE);

					//속성값 넘기기
					SetDlgItemInt(hUnitAttr, IDC_HP, unitdata[tile_no].hp, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_MP, unitdata[tile_no].mp, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_POW, unitdata[tile_no].pow, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_MPOW, unitdata[tile_no].mpow, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_DEF, unitdata[tile_no].def, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_MDEF, unitdata[tile_no].mdef, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_AGL, unitdata[tile_no].agl, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_MOV, unitdata[tile_no].mov, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_JUMP, unitdata[tile_no].jump, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_SWIM, unitdata[tile_no].swim, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_SEE, unitdata[tile_no].see, TRUE);
					SetDlgItemInt(hUnitAttr, IDC_ACT, unitdata[tile_no].act, TRUE);
					
					HWND hCombo = GetDlgItem(hUnitAttr, IDC_ATTR);
					SendMessage(hCombo, CB_SETCURSEL, unitdata[tile_no].attr, 0);

					ShowWindow(hUnitAttr,SW_SHOW);

					redraw = true;
					open_dialog = true;
				}
				//이벤트 번호 변경
				else if(MouseX<640 && MouseY<480)
				{
					if(!open_dialog)
					{
						int tx = MouseX / tile_size + fx;
						int ty = MouseY / tile_size + fy;

						if(tx < x_size && ty < y_size)
						{
							SAVE_UNDO;

							SetDlgItemInt(hEventID, IDC_EVENT_ID, eventland_map[tx][ty], TRUE);

							selected_event_x = tx;
							selected_event_y = ty;

							redraw = true;
							open_dialog = true;

							ShowWindow(hEventID, SW_SHOW);
						}
					}
				}
			}
		}
		else
		{
			rmouse_click = false;			
		}

		//다시 그리기
		if(redraw)
		{
			RedrawAll();
			redraw=false;
			jdd->DrawPicture(backbuffer, dest_surface, 0, 0, NULL);
		}

		jdd->Render();
	}

	ClearAll();
	SaveAttribute();

	return 0;
}

//윈도우 메세지 처리
LRESULT CALLBACK WndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    switch ( msg )
    {
		case WM_LBUTTONDOWN  :	if(mouse_control)LButton=true;
								break;

		case WM_RBUTTONDOWN  :	if(mouse_control)RButton=true;
								break;

		case WM_LBUTTONUP    :	if(mouse_control)LButton=false;
								break;

		case WM_RBUTTONUP    :	if(mouse_control)RButton=false;
								break;

		case WM_MOUSEMOVE    :	if(mouse_control)
								{
									MouseX=LOWORD(lParam);
									MouseY=HIWORD(lParam);
								}
								break;

		case MM_MCINOTIFY    :	if ( ReplayFlag && wParam == MCI_NOTIFY_SUCCESSFUL )_MidiReplay();
								break;

		case WM_DESTROY 	 :	_CrtDumpMemoryLeaks();
								break;

		case WM_CLOSE		 :	gameover=TRUE;
								break;

		case WM_SIZE		 :	if(wParam == SIZE_MINIMIZED)
								{
									activate=false;
								}
								else
								{
									activate=true;
								}
								break;

		case WM_MOVE		 :	if(jdd)
								{
									jdd->OnMove(LOWORD(lParam), HIWORD(lParam));
								}
								break;

		case WM_PAINT		 :	if(jdd)
								{
									jdd->DrawPicture(backbuffer, dest_surface, 0, 0, NULL);
									jdd->Render();
								}
								break;
		

		case WM_ACTIVATE	 : if(LOWORD(wParam))
							   {
								   activate=true;
							   }
							   else
							   {
								   activate=false;
							   }

							   break;
	}

	return DefWindowProc(wnd,msg,wParam,lParam);
}