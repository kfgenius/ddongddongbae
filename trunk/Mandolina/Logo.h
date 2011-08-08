#pragma once
#include "gameprocess.h"

class CLogo :
	public CGameProcess
{
private:
	int game_time;

public:
	void Process();

	CLogo(void);
	~CLogo(void);
};
