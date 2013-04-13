// NabulDlg.h : header file
//

#if !defined(AFX_NABULDLG_H__A840D2A7_8CA5_11D4_AF27_00C026261B5A__INCLUDED_)
#define AFX_NABULDLG_H__A840D2A7_8CA5_11D4_AF27_00C026261B5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Book.h"
#include "Open.h"
/////////////////////////////////////////////////////////////////////////////
// CNabulDlg dialog

class CNabulDlg : public CDialog
{
// Construction
public:
	CString m_text2;
	CBook m_book;
	COpen m_open;	
	CNabulDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNabulDlg)
	enum { IDD = IDD_NABUL_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNabulDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CNabulDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOk();
	afx_msg void OnBook();
	afx_msg void OnGame();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NABULDLG_H__A840D2A7_8CA5_11D4_AF27_00C026261B5A__INCLUDED_)
