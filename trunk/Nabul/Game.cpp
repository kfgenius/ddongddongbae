// Game.cpp : implementation file
//

#include "stdafx.h"
#include "Nabul.h"
#include "Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGame dialog

CString m_text[56]=
{"시드니","올림픽","봉화","태권도","하키","싸이클","야구","축구",
 "수영","체조","복싱","유도","레슬링","유니존","마라톤","발리볼",
 "배구","평행대","육상","성화","호주","2000","수구","다이빙",
 "투포환","복싱","권투","원반","창","높이","멀리","빠르게",
 "미국","북한","한반도","한국","중국","일본","베트남","사우디",
 "원주민","캥거루","코알라","핸드볼","손공","싸커","발레","장대",
 "사격","승마","카누","카약","양궁","자전거","허들","달리기"};
CString m_bost[7]=
{"나랏말쓰미듕국과서르달라","서로사맛디아니할새",
"이런전챠로하야어린백성이","니르고저할배있어도",
"그뜻을시러펴지못하니라","내이를어여삐여기어",
"스믈넉자를맹가노니"};

int m_att[10]={1,2,3,3,4,2,0,2,10,5};
int m_def[10]={1,1,1,2,2,3,1,1,1,3};

CGame::CGame(CWnd* pParent /*=NULL*/)
	: CDialog(CGame::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGame)
	m_type = _T("");
	//}}AFX_DATA_INIT
}


void CGame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGame)
	DDX_Control(pDX, IDC_TYPE, m_type2);
	DDX_Control(pDX, IDC_ITEM2, m_item2);
	DDX_Control(pDX, IDC_ITEM1, m_item1);
	DDX_Control(pDX, IDC_CURE, m_cure);
	DDX_Control(pDX, IDC_ROBE, m_wp2);
	DDX_Control(pDX, IDC_ROD, m_wp1);
	DDX_Text(pDX, IDC_TYPE, m_type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGame, CDialog)
	//{{AFX_MSG_MAP(CGame)
	ON_EN_VSCROLL(IDC_TYPE, OnVscrollType)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_TYPE, OnChangeType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGame message handlers



void CGame::OnVscrollType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	char tmp2[80];
	int i;
	for(i=0;i<strlen(m_type)-2;i++)
		tmp2[i]=m_type[i];
	tmp2[i]='\0';
	for(i=0;i<5;i++)
		if(!strcmp(tmp2,monster[i].text)&&monster[i].res==0){
			monster[i].def--;
			m_exp++;
			if(monster[i].def==0){
				Die(i);
				InitMonster(i);
				m_money++;
				m_wave.Load(IDR_WAVE3,AfxGetInstanceHandle());
				m_wave.Play();
			}
			else
			{
				m_wave.Load(IDR_WAVE10,AfxGetInstanceHandle());
				m_wave.Play();
			}
			if(m_exp>=100){
				m_wave.Load(IDR_WAVE8,AfxGetInstanceHandle());
				m_wave.Play();
				m_lv++;
				m_hpmax+=10;
				m_mpmax+=10;
				m_exp=0;
			}
			InvalidateRect(CRect(155,395,400,460));
		}
	if(!strcmp(tmp2,m_bost[m_stage])&&m_boss>50){
			m_bosp--;
			m_wave.Load(IDR_WAVE10,AfxGetInstanceHandle());
			m_wave.Play();
		}
	if(!strcmp(tmp2,"미래를향해")&&m_stage<6){
			m_stage++;
			next();
		}
	if(!strcmp(tmp2,"다덤비라")){
		m_hpmax=999;
		m_mpmax=999;
	}
	if(!strcmp(tmp2,"용기와지혜")){
		m_hp=m_hpmax;
		m_mp=m_mpmax;
	}
	if(!strcmp(tmp2,"화수분"))m_money=999;
	if(!strcmp(tmp2,"동흥의노래")){
		m_wave.Load(IDR_WAVE7,AfxGetInstanceHandle());
		m_wave.Play();
	}	
	if(!strcmp(tmp2,"약초사기")&&m_money>=10){
		m_wave.Load(IDR_WAVE6,AfxGetInstanceHandle());
		m_wave.Play();
		m_herb++;
		m_money-=10;
	}
	if(!strcmp(tmp2,"마약사기")&&m_money>=10){
		m_wave.Load(IDR_WAVE6,AfxGetInstanceHandle());
		m_wave.Play();
		m_drug++;
		m_money-=10;
	}
	if(!strcmp(tmp2,"지팡이개조")&&m_money>=50&&m_rod<3){
		m_wave.Load(IDR_WAVE6,AfxGetInstanceHandle());
		m_wave.Play();
		m_rod++;
		m_money-=50;
	}
	if(!strcmp(tmp2,"마법복개조")&&m_money>=50&&m_robe<3){
		m_wave.Load(IDR_WAVE6,AfxGetInstanceHandle());
		m_wave.Play();
		m_robe++;
		m_money-=50;
	}
	if(!strcmp(tmp2,"히히아나파")&&m_mp>=10&&m_condition==1){
		m_wave.Load(IDR_WAVE1,AfxGetInstanceHandle());
		m_wave.Play();
		m_mp-=10;
		m_condition=0;
	}
	if(!strcmp(tmp2,"해독")&&m_money>=10&&m_condition==1){
		m_wave.Load(IDR_WAVE1,AfxGetInstanceHandle());
		m_wave.Play();
		m_money-=10;
		m_condition=0;
	}
	if(!strcmp(tmp2,"피가모가가")&&m_mp>=30){
		m_wave.Load(IDR_WAVE1,AfxGetInstanceHandle());
		m_wave.Play();
		m_mp-=30;
		m_hp+=10;
		if(m_hp>m_hpmax)m_hp=m_hpmax;
	}
	if(!strcmp(tmp2,"꾸어엉꾸어엉")&&m_mp>=60){
		m_wave.Load(IDR_WAVE5,AfxGetInstanceHandle());
		m_wave.Play();
		m_mp-=60;
		for(i=0;i<5;i++)
			monster[i].prz=20;
	}
	if(!strcmp(tmp2,"뒈져부럿!")&&m_mp>=100){
		m_wave.Load(IDR_WAVE2,AfxGetInstanceHandle());
		m_wave.Play();
		m_mp-=100;
		for(i=0;i<5;i++)
			InitMonster(i);
	}
	if(!strcmp(tmp2,"약초")&&m_herb>0){
		m_wave.Load(IDR_WAVE6,AfxGetInstanceHandle());
		m_wave.Play();
		m_hp+=10;
		if(m_hp>m_hpmax)m_hp=m_hpmax;
		m_herb--;
	}
	if(!strcmp(tmp2,"마약")&&m_drug>0){
		m_wave.Load(IDR_WAVE6,AfxGetInstanceHandle());
		m_wave.Play();
		m_mp=m_mpmax;
		m_drug--;
	}	
	m_type="";
	UpdateData(false);
}

