//���� ���μ��� �⺻��

#pragma once

#include "process.h"
#include "dscript.h"

class CGameProcess :public CProcess
{
public:
	//��ũ��Ʈ
	CScript* m_script;

	//���μ���
	virtual void Process();

	//������, �Ҹ���
	CGameProcess(void);
	virtual ~CGameProcess(void);
};
