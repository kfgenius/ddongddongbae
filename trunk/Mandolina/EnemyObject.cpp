#include "EnemyObject.h"

#include <math.h>

#include "global.h"

CEnemyObject::CEnemyObject(float radius)
: CObject(radius)
{
}

CEnemyObject::~CEnemyObject()
{
}

void CEnemyObject::Update()
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

void CEnemyObject::Draw()
{
	if(life_ == false)
	{
		return;
	}

	//RenderManager::getInstance()->DrawCircle(x_, y_, radius_);
	JBrush brush = jdd->CreateBrush(JColor(0, 0, 255), 1.0f);
	RECT rect;	
	::SetRect(&rect, x_ - radius_, y_ - radius_, x_ + radius_, y_ + radius_);
	jdd->DrawEllipse(backbuffer, brush, &rect, 1);

}

bool CEnemyObject::IsLineInSprite(float x1, float y1, float x2, float y2, float gradient, float intercept)
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