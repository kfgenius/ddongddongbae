// GM.h : main header file for the GM application
//

#if !defined(AFX_GM_H__F00A59A9_C1F8_41E9_B936_F09837303206__INCLUDED_)
#define AFX_GM_H__F00A59A9_C1F8_41E9_B936_F09837303206__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGMApp:
// See GM.cpp for the implementation of this class
//

class CGMApp : public CWinApp
{
public:
	CGMApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGMApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GM_H__F00A59A9_C1F8_41E9_B936_F09837303206__INCLUDED_)
