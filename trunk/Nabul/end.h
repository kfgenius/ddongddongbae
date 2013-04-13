#if !defined(AFX_END_H__19051A41_964D_11D4_8E2A_000102626C18__INCLUDED_)
#define AFX_END_H__19051A41_964D_11D4_8E2A_000102626C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// end.h : header file
//
#include"Midi.h"
/////////////////////////////////////////////////////////////////////////////
// Cend dialog

class Cend : public CDialog
{
// Construction
public:
	void ending();
	CMidi m_midi;
	int m_time;
	Cend(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Cend)
	enum { IDD = IDD_END };
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cend)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Cend)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_END_H__19051A41_964D_11D4_8E2A_000102626C18__INCLUDED_)
