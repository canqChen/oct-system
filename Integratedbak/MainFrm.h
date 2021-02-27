// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���  
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�  
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ������� 
// http://go.microsoft.com/fwlink/?LinkId=238214��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// MainFrm.h : CMainFrame ��Ľӿ�
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
	// ����
public:

	// ����
public:

	// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CRibbonBar m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar m_wndStatusBar;
	CChildView m_wndView;
	void InitializeRibbon();
	// ���ɵ���Ϣӳ�亯��
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
