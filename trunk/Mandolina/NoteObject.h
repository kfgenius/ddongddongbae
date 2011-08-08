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

	bool IsLineInSprite(float x1, float y1, float x2, float y2, float gradient, float intercept);

	bool IsInScreen();

	void SetGradient(float gradient);
	void Bounce(float gradient);

	float radius_;
	float radiusPow_;
	float angle_;

	float touch_able;
};

#endif //__NOTE_OBJECT_H__