// Nabul.h : main header file for the NABUL application
//

#if !defined(AFX_NABUL_H__A840D2A5_8CA5_11D4_AF27_00C026261B5A__INCLUDED_)
#define AFX_NABUL_H__A840D2A5_8CA5_11D4_AF27_00C026261B5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNabulApp:
// See Nabul.cpp for the implementation of this class
//

class CNabulApp : public CWinApp
{
public:
	CNabulApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNabulApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNabulApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NABUL_H__A840D2A5_8CA5_11D4_AF27_00C026261B5A__INCLUDED_)
