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
		m_text="�� ������ ������ ���ҳ����Դϴ�.";break;
	case 1:
		m_text="������ ���ҳ����� Ÿ�� ���� �����Դϴ�.";break;
	case 2:
		m_text="������ ���ʿ� ���ڸ� ���̰� �ִµ�";break;
	case 3:
		m_text="�� ���ڸ� ġ�� �� ������ 1�������� �ݴϴ�.";break;
	case 4:
		m_text="�׸��� ���ҳ����� ����,ü��,����,������ �ֽ��ϴ�.";break;
	case 5:
		m_text="ü���� 0�� �Ǹ� ������ ���������ϴ�.";break;
	case 6:
		m_text="������ ������ ���µ� ���˴ϴ�.";break;
	case 7:
		m_text="���� ��� �ص����� <�����Ƴ���>�� ���� ������ 10�����մϴ�.";break;
	case 8:
		m_text="������ 10�� �Ǹ� ������ �����ϴ�.";break;
	case 9:
		m_text="�׸��� ���� ������ ���� ������ ���ɴϴ�.";break;
	case 10:
		m_text="������ ���ָ� ���� ������ �Ѿ�ϴ�.";break;
	case 11:
		m_text="�׷��� �����ۿ� ���Ͽ� ������ �ϰڽ��ϴ�.";break;
	case 12:
		m_text="�������� ������ �������� �̸��� ġ�� �˴ϴ�.";break;
	case 13:
		m_text="���ʴ� ü�� 10�� ȸ���� �ݴϴ�.";break;
	case 14:
		m_text="������ ���� 10�� ȸ���� �ݴϴ�.";break;
	case 15:
		m_text="�׸��� �� ������ �����߿� ���� ���� �̿��� �� �ֽ��ϴ�.";break;
	case 16:
		m_text="���ǿ� ���� �� �� ���ٴ� ���� ������ ����.";break;
	case 17:
		m_text="���� ���� �̿��Ҷ����� �޴��� ġ�� �˴ϴ�.";break;
	case 18:
		m_text="���� ���, <���ʻ��>��� ���Դϴ�.";break;
	case 19:
		m_text="�׸��� ���� �߿��� ���̳� ���� ���� ���� �ֽ��ϴ�.";break;
	case 20:
		m_text="�ɷ��� ���� �ֹ��� �ܿ�ų� �������� ġ�Ḧ �޽��ϴ�.";break;
	case 21:
		m_text="���� ��� ����!!!";break;
	case 22:
		m_text="<�ǰ��𰡰�>�� ġ�� ü���� 10ȸ��, ���� 30�Ҹ�";break;
	case 23:
		m_text="<�پ���پ��>�� ġ�� ��� ���� ����, ���� 60�Ҹ�";break;
	case 24:
		m_text="<�����η�!>�� ġ�� ��� ������ 1������, ���� 100�Ҹ�";break;
	case 25:
		m_text="�̻�!";break;
	case 26:
		MessageBox("����ϱ���! ������ �� �дٴ�...","������");OnOK();break;
	}
	m_spage=itoa(m_page+1,buffer,10);
	UpdateData(false);
}

void CBook::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	InvalidateRect(CRect(400,72,440,92));
	if(m_time==0){
		MessageBox("��å�� 2���̻� �� ����!","������");
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
