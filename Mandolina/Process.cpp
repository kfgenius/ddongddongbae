#include "Process.h"

CProcess::CProcess(void)
{
	stop = FALSE;
}

CProcess::~CProcess(void)
{
}

//프로세스 상태 확인
bool CProcess::IsStop()
{
	return stop;
}

//프로세스 정지
void CProcess::Stop()
{
	stop = TRUE;
}

//모드 얻기
int CProcess::GetMode()
{
	return mode;
}

//모드 정하기
void CProcess::SetMode(int mode)
{
	this->mode = mode;
}

//모드 변환
//(모드 정하기와 내용은 같지만 용도는 오버라이딩 후 구분해서 사용)
void CProcess::ChangeMode(int mode)
{
	this->mode = mode;
}
