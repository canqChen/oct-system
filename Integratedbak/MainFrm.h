// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问 
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "stdafx.h"
#include "ChildView.h"
#include "DASDlg.h"
#include "ShowDlg.h"
#include "SingleDlg.h"
#include "MultiDlg.h"
#include "RibbonBar.h"
#include "Myresource.h"
#include "resource.h"

class CMainFrame : public CFrameWndEx
{

public:
	CMainFrame();
protected:
	DECLARE_DYNAMIC(CMainFrame)
	HICON m_hIcon;
	// 特性
public:

	// 操作
public:

	// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CRibbonBar m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar m_wndStatusBar;
	CChildView m_wndView;
	void InitializeRibbon();
	// 生成的消息映射函数
protected:
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
private:
	CSingleDlg* m_singleDlg;
	CMultiDlg* m_multiDlg;
	CMFCRibbonCategory* pAcquisitionCategory;
	CMFCRibbonCategory* pAnalysisCategory;
	CMFCRibbonComboBox* pComboBoxColorMode;
	CMFCRibbonComboBox* pComboBoxMonitorMode;
	CRect ShowDlgRect;
	bool bInitRibbon;
	bool bIsPressAcquBn;
	bool bOnSinglePanel;
	bool bOnMultiPanel;
public:
	afx_msg void OnBnMonitor();
	afx_msg void OnUpdateBnMonitor(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnUpdateBnZoomin(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBnZoomout(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBnZoomreset(CCmdUI *pCmdUI);
	afx_msg void OnBnZoomin();
	afx_msg void OnBnZoomout();
	afx_msg void OnBnZoomreset();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
protected:
	afx_msg LRESULT OnAfxWmOnChangeRibbonCategory(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnSingle();
	afx_msg void OnUpdateBnSingle(CCmdUI *pCmdUI);
	afx_msg void OnBnMulti();
	afx_msg void OnUpdateBnMulti(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBnSettings(CCmdUI *pCmdUI);
	afx_msg void OnBnSettings();
	afx_msg void OnSglcolor();
	afx_msg void OnUpdateSglcolor(CCmdUI *pCmdUI);
	afx_msg void OnSglgray();
	afx_msg void OnUpdateSglgray(CCmdUI *pCmdUI);
	afx_msg void OnMultiuncolor();
	afx_msg void OnUpdateMultiuncolor(CCmdUI *pCmdUI);
	afx_msg void OnMultiungray();
	afx_msg void OnUpdateMultiungray(CCmdUI *pCmdUI);
	afx_msg void OnMulticolor();
	afx_msg void OnUpdateMulticolor(CCmdUI *pCmdUI);
	afx_msg void OnMultigray();
	afx_msg void OnUpdateMultigray(CCmdUI *pCmdUI);
	afx_msg void OnBnSavebmp();
	afx_msg void OnUpdateBnSavebmp(CCmdUI *pCmdUI);
	afx_msg void OnBnSavejpeg();
	afx_msg void OnUpdateBnSavejpeg(CCmdUI *pCmdUI);
	afx_msg void OnBnSavetiff();
	afx_msg void OnUpdateBnSavetiff(CCmdUI *pCmdUI);
	afx_msg void OnBnSavegif();
	afx_msg void OnUpdateBnSavegif(CCmdUI *pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnComboColormode();
	afx_msg void OnUpdateComboColormode(CCmdUI *pCmdUI);
	afx_msg void OnNcMButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnComboMonitormode();
	afx_msg void OnUpdateComboMonitormode(CCmdUI *pCmdUI);
};
