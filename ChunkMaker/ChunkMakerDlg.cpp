// chunkmakerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "chunkmaker.h"
#include "chunkmakerDlg.h"
#include ".\chunkmakerdlg.h"
#include "dataedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CchunkmakerDlg ��ȭ ����



CchunkmakerDlg::CchunkmakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CchunkmakerDlg::IDD, pParent)
	, project_name(_T("�ӽ�������Ʈ"))
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


// CchunkmakerDlg �޽��� ó����

BOOL CchunkmakerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	ci_count=0;
	::GetCurrentDirectory(256, sdir);
	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CchunkmakerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
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

	//MLC�� ���������� ���� �� �ִ��� �˻�
	if(ci_count<=0)return;
	for(int i=0; i<ci_count; i++)
	{
		//���� �̸��� ����
		for(int j=0; j<ci_count; j++)
		{
			if(i!=j && strcmp(m_ci[i].name, m_ci[j].name)==0)
			{
				char msg[256];
				sprintf(msg, "%s��� �̸��� ���� �׸��� 2���̻� �����մϴ�.", m_ci[i].name);
				MessageBox(msg);
				return;
			}
		}
		//������ ã�� �� ����
		CFile test;
		strcpy(temp_path[i], cdir);
		strcat(temp_path[i], m_ci[i].filepath);
		if(test.Open(temp_path[i], CFile::modeRead))test.Close();
		else
		{
			CString err_msg;
			err_msg.Format("%s ������ ã�� �� �����ϴ�!", m_ci[i].filepath);
			MessageBox(err_msg);
			return;
		}
	}

	//chunk ���� �����
	UpdateData(true);
	if(project_name=="")strcat(cdir,"�̸�����");
		else strcat(cdir,project_name);

	const char szFilters[]="�����̺� ��� ����(*.mlc)|*.mlc||";
	CFileDialog m_fdlg (FALSE, "", cdir, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);
	if(m_fdlg.DoModal()==IDOK)
	{
		CFile mlc_file;
		CString mlc_file_name=m_fdlg.GetFileName();
		if(mlc_file.Open(mlc_file_name, CFile::modeCreate | CFile::modeWrite))
		{
			//Chunk�������� ǥ��
			mlc_file.Write("CHNK",4);

			for(int i=0; i<ci_count; i++)
			{
				chunkdata filedata;
				picturedata pd;
				ChunkInfo* pChunkInfo=&m_ci[i];

				//���� ����
				ZeroMemory(filedata.name, 128);
				ZeroMemory(filedata.filename, 128);
				strcpy(filedata.name,pChunkInfo->name);
				strcpy(filedata.filename, pChunkInfo->filename);

				filedata.type=1;

				//�׸� ����
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
				//������
				if(pChunkInfo->opacity!=1.0f)pd.flag|=PicInfo_Opacity;
				pd.opacity=pChunkInfo->opacity;
				//Į��Ű
				if(pChunkInfo->colorkey)pd.flag|=PicInfo_ColorKey;
				pd.colorkey=pChunkInfo->color;
				//��Ÿ(�������� ����)
				pd.sysmem=1;
				ZeroMemory(pd.maskname, 128);

				char* buffer;
				CFile source_file;
				//�������Ͽ��� ���� �о����(�ڽ��� ����)
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

				//���� ����
				delete [] buffer;
				buffer=NULL;
			}

			mlc_file.Close();
		}

		char szResult[80];
		sprintf(szResult, "���������� %s ������ �����Ͽ����ϴ�.", mlc_file_name);
		MessageBox(szResult);
	}

	::SetCurrentDirectory(sdir);
}

void CchunkmakerDlg::OnBnClickedAdd()
{
	char cdir[256];
	strcpy(cdir, sdir);
	strcat(cdir,"\\*.gif;*.jpg;*.tif;*.bmp");

	//�׸� ���� �ҷ�����
	const char szFilters[]="�׸� ����(*.gif,*.jpg,*.tif,*.bmp)|*.gif;*.jpg;*.tif;*.bmp||";
	CFileDialog m_fdlg(TRUE, "", cdir, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY, szFilters, this);
	
	//�� ����(���ϸ��� ���� ��츦 ����ؼ�)
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
				err_msg.Format("%d������ �ۿ� ���� �� �����ϴ�.", FILEMAX);
				break;		//�Ѱ�ġ �ʰ��� �� �̻� �߰� �Ұ���
			}
			ChunkInfo* pChunkInfo=&m_ci[ci_count];

			//���� �������� �˻�
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
			//���� ������ ���
			if(sub_folder)
			{
				file_path="";
				++i;
				for(; i<(size_t)tmp_buffer.GetLength(); ++i)file_path += tmp_buffer.GetAt(i);
			}
			//�ƴ� ��� ���� ��η�
			else
			{
				file_path = tmp_buffer;
			}
			//����Ʈ�� �߰�	
			strcpy(pChunkInfo->filepath, file_path.GetBuffer(256));	//���� ���
			file_path.ReleaseBuffer();
			GetFileName(pChunkInfo->filename, pChunkInfo->filepath);					//���� �̸�
			//�׸� �̸�
			char title[80];
			strcpy(pChunkInfo->name, GetFileTitle(title, pChunkInfo->filename));
			//Į��Ű, ����
			pChunkInfo->colorkey=false;
			pChunkInfo->opacity=1.0f;
			pChunkInfo->color=0x0000ff;
			pChunkInfo->layertype=0;

			//����Ʈ�� ǥ���� ���� ����
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

		//�����Ϳ��� ���ֱ�
		ci_count--;
		for(int i=sels[0]; i<ci_count; i++)memcpy(&m_ci[i], &m_ci[i+1], sizeof(ChunkInfo));
		//����Ʈ���� ���ֱ�
		filelist.DeleteString(sels[0]);
	}
}

