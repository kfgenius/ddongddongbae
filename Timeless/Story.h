#pragma once
#include "gameprocess.h"

class CStory :
	public CGameProcess
{
public:
	void Control();
	void Process();
	void Render();

	CStory(void);
	~CStory(void);
};