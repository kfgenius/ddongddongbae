#if !defined(AFX_BOOK_H__00955681_8E4D_11D4_AF27_00C026261B5A__INCLUDED_)
#define AFX_BOOK_H__00955681_8E4D_11D4_AF27_00C026261B5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Book.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBook dialog

class CBook : public CDialog
{
// Construction
public:
	char buffer[20];
	int m_time;
	void NextPage();
	int m_page;
	CBook(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBook)
	enum { IDD = IDD_BOOK };
	CString	m_text;
	CString	m_type;
	CString	m_spage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBook)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBook)
	afx_msg void OnVscrollType();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOOK_H__00955681_8E4D_11D4_AF27_00C026261B5A__INCLUDED_)
