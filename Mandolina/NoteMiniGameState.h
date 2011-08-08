#pragma once
#include "gameprocess.h"

#include "NoteObject.h"

class NoteMiniGameState :
	public CGameProcess
{
	enum Defines
	{
		THROWED_OBJECT_MAX = 10,
		EFFECT_MAX = 20,
		NOTE_MAX = 20
	};

//methods
public:
	NoteMiniGameState();
	~NoteMiniGameState();

	virtual void Draw();
	virtual void Process();

	void LoadSpriteFiles();

protected:
	virtual void TouchesDown(int x, int y);
	virtual void TouchesUp(int x, int y);
	virtual void TouchesMove(int x, int y);

private:
	void RenderNormalState();

	int GetFreeNoteID();

	void NormalTouchesMove(int x, int y);

//member variable
private:
	int old_x;
	int old_y;
	int origin_x;
	int origin_y;

	bool old_LButton;

	CNoteObject* m_note[NOTE_MAX];
};