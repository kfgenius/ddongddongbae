//���μ��� �⺻��
//2010. 7. 31
//�ڵ���
#pragma once

#include <windows.h>

class CProcess
{
private:
	bool stop;	//�ߴ� ����
	int mode;	//���

public:
	//���μ��� �ߴ� ����
	virtual bool IsStop();
	virtual void Stop();

	//��� ����
	virtual int GetMode();
	virtual void SetMode(int mode);
	virtual void ChangeMode(int mode);

	//���μ��� ó��
	virtual void Process()PURE;

	//������, �Ҹ���
	CProcess(void);
	virtual ~CProcess(void);
};
