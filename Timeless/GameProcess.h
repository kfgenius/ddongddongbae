//���� ���μ��� �⺻��

#pragma once

#include "process.h"
#include "donglib.h"
#include "dscript.h"

class CGameProcess :public CProcess
{
public:
	//���μ���
	CScript* m_script;

	//���μ���
	virtual void BeginProcess();
	virtual void Control();
	virtual void Process();
	virtual void Render();
	virtual void EndProcess();

	CGameProcess(void);
	virtual ~CGameProcess(void);
};