void CGame::OnPaint() 
{
	CClientDC dc1(this); // device context for painting
		
	// TODO: Add your message handler code here
	HBITMAP hbitmap1;
	HDC hMemDC1;
//장비
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_ITEM));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,20,402,50,50,hMemDC1,m_rod*50,0,50,50,SRCCOPY);
	::StretchBlt(dc1.m_hDC,70,402,50,50,hMemDC1,m_robe*50,50,50,50,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);

//상태
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_FACE));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,100,340,50,50,hMemDC1,m_condition*50,0,50,50,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
//나불나불
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_MAGE));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,160,350,25,45,hMemDC1,0,0,25,45,SRCPAINT);
	::StretchBlt(dc1.m_hDC,160,350,25,45,hMemDC1,25,0,25,45,SRCAND);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
//관문
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,0,340,100,50,hMemDC1,0,m_stage*50,100,50,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);

	Data();
	// Do not call CDialog::OnPaint() for painting messages
}

BOOL CGame::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	srand( (unsigned)time( NULL ) );
	m_back=m_over2=m_bosss=m_over=m_boss=m_prz=m_stage=m_shield=m_rod=m_robe=m_money=m_condition=m_herb=m_drug=m_exp=0;
	m_hp=m_hpmax=m_mp=m_mpmax=10;
	m_bosp=5;
	m_lv=1;

	m_item1.EnableWindow(false);
	m_item2.EnableWindow(false);
	m_wp1.EnableWindow(false);
	m_wp2.EnableWindow(false);
	m_cure.EnableWindow(false);

	for(int i=0;i<5;i++)InitMonster(i);
	SetTimer(1,1000,NULL);
	m_midi.Open("stage1.mid");
	m_midi.Play();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGame::Data()
{
	CPaintDC dc(this);
	dc.SetBkMode(7);
	char tmp[80];
	dc.TextOut(198,398,itoa(m_lv,buffer,10));
	dc.TextOut(155,419,itoa(m_money,buffer,10));
	dc.TextOut(155,440,itoa(m_herb,buffer,10));
	dc.TextOut(213,440,itoa(m_drug,buffer,10));
	//경험
	strcpy(tmp,itoa(m_exp,buffer,10));
	strcat(tmp," /100");
	dc.TextOut(280,440,tmp);
	if(m_hp<0)m_hp=0;
	//체력
	strcpy(tmp,itoa(m_hp,buffer,10));
	strcat(tmp," /");
	strcat(tmp,itoa(m_hpmax,buffer,10));
	dc.TextOut(280,395,tmp);
	//마력
	strcpy(tmp,itoa(m_mp,buffer,10));
	strcat(tmp," /");
	strcat(tmp,itoa(m_mpmax,buffer,10));
	dc.TextOut(280,418,tmp);
}

