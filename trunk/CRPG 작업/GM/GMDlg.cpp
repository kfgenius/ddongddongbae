// GMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GM.h"
#include "GMDlg.h"

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
// CGMDlg dialog

CGMDlg::CGMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGMDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGMDlg)
	m_player = -1;
	m_turn = _T("");
	m_angle = 0;
	m_dice = _T("");
	m_name1 = _T("");
	m_name3 = _T("");
	m_name4 = _T("");
	m_name5 = _T("");
	m_name6 = _T("");
	m_name2 = _T("");
	m_event = _T("");
	m_EventName = _T("");
	m_tr = _T("");
	m_spot = _T("");
	m_class1 = _T("");
	m_class2 = _T("");
	m_class3 = _T("");
	m_class4 = _T("");
	m_class5 = _T("");
	m_class6 = _T("");
	m_dir3 = _T("");
	m_dir4 = _T("");
	m_dir2 = _T("");
	m_memo2 = _T("");
	m_state3 = _T("");
	m_memo3 = _T("");
	m_state4 = _T("");
	m_memo4 = _T("");
	m_dir5 = _T("");
	m_state5 = _T("");
	m_memo5 = _T("");
	m_dir6 = _T("");
	m_state6 = _T("");
	m_dir1 = _T("");
	m_memo6 = _T("");
	m_state1 = _T("");
	m_memo1 = _T("");
	m_state2 = _T("");
	m_top = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGMDlg)
	DDX_Control(pDX, IDC_TRLIST, m_trList);
	DDX_Control(pDX, IDC_EVENTLIST, m_eventlist);
	DDX_CBIndex(pDX, IDC_PLAYER, m_player);
	DDX_Text(pDX, IDC_TURN, m_turn);
	DDX_Text(pDX, IDC_ANGLE, m_angle);
	DDX_Text(pDX, IDC_DICETEXT, m_dice);
	DDX_Text(pDX, IDC_EDIT1, m_name1);
	DDX_Text(pDX, IDC_EDIT11, m_name3);
	DDX_Text(pDX, IDC_EDIT16, m_name4);
	DDX_Text(pDX, IDC_EDIT21, m_name5);
	DDX_Text(pDX, IDC_EDIT26, m_name6);
	DDX_Text(pDX, IDC_EDIT6, m_name2);
	DDX_Text(pDX, IDC_EVENT, m_event);
	DDX_Text(pDX, IDC_EVENTNAME, m_EventName);
	DDX_Text(pDX, IDC_TR, m_tr);
	DDX_Text(pDX, IDC_SPOT, m_spot);
	DDX_Text(pDX, IDC_EDIT2, m_class1);
	DDX_Text(pDX, IDC_EDIT7, m_class2);
	DDX_Text(pDX, IDC_EDIT12, m_class3);
	DDX_Text(pDX, IDC_EDIT17, m_class4);
	DDX_Text(pDX, IDC_EDIT22, m_class5);
	DDX_Text(pDX, IDC_EDIT27, m_class6);
	DDX_Text(pDX, IDC_EDIT13, m_dir3);
	DDX_Text(pDX, IDC_EDIT18, m_dir4);
	DDX_Text(pDX, IDC_EDIT8, m_dir2);
	DDX_Text(pDX, IDC_EDIT10, m_memo2);
	DDX_Text(pDX, IDC_EDIT14, m_state3);
	DDX_Text(pDX, IDC_EDIT15, m_memo3);
	DDX_Text(pDX, IDC_EDIT19, m_state4);
	DDX_Text(pDX, IDC_EDIT20, m_memo4);
	DDX_Text(pDX, IDC_EDIT23, m_dir5);
	DDX_Text(pDX, IDC_EDIT24, m_state5);
	DDX_Text(pDX, IDC_EDIT25, m_memo5);
	DDX_Text(pDX, IDC_EDIT28, m_dir6);
	DDX_Text(pDX, IDC_EDIT29, m_state6);
	DDX_Text(pDX, IDC_EDIT3, m_dir1);
	DDX_Text(pDX, IDC_EDIT30, m_memo6);
	DDX_Text(pDX, IDC_EDIT4, m_state1);
	DDX_Text(pDX, IDC_EDIT5, m_memo1);
	DDX_Text(pDX, IDC_EDIT9, m_state2);
	DDX_Check(pDX, IDC_TOP, m_top);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGMDlg, CDialog)
	//{{AFX_MSG_MAP(CGMDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_LBN_SELCHANGE(IDC_EVENTLIST, OnSelchangeEventlist)
	ON_BN_CLICKED(IDC_DICE, OnDice)
	ON_BN_CLICKED(IDC_ADD2, OnAdd2)
	ON_BN_CLICKED(IDC_DEL2, OnDel2)
	ON_LBN_SELCHANGE(IDC_TRLIST, OnSelchangeTrlist)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGMDlg message handlers

