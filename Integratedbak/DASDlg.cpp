// DASDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Integratedbak.h"
#include "DASDlg.h"
#include "afxdialogex.h"

Vimba* Apicontroller = Vimba::GetInstance();
CDASDlg* CDASDlg::pInstance = NULL;
CDASDlg::CGarbo CDASDlg::Garbo;

#pragma warning(once:4244)

// CDASDlg 对话框

// 定义全局变量
CWnd*			mCWnd;						// this
CRect			PictureCRect;				// 图像控件位置
int				CameraRefreshTime;			// 相机监控刷新时间
CEvent			CameraStopEvent(0, 1);		// 退出实时监控事件
CEvent			ProcessEvent(0, 1);
double			exposure;					// 曝光时间
double			gain;						// 增	益
CString			cExposure;					// 控件值
CString			cGain;						// 控件值
CPoint			m_PreviousPoint;			//
bool			bMaximize = true;			//
bool			bScaleControl = false;		//
bool			bLButtonClick = false;		//
CDASDlg::unit	Unit;						// 
CString			SavePath;					// 文件保存路径
CString			IniPath;					// ini文件路径
CString			XmlFile;					// default_settings
CString			filename_prefix = _T("im_");
int				MAX_EXPOSURE_SLD_MS = 5000;	// 滑块最大曝光值(ms)
int				MAX_EXPOSURE_SLD_US = 999;	// 滑块最大曝光值(us)

UINT Count = 0;
double s1 = 0, s2 = 0;
double fps = 0;

extern CWnd* mSingleWnd;
extern CWnd* mMultiWnd;
extern IMGINFO DataInfo;
// 活动控件ID
UINT ActiveID[] = { IDC_EDIT_Camera_Exposure,	IDC_BUTTON_Monitor,		IDC_BUTTON_Sampling };

UINT CtrlID[] = { IDC_STATIC_GExposure ,		IDC_STATIC_GGain	,	IDC_SLIDER_Exposure,	IDC_SLIDER_Gain,
					IDC_EDIT_Camera_Exposure,	IDC_EDIT_Camera_Gain,	IDC_COMBO_Exposure_Unit,IDC_STATIC_Camera_Gain_Unit };

IMPLEMENT_DYNAMIC(CDASDlg, CDialogEx)

CDASDlg::CDASDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DataAcquisition, pParent)
	, cSamplingNum(_T("2"))
	, interval(200)
{
	PictureCWnd = NULL;
	bMonitor = false;
	bAcquiring = false;

	m_scrollbar_hor = new CScrollBar();
	m_scrollbar_ver = new CScrollBar();
	font = new CFont;
}

CDASDlg::~CDASDlg()
{
	delete m_scrollbar_hor;
	m_scrollbar_hor = NULL;
	delete m_scrollbar_ver;
	m_scrollbar_ver = NULL;
	delete font;
}

void CDASDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	// 相机模块文本框
	CTime ct = CTime::GetCurrentTime();
	CurrentDate = ct.Format("20%y%m%d_");
	DDX_Text(pDX, IDC_EDIT_Camera_Exposure, cExposure);
	DDX_Text(pDX, IDC_EDIT_Camera_Gain, cGain);
	DDX_Control(pDX, IDC_SLIDER_Exposure, Slider_Exposure);
	DDX_Control(pDX, IDC_SLIDER_Gain, Slider_Gain);
	DDX_Control(pDX, IDC_COMBO_Exposure_Unit, ComboExposureUnit);
	DDX_Control(pDX, IDC_EDIT_Camera_Exposure, EditExposure);
	DDX_Control(pDX, IDC_EDIT_Camera_Gain, EditGain);
	DDX_Text(pDX, IDC_EDIT_Camera_SamplingNumber, cSamplingNum);
	DDX_Text(pDX, IDC_EDIT_Camera_Interval, interval);
}


BEGIN_MESSAGE_MAP(CDASDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_Monitor, &CDASDlg::OnBnClickedButtonMonitor)
	ON_BN_CLICKED(IDC_BUTTON_Sampling, &CDASDlg::OnBnClickedButtonAcquiring)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_Exposure, &CDASDlg::OnNMCustomdrawSliderExposure)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_Gain, &CDASDlg::OnNMCustomdrawSliderGain)
	ON_EN_CHANGE(IDC_EDIT_Camera_Exposure, &CDASDlg::OnEnChangeEditCameraExposure)
	ON_EN_CHANGE(IDC_EDIT_Camera_Gain, &CDASDlg::OnEnChangeEditCameraGain)
	ON_EN_CHANGE(IDC_EDIT_Camera_Exposure, &CDASDlg::OnEnChangeEditCameraExposure)
	ON_EN_CHANGE(IDC_EDIT_Camera_Gain, &CDASDlg::OnEnChangeEditCameraGain)
	ON_CBN_SELCHANGE(IDC_COMBO_Exposure_Unit, &CDASDlg::OnCbnSelchangeComboExposureUnit)
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_FRAME_READY, &CDASDlg::OnFrameReady)
	ON_BN_CLICKED(IDC_RADIO_Single, &CDASDlg::OnBnClickedRadioSingle)
	ON_BN_CLICKED(IDC_RADIO_Multi, &CDASDlg::OnBnClickedRadioMulti)
	ON_BN_CLICKED(IDC_RADIO_AF_Bgshot, &CDASDlg::OnBnClickedRadioAfBgshot)
END_MESSAGE_MAP()


// CDASDlg 消息处理程序

