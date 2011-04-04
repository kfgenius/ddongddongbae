//프로세스 기본형
//2010. 7. 31
//박동흥
#pragma once

#include <windows.h>

class CProcess
{
private:
	bool stop;	//중단 여부
	int mode;	//모드

public:
	//프로세스 중단 관련
	virtual bool IsStop();
	virtual void Stop();

	//모드 관련
	virtual int GetMode();
	virtual void SetMode(int mode);
	virtual void ChangeMode(int mode);

	//조작, 처리, 출력
	virtual void BeginProcess()PURE;
	virtual void Control()PURE;
	virtual void Process()PURE;
	virtual void Render()PURE;
	virtual void EndProcess()PURE;

	//생성자, 소멸자
	CProcess(void);
	virtual ~CProcess(void);
};
