// GMDlg.h : header file
//

#if !defined(AFX_GMDLG_H__11C063E4_0CA0_47A3_9AB3_B70062817669__INCLUDED_)
#define AFX_GMDLG_H__11C063E4_0CA0_47A3_9AB3_B70062817669__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGMDlg dialog

class CGMDlg : public CDialog
{
// Construction
public:
	int m_trNum;
	int m_eventNum;
	int m_turnNum;
	CGMDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGMDlg)
	enum { IDD = IDD_GM_DIALOG };
	CListBox	m_trList;
	CListBox	m_eventlist;
	int		m_player;
	CString	m_turn;
	int		m_angle;
	CString	m_dice;
	CString	m_name1;
	CString	m_name3;
	CString	m_name4;
	CString	m_name5;
	CString	m_name6;
	CString	m_name2;
	CString	m_event;
	CString	m_EventName;
	CString	m_tr;
	CString	m_spot;
	CString	m_class1;
	CString	m_class2;
	CString	m_class3;
	CString	m_class4;
	CString	m_class5;
	CString	m_class6;
	CString	m_dir3;
	CString	m_dir4;
	CString	m_dir2;
	CString	m_memo2;
	CString	m_state3;
	CString	m_memo3;
	CString	m_state4;
	CString	m_memo4;
	CString	m_dir5;
	CString	m_state5;
	CString	m_memo5;
	CString	m_dir6;
	CString	m_state6;
	CString	m_dir1;
	CString	m_memo6;
	CString	m_state1;
	CString	m_memo1;
	CString	m_state2;
	BOOL	m_top;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGMDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGMDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnNext();
	afx_msg void OnSelchangeEventlist();
	afx_msg void OnDice();
	afx_msg void OnAdd2();
	afx_msg void OnDel2();
	afx_msg void OnSelchangeTrlist();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnTop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GMDLG_H__11C063E4_0CA0_47A3_9AB3_B70062817669__INCLUDED_)
