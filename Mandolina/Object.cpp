#include "Object.h"

#include "global.h"

#include <math.h>

CObject::CObject()
{
	life = false;
	speed = 1;
}

CObject::~CObject()
{
}

void CObject::GoAhead()
{
	x += cosf(angle) * speed;
	y += sinf(angle) * speed;
}

void CObject::Set(float x, float y, CAttribute* attribute)
{
	this->x = x;
	this->y = y;

	life = true;

	radius = attribute->radius;
	radiusPow = radius * radius;
	pic_name = attribute->pic_name;
	cx = attribute->cx;
	cy = attribute->cy;
}

float CObject::GetX()
{
	return x;
}

float CObject::GetY()
{
	return y;
}

float CObject::GetRadius()
{
	return radius;
}

float CObject::GetRadiusPow()
{
	return radiusPow;
}

bool CObject::GetLife()
{
	return life;
}

bool CObject::IsLineInSprite(float x1, float y1, float x2, float y2, float gradient, float intercept)
{
	if(life == false)
	{
		return false;
	}

	float a = x;
	float b = y;
	float c = gradient;
	float d = intercept;
	float e = d - b;

	if (radius < 0)
	{
		return false;
	}
	else if (x1 != x2)
	{
		float D = (c * e - a) * (c * e - a) - (a * a + e * e - radiusPow) * (c * c + 1);
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
			if (sqrt((a - x1) * (a - x1) + (b - y1) * (b - y1)) <= radius)
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
			float D = b * b - (x1 - a) * (x1 - a) - b * b + radiusPow;
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
	if(life == false)
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

void CObject::Die()
{
	life = false;
}

bool CObject::IsInScreen()
{
	const int margin = (int)radius + 100;

	if(x < -margin || x > SCREEN_WIDTH + margin
		|| y < -margin || y > SCREEN_HEIGHT + margin)
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
	jdd->DrawPicture(backbuffer, pic_name, x + cx, y + cy, NULL);
}

void CObject::SetAngle(float angle)
{
	this->angle = angle;
}

void CObject::SetSpeed(float speed)
{
	this->speed = speed;
}
