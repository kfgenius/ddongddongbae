#include "GameProcess.h"

CGameProcess::CGameProcess(void)
{
	m_script = NULL;
}

CGameProcess::~CGameProcess(void)
{
	if(m_script)
	{
		delete m_script;
		m_script = NULL;
	}
}