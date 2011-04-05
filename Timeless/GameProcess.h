//게임 프로세스 기본형

#pragma once

#include "process.h"
#include "dscript.h"

class CGameProcess :public CProcess
{
public:
	//프로세스
	CScript* m_script;

	//프로세스
	virtual void Process()PURE;

	CGameProcess(void);
	virtual ~CGameProcess(void);
};
