#pragma once
#include "afxwin.h"


// CDataEdit ��ȭ �����Դϴ�.

class CDataEdit : public CDialog
{
	DECLARE_DYNAMIC(CDataEdit)

public:
	CDataEdit(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	int layertype;
	CEdit colorkey_control;
	afx_msg void OnCbnSelchangeCombo1();
	BOOL OnInitDialog();
};
