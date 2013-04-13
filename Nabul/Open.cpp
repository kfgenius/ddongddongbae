// Open.cpp : implementation file
//

#include "stdafx.h"
#include "Nabul.h"
#include "Open.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpen dialog
CString m_optext[]={
"마계에 쳐 박혀 아무말 없이 지내던 마왕...",
"\n어느날 맛이 가서 전쟁을 일으킨다.",
"\n그리고 세상 곳곳은 오염되어 간다.",
"\n자! 이제 당신이 이 세상을 구해야 한다!"};


COpen::COpen(CWnd* pParent /*=NULL*/)
	: CDialog(COpen::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpen)
	m_text = _T("");
	//}}AFX_DATA_INIT
}


void COpen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpen)
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpen, CDialog)
	//{{AFX_MSG_MAP(COpen)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpen message handlers

void COpen::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	opening(m_time);
	m_time++;
	CDialog::OnTimer(nIDEvent);
}

BOOL COpen::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_time=0;
	m_midi.Open("opening.mid");
	m_midi.Play();
	// TODO: Add extra initialization here
	SetTimer(1,100,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COpen::OnDestroy() 
{
	m_midi.Close();
	m_game.DoModal();
	KillTimer(1);
	CDialog::OnDestroy();
}


void COpen::opening(int i)
{
	CClientDC dc1(this); // device context for painting
		
	// TODO: Add your message handler code here
	HBITMAP hbitmap1;
	HDC hMemDC1;
	
	if(i==0){
		hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE6));
		hMemDC1 = ::CreateCompatibleDC(NULL);
		SelectObject(hMemDC1,hbitmap1);
		::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
		m_text=m_optext[0];
		::DeleteDC(hMemDC1);
		::DeleteObject(hbitmap1);
	}
	if(i>=31&&i<41){
		hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE7));
		hMemDC1 = ::CreateCompatibleDC(NULL);
		SelectObject(hMemDC1,hbitmap1);
		::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,(i-30)*35,340,SRCCOPY);
		::DeleteDC(hMemDC1);
		::DeleteObject(hbitmap1);
	}
//헤멤의 숲
	if(i==50){
		hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE12));
		hMemDC1 = ::CreateCompatibleDC(NULL);
		SelectObject(hMemDC1,hbitmap1);
		::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
		::DeleteDC(hMemDC1);
		::DeleteObject(hbitmap1);
	}
	if(i>=61&&i<66){
		hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE1));
		hMemDC1 = ::CreateCompatibleDC(NULL);
		SelectObject(hMemDC1,hbitmap1);
		::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,(i-60)*68,SRCCOPY);
		::DeleteDC(hMemDC1);
		::DeleteObject(hbitmap1);
	}
//카베사막
	if(i==70){
		hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE22));
		hMemDC1 = ::CreateCompatibleDC(NULL);
		SelectObject(hMemDC1,hbitmap1);
		::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
		::DeleteDC(hMemDC1);
		::DeleteObject(hbitmap1);
	}
	if(i>=81&&i<86){
		hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE2));
		hMemDC1 = ::CreateCompatibleDC(NULL);
		SelectObject(hMemDC1,hbitmap1);
		::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,(i-80)*68,SRCCOPY);
		::DeleteDC(hMemDC1);
		::DeleteObject(hbitmap1);
	}
//죽음의 늪
	if(i==90){
		hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE42));
		hMemDC1 = ::CreateCompatibleDC(NULL);
		SelectObject(hMemDC1,hbitmap1);
		::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
		::DeleteDC(hMemDC1);
		::DeleteObject(hbitmap1);
	}
	if(i>=101&&i<106){
		hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE4));
		hMemDC1 = ::CreateCompatibleDC(NULL);
		SelectObject(hMemDC1,hbitmap1);
		::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,(i-100)*68,SRCCOPY);
		::DeleteDC(hMemDC1);
		::DeleteObject(hbitmap1);
	}
	if(i==30)
		m_text+=m_optext[1];
	if(i==50)
		m_text+=m_optext[2];
	if(i==110)
		m_text+=m_optext[3];
	UpdateData(false);
}
