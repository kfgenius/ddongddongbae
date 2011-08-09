#ifndef __ENEMY_OBJECT_H__
#define __ENEMY_OBJECT_H__

#include "Object.h"

class CEnemyObject : public CObject
{
//methods
public:
	CEnemyObject(float radius);
	virtual ~CEnemyObject();

	virtual void Update();
	virtual void Draw();

	bool IsLineInSprite(float x1, float y1, float x2, float y2, float gradient, float intercept);
};

#endif //__ENEMY_OBJECT_H__