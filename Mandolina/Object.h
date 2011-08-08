#ifndef __OBJECT_H__
#define __OBJECT_H__

class CObject
{
//methods
public:
	CObject();
	virtual ~CObject();

	virtual void Update();
	virtual void Draw();

	virtual void Set(float x, float y);

	virtual float GetLeft();
	virtual float GetRight();
	virtual float GetTop();
	virtual float GetBottom();

	virtual bool GetLife();
	virtual void MoveDown(float y);

	virtual bool IsCollision(CObject* object);
	virtual void Done();
	virtual void Die();

//member variable
private:
	void SetRect();

protected:
	float x_;
	float y_;
	float left_;
	float top_;
	float width_;
	float height_;

	bool life_;
	bool isDone_;

	int doneAnim_;
};

#endif //__OBJECT_H__