// MasterDlg.h : header file
//

#if !defined(AFX_MASTERDLG_H__6594D92E_67F3_43BF_8A8C_CAA0ACF8E946__INCLUDED_)
#define AFX_MASTERDLG_H__6594D92E_67F3_43BF_8A8C_CAA0ACF8E946__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMasterDlg dialog

class CMasterDlg : public CDialog
{
// Construction
public:
	void Equip(int num);
	int Item(CString);
	int m_listNum;
	CMasterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMasterDlg)
	enum { IDD = IDD_MASTER_DIALOG };
	CComboBox	m_ctrlKind;
	CButton	m_delete;
	CButton	m_item;
	CListBox	m_list;
	int		m_att;
	int		m_def;
	int		m_int;
	int		m_luck;
	int		m_hp;
	int		m_mp;
	int		m_damage;
	int		m_level;
	int		m_hpMax;
	int		m_mpMax;
	int		m_kind;
	int		m_minus;
	CString	m_what;
	CString	m_name;
	CString	m_class;
	CString	m_Cname;
	int		m_money;
	int		m_book;
	int		m_arm;
	int		m_wp;
	int		m_etc;
	CString	m_effect;
	BOOL	m_top;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMasterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMasterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void OnLevel();
	afx_msg void OnCatt();
	afx_msg void OnCdef();
	afx_msg void OnCmag();
	afx_msg void OnRest();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnGet();
	afx_msg void OnDel();
	afx_msg void OnSelchangeList();
	afx_msg void OnSelchangeKind();
	afx_msg void OnTop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MASTERDLG_H__6594D92E_67F3_43BF_8A8C_CAA0ACF8E946__INCLUDED_)
