// DataEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "chunkmaker.h"
#include "DataEdit.h"
#include ".\dataedit.h"


// CDataEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDataEdit, CDialog)
CDataEdit::CDataEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDataEdit::IDD, pParent)
	, Name(_T(""))
	, use_colorkey(0)
	, opacity(0)
	, colorkey(_T(""))
	, layertype(0)
{
}

CDataEdit::~CDataEdit()
{
}

void CDataEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PATH, path);
	DDX_Text(pDX, IDC_EDIT1, Name);
	DDV_MaxChars(pDX, Name, 127);
	DDX_CBIndex(pDX, IDC_COMBO1, use_colorkey);
	DDX_Text(pDX, IDC_EDIT3, opacity);
	DDX_Text(pDX, IDC_EDIT2, colorkey);
	DDV_MaxChars(pDX, colorkey, 6);
	DDX_CBIndex(pDX, IDC_LAYERTYPE, layertype);
	DDX_Control(pDX, IDC_EDIT2, colorkey_control);
}


BEGIN_MESSAGE_MAP(CDataEdit, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CDataEdit 메시지 처리기입니다.

void CDataEdit::OnBnClickedOk()
{
    UpdateData(true);
	if(!multi && Name=="")MessageBox("이름을 적어주세요.");
		else OnOK();
}

//십진수를 16진수 문자로
void CDataEdit::DtoX(CString* x, int d)
{
	x->Format("%06x",d);
}

//16진수 문자를 10진수로
int CDataEdit::XtoD(CString x)
{
	int d=0;	//10진수
	for(int i=0; i<x.GetLength(); i++)
	{
		d*=16;
		if(x.GetAt(i)>='0' && x.GetAt(i)<='9')d+=(x.GetAt(i)-'0');
			else if(x.GetAt(i)>='a' && x.GetAt(i)<='f')d+=(x.GetAt(i)-'a'+10);
			else if(x.GetAt(i)>='A' && x.GetAt(i)<='F')d+=(x.GetAt(i)-'A'+10);
	}

	return d;
}

void CDataEdit::OnEnChangeEdit2()
{
	UpdateData(true);
	if(colorkey.GetLength()==6)
	{
		show_color=XtoD(colorkey);
		Invalidate(false);
	}
}

void CDataEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if(use_colorkey==1)dc.FillSolidRect(CRect(240,56,260,76), RGB((show_color&0xff0000)>>16, (show_color&0x00ff00)>>8, show_color&0x0000ff));
}

void CDataEdit::OnCbnSelchangeCombo1()
{
	UpdateData(true);
	if(use_colorkey==0)colorkey_control.EnableWindow(false);
		else colorkey_control.EnableWindow(true);
	Invalidate(false);
}

BOOL CDataEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(use_colorkey==0)colorkey_control.EnableWindow(false);
		else colorkey_control.EnableWindow(true);

	return TRUE;
}