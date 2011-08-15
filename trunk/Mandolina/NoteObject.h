#ifndef __NOTE_OBJECT_H__
#define __NOTE_OBJECT_H__

#include "Object.h"

class CNoteObject : public CObject
{
//methods
public:
	CNoteObject();
	virtual ~CNoteObject();

	virtual void Update();
	virtual void Draw();

	virtual void Set(float x, float y, CAttribute* attribute);

	void Bounce(float gradient);

	float touch_able;
};

#endif //__NOTE_OBJECT_H__