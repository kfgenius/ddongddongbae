#ifndef __OBJECT_H__
#define __OBJECT_H__

class CObject
{
//methods
public:
	CObject(float radius);
	virtual ~CObject();

	virtual void Update();
	virtual void Draw();

	virtual void Set(float x, float y);

	virtual float GetX();
	virtual float GetY();
	virtual float GetRadius();
	virtual float GetRadiusPow();

	virtual bool GetLife();
	virtual void MoveDown(float y);

	virtual bool IsLineInSprite(float x1, float y1, float x2, float y2, float gradient, float intercept);
	virtual bool IsCollision(CObject* object);
	virtual void Done();
	virtual void Die();

	virtual bool IsInScreen();

	void SetAngle(float angle);

//member variable
private:

protected:
	float x_;
	float y_;

	bool life_;
	bool isDone_;

	int doneAnim_;

	float radius_;
	float radiusPow_;
	float angle_;
};

#endif //__OBJECT_H__