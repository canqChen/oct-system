#pragma once
#ifndef __SHOWDLG_H__
#define __SHOWDLG_H__
#include "Mymatrix.cpp"
#include "afxmenubutton.h"
#include "resource.h"
#include "DASDlg.h"

// CShowDlg 对话框

class CShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowDlg)

public:
	CShowDlg(CWnd* pParent = NULL);   // 标准构造函数
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
	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_Analysis_Result };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	static enum STATUS { NONE, SGLCOLOR, SGLGRAY, MLTUNCOLOR, MLTUNGRAY, MLTCOLOR, MLTGRAY };
	STATUS status;

	void loadGrayImg(Mymatrix<double>* &Ang, Mymatrix<double>* &Am, int wrapFlag = 1, int flag = 1);
	void loadGrayBar();
	//将数据映射成24位真彩图片
	void loadColorImg(Mymatrix<double>* &Ang, Mymatrix<double>* &Am, int wrapFlag = 1, int flag = 1);
	void reset(bool  is_unwrap);
	void sglreset();
	void setHeader();


	//加载colorbar
	void loadColorBar();

	//设置x轴名称
	void set_X_axis_Name(CString& name);
	//设置y轴名称
	void set_Y_axis_Name(CString& name);
	//设置图片标题
	void setTitle(CString& title);
	void setColorbarrange(double min, double max);
	//设置 两数轴范围
	void set_X_Y_range(double xmin, double xmax, double ymin, double ymax);
	void PaintImageCoordinate(CMemDC &dc);

	void showPhaseColor(int i);
	void showPhaseGray(int i);
	void showUnPhaseColor(int i);
	void showUnPhaseGray(int i);
	void showSglColor();
	void showSglGray();
	void saveImg(CString picformat);

	//初始化showdlg中的数据矩阵，传入矩阵，要赋值的起始行列，要赋值的行列数，并且设置数据中的最大最小值
	//void setData(Mymatrix<double>&mat, size_t startrowindex, size_t startcolindex, size_t row, size_t col);
private:
	static CShowDlg* _showDlg;	//单例
	const int kImagefreshrate = 100;
	CRect destrc;

	//保存gif图片每一帧
	CImage Colorimg[500];
	CImage Grayimg[500];

	CImage m_PhaseColorImg1;	//显示图片对象
	CImage m_PhaseColorImg2;	//显示图片对象
	CImage m_PhaseColorImg3;	//显示图片对象

	CImage m_PhaseGrayImg1;
	CImage m_PhaseGrayImg2;
	CImage m_PhaseGrayImg3;

	CImage m_unPhaseColorImg1;	//显示图片对象
	CImage m_unPhaseColorImg2;	//显示图片对象
	CImage m_unPhaseColorImg3;	//显示图片对象

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

	CImage m_colorBar;	//颜色条对象
	CImage m_grayBar;

	BYTE *p_ColorBar;
	BYTE *p_ColorImg;	//指向图片像素矩阵
	BYTE *p_GrayBar;
	BYTE *p_GrayImg;

	RECT imgRect1;   //图片大小矩形
	RECT imgRect2;   //图片大小矩形
	RECT imgRect3;   //图片大小矩形
	RECT colorBarRect;//colorbar矩形大小
	RECT clientRect;  //客户区大小

	RECT exhibitRect1;
	RECT exhibitRect2;
	RECT exhibitRect3;

	CWnd *m_pXY;//显示该点信息文本框
	RECT xyWndRect;

	struct {
		int borderX1;//与左边框距离
		int borderX2;//右
		int borderY1;//上
		int borderY2;//下
		int gap;  //colorbar和图片距离
		int barWidth;//颜色条宽度
		int geap;  //colorbar顶与图片顶距离
	}clientSizeInfo;//可显示区边界与窗口的距离大小参数

	struct {
		double colorbarMax;	//colorbar最大标值
		double colorbarMin;	//colorbar最小标值
		CString  nameX;//图片x坐标名
		CString  nameY;	//图片y坐标名
		CString  title;	//标题
		double maxX;	//图片x坐标（图片像素宽）最大值
		double minX;	//图片x坐标（图片像素宽）最小值
		double maxY;	//图片y坐标最大值
		double minY;		//图片y坐标最小值
	}dataInfo;		//图片显示窗口参数
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