void CchunkmakerDlg::OnBnClickedLoad()
{
	char cdir[256];
	strcpy(cdir, sdir);
	strcat(cdir,"\\*.cmp");

	//������Ʈ ���� �ҷ�����
	const char szFilters[]="ChunkMaker ������Ʈ ����(*.cmp)|*.cmp||";
	CFileDialog m_fdlg (TRUE, "", cdir, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
	UpdateData(true);

	CString fileName;
	if(m_fdlg.DoModal ()==IDOK )
	{
		CFile cmp_file;
		if(cmp_file.Open(m_fdlg.GetFileName(), CFile::modeRead))
		{
			//CMP�������� Ȯ��
			char mark[11];
			cmp_file.Read(mark,10);
			mark[10]=NULL;
			if(strcmp(mark, "ChunkMaker")!=0)
			{
				MessageBox("ChunkMaker ������Ʈ ������ �ƴմϴ�!");
				::SetCurrentDirectory(sdir);
				return;
			}
			//����
			int ver;
			cmp_file.Read(&ver, sizeof(int));
			//������Ʈ �̸�
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
			//��ȣ
			if(ver>=12)
			{
				char key[9];
				char key2[9]="DONGHUNG";
				cmp_file.Read(key, 8);
			}
			//����Ʈ ����
			filelist.ResetContent();
			//���ϸ���Ʈ �ҷ�����
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
	if(project_name=="")strcat(cdir,"�̸�����");
		else strcat(cdir,project_name);

	//������Ʈ ���� �����ϱ�
	const char szFilters[]="ChunkMaker ������Ʈ ����(*.cmp)|*.cmp||";
	CFileDialog m_fdlg (FALSE, "", cdir, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);

	CString fileName;
	if(m_fdlg.DoModal ()==IDOK )
	{
		CFile cmp_file;
		if(cmp_file.Open(m_fdlg.GetFileName(), CFile::modeCreate | CFile::modeWrite))
		{
			//CMP�������� ǥ��
			cmp_file.Write("ChunkMaker",10);
			//����
			int ver=12;
			cmp_file.Write(&ver,sizeof(int));
			//������Ʈ �̸�
			int name_length=project_name.GetLength();
			char* name_buffer=new char[name_length+1];
			strcpy(name_buffer, project_name.GetBuffer(name_length));
			project_name.ReleaseBuffer();
			cmp_file.Write(&name_length, sizeof(int));
			cmp_file.Write(name_buffer, name_length);
			delete name_buffer;
			//��ȣ
			char key[9];
			char key2[9]="DONGHUNG";
			cmp_file.Write(key, 8);
			//���ϸ���Ʈ ����
			cmp_file.Write(&ci_count, sizeof(int));
			for(int i=0; i<ci_count; i++)cmp_file.Write(&m_ci[i], sizeof(ChunkInfo));

			cmp_file.Close();
		}
	}

	::SetCurrentDirectory(sdir);
}

// ���� ��ο��� ���� �̸� �κи� �����ִ� �Լ�
void CchunkmakerDlg::GetFileName(char* title, char* path)
{
	size_t last_slash=-1;
	//���� �̸� ���� ��κκ��� ã��
	for(size_t i=0; i<strlen(path); i++)
		if(path[i]=='\\')last_slash=i;
	if(last_slash==-1)
	{
		strcpy(title, path);
		return;
	}
	//�� �κк��� ����
	for(i=last_slash+1; i<strlen(path); i++)title[i-last_slash-1]=path[i];
	title[i-last_slash-1]=NULL;
}

// ���� �̸����� Ȯ���ڸ� �� �̸��� �����ִ� �Լ�
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

// ����Ʈ�� ǥ���� ������ ��ȯ
void CchunkmakerDlg::MakeList(CString* str, int ci_no)
{
	CString colorkey_code;
	CString layertype_text;
	ChunkInfo* pChunkInfo=&m_ci[ci_no];

	//Į��Ű
	if(pChunkInfo->colorkey)colorkey_code.Format("%06x", pChunkInfo->color);
		else colorkey_code="����";
	//���̾� Ÿ��
	if(pChunkInfo->layertype==0)layertype_text="Normal";
		else if(pChunkInfo->layertype==1)layertype_text="Light";
		else if(pChunkInfo->layertype==2)layertype_text="Dark";
		else if(pChunkInfo->layertype==3)layertype_text="Addition";

	str->Format("%s(%s)   ����Ű:%s   ������:%1.2f   Ÿ��: %s", 
		pChunkInfo->name, pChunkInfo->filename, colorkey_code, pChunkInfo->opacity, layertype_text);
}

void CchunkmakerDlg::OnLbnDblclkList()
{
	UpdateData(true);

	ChunkInfo* pChunkInfo=&m_ci[selected_list];
	//���� ������
    CDataEdit m_de;
	m_de.multi=false;
	m_de.path=pChunkInfo->filepath;
	m_de.Name=pChunkInfo->name;
	m_de.DtoX(&m_de.colorkey, pChunkInfo->color);
	m_de.show_color=pChunkInfo->color;
	m_de.use_colorkey=(pChunkInfo->colorkey)?1:0;
	m_de.opacity=pChunkInfo->opacity;
	m_de.layertype=pChunkInfo->layertype;
	//������ ���� �ݿ�
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

		//���� ����
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
	//������ ���� �ݿ�
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
		//����Ʈ ����
		filelist.ResetContent();
		for(i=0; i<ci_count; i++)
		{
			CString list_data;
			MakeList(&list_data, i);
			filelist.AddString(list_data);
		}
		//�缱��
		for(i=0; i<nCount; i++)filelist.SetSel(sels[i]);
	}
}
