#ifndef __DASDlg__
#define __DASDlg__

#include "Vimba.h"
#include "Camera.h"
#include "FrameObserver.h"
//#include "SamplingParam.h"
#include "SamplingProg.h"
#include "Myresource.h"
#include "resource.h"

// CDASDlg 对话框

class CDASDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDASDlg)

public:
	CDASDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDASDlg();
	static CDASDlg*	GetInstance()
	{
		if (pInstance == NULL)
			pInstance = new CDASDlg();
		return pInstance;
	}
	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DataAcquisition };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnFrameReady(WPARAM status, LPARAM lParam);

private:
	static CDASDlg*	pInstance;					// 单例
	CTime			ct;
	CSliderCtrl		Slider_Exposure;			// 曝光滑块
	CSliderCtrl		Slider_Gain;				// 增益滑块
	CFont			*font;						// 字体
	CWnd*			PictureCWnd;				// 图像控件句柄
	CEdit			EditExposure;				// 曝光控件
	CEdit			EditGain;					// 增益控件 
	CComboBox		ComboExposureUnit;			// 曝光时间单位转换
	CString			Bgfilename;					// 背景文件名
	CString			BgPath;						// 背景路径
	CString			path;						// 保存路径(文件夹)
	CString			CurrentDate;				// 当前日期
	CString			cSamplingNum;				// 采样数量
	CString			foldername;					// 文件夹名称
	int				Bgname_count = 1;			// 背景命名计数
	int				filename_count = 1;			// 文件夹命名计数
	int				SamplingNumber;				// 采样数量
	int				interval;					// 采样间隔
	void			OnIni();					// ini文件操作
	void			BrowseCurrentAllFileSingle(CString strDir);	//遍历文件夹
	void			BrowseCurrentAllFileMulti(CString strDir);	//遍历文件夹

	CScrollBar		*m_scrollbar_hor;
	CScrollBar		*m_scrollbar_ver;

	afx_msg void	OnNMCustomdrawSliderExposure(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnNMCustomdrawSliderGain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnEnChangeEditCameraExposure();
	afx_msg void	OnEnChangeEditCameraGain();
	afx_msg void	OnCbnSelchangeComboExposureUnit();
	BOOL			PreTranslateMessage(MSG * pMsg);

public:
	virtual BOOL	OnInitDialog();
	afx_msg void	OnPaint();
	afx_msg void	OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void	OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnBnClickedButtonMonitor();
	afx_msg void	OnBnClickedButtonAcquiring();
	afx_msg void	OnBnClickedRadioSingle();
	afx_msg void	OnBnClickedRadioMulti();
	void			OnChangeComboExposureUnit();
	int				Target_Width = 0;
	int				Target_Height = 0;
	void			OnBnClickedButtonBgshot();
	void			OnAFBgshot();
	void			ChangeCtrlSize(CRect &rect);
	void			MouseWheel(UINT nFlags, short zDelta, CPoint pt);
	enum			status { SingleShot, MultiShot, AFBgShot };
	status			AcquisitionStatus;
	enum			unit { MS, US };
	bool			bMonitor;
	bool			bAcquiring;

private:
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (CDASDlg::pInstance)
				delete CDASDlg::pInstance;
		}
	};
	static CGarbo Garbo;

	virtual void OnOK();
public:
	afx_msg void OnBnClickedRadioAfBgshot();
};

#endif // !__DASDlg__
