#include "Process.h"

CProcess::CProcess(void)
{
	stop = false;
}

CProcess::~CProcess(void)
{
}

//���μ��� ���� Ȯ��
bool CProcess::IsStop()
{
	return stop;
}

//���μ��� ����
void CProcess::Stop()
{
	stop = true;
}

//��� ���
int CProcess::GetMode()
{
	return mode;
}

//��� ���ϱ�
void CProcess::SetMode(int mode)
{
	this->mode = mode;
}

//��� ��ȯ
//(��� ���ϱ�� ������ ������ �뵵�� �������̵� �� �����ؼ� ���)
void CProcess::ChangeMode(int mode)
{
	this->mode = mode;
}
