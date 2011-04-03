//게임 프로세스 기본형

#pragma once

#include "process.h"
#include "donglib.h"
#include "dscript.h"

class CGameProcess :public CProcess
{
public:
	//프로세스
	CScript* m_script;

	//프로세스
	virtual void BeginProcess();
	virtual void Control();
	virtual void Process();
	virtual void Render();
	virtual void EndProcess();

	CGameProcess(void);
	virtual ~CGameProcess(void);
};
