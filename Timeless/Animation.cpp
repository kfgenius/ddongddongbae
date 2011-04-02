#include <windows.h>

#include "Animation.h"
#include "Global.h"

//CAnimation �޼ҵ�
CAnimationData::CAnimationData(int x, int y, AniType vtype, int vframe_max, int vdelay_max)
{
	Set(x, y, vtype, vframe_max, vdelay_max);
	frame=delay=y_point=0;
	walk=FALSE;
}

//���� : Xũ��, Yũ��, �ִϸ��̼�Ÿ��, �׸� ��, �����ð�
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

//�ִϸ��̼� ó���� ����(�̰��� ������ ������ �ڵ����� �ִϸ��̼��� ���� ����)
void CAnimationData::Process()
{
	if(type==ani_loop)	//������ �ݺ� �ִϸ��̼�
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
	else if(type==ani_exchange)	//�Դٰ��� �ݺ� �ִϸ��̼�
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
	if(type==ani_once)	//������ �ѹ� �ִϸ��̼�
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

//�ִϸ��̼��� ����������
void CAnimationData::Reset()
{
	SetFrame(0);
	delay=0;
}

//�����¿� �ִϸ��̼��� �ϴ� ��� Y��ǥ�� �ٲ㼭 �ִϸ��̼�
void CAnimationData::SetWalk(int up_ypos, int down_ypos, int left_ypos, int right_ypos)
{
	up=up_ypos;
	down=down_ypos;
	left=left_ypos;
	right=right_ypos;
	
	walk=TRUE;
}

//��� ���ϱ�
void CAnimationData::SetFrame(int vframe)
{
	frame=vframe;
	if(type==ani_exchange)	//�Դٰ��� �ݺ� �ִϸ��̼�
	{
		int frame2=abs((frame_max/2)-frame);
		SetRect(&ani_rect, x_size*frame2, y_point, x_size*(frame2+1), y_point+y_size);
	}
	else //��Ÿ
	{
		SetRect(&ani_rect, x_size*frame, y_point, x_size*(frame+1), y_point+y_size);
	}
}

//X,Y �� ��������
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

	//���� ����
	if(dir==dir_up)y_point=up;
	else if(dir==dir_left)y_point=left;
	else if(dir==dir_down)y_point=down;
	else if(dir==dir_right)y_point=right;
	//�ݴ�� ����
	else if(dir==dir_turn)
	{
		if(dir==dir_up)y_point=down;
		else if(dir==dir_left)y_point=right;
		else if(dir==dir_down)y_point=up;
		else if(dir==dir_right)y_point=left;
	}
	//�������� ����
	else if(dir==dir_turn_left)
	{
		if(dir==dir_up)y_point=left;
		else if(dir==dir_left)y_point=down;
		else if(dir==dir_down)y_point=right;
		else if(dir==dir_right)y_point=up;
	}
	//���������� ����
	else if(dir==dir_turn_right)
	{
		if(dir==dir_up)y_point=right;
		else if(dir==dir_left)y_point=up;
		else if(dir==dir_down)y_point=left;
		else if(dir==dir_right)y_point=down;
	}
	//�ƹ����� ����
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
	//���� �̹� �����Ǿ� �ִٸ� �����
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
	//�ڵ� �ִϸ��̼�
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
		OutputDebugString("�߸��� �ִϸ��̼� ID�� ��û�ϼ̽��ϴ�.\n");
		exit(1);
		//OutputDebugString("������ ���� ���� �ӽ� �ִϸ��̼� ������ �����ؼ� �����ϴ�.\n");
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