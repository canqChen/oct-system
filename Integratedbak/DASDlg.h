#ifndef __DASDlg__
#define __DASDlg__

#include "Vimba.h"
#include "Camera.h"
#include "FrameObserver.h"
//#include "SamplingParam.h"
#include "SamplingProg.h"
#include "Myresource.h"
#include "resource.h"

// CDASDlg �Ի���

class CDASDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDASDlg)

public:
	CDASDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDASDlg();
	static CDASDlg*	GetInstance()
	{
		if (pInstance == NULL)
			pInstance = new CDASDlg();
		return pInstance;
	}
	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DataAcquisition };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnFrameReady(WPARAM status, LPARAM lParam);

private:
	static CDASDlg*	pInstance;					// ����
	CTime			ct;
	CSliderCtrl		Slider_Exposure;			// �ع⻬��
	CSliderCtrl		Slider_Gain;				// ���滬��
	CFont			*font;						// ����
	CWnd*			PictureCWnd;				// ͼ��ؼ����
	CEdit			EditExposure;				// �ع�ؼ�
	CEdit			EditGain;					// ����ؼ� 
	CComboBox		ComboExposureUnit;			// �ع�ʱ�䵥λת��
	CString			Bgfilename;					// �����ļ���
	CString			BgPath;						// ����·��
	CString			path;						// ����·��(�ļ���)
	CString			CurrentDate;				// ��ǰ����
	CString			cSamplingNum;				// ��������
	CString			foldername;					// �ļ�������
	int				Bgname_count = 1;			// ������������
	int				filename_count = 1;			// �ļ�����������
	int				SamplingNumber;				// ��������
	int				interval;					// �������
	void			OnIni();					// ini�ļ�����
	void			BrowseCurrentAllFileSingle(CString strDir);	//�����ļ���
	void			BrowseCurrentAllFileMulti(CString strDir);	//�����ļ���

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
