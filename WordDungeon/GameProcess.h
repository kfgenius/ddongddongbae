//�������� ���μ��� �⺻��
//2010. 7. 31
//�ڵ���

#pragma once

#include "process.h"
#include "donglib.h"
#include "dscript.h"

class CGameProcess :public CProcess
{
public:
	//���μ���
	CScript* m_script;

	//����, ó��, ���
	virtual void Control()PURE;
	virtual void Process()PURE;
	virtual void Render()PURE;

	CGameProcess(void);
	~CGameProcess(void);
};
