// end.cpp : implementation file
//

#include "stdafx.h"
#include "Nabul.h"
#include "end.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Cend dialog
CString m_edtext[]=
{"Á¦ÀÛ: ÆÎ°Ô¾Æ",
"±âÈ¹:¹Úµ¿Èï\nÁÖ ±×·¡ÇÈ:¹Úµ¿Èï\nÇÁ·Î±×·¡¹Ö:¹Úµ¿Èï\n\nÇíÇí...Èûµé´Ù...\n°ÅÀÇ ³ª È¥ÀÚ ¸¸µé¾ú´Ù...\n´ã¿£ ÆÀ ¸ðµÎ°¡ ÈûÀ» ¸ðÀ¸¸é ÁÁÀ»°Çµ¥...",
"¹è°æ±×¸²: ÇãÀÏ\n¼Ò¿Ü ´çÇÑ ¸â¹ö: ±èÁÖÇõ\nº£Å¸ Å×½ºÆ®: ±èÀºÁØ",
"\n\n\n\n\n\n\n\n               ³¡"};

Cend::Cend(CWnd* pParent /*=NULL*/)
	: CDialog(Cend::IDD, pParent)
{
	//{{AFX_DATA_INIT(Cend)
	m_text = _T("");
	//}}AFX_DATA_INIT
}


void Cend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Cend)
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Cend, CDialog)
	//{{AFX_MSG_MAP(Cend)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cend message handlers

void Cend::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	ending();
	m_time++;
	CDialog::OnTimer(nIDEvent);
}

BOOL Cend::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_time=0;
	m_midi.Open("end.mid");
	m_midi.Play();
	SetTimer(1,100,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Cend::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	m_midi.Close();
	KillTimer(1);
}

void Cend::ending()
{
	CClientDC dc1(this); // device context for painting
		
	// TODO: Add your message handler code here
	HBITMAP hbitmap1;
	HDC hMemDC1;
	if(m_time==0||m_time==10||m_time==20||m_time==30||m_time==40||m_time==50||m_time==60||m_time==70||m_time==80){
		switch(m_time){
			case 0:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE6));break;
			case 10:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE62));break;
			case 20:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE63));break;
			case 30:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE64));break;
			case 40:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE65));break;
			case 50:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE66));break;
			case 60:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE67));break;
			case 70:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE68));break;
			case 80:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE4));break;
		}
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
//Á×À½ÀÇ ´Ë
	if(m_time==110){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE42));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCPAINT);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
	if(m_time==120){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE42));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
//È¥µ·ÀÇ Å¾
	if(m_time==150){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE3));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
	if(m_time==180){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE32));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCPAINT);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
	if(m_time==190){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE32));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
//Ä«º£»ç¸·
	if(m_time==220){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE2));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
	if(m_time==250){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE22));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCPAINT);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
	if(m_time==260){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE22));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
//Çì¸âÀÇ ½£
	if(m_time==290){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE1));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
	if(m_time==320){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE12));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCPAINT);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
	if(m_time==330){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE12));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,350,340,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
	if(m_time>=361&&m_time<371){
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_END));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,20,350,340,hMemDC1,0,0,(m_time-360)*35,340,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
	}
//³»¿ë
	if(m_time==0){
	m_text=m_edtext[0];
	UpdateData(false);
	}
	if(m_time==80){
	m_text=m_edtext[1];
	UpdateData(false);
	}
	if(m_time==220){
	m_text=m_edtext[2];
	UpdateData(false);
	}
	if(m_time==370){
	m_text=m_edtext[3];
	UpdateData(false);
	}
}
