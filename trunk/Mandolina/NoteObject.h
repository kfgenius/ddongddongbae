#ifndef __NOTE_OBJECT_H__
#define __NOTE_OBJECT_H__

#include "Object.h"

class CNoteObject : public CObject
{
//methods
public:
	CNoteObject(float radius);
	virtual ~CNoteObject();

	virtual void Update();
	virtual void Draw();

	virtual void Set(float x, float y);

	void Bounce(float gradient);

	float touch_able;
};

#endif //__NOTE_OBJECT_H__