#ifndef __ENEMY_OBJECT_H__
#define __ENEMY_OBJECT_H__

#include "Object.h"

class CEnemyObject : public CObject
{
//methods
public:
	CEnemyObject();
	virtual ~CEnemyObject();

	virtual void Update();
	virtual void Draw();
};

#endif //__ENEMY_OBJECT_H__