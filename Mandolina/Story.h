#pragma once
#include "gameprocess.h"

class CStory :
	public CGameProcess
{
public:
	void Process();

	CStory(void);
	~CStory(void);
};