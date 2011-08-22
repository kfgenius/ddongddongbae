// chunkmakerDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"

#define FILEMAX 100

enum PicLayerType
{
	PicOverlayNormal=0x0000,
	PicOverlayLight=0x0001,
	PicOverlayDark=0x0002,
	PicOverlayAddition=0x0004,
	PicOverlayForceDWORD=0xffffffff
};

enum PicInfoFlag
{
	PicInfo_None=0,
	PicInfo_StreamSource=0x0001,
	PicInfo_FileSource=0x0002,
	PicInfo_LayerType=0x0004,
	PicInfo_Opacity=0x0008,
	PicInfo_MaskName=0x0010,
	PicInfo_ColorKey=0x0020,
	PicInfo_RedrawFunc=0x0040,
	PicInfo_ClipRegion=0x0080,
	PicInfo_ForceDWORD=0xffffffff
};

struct chunkdata
{
	char name[128];
	char filename[128];
	DWORD type;
	DWORD size;
};

struct picturedata
{
	DWORD flag;
	PicLayerType layertype;
	float opacity;
	DWORD colorkey;
	DWORD sysmem;
	char maskname[128];
};

struct ChunkInfo
{
    char filename[128];
	char filepath[256];
	char name[128];
	float opacity;
	bool colorkey;
	DWORD color;
	int layertype;
};

// CchunkmakerDlg ��ȭ ����
class CchunkmakerDlg : public CDialog
{
// ����
public:
	int selected_list;
	char new_buffer[2000];

	CchunkmakerDlg(CWnd* pParent = NULL);	// ǥ�� ������

	afx_msg void OnBnClickedExport();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedDelete();
	// ������Ʈ �̸�
	CString project_name;
	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedSave();
	afx_msg void OnLbnDblclkList();
	CListBox filelist;	

// ��ȭ ���� ������
	enum { IDD = IDD_CHUNKMAKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	int ci_count;
	char sdir[256];
	ChunkInfo m_ci[FILEMAX];

	void GetFileName(char* title, char* path);		// ���� ��ο��� ���� �̸� �κи� �����ִ� �Լ�
	char* GetFileTitle(char* title, char* name);	// ���� �̸����� Ȯ���ڸ� �� �̸��� �����ִ� �Լ�
	void MakeList(CString* str, int ci_no);			// ����Ʈ�� ǥ���� ������ ��ȯ
	void DtoX(CString x, int d);
	int XtoD(CString x);
public:
	afx_msg void OnBnClickedEdit();
};
