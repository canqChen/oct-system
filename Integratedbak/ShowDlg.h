#pragma once
#ifndef __SHOWDLG_H__
#define __SHOWDLG_H__
#include "Mymatrix.cpp"
#include "afxmenubutton.h"
#include "resource.h"
#include "DASDlg.h"

// CShowDlg �Ի���

class CShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowDlg)

public:
	CShowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowDlg();
	static CShowDlg* getInstance()
	{
		if (_showDlg == NULL)
		{
			_showDlg = new CShowDlg();
			CWnd *wnd = AfxGetMainWnd();
			_showDlg->Create(IDD_DIALOG_Analysis_Result, wnd);
			return _showDlg;
		}
		return _showDlg;
	}

	static void exitInstace()
	{
		delete _showDlg;
		_showDlg = NULL;
	}
	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_Analysis_Result };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	static enum STATUS { NONE, SGLCOLOR, SGLGRAY, MLTUNCOLOR, MLTUNGRAY, MLTCOLOR, MLTGRAY };
	STATUS status;

	void loadGrayImg(Mymatrix<double>* &Ang, Mymatrix<double>* &Am, int wrapFlag = 1, int flag = 1);
	void loadGrayBar();
	//������ӳ���24λ���ͼƬ
	void loadColorImg(Mymatrix<double>* &Ang, Mymatrix<double>* &Am, int wrapFlag = 1, int flag = 1);
	void reset(bool  is_unwrap);
	void sglreset();
	void setHeader();


	//����colorbar
	void loadColorBar();

	//����x������
	void set_X_axis_Name(CString& name);
	//����y������
	void set_Y_axis_Name(CString& name);
	//����ͼƬ����
	void setTitle(CString& title);
	void setColorbarrange(double min, double max);
	//���� �����᷶Χ
	void set_X_Y_range(double xmin, double xmax, double ymin, double ymax);
	void PaintImageCoordinate(CMemDC &dc);

	void showPhaseColor(int i);
	void showPhaseGray(int i);
	void showUnPhaseColor(int i);
	void showUnPhaseGray(int i);
	void showSglColor();
	void showSglGray();
	void saveImg(CString picformat);

	//��ʼ��showdlg�е����ݾ��󣬴������Ҫ��ֵ����ʼ���У�Ҫ��ֵ�����������������������е������Сֵ
	//void setData(Mymatrix<double>&mat, size_t startrowindex, size_t startcolindex, size_t row, size_t col);
private:
	static CShowDlg* _showDlg;	//����
	const int kImagefreshrate = 100;
	CRect destrc;

	//����gifͼƬÿһ֡
	CImage Colorimg[500];
	CImage Grayimg[500];

	CImage m_PhaseColorImg1;	//��ʾͼƬ����
	CImage m_PhaseColorImg2;	//��ʾͼƬ����
	CImage m_PhaseColorImg3;	//��ʾͼƬ����

	CImage m_PhaseGrayImg1;
	CImage m_PhaseGrayImg2;
	CImage m_PhaseGrayImg3;

	CImage m_unPhaseColorImg1;	//��ʾͼƬ����
	CImage m_unPhaseColorImg2;	//��ʾͼƬ����
	CImage m_unPhaseColorImg3;	//��ʾͼƬ����

	CImage m_unPhaseGrayImg1;
	CImage m_unPhaseGrayImg2;
	CImage m_unPhaseGrayImg3;

	BYTE* PhaseColorImg1[500];
	BYTE* PhaseColorImg2[500];
	BYTE* PhaseColorImg3[500];

	BYTE* PhaseGrayImg1[500];
	BYTE* PhaseGrayImg2[500];
	BYTE* PhaseGrayImg3[500];

	BYTE* unPhaseColorImg1[500];
	BYTE* unPhaseColorImg2[500];
	BYTE* unPhaseColorImg3[500];

	BYTE* unPhaseGrayImg1[500];
	BYTE* unPhaseGrayImg2[500];
	BYTE* unPhaseGrayImg3[500];

	CImage m_colorBar;	//��ɫ������
	CImage m_grayBar;

	BYTE *p_ColorBar;
	BYTE *p_ColorImg;	//ָ��ͼƬ���ؾ���
	BYTE *p_GrayBar;
	BYTE *p_GrayImg;

	RECT imgRect1;   //ͼƬ��С����
	RECT imgRect2;   //ͼƬ��С����
	RECT imgRect3;   //ͼƬ��С����
	RECT colorBarRect;//colorbar���δ�С
	RECT clientRect;  //�ͻ�����С

	RECT exhibitRect1;
	RECT exhibitRect2;
	RECT exhibitRect3;

	CWnd *m_pXY;//��ʾ�õ���Ϣ�ı���
	RECT xyWndRect;

	struct {
		int borderX1;//����߿����
		int borderX2;//��
		int borderY1;//��
		int borderY2;//��
		int gap;  //colorbar��ͼƬ����
		int barWidth;//��ɫ�����
		int geap;  //colorbar����ͼƬ������
	}clientSizeInfo;//����ʾ���߽��봰�ڵľ����С����

	struct {
		double colorbarMax;	//colorbar����ֵ
		double colorbarMin;	//colorbar��С��ֵ
		CString  nameX;//ͼƬx������
		CString  nameY;	//ͼƬy������
		CString  title;	//����
		double maxX;	//ͼƬx���꣨ͼƬ���ؿ����ֵ
		double minX;	//ͼƬx���꣨ͼƬ���ؿ���Сֵ
		double maxY;	//ͼƬy�������ֵ
		double minY;		//ͼƬy������Сֵ
	}dataInfo;		//ͼƬ��ʾ���ڲ���
	struct {
		BITMAPINFOHEADER  bmiHeader;
		RGBQUAD			  bmiColors[256];
	}GrayBitmapInfo1;

	struct {
		BITMAPINFOHEADER  bmiHeader;
		RGBQUAD			  bmiColors[256];
	}GrayBitmapInfo2;
	
	BITMAPINFOHEADER Colorbmphead1;
	BITMAPINFOHEADER Colorbmphead2;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	bool bSglMenuShow;
	bool bMultiUnwrapMenuShow;
	bool bMultiWrapMenuShow;
	bool bSaveSgl;
	bool bSaveMulti;
	void MenuCheck();
	void PlayOnce();
	void UpdateRect();
	void showMultiUnwrapColor();
	void showMultiUnwrapGray();
	void showMultiWrapColor();
	void showMultiWrapGray();

private:
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (CShowDlg::_showDlg)
				delete CShowDlg::_showDlg;
		}
	};
	static CGarbo Garbo;
};

#endif