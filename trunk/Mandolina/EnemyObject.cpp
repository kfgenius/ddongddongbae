#include "EnemyObject.h"

#include <math.h>

#include "global.h"

CEnemyObject::CEnemyObject()
{
}

CEnemyObject::~CEnemyObject()
{
}

void CEnemyObject::Update()
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

void CEnemyObject::Draw()
{
	if(life == false)
	{
		return;
	}

	CObject::Draw();

	//RenderManager::getInstance()->DrawCircle(x_, y_, radius_);
	JBrush brush = jdd->CreateBrush(JColor(0, 0, 255), 1.0f);
	RECT rect;	
	SetRect(&rect, (int)(x - radius), (int)(y - radius), (int)(x + radius), (int)(y + radius));
	jdd->DrawEllipse(backbuffer, brush, &rect, 1);

}