void CGame::shield()
{
	CClientDC dc1(this); // device context for painting
		
	// TODO: Add your message handler code here
	HBITMAP hbitmap1;
	HDC hMemDC1;
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_SHIELD));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	for(int i=0;i<14;i++){
		::StretchBlt(dc1.m_hDC,i*25,300,25,20,hMemDC1,m_shield*25,0,25,20,SRCPAINT);
		::StretchBlt(dc1.m_hDC,i*25,300,25,20,hMemDC1,m_shield*25,20,25,20,SRCAND);
	}
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
}

void CGame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	m_time++;
	if(m_time>100)m_time=0;
	if(m_bosp<1){
		m_wave.Load(IDR_WAVE4,AfxGetInstanceHandle());
		m_wave.Play();
		m_stage++;
		next();
	}
	mon();
	shield();
	shop();
	music();
	if(m_prz>0)m_prz--;
	else if(m_condition==2)m_condition=0;	
	if(m_shield==0)m_shield=1;
		else m_shield=0;
	if(m_mp<m_mpmax)m_mp+=m_rod;
	if(m_mp>m_mpmax)m_mp=m_mpmax;
	if(m_condition==1)m_hp--;
	InvalidateRect(CRect(155,395,400,460));
	if(m_hp<1){
	m_type2.EnableWindow(false);
	gameover();
	}
	CDialog::OnTimer(nIDEvent);
}



void CGame::mon()
{
	CClientDC dc1(this); // device context for painting
	
	HBITMAP hbitmap1;
	HDC hMemDC1;
		
	switch(m_stage){
	case 0:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE1));break;
	case 1:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE2));break;
	case 2:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE3));break;
	case 3:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE4));break;
	case 4:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE5));break;
	case 5:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE6));break;
	case 6:	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_STAGE7));break;
	}
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,0,0,350,340,hMemDC1,0,0,350,340,SRCCOPY);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);

	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_MON));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	for(int i=0;i<5;i++){
		if(monster[i].res>0)
			monster[i].res--;
		if(monster[i].prz>0)
			monster[i].prz--;
		if(monster[i].res>0)
			continue;

		if(monster[i].prz==0)monster[i].pos+=10;
		if(monster[i].pos>150){
			if(monster[i].att-m_robe>0)m_hp-=(monster[i].att-m_robe);
			if(monster[i].mon==2||monster[i].mon==5){
			m_wave.Load(IDR_WAVE9,AfxGetInstanceHandle());
			m_wave.Play();
			}
			switch(monster[i].mon){
				case 2:m_condition=1;break;
				case 5:m_condition=2;m_prz=5;;break;
				case 6:
				case 7:if(m_money>0)m_money--;break;
				case 8:m_mp=0;break;
			}
			InvalidateRect(CRect(155,395,400,450));
			InitMonster(i);
		}
		if(monster[i].res==0){
			::StretchBlt(dc1.m_hDC,10+70*i,100+monster[i].pos,50,50,hMemDC1,monster[i].mon*50,0,50,50,SRCPAINT);
			::StretchBlt(dc1.m_hDC,10+70*i,100+monster[i].pos,50,50,hMemDC1,monster[i].mon*50,50,50,50,SRCAND);
			switch(monster[i].def){
			case 1:dc1.SetTextColor(RGB(0,0,0));break;
			case 2:dc1.SetTextColor(RGB(255,0,0));break;
			case 3:dc1.SetTextColor(RGB(0,0,255));break;
			}
		if(monster[i].mon==8&&!(m_time%4))monster[i].text=m_text[rand()%56];
		dc1.TextOut(10+70*i,90+monster[i].pos,monster[i].text);
		}
	}
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);

