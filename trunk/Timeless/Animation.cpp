#include <windows.h>

#include "Animation.h"
#include "Global.h"

//CAnimation 메소드
CAnimationData::CAnimationData(int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	Set(x, y, vtype, vframe_max, vdelay_max);
	frame=delay=y_point=0;
	walk=FALSE;
}

//설정 : X크기, Y크기, 애니메이션타입, 그림 수, 지연시간
void CAnimationData::Set(int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	x_size=x;
	y_size=y;
	SetRect(&ani_rect, 0, 0, x_size, y_size);
	type=vtype;
	if(type==2)frame_max=(vframe_max-1)*2;
		else frame_max=vframe_max;
	delay_max=vdelay_max;
}

//애니메이션 처리를 해줌(이것을 해주지 않으면 자동으로 애니메이션이 되지 않음)
void CAnimationData::Process()
{
	if(type==ani_loop)	//일직선 반복 애니메이션
	{
		++delay;
		if(delay>=delay_max)
		{
			delay=0;
			++frame;
			if(frame>=frame_max)frame=0;
		}
		
		SetRect(&ani_rect, x_size*frame, y_point, x_size*(frame+1), y_point+y_size);
	}
	else if(type==ani_exchange)	//왔다갔다 반복 애니메이션
	{
		++delay;
		if(delay>=delay_max)
		{
			delay=0;
			++frame;
			if(frame>=frame_max)frame=0;
		}
		int frame2=abs((frame_max/2)-frame);
		SetRect(&ani_rect, x_size*frame2, y_point, x_size*(frame2+1), y_point+y_size);
	}
	if(type==ani_once)	//일직선 한번 애니메이션
	{
		++delay;
		if(delay>=delay_max)
		{
			delay=0;
			++frame;
			if(frame>=frame_max)frame=frame_max-1;
		}
		
		SetRect(&ani_rect, x_size*frame, y_point, x_size*(frame+1), y_point+y_size);
	}
}

//애니메이션을 시작점으로
void CAnimationData::Reset()
{
	SetFrame(0);
	delay=0;
}

//상하좌우 애니메이션을 하는 경우 Y좌표를 바꿔서 애니메이션
void CAnimationData::SetWalk(int up_ypos, int down_ypos, int left_ypos, int right_ypos)
{
	up=up_ypos;
	down=down_ypos;
	left=left_ypos;
	right=right_ypos;
	
	walk=TRUE;
}

//장면 정하기
void CAnimationData::SetFrame(int vframe)
{
	frame=vframe;
	if(type==ani_exchange)	//왔다갔다 반복 애니메이션
	{
		int frame2=abs((frame_max/2)-frame);
		SetRect(&ani_rect, x_size*frame2, y_point, x_size*(frame2+1), y_point+y_size);
	}
	else //기타
	{
		SetRect(&ani_rect, x_size*frame, y_point, x_size*(frame+1), y_point+y_size);
	}
}

//X,Y 값 가져오기
int CAnimationData::GetXSize()
{
	return x_size;
}

int CAnimationData::GetYSize()
{
	return y_size;
}

void CAnimationData::SetDirection(WalkDirection dir)
{
	if(!walk)return;

	//방향 설정
	if(dir==dir_up)y_point=up;
	else if(dir==dir_left)y_point=left;
	else if(dir==dir_down)y_point=down;
	else if(dir==dir_right)y_point=right;
	//반대로 돌기
	else if(dir==dir_turn)
	{
		if(dir==dir_up)y_point=down;
		else if(dir==dir_left)y_point=right;
		else if(dir==dir_down)y_point=up;
		else if(dir==dir_right)y_point=left;
	}
	//왼쪽으로 돌기
	else if(dir==dir_turn_left)
	{
		if(dir==dir_up)y_point=left;
		else if(dir==dir_left)y_point=down;
		else if(dir==dir_down)y_point=right;
		else if(dir==dir_right)y_point=up;
	}
	//오른쪽으로 돌기
	else if(dir==dir_turn_right)
	{
		if(dir==dir_up)y_point=right;
		else if(dir==dir_left)y_point=up;
		else if(dir==dir_down)y_point=left;
		else if(dir==dir_right)y_point=down;
	}
	//아무데나 보기
	else if(dir==dir_random)
	{
		int wd=rand()%4;
		if(wd==0)y_point=up;
		else if(wd==1)y_point=left;
		else if(wd==2)y_point=down;
		else if(wd==3)y_point=right;
	}

	SetFrame(frame);
}

void CAnimationData::Draw(char* dest, char* src, int x, int y, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPicture(dest, src, x, y, &ani_rect);
}

void CAnimationData::Draw(int dest, int src, int x, int y, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPicture(dest, src, x, y, &ani_rect);
}

void CAnimationData::DrawEx(char* dest, char* src, int x, int y, DWORD dwFlags, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPictureEx(dest, src, x, y, &ani_rect, dwFlags);
}

void CAnimationData::DrawEx(int dest, int src, int x, int y, DWORD dwFlags, int frame)
{
	if(frame>=0)SetFrame(frame);
	jdd->DrawPictureEx(dest, src, x, y, &ani_rect, dwFlags);
}

void CAnimation::CreateAnimation(int id, int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	//만약 이미 생성되어 있다면 지우기
	DeleteAnimation(id);

	anidata[id] = new CAnimationData(x, y, vtype, vframe_max, vdelay_max);
}

void CAnimation::DeleteAnimation(int id)
{
	if(anidata[id] != NULL)
	{
		delete anidata[id];
		anidata[id] = NULL;
	}
}

void CAnimation::Process()
{
	//자동 애니메이션
	std::map<int, CAnimationData*>::iterator it;
	for(it = anidata.begin(); it != anidata.end(); ++it)
	{
		it->second->Process();
	}
}

CAnimationData* CAnimation::GetAni(int id)
{
	if(anidata[id] == NULL)
	{
		OutputDebugString("잘못된 애니메이션 ID를 요청하셨습니다.\n");
		exit(1);
		//OutputDebugString("에러를 막기 위해 임시 애니메이션 정보를 생성해서 보냅니다.\n");
		//anidata[id]=new CAnimationData(10, 10, ani_normal, 1, 1);
		//anidata[id] = new CAnimationData(640, 480, ani_exchange, 3, 1000);
	}

	return anidata[id];
}

CAnimation::CAnimation()
{
}

CAnimation::~CAnimation()
{
	std::map<int, CAnimationData*>::iterator it;
	for(it = anidata.begin(); it != anidata.end(); ++it)
	{
		if((it->second) != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	anidata.clear();
}