//말빨동굴 프로세스 기본형
//2010. 7. 31
//박동흥

#pragma once

#include "process.h"
#include "donglib.h"
#include "dscript.h"

class CGameProcess :public CProcess
{
public:
	//프로세스
	CScript* m_script;

	//조작, 처리, 출력
	virtual void Control()PURE;
	virtual void Process()PURE;
	virtual void Render()PURE;

	CGameProcess(void);
	~CGameProcess(void);
};
