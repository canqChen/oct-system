#pragma once
#include "afxwin.h"

// CSplashWnd

class CSplashWnd : public CWnd
{
	DECLARE_DYNAMIC(CSplashWnd)

public:
	CSplashWnd();
	virtual ~CSplashWnd();
	virtual void PostNcDestroy();

public:
	CBitmap m_bitmap;

protected:
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;

public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();

#ifndef _WIN64
	afx_msg void OnTimer(UINT nIDEvent);
#else
	afx_msg void OnTimer(UINT_PTR nIDEvent);
#endif // !_WIN64

protected:
	DECLARE_MESSAGE_MAP()
};