BOOL CDASDlg::OnInitDialog()
{
	SetBackgroundColor(RGB(255, 255, 255));
	CDialogEx::OnInitDialog();

	// 初始化时填充控件
	ComboExposureUnit.AddString(_T("ms"));
	ComboExposureUnit.AddString(_T("μs"));
	ComboExposureUnit.SetCurSel(0);

	// 设置字体
	font->CreateFont(
		24,							// nHeight 
		0,							// nWidth 
		0,							// nEscapement 
		0,							// nOrientation 
		FW_LIGHT,					// nWeight 
		FALSE,						// bItalic 
		FALSE,						// bUnderline 
		0,							// cStrikeOut 
		ANSI_CHARSET,				// nCharSet 
		OUT_DEFAULT_PRECIS,			// nOutPrecision 
		CLIP_DEFAULT_PRECIS,		// nClipPrecision 
		DEFAULT_QUALITY,			// nQuality 
		DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily 
		_T("微软雅黑"));				// lpszFac

	GetDlgItem(IDC_BUTTON_Monitor)->SetFont(font, TRUE);
	GetDlgItem(IDC_BUTTON_Sampling)->SetFont(font, TRUE);

	// 操作ini文件
	OnIni();

	if (exposure < 1000)
	{
		ComboExposureUnit.SetCurSel(1);
		int tempexposure = exposure;
		OnCbnSelchangeComboExposureUnit();
		Slider_Exposure.SetPos(tempexposure);
	}
	else
	{
		ComboExposureUnit.SetCurSel(0);
		int tempexposure = exposure;
		OnCbnSelchangeComboExposureUnit();
		Slider_Exposure.SetPos(tempexposure * 0.001);
	}

	Slider_Gain.SetRange(0, 33);
	Slider_Gain.SetTicFreq(1);
	Slider_Gain.SetPos(gain);

	mCWnd = this;
	PictureCWnd = GetDlgItem(IDC_STATIC_Picture);
	PictureCWnd->GetClientRect(&PictureCRect);

	((CButton*)GetDlgItem(IDC_RADIO_Single))->SetCheck(TRUE);
	OnBnClickedRadioSingle();

	/////////////////////////////////////

	//SetDlgItemInt(IDC_EDIT_Width, Apicontroller->Width);
	//SetDlgItemInt(IDC_EDIT_Height, Apicontroller->Height);
	//SetDlgItemInt(IDC_EDIT_OffsetX, Apicontroller->OffsetX);
	//SetDlgItemInt(IDC_EDIT_OffsetY, Apicontroller->OffsetY);
	SetDlgItemInt(IDC_EDIT_BINNING_HOR, Apicontroller->BinningHor);
	SetDlgItemInt(IDC_EDIT_BINNING_VER, Apicontroller->BinningVer);
	SetDlgItemInt(IDC_EDIT_PHASESTEP, 10);
	GetDlgItem(IDC_EDIT_PHASEMIN)->SetWindowTextW(CString(_T("0.01")));
	GetDlgItem(IDC_EDIT_PHASEMAX)->SetWindowTextW(CString(_T("0.1")));
	/////////////////////////////////////

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CDASDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	CMemDC dcmem(*(mCWnd->GetDC()), PictureCRect);
	if (bMonitor == false)
		dcmem.GetDC().FillSolidRect(PictureCRect, RGB(205, 205, 205));
	if (bMaximize == true)
	{
		CRect rc_e;
		CRect rc_g;
		mCWnd->GetDlgItem(IDC_SLIDER_Exposure)->GetWindowRect(&rc_e);
		mCWnd->GetDlgItem(IDC_SLIDER_Gain)->GetWindowRect(&rc_g);
		mCWnd->ScreenToClient(&rc_e);
		mCWnd->ScreenToClient(&rc_g);
		CString cstr;
		CPen *pPenD = new CPen();
		pPenD->CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		CPen *pPen = NULL;
		CGdiObject *pOldPen = dc.SelectObject(pPenD);

		dc.MoveTo(rc_e.left + 13, rc_e.top - 5);
		dc.LineTo(rc_e.right - 13, rc_e.top - 5);

		int interval_x = (rc_e.right - rc_e.left - 24) / 5;
		int index;
		if (ComboExposureUnit.GetCurSel() == 0)
			Unit = MS;
		else
			Unit = US;
		switch (Unit)
		{
		case US:	index = MAX_EXPOSURE_SLD_US*0.2;	break;
		case MS:	index = MAX_EXPOSURE_SLD_MS*0.2;	break;
		default:	break;
		}
		for (int i = 0; i <= 5; i++)
		{
			int axie_x = i * index;
			if (Unit == US && i == 0)
			{
				axie_x = 43;
			}
			cstr.Format(_T("%d"), axie_x);
			dc.TextOut(rc_e.left + 6 + (interval_x)* i, rc_e.top - 35, cstr);
			dc.MoveTo(i * interval_x + rc_e.left + 13, rc_e.top - 5);
			dc.LineTo(i * interval_x + rc_e.left + 13, rc_e.top - 10);
		}

		dc.MoveTo(rc_g.left + 13, rc_g.top - 5);
		dc.LineTo(rc_g.right - 13, rc_g.top - 5);
		interval_x = (rc_g.right - rc_g.left - 24) / 5;
		for (int i = 0; i <= 5; i++)
		{
			int axie_x = i * 33 * 0.2;
			cstr.Format(_T("%d"), axie_x);
			dc.TextOut(rc_g.left + 6 + (interval_x)* i, rc_g.top - 35, cstr);
			dc.MoveTo(i * interval_x + rc_g.left + 13, rc_g.top - 5);
			dc.LineTo(i * interval_x + rc_g.left + 13, rc_g.top - 10);
		}

		dc.SelectObject(pOldPen);

		delete pPenD;
		if (pPen != NULL)
			delete pPen;
	}
}


// 防止界面卡死程序
void DoEvents()
{
	// 定义一个MSG类型的变量
	MSG msg;
	// 获取消息并把该消息从消息队列中移除（防止重复响应）。
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// 将消息移交给过程函数
		DispatchMessage(&msg);
		// 翻译消息，在合适的机会产生char消息
		TranslateMessage(&msg);
	}
}

