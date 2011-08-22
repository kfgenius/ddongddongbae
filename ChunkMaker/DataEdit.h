#pragma once
#include "afxwin.h"


// CDataEdit 대화 상자입니다.

class CDataEdit : public CDialog
{
	DECLARE_DYNAMIC(CDataEdit)

public:
	CDataEdit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDataEdit();

	bool multi;
	CString path;
	CString Name;
	int use_colorkey;
	int show_color;
	float opacity;
	void DtoX(CString* x, int d);
	int XtoD(CString x);
	CString colorkey;

	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit2();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	int layertype;
	CEdit colorkey_control;
	afx_msg void OnCbnSelchangeCombo1();
	BOOL OnInitDialog();
};
