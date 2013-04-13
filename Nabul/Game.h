#if !defined(AFX_GAME_H__B8A41FE3_8D9E_11D4_AF27_00C026261B5A__INCLUDED_)
#define AFX_GAME_H__B8A41FE3_8D9E_11D4_AF27_00C026261B5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Game.h : header file
//
#include "end.h"
#include "Wave.h"
#include "Midi.h"
/////////////////////////////////////////////////////////////////////////////
// CGame dialog

class CGame : public CDialog
{
// Construction
public:
	int m_time;
	void Die(int i);
	void music();
	CWave m_wave;
	CMidi m_midi;
	Cend m_end;
	void gameover();
	void next();
	void shop();
	void InitMonster(int num);
	void mon();
	void shield();
	void Data();
	int m_lv,m_condition,m_robe,m_rod,m_money,m_hp,m_hpmax,m_mp,m_mpmax,m_exp,m_herb,m_drug,m_shield;
	typedef struct{
		int att;
		int def;
		int res;
		int prz;
		int pos;
		int mon;
		CString text;
	}Monster;
	Monster monster[5];
	int m_boss,m_stage,m_prz,m_bosp,m_over;
	int m_over2,m_bosss,m_back;
	
	char buffer[20];
	CGame(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGame)
	enum { IDD = IDD_GAME };
	CEdit	m_type2;
	CStatic	m_item2;
	CStatic	m_item1;
	CStatic	m_cure;
	CStatic	m_wp2;
	CStatic	m_wp1;
	CString	m_type;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGame)
	afx_msg void OnVscrollType();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnChangeType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAME_H__B8A41FE3_8D9E_11D4_AF27_00C026261B5A__INCLUDED_)
