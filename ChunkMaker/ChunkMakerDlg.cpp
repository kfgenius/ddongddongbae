// chunkmakerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "chunkmaker.h"
#include "chunkmakerDlg.h"
#include ".\chunkmakerdlg.h"
#include "dataedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CchunkmakerDlg 대화 상자



CchunkmakerDlg::CchunkmakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CchunkmakerDlg::IDD, pParent)
	, project_name(_T("임시프로젝트"))
	, selected_list(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CchunkmakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROJECTNAME, project_name);
	DDV_MaxChars(pDX, project_name, 127);
	DDX_Control(pDX, IDC_LIST, filelist);
	DDX_LBIndex(pDX, IDC_LIST, selected_list);
}

BEGIN_MESSAGE_MAP(CchunkmakerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_EXPORT, OnBnClickedExport)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_EXIT, OnBnClickedExit)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_LOAD, OnBnClickedLoad)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_LBN_DBLCLK(IDC_LIST, OnLbnDblclkList)
	ON_BN_CLICKED(IDC_EDIT, OnBnClickedEdit)
END_MESSAGE_MAP()


// CchunkmakerDlg 메시지 처리기

BOOL CchunkmakerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	ci_count=0;
	::GetCurrentDirectory(256, sdir);
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CchunkmakerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CchunkmakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CchunkmakerDlg::OnBnClickedExport()
{
	char cdir[256];
	strcpy(cdir, sdir);
	strcat(cdir,"\\");

	char temp_path[FILEMAX][256];

	//MLC를 정상적으로 만들 수 있는지 검사
	if(ci_count<=0)return;
	for(int i=0; i<ci_count; i++)
	{
		//같은 이름이 존재
		for(int j=0; j<ci_count; j++)
		{
			if(i!=j && strcmp(m_ci[i].name, m_ci[j].name)==0)
			{
				char msg[256];
				sprintf(msg, "%s라는 이름을 가진 그림이 2개이상 존재합니다.", m_ci[i].name);
				MessageBox(msg);
				return;
			}
		}
		//파일을 찾을 수 없음
		CFile test;
		strcpy(temp_path[i], cdir);
		strcat(temp_path[i], m_ci[i].filepath);
		if(test.Open(temp_path[i], CFile::modeRead))test.Close();
		else
		{
			CString err_msg;
			err_msg.Format("%s 파일을 찾을 수 없습니다!", m_ci[i].filepath);
			MessageBox(err_msg);
			return;
		}
	}

	//chunk 파일 만들기
	UpdateData(true);
	if(project_name=="")strcat(cdir,"이름없음");
		else strcat(cdir,project_name);

	const char szFilters[]="문라이브 덩어리 파일(*.mlc)|*.mlc||";
	CFileDialog m_fdlg (FALSE, "", cdir, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);
	if(m_fdlg.DoModal()==IDOK)
	{
		CFile mlc_file;
		CString mlc_file_name=m_fdlg.GetFileName();
		if(mlc_file.Open(mlc_file_name, CFile::modeCreate | CFile::modeWrite))
		{
			//Chunk파일임을 표시
			mlc_file.Write("CHNK",4);

			for(int i=0; i<ci_count; i++)
			{
				chunkdata filedata;
				picturedata pd;
				ChunkInfo* pChunkInfo=&m_ci[i];

				//파일 정보
				ZeroMemory(filedata.name, 128);
				ZeroMemory(filedata.filename, 128);
				strcpy(filedata.name,pChunkInfo->name);
				strcpy(filedata.filename, pChunkInfo->filename);

				filedata.type=1;

				//그림 정보
				pd.flag=PicInfo_None;
				if(pChunkInfo->layertype==0)
				{
					pd.layertype=PicOverlayNormal;
				}
				else
				{
					pd.flag|=PicInfo_LayerType;
					if(pChunkInfo->layertype==1)pd.layertype=PicOverlayLight;
						else if(pChunkInfo->layertype==2)pd.layertype=PicOverlayDark;
						else if(pChunkInfo->layertype==3)pd.layertype=PicOverlayAddition;
				}
				//불투명도
				if(pChunkInfo->opacity!=1.0f)pd.flag|=PicInfo_Opacity;
				pd.opacity=pChunkInfo->opacity;
				//칼라키
				if(pChunkInfo->colorkey)pd.flag|=PicInfo_ColorKey;
				pd.colorkey=pChunkInfo->color;
				//기타(설정하지 않음)
				pd.sysmem=1;
				ZeroMemory(pd.maskname, 128);

				char* buffer;
				CFile source_file;
				//원본파일에서 내용 읽어오기(자신의 폴더)
				if(source_file.Open(temp_path[i], CFile::modeRead))
				{
					filedata.size=(DWORD)source_file.GetLength();
					buffer=new char[filedata.size];
					source_file.Read(buffer, filedata.size);
					filedata.size+=sizeof(pd);

					source_file.Close();
				}

				mlc_file.Write(&filedata, sizeof(filedata));
				mlc_file.Write(&pd, sizeof(picturedata));
				mlc_file.Write(buffer, filedata.size-sizeof(pd));

				//버퍼 비우기
				delete [] buffer;
				buffer=NULL;
			}

			mlc_file.Close();
		}

		char szResult[80];
		sprintf(szResult, "성공적으로 %s 파일을 생성하였습니다.", mlc_file_name);
		MessageBox(szResult);
	}

	::SetCurrentDirectory(sdir);
}

void CchunkmakerDlg::OnBnClickedAdd()
{
	char cdir[256];
	strcpy(cdir, sdir);
	strcat(cdir,"\\*.gif;*.jpg;*.tif;*.bmp");

	//그림 파일 불러오기
	const char szFilters[]="그림 파일(*.gif,*.jpg,*.tif,*.bmp)|*.gif;*.jpg;*.tif;*.bmp||";
	CFileDialog m_fdlg(TRUE, "", cdir, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY, szFilters, this);
	
	//새 버퍼(파일명이 많을 경우를 대비해서)
	m_fdlg.m_ofn.lpstrFile=(LPSTR)new_buffer;
	m_fdlg.m_ofn.nMaxFile=sizeof(new_buffer)-1;
	strcpy(new_buffer, cdir);

	CString fileName;
	POSITION pos;
	if(m_fdlg.DoModal ()==IDOK )
	{
		UpdateData(true);
		pos=m_fdlg.GetStartPosition();
		while(pos)
		{
			if(ci_count>=FILEMAX)
			{
				CString err_msg;
				err_msg.Format("%d개까지 밖에 넣을 수 없습니다.", FILEMAX);
				break;		//한계치 초과시 더 이상 추가 불가능
			}
			ChunkInfo* pChunkInfo=&m_ci[ci_count];

			//하위 폴더인지 검사
			CString tmp_buffer, file_path;
			bool sub_folder = true;
			tmp_buffer=m_fdlg.GetNextPathName(pos);
			size_t strmax = strlen(sdir);
			for(size_t i=0; i<strmax; ++i)
			{
				if(sdir[i] != tmp_buffer.GetAt(i))
				{
					CString x = tmp_buffer.GetAt(i);
					sub_folder = false;
					break;
				}
			}
			//하위 폴더인 경우
			if(sub_folder)
			{
				file_path="";
				++i;
				for(; i<(size_t)tmp_buffer.GetLength(); ++i)file_path += tmp_buffer.GetAt(i);
			}
			//아닌 경우 절대 경로로
			else
			{
				file_path = tmp_buffer;
			}
			//리스트에 추가	
			strcpy(pChunkInfo->filepath, file_path.GetBuffer(256));	//파일 경로
			file_path.ReleaseBuffer();
			GetFileName(pChunkInfo->filename, pChunkInfo->filepath);					//파일 이름
			//그림 이름
			char title[80];
			strcpy(pChunkInfo->name, GetFileTitle(title, pChunkInfo->filename));
			//칼라키, 투명도
			pChunkInfo->colorkey=false;
			pChunkInfo->opacity=1.0f;
			pChunkInfo->color=0x0000ff;
			pChunkInfo->layertype=0;

			//리스트에 표시할 정보 수정
			CString list_data;
			MakeList(&list_data, ci_count);
			filelist.AddString(list_data);
			ci_count++;
		}
	}

	::SetCurrentDirectory(sdir);
}

void CchunkmakerDlg::OnBnClickedExit()
{
	OnOK();
}

