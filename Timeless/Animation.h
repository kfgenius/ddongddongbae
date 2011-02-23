#pragma once

#include <map>

using namespace std;

//애니메이션 데이터 클래스
enum AniType	//애니메이션 타입
{
	ani_normal,
	ani_loop,
	ani_exchange,
	ani_once
};

enum WalkDirection	//걷는 방향
{
	dir_up,
	dir_left,
	dir_down,
	dir_right,
	dir_turn,
	dir_turn_left,
	dir_turn_right,
	dir_random
};

class CAnimationData
{
private:
	//공통 값
	int x_size, y_size;
	BOOL walk;					//보행 여부(기본값: FALSE)
	int up, down, left, right;	//보행 그래픽을 위한 상하좌우 애니메이션 그림의 위치(Y좌표를 증가)
	char type;
	int frame_max;
	int delay_max;

	//자신 만의 값
	int frame;
	int delay;
	int y_point;

public:
	RECT ani_rect;

	void Set(int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
	void Process();
	void SetFrame(int vframe);
	void Reset();		//애니메이션을 시작점으로
	int GetXSize();
	int GetYSize();

	//상하좌우 걷기 관련
	void SetWalk(int up_ypos, int down_ypos, int left_ypos, int right_ypos);
	void SetDirection(WalkDirection dir);

	//그리기
	void Draw(char* dest, char* src, int x, int y, int frame=-1);
	void Draw(int dest, int src, int x, int y, int frame=-1);
	void DrawEx(char* dest, char* src, int x, int y, DWORD dwFlags, int frame=-1);
	void DrawEx(int dest, int src, int x, int y, DWORD dwFlags, int frame=-1);

	//생성자
	CAnimationData(int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
};

//애니메이션 관리 클래스
class CAnimation
{
private:
	map<int, CAnimationData*> anidata;

public:
	void CreateAnimation(int id, int x, int y, AniType vtype=ani_normal, int vframe_max=0, int vdelay_max=0);
	void Process();	//애니메이션을 진행하는 함수(이것을 실행하지 않으면 움직이지 않음)
	CAnimationData* GetAni(int id);

	//생성자 소멸자
	CAnimation();
	~CAnimation();
};