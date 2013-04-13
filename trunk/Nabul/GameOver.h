#if !defined(AFX_GAMEOVER_H__8CE1B8E3_9645_11D4_8E2A_000102626C18__INCLUDED_)
#define AFX_GAMEOVER_H__8CE1B8E3_9645_11D4_8E2A_000102626C18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameOver.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGameOver dialog

class CGameOver : public CDialog
{
// Construction
public:
	CGameOver(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGameOver)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameOver)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGameOver)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMEOVER_H__8CE1B8E3_9645_11D4_8E2A_000102626C18__INCLUDED_)