void CchunkmakerDlg::OnBnClickedDelete()
{
	UpdateData(true);
	while(filelist.GetSelCount()>0)
	{
		int sels[1];
		filelist.GetSelItems(1, (LPINT)sels);

		//데이터에서 없애기
		ci_count--;
		for(int i=sels[0]; i<ci_count; i++)memcpy(&m_ci[i], &m_ci[i+1], sizeof(ChunkInfo));
		//리스트에서 없애기
		filelist.DeleteString(sels[0]);
	}
}

void CchunkmakerDlg::OnBnClickedLoad()
{
	char cdir[256];
	strcpy(cdir, sdir);
	strcat(cdir,"\\*.cmp");

	//프로젝트 파일 불러오기
	const char szFilters[]="ChunkMaker 프로젝트 파일(*.cmp)|*.cmp||";
	CFileDialog m_fdlg (TRUE, "", cdir, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
	UpdateData(true);

	CString fileName;
	if(m_fdlg.DoModal ()==IDOK )
	{
		CFile cmp_file;
		if(cmp_file.Open(m_fdlg.GetFileName(), CFile::modeRead))
		{
			//CMP파일인지 확인
			char mark[11];
			cmp_file.Read(mark,10);
			mark[10]=NULL;
			if(strcmp(mark, "ChunkMaker")!=0)
			{
				MessageBox("ChunkMaker 프로젝트 파일이 아닙니다!");
				::SetCurrentDirectory(sdir);
				return;
			}
			//버전
			int ver;
			cmp_file.Read(&ver, sizeof(int));
			//프로젝트 이름
			if(ver>=11)
			{
				int name_length;
				cmp_file.Read(&name_length, sizeof(int));
				char* name_buffer=new char[name_length+1];
				cmp_file.Read(name_buffer, name_length);
				name_buffer[name_length]=NULL;
				project_name=name_buffer;
				delete name_buffer;				
			}
			//암호
			if(ver>=12)
			{
				char key[9];
				char key2[9]="DONGHUNG";
				cmp_file.Read(key, 8);
			}
			//리스트 비우기
			filelist.ResetContent();
			//파일리스트 불러오기
			cmp_file.Read(&ci_count, sizeof(int));
			for(int i=0; i<ci_count; i++)
			{
				cmp_file.Read(&m_ci[i], sizeof(ChunkInfo));
				CString list_data;
				MakeList(&list_data, i);
				filelist.AddString(list_data);
			}

			UpdateData(false);
			cmp_file.Close();
		}
	}	

	::SetCurrentDirectory(sdir);
}

void CchunkmakerDlg::OnBnClickedSave()
{
	char cdir[256];
	strcpy(cdir, sdir);
	strcat(cdir,"\\");
	UpdateData(true);
	if(project_name=="")strcat(cdir,"이름없음");
		else strcat(cdir,project_name);

	//프로젝트 파일 저장하기
	const char szFilters[]="ChunkMaker 프로젝트 파일(*.cmp)|*.cmp||";
	CFileDialog m_fdlg (FALSE, "", cdir, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);

	CString fileName;
	if(m_fdlg.DoModal ()==IDOK )
	{
		CFile cmp_file;
		if(cmp_file.Open(m_fdlg.GetFileName(), CFile::modeCreate | CFile::modeWrite))
		{
			//CMP파일임을 표시
			cmp_file.Write("ChunkMaker",10);
			//버전
			int ver=12;
			cmp_file.Write(&ver,sizeof(int));
			//프로젝트 이름
			int name_length=project_name.GetLength();
			char* name_buffer=new char[name_length+1];
			strcpy(name_buffer, project_name.GetBuffer(name_length));
			project_name.ReleaseBuffer();
			cmp_file.Write(&name_length, sizeof(int));
			cmp_file.Write(name_buffer, name_length);
			delete name_buffer;
			//암호
			char key[9];
			char key2[9]="DONGHUNG";
			cmp_file.Write(key, 8);
			//파일리스트 저장
			cmp_file.Write(&ci_count, sizeof(int));
			for(int i=0; i<ci_count; i++)cmp_file.Write(&m_ci[i], sizeof(ChunkInfo));

			cmp_file.Close();
		}
	}

	::SetCurrentDirectory(sdir);
}

// 파일 경로에서 파일 이름 부분만 구해주는 함수
void CchunkmakerDlg::GetFileName(char* title, char* path)
{
	size_t last_slash=-1;
	//파일 이름 앞의 경로부분을 찾기
	for(size_t i=0; i<strlen(path); i++)
		if(path[i]=='\\')last_slash=i;
	if(last_slash==-1)
	{
		strcpy(title, path);
		return;
	}
	//그 부분부터 복사
	for(i=last_slash+1; i<strlen(path); i++)title[i-last_slash-1]=path[i];
	title[i-last_slash-1]=NULL;
}

// 파일 이름에서 확장자를 뺀 이름만 구해주는 함수
char* CchunkmakerDlg::GetFileTitle(char* title, char* name)
{
	size_t i;
	for(i=0; i<strlen(name); i++)
	{
		if(name[i]=='.')break;
		title[i]=name[i];
	}
	title[i]=NULL;

	return title;
}

// 리스트에 표시할 정보로 전환
void CchunkmakerDlg::MakeList(CString* str, int ci_no)
{
	CString colorkey_code;
	CString layertype_text;
	ChunkInfo* pChunkInfo=&m_ci[ci_no];

	//칼라키
	if(pChunkInfo->colorkey)colorkey_code.Format("%06x", pChunkInfo->color);
		else colorkey_code="없음";
	//레이어 타입
	if(pChunkInfo->layertype==0)layertype_text="Normal";
		else if(pChunkInfo->layertype==1)layertype_text="Light";
		else if(pChunkInfo->layertype==2)layertype_text="Dark";
		else if(pChunkInfo->layertype==3)layertype_text="Addition";

	str->Format("%s(%s)   투명키:%s   불투명도:%1.2f   타입: %s", 
		pChunkInfo->name, pChunkInfo->filename, colorkey_code, pChunkInfo->opacity, layertype_text);
}

void CchunkmakerDlg::OnLbnDblclkList()
{
	UpdateData(true);

	ChunkInfo* pChunkInfo=&m_ci[selected_list];
	//정보 보내기
    CDataEdit m_de;
	m_de.multi=false;
	m_de.path=pChunkInfo->filepath;
	m_de.Name=pChunkInfo->name;
	m_de.DtoX(&m_de.colorkey, pChunkInfo->color);
	m_de.show_color=pChunkInfo->color;
	m_de.use_colorkey=(pChunkInfo->colorkey)?1:0;
	m_de.opacity=pChunkInfo->opacity;
	m_de.layertype=pChunkInfo->layertype;
	//수정된 정보 반영
	if(m_de.DoModal()==IDOK)
	{
		strcpy(pChunkInfo->filepath, m_de.path.GetBuffer(256));
		m_de.path.ReleaseBuffer();
		strcpy(pChunkInfo->name, m_de.Name.GetBuffer(128));
		m_de.Name.ReleaseBuffer();
		pChunkInfo->color=m_de.XtoD(m_de.colorkey);
		pChunkInfo->colorkey=(m_de.use_colorkey==1)?true:false;
		pChunkInfo->opacity=m_de.opacity;
		pChunkInfo->layertype=m_de.layertype;

		//정보 갱신
		CString list_data;
		MakeList(&list_data, selected_list);
		filelist.DeleteString(selected_list);
		filelist.InsertString(selected_list, list_data);
	}
}

void CchunkmakerDlg::OnBnClickedEdit()
{
	int nCount = filelist.GetSelCount();
	if(nCount<1)return;
	int sels[FILEMAX];
	filelist.GetSelItems(nCount, (LPINT)sels);

    CDataEdit m_de;
	m_de.multi=true;
	//수정된 정보 반영
	if(m_de.DoModal()==IDOK)
	{
		for(int i=0; i<nCount; i++)
		{
			char tmp_name[128];
			ChunkInfo* pChunkInfo=&m_ci[sels[i]];
			sprintf(tmp_name,"%s%s",m_de.Name.GetBuffer(128),pChunkInfo->name);
			m_de.Name.ReleaseBuffer();
			strcpy(pChunkInfo->name, tmp_name);

			pChunkInfo->color=m_de.XtoD(m_de.colorkey);
			pChunkInfo->colorkey=(m_de.use_colorkey==1)?true:false;
			pChunkInfo->opacity=m_de.opacity;
			pChunkInfo->layertype=m_de.layertype;
		}
		//리스트 갱신
		filelist.ResetContent();
		for(i=0; i<ci_count; i++)
		{
			CString list_data;
			MakeList(&list_data, i);
			filelist.AddString(list_data);
		}
		//재선택
		for(i=0; i<nCount; i++)filelist.SetSel(sels[i]);
	}
}
