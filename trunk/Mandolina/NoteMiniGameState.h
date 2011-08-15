#pragma once
#include "gameprocess.h"

#include "NoteObject.h"
#include "EnemyObject.h"

class NoteMiniGameState :
	public CGameProcess
{
	enum Defines
	{
		NOTE_MAX = 20,
		ENEMY_MAX = 10
	};

private:
	void RenderNormalState();

	int GetFreeNoteID();

	void NormalTouchesMove(int x, int y);

	int old_x;
	int old_y;
	int origin_x;
	int origin_y;

	bool old_LButton;

	CAttribute* m_note_attribute;
	CAttribute* m_mold_attribute;

	CNoteObject* m_note[NOTE_MAX];
	CEnemyObject* m_enemy[ENEMY_MAX];

protected:
	virtual void TouchesDown(int x, int y);
	virtual void TouchesUp(int x, int y);
	virtual void TouchesMove(int x, int y);

public:
	NoteMiniGameState();
	~NoteMiniGameState();

	virtual void Draw();
	virtual void Process();

	void LoadSpriteFiles();
};