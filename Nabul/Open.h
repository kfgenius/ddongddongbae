#if !defined(AFX_OPEN_H__09B81201_9FA7_11D4_8E2A_000102626C18__INCLUDED_)
#define AFX_OPEN_H__09B81201_9FA7_11D4_8E2A_000102626C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Open.h : header file
//
#include "Game.h"
/////////////////////////////////////////////////////////////////////////////
// COpen dialog

class COpen : public CDialog
{
// Construction
public:
	void opening(int i);
	CGame m_game;
	CMidi m_midi;
	int m_time;
	COpen(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpen)
	enum { IDD = IDD_OPEN };
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpen)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPEN_H__09B81201_9FA7_11D4_8E2A_000102626C18__INCLUDED_)