//보스"
	if(m_boss>50){
		if(m_bosss==0&&m_over2==0){
			m_bosss=1;
			m_midi.Close();
			m_midi.Open("boss.mid");
		}
		hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BOSS));
		hMemDC1 = ::CreateCompatibleDC(NULL);
		SelectObject(hMemDC1,hbitmap1);
		::StretchBlt(dc1.m_hDC,150,50,100,50,hMemDC1,0,m_stage*50,100,50,SRCPAINT);
		::StretchBlt(dc1.m_hDC,150,50,100,50,hMemDC1,100,m_stage*50,100,50,SRCAND);
		::DeleteDC(hMemDC1);
		::DeleteObject(hbitmap1);
		dc1.SetTextColor(RGB(0,m_bosp*20,0));
		dc1.TextOut(100,30,m_bost[m_stage]);
	}
}

void CGame::InitMonster(int num)
{
	CClientDC dc1(this); // device context for painting

	monster[num].prz=0;
	monster[num].pos=0;
	monster[num].text=m_text[rand()%56];
	monster[num].mon=0+(rand()%2);
	switch(m_stage){
	case 1:monster[num].mon+=2;break;
	case 2:if(monster[num].mon==1)monster[num].mon=4;break;
	case 3:if(monster[num].mon==0)monster[num].mon=3;
			   else monster[num].mon=5;break;
	case 4:monster[num].mon+=6;break;
	case 5:if(monster[num].mon==0)monster[num].mon=4;
			   else monster[num].mon=9;break;
	case 6:monster[num].mon=8;break;
	}
	monster[num].att=m_att[monster[num].mon];
	monster[num].def=m_def[monster[num].mon];
	monster[num].res=0+(rand()%10);
	m_boss++;
}

void CGame::shop()
{
	if(m_money>=10){
	m_item1.EnableWindow(true);
	m_item2.EnableWindow(true);
	if(m_condition==1)m_cure.EnableWindow(true);
		else m_cure.EnableWindow(false);
	}
	if(m_money>=50){
		if(m_rod==3)m_wp1.EnableWindow(false);
			else m_wp1.EnableWindow(true);
		if(m_robe==3)m_wp2.EnableWindow(false);
			else m_wp2.EnableWindow(true);
	}
	if(m_money<10){
		m_item1.EnableWindow(false);
		m_item2.EnableWindow(false);
		m_wp1.EnableWindow(false);
		m_wp2.EnableWindow(false);
		m_cure.EnableWindow(false);
	}
}

void CGame::next()
{
	
	m_bosss=m_boss=0;
	m_bosp=5+m_stage;
	m_midi.Close();
	switch(m_stage){
	case 1:SetTimer(1,900,NULL);m_midi.Open("stage2.mid");break;
	case 2:SetTimer(1,800,NULL);m_midi.Open("stage3.mid");break;
	case 3:SetTimer(1,900,NULL);m_midi.Open("stage4.mid");break;
	case 4:SetTimer(1,300,NULL);m_midi.Open("stage5.mid");break;
	case 5:SetTimer(1,700,NULL);m_midi.Open("stage6.mid");break;
	case 6:SetTimer(1,700,NULL);m_midi.Open("stage7.mid");break;
	case 7:OnOK();
		m_end.DoModal();
	}	
}

void CGame::gameover()
{
	CClientDC dc1(this); // device context for painting
	if(m_over2==0){
		m_midi.Close();
		m_midi.Open("GameOver.mid");
		m_over2=1;
	}
	// TODO: Add your message handler code here
	HBITMAP hbitmap1;
	HDC hMemDC1;
	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_GAMEOVER));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,50,150,270,80,hMemDC1,0,0,270,80,SRCPAINT);
	::StretchBlt(dc1.m_hDC,50,150,270,80,hMemDC1,0,80,270,80,SRCAND);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
}

void CGame::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	m_midi.Stop();
	m_midi.Close();
	KillTimer(1);
}

void CGame::OnChangeType() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if(m_prz>0)UpdateData(false);	
}

void CGame::music()
{
	if(!m_midi.IsPlaying()){
		m_midi.SeekTo(0,0);
		m_midi.Play();
	}
}

void CGame::Die(int i)
{
	CClientDC dc1(this); // device context for painting
		
	// TODO: Add your message handler code here
	HBITMAP hbitmap1;
	HDC hMemDC1;

	hbitmap1= ::LoadBitmap(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_DIE));
	hMemDC1 = ::CreateCompatibleDC(NULL);
	SelectObject(hMemDC1,hbitmap1);
	::StretchBlt(dc1.m_hDC,10+70*i,100+monster[i].pos,50,50,hMemDC1,0,0,50,50,SRCPAINT);
	::StretchBlt(dc1.m_hDC,10+70*i,100+monster[i].pos,50,50,hMemDC1,0,50,50,50,SRCAND);
	::DeleteDC(hMemDC1);
	::DeleteObject(hbitmap1);
}