// 滑块消息响应
void CDASDlg::OnNMCustomdrawSliderExposure(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData(TRUE);
	switch (Unit)
	{
	case CDASDlg::MS:	exposure = Slider_Exposure.GetPos() * 1000;	break;
	case CDASDlg::US:	exposure = Slider_Exposure.GetPos();	break;
	default:	break;
	}
	cExposure.Format(_T("%d"), (int)Slider_Exposure.GetPos());
	UpdateData(FALSE);
	mCWnd->SetFocus();
	*pResult = 0;
}
void CDASDlg::OnNMCustomdrawSliderGain(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	UpdateData(TRUE);
	gain = Slider_Gain.GetPos();
	cGain.Format(_T("%d"), (int)gain);
	UpdateData(FALSE);
	*pResult = 0;
	mCWnd->SetFocus();
}

// 编辑框消息响应
void CDASDlg::OnEnChangeEditCameraExposure()
{
	UpdateData(TRUE);
	int length = cExposure.GetLength();
	for (int i = 0; i < length; i++)
	{
		if (cExposure.GetAt(i) < '0' || cExposure.GetAt(i) > '9')
		{
			cExposure = cExposure.Left(i);
			EditExposure.SetWindowText(cExposure);
			EditExposure.SetSel(i, i, TRUE);
			return;
		}
	}
	UpdateData(FALSE);
}
void CDASDlg::OnEnChangeEditCameraGain()
{
	UpdateData(true);
	int length = cGain.GetLength();
	for (int i = 0; i < length; i++)
	{
		if (cGain.GetAt(i) < '0' || cGain.GetAt(i) > '9')
		{
			cGain = cGain.Left(i);
			EditGain.SetWindowText(cGain);
			EditGain.SetSel(i, i, TRUE);
			return;
		}
	}
	UpdateData(false);
}

// 相机实时监控线程
static UINT Thread_Camera(LPVOID lpParam)
{
	// 不断循环检测
	DWORD dw;
	CWnd *pWnd = mCWnd->GetDlgItem(IDC_STATIC_Picture);
	double tempexposure = -1, tempgain = -1;
	if (Apicontroller->InitCamera())
		while (1)
		{
			// 检测曝光/增益值改变
			dw = WaitForSingleObject(CameraStopEvent, 16);
			if (dw == WAIT_TIMEOUT)
			{
				if (tempexposure != exposure)
				{
					Apicontroller->SetExposure(exposure);
					tempexposure = exposure;
				}
				if (tempgain != gain)
				{
					Apicontroller->SetGain(gain);
					tempgain = gain;
				}
				Apicontroller->Draw(pWnd);
			}
			else
			{
				break;
			}
		}
	Apicontroller->StopContinuousImageAcquisition();
	Apicontroller->bIsCameraGet = false;
	CDASDlg::GetInstance()->bMonitor = false;
	Apicontroller->ApiShutdown();
	CameraStopEvent.ResetEvent();
	DoEvents();
	return 0;
}

static UINT Thread_AMP_FREQ(LPVOID lpParam)
{

	// 不断循环检测
	DWORD dw;
	double tempexposure = -1, tempgain = -1;
	if (Apicontroller->InitCamera())
		while (1)
		{
			// 检测曝光/增益值改变
			dw = WaitForSingleObject(CameraStopEvent, 32);
			if (dw == WAIT_TIMEOUT)
			{
				if (tempexposure != exposure)
				{
					Apicontroller->SetExposure(exposure);
					tempexposure = exposure;
				}
				if (tempgain != gain)
				{
					Apicontroller->SetGain(gain);
					tempgain = gain;
				}
				CWnd* pWnd = mCWnd->GetDlgItem(IDC_STATIC_Picture);
				Apicontroller->phaseStep = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_PHASESTEP);
				if (Apicontroller->phaseStep <= 0)
				{
					AfxGetMainWnd()->MessageBox(_T("步长必须大于0！"), _T("警告"));
					break;
				}
				CString str[2];

				CDASDlg::GetInstance()->GetDlgItem(IDC_EDIT_PHASEMAX)->GetWindowTextW(str[0]);
				Apicontroller->phaseMax = _wtof(str[0]);
				CDASDlg::GetInstance()->GetDlgItem(IDC_EDIT_PHASEMIN)->GetWindowTextW(str[1]);
				Apicontroller->phaseMin = _wtof(str[1]);
				if (Apicontroller->phaseMax <= Apicontroller->phaseMin ||
					Apicontroller->phaseMax <= 0 || Apicontroller->phaseMin < 0 ||
					Apicontroller->phaseMax>1 || Apicontroller->phaseMin > 1)
				{
					AfxGetMainWnd()->MessageBox(_T("相频显示范围选择错误！"), _T("警告"));
					break;
				}
				Apicontroller->DataProcess();
				
				Apicontroller->Draw(pWnd);
				DoEvents();
			}
			else
			{
				break;
			}
		}
	Apicontroller->StopContinuousImageAcquisition();
	Apicontroller->bIsCameraGet = false;
	CDASDlg::GetInstance()->bMonitor = false;
	Apicontroller->ApiShutdown();
	CameraStopEvent.ResetEvent();
	DoEvents();
	mCWnd->RedrawWindow();
	return 0;
}

static UINT Thread_Process(LPVOID lpParam)
{
	DWORD dw;
	while (1)
	{
		if (Apicontroller->bIsCameraGet)
		{
			dw = WaitForSingleObject(ProcessEvent, 16);
			if (dw == WAIT_TIMEOUT)
			{
				Apicontroller->DataProcess();
			}
			else
			{
				break;
			}
		}
	}
	ProcessEvent.ResetEvent();
	return 0;
}

// 曝光时间单位改变事件
void CDASDlg::OnCbnSelchangeComboExposureUnit()
{
	if (ComboExposureUnit.GetCurSel() == 0)
		Unit = MS;
	else
		Unit = US;
	switch (Unit)
	{
	case US:
		//exposure = 43;
		Slider_Exposure.SetRange(43, MAX_EXPOSURE_SLD_US);
		Slider_Exposure.SetPos(exposure);
		cExposure.Format(_T("%d"), (int)exposure);
		break;
	case MS:
		//exposure = 1000;
		Slider_Exposure.SetRange(1, MAX_EXPOSURE_SLD_MS);
		Slider_Exposure.SetPos(exposure*0.001);
		cExposure.Format(_T("%d"), (int)exposure*0.001);
		break;
	default:
		break;
	}
	Slider_Exposure.SetTicFreq(1);

	UpdateData(FALSE);
	RedrawWindow();
}

void CDASDlg::OnChangeComboExposureUnit()
{
	if (ComboExposureUnit.GetCurSel() == 0)
		Unit = MS;
	else
		Unit = US;
	switch (Unit)
	{
	case US:
		if (exposure > MAX_EXPOSURE_SLD_US)
			exposure = MAX_EXPOSURE_SLD_US;
		Slider_Exposure.SetRange(43, MAX_EXPOSURE_SLD_US);
		Slider_Exposure.SetPos(exposure);
		cExposure.Format(_T("%d"), (int)exposure);
		break;
	case MS:
		if (exposure > MAX_EXPOSURE_SLD_MS * 1000)
			exposure = MAX_EXPOSURE_SLD_MS * 1000;
		Slider_Exposure.SetRange(1, MAX_EXPOSURE_SLD_MS);
		Slider_Exposure.SetPos(exposure*0.001);
		cExposure.Format(_T("%d"), (int)exposure*0.001);
		break;
	default:
		break;
	}
	Slider_Exposure.SetTicFreq(1);

	UpdateData(FALSE);
	RedrawWindow();
}

// 监控按钮事件
void CDASDlg::OnBnClickedButtonMonitor()
{
	Count = 0;

	switch (bMonitor)
	{
	case false:
		bMonitor = true;
		VmbErrorType err;
		if (Apicontroller->m_mode == Apicontroller->NORMAL)
		{

			err = Apicontroller->StartContinuousImageAcquisition();
			if (VmbErrorSuccess != err)
			{
				Apicontroller->ApiShutdown();
				bMonitor = false;
				Apicontroller->bIsCameraGet = false;
				break;
			}
			AfxBeginThread(Thread_Camera, this);
		}
		else
		{
			int temp1 = GetDlgItemInt(IDC_EDIT_BINNING_HOR);
			int temp2 = GetDlgItemInt(IDC_EDIT_BINNING_VER);
			if (temp1 <= 0 || temp2 <= 0 || temp1 > 4 || temp2 > 4)
			{
				AfxGetMainWnd()->MessageBox(_T("横纵缩放倍数应大于0，小于等于4！"), _T("警告"));
				return;
			}
			CString bk;
			bk.Format(_T("Data\\BK_%d_%d.bin"), temp1, temp2);
			if (!DataCheck(bk))
			{
				AfxGetMainWnd()->MessageBox(_T("不存在干涉背景图！请先采集再开启监控！"), _T("警告"));
				return;
			}
			switch (temp1)
			{
			case 1:DataInfo.targetWidth = 1388;
				DataInfo.freCount = 694;
				DataInfo.endCol = 694;
				break;
			case 2:DataInfo.targetWidth = 694;
				DataInfo.freCount = 650;
				DataInfo.endCol = 650;				break;
			case 3:DataInfo.targetWidth = 462;
				DataInfo.freCount = 450; DataInfo.endCol = 450; break;
			case 4:DataInfo.targetWidth = 346;
				DataInfo.freCount = 340; DataInfo.endCol = 346; break;
			default:
				break;
			}
			switch (temp2)
			{
			case 1:DataInfo.targetHeight = 1038; DataInfo.endRow = 1038; break;
			case 2:DataInfo.targetHeight = 518; DataInfo.endRow = 500; break;
			case 3:DataInfo.targetHeight = 346; DataInfo.endRow = 346; break;
			case 4:DataInfo.targetHeight = 258; DataInfo.endRow = 258; break;
			default:
				break;
			}
			//DataInfo.freCount = DataInfo.targetWidth / 2;
			//AfxBeginThread(Thread_Process, this);
			err = Apicontroller->StartContinuousImageAcquisition();
			if (VmbErrorSuccess != err)
			{
				Apicontroller->ApiShutdown();
				bMonitor = false;
				Apicontroller->bIsCameraGet = false;
				break;
			}
			AfxBeginThread(Thread_AMP_FREQ, this);
			//AfxBeginThread(Thread_Process, this);
		}
		break;
	case true:
		CameraStopEvent.SetEvent();
		// 		if (Apicontroller->m_mode != Vimba::NORMAL)
		// 			ProcessEvent.SetEvent();
		Sleep(500);
		DoEvents();
		if (mCWnd->GetDlgItem(IDC_SCROLLBAR_HOR) != NULL)
			m_scrollbar_hor->DestroyWindow();
		if (mCWnd->GetDlgItem(IDC_SCROLLBAR_VER) != NULL)
			m_scrollbar_ver->DestroyWindow();
		Apicontroller->offsetX = 0;
		Apicontroller->offsetY = 0;
		Apicontroller->scale = 1.0;
		break;
	}
	Sleep(150);
}

// 开始采样按钮事件
void CDASDlg::OnBnClickedButtonAcquiring()
{
	if (bMonitor == true)
		OnBnClickedButtonMonitor();
	bAcquiring = true;

	CString cstr;
	AcquiringParam param;
	CSamplingProg SamplingProgDlg;

	switch (AcquisitionStatus)
	{
	case CDASDlg::SingleShot:
		OnBnClickedButtonBgshot();
		OnBnClickedRadioSingle();
		break;
	case CDASDlg::MultiShot:
		if (MessageBox(_T("开始采样？"), _T("提示"), MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
			return;

		GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->GetWindowTextW(cSamplingNum);
		SamplingNumber = _wtoi(cSamplingNum);

		// 检测参数是否都填上
		if (SamplingNumber == 0 || SamplingNumber > 500)
		{
			MessageBox(_T("请正确填写参数，样本数量范围为1到500"), _T("提示"), MB_OK | MB_ICONWARNING);
			return;
		}
		mCWnd->GetDlgItem(IDC_EDIT_Camera_Interval)->GetWindowTextW(cstr);
		interval = _wtoi(cstr);
		if (interval == 0)
		{
			MessageBox(_T("请正确填写参数"), _T("提示"), MB_OK | MB_ICONWARNING);
			return;
		}
		if (interval > 5000)
		{
			MessageBox(_T("间隔过大"), _T("提示"), MB_OK | MB_ICONWARNING);
			return;
		}
		param = { exposure,gain,path,interval,SamplingNumber };
		SamplingProgDlg.param_ = &param;
		SamplingProgDlg.DoModal();
		OnBnClickedRadioMulti();
		break;
	case CDASDlg::AFBgShot:
		Sleep(200);
		OnBnClickedRadioAfBgshot();
		OnAFBgshot();
		break;
	default:
		break;
	}

	bAcquiring = false;
	return;
}

void CDASDlg::OnBnClickedButtonBgshot()
{
	if (bMonitor == true)
		OnBnClickedButtonMonitor();

	if (!Apicontroller->InitCamera())
	{
		Apicontroller->ApiShutdown();
		return;
	}
	Apicontroller->SetParam();
	Apicontroller->BgShot(exposure, gain, BgPath);

	CString cstr;
	cstr.Format(_T("%s%s"), _T("背景保存为"), BgPath);
	AfxGetMainWnd()->MessageBox(cstr, _T("保存成功"));
	mSingleWnd->GetDlgItem(IDC_EDIT_BG_S)->SetWindowTextW(Bgfilename);
	mMultiWnd->GetDlgItem(IDC_EDIT_BG_M)->SetWindowTextW(Bgfilename);
	Apicontroller->ApiShutdown();
	return;
}

void CDASDlg::OnAFBgshot()
{
	if (bMonitor == true)
		OnBnClickedButtonMonitor();

	if (!Apicontroller->InitCamera())
	{
		Apicontroller->ApiShutdown();
		return;
	}
	int temp1 = GetDlgItemInt(IDC_EDIT_BINNING_HOR);
	int temp2 = GetDlgItemInt(IDC_EDIT_BINNING_VER);
	BgPath.Format(_T("%sBK_%d_%d.bin"), SavePath, temp1, temp2);
	Apicontroller->BgShot(exposure, gain, BgPath);

	CString cstr;
	cstr.Format(_T("%s%s"), _T("背景保存为"), BgPath);
	AfxGetMainWnd()->MessageBox(cstr, _T("保存成功"));
	Apicontroller->ApiShutdown();
	return;
}

// 实时响应调度消息
BOOL CDASDlg::PreTranslateMessage(MSG* pMsg)
{
	// Enter键入数据
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_TAB)
		{
			if (GetDlgItem(IDC_EDIT_Camera_Exposure) == GetFocus())
			{
				switch (Unit)
				{
				case MS:
					if (exposure < 1)   exposure = 1000;
					if (exposure > MAX_EXPOSURE_SLD_MS) exposure = MAX_EXPOSURE_SLD_MS * 1000;
					exposure = _wtoi(cExposure) * 1000;
					Slider_Exposure.SetPos(exposure*0.001);
					break;
				case US:
					if (exposure < 43)	exposure = 43;
					if (exposure > MAX_EXPOSURE_SLD_US) exposure = MAX_EXPOSURE_SLD_US;
					exposure = _wtoi(cExposure);
					Slider_Exposure.SetPos(exposure);
					break;
				default:
					break;
				}
				cExposure.Format(_T("%d"), (int)Slider_Exposure.GetPos());
				EditExposure.SetWindowTextW(cExposure);
				return FALSE;
			}
			if (GetDlgItem(IDC_EDIT_Camera_Gain) == GetFocus())
			{
				gain = _wtoi(cGain);
				if (gain > 33)
					gain = 33;
				cGain.Format(_T("%d"), (int)gain);
				EditGain.SetWindowTextW(cGain);
				Slider_Gain.SetPos(gain);
				return FALSE;
			}
			if (GetDlgItem(IDC_EDIT_Camera_SamplingNumber) == GetFocus())
			{
				GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->GetWindowTextW(cSamplingNum);
				SamplingNumber = _wtoi(cSamplingNum);
				if (SamplingNumber > 500 || SamplingNumber < 1)
				{
					MessageBox(_T("样本数量范围为1到500"), _T("警告"), MB_OK | MB_ICONWARNING);
					GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->SetWindowText(_T("1"));
					GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->SetFocus();
					SamplingNumber = 0;
				}
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	CRect dlgrect;
	mCWnd->GetWindowRect(&dlgrect);
	CRect rect_hor = CRect(PictureCRect.left, PictureCRect.top - dlgrect.top, PictureCRect.right, PictureCRect.bottom + 22);
	CRect rect_ver = CRect(PictureCRect.left, PictureCRect.top, PictureCRect.right + 22, PictureCRect.bottom);

	if (m_scrollbar_hor->m_hWnd == NULL)
		m_scrollbar_hor->Create(WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_RIGHTALIGN, rect_hor, mCWnd, IDC_SCROLLBAR_HOR);
	if (m_scrollbar_ver->m_hWnd == NULL)
		m_scrollbar_ver->Create(WS_CHILD | WS_VISIBLE | SBS_VERT | SBS_BOTTOMALIGN, rect_ver, mCWnd, IDC_SCROLLBAR_VER);

	int blockwitdh = 50 * Apicontroller->scale;
	int hSB_MAX = 100 + blockwitdh;
	int vSB_MAX = 100 + blockwitdh*0.628;

	m_scrollbar_hor->SetScrollRange(0, hSB_MAX);
	m_scrollbar_ver->SetScrollRange(0, vSB_MAX);

	SCROLLINFO si;
	m_scrollbar_hor->GetScrollInfo(&si);
	si.nPage = blockwitdh;
	m_scrollbar_hor->SetScrollInfo(&si);

	m_scrollbar_ver->GetScrollInfo(&si);
	si.nPage = blockwitdh * 0.628;
	m_scrollbar_ver->SetScrollInfo(&si);

	m_scrollbar_hor->SetScrollPos(Apicontroller->offsetX);
	m_scrollbar_ver->SetScrollPos(Apicontroller->offsetY);

	if (Apicontroller->scale >= 1.0)
	{
		Apicontroller->offsetX = 50;
		Apicontroller->offsetY = 50;
		m_scrollbar_hor->SetScrollPos(Apicontroller->offsetX);
		m_scrollbar_ver->SetScrollPos(Apicontroller->offsetY);
	}

	if (Apicontroller->scale < 0.10)
		Apicontroller->scale = 0.10;
	if (Apicontroller->scale > 1.0)
		Apicontroller->scale = 1.0;

	if (bMonitor == true)
	{
		GetDlgItem(IDC_COMBO_Exposure_Unit)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_Exposure_Unit)->EnableWindow(TRUE);
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDASDlg::MouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (bScaleControl)
		if (CDASDlg::GetInstance()->bMonitor)
		{
			if (zDelta == -120 && Apicontroller->scale < 1.0)
			{
				Apicontroller->scale += 0.05;
			}
			if (zDelta == +120 && Apicontroller->scale > 0.1)
			{
				Apicontroller->scale -= 0.05;
			}
		}
	//if (Apicontroller->scale < 1.0)
	//{
	//	CRect dlgrect;
	//	mCWnd->GetWindowRect(&dlgrect);
	//	CRect rect_hor = CRect(PictureCRect.left, PictureCRect.top - dlgrect.top, PictureCRect.right, PictureCRect.bottom + 22);
	//	CRect rect_ver = CRect(PictureCRect.left, PictureCRect.top, PictureCRect.right + 22, PictureCRect.bottom);
	//	if (m_scrollbar_hor->m_hWnd == NULL)
	//		m_scrollbar_hor->Create(WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_RIGHTALIGN, rect_hor, mCWnd, IDC_SCROLLBAR_HOR);
	//	if (m_scrollbar_ver->m_hWnd == NULL)
	//		m_scrollbar_ver->Create(WS_CHILD | WS_VISIBLE | SBS_VERT | SBS_BOTTOMALIGN, rect_ver, mCWnd, IDC_SCROLLBAR_VER);
	//	int blockwitdh = 50 * Apicontroller->scale;
	//	int hSB_MAX = 100 + blockwitdh;
	//	int vSB_MAX = 100 + blockwitdh*0.628;
	//	m_scrollbar_hor->SetScrollRange(0, hSB_MAX);
	//	m_scrollbar_ver->SetScrollRange(0, vSB_MAX);
	//	SCROLLINFO si;
	//	m_scrollbar_hor->GetScrollInfo(&si);
	//	si.nPage = blockwitdh;
	//	m_scrollbar_hor->SetScrollInfo(&si);
	//	m_scrollbar_ver->GetScrollInfo(&si);
	//	si.nPage = blockwitdh * 0.628;
	//	m_scrollbar_ver->SetScrollInfo(&si);
	//	m_scrollbar_hor->SetScrollPos(Apicontroller->offsetX);
	//	m_scrollbar_ver->SetScrollPos(Apicontroller->offsetY);
	//}
	//if (Apicontroller->scale >= 1.0)
	//{
	//	if (m_scrollbar_hor->m_hWnd != NULL)
	//		m_scrollbar_hor->DestroyWindow();
	//	if (m_scrollbar_ver->m_hWnd != NULL)
	//		m_scrollbar_ver->DestroyWindow();
	//	Apicontroller->offsetX = 50;
	//	Apicontroller->offsetY = 50;
	//	m_scrollbar_hor->SetScrollPos(Apicontroller->offsetX);
	//	m_scrollbar_ver->SetScrollPos(Apicontroller->offsetY);
	//}
}

// 获取工作路径
CString GetWorkingDir()
{
	CString pFileName;
	int nPos = GetCurrentDirectory(MAX_PATH, pFileName.GetBuffer(MAX_PATH));
	pFileName.ReleaseBuffer();
	CString csFullPath(pFileName);
	if (nPos < 0)
		return CString(_T(""));
	else
		return csFullPath;
}

// 改变控件大小
void CDASDlg::ChangeCtrlSize(CRect &rect)
{
	if (mCWnd == NULL)
		return;
	if (mCWnd->m_hWnd == NULL)
		return;
	CRect rc;
	CRect rc2;
	CWnd *pWnd;
	CWnd *pWnd2;
	int width = rect.Width();
	int height = rect.Height();
	int pHeight = 0.95*height;
	int pWidth = pHeight * 1.5921;
	int left = 0.5*(width - pWidth);
	int top = 0.01*height;

	pWnd = mCWnd->GetDlgItem(IDC_STATIC_Picture);

	left = 0.02*width;
	pWnd = mCWnd->GetDlgItem(IDC_STATIC_Picture);
	PictureCRect = CRect(left, top, left + pWidth, top + pHeight);
	pWnd->MoveWindow(rc);

	pWnd = mCWnd->GetDlgItem(IDC_STATIC_GExposure);
	pWnd2 = mCWnd->GetDlgItem(IDC_STATIC_GGain);
	left = 0.04*width + pWidth;
	top = 0.02*height;
	pWidth = 0.98*width - left;
	pHeight = 150;
	int top2 = 0.05*height + top + 150;
	rc = CRect(left, top, left + pWidth, top + pHeight);
	rc2 = CRect(left, top2, left + pWidth, top2 + pHeight);
	pWnd->MoveWindow(rc);
	pWnd2->MoveWindow(rc2);

	pWnd = mCWnd->GetDlgItem(IDC_SLIDER_Exposure);
	pWnd2 = mCWnd->GetDlgItem(IDC_SLIDER_Gain);
	left = 0.02*width + left;
	top = 0.65*pHeight;
	pWidth = 0.5*pWidth;
	pHeight = 30;
	top2 = 0.05*height + top + 150;
	rc = CRect(left, top, left + pWidth, top + pHeight);
	rc2 = CRect(left, top2, left + pWidth, top2 + pHeight);
	pWnd->MoveWindow(rc);
	pWnd2->MoveWindow(rc2);

	pWnd = mCWnd->GetDlgItem(IDC_EDIT_Camera_Exposure);
	pWnd2 = mCWnd->GetDlgItem(IDC_EDIT_Camera_Gain);
	left = 0.01*width + left + pWidth;
	pWidth = 0.08*width;
	pHeight = 27;
	top2 = 0.05*height + top + 150;
	rc = CRect(left, top, left + pWidth, top + pHeight);
	rc2 = CRect(left, top2, left + pWidth, top2 + pHeight);
	pWnd->MoveWindow(rc);
	pWnd2->MoveWindow(rc2);

	pWnd = mCWnd->GetDlgItem(IDC_COMBO_Exposure_Unit);
	pWnd2 = mCWnd->GetDlgItem(IDC_STATIC_Camera_Gain_Unit);
	left = 0.01*width + left + pWidth;
	pWidth = 50;
	pHeight = 18;
	top2 = 0.05*height + top + 155;
	rc = CRect(left, top, left + pWidth, top + pHeight);
	rc2 = CRect(left, top2, left + pWidth, top2 + pHeight);
	pWnd->MoveWindow(rc);
	pWnd2->MoveWindow(rc2);

	CScrollBar* m_sb_h = (CScrollBar*)mCWnd->GetDlgItem(IDC_SCROLLBAR_HOR);
	CScrollBar* m_sb_v = (CScrollBar*)mCWnd->GetDlgItem(IDC_SCROLLBAR_VER);

	if (m_sb_h != NULL && m_sb_v != NULL)
	{
		if (m_sb_h->m_hWnd != NULL)
			m_sb_h->DestroyWindow();
		if (m_sb_v->m_hWnd != NULL)
			m_sb_v->DestroyWindow();
	}

	mCWnd->RedrawWindow();
	mCWnd->UpdateData(FALSE);
}

void CDASDlg::BrowseCurrentAllFileSingle(CString strDir)
{
	if (strDir == _T(""))
	{
		return;
	}
	else
	{
		if (strDir.Right(1) != _T("\\"))
			strDir += L"\\";
		strDir = strDir + _T("*.*");
	}

	CFileFind finder;
	CString strPath;
	BOOL bWorking = finder.FindFile(strDir);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		strPath = finder.GetFilePath();
		//strPath就是所要获取Test目录下的文件夹和文件（包括路径）
		CString tPath = BgPath;
		//tPath.Delete(BgPath.GetLength() - 1);
		if (tPath == strPath)
		{
			Bgname_count++;
			Bgfilename.Format(_T("%s%s%.2d"), _T("BG_"), CurrentDate, Bgname_count);
			BgPath.Format(_T("%s%s%s"), SavePath, Bgfilename, _T(".bin"));
		}
	}
}
void CDASDlg::BrowseCurrentAllFileMulti(CString strDir)
{
	if (strDir == _T(""))
	{
		return;
	}
	else
	{
		if (strDir.Right(1) != _T("\\"))
			strDir += L"\\";
		strDir = strDir + _T("*.*");
	}

	CFileFind finder;
	CString strPath;
	BOOL bWorking = finder.FindFile(strDir);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		strPath = finder.GetFilePath();
		//strPath就是所要获取Test目录下的文件夹和文件（包括路径）
		CString tPath = path;
		tPath.Delete(path.GetLength() - 1);
		if (tPath == strPath)
		{
			filename_count++;
			foldername.Format(_T("%s%.2d"), CurrentDate, filename_count);
			path.Format(_T("%s%s%s"), SavePath, foldername, _T("\\"));
		}
	}
}

// Ini配置文件操作
void CDASDlg::OnIni()
{
	// 获取当前路径(单斜杠）
	TCHAR FilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, FilePath, MAX_PATH);
	(_tcsrchr(FilePath, _T('\\')))[1] = 0;//删除文件名，只获得路径字串
	IniPath = FilePath;
	XmlFile = IniPath + _T("default_settings.xml");
	IniPath = IniPath + _T("UserSettings.ini");

	// 如果文件不存在就创建
	int IsExist;
	_wsopen_s(&IsExist, IniPath, _O_APPEND, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (IsExist == -1)
	{
		WritePrivateProfileString(_T("Camera"), _T("Exposure"), _T("10000"), IniPath);
		WritePrivateProfileString(_T("Camera"), _T("Gain"), _T("0"), IniPath);
		WritePrivateProfileString(_T("Data"), _T("Binning_horizontal"), _T("1"), IniPath);
		WritePrivateProfileString(_T("Data"), _T("Binning_vertical"), _T("1"), IniPath);
		WritePrivateProfileString(_T("Data"), _T("OffsetX"), _T("0"), IniPath);
		WritePrivateProfileString(_T("Data"), _T("OffsetY"), _T("0"), IniPath);
		WritePrivateProfileString(_T("Data"), _T("Width"), _T("1388"), IniPath);
		WritePrivateProfileString(_T("Data"), _T("Height"), _T("1038"), IniPath);
	}
	else
	{
		_close(IsExist);
	}
	// 读取值
	exposure = GetPrivateProfileInt(_T("Camera"), _T("Exposure"), 10000, IniPath);
	gain = GetPrivateProfileInt(_T("Camera"), _T("Gain"), 0, IniPath);
	Apicontroller->BinningHor = GetPrivateProfileInt(_T("Data"), _T("Binning_horizontal"), 1, IniPath);
	Apicontroller->BinningVer = GetPrivateProfileInt(_T("Data"), _T("Binning_vertical"), 1, IniPath);
	/*Apicontroller->OffsetX = GetPrivateProfileInt(_T("Data"), _T("OffsetX"), 0, IniPath);
	Apicontroller->OffsetY = GetPrivateProfileInt(_T("Data"), _T("OffsetY"), 0, IniPath);
	Apicontroller->Width = GetPrivateProfileInt(_T("Data"), _T("Width"), 1388, IniPath);
	Apicontroller->Height = GetPrivateProfileInt(_T("Data"), _T("Height"), 1038, IniPath);*/
	CString workingdir;
	workingdir = GetWorkingDir();
	SavePath.Format(_T("%s%s"), workingdir, _T("\\Data\\"));
	if (!PathIsDirectory(SavePath))
		CreateDirectory(SavePath, NULL);
	UpdateData(FALSE);
}

// 滚动条消息响应
void CDASDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == GetDlgItem(IDC_SCROLLBAR_HOR))
	{
		int offsetX = Apicontroller->offsetX;
		switch (nSBCode)
		{
		case SB_LINELEFT:	offsetX -= 1;	break;
		case SB_LINERIGHT:	offsetX += 1;	break;
		case SB_PAGELEFT:	offsetX -= 10;	break;
		case SB_PAGERIGHT:	offsetX += 10;	break;
		case SB_LEFT:		offsetX = -50;	break;
		case SB_RIGHT:		offsetX = 50;	break;
		case SB_THUMBPOSITION:	offsetX = nPos;	break;
		case SB_THUMBTRACK:		offsetX = nPos; break;
		default:	break;
		}
		m_scrollbar_hor->SetScrollPos(offsetX);
		if (offsetX > 100)	offsetX = 100;
		if (offsetX < 0)	offsetX = 0;
		Apicontroller->offsetX = offsetX;
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
void CDASDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == GetDlgItem(IDC_SCROLLBAR_VER))
	{
		int offsetY = Apicontroller->offsetY;
		switch (nSBCode)
		{
		case SB_LINELEFT:	offsetY -= 1;	break;
		case SB_LINERIGHT:	offsetY += 1;	break;
		case SB_PAGELEFT:	offsetY -= 10;	break;
		case SB_PAGERIGHT:	offsetY += 10;	break;
		case SB_LEFT:		offsetY = -50;	break;
		case SB_RIGHT:		offsetY = 50;	break;
		case SB_THUMBPOSITION:	offsetY = nPos;	break;
		case SB_THUMBTRACK:		offsetY = nPos; break;
		default:	break;
		}
		m_scrollbar_ver->SetScrollPos(offsetY);
		if (offsetY > 100)	offsetY = 100;
		if (offsetY < 0)	offsetY = 0;
		Apicontroller->offsetY = offsetY;
	}
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

// 鼠标消息响应
void CDASDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	bLButtonClick = true;
	m_PreviousPoint = point;
	CDialogEx::OnLButtonDown(nFlags, point);
}
void CDASDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	bLButtonClick = false;
	CDialogEx::OnLButtonUp(nFlags, point);
}
void CDASDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// 判断鼠标位置
	CPoint pt;
	CRect rc = PictureCRect;
	ClientToScreen(&rc);
	GetCursorPos(&pt);
	if (rc.PtInRect(pt))
	{
		if (Apicontroller->scale <= 1.0)
			bScaleControl = true;
	}
	else
		bScaleControl = false;
	if (bLButtonClick)
		if (bScaleControl)
			if (Apicontroller->scale < 1.0)
			{
				int tempX = Apicontroller->offsetX;
				int tempY = Apicontroller->offsetY;
				CSize m_roll = point - m_PreviousPoint;
				tempX -= m_roll.cx*0.4;
				tempY -= m_roll.cy*0.4;
				m_scrollbar_hor->SetScrollPos(tempX);
				m_scrollbar_ver->SetScrollPos(tempY);
				if (tempX > 100)	tempX = 100;
				if (tempX < 0)		tempX = 0;
				if (tempY > 100)	tempY = 100;
				if (tempY < 0)		tempY = 0;
				Apicontroller->offsetX = tempX;
				Apicontroller->offsetY = tempY;
				m_PreviousPoint = point;
			}

	CDialogEx::OnMouseMove(nFlags, point);
}

// 实时图像消息响应
afx_msg LRESULT CDASDlg::OnFrameReady(WPARAM status, LPARAM lParam)
{
	CWnd* pWnd = mCWnd->GetDlgItem(IDC_STATIC_Picture);
	Apicontroller->ImageAcuqisition(status);
	Count++;
	s2 = GetTickCount();
	fps = 1000 / (s2 - s1);
	s1 = s2;
	return 0;
}

void CDASDlg::OnBnClickedRadioSingle()
{
	AcquisitionStatus = SingleShot;
	Bgfilename.Format(_T("%s%s%.2d"), _T("BG_"), CurrentDate, Bgname_count);
	BgPath.Format(_T("%s%s%s"), SavePath, Bgfilename, _T(".bin"));
	BrowseCurrentAllFileSingle(SavePath);
	GetDlgItem(IDC_EDIT_Camera_Filename)->SetWindowTextW(Bgfilename);
	GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Camera_Interval)->EnableWindow(FALSE);
}
void CDASDlg::OnBnClickedRadioMulti()
{
	AcquisitionStatus = MultiShot;
	// 设置数据保存路径
	foldername.Format(_T("%s%.2d"), CurrentDate, filename_count);
	path.Format(_T("%s%s%s"), SavePath, foldername, _T("\\"));
	BrowseCurrentAllFileMulti(SavePath);
	GetDlgItem(IDC_EDIT_Camera_Filename)->SetWindowTextW(foldername);
	GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_Camera_Interval)->EnableWindow(TRUE);
}
void CDASDlg::OnBnClickedRadioAfBgshot()
{
	AcquisitionStatus = AFBgShot;
	// 设置数据保存路径
	Bgfilename.Format(_T("BK"));
	BgPath.Format(_T("%s%s%s"), SavePath, Bgfilename, _T(".bin"));
	BrowseCurrentAllFileMulti(SavePath);
	GetDlgItem(IDC_EDIT_Camera_Filename)->SetWindowTextW(Bgfilename);
	GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Camera_Interval)->EnableWindow(FALSE);
}

void CDASDlg::OnOK()
{
	// do nothing
}