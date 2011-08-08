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

void CGameProcess::Process()
{
	if(m_script)
	{
		if(m_script->IsEnd())
		{
			FadeOut();
			Stop();
		}
	}
}