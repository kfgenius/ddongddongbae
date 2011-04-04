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

	//����, ó��, ���
	virtual void BeginProcess()PURE;
	virtual void Control()PURE;
	virtual void Process()PURE;
	virtual void Render()PURE;
	virtual void EndProcess()PURE;

	//������, �Ҹ���
	CProcess(void);
	virtual ~CProcess(void);
};
