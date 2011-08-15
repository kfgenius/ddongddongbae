#include "NoteObject.h"

#include <math.h>

#include "global.h"

CNoteObject::CNoteObject()
{
}

CNoteObject::~CNoteObject()
{
}

void CNoteObject::Update()
{
	if(life == false)
	{
		return;
	}

	GoAhead();

	if(IsInScreen() == false)
	{
		life = false;
	}
}

void CNoteObject::Draw()
{
	if(life == false)
	{
		return;
	}

	CObject::Draw();

	JBrush brush = jdd->CreateBrush(JColor(255, 0, 0), 1.0f);
	RECT rect;	
	SetRect(&rect, (int)(x - radius), (int)(y - radius), (int)(x + radius), (int)(y + radius));
	jdd->DrawEllipse(backbuffer, brush, &rect, 1);
}

void CNoteObject::Set(float x, float y, CAttribute* attribute)
{
	CObject::Set(x, y, attribute);

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