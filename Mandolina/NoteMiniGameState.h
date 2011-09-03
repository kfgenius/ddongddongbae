#pragma once
#include "gameprocess.h"

#include "NoteObject.h"
#include "EnemyObject.h"

struct StagePhase
{
	int note;
	int delay;
};

class NoteMiniGameState :
	public CGameProcess
{
	enum Defines
	{
		NOTE_MAX = 20,
		ENEMY_MAX = 10,
		MOLD_TYPE_MAX = 9
	};

	enum MoldType
	{
		mold_normal,
		mold_move,
		mold_fat,
		mold_twin,
		mold_mini,
		mold_brain,
		mold_block,
		mold_twin_left,
		mold_twin_right
	};

private:
	void RenderNormalState();

	int GetFreeNoteID();
	int GetFreeEnemyID();

	void NormalTouchesMove(int x, int y);

	bool IsNoNote();
	bool IsNoEnemy();

	int GetColonPos(char* buffer);
	int CharToNote(char* buffer);

	void InitSound();
	void LoadStagePhase(int stage);
	void LoadStageMap(int stage);

	int old_x;
	int old_y;
	int origin_x;
	int origin_y;

	BOOL old_LButton;

	CAttribute* m_note_attribute;
	CAttribute* m_mold_attribute[MOLD_TYPE_MAX];

	CNoteObject* m_note[NOTE_MAX];
	CEnemyObject* m_enemy[ENEMY_MAX];

	StagePhase* stage_phase;
	int stage_phase_max;
	int phase;
	float next_time;
	bool note_end;
	bool is_clear;
	float game_speed;

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