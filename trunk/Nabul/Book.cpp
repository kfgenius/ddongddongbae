// Book.cpp : implementation file
//

#include "stdafx.h"
#include "Nabul.h"
#include "Book.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBook dialog


CBook::CBook(CWnd* pParent /*=NULL*/)
	: CDialog(CBook::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBook)
	m_text = _T("");
	m_type = _T("");
	m_spage = _T("");
	//}}AFX_DATA_INIT
}


void CBook::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBook)
	DDX_Text(pDX, IDC_TEXT, m_text);
	DDX_Text(pDX, IDC_TYPE, m_type);
	DDX_Text(pDX, IDC_PAGE, m_spage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBook, CDialog)
	//{{AFX_MSG_MAP(CBook)
	ON_EN_VSCROLL(IDC_TYPE, OnVscrollType)
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBook message handlers

void CBook::OnVscrollType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	char tmp2[80];
	int i;
	for(i=0;i<strlen(m_type)-2;i++)
		tmp2[i]=m_type[i];
	tmp2[i]='\0';
	if(!strcmp(tmp2,m_text)) NextPage();
		else
			MessageBeep(1);
	m_type="";
	UpdateData(false);
}

BOOL CBook::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_page=-1;
	m_time=120;
	SetTimer(1,1000,NULL);
	NextPage();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBook::NextPage()
{
	m_page++;
	switch(m_page){
	case 0:
		m_text="본 게임은 마법사 나불나불입니다.";break;
	case 1:
		m_text="마법사 나불나불은 타자 연습 게임입니다.";break;
	case 2:
		m_text="적들은 위쪽에 글자를 붙이고 있는데";break;
	case 3:
		m_text="그 글자를 치면 그 적에게 1데미지를 줍니다.";break;
	case 4:
		m_text="그리고 나불나불은 레벨,체력,마력,경험이 있습니다.";break;
	case 5:
		m_text="체력이 0이 되면 게임이 끝나버립니다.";break;
	case 6:
		m_text="마력은 마법을 쓰는데 사용됩니다.";break;
	case 7:
		m_text="예를 들어 해독마법 <히히아나퍼>를 쓰면 마력이 10감소합니다.";break;
	case 8:
		m_text="경험이 10이 되면 레벨이 오릅니다.";break;
	case 9:
		m_text="그리고 적을 없에다 보면 보스가 나옵니다.";break;
	case 10:
		m_text="보스를 없애면 다음 판으로 넘어갑니다.";break;
	case 11:
		m_text="그러면 아이템에 대하여 설명을 하겠습니다.";break;
	case 12:
		m_text="아이템을 쓰려면 아이템의 이름을 치면 됩니다.";break;
	case 13:
		m_text="약초는 체력 10을 회복해 줍니다.";break;
	case 14:
		m_text="마약은 마력 10을 회복해 줍니다.";break;
	case 15:
		m_text="그리고 이 게임은 게임중에 상점 등을 이용할 수 있습니다.";break;
	case 16:
		m_text="끝판에 돈을 쓸 수 없다는 점을 보완한 거죠.";break;
	case 17:
		m_text="상점 등을 이용할때에는 메뉴를 치면 됩니다.";break;
	case 18:
		m_text="예를 들어, <약초사기>라고 말입니다.";break;
	case 19:
		m_text="그리고 몬스터 중에는 독이나 마비를 쓰는 놈이 있습니다.";break;
	case 20:
		m_text="걸렸을 때는 주문을 외우거나 병원에서 치료를 받습니다.";break;
	case 21:
		m_text="드디어 비기 공개!!!";break;
	case 22:
		m_text="<피가모가가>라 치면 체력이 10회복, 마력 30소모";break;
	case 23:
		m_text="<꾸어엉꾸어엉>라 치면 모든 적이 마비, 마력 60소모";break;
	case 24:
		m_text="<뒈져부럿!>라 치면 모든 적에게 1데미지, 마력 100소모";break;
	case 25:
		m_text="이상끝!";break;
	case 26:
		MessageBox("대단하군요! 끝까지 다 읽다니...","마법서");OnOK();break;
	}
	m_spage=itoa(m_page+1,buffer,10);
	UpdateData(false);
}

void CBook::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	InvalidateRect(CRect(400,72,440,92));
	if(m_time==0){
		MessageBox("본책은 2분이상 못 읽음!","마법서");
		OnOK();
	}
	m_time--;
	CDialog::OnTimer(nIDEvent);
}

void CBook::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	dc.SetBkMode(8);
	dc.TextOut(400,72,itoa(m_time,buffer,10));

	// Do not call CDialog::OnPaint() for painting messages
}

void CBook::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	KillTimer(1);
}
