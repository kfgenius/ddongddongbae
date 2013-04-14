/////////////////////////////////////////////////////////////////////
//
// BaseImage.h: interface for the CBaseImage class.
//
// JPEG �� �����쿡 ����ϴµ� ����ϴ� �߰�Ŭ����
// 
// Ư¡ : MFC �� ����Ͽ����Ƿ� �ణ�� �Ǽ��ν� W32API �� ����Ҽ� �ְ� ������
//
// �ֿ䵿�� : JPEG �� �о�鿩 DIB �� ��Ʈ������ �������� DC �� ����Ѵ�. �̻�.
//
// ������ : ��
// ������ : cuwaki@naver.com
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_BASEIMAGE_H__0D8E8F3C_2961_11D2_8B72_006097AD9DDC__INCLUDED_)
//#define AFX_BASEIMAGE_H__0D8E8F3C_2961_11D2_8B72_006097AD9DDC__INCLUDED_
//
//#include "Jpeg.h"	// Added by ClassView
//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#ifndef _JPEG_MODULE_INTERFACE_
#define _JPEG_MODULE_INTERFACE_

#include "Jpeg.h"

// ������ MFC �� WIN32API �� �Բ� ���� ���� �Ǽ��� ��ȯ��
#undef _WINDOWS_

#include "afxwin.h"

#define PALVERSION 0x300
#define DIB_HEADER_MARKER ((WORD)('M'<<8) | 'B')

class CBaseImage  
{
protected:
	BITMAPFILEHEADER bmpHeader;
	LPBITMAPINFO pBitmapInfo;
	LPSTR m_pDib;
	LPSTR tDib;
	int m_ndumy;
	int m_nColor;
	LPLOGPALETTE lpLogPalette;
	CPalette *cPalette;
	char m_strFileName[512];
public:
	CBitmap *m_pBitmap;
	CBaseImage();
	virtual ~CBaseImage();

public:
	void FlipY();
	BOOL SaveJpg(LPCSTR file);
	CJpeg *m_pJpg;
	BOOL LoadJpg(LPCSTR file);
	void MakeBitmap();
	BOOL SaveImage(LPCSTR *file=NULL);
	/*
	Create
	size ũ���� ������ colornum�� ������ ����� backcolor�� �����.
	*/
	BOOL Create(SIZE size, int colornum,COLORREF backcolor);
	/* 
	GetSize:
	�̹����� ũ�⸦ �ȴ�.
	*/
	SIZE GetSize();
	/*
	PaintImage
	ȭ�鿡 �̹����� ����Ѵ�.
	*/
	BOOL  PaintImage(int x, int y, HDC hDC);
	/*
	LoadBmp
	filename���� bmp�� �ε��Ѵ�.
	*/
	BOOL LoadBmp(LPCSTR filename);
	/*
	SaveImage
	bmp ������ �����Ѵ�.
	*/
	BOOL SaveImage(LPCSTR file);
	//���ϸ� ���
	LPCSTR GetFileName(){return m_strFileName;}
	//���� Į��� ���
	int GetColorNum(){	return m_nColor;}
	//�ȷ�Ʈ ���
	CPalette * GetPalette(){return cPalette;}
	LPBITMAPINFO GetBitmapInfo(){return pBitmapInfo;};


protected:
	/*
	SetBmpInfo:
		szBitmapInfo�� ���� �����ϴ� �Լ�
		������: 16Į�� 2Į�� ���� ���� 4�� 8���� ���� �̹��� �����ȵ�
	*/
	BOOL SetBmpInfo(SIZE size, int colornum);
	/*
	AllocBmpInfo
	pBitmapInfo�� �޸𸮿� �����ϴ� �Լ�
	*/
	BOOL AllocBmpInfo(int colornum);
	/*
	CreatePalette
	mode�� 1�̸� pBitmapInfo ����
	�׷��� ������ ����
	*/

	BOOL CreatePalette(BOOL mode);
	/*
	FindColorInPalette
	�ȷ�Ʈ �ȿ��� fi �� ���� �ִ°��� ã�´�.
	version up: ����� ������ ����
	*/
	int FindColorInPalette(COLORREF fi);
	/*
	SetSystemPalette
	�����쿡 �����Ǿ��ִ� �ȷ�Ʈ�� ����Ѵ�.
	*/
	BOOL SetSystemPalette(HDC hDC);

};


// ����ϴµ� �������̰�........
typedef CBaseImage CJpg;

// ������ MFC �� WIN32API �� �Բ� ���� ���� �Ǽ��� ��ȯ��
#define _WINDOWS_


//#endif // !defined(AFX_BASEIMAGE_H__0D8E8F3C_2961_11D2_8B72_006097AD9DDC__INCLUDED_)
#endif //_JPEG_MODULE_INTERFACE_