BOOL CGMDlg::OnInitDialog()
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
	m_turnNum=0;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGMDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGMDlg::OnPaint() 
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
HCURSOR CGMDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGMDlg::OnAdd() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CString tmp;
	tmp.Format("[%s] %s",m_EventName,m_event);
	m_eventlist.AddString(tmp);
}

void CGMDlg::OnDel() 
{
	// TODO: Add your control notification handler code here
	m_eventlist.DeleteString(m_eventNum);
	m_eventNum=-1;
}

void CGMDlg::OnNext() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	m_turnNum++;
	if(m_turnNum>(m_player+1))m_turnNum=1;
	switch(m_turnNum)
	{
		case 1: m_turn.Format("%s의 턴",m_name1); break;
		case 2: m_turn.Format("%s의 턴",m_name2); break;
		case 3: m_turn.Format("%s의 턴",m_name3); break;
		case 4: m_turn.Format("%s의 턴",m_name4); break;
		case 5: m_turn.Format("%s의 턴",m_name5); break;
		case 6: m_turn.Format("%s의 턴",m_name6); break;
	}
	UpdateData(false);
}

void CGMDlg::OnSelchangeEventlist() 
{
	// TODO: Add your control notification handler code here
	m_eventNum=m_eventlist.GetCurSel();
}

void CGMDlg::OnDice() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	int dice;
	if(m_angle>0)dice=rand()%m_angle+1;
		else return;
	m_dice.Format("%d",dice);
	UpdateData(false);
}

void CGMDlg::OnAdd2() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CString tmp;
	tmp.Format("[%s] %s",m_spot,m_tr);
	m_trList.AddString(tmp);
}

void CGMDlg::OnDel2() 
{
	// TODO: Add your control notification handler code here
	m_trList.DeleteString(m_trNum);
	m_trNum=-1;
}

void CGMDlg::OnSelchangeTrlist() 
{
	// TODO: Add your control notification handler code here
	m_trNum=m_trList.GetCurSel();
}

void CGMDlg::OnSave() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CFileDialog fileDlg(false, "*.tsf", "*.tsf", 0,
		"상황저장파일 (*.tsf)|*.tsf||");
	if (fileDlg.DoModal()==IDCANCEL) return;
	CString m_fname=fileDlg.GetFileName(), list;
	CFile m_file;

	m_file.Open(m_fname,CFile::modeCreate | CFile::modeWrite);
	CArchive ar(&m_file, CArchive::store);
	ar << m_player << m_turn;
	ar << m_name1 << m_class1 << m_dir1 << m_state1 << m_memo1
		<< m_name2 << m_class2 << m_dir2 << m_state2 << m_memo2
		<< m_name3 << m_class3 << m_dir3 << m_state3 << m_memo3
		<< m_name4 << m_class4 << m_dir4 << m_state4 << m_memo4
		<< m_name5 << m_class5 << m_dir5 << m_state5 << m_memo5
		<< m_name6 << m_class6 << m_dir6 << m_state6 << m_memo6;
	
	int count=m_eventlist.GetCount();
	ar	<< count;
	for(int i=0; i< count; i++)
	{
		m_eventlist.GetText(i, list);
		ar << list;
	}
	
	count=m_trList.GetCount();
	ar	<< count;
	for(i=0; i< count; i++)
	{
		m_trList.GetText(i, list);
		ar << list;
	}

	ar.Close();
	m_file.Close();
}

void CGMDlg::OnLoad() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	CFileDialog fileDlg(true, "*.tsf", "*.tsf", 0,
		"상황저장파일 (*.tsf)|*.tsf||");
	if (fileDlg.DoModal()==IDCANCEL) return;
	CString m_fname=fileDlg.GetFileName(), list;
	CFile m_file;

	m_file.Open(m_fname,CFile::modeRead);
	CArchive ar(&m_file, CArchive::load);
	ar >> m_player >> m_turn;
	ar >> m_name1 >> m_class1 >> m_dir1 >> m_state1 >> m_memo1
		>> m_name2 >> m_class2 >> m_dir2 >> m_state2 >> m_memo2
		>> m_name3 >> m_class3 >> m_dir3 >> m_state3 >> m_memo3
		>> m_name4 >> m_class4 >> m_dir4 >> m_state4 >> m_memo4
		>> m_name5 >> m_class5 >> m_dir5 >> m_state5 >> m_memo5
		>> m_name6 >> m_class6 >> m_dir6 >> m_state6 >> m_memo6;
	
	int count=m_eventlist.GetCount();
	for(int i=0; i<count; i++)
		m_eventlist.DeleteString(0);
	ar	>> count;
	for(i=0; i< count; i++)
	{
		ar >> list;
		m_eventlist.AddString(list);
	}
	
	count=m_trList.GetCount();
	for(i=0; i<count; i++)
		m_trList.DeleteString(0);
	ar	>> count;
	for(i=0; i< count; i++)
	{
		ar >> list;
		m_trList.AddString(list);
	}

	ar.Close();
	m_file.Close();
	UpdateData(false);
}

void CGMDlg::OnTop() 
{
	UpdateData(true);
	if(m_top)
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	else
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	UpdateData(false);
}
