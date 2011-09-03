#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "Attribute.h"

class CObject
{
private:

protected:
	void GoAhead();

	float x;
	float y;
	float speed;

	bool life;
	bool isDone;

	int doneAnim;

	float radius;
	float radiusPow;
	float angle;

	char* pic_name;

	int cx;
	int cy;

public:
	CObject();
	virtual ~CObject();

	virtual void Update();
	virtual void Draw();

	virtual void Set(float x, float y, CAttribute* attribute);

	virtual float GetX();
	virtual float GetY();
	virtual float GetRadius();
	virtual float GetRadiusPow();

	virtual bool GetLife();

	virtual bool IsLineInSprite(float x1, float y1, float x2, float y2, float gradient, float intercept);
	virtual bool IsCollision(CObject* object);
	virtual void Die();

	virtual bool IsInScreen();

	virtual void SetAngle(float angle);
	virtual void SetSpeed(float speed);
};

#endif //__OBJECT_H__