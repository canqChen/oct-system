// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Integratedbak.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(once:4244)

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//inline void EnableMemLeakCheck()
//{
//	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
//}

using namespace std;

Vimba* CameraAcess = Vimba::GetInstance();

extern vector<Mymatrix<double>*>p_UnPhaseDataVec1;
extern vector<Mymatrix<double>*>p_UnPhaseDataVec2;
extern vector<Mymatrix<double>*>p_UnPhaseDataVec3;
extern vector<Mymatrix<double>*>p_UnPhaseDataVec4;

extern Mymatrix<double>** unPhaseData1;
extern Mymatrix<double>** unPhaseData2;
extern Mymatrix<double>** unPhaseData3;
extern Mymatrix<double>** unPhaseData4;
extern Mymatrix<double>** PhaseData1;
extern Mymatrix<double>** PhaseData2;
extern Mymatrix<double>** PhaseData3;
extern Mymatrix<double>** PhaseData4;

extern vector<Mymatrix<double>*>p_PhaseDataVec1;
extern vector<Mymatrix<double>*>p_PhaseDataVec2;
extern vector<Mymatrix<double>*>p_PhaseDataVec3;
extern vector<Mymatrix<double>*>p_PhaseDataVec4;

extern CWnd* mCWnd;
extern CWnd* mSingleWnd;
extern CWnd* mMultiWnd;
extern CString IniPath;
extern double exposure;
extern double gain;
extern double fps;
extern CString cExposure;
extern CString cGain;
extern bool bMaximize;
extern bool bScaleControl;

extern int MAX_EXPOSURE_SLD_MS;
extern int MAX_EXPOSURE_SLD_US;
CRect TgtRect;

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_BN_Monitor, &CMainFrame::OnBnMonitor)
	ON_COMMAND(ID_BN_ZOOMIN, &CMainFrame::OnBnZoomin)
	ON_COMMAND(ID_BN_ZOOMOUT, &CMainFrame::OnBnZoomout)
	ON_COMMAND(ID_BN_ZOOMRESET, &CMainFrame::OnBnZoomreset)
	ON_UPDATE_COMMAND_UI(ID_BN_Monitor, &CMainFrame::OnUpdateBnMonitor)
	ON_UPDATE_COMMAND_UI(ID_BN_ZOOMIN, &CMainFrame::OnUpdateBnZoomin)
	ON_UPDATE_COMMAND_UI(ID_BN_ZOOMOUT, &CMainFrame::OnUpdateBnZoomout)
	ON_UPDATE_COMMAND_UI(ID_BN_ZOOMRESET, &CMainFrame::OnUpdateBnZoomreset)
	ON_COMMAND(ID_BN_Single, &CMainFrame::OnBnSingle)
	ON_COMMAND(ID_BN_Multi, &CMainFrame::OnBnMulti)
	ON_UPDATE_COMMAND_UI(ID_BN_Single, &CMainFrame::OnUpdateBnSingle)
	ON_UPDATE_COMMAND_UI(ID_BN_Multi, &CMainFrame::OnUpdateBnMulti)
	ON_UPDATE_COMMAND_UI(ID_BN_Settings, &CMainFrame::OnUpdateBnSettings)
	ON_COMMAND(ID_BN_Settings, &CMainFrame::OnBnSettings)
	ON_COMMAND(ID_SGLCOLOR, &CMainFrame::OnSglcolor)
	ON_UPDATE_COMMAND_UI(ID_SGLCOLOR, &CMainFrame::OnUpdateSglcolor)
	ON_COMMAND(ID_SGLGRAY, &CMainFrame::OnSglgray)
	ON_UPDATE_COMMAND_UI(ID_SGLGRAY, &CMainFrame::OnUpdateSglgray)
	ON_COMMAND(ID_MULTIUNCOLOR, &CMainFrame::OnMultiuncolor)
	ON_UPDATE_COMMAND_UI(ID_MULTIUNCOLOR, &CMainFrame::OnUpdateMultiuncolor)
	ON_COMMAND(ID_MULTIUNGRAY, &CMainFrame::OnMultiungray)
	ON_UPDATE_COMMAND_UI(ID_MULTIUNGRAY, &CMainFrame::OnUpdateMultiungray)
	ON_COMMAND(ID_MULTICOLOR, &CMainFrame::OnMulticolor)
	ON_UPDATE_COMMAND_UI(ID_MULTICOLOR, &CMainFrame::OnUpdateMulticolor)
	ON_COMMAND(ID_MULTIGRAY, &CMainFrame::OnMultigray)
	ON_UPDATE_COMMAND_UI(ID_MULTIGRAY, &CMainFrame::OnUpdateMultigray)
	ON_REGISTERED_MESSAGE(AFX_WM_ON_CHANGE_RIBBON_CATEGORY, &CMainFrame::OnAfxWmOnChangeRibbonCategory)
	ON_COMMAND(ID_BN_SaveBMP, &CMainFrame::OnBnSavebmp)
	ON_UPDATE_COMMAND_UI(ID_BN_SaveBMP, &CMainFrame::OnUpdateBnSavebmp)
	ON_COMMAND(ID_BN_SaveJPEG, &CMainFrame::OnBnSavejpeg)
	ON_UPDATE_COMMAND_UI(ID_BN_SaveJPEG, &CMainFrame::OnUpdateBnSavejpeg)
	ON_COMMAND(ID_BN_SaveTIFF, &CMainFrame::OnBnSavetiff)
	ON_UPDATE_COMMAND_UI(ID_BN_SaveTIFF, &CMainFrame::OnUpdateBnSavetiff)
	ON_COMMAND(ID_BN_SaveGIF, &CMainFrame::OnBnSavegif)
	ON_UPDATE_COMMAND_UI(ID_BN_SaveGIF, &CMainFrame::OnUpdateBnSavegif)
	ON_COMMAND(ID_COMBO_ColorMode, &CMainFrame::OnComboColormode)
	ON_UPDATE_COMMAND_UI(ID_COMBO_ColorMode, &CMainFrame::OnUpdateComboColormode)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEWHEEL()
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_SETCURSOR()
	ON_WM_NCMBUTTONDBLCLK()
	ON_WM_NCMBUTTONDOWN()
	ON_COMMAND(ID_COMBO_MonitorMode, &CMainFrame::OnComboMonitormode)
	ON_UPDATE_COMMAND_UI(ID_COMBO_MonitorMode, &CMainFrame::OnUpdateComboMonitormode)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	m_singleDlg = new CSingleDlg();
	m_multiDlg = new CMultiDlg();
	bInitRibbon = false;
	bIsPressAcquBn = false;
	bOnSinglePanel = false;
	bOnMultiPanel = false;
}

CMainFrame::~CMainFrame()
{
	delete m_singleDlg;
	delete m_multiDlg;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//// 显示Splash窗口
	//CSplashWnd::ShowSplashScreen(this);
	//Sleep(3000);

	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, true);
	SetIcon(m_hIcon, false);

	BOOL bNameValid;

	// 创建一个视图以占用框架的工作区
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	CString strTitlePane1;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndRibbonBar.ShowWindow(SW_SHOW);

	// 创建采集模块窗口
	if (!CDASDlg::GetInstance()->Create(IDD_DIALOG_DataAcquisition, this))
	{
		TRACE0("未能创建窗口\n");
		return -1;      // 未能创建
	}

	// 创建ShowDlg
	CShowDlg::getInstance()->ShowWindow(SW_HIDE);

	// 创建SingleDlg
	if (!m_singleDlg->Create(IDD_DIALOG_Analysis_Single, this))
	{
		TRACE0("未能创建SingleDlg\n");
		return -1;		//未能创建
	}
	// 创建CMultiDlg
	if (!m_multiDlg->Create(IDD_DIALOG_Analysis_Multi, this))
	{
		TRACE0("未能创建MultiDlg\n");
		return -1;		//未能创建
	}
	int width = GetSystemMetrics(SM_CXFULLSCREEN);
	int height = GetSystemMetrics(SM_CYFULLSCREEN);
	TgtRect = CRect(2, 200, width, height - 20);
	CDASDlg::GetInstance()->MoveWindow(&TgtRect);
	CDASDlg::GetInstance()->ShowWindow(SW_SHOW);
	RedrawWindow();

	CDASDlg::GetInstance()->ChangeCtrlSize(TgtRect);

	// RibbonBar
	CMFCRibbonQuickAccessToolBarDefaultState qatState;
	qatState.AddCommand(ID_APP_ABOUT);
	m_wndRibbonBar.SetQuickAccessDefaultState(qatState);

	InitializeRibbon();

	OnComboMonitormode();
	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 设置用于绘制所有用户界面元素的视觉管理器
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	// 设置视觉管理器使用的视觉样式
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE /*| WS_MINIMIZEBOX*/ | WS_DLGFRAME /*| WS_THICKFRAME*/
		/*| WS_MAXIMIZEBOX*/ | WS_MAXIMIZE | WS_SYSMENU | WS_CLIPCHILDREN;

	cs.cx = GetSystemMetrics(SM_CXSCREEN);
	cs.cy = GetSystemMetrics(SM_CYFULLSCREEN);

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 将焦点前移到视图窗口
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 让视图第一次尝试该命令
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 否则，执行默认处理
	return CFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnClose()
{
	if (CDASDlg::GetInstance()->bMonitor == true)
	{
		if (MessageBox(_T("相机仍在运行,是否退出?"), _T("警告"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			CDASDlg::GetInstance()->OnBnClickedButtonMonitor();
			Sleep(500);
		}
		else
		{
			return;
		}
	}
	bInitRibbon = false;

	CString cExposure;
	CString cGain;
	mCWnd->GetDlgItem(IDC_EDIT_Camera_Exposure)->GetWindowTextW(cExposure);
	mCWnd->GetDlgItem(IDC_EDIT_Camera_Gain)->GetWindowTextW(cGain);
	cExposure.Format(_T("%d"), (int)exposure);
	cGain.Format(_T("%d"), (int)gain);
	WritePrivateProfileString(_T("Camera"), _T("Exposure"), cExposure, IniPath);
	WritePrivateProfileString(_T("Camera"), _T("Gain"), cGain, IniPath);

	//CameraAcess->Width = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_Width);
	//CameraAcess->Height = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_Height);
	//CameraAcess->OffsetX = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_OffsetX);
	//CameraAcess->OffsetY = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_OffsetY);
	CameraAcess->BinningHor = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_HOR);
	CameraAcess->BinningVer = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_VER);

	CString tmp;
	tmp.Format(_T("%d"), CameraAcess->BinningHor);
	WritePrivateProfileString(_T("Data"), _T("Binning_horizontal"), tmp, IniPath);
	tmp.Format(_T("%d"), CameraAcess->BinningVer);
	WritePrivateProfileString(_T("Data"), _T("Binning_vertical"), tmp, IniPath);
	//tmp.Format(_T("%d"), CameraAcess->OffsetX);
	//WritePrivateProfileString(_T("Data"), _T("OffsetX"), tmp, IniPath);
	//tmp.Format(_T("%d"), CameraAcess->OffsetY);
	//WritePrivateProfileString(_T("Data"), _T("OffsetY"), tmp, IniPath);
	//tmp.Format(_T("%d"), CameraAcess->Width);
	//WritePrivateProfileString(_T("Data"), _T("Width"), tmp, IniPath);
	//tmp.Format(_T("%d"), CameraAcess->Height);
	//WritePrivateProfileString(_T("Data"), _T("Height"), tmp, IniPath);

	CShowDlg::getInstance()->reset(true);
	CShowDlg::getInstance()->reset(false);
	CShowDlg::getInstance()->sglreset();
	if (p_UnPhaseDataVec1.size() > 0)
	{
		for (auto i : p_UnPhaseDataVec4)
		{
			delete i;
		}
		p_UnPhaseDataVec4.clear();
		for (auto i : p_UnPhaseDataVec3)
		{
			delete i;
		}
		p_UnPhaseDataVec3.clear();
		for (auto i : p_UnPhaseDataVec2)
		{
			delete i;
		}
		p_UnPhaseDataVec2.clear();
		for (auto i : p_UnPhaseDataVec1)
		{
			delete i;
		}
		p_UnPhaseDataVec1.clear();
		delete[] unPhaseData1;
		delete[] unPhaseData2;
		delete[] unPhaseData3;
		delete[] unPhaseData4;
	}
	if (p_PhaseDataVec1.size() > 0)
	{
		for (auto i : p_PhaseDataVec4)
		{
			delete i;
		}
		p_PhaseDataVec4.clear();
		for (auto i : p_PhaseDataVec3)
		{
			delete i;
		}
		p_PhaseDataVec3.clear();
		for (auto i : p_PhaseDataVec2)
		{
			delete i;
		}
		p_PhaseDataVec2.clear();
		for (auto i : p_PhaseDataVec1)
		{
			delete i;
		}
		p_PhaseDataVec1.clear();
		delete[] PhaseData1;
		delete[] PhaseData2;
		delete[] PhaseData3;
		delete[] PhaseData4;
	}

	//CShowDlg::exitInstace(); 
	CFrameWndEx::OnClose();
}

// RibbonButtons
void CMainFrame::OnUpdateBnZoomin(CCmdUI *pCmdUI)
{
	if (CDASDlg::GetInstance()->bMonitor == true && CameraAcess->scale != 0.10)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}
void CMainFrame::OnUpdateBnZoomout(CCmdUI *pCmdUI)
{
	if (CDASDlg::GetInstance()->bMonitor == true && CameraAcess->scale != 1.0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}
void CMainFrame::OnUpdateBnZoomreset(CCmdUI *pCmdUI)
{
	if (CameraAcess->scale != 1.0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnBnZoomin()
{
	CameraAcess->scale -= 0.10;
	if (CameraAcess->scale < 0.10)
		CameraAcess->scale = 0.10;
}
void CMainFrame::OnBnZoomout()
{
	CameraAcess->scale += 0.10;
	if (CameraAcess->scale > 1.0)
		CameraAcess->scale = 1.0;
}
void CMainFrame::OnBnZoomreset()
{
	CameraAcess->scale = 1.0;
	CameraAcess->offsetX = 50;
	CameraAcess->offsetY = 50;
}

void CMainFrame::OnBnMonitor()
{
	CDASDlg::GetInstance()->OnBnClickedButtonMonitor();
}

void CMainFrame::OnUpdateBnMonitor(CCmdUI *pCmdUI)
{
	CMFCRibbonButton *pBnMonitor = NULL;
	pBnMonitor = DYNAMIC_DOWNCAST(CMFCRibbonButton, m_wndRibbonBar.FindByID(ID_BN_Monitor));
	if (CDASDlg::GetInstance()->bMonitor == true)
	{
		pBnMonitor->SetImageIndex(5, TRUE);
		pCmdUI->SetRadio(TRUE);
		pCmdUI->SetText(_T("停止监控(&F5)"));
	}
	else
	{
		pBnMonitor->SetImageIndex(0, TRUE);
		pCmdUI->SetRadio(FALSE);
		pCmdUI->SetText(_T("开启监控(&F5)"));
	}
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CRect MainDlgRect = CRect(0, 210, cx, 0.97 *cy);
	if (nType == SIZE_MAXIMIZED)
	{
		bMaximize = true;
	}
	else
	{
		bMaximize = false;
	}

	if (nType != SIZE_MINIMIZED)
	{
		CDASDlg::GetInstance()->ChangeCtrlSize(MainDlgRect);
		if (CShowDlg::getInstance()->m_hWnd != NULL)
		{
			ShowDlgRect = CRect(TgtRect.left + TgtRect.Width()*0.25, TgtRect.top, TgtRect.right, TgtRect.bottom);
			CShowDlg::getInstance()->MoveWindow(&ShowDlgRect);
			CShowDlg::getInstance()->UpdateRect();
		}
	}
	AfxGetApp()->GetMainWnd()->RedrawWindow();
	RedrawWindow();
	if (mCWnd != NULL)
		if (mCWnd->m_hWnd != NULL)
			mCWnd->RedrawWindow();
	UpdateData(FALSE);
	if (m_wndStatusBar.m_hWnd != NULL)
		m_wndStatusBar.RedrawWindow();
	CFrameWndEx::OnSize(nType, cx, cy);
}
void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	int max_w = GetSystemMetrics(SM_CXMAXIMIZED);
	int max_h = GetSystemMetrics(SM_CYMAXIMIZED);
	if (mCWnd != NULL)
	{
		int min_w = max_w*0.75;
		int min_h = max_h*0.75;

		CPoint min_pt(min_w, min_h);
		CPoint max_pt(max_w, max_h);
		lpMMI->ptMinTrackSize = min_pt;
		lpMMI->ptMaxTrackSize = max_pt;
	}
	CFrameWndEx::OnGetMinMaxInfo(lpMMI);
}

// 鼠标消息响应
BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CDASDlg::GetInstance()->MouseWheel(nFlags, zDelta, pt);
	return CFrameWndEx::OnMouseWheel(nFlags, zDelta, pt);
}

void CMainFrame::InitializeRibbon()
{
	BOOL bNameValid;
	CString strTemp;
	bNameValid = strTemp.LoadStringW(IDS_Category_Acquisition);
	ASSERT(bNameValid);
	pAcquisitionCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_Category_Small, IDB_Category_Large);

	CMFCRibbonPanel* pPanel = pAcquisitionCategory->AddPanel(_T(""));
	pPanel->SetCenterColumnVert(TRUE);
	pPanel->SetJustifyColumns(TRUE);
	std::auto_ptr<CMFCRibbonButton> pBnMonitor(new CMFCRibbonButton(ID_BN_Monitor, _T("开启监控(&F5)"), 0, 0));
	pPanel->Add(pBnMonitor.release());
	pPanel->SetCenterColumnVert(TRUE);
	pPanel->SetJustifyColumns(TRUE);
	CMFCRibbonPanel* pPanelZoom = pAcquisitionCategory->AddPanel(_T(""));
	pPanelZoom->SetCenterColumnVert(TRUE);
	pPanelZoom->SetJustifyColumns(TRUE);
	std::auto_ptr<CMFCRibbonButton> pBnZoomIn(new CMFCRibbonButton(ID_BN_ZOOMIN, _T("放大(&+)"), 2, 2));
	pPanelZoom->Add(pBnZoomIn.release());
	pPanelZoom->AddSeparator();
	std::auto_ptr<CMFCRibbonButton>pBnZoomOut(new CMFCRibbonButton(ID_BN_ZOOMOUT, _T("缩小(&-)"), 3, 3));
	pPanelZoom->Add(pBnZoomOut.release());
	pPanelZoom->AddSeparator();
	std::auto_ptr<CMFCRibbonButton>pBnZoomReset(new CMFCRibbonButton(ID_BN_ZOOMRESET, _T("复位"), 4, 4));
	pPanelZoom->Add(pBnZoomReset.release());
	CMFCRibbonPanel* pPanelMode = pAcquisitionCategory->AddPanel(_T(""));
	pPanelMode->SetCenterColumnVert(TRUE);
	pPanelMode->SetJustifyColumns(TRUE);
	std::auto_ptr<CMFCRibbonComboBox> pComboMonitorMode(new CMFCRibbonComboBox(ID_COMBO_MonitorMode, FALSE, 70));
	pPanelMode->Add(pComboMonitorMode.release());
	pComboBoxMonitorMode = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pPanelMode->FindByID(ID_COMBO_MonitorMode));
	pComboBoxMonitorMode->SetText(_T("监控模式"));
	pComboBoxMonitorMode->AddItem(_T("干涉条纹"));
	pComboBoxMonitorMode->AddItem(_T("截面轮廓"));
	pComboBoxMonitorMode->AddItem(_T("变形速率"));
	pComboBoxMonitorMode->SelectItem(0);
	pComboBoxMonitorMode->EnableDropDownListResize(FALSE);
	std::auto_ptr<CMFCRibbonComboBox> pComboColorMode(new CMFCRibbonComboBox(ID_COMBO_ColorMode, FALSE, 70));
	pPanelMode->Add(pComboColorMode.release());
	pComboBoxColorMode = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pPanelMode->FindByID(ID_COMBO_ColorMode));
	pComboBoxColorMode->SetText(_T("色彩模式"));
	pComboBoxColorMode->AddItem(_T("灰度"));
	pComboBoxColorMode->AddItem(_T("彩色"));
	pComboBoxColorMode->SelectItem(0);
	pComboBoxColorMode->EnableDropDownListResize(FALSE);

	bNameValid = strTemp.LoadStringW(IDS_Category_Analysis);
	ASSERT(bNameValid);
	pAnalysisCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_Category_Analysis_Small, IDB_Category_Analysis_Large);

	CMFCRibbonPanel* pAnalysisPanel = pAnalysisCategory->AddPanel(_T(""));
	pAnalysisPanel->SetCenterColumnVert(TRUE);
	pAnalysisPanel->SetJustifyColumns(TRUE);
	std::auto_ptr<CMFCRibbonButton> pBnSingle(new CMFCRibbonButton(ID_BN_Single, _T("单组数据处理分析"), 0, 0));
	pAnalysisPanel->Add(pBnSingle.release());
	pAnalysisPanel->AddSeparator();
	std::auto_ptr<CMFCRibbonButton> pBnMulti(new CMFCRibbonButton(ID_BN_Multi, _T("连续图片处理分析"), 1, 1));
	pAnalysisPanel->Add(pBnMulti.release());
	pAnalysisPanel->AddSeparator();
	std::auto_ptr<CMFCRibbonButton> pBnMenu(new CMFCRibbonButton(ID_BN_Settings, _T("图像选项"), 2, 2));
	pAnalysisPanel->Add(pBnMenu.release());
	CMFCRibbonButton* pBnSettingMenu = DYNAMIC_DOWNCAST(CMFCRibbonButton, pAnalysisPanel->FindByID(ID_BN_Settings));
	pBnSettingMenu->SetMenu(IDR_MENU_Settings);
	pBnSettingMenu->SetAlwaysLargeImage(TRUE);
	bInitRibbon = true;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (CameraAcess->bAcquistion == true)
		m_wndStatusBar.SetInformation(_T("正在采样..."));
	else if (CameraAcess->bSavingData == true)
		m_wndStatusBar.SetInformation(_T("正在保存数据..."));
	else if (CDASDlg::GetInstance()->bMonitor == true)
	{
		//m_wndStatusBar.SetInformation(_T("正在监控..."));
		CString cstr;
		cstr.Format(_T("%s%.2f"), _T("帧数: "), fps);
		m_wndStatusBar.SetInformation(cstr);
	}
	else
	{
		m_wndStatusBar.SetInformation(_T("就绪"));
	}

	m_wndStatusBar.RedrawWindow();

	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		switch (pMsg->wParam)
		{
		case VK_F5:	OnBnMonitor();	break;
			//case VK_F6:	OnBnAcquisition();	break;
			//case VK_F7:	OnBnBgshot();	break;
		default:	break;
		}
		if (CDASDlg::GetInstance()->bMonitor == true)
		{
			if (pMsg->wParam == VK_OEM_PLUS || pMsg->wParam == VK_ADD)
			{
				OnBnZoomin();
			}
			if (pMsg->wParam == VK_OEM_MINUS || pMsg->wParam == VK_SUBTRACT)
			{
				OnBnZoomout();
			}
		}
		return CFrameWndEx::PreTranslateMessage(pMsg);
	}

	return CFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// Prevent the app from moving
	if (nID == SC_MOVE || nID == 0xF012)	//0xF012 SC_DRAGMOVE
	{
		return;
	}
	CFrameWndEx::OnSysCommand(nID, lParam);
}

afx_msg LRESULT CMainFrame::OnAfxWmOnChangeRibbonCategory(WPARAM wParam, LPARAM lParam)
{
	CMFCRibbonCategory *pCategory = m_wndRibbonBar.GetActiveCategory();
	int nCategory = m_wndRibbonBar.GetCategoryIndex(pCategory);

	CRect ModuleRect = CRect(TgtRect.left, TgtRect.top, TgtRect.Width()*0.25 + TgtRect.left, TgtRect.bottom);
	ShowDlgRect = CRect(TgtRect.left + TgtRect.Width()*0.25, TgtRect.top, TgtRect.right, TgtRect.bottom);

	switch (nCategory)
	{
	case 0:
		if (CShowDlg::getInstance() != NULL && mSingleWnd != NULL && mMultiWnd != NULL)
		{
			m_singleDlg->ShowWindow(SW_HIDE);
			m_multiDlg->ShowWindow(SW_HIDE);
			CShowDlg::getInstance()->ShowWindow(SW_HIDE);
		}
		if (mCWnd != NULL)
		{
			CDASDlg::GetInstance()->MoveWindow(&TgtRect);
			CDASDlg::GetInstance()->ShowWindow(SW_SHOW);
			//RedrawWindow();
			CDASDlg::GetInstance()->ChangeCtrlSize(TgtRect);
		}
		break;
	case 1:
		if (mCWnd != NULL)
		{
			CDASDlg::GetInstance()->ShowWindow(SW_HIDE);
		}
		if (CShowDlg::getInstance() != NULL)
		{
			CShowDlg::getInstance()->MoveWindow(&ShowDlgRect);
			CShowDlg::getInstance()->ShowWindow(SW_SHOW);
			CRect StaticRect = CRect(0, 10, ShowDlgRect.Width() - 30, ShowDlgRect.Height() - 10);
			CShowDlg::getInstance()->GetDlgItem(IDC_STATIC_Analysis_Result)->MoveWindow(&StaticRect);
		}
		if (mSingleWnd != NULL)
		{
			m_singleDlg->MoveWindow(&ModuleRect);
			//mSingleWnd->SetDlgItemInt(IDC_EDIT_Target_width, CameraAcess->Width);
			//mSingleWnd->SetDlgItemInt(IDC_EDIT_Target_Height, CameraAcess->Height);
		}
		if (mMultiWnd != NULL)
		{
			m_multiDlg->MoveWindow(&ModuleRect);
			//mMultiWnd->SetDlgItemInt(IDC_EDIT_Target_width2, CameraAcess->Width);
			//mMultiWnd->SetDlgItemInt(IDC_EDIT_Target_Height2, CameraAcess->Height);
		}
		RedrawWindow();
		OnBnSingle();
		break;
	default:
		break;
	}
	CShowDlg::getInstance()->PlayOnce();
	return 0;
}

void CMainFrame::OnBnSingle()
{
	bOnSinglePanel = true;
	bOnMultiPanel = false;
	m_singleDlg->ShowWindow(SW_SHOW);
	m_multiDlg->ShowWindow(SW_HIDE);
	m_wndRibbonBar.RedrawWindow();
}
void CMainFrame::OnUpdateBnSingle(CCmdUI *pCmdUI)
{
	if (bOnSinglePanel == true)
	{
		pCmdUI->SetRadio(TRUE);
	}
	else
	{
		pCmdUI->SetRadio(FALSE);
	}
}

void CMainFrame::OnBnMulti()
{
	bOnMultiPanel = true;
	bOnSinglePanel = false;
	m_multiDlg->ShowWindow(SW_SHOW);
	m_singleDlg->ShowWindow(SW_HIDE);
	m_wndRibbonBar.RedrawWindow();
}
void CMainFrame::OnUpdateBnMulti(CCmdUI *pCmdUI)
{
	if (bOnMultiPanel == true)
	{
		pCmdUI->SetRadio(TRUE);
	}
	else
	{
		pCmdUI->SetRadio(FALSE);
	}
}

void CMainFrame::OnUpdateBnSettings(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}
void CMainFrame::OnBnSettings()
{
	// TODO: 在此添加命令处理程序代码
}

void CMainFrame::OnSglcolor()
{
	CShowDlg::getInstance()->showSglColor();
}
void CMainFrame::OnUpdateSglcolor(CCmdUI *pCmdUI)
{
	CShowDlg::getInstance()->MenuCheck();
	if (CShowDlg::getInstance()->bSglMenuShow == true)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnSglgray()
{
	CShowDlg::getInstance()->showSglGray();
}
void CMainFrame::OnUpdateSglgray(CCmdUI *pCmdUI)
{
	CShowDlg::getInstance()->MenuCheck();
	if (CShowDlg::getInstance()->bSglMenuShow == true)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnMultiuncolor()
{
	CShowDlg::getInstance()->showMultiUnwrapColor();
}
void CMainFrame::OnUpdateMultiuncolor(CCmdUI *pCmdUI)
{
	CShowDlg::getInstance()->MenuCheck();
	if (CShowDlg::getInstance()->bMultiUnwrapMenuShow == true)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnMultiungray()
{
	CShowDlg::getInstance()->showMultiUnwrapGray();
}
void CMainFrame::OnUpdateMultiungray(CCmdUI *pCmdUI)
{
	CShowDlg::getInstance()->MenuCheck();
	if (CShowDlg::getInstance()->bMultiUnwrapMenuShow == true)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnMulticolor()
{
	CShowDlg::getInstance()->showMultiWrapColor();
}
void CMainFrame::OnUpdateMulticolor(CCmdUI *pCmdUI)
{
	CShowDlg::getInstance()->MenuCheck();
	if (CShowDlg::getInstance()->bMultiWrapMenuShow == true)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnMultigray()
{
	CShowDlg::getInstance()->showMultiWrapGray();
}
void CMainFrame::OnUpdateMultigray(CCmdUI *pCmdUI)
{
	CShowDlg::getInstance()->MenuCheck();
	if (CShowDlg::getInstance()->bMultiWrapMenuShow == true)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnBnSavebmp()
{
	CShowDlg::getInstance()->saveImg(_T("bmp"));
}
void CMainFrame::OnUpdateBnSavebmp(CCmdUI *pCmdUI)
{
	CShowDlg::getInstance()->MenuCheck();
	if (CShowDlg::getInstance()->bSaveSgl == true)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnBnSavejpeg()
{
	CShowDlg::getInstance()->saveImg(_T("jpeg"));
}
void CMainFrame::OnUpdateBnSavejpeg(CCmdUI *pCmdUI)
{
	CShowDlg::getInstance()->MenuCheck();
	if (CShowDlg::getInstance()->bSaveSgl == true)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnBnSavetiff()
{
	CShowDlg::getInstance()->saveImg(_T("tiff"));
}
void CMainFrame::OnUpdateBnSavetiff(CCmdUI *pCmdUI)
{
	CShowDlg::getInstance()->MenuCheck();
	if (CShowDlg::getInstance()->bSaveSgl == true)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnBnSavegif()
{
	CShowDlg::getInstance()->saveImg(_T(""));
}
void CMainFrame::OnUpdateBnSavegif(CCmdUI *pCmdUI)
{
	CShowDlg::getInstance()->MenuCheck();
	if (CShowDlg::getInstance()->bSaveMulti == true)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (CDASDlg::GetInstance()->bMonitor == true)
		if (bScaleControl == true)
		{
			SetCursor(LoadCursor(NULL, IDC_HAND));
			return TRUE;
		}

	return CFrameWndEx::OnSetCursor(pWnd, nHitTest, message);
}

void CMainFrame::OnComboColormode()
{
	int cursel = pComboBoxColorMode->GetCurSel();
	if (cursel == 0)
		CameraAcess->colormode = CameraAcess->MONO;
	else
		CameraAcess->colormode = CameraAcess->RGB;
}
void CMainFrame::OnUpdateComboColormode(CCmdUI *pCmdUI)
{
	if (CameraAcess->m_mode == CameraAcess->NORMAL)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnComboMonitormode()
{
	int cursel = pComboBoxMonitorMode->GetCurSel();
	switch (cursel)
	{
	case 0:
		CameraAcess->m_mode = CameraAcess->NORMAL;
		//MAX_EXPOSURE_SLD_MS = 5000;
		//CDASDlg::GetInstance()->OnChangeComboExposureUnit();
		mCWnd->GetDlgItem(IDC_RADIO_Single)->EnableWindow(TRUE);
		mCWnd->GetDlgItem(IDC_RADIO_Multi)->EnableWindow(TRUE);
		mCWnd->GetDlgItem(IDC_RADIO_AF_Bgshot)->EnableWindow(FALSE);
		CDASDlg::GetInstance()->OnBnClickedRadioSingle();
		((CButton*)mCWnd->GetDlgItem(IDC_RADIO_Single))->SetCheck(TRUE);
		((CButton*)mCWnd->GetDlgItem(IDC_RADIO_Multi))->SetCheck(FALSE);
		((CButton*)mCWnd->GetDlgItem(IDC_RADIO_AF_Bgshot))->SetCheck(FALSE);
		mCWnd->SetDlgItemInt(IDC_EDIT_BINNING_HOR, 1);
		mCWnd->SetDlgItemInt(IDC_EDIT_BINNING_VER, 1);

		mCWnd->GetDlgItem(IDC_EDIT_PHASEMAX)->EnableWindow(FALSE);
		mCWnd->GetDlgItem(IDC_EDIT_PHASEMIN)->EnableWindow(FALSE);
		mCWnd->GetDlgItem(IDC_EDIT_PHASESTEP)->EnableWindow(FALSE);

		break;
	case 1:
		CameraAcess->m_mode = CameraAcess->AMP_FREQ;
		//MAX_EXPOSURE_SLD_MS = 800;
		//CDASDlg::GetInstance()->OnChangeComboExposureUnit();
		mCWnd->GetDlgItem(IDC_RADIO_Single)->EnableWindow(FALSE);
		mCWnd->GetDlgItem(IDC_RADIO_Multi)->EnableWindow(FALSE);
		mCWnd->GetDlgItem(IDC_RADIO_AF_Bgshot)->EnableWindow(TRUE);
		
		((CButton*)mCWnd->GetDlgItem(IDC_RADIO_Single))->SetCheck(FALSE);
		((CButton*)mCWnd->GetDlgItem(IDC_RADIO_Multi))->SetCheck(FALSE);
		((CButton*)mCWnd->GetDlgItem(IDC_RADIO_AF_Bgshot))->SetCheck(TRUE);
		mCWnd->SetDlgItemInt(IDC_EDIT_BINNING_HOR, 1);
		mCWnd->SetDlgItemInt(IDC_EDIT_BINNING_VER, 1);
		CDASDlg::GetInstance()->OnBnClickedRadioAfBgshot();
		mCWnd->GetDlgItem(IDC_EDIT_PHASEMAX)->EnableWindow(FALSE);
		mCWnd->GetDlgItem(IDC_EDIT_PHASEMIN)->EnableWindow(FALSE);
		mCWnd->GetDlgItem(IDC_EDIT_PHASESTEP)->EnableWindow(FALSE);

		break;
	case 2:
		CameraAcess->m_mode = CameraAcess->PHASE_FREQ;
		//MAX_EXPOSURE_SLD_MS = 800;
		//CDASDlg::GetInstance()->OnChangeComboExposureUnit();
		mCWnd->GetDlgItem(IDC_RADIO_Single)->EnableWindow(FALSE);
		mCWnd->GetDlgItem(IDC_RADIO_Multi)->EnableWindow(FALSE);
		mCWnd->GetDlgItem(IDC_RADIO_AF_Bgshot)->EnableWindow(TRUE);
		
		((CButton*)mCWnd->GetDlgItem(IDC_RADIO_Single))->SetCheck(FALSE);
		((CButton*)mCWnd->GetDlgItem(IDC_RADIO_Multi))->SetCheck(FALSE);
		((CButton*)mCWnd->GetDlgItem(IDC_RADIO_AF_Bgshot))->SetCheck(TRUE);
		mCWnd->SetDlgItemInt(IDC_EDIT_BINNING_HOR, 1);
		mCWnd->SetDlgItemInt(IDC_EDIT_BINNING_VER, 1);
		CDASDlg::GetInstance()->OnBnClickedRadioAfBgshot();
		mCWnd->GetDlgItem(IDC_EDIT_PHASEMAX)->EnableWindow(TRUE);
		mCWnd->GetDlgItem(IDC_EDIT_PHASEMIN)->EnableWindow(TRUE);
		mCWnd->GetDlgItem(IDC_EDIT_PHASESTEP)->EnableWindow(TRUE);

		break;
	}
}
void CMainFrame::OnUpdateComboMonitormode(CCmdUI *pCmdUI)
{
	if (CDASDlg::GetInstance()->bMonitor == true)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}


void CMainFrame::OnNcMButtonDblClk(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	/*CFrameWndEx::OnNcMButtonDblClk(nHitTest, point);*/
}

void CMainFrame::OnNcMButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CFrameWndEx::OnNcMButtonDown(nHitTest, point);
}

