//���� ���μ��� �⺻��

#pragma once

#include "process.h"
#include "dscript.h"

class CGameProcess :public CProcess
{
public:
	//���μ���
	CScript* m_script;

	//���μ���
	virtual void Process()PURE;

	CGameProcess(void);
	virtual ~CGameProcess(void);
};
