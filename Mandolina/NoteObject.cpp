#include "NoteObject.h"

#include <math.h>

#include "global.h"

CNoteObject::CNoteObject(float radius)
: CObject(radius)
{
}

CNoteObject::~CNoteObject()
{
}

void CNoteObject::Update()
{
	if(life_ == false)
	{
		return;
	}

	//x_ += cos(gradient_);
	//y_ += sin(gradient_);
	x_ += cosf(angle_);
	y_ += sinf(angle_);

	if(IsInScreen() == false)
	{
		life_ = false;
	}
}

void CNoteObject::Draw()
{
	if(life_ == false)
	{
		return;
	}

	JBrush brush = jdd->CreateBrush(JColor(255, 0, 0), 1.0f);
	RECT rect;	
	::SetRect(&rect, (int)(x_ - radius_), (int)(y_ - radius_), (int)(x_ + radius_), (int)(y_ + radius_));
	jdd->DrawEllipse(backbuffer, brush, &rect, 1);
}

void CNoteObject::Set(float x, float y)
{
	CObject::Set(x, y);

	touch_able = true;
}

void CNoteObject::Bounce(float angle)
{
	if(touch_able == true)
	{
		SetAngle(angle);
		touch_able = false;
	}
}