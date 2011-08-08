#include "Object.h"

#include "global.h"

CObject::CObject()
: life_(false)
{
}

CObject::~CObject()
{
}

void CObject::Set(float x, float y)
{
	x_ = x;
	y_ = y;

	life_ = true;
	isDone_ = false;

	SetRect();
}

float CObject::GetLeft()
{
	return x_ + left_;
}

float CObject::GetRight()
{
	return x_ + width_;
}

float CObject::GetTop()
{
	return y_ + top_;
}

float CObject::GetBottom()
{
	return y_ + height_;
}

bool CObject::GetLife()
{
	return life_;
}

void CObject::MoveDown(float y)
{
	y_ += y;

	if(y_ > SCREEN_HEIGHT)
	{
		life_ = false;
	}
}

bool CObject::IsCollision(CObject* object)
{
	if(life_ == false)
	{
		return false;
	}

	if(GetRight() >= object->GetLeft() && GetLeft() <= object->GetRight()
		&& GetBottom() >= object->GetTop() && GetTop() <= object->GetBottom())
	{
		return true;
	}

	return false;
}

void CObject::Done()
{
	isDone_ = true;
}

void CObject::Die()
{
	life_ = false;
}

void CObject::SetRect()
{
	/*int rect[4];
	spriteInstance_->GetCurrentFrameRect(rect);
	left_ = (float)rect[0];
	top_ = (float)rect[1];
	width_ = (float)rect[2];
	height_ = (float)rect[3];*/
}

void CObject::Update()
{
}

void CObject::Draw()
{
}