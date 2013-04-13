// MasterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Master.h"
#include "MasterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMasterDlg dialog

CMasterDlg::CMasterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMasterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMasterDlg)
	m_att = 0;
	m_def = 0;
	m_int = 0;
	m_luck = 0;
	m_hp = 0;
	m_mp = 0;
	m_damage = 0;
	m_level = 0;
	m_hpMax = 0;
	m_mpMax = 0;
	m_kind = -1;
	m_minus = 0;
	m_what = _T("");
	m_name = _T("");
	m_class = _T("");
	m_Cname = _T("");
	m_money = 0;
	m_book = 0;
	m_arm = 0;
	m_wp = 0;
	m_etc = 0;
	m_effect = _T("MP");
	m_top = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMasterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMasterDlg)
	DDX_Control(pDX, IDC_KIND, m_ctrlKind);
	DDX_Control(pDX, IDC_DEL, m_delete);
	DDX_Control(pDX, IDC_CMAG, m_item);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_ATT, m_att);
	DDX_Text(pDX, IDC_DEF, m_def);
	DDX_Text(pDX, IDC_INT, m_int);
	DDX_Text(pDX, IDC_LUK, m_luck);
	DDX_Text(pDX, IDC_HP, m_hp);
	DDX_Text(pDX, IDC_MP, m_mp);
	DDX_Text(pDX, IDC_DAMAGE, m_damage);
	DDX_Text(pDX, IDC_LEV, m_level);
	DDX_Text(pDX, IDC_HPMAX, m_hpMax);
	DDX_Text(pDX, IDC_MPMAX, m_mpMax);
	DDX_CBIndex(pDX, IDC_KIND, m_kind);
	DDX_Text(pDX, IDC_MINUS, m_minus);
	DDX_Text(pDX, IDC_WHAT, m_what);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_CLASS, m_class);
	DDX_Text(pDX, IDC_CNAME, m_Cname);
	DDX_Text(pDX, IDC_MONEY, m_money);
	DDX_Text(pDX, IDC_BOOK, m_book);
	DDX_Text(pDX, IDC_ARM, m_arm);
	DDX_Text(pDX, IDC_WP, m_wp);
	DDX_Text(pDX, IDC_ETC, m_etc);
	DDX_Text(pDX, IDC_EFFECT, m_effect);
	DDX_Check(pDX, IDC_TOP, m_top);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMasterDlg, CDialog)
	//{{AFX_MSG_MAP(CMasterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_LEVEL, OnLevel)
	ON_BN_CLICKED(IDC_CATT, OnCatt)
	ON_BN_CLICKED(IDC_CDEF, OnCdef)
	ON_BN_CLICKED(IDC_CMAG, OnCmag)
	ON_BN_CLICKED(IDC_REST, OnRest)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_GET, OnGet)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_LBN_SELCHANGE(IDC_LIST, OnSelchangeList)
	ON_CBN_SELCHANGE(IDC_KIND, OnSelchangeKind)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMasterDlg message handlers

BOOL CMasterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	srand((unsigned)time(NULL));
	m_kind=0;
	UpdateData(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMasterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMasterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMasterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMasterDlg::OnStart() 
{
	// TODO: Add your control notification handler code here
	m_att=rand()%18+1;
	m_def=rand()%18+1;
	m_int=rand()%18+1;
	m_luck=rand()%18+1;
	m_hp=m_hpMax=m_att*2+m_def*3;
	m_mp=m_mpMax=m_int;
	m_level=1;
	m_money=0;
	m_wp=m_arm=m_book=m_etc=1;
	int count=m_list.GetCount();
	for(int i=0;i<count;i++)
		m_list.DeleteString(0);
	UpdateData(false);
}

void CMasterDlg::OnLevel() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	m_att+=rand()%3;
	m_def+=rand()%3;
	m_int+=rand()%3;
	m_luck+=rand()%3;
	m_level++;
	m_hpMax=m_att*2+m_def*3;
	m_mpMax=m_int;
	UpdateData(false);
}

void CMasterDlg::OnCatt() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	int crt=rand()%10;
	crt=(!crt)?2:1;
	m_damage=m_att*crt+(rand()%(m_luck+1));
	UpdateData(false);
}

void CMasterDlg::OnCdef() 
{
	UpdateData(true);
	if(m_damage>0)	//데미지 받기
	{
		int hit=(m_damage-(m_def/2));
		if(hit<0)hit=0;
		m_hp-=hit;
		if(m_hp<0)m_hp=0;
	}
	else if(m_damage<0)	//회복받기
	{
		m_hp-=m_damage;
		if(m_hp>m_hpMax)m_hp=m_hpMax;
	}
	UpdateData(false);
}

void CMasterDlg::OnCmag() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CString m_tmp;
	int m_to, m_case;
	m_list.GetText(m_listNum, m_tmp);
	CString m_org=m_tmp;

	m_case=Item(m_tmp.Left(6));
	if(m_case==99)return;

	m_to=m_tmp.FindOneOf(")");
	m_tmp=m_tmp.Left(m_to);
	m_to=m_tmp.FindOneOf("(");
	m_tmp=m_tmp.Right(m_tmp.GetLength()-m_to-1);
	int plus=atoi(m_tmp);
	
	switch(m_case)
	{
		case 0:	if(m_mp<plus){
					MessageBox("MP가 없습니다.","안됐수");
					return;
				}
				m_damage=m_int+(rand()%(m_luck+1))+(plus*5);
				m_mp-=plus;
				break;
		case 1:	if(m_mp<plus){
					MessageBox("MP가 없습니다.","안됐수");
					return;
				}
				m_damage=plus*10*-1;
				m_mp-=plus;
				break;
		case 2: m_hp+=plus;
				if(m_hp>m_hpMax)m_hp=m_hpMax;
				m_list.DeleteString(m_listNum);
				m_listNum=-1;
				break;
		case 3: m_mp+=plus;
				if(m_mp>m_mpMax)m_mp=m_mpMax;
				m_list.DeleteString(m_listNum);
				m_listNum=-1;
				break;
		case 8:	case 9: case 10: case 11:
				Equip(m_listNum);
				break;
		case 4:	case 5: case 6:	case 7:
				CString etmp;
				int count=m_list.GetCount(), count2=0;
				for(int i=0; i< count; i++)
				{
					m_list.GetText(i,etmp);
					if(Item(etmp.Left(6))==(m_case+4))count2++;
				}
				bool fail=false;
				if(m_case==4 && count2>=m_wp)fail=true;
				if(m_case==5 && count2>=m_arm)fail=true;
				if(m_case==6 && count2>=m_book)fail=true;
				if(m_case==7 && count2>=m_etc)fail=true;
				if(fail)
				{
					MessageBox("더 이상 장비할 수 없습니다.","안됐수");
					return;
				}

				m_org="*"+m_org;
				m_list.DeleteString(m_listNum);
				m_list.AddString(m_org);
				if(m_case==4)m_att+=plus;
				if(m_case==5)m_def+=plus;
				if(m_case==6)m_int+=plus;
				if(m_case==7)m_luck+=plus;
				break;
	}

	UpdateData(false);
}

void CMasterDlg::OnRest() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	m_hp=m_hpMax;
	m_mp=m_mpMax;
	UpdateData(false);
}

void CMasterDlg::OnSave() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CFileDialog fileDlg(false, "*.csf", "*.csf", 0,
		"캐릭터시트파일 (*.csf)|*.csf||");
	if (fileDlg.DoModal()==IDCANCEL) return;
	CString m_fname=fileDlg.GetFileName(), list;
	CFile m_file; int count=m_list.GetCount();

	m_file.Open(m_fname,CFile::modeCreate | CFile::modeWrite);
	CArchive ar(&m_file, CArchive::store);
	ar << m_Cname << m_class << m_level << m_hp << m_hpMax
		<< m_mp << m_mpMax << m_att << m_def << m_int << m_luck
		<< m_money << count;
	for(int i=0; i< count; i++)
	{
		m_list.GetText(i, list);
		ar << list;
	}
	ar << m_wp << m_arm << m_book << m_etc;
	ar.Close();
	m_file.Close();
}

void CMasterDlg::OnLoad() 
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDlg(true, "*.csf", "*.csf", 0,
		"캐릭터시트파일 (*.csf)|*.csf||");
	if (fileDlg.DoModal()==IDCANCEL) return;
	CString m_fname=fileDlg.GetFileName(), list;
	CFile m_file; int count;

	count=m_list.GetCount();
	for(int i=0; i< count; i++)
		m_list.DeleteString(0);

	m_file.Open(m_fname,CFile::modeRead);
	CArchive ar(&m_file, CArchive::load);
	ar >> m_Cname >> m_class >> m_level >> m_hp >> m_hpMax
		>> m_mp >> m_mpMax >> m_att >> m_def >> m_int >> m_luck
		>> m_money >> count;
	for(i=0; i< count; i++)
	{
		ar >> list;
		m_list.AddString(list);
	}
	ar >> m_wp >> m_arm >> m_book >> m_etc;
	ar.Close();
	m_file.Close();
	UpdateData(false);
}

void CMasterDlg::OnGet() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CString name[]={"공격","회복","음식","MP약","무기","갑옷","지식","장식","기타"};
	CString Get;
	Get.Format("[%s]%s(%d): %s",name[m_kind], m_name, m_minus, m_what);
	m_list.AddString(Get);
}

void CMasterDlg::OnDel() 
{
	m_list.DeleteString(m_listNum);
	m_listNum=-1;
}

void CMasterDlg::OnSelchangeList() 
{
	CString m_tmp;
	m_listNum=m_list.GetCurSel();
	m_list.GetText(m_listNum, m_tmp);
	if(Item(m_tmp.Left(6))<4)SetDlgItemText(IDC_CMAG,"사용");
		else if(Item(m_tmp.Left(6))<8)SetDlgItemText(IDC_CMAG,"장비");
		else SetDlgItemText(IDC_CMAG,"해체");
	if(Item(m_tmp.Left(6))==99)m_item.EnableWindow(false);
		else m_item.EnableWindow(true);
	if(Item(m_tmp.Left(6))>=8 && Item(m_tmp.Left(6))<12)m_delete.EnableWindow(false);
		else m_delete.EnableWindow(true);
}

int CMasterDlg::Item(CString name)
{
	int m_case;
	if(name=="[공격]")m_case=0;
	if(name=="[회복]")m_case=1;
	if(name=="[음식]")m_case=2;
	if(name=="[MP약]")m_case=3;
	if(name=="[무기]")m_case=4;
	if(name=="[갑옷]")m_case=5;
	if(name=="[지식]")m_case=6;
	if(name=="[장식]")m_case=7;
	if(name=="*[무기")m_case=8;
	if(name=="*[갑옷")m_case=9;
	if(name=="*[지식")m_case=10;
	if(name=="*[장식")m_case=11;
	if(name=="[기타]")m_case=99;
	return m_case;
}

void CMasterDlg::Equip(int num)
{
	CString equip;
	m_list.GetText(num, equip);
	int m_case=Item(equip.Left(6));
	equip=equip.Right(equip.GetLength()-1);
	m_list.DeleteString(num);
	m_list.AddString(equip);

	int m_to=equip.FindOneOf(")");
	equip=equip.Left(m_to);
	m_to=equip.FindOneOf("(");
	equip=equip.Right(equip.GetLength()-m_to-1);
	int plus=atoi(equip);

	if(m_case==8)m_att-=plus;
	if(m_case==9)m_def-=plus;
	if(m_case==10)m_int-=plus;
	if(m_case==11)m_luck-=plus;
}

void CMasterDlg::OnSelchangeKind() 
{
	UpdateData(true);
	if(m_ctrlKind.GetCurSel()<2)m_effect="MP";
		else m_effect="효과";
	UpdateData(false);
}

void CMasterDlg::OnTop() 
{
	UpdateData(true);
	if(m_top)
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	else
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	UpdateData(false);
}
