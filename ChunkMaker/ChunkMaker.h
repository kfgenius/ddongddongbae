// chunkmaker.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CchunkmakerApp:
// �� Ŭ������ ������ ���ؼ��� chunkmaker.cpp�� �����Ͻʽÿ�.
//

class CchunkmakerApp : public CWinApp
{
public:
	CchunkmakerApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CchunkmakerApp theApp;
