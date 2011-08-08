//게임 프로세스 기본형

#pragma once

#include "process.h"
#include "dscript.h"

class CGameProcess :public CProcess
{
public:
	//스크립트
	CScript* m_script;

	//프로세스
	virtual void Process();

	//생성자, 소멸자
	CGameProcess(void);
	virtual ~CGameProcess(void);
};
