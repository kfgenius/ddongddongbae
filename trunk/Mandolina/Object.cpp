#include "Object.h"

#include "global.h"

#include <math.h>

CObject::CObject(float radius)
{
	life_ = false;
	radius_ = radius;
	radiusPow_ = radius * radius;
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
}

float CObject::GetX()
{
	return x_;
}

float CObject::GetY()
{
	return y_;
}

float CObject::GetRadius()
{
	return radius_;
}

float CObject::GetRadiusPow()
{
	return radiusPow_;
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

bool CObject::IsLineInSprite(float x1, float y1, float x2, float y2, float gradient, float intercept)
{
	if(life_ == false)
	{
		return false;
	}

	float a = x_;
	float b = y_;
	float c = gradient;
	float d = intercept;
	float e = d - b;

	if (radius_ < 0)
	{
		return false;
	}
	else if (x1 != x2)
	{
		float D = (c * e - a) * (c * e - a) - (a * a + e * e - radiusPow_) * (c * c + 1);
		if (D >= 0)
		{
			if ((a - c * e + sqrt(D)) / (c * c + 1) >= min(x1, x2) && (a - c * e - sqrt(D)) / (c * c + 1) <= max(x1, x2))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (y1 == y2)
		{
			if (sqrt((a - x1) * (a - x1) + (b - y1) * (b - y1)) <= radius_)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			float D = b * b - (x1 - a) * (x1 - a) - b * b + radiusPow_;
			if (D >= 0)
			{
				if (b + sqrt(D) > min(y1, y2) && b - sqrt(D) < max(y1, y2))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
}

bool CObject::IsCollision(CObject* object)
{
	if(life_ == false)
	{
		return false;
	}

	float dx = object->GetX() - this->GetX();
	float dy = object->GetY() - this->GetY();
				
	float distance = sqrtf(dx * dx + dy * dy);
	float collision_distance = object->GetRadius() + this->GetRadius();

	if(distance <= collision_distance)
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

bool CObject::IsInScreen()
{
	const int margin = (int)radius_ + 100;

	if(x_ < -margin || x_ > SCREEN_WIDTH + margin
		|| y_ < -margin || y_ > SCREEN_HEIGHT + margin)
	{
		return false;
	}

	return true;
}

void CObject::Update()
{
}

void CObject::Draw()
{
}

void CObject::SetAngle(float angle)
{
	angle_ = angle;
}
