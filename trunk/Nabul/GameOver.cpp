// GameOver.cpp : implementation file
//

#include "stdafx.h"
#include "Nabul.h"
#include "GameOver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGameOver dialog


CGameOver::CGameOver(CWnd* pParent /*=NULL*/)
	: CDialog(CGameOver::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGameOver)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGameOver::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGameOver)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGameOver, CDialog)
	//{{AFX_MSG_MAP(CGameOver)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGameOver message handlers

void CGameOver::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnOK();
	CDialog::OnLButtonDown(nFlags, point);
}

void CGameOver::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnOK();
	CDialog::OnRButtonDown(nFlags, point);
}
