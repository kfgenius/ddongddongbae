/////////////////////////////////////////////////////////////////////
//
// BaseImage.h: interface for the CBaseImage class.
//
// JPEG 를 윈도우에 출력하는데 사용하는 중계클래스
// 
// 특징 : MFC 를 사용하였으므로 약간의 꽁수로써 W32API 와 사용할수 있게 변경함
//
// 주요동작 : JPEG 를 읽어들여 DIB 형 비트맵으로 변경한후 DC 에 출력한다. 이상.
//
// 원저자 : 모름
// 변경자 : cuwaki@naver.com
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

// 다음은 MFC 와 WIN32API 를 함께 쓰기 위한 꽁수의 일환임
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
	size 크기의 영상을 colornum의 색으로 배경을 backcolor로 만든다.
	*/
	BOOL Create(SIZE size, int colornum,COLORREF backcolor);
	/* 
	GetSize:
	이미지의 크기를 안다.
	*/
	SIZE GetSize();
	/*
	PaintImage
	화면에 이미지를 출력한다.
	*/
	BOOL  PaintImage(int x, int y, HDC hDC);
	/*
	LoadBmp
	filename으로 bmp를 로드한다.
	*/
	BOOL LoadBmp(LPCSTR filename);
	/*
	SaveImage
	bmp 파일을 저장한다.
	*/
	BOOL SaveImage(LPCSTR file);
	//파일명 얻기
	LPCSTR GetFileName(){return m_strFileName;}
	//파일 칼라수 얻기
	int GetColorNum(){	return m_nColor;}
	//팔레트 얻기
	CPalette * GetPalette(){return cPalette;}
	LPBITMAPINFO GetBitmapInfo(){return pBitmapInfo;};


protected:
	/*
	SetBmpInfo:
		szBitmapInfo에 값을 설정하는 함수
		버전업: 16칼라 2칼라 각각 가로 4와 8보다 작은 이미지 설정안됨
	*/
	BOOL SetBmpInfo(SIZE size, int colornum);
	/*
	AllocBmpInfo
	pBitmapInfo를 메모리에 설정하는 함수
	*/
	BOOL AllocBmpInfo(int colornum);
	/*
	CreatePalette
	mode가 1이면 pBitmapInfo 에서
	그렇지 않으면 만듬
	*/

	BOOL CreatePalette(BOOL mode);
	/*
	FindColorInPalette
	팔레트 안에서 fi 의 색이 있는가를 찾는다.
	version up: 비슷한 색으로 가기
	*/
	int FindColorInPalette(COLORREF fi);
	/*
	SetSystemPalette
	윈도우에 설정되어있는 팔레트를 사용한다.
	*/
	BOOL SetSystemPalette(HDC hDC);

};


// 사용하는데 직관적이게........
typedef CBaseImage CJpg;

// 다음은 MFC 와 WIN32API 를 함께 쓰기 위한 꽁수의 일환임
#define _WINDOWS_


//#endif // !defined(AFX_BASEIMAGE_H__0D8E8F3C_2961_11D2_8B72_006097AD9DDC__INCLUDED_)
#endif //_JPEG_MODULE_INTERFACE_

