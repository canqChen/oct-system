// CShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Integratedbak.h"
#include "ShowDlg.h"
#include "SingleDlg.h"
#include "afxdialogex.h"
#include <vector>
#include "GifEncoder.h"
#include "MyAlgorithm.h"
#include"resource.h"


#pragma warning (once:4018)
#pragma warning (once:4244)

using namespace std;
extern SINGLEDATA sglData;
extern MULTIDATA multiData;
extern AREA area;
extern CString folder_name;
extern CString folder_dir;

vector<BYTE*>PhaseColorVec1;
vector<BYTE*>PhaseColorVec2;
vector<BYTE*>PhaseColorVec3;
vector<BYTE*>unPhaseColorVec1;
vector<BYTE*>unPhaseColorVec2;
vector<BYTE*>unPhaseColorVec3;

vector<BYTE*>PhaseGrayVec1;
vector<BYTE*>PhaseGrayVec2;
vector<BYTE*>PhaseGrayVec3;

vector<BYTE*>unPhaseGrayVec1;
vector<BYTE*>unPhaseGrayVec2;
vector<BYTE*>unPhaseGrayVec3;

CImage singleColorImg1;	//显示图片对象
CImage singleGrayImg1;
CImage singleColorImg2;	//显示图片对象
CImage singleGrayImg2;
CImage singleColorImg3;	//显示图片对象
CImage singleGrayImg3;

int PhaseGrayImgNum1 = 0;
int PhaseColorImgNum1 = 0;
int PhaseGrayImgNum2 = 0;
int PhaseColorImgNum2 = 0;
int PhaseGrayImgNum3 = 0;
int PhaseColorImgNum3 = 0;

int unPhaseGrayImgNum1 = 0;
int unPhaseColorImgNum1 = 0;
int unPhaseGrayImgNum2 = 0;
int unPhaseColorImgNum2 = 0;
int unPhaseGrayImgNum3 = 0;
int unPhaseColorImgNum3 = 0;CString work_dir = GetWorkingDir();

CShowDlg* CShowDlg::_showDlg = NULL;
CShowDlg::CGarbo CShowDlg::Garbo;

// CShowDlg 对话框

IMPLEMENT_DYNAMIC(CShowDlg, CDialogEx)

CShowDlg::CShowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_Analysis_Result, pParent)
{
	clientSizeInfo.borderX1 = 70;
	clientSizeInfo.borderX2 = 120;
	clientSizeInfo.borderY1 = 90;
	clientSizeInfo.borderY2 = 110;
	clientSizeInfo.barWidth = 80;
	clientSizeInfo.gap = 45;
	clientSizeInfo.geap = 120;
	double maxx = 2.0;
	double maxy = 4.2;

	set_X_axis_Name(CString(_T("Λ-axis[mm]")));
	set_Y_axis_Name(CString(_T("Y-axis[mm]")));
	set_X_Y_range(0, maxx, 0, maxy);
}

CShowDlg::~CShowDlg()
{

}

void CShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CShowDlg 消息处理程序


BOOL CShowDlg::OnInitDialog()
{
	SetBackgroundColor(RGB(255, 255, 255));
	CDialogEx::OnInitDialog();

	GetClientRect(&clientRect);
	status = NONE;

	bSglMenuShow = false;
	bMultiUnwrapMenuShow = false;
	bMultiWrapMenuShow = false;

	/*m_pXY = GetDlgItem(IDC_SHOW_XY);
	m_pXY->GetWindowRect(&xyWndRect);*/

	////图片展示区边界坐标
	//exhibitRect1 = { clientSizeInfo.borderX1,
	//	clientSizeInfo.borderY1,
	//	clientSizeInfo.borderX1 + (clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap * 3 - clientSizeInfo.borderX1) / 2,
	//	clientRect.bottom - clientSizeInfo.borderY2 };
	//exhibitRect2 = { clientSizeInfo.borderX1 + (clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap * 3 - clientSizeInfo.borderX1) / 2 + clientSizeInfo.gap,
	//	clientSizeInfo.borderY1,
	//	clientSizeInfo.borderX1 + (clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap * 3 - clientSizeInfo.borderX1) * 3 / 4 + clientSizeInfo.gap,
	//	clientRect.bottom - clientSizeInfo.borderY2 };
	//exhibitRect3 = { clientSizeInfo.borderX1 + (clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap * 3 - clientSizeInfo.borderX1) * 3 / 4 + clientSizeInfo.gap * 2,
	//	clientSizeInfo.borderY1,
	//	clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap ,
	//	clientRect.bottom - clientSizeInfo.borderY2 };

	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CShowDlg::setHeader()
{
	int GraybmpWidthBytes1 = ((area.signalWidth() * 8 + 31) & ~31) / 8;
	int ColorbmpWidthBytes1 = ((area.signalWidth() * 24 + 31) & ~31) / 8;

	int GraybmpWidthBytes2 = ((area.signalWidth() / 2 * 8 + 31) & ~31) / 8;
	int ColorbmpWidthBytes2 = ((area.signalWidth() / 2 * 24 + 31) & ~31) / 8;

	GrayBitmapInfo1.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GrayBitmapInfo1.bmiHeader.biWidth = area.signalWidth();
	GrayBitmapInfo1.bmiHeader.biHeight = -area.signalHeight();
	GrayBitmapInfo1.bmiHeader.biPlanes = 1;
	GrayBitmapInfo1.bmiHeader.biBitCount = 8;
	GrayBitmapInfo1.bmiHeader.biCompression = BI_RGB;
	GrayBitmapInfo1.bmiHeader.biSizeImage = GraybmpWidthBytes1*area.signalHeight();
	GrayBitmapInfo1.bmiHeader.biXPelsPerMeter = 0;
	GrayBitmapInfo1.bmiHeader.biYPelsPerMeter = 0;
	GrayBitmapInfo1.bmiHeader.biClrUsed = 0;
	GrayBitmapInfo1.bmiHeader.biClrImportant = 0;

	GrayBitmapInfo2.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GrayBitmapInfo2.bmiHeader.biWidth = area.signalWidth() / 2;
	GrayBitmapInfo2.bmiHeader.biHeight = -area.signalHeight();
	GrayBitmapInfo2.bmiHeader.biPlanes = 1;
	GrayBitmapInfo2.bmiHeader.biBitCount = 8;
	GrayBitmapInfo2.bmiHeader.biCompression = BI_RGB;
	GrayBitmapInfo2.bmiHeader.biSizeImage = GraybmpWidthBytes2*area.signalHeight();
	GrayBitmapInfo2.bmiHeader.biXPelsPerMeter = 0;
	GrayBitmapInfo2.bmiHeader.biYPelsPerMeter = 0;
	GrayBitmapInfo2.bmiHeader.biClrUsed = 0;
	GrayBitmapInfo2.bmiHeader.biClrImportant = 0;

	for (int i = 0; i < 256; i++)
	{
		GrayBitmapInfo1.bmiColors[i].rgbBlue = (BYTE)i;
		GrayBitmapInfo1.bmiColors[i].rgbGreen = (BYTE)i;
		GrayBitmapInfo1.bmiColors[i].rgbRed = (BYTE)i;
		GrayBitmapInfo1.bmiColors[i].rgbReserved = 0;

		GrayBitmapInfo2.bmiColors[i].rgbBlue = (BYTE)i;
		GrayBitmapInfo2.bmiColors[i].rgbGreen = (BYTE)i;
		GrayBitmapInfo2.bmiColors[i].rgbRed = (BYTE)i;
		GrayBitmapInfo2.bmiColors[i].rgbReserved = 0;
	}

	Colorbmphead1.biSize = sizeof(BITMAPINFOHEADER);
	Colorbmphead1.biWidth = area.signalWidth();
	Colorbmphead1.biHeight = 0-area.signalHeight();
	Colorbmphead1.biPlanes = 1;
	Colorbmphead1.biBitCount = 24;
	Colorbmphead1.biCompression = BI_RGB;
	Colorbmphead1.biSizeImage = ColorbmpWidthBytes1*area.signalHeight();
	Colorbmphead1.biXPelsPerMeter = 0;
	Colorbmphead1.biYPelsPerMeter = 0;
	Colorbmphead1.biClrUsed = 0;
	Colorbmphead1.biClrImportant = 0;

	Colorbmphead2.biSize = sizeof(BITMAPINFOHEADER);
	Colorbmphead2.biWidth = area.signalWidth() / 2;
	Colorbmphead2.biHeight = 0-area.signalHeight();
	Colorbmphead2.biPlanes = 1;
	Colorbmphead2.biBitCount = 24;
	Colorbmphead2.biCompression = BI_RGB;
	Colorbmphead2.biSizeImage = ColorbmpWidthBytes2*area.signalHeight();
	Colorbmphead2.biXPelsPerMeter = 0;
	Colorbmphead2.biYPelsPerMeter = 0;
	Colorbmphead2.biClrUsed = 0;
	Colorbmphead2.biClrImportant = 0;
}

void CShowDlg::OnPaint()
{
	CPaintDC dc(this); 
	GetClientRect(&clientRect);
	exhibitRect1 = { clientSizeInfo.borderX1,
		clientSizeInfo.borderY1,
		clientSizeInfo.borderX1 + (clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap * 3 - clientSizeInfo.borderX1) / 2,
		clientRect.bottom - clientSizeInfo.borderY2 };
	exhibitRect2 = { clientSizeInfo.borderX1 + (clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap * 3 - clientSizeInfo.borderX1) / 2 + clientSizeInfo.gap,
		clientSizeInfo.borderY1,
		clientSizeInfo.borderX1 + (clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap * 3 - clientSizeInfo.borderX1) * 3 / 4 + clientSizeInfo.gap,
		clientRect.bottom - clientSizeInfo.borderY2 };
	exhibitRect3 = { clientSizeInfo.borderX1 + (clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap * 3 - clientSizeInfo.borderX1) * 3 / 4 + clientSizeInfo.gap * 2,
		clientSizeInfo.borderY1,
		clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap ,
		clientRect.bottom - clientSizeInfo.borderY2 };

	destrc = CRect(
		clientSizeInfo.borderX1,
		clientSizeInfo.borderY1,
		clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - clientSizeInfo.gap /*- clientSizeInfo.borderX1*/,
		clientRect.bottom - clientSizeInfo.borderY2 /*- clientSizeInfo.borderY1*/);

	CMemDC dcmem(*(this->GetDC()), clientRect);
	dcmem.GetDC().SetStretchBltMode(COLORONCOLOR);
	dcmem.GetDC().FillSolidRect(&clientRect, RGB(255, 255, 255));

	TEXTMETRIC tm;
	dcmem.GetDC().GetTextMetrics(&tm);

	CFont myFont;
	myFont.CreateFontW(tm.tmHeight*1.3, tm.tmAveCharWidth*1.3,
		0,
		0,
		10,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		_T("Arial"));

	CFont *pOldFont = dcmem.GetDC().SelectObject(&myFont);

	switch (status)
	{
	case CShowDlg::NONE:
	{
		break;
	}
	case CShowDlg::SGLCOLOR:
	{
		if (m_colorBar.IsNull())
		{
			bSglMenuShow = false;
		}
		else
		{
			bSglMenuShow = true;
		}
		if (!singleColorImg1.IsNull())
		{
			//将源矩形复制到目标矩形
			dcmem.GetDC().StretchBlt(exhibitRect1.left,
				exhibitRect1.top,
				exhibitRect1.right - exhibitRect1.left,
				exhibitRect1.bottom - exhibitRect1.top,
				CDC::FromHandle(singleColorImg1.GetDC()),
				0,
				0,
				imgRect1.right,
				imgRect1.bottom,
				SRCCOPY);
			singleColorImg1.ReleaseDC();
			dcmem.GetDC().StretchBlt(
				exhibitRect2.left,
				exhibitRect2.top,
				exhibitRect2.right - exhibitRect2.left,
				exhibitRect2.bottom - exhibitRect2.top,
				CDC::FromHandle(singleColorImg2.GetDC()),
				0,
				0,
				imgRect2.right,
				imgRect2.bottom,
				SRCCOPY);
			singleColorImg2.ReleaseDC();
			dcmem.GetDC().StretchBlt(
				exhibitRect3.left,
				exhibitRect3.top,
				exhibitRect3.right - exhibitRect3.left,
				exhibitRect3.bottom - exhibitRect3.top,
				CDC::FromHandle(singleColorImg3.GetDC()),
				0,
				0,
				imgRect3.right,
				imgRect3.bottom,
				SRCCOPY);

			singleColorImg3.ReleaseDC();
			//Colorbar
			dcmem.GetDC().StretchBlt(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
				clientSizeInfo.borderY1 + clientSizeInfo.geap,
				clientSizeInfo.barWidth,
				clientRect.bottom - clientSizeInfo.borderY2 - clientSizeInfo.borderY1 - clientSizeInfo.geap,
				CDC::FromHandle(m_colorBar.GetDC()),
				0,
				0,
				colorBarRect.right,
				colorBarRect.bottom,
				SRCCOPY);
			m_colorBar.ReleaseDC();
			PaintImageCoordinate(dcmem);
			RECT barrect;
			barrect.left = clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth;
			barrect.right = barrect.left + clientSizeInfo.barWidth;
			barrect.top = clientSizeInfo.borderY1 + clientSizeInfo.geap;
			barrect.bottom = clientRect.bottom - clientSizeInfo.borderY2;
			int barheight = barrect.bottom - barrect.top;

			//标上colorbar的值
			CString ch;
			int times = 5;
			//每一节长度
			double length = barheight*1.0 / (times - 1);
			//每一部分差值
			double part = (dataInfo.colorbarMax-dataInfo.colorbarMin) / 4.0;
			for (int i = 0; i < times; i++)
			{
				dcmem.GetDC().Rectangle(barrect.right - 5, barrect.bottom - length*i - 1,
					barrect.right, barrect.bottom - length*i + 1);
				ch.Format(_T("%.2g"), double(dataInfo.colorbarMin + part*i));
				dcmem.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 8,
					barrect.bottom - length*i - tm.tmHeight / 2,
					ch, ch.GetLength());
			}
			ch = "0";
			dcmem.GetDC().TextOut(barrect.left - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
			ch = "1";
			dcmem.GetDC().TextOut(barrect.right - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
			break;
		}
	}
	case CShowDlg::SGLGRAY:
	{
		if (!singleGrayImg1.IsNull())
		{
			//将源矩形复制到目标矩形
			dcmem.GetDC().StretchBlt(exhibitRect1.left,
				exhibitRect1.top,
				exhibitRect1.right - exhibitRect1.left,
				exhibitRect1.bottom - exhibitRect1.top,
				CDC::FromHandle(singleGrayImg1.GetDC()),
				0,
				0,
				imgRect1.right,
				imgRect1.bottom,
				SRCCOPY);
			singleGrayImg1.ReleaseDC();
			dcmem.GetDC().StretchBlt(exhibitRect2.left,
				exhibitRect2.top,
				exhibitRect2.right - exhibitRect2.left,
				exhibitRect2.bottom - exhibitRect2.top,
				CDC::FromHandle(singleGrayImg2.GetDC()),
				0,
				0,
				imgRect2.right,
				imgRect2.bottom,
				SRCCOPY);
			singleGrayImg2.ReleaseDC();
			dcmem.GetDC().StretchBlt(exhibitRect3.left,
				exhibitRect3.top,
				exhibitRect3.right - exhibitRect3.left,
				exhibitRect3.bottom - exhibitRect3.top,
				CDC::FromHandle(singleGrayImg3.GetDC()),
				0,
				0,
				imgRect3.right,
				imgRect3.bottom,
				SRCCOPY);

			singleGrayImg3.ReleaseDC();
			//Colorbar
			dcmem.GetDC().StretchBlt(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
				clientSizeInfo.borderY1 + clientSizeInfo.geap,
				clientSizeInfo.barWidth,
				clientRect.bottom - clientSizeInfo.borderY2 - clientSizeInfo.borderY1 - clientSizeInfo.geap,
				CDC::FromHandle(m_grayBar.GetDC()),
				0,
				0,
				colorBarRect.right,
				colorBarRect.bottom,
				SRCCOPY);
			m_grayBar.ReleaseDC();
			PaintImageCoordinate(dcmem);
			RECT barrect;
			barrect.left = clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth;
			barrect.right = barrect.left + clientSizeInfo.barWidth;
			barrect.top = clientSizeInfo.borderY1 + clientSizeInfo.geap;
			barrect.bottom = clientRect.bottom - clientSizeInfo.borderY2;
			int barheight = barrect.bottom - barrect.top;
			//标上colorbar的值
			CString ch;
			int times = 5;
			//每一节长度
			double length = barheight*1.0 / (times - 1);
			//每一部分差值
			double part = (dataInfo.colorbarMax - dataInfo.colorbarMin) / 4.0;
			for (int i = 0; i < times; i++)
			{
				dcmem.GetDC().Rectangle(barrect.right - 5, barrect.bottom - length*i - 1,
					barrect.right, barrect.bottom - length*i + 1);
				ch.Format(_T("%.2g"), double(dataInfo.colorbarMin + part*i));
				dcmem.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 8,
					barrect.bottom - length*i - tm.tmHeight / 2,
					ch, ch.GetLength());
			}
			ch = "0";
			dcmem.GetDC().TextOut(barrect.left - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
			ch = "1";
			dcmem.GetDC().TextOut(barrect.right - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
			break;
		}
	}
	case CShowDlg::MLTUNCOLOR:
	{
		if (m_colorBar.IsNull())
		{
			bSglMenuShow = false;
		}
		else
		{
			bSglMenuShow = true;
		}
		if (!m_colorBar.IsNull())
		{
			//Colorbar
			dcmem.GetDC().StretchBlt(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
				clientSizeInfo.borderY1 + clientSizeInfo.geap,
				clientSizeInfo.barWidth,
				clientRect.bottom - clientSizeInfo.borderY2 - clientSizeInfo.borderY1 - clientSizeInfo.geap,
				CDC::FromHandle(m_colorBar.GetDC()),
				0,
				0,
				colorBarRect.right,
				colorBarRect.bottom,
				SRCCOPY);
			m_colorBar.ReleaseDC();
			PaintImageCoordinate(dcmem);
			RECT barrect;
			barrect.left = clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth;
			barrect.right = barrect.left + clientSizeInfo.barWidth;
			barrect.top = clientSizeInfo.borderY1 + clientSizeInfo.geap;
			barrect.bottom = clientRect.bottom - clientSizeInfo.borderY2;
			int barheight = barrect.bottom - barrect.top;
			//标上colorbar的值
			CString ch;
			int times = 5;
			//每一节长度
			double length = barheight*1.0 / (times - 1);
			//每一部分差值
			double part = (dataInfo.colorbarMax - dataInfo.colorbarMin) / 4.0;
			for (int i = 0; i < times; i++)
			{
				dcmem.GetDC().Rectangle(barrect.right - 5, barrect.bottom - length*i - 1,
					barrect.right, barrect.bottom - length*i + 1);
				ch.Format(_T("%.2g"), double(dataInfo.colorbarMin + part*i));
				dcmem.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 8,
					barrect.bottom - length*i - tm.tmHeight / 2,
					ch, ch.GetLength());
			}
			ch = "0";
			dcmem.GetDC().TextOut(barrect.left - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
			ch = "1";
			dcmem.GetDC().TextOut(barrect.right - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
		}
		break;
	}
	case CShowDlg::MLTUNGRAY:
	{
		if (m_colorBar.IsNull())
		{
			bSglMenuShow = false;
		}
		else
		{
			bSglMenuShow = true;
		}
		if (!m_grayBar.IsNull())
		{
			//Colorbar
			dcmem.GetDC().StretchBlt(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
				clientSizeInfo.borderY1 + clientSizeInfo.geap,
				clientSizeInfo.barWidth,
				clientRect.bottom - clientSizeInfo.borderY2 - clientSizeInfo.borderY1 - clientSizeInfo.geap,
				CDC::FromHandle(m_grayBar.GetDC()),
				0,
				0,
				colorBarRect.right,
				colorBarRect.bottom,
				SRCCOPY);
			m_grayBar.ReleaseDC();
			PaintImageCoordinate(dcmem);
			RECT barrect;
			barrect.left = clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth;
			barrect.right = barrect.left + clientSizeInfo.barWidth;
			barrect.top = clientSizeInfo.borderY1 + clientSizeInfo.geap;
			barrect.bottom = clientRect.bottom - clientSizeInfo.borderY2;
			int barheight = barrect.bottom - barrect.top;
			//标上colorbar的值
			CString ch;

			int times = 5;
			//每一节长度
			double length = barheight*1.0 / (times - 1);
			//每一部分差值
			double part = (dataInfo.colorbarMax - dataInfo.colorbarMin) / 4.0;
			for (int i = 0; i < times; i++)
			{
				dcmem.GetDC().Rectangle(barrect.right - 5, barrect.bottom - length*i - 1,
					barrect.right, barrect.bottom - length*i + 1);
				ch.Format(_T("%.2g"), double(dataInfo.colorbarMin + part*i));
				dcmem.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 8,
					barrect.bottom - length*i - tm.tmHeight / 2,
					ch, ch.GetLength());
			}
			ch = "0";
			dcmem.GetDC().TextOut(barrect.left - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
			ch = "1";
			dcmem.GetDC().TextOut(barrect.right - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
		}
		break;
	}
	case CShowDlg::MLTCOLOR:
	{
		if (m_colorBar.IsNull())
		{
			bSglMenuShow = false;
		}
		else
		{
			bSglMenuShow = true;
		}
		if (!m_colorBar.IsNull())
		{
			//Colorbar
			dcmem.GetDC().StretchBlt(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
				clientSizeInfo.borderY1 + clientSizeInfo.geap,
				clientSizeInfo.barWidth,
				clientRect.bottom - clientSizeInfo.borderY2 - clientSizeInfo.borderY1 - clientSizeInfo.geap,
				CDC::FromHandle(m_colorBar.GetDC()),
				0,
				0,
				colorBarRect.right,
				colorBarRect.bottom,
				SRCCOPY);
			m_colorBar.ReleaseDC();
			PaintImageCoordinate(dcmem);
			RECT barrect;
			barrect.left = clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth;
			barrect.right = barrect.left + clientSizeInfo.barWidth;
			barrect.top = clientSizeInfo.borderY1 + clientSizeInfo.geap;
			barrect.bottom = clientRect.bottom - clientSizeInfo.borderY2;
			int barheight = barrect.bottom - barrect.top;
			//标上colorbar的值
			CString ch;
			int times = 5;
			//每一节长度
			double length = barheight*1.0 / (times - 1);
			//每一部分差值
			double part = (dataInfo.colorbarMax - dataInfo.colorbarMin) / 4.0;
			for (int i = 0; i < times; i++)
			{
				dcmem.GetDC().Rectangle(barrect.right - 5, barrect.bottom - length*i - 1,
					barrect.right, barrect.bottom - length*i + 1);
				ch.Format(_T("%.2g"), double(dataInfo.colorbarMin + part*i));
				dcmem.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 8,
					barrect.bottom - length*i - tm.tmHeight / 2,
					ch, ch.GetLength());
			}
			ch = "0";
			dcmem.GetDC().TextOut(barrect.left - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
			ch = "1";
			dcmem.GetDC().TextOut(barrect.right - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());

		}
		break;
	}
	case CShowDlg::MLTGRAY:
	{
		if (m_grayBar.IsNull())
		{
			bSglMenuShow = false;
		}
		else
		{
			bSglMenuShow = true;
		}
		if (!m_grayBar.IsNull())
		{
			//Colorbar
			dcmem.GetDC().StretchBlt(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
				clientSizeInfo.borderY1 + clientSizeInfo.geap,
				clientSizeInfo.barWidth,
				clientRect.bottom - clientSizeInfo.borderY2 - clientSizeInfo.borderY1 - clientSizeInfo.geap,
				CDC::FromHandle(m_grayBar.GetDC()),
				0,
				0,
				colorBarRect.right,
				colorBarRect.bottom,
				SRCCOPY);
			m_grayBar.ReleaseDC();
			PaintImageCoordinate(dcmem);
			RECT barrect;
			barrect.left = clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth;
			barrect.right = barrect.left + clientSizeInfo.barWidth;
			barrect.top = clientSizeInfo.borderY1 + clientSizeInfo.geap;
			barrect.bottom = clientRect.bottom - clientSizeInfo.borderY2;
			int barheight = barrect.bottom - barrect.top;
			//标上colorbar的值
			CString ch;
			int times = 5;
			//每一节长度
			double length = barheight*1.0 / (times - 1);
			//每一部分差值
			double part = (dataInfo.colorbarMax - dataInfo.colorbarMin) / 4.0;
			for (int i = 0; i < times; i++)
			{
				dcmem.GetDC().Rectangle(barrect.right - 5, barrect.bottom - length*i - 1,
					barrect.right, barrect.bottom - length*i + 1);
				ch.Format(_T("%.2g"), double(dataInfo.colorbarMin + part*i));
				dcmem.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 8,
					barrect.bottom - length*i - tm.tmHeight / 2,
					ch, ch.GetLength());
			}
			ch = "0";
			dcmem.GetDC().TextOut(barrect.left - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
			ch = "1";
			dcmem.GetDC().TextOut(barrect.right - 2,
				barrect.bottom + 10,
				ch, ch.GetLength());
		}
		break;
	}
	default:
		break;
	}

	dcmem.GetDC().SelectObject(pOldFont);
}

void CShowDlg::loadColorImg(Mymatrix<double>* &Ang, Mymatrix<double>* &Am, int wrapFlag, int flag)
{
	if (p_ColorImg != NULL)
	{
		delete[]p_ColorImg;
	}
	int width = Ang->Col;
	int height = Ang->Row;
	double dis_max, dis_min;
	if (wrapFlag == 0 || wrapFlag == 2)
	{
		dis_max = PI;
		dis_min = -PI;
	}
	else
	{
		if (flag==1)
		{
			
			dis_min = multiData.unwrapMin;
			dis_max = multiData.unwrapMax;
		}
		else
		{
			dis_min = multiData.umMin;
			dis_max = multiData.umMax;
		}
	}
	Mymatrix<double> h(Ang->Row, Ang->Col);
	for (int i = 0; i < Ang->Row; i++)
	{
		for (int j = 0; j < Ang->Col; j++)
		{
			h.Data[i][j] = double((Ang->Data[i][j] - dis_min) / (dis_max - dis_min));
		}
	}

	//图片每行字节数
	int bmpWidthBytes = ((width * 24 + 31) & ~31) / 8;

	p_ColorImg = new BYTE[bmpWidthBytes * height];

	for (int i = 0; i < height; i++)
	{
		//int imgLineIndex = (height - i - 1)*bmpWidthBytes;
		int imgLineIndex = i*bmpWidthBytes;
		for (int j = 0; j < width; j++)
		{
			switch (int(ceil(h(i, j) * 72)))
			{
			case 0:
			case 1:
			{
				p_ColorImg[imgLineIndex + j * 3] = 127;
				p_ColorImg[imgLineIndex + j * 3 + 1] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 2:case 3:case 4:case 5:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(127 + 14 * 72.0*(h(i, j) - 1 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 6:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(183 + 15 * 72.0*(h(i, j) - 5 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 7:case 8:case 9:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(198 + 14 * 72.0*(h(i, j) - 6 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 10:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(240 + 15 * 72.0*(h(i, j) - 9 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:
			{
				p_ColorImg[imgLineIndex + j * 3] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(14 * 72.0*(h(i, j) - 10 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 19:
			{
				p_ColorImg[imgLineIndex + j * 3] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(112 + 15 * 72.0*(h(i, j) - 18 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 20:case 21:case 22:case 23:
			{
				p_ColorImg[imgLineIndex + j * 3] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(127 + 14 * 72.0*(h(i, j) - 19 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 24:
			{
				p_ColorImg[imgLineIndex + j * 3] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(183 + 15 * 72.0*(h(i, j) - 23 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 25:case 26:case 27:
			{
				p_ColorImg[imgLineIndex + j * 3] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(198 + 14 * 72.0*(h(i, j) - 24 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 28:
			{
				p_ColorImg[imgLineIndex + j * 3] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(240 + 15 * 72.0*(h(i, j) - 27 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 29:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(255 - 15 * 72.0*(h(i, j) - 28 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(14 * 72.0*(h(i, j) - 28 * 1.0 / 72));
				break;
			}
			case 30:case 31:case 32:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(240 - 14 * 72.0*(h(i, j) - 29 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(14 + 14 * 72.0*(h(i, j) - 29 * 1.0 / 72));
				break;
			}
			case 33:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(198 - 15 * 72.0*(h(i, j) - 32 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(56 + 14 * 72.0*(h(i, j) - 32 * 1.0 / 72));
				break;
			}
			case 34:case 35:case 36:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(183 - 14 * 72.0*(h(i, j) - 33 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(70 + 14 * 72.0*(h(i, j) - 33 * 1.0 / 72));
				break;
			}
			case 37:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(141 - 14 * 72.0*(h(i, j) - 36 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(112 + 15 * 72.0*(h(i, j) - 36 * 1.0 / 72));
				break;
			}
			case 38:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(127 - 15 * 72.0*(h(i, j) - 37 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(127 + 14 * 72.0*(h(i, j) - 37 * 1.0 / 72));
				break;
			}
			case 39:case 40:case 41:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(112 - 14 * 72.0*(h(i, j) - 38 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(141 + 14 * 72.0*(h(i, j) - 38 * 1.0 / 72));
				break;
			}
			case 42:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(70 - 14 * 72.0*(h(i, j) - 41 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(183 + 15 * 72.0*(h(i, j) - 41 * 1.0 / 72));
				break;
			}
			case 43:case 44:case 45:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(56 - 14 * 72.0*(h(i, j) - 42 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(198 + 15 * 72.0*(h(i, j) - 42 * 1.0 / 72));
				break;
			}
			case 46:
			{
				p_ColorImg[imgLineIndex + j * 3] = BYTE(14 - 14 * 72.0*(h(i, j) - 45 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 1] = 255;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(240 +15 * 72.0*(h(i, j) - 45 * 1.0 / 72));
				break;
			}
			case 47:
			{
				p_ColorImg[imgLineIndex + j * 3] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(255 - 15 * 72.0*(h(i, j) - 46 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 48:case 49:case 50:
			{
				p_ColorImg[imgLineIndex + j * 3] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(240 - 14 * 72.0*(h(i, j) - 47 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 51:
			{
				p_ColorImg[imgLineIndex + j * 3] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(198 - 15 * 72.0*(h(i, j) - 50 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 52:case 53:case 54:case 55:
			{
				p_ColorImg[imgLineIndex + j * 3] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(183 - 14 * 72.0*(h(i, j) - 51 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 56:
			{
				p_ColorImg[imgLineIndex + j * 3] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(127 - 15 * 72.0*(h(i, j) - 55 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 57:case 58:case 59:case 60:case 61:case 62:case 63:case 64:
			{
				p_ColorImg[imgLineIndex + j * 3] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(112 - 14 * 72.0*(h(i, j) - 56 * 1.0 / 72));
				p_ColorImg[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 65:
			{
				p_ColorImg[imgLineIndex + j * 3] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 1] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(255 - 15 * 72.0*(h(i, j) - 64 * 1.0 / 72));
				break;
			}
			case 66:case 67:case 68:
			{
				p_ColorImg[imgLineIndex + j * 3] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 1] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(240 - 14 * 72.0*(h(i, j) - 65 * 1.0 / 72));
				break;
			}
			case 69:
			{
				p_ColorImg[imgLineIndex + j * 3] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 1] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(198 - 15 * 72.0*(h(i, j) - 68 * 1.0 / 72));
				break;
			}
			case 70:case 71:case 72:
			{
				p_ColorImg[imgLineIndex + j * 3] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 1] = 0;
				p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(183 - 14 * 72.0*(h(i, j) - 69 * 1.0 / 72));
				break;
			}
			}
		}
		for (int j = width * 3; j < bmpWidthBytes; j++)
		{
			p_ColorImg[imgLineIndex + j] = 0;
		}
	}

	for (int i = 0; i < height; i++)
	{
		int imgLineIndex = i*bmpWidthBytes;
		for (int j = 0; j < width; j++)
		{
			p_ColorImg[imgLineIndex + j * 3] = BYTE(p_ColorImg[imgLineIndex + j * 3] * Am->Data[i][j]);
			p_ColorImg[imgLineIndex + j * 3 + 1] = BYTE(p_ColorImg[imgLineIndex + j * 3 + 1] * Am->Data[i][j]);
			p_ColorImg[imgLineIndex + j * 3 + 2] = BYTE(p_ColorImg[imgLineIndex + j * 3 + 2] * Am->Data[i][j]);
		}
	}
	switch (wrapFlag)
	{
	case 0:
	{
		if (flag == 1)
		{
			HBITMAP himg = CreateDIBitmap(GetWindowDC()->GetSafeHdc(),
				&Colorbmphead1,
				CBM_INIT,
				p_ColorImg,
				(BITMAPINFO *)&Colorbmphead1,
				DIB_RGB_COLORS);

			singleColorImg1.Attach(himg);
			imgRect1.left = 0;
			imgRect1.right = width;
			imgRect1.top = 0;
			imgRect1.bottom = height;
		}
		else if (flag == 2)
		{
			HBITMAP himg = CreateDIBitmap(GetWindowDC()->GetSafeHdc(),
				&Colorbmphead2,
				CBM_INIT,
				p_ColorImg,
				(BITMAPINFO *)&Colorbmphead2,
				DIB_RGB_COLORS);

			singleColorImg2.Attach(himg);
			imgRect2.left = 0;
			imgRect2.right = width;
			imgRect2.top = 0;
			imgRect2.bottom = height;
		}
		else
		{
			HBITMAP himg = CreateDIBitmap(GetWindowDC()->GetSafeHdc(),
				&Colorbmphead2,
				CBM_INIT,
				p_ColorImg,
				(BITMAPINFO *)&Colorbmphead2,
				DIB_RGB_COLORS);

			singleColorImg3.Attach(himg);
			imgRect3.left = 0;
			imgRect3.right = width;
			imgRect3.top = 0;
			imgRect3.bottom = height;
			status = SGLCOLOR;
			CShowDlg::getInstance()->setTitle(CString(_T("卷绕相位静态彩图")));
			CShowDlg::getInstance()->set_X_Y_range(9.65*sglData.freCount/694*((area.startCol-1)*1.0 / sglData.freCount), 
				9.65*sglData.freCount / 694 * (area.endCol*1.0 / sglData.freCount),
				8.4*(1 - area.endRow*1.0 / 1038), 
				8.4*(1 - (area.startRow - 1)*1.0 / 1038));
			CShowDlg::getInstance()->setColorbarrange(-PI, PI);
			CShowDlg::getInstance()->loadColorBar();
			RedrawWindow();
		}
		break;
	}
	case 1:
	{
		if (flag == 1)
		{
			unPhaseColorImg1[unPhaseColorImgNum1] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				unPhaseColorImg1[unPhaseColorImgNum1][i] = p_ColorImg[i];
			}
			unPhaseColorVec1.push_back(unPhaseColorImg1[unPhaseColorImgNum1]);
			unPhaseColorImgNum1++;
			imgRect1.left = 0;
			imgRect1.right = width;
			imgRect1.top = 0;
			imgRect1.bottom = height;
		}
		else if (flag == 2)
		{
			unPhaseColorImg2[unPhaseColorImgNum2] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				unPhaseColorImg2[unPhaseColorImgNum2][i] = p_ColorImg[i];
			}
			unPhaseColorVec2.push_back(unPhaseColorImg2[unPhaseColorImgNum2]);
			unPhaseColorImgNum2++;
			imgRect2.left = 0;
			imgRect2.right = width;
			imgRect2.top = 0;
			imgRect2.bottom = height;
		}
		else
		{
			unPhaseColorImg3[unPhaseColorImgNum3] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				unPhaseColorImg3[unPhaseColorImgNum3][i] = p_ColorImg[i];
			}
			unPhaseColorVec3.push_back(unPhaseColorImg3[unPhaseColorImgNum3]);
			unPhaseColorImgNum3++;
			imgRect3.left = 0;
			imgRect3.right = width;
			imgRect3.top = 0;
			imgRect3.bottom = height;
		}
		break;
	}
	case 2:
	{
		if (flag == 1)
		{
			PhaseColorImg1[PhaseColorImgNum1] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				PhaseColorImg1[PhaseColorImgNum1][i] = p_ColorImg[i];
			}
			PhaseColorVec1.push_back(PhaseColorImg1[PhaseColorImgNum1]);
			PhaseColorImgNum1++;
			imgRect1.left = 0;
			imgRect1.right = width;
			imgRect1.top = 0;
			imgRect1.bottom = height;
		}
		else if (flag == 2)
		{
			PhaseColorImg2[PhaseColorImgNum2] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				PhaseColorImg2[PhaseColorImgNum2][i] = p_ColorImg[i];
			}
			PhaseColorVec2.push_back(PhaseColorImg2[PhaseColorImgNum2]);
			PhaseColorImgNum2++;
			imgRect2.left = 0;
			imgRect2.right = width;
			imgRect2.top = 0;
			imgRect2.bottom = height;
		}
		else
		{
			PhaseColorImg3[PhaseColorImgNum3] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				PhaseColorImg3[PhaseColorImgNum3][i] = p_ColorImg[i];
			}
			PhaseColorVec3.push_back(PhaseColorImg3[PhaseColorImgNum3]);
			PhaseColorImgNum3++;
			imgRect3.left = 0;
			imgRect3.right = width;
			imgRect3.top = 0;
			imgRect3.bottom = height;
		}
		break;
	}
	default:
		break;
		this->RedrawWindow();
	}
}
void CShowDlg::loadGrayImg(Mymatrix<double>* &Ang, Mymatrix<double>* &Am, int wrapFlag, int flag)
{
	if (p_GrayImg != NULL)
	{
		delete[]p_GrayImg;
	}
	int width = Ang->Col;
	int height = Ang->Row;
	double dis_max, dis_min;
	if (wrapFlag == 0 || wrapFlag == 2)
	{
		dis_max = PI;
		dis_min = -PI;
	}
	else
	{
		if (flag == 1)
		{
			dis_min = multiData.unwrapMin;
			dis_max = multiData.unwrapMax;
		}
		else
		{
			
			dis_min = multiData.umMin;
			dis_max = multiData.umMax;
		}
	}
	Mymatrix<double> h(Ang->Row, Ang->Col);
	for (int i = 0; i < Ang->Row; i++)
	{
		for (int j = 0; j < Ang->Col; j++)
		{
			h.Data[i][j] = double((Ang->Data[i][j] - dis_min) / (dis_max - dis_min));
		}
	}
	//颜色映射
	double max = h(0, 0), min = h(0, 0);
	for (int i = 0; i < h.Row; i++)
	{
		for (int j = 0; j < h.Col; j++)
		{
			if (max < h(i, j))
			{
				max = h(i, j);
			}
			else if (min > h(i, j))
			{
				min = h(i, j);
			}
		}
	}
	double part = (max - min) / 255.0;
	int bmpWidthBytes = ((width * 8 + 31) & ~31) / 8;
	p_GrayImg = new BYTE[bmpWidthBytes * height];
	for (int i = 0; i < height; i++)
	{
		//int imgLineIndex = (height - i - 1)*bmpWidthBytes;
		int imgLineIndex = i*bmpWidthBytes;
		for (int j = 0; j < width; j++)
		{
			p_GrayImg[imgLineIndex + j] = BYTE((h.Data[i][j] - min) / part);
		}
		for (int j = width; j < bmpWidthBytes; j++)
		{
			p_GrayImg[imgLineIndex + j] = 0;
		}
	}
	for (int i = 0; i < height; i++)
	{
		int imgLineIndex = i*bmpWidthBytes;
		for (int j = 0; j < width; j++)
		{
			p_GrayImg[imgLineIndex + j] = BYTE(p_GrayImg[imgLineIndex + j] * Am->Data[i][j]);
		}
	}

	switch (wrapFlag)
	{
	case 0:
	{
		if (flag == 1)
		{
			HBITMAP himg = CreateDIBitmap(GetWindowDC()->GetSafeHdc(),
				&GrayBitmapInfo1.bmiHeader,
				CBM_INIT,
				p_GrayImg,
				(BITMAPINFO *)&GrayBitmapInfo1,
				DIB_RGB_COLORS);

			singleGrayImg1.Attach(himg);
			imgRect1.left = 0;
			imgRect1.right = width;
			imgRect1.top = 0;
			imgRect1.bottom = height;
		}
		else if (flag == 2)
		{
			HBITMAP himg = CreateDIBitmap(GetWindowDC()->GetSafeHdc(),
				&GrayBitmapInfo2.bmiHeader,
				CBM_INIT,
				p_GrayImg,
				(BITMAPINFO *)&GrayBitmapInfo2,
				DIB_RGB_COLORS);

			singleGrayImg2.Attach(himg);
			imgRect2.left = 0;
			imgRect2.right = width;
			imgRect2.top = 0;
			imgRect2.bottom = height;
		}
		else
		{
			HBITMAP himg = CreateDIBitmap(GetWindowDC()->GetSafeHdc(),
				&GrayBitmapInfo2.bmiHeader,
				CBM_INIT,
				p_GrayImg,
				(BITMAPINFO *)&GrayBitmapInfo2,
				DIB_RGB_COLORS);

			singleGrayImg3.Attach(himg);
			imgRect3.left = 0;
			imgRect3.right = width;
			imgRect3.top = 0;
			imgRect3.bottom = height;
		}

		break;
	}
	case 1:
	{
		if (flag == 1)
		{
			unPhaseGrayImg1[unPhaseGrayImgNum1] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				unPhaseGrayImg1[unPhaseGrayImgNum1][i] = p_GrayImg[i];
			}
			unPhaseGrayVec1.push_back(unPhaseGrayImg1[unPhaseGrayImgNum1]);
			unPhaseGrayImgNum1++;
			imgRect1.left = 0;
			imgRect1.right = width;
			imgRect1.top = 0;
			imgRect1.bottom = height;
		}
		else if (flag == 2)
		{
			unPhaseGrayImg2[unPhaseGrayImgNum2] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				unPhaseGrayImg2[unPhaseGrayImgNum2][i] = p_GrayImg[i];
			}
			unPhaseGrayVec2.push_back(unPhaseGrayImg2[unPhaseGrayImgNum2]);
			unPhaseGrayImgNum2++;
			imgRect2.left = 0;
			imgRect2.right = width;
			imgRect2.top = 0;
			imgRect2.bottom = height;
		}
		else
		{
			unPhaseGrayImg3[unPhaseGrayImgNum3] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				unPhaseGrayImg3[unPhaseGrayImgNum3][i] = p_GrayImg[i];
			}
			unPhaseGrayVec3.push_back(unPhaseGrayImg3[unPhaseGrayImgNum3]);
			unPhaseGrayImgNum3++;
			imgRect3.left = 0;
			imgRect3.right = width;
			imgRect3.top = 0;
			imgRect3.bottom = height;
		}
		break;
	}
	case 2:
	{
		if (flag == 1)
		{
			PhaseGrayImg1[PhaseGrayImgNum1] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				PhaseGrayImg1[PhaseGrayImgNum1][i] = p_GrayImg[i];
			}
			PhaseGrayVec1.push_back(PhaseGrayImg1[PhaseGrayImgNum1]);
			PhaseGrayImgNum1++;
			imgRect1.left = 0;
			imgRect1.right = width;
			imgRect1.top = 0;
			imgRect1.bottom = height;
		}
		else if (flag == 2)
		{
			PhaseGrayImg2[PhaseGrayImgNum2] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				PhaseGrayImg2[PhaseGrayImgNum2][i] = p_GrayImg[i];
			}
			PhaseGrayVec2.push_back(PhaseGrayImg2[PhaseGrayImgNum2]);
			PhaseGrayImgNum2++;
			imgRect2.left = 0;
			imgRect2.right = width;
			imgRect2.top = 0;
			imgRect2.bottom = height;
		}
		else
		{
			PhaseGrayImg3[PhaseGrayImgNum3] = new BYTE[bmpWidthBytes * height];
			for (int i = 0; i < bmpWidthBytes * height; i++)
			{
				PhaseGrayImg3[PhaseGrayImgNum3][i] = p_GrayImg[i];
			}
			PhaseGrayVec3.push_back(PhaseGrayImg3[PhaseGrayImgNum3]);
			PhaseGrayImgNum3++;
			imgRect3.left = 0;
			imgRect3.right = width;
			imgRect3.top = 0;
			imgRect3.bottom = height;
		}
		break;
	}
	default:
		break;
	}
}

void CShowDlg::loadColorBar()
{
	if (!m_colorBar.IsNull())
	{
		return;
	}
	if (p_ColorBar != NULL)
	{
		//UnloadColorBar();
	}
#pragma region 生成colorbar
	//生成colorbar
	Mymatrix<double>* bar_ang_l = new Mymatrix<double>(1, 629, -PI);
	for (int i = 0; i < 629; i++)
	{
		bar_ang_l->Data[0][i] += double(0.01*i*1.0);
	}
	Mymatrix<double>*bar_am_l = new Mymatrix<double>(1, 101);
	for (int i = 0; i < 101; i++)
	{
		bar_am_l->Data[0][i] = double(0.01*i);
	}
	Mymatrix<double>*  one1 = new Mymatrix<double>(629, 1, 1);
	Mymatrix<double>*  one2 = new Mymatrix<double>(101, 1, 1);
	Mymatrix<double>*bar_am = matMultiply(*one1, *bar_am_l);
	Mymatrix<double>*bar_ang = matMultiply(*one2, *bar_ang_l);
	bar_ang->Transform();
	bar_am->Flipud();
	delete bar_am_l;
	delete bar_ang_l;
	delete one1;
	delete one2;

#pragma endregion
	int width = bar_ang->Col;
	int height = bar_ang->Row;
	double dis_max = PI;
	double dis_min = -PI;

	Mymatrix<double> h(bar_ang->Row, bar_ang->Col);
	for (int i = 0; i < bar_ang->Row; i++)
	{
		for (int j = 0; j < bar_ang->Col; j++)
		{
			h.Data[i][j] = double((bar_ang->Data[i][j] - dis_min) / (dis_max - dis_min));
		}
	}

	//图片每行字节数
	int bmpWidthBytes = ((width * 24 + 31) & ~31) / 8;

	p_ColorBar = new BYTE[bmpWidthBytes * height];

	for (int i = 0; i < height; i++)
	{
		int imgLineIndex = (height - i - 1)*bmpWidthBytes;
		//int imgLineIndex = i*bmpWidthBytes;
		for (unsigned int j = 0; j < width; j++)
		{
			switch (int(ceil(h(i, j) * 72)))
			{
			case 0:
			case 1:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 2] = 127;
				break;
			}
			case 2:case 3:case 4:case 5:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(127 + 14 * 72.0 * (h(i, j) - 1 * 1.0 / 72));
				break;
			}
			case 6:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(183 + 15 * 72.0*(h(i, j) - 5 * 1.0 / 72));
				break;
			}
			case 7:case 8:case 9:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(198 + 14 * 72.0*(h(i, j) - 6 * 1.0 / 72));
				break;
			}
			case 10:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(240 + 15 * 72.0*(h(i, j) - 9 * 1.0 / 72));
				break;
			}
			case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(14 * 72.0*(h(i, j) - 10 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 19:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(112 + 15 * 72.0*(h(i, j) - 18 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 20:case 21:case 22:case 23:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(127 + 14 * 72.0*(h(i, j) - 19 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 24:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(183 + 15 * 72.0*(h(i, j) - 23 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 25:case 26:case 27:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(198 + 14 * 72.0*(h(i, j) - 24 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 28:
			{
				p_ColorBar[imgLineIndex + j * 3] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(240 + 15 * 72.0*(h(i, j) - 27 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 29:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(14 * 72.0*(h(i, j) - 28 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(255 - 15 * 72.0*(h(i, j) - 28 * 1.0 / 72));
				break;
			}
			case 30:case 31:case 32:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(14 + 14 * 72.0*(h(i, j) - 29 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(240 - 14 * 72.0*(h(i, j) - 29 * 1.0 / 72));
				break;
			}
			case 33:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(56 + 14 * 72.0*(h(i, j) - 32 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(198 - 15 * 72.0*(h(i, j) - 32 * 1.0 / 72));
				break;
			}
			case 34:case 35:case 36:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(70 + 14 * 72.0*(h(i, j) - 33 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(183 - 14 * 72.0*(h(i, j) - 33 * 1.0 / 72));
				break;
			}
			case 37:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(112 + 15 * 72.0*(h(i, j) - 36 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(141 - 14 * 72.0*(h(i, j) - 36 * 1.0 / 72));
				break;
			}
			case 38:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(127 + 14 * 72.0*(h(i, j) - 37 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(127 - 15 * 72.0*(h(i, j) - 37 * 1.0 / 72));
				break;
			}
			case 39:case 40:case 41:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(141 + 14 * 72.0*(h(i, j) - 38 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(112 - 14 * 72.0*(h(i, j) - 38 * 1.0 / 72));
				break;
			}
			case 42:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(183 + 15 * 72.0*(h(i, j) - 41 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(70 - 14 * 72.0*(h(i, j) - 41 * 1.0 / 72));
				break;
			}
			case 43:case 44:case 45:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(198 + 15 * 72.0*(h(i, j) - 42 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(56 - 14 * 72.0*(h(i, j) - 42 * 1.0 / 72));
				break;
			}
			case 46:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(240 + 15 * 72.0*(h(i, j) - 45 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(14 - 14 * 72.0*(h(i, j) - 45 * 1.0 / 72));
				break;
			}
			case 47:
			{
				p_ColorBar[imgLineIndex + j * 3] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(255 - 15 * 72.0*(h(i, j) - 46 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 48:case 49:case 50:
			{
				p_ColorBar[imgLineIndex + j * 3] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(240 - 14 * 72.0*(h(i, j) - 47 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 51:
			{
				p_ColorBar[imgLineIndex + j * 3] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(198 - 15 * 72.0*(h(i, j) - 50 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 52:case 53:case 54:case 55:
			{
				p_ColorBar[imgLineIndex + j * 3] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(183 - 14 * 72.0*(h(i, j) - 51 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 56:
			{
				p_ColorBar[imgLineIndex + j * 3] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(127 - 15 * 72.0*(h(i, j) - 55 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 57:case 58:case 59:case 60:case 61:case 62:case 63:case 64:
			{
				p_ColorBar[imgLineIndex + j * 3] = 255;
				p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(112 - 14 * 72.0*(h(i, j) - 56 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 65:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(255 - 15 * 72.0*(h(i, j) - 64 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 66:case 67:case 68:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(240 - 14 * 72.0*(h(i, j) - 65 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 69:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(198 - 15 * 72.0*(h(i, j) - 68 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 70:case 71:case 72:
			{
				p_ColorBar[imgLineIndex + j * 3] = BYTE(183 - 14 * 72.0*(h(i, j) - 69 * 1.0 / 72));
				p_ColorBar[imgLineIndex + j * 3 + 1] = 0;
				p_ColorBar[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			}


		}
		for (int j = width * 3; j < bmpWidthBytes; j++)
		{
			p_ColorBar[imgLineIndex + j] = 0;
		}
	}

	for (int i = 0; i < height; i++)
	{
		int imgLineIndex = i*bmpWidthBytes;
		for (int j = 0; j < width; j++)
		{
			if (bar_am->Data[i][j] >= 1)
			{
				bar_am->Data[i][j] = 1;
			}
			else if (bar_am->Data[i][j] <= 0)
			{
				bar_am->Data[i][j] = 0;
			}
			p_ColorBar[imgLineIndex + j * 3] = BYTE(p_ColorBar[imgLineIndex + j * 3] * bar_am->Data[i][j]);
			p_ColorBar[imgLineIndex + j * 3 + 1] = BYTE(p_ColorBar[imgLineIndex + j * 3 + 1] * bar_am->Data[i][j]);
			p_ColorBar[imgLineIndex + j * 3 + 2] = BYTE(p_ColorBar[imgLineIndex + j * 3 + 2] * bar_am->Data[i][j]);
		}
	}

	//创建位图资源
	BITMAPINFOHEADER bmpColorBarhead;
	bmpColorBarhead.biSize = sizeof(BITMAPINFOHEADER);
	bmpColorBarhead.biWidth = width;
	bmpColorBarhead.biHeight = height;
	bmpColorBarhead.biPlanes = 1;
	bmpColorBarhead.biBitCount = 24;
	bmpColorBarhead.biCompression = BI_RGB;
	bmpColorBarhead.biSizeImage = bmpWidthBytes*height;
	bmpColorBarhead.biXPelsPerMeter = 0;
	bmpColorBarhead.biYPelsPerMeter = 0;
	bmpColorBarhead.biClrUsed = 0;
	bmpColorBarhead.biClrImportant = 0;

	HBITMAP hbar = CreateDIBitmap(this->GetDC()->GetSafeHdc(),
		&bmpColorBarhead,
		CBM_INIT,
		p_ColorBar,
		(BITMAPINFO *)&bmpColorBarhead,
		DIB_RGB_COLORS);
	m_colorBar.Attach(hbar);
	colorBarRect.left = 0;
	colorBarRect.right = width;
	colorBarRect.top = 0;
	colorBarRect.bottom = height;

	delete bar_am;
	delete bar_ang;
}
void CShowDlg::loadGrayBar()
{
	if (!m_grayBar.IsNull())
	{
		return;
	}
	if (p_GrayBar != NULL)
	{
		//UnloadGrayBar();
	}

#pragma region 生成colorbar
	//生成colorbar
	Mymatrix<double>* bar_ang_l = new Mymatrix<double>(1, 629, -PI);
	for (int i = 0; i < 629; i++)
	{
		bar_ang_l->Data[0][i] += double(0.01*i*1.0);
	}
	Mymatrix<double>*bar_am_l = new Mymatrix<double>(1, 101);
	for (int i = 0; i < 101; i++)
	{
		bar_am_l->Data[0][i] = double(0.01*i);
	}
	Mymatrix<double>*  one1 = new Mymatrix<double>(629, 1, 1);
	Mymatrix<double>*  one2 = new Mymatrix<double>(101, 1, 1);
	Mymatrix<double>*bar_am = matMultiply(*one1, *bar_am_l);
	Mymatrix<double>*bar_ang = matMultiply(*one2, *bar_ang_l);
	bar_ang->Transform();
	bar_am->Flipud();
	delete bar_am_l;
	delete bar_ang_l;
	delete one1;
	delete one2;
#pragma endregion

	int width = bar_ang->Col;
	int height = bar_ang->Row;
	double dis_max = PI;
	double dis_min = -PI;

	Mymatrix<double> h(bar_ang->Row, bar_ang->Col);
	for (int i = 0; i < bar_ang->Row; i++)
	{
		for (int j = 0; j < bar_ang->Col; j++)
		{
			h.Data[i][j] = double((bar_ang->Data[i][j] - dis_min) / (dis_max - dis_min));
		}
	}

	double max = h(0, 0), min = h(0, 0);
	for (int i = 0; i < h.Row; i++)
	{
		for (int j = 0; j < h.Col; j++)
		{
			if (max < h(i, j))
			{
				max = h(i, j);
			}
			else if (min > h(i, j))
			{
				min = h(i, j);
			}
		}
	}
	double part = (max - min) / 255.0;
	int bmpWidthBytes = ((width * 8 + 31) & ~31) / 8;
	p_GrayBar = new BYTE[bmpWidthBytes * height];
	for (int i = 0; i < height; i++)
	{
		int imgLineIndex = (height - i - 1)*bmpWidthBytes;
		for (int j = 0; j < width; j++)
		{
			p_GrayBar[imgLineIndex + j] = BYTE((h.Data[i][j] - min) / part);
		}
		for (int j = width; j < bmpWidthBytes; j++)
		{
			p_GrayBar[imgLineIndex + j] = 0;
		}
	}
	for (int i = 0; i < height; i++)
	{
		int imgLineIndex = i*bmpWidthBytes;
		for (int j = 0; j < width; j++)
		{
			if (bar_am->Data[i][j] >= 1)
			{
				bar_am->Data[i][j] = 1;
			}
			else if (bar_am->Data[i][j] <= 0)
			{
				bar_am->Data[i][j] = 0;
			}
			p_GrayBar[imgLineIndex + j] = BYTE(p_GrayBar[imgLineIndex + j] * bar_am->Data[i][j]);
		}
	}

	//创建位图资源
	struct {
		BITMAPINFOHEADER  bmiHeader;
		RGBQUAD			  bmiColors[256];
	}MyBitmapInfo;

	MyBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	MyBitmapInfo.bmiHeader.biWidth = width;
	MyBitmapInfo.bmiHeader.biHeight = height;
	MyBitmapInfo.bmiHeader.biPlanes = 1;
	MyBitmapInfo.bmiHeader.biBitCount = 8;
	MyBitmapInfo.bmiHeader.biCompression = BI_RGB;
	MyBitmapInfo.bmiHeader.biSizeImage = bmpWidthBytes*height;
	MyBitmapInfo.bmiHeader.biXPelsPerMeter = 0;
	MyBitmapInfo.bmiHeader.biYPelsPerMeter = 0;
	MyBitmapInfo.bmiHeader.biClrUsed = 0;
	MyBitmapInfo.bmiHeader.biClrImportant = 0;

	for (int i = 0; i < 256; i++)
	{
		MyBitmapInfo.bmiColors[i].rgbBlue = (BYTE)i;
		MyBitmapInfo.bmiColors[i].rgbGreen = (BYTE)i;
		MyBitmapInfo.bmiColors[i].rgbRed = (BYTE)i;
		MyBitmapInfo.bmiColors[i].rgbReserved = 0;
	}

	//生成bmp文件
	HBITMAP hbar = CreateDIBitmap(this->GetDC()->GetSafeHdc(),
		&MyBitmapInfo.bmiHeader,
		CBM_INIT,
		p_GrayBar,
		(BITMAPINFO *)&MyBitmapInfo,
		DIB_RGB_COLORS);
	m_grayBar.Attach(hbar);

	colorBarRect.left = 0;
	colorBarRect.right = width;
	colorBarRect.top = 0;
	colorBarRect.bottom = height;

	delete bar_am;
	delete bar_ang;
}

void CShowDlg::set_X_axis_Name(CString& name)
{
	dataInfo.nameX = name;
}
void CShowDlg::set_Y_axis_Name(CString& name)
{
	dataInfo.nameY = name;
}

void CShowDlg::setTitle(CString& title)
{
	dataInfo.title = title;
}

void CShowDlg::set_X_Y_range(double xmin, double xmax, double ymin, double ymax)
{
	dataInfo.minX = xmin;
	dataInfo.maxX = xmax;
	dataInfo.maxY = ymax;
	dataInfo.minY = ymin;
}

void CShowDlg::setColorbarrange(double min, double max)
{
	dataInfo.colorbarMax = max;
	dataInfo.colorbarMin = min;
}

//绘制图片坐标信息
void CShowDlg::PaintImageCoordinate(CMemDC &dc)
{
	int realwidth = exhibitRect1.right - exhibitRect1.left;//图片宽度
	int width = int((exhibitRect1.right - exhibitRect1.left)*int(dataInfo.maxX)*1.0 / dataInfo.maxX);
	int realheight = (exhibitRect1.bottom - exhibitRect1.top);	//图片实际高度
	int height = int((exhibitRect1.bottom - exhibitRect1.top)*int(dataInfo.maxY-dataInfo.minY)*1.0 / double(dataInfo.maxY-dataInfo.minY));//可用坐标长高度

																									  //坐标刻度线长
	int length = 5;

	TEXTMETRIC tm;	//字体信息
	dc.GetDC().GetTextMetrics(&tm);

#pragma region 绘坐标轴和非线性幅值框图

	//坐标轴
	dc.GetDC().MoveTo(exhibitRect1.left, exhibitRect1.top);
	dc.GetDC().LineTo(exhibitRect1.left, exhibitRect1.bottom);
	dc.GetDC().LineTo(exhibitRect1.right, exhibitRect1.bottom);

	dc.GetDC().MoveTo(exhibitRect2.left, exhibitRect2.top);
	dc.GetDC().LineTo(exhibitRect2.left, exhibitRect2.bottom);
	dc.GetDC().LineTo(exhibitRect2.right, exhibitRect2.bottom);

	dc.GetDC().MoveTo(exhibitRect3.left, exhibitRect3.top);
	dc.GetDC().LineTo(exhibitRect3.left, exhibitRect3.bottom);
	dc.GetDC().LineTo(exhibitRect3.right, exhibitRect3.bottom);

	//非线性幅值框图
	dc.GetDC().MoveTo(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth, exhibitRect1.top);
	dc.GetDC().LineTo(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth, exhibitRect1.top + clientSizeInfo.barWidth);
	dc.GetDC().LineTo(clientRect.right - clientSizeInfo.borderX2, exhibitRect1.top + clientSizeInfo.barWidth);

	dc.GetDC().MoveTo(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth, exhibitRect1.top);
	dc.GetDC().LineTo(clientRect.right - clientSizeInfo.borderX2, exhibitRect1.top);
	dc.GetDC().LineTo(clientRect.right - clientSizeInfo.borderX2, exhibitRect1.top + clientSizeInfo.barWidth);

	dc.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 5, exhibitRect1.top - tm.tmHeight / 2, _T("1"));
	dc.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 5, exhibitRect1.top + clientSizeInfo.barWidth / 2 - tm.tmHeight / 2, _T("0.5"));
	dc.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 5, exhibitRect1.top + clientSizeInfo.barWidth - tm.tmHeight / 2, _T("0"));

	dc.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth - tm.tmAveCharWidth / 2, exhibitRect1.top + clientSizeInfo.barWidth + 5, _T("0"));
	dc.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth / 2 - tm.tmAveCharWidth, exhibitRect1.top + clientSizeInfo.barWidth + 5, _T("0.5"));
	dc.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 - tm.tmAveCharWidth / 2, exhibitRect1.top + clientSizeInfo.barWidth + 5, _T("1"));

	dc.GetDC().Rectangle(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
		exhibitRect1.top + clientSizeInfo.barWidth / 2,
		clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth + 2,
		exhibitRect1.top + clientSizeInfo.barWidth / 2 + 2);
	dc.GetDC().Rectangle(clientRect.right - clientSizeInfo.borderX2 - 2,
		exhibitRect1.top + clientSizeInfo.barWidth / 2,
		clientRect.right - clientSizeInfo.borderX2,
		exhibitRect1.top + clientSizeInfo.barWidth / 2 + 2);
	dc.GetDC().Rectangle(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth / 2,
		exhibitRect1.top + clientSizeInfo.barWidth - 2,
		clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth / 2 + 2,
		exhibitRect1.top + clientSizeInfo.barWidth);
	dc.GetDC().Rectangle(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth / 2,
		exhibitRect1.top,
		clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth / 2 + 2,
		exhibitRect1.top + 2);
	if (status == SGLCOLOR || status == SGLGRAY)
	{
		CPen pen(PS_SOLID, 3, RGB(0, 0, 255));
		CPen *pOldPen = dc.GetDC().SelectObject(&pen);  //SelectObject的返回值是被替换的画笔的指针
		dc.GetDC().MoveTo(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
			exhibitRect1.top + clientSizeInfo.barWidth - 1);
		dc.GetDC().LineTo(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
			exhibitRect1.top + clientSizeInfo.barWidth - sglData.ampMin*clientSizeInfo.barWidth*1.0);
		dc.GetDC().LineTo(clientRect.right - clientSizeInfo.borderX2 - 2,
			exhibitRect1.top + clientSizeInfo.barWidth - sglData.ampMax*clientSizeInfo.barWidth*1.0);
		dc.GetDC().LineTo(clientRect.right - clientSizeInfo.borderX2,
			exhibitRect1.top);
		dc.GetDC().SelectObject(pOldPen);  //将先前画笔选择回设备描述表*/
	}
	else
	{
		CPen pen(PS_SOLID, 3, RGB(0, 0, 255));
		CPen *pOldPen = dc.GetDC().SelectObject(&pen);  //SelectObject的返回值是被替换的画笔的指针
		dc.GetDC().MoveTo(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
			exhibitRect1.top + clientSizeInfo.barWidth - 1);
		dc.GetDC().LineTo(clientRect.right - clientSizeInfo.borderX2 - clientSizeInfo.barWidth,
			exhibitRect1.top + clientSizeInfo.barWidth - multiData.ampMin*clientSizeInfo.barWidth*1.0);
		dc.GetDC().LineTo(clientRect.right - clientSizeInfo.borderX2 - 2,
			exhibitRect1.top + clientSizeInfo.barWidth - multiData.ampMax*clientSizeInfo.barWidth*1.0);
		dc.GetDC().LineTo(clientRect.right - clientSizeInfo.borderX2,
			exhibitRect1.top);
		dc.GetDC().SelectObject(pOldPen);  //将先前画笔选择回设备描述表*/
	}
#pragma endregion 绘坐标轴和非线性幅值框图
	CString ch;
	int chWidth = 0;//显示的字符串长度
					//画Y轴

	if (0 != dataInfo.nameY.GetLength())
	{
		CFont myFont;
		myFont.CreateFontW(tm.tmHeight*1.3, tm.tmAveCharWidth*1.3,
			900,
			0,
			10,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			_T("Arial"));

		CFont *pOldFont = dc.GetDC().SelectObject(&myFont);

		//y轴名
		dc.GetDC().TextOut(exhibitRect1.left - tm.tmHeight * 3.5,
			clientSizeInfo.borderY1 + realheight / 2 + dataInfo.nameY.GetLength()*tm.tmHeight / 2.5,
			dataInfo.nameY, dataInfo.nameY.GetLength());

		/*dc.GetDC().TextOut(exhibitRect2.left - tm.tmHeight * 3,
			clientSizeInfo.borderY1 + realheight / 2 + dataInfo.nameY.GetLength()*tm.tmAveCharWidth,
			dataInfo.nameY, dataInfo.nameY.GetLength());

		dc.GetDC().TextOut(exhibitRect3.left - tm.tmHeight * 3,
			clientSizeInfo.borderY1 + realheight / 2 + dataInfo.nameY.GetLength()*tm.tmAveCharWidth,
			dataInfo.nameY, dataInfo.nameY.GetLength());*/

		

		double partHeight = realheight / 4.0;
		for (int i = 0; i < 5; i++)
		{
			//绘制刻度线
			dc.GetDC().Rectangle(exhibitRect1.left - length,
				exhibitRect1.bottom - partHeight * i,
				exhibitRect1.left, exhibitRect1.bottom - partHeight * i + 2);
			dc.GetDC().Rectangle(exhibitRect2.left - length,
				exhibitRect2.bottom - partHeight * i,
				exhibitRect2.left, exhibitRect2.bottom - partHeight * i + 2);
			dc.GetDC().Rectangle(exhibitRect3.left - length,
				exhibitRect3.bottom - partHeight * i,
				exhibitRect3.left, exhibitRect3.bottom - partHeight * i + 2);
			//计算出刻度值
			double temp = double(dataInfo.minY + i*(dataInfo.maxY - dataInfo.minY)*1.0 / 4.0);
			ch.Format(_T("%.2g"), temp);
			chWidth = ch.GetLength()*tm.tmAveCharWidth;

			//标上刻度值
			dc.GetDC().TextOut(exhibitRect1.left - length - tm.tmHeight*1.5,
				exhibitRect1.bottom - partHeight*i + chWidth/2,
				ch, ch.GetLength());
			dc.GetDC().TextOut(exhibitRect2.left - length - tm.tmHeight*1.5,
				exhibitRect2.bottom - partHeight * i + chWidth/2,
				ch, ch.GetLength());
			dc.GetDC().TextOut(exhibitRect3.left - length - tm.tmHeight*1.5,
				exhibitRect3.bottom - partHeight * i + chWidth/2,
				ch, ch.GetLength());
		}
		dc.GetDC().SelectObject(pOldFont);
	}
	//画X轴
	if (0 != dataInfo.nameY.GetLength())
	{
		CFont myFont;
		myFont.CreateFontW(tm.tmHeight*1.3, tm.tmAveCharWidth*1.3,
			0,
			0,
			10,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			_T("Arial"));
		
		CFont *pOldFont = dc.GetDC().SelectObject(&myFont);
		if (status == MLTUNCOLOR || status == MLTUNGRAY)
		{

			dc.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 40,
				clientSizeInfo.borderY1 + clientSizeInfo.geap-10 ,
				_T("μm"), 2);
		}
		else
		{
			dc.GetDC().TextOut(clientRect.right - clientSizeInfo.borderX2 + 40,
				clientSizeInfo.borderY1 + clientSizeInfo.geap-10 ,
				_T("rad"), 3);
		}
		//x轴名
		dc.GetDC().TextOut(exhibitRect1.left + realwidth / 2 - dataInfo.nameX.GetLength() / 2 * tm.tmAveCharWidth,
			exhibitRect1.bottom + tm.tmAveCharWidth + length + 15,
			dataInfo.nameX, dataInfo.nameX.GetLength());
		dc.GetDC().TextOut(exhibitRect2.left + realwidth / 4 - dataInfo.nameX.GetLength() / 2 * tm.tmAveCharWidth,
			exhibitRect2.bottom + tm.tmAveCharWidth + length + 15,
			dataInfo.nameX, dataInfo.nameX.GetLength());
		dc.GetDC().TextOut(exhibitRect3.left + realwidth / 4 - dataInfo.nameX.GetLength() / 2 * tm.tmAveCharWidth,
			exhibitRect3.bottom + tm.tmAveCharWidth + length + 15,
			dataInfo.nameX, dataInfo.nameX.GetLength());

		//abc图标志
		dc.GetDC().TextOutW(exhibitRect1.left + realwidth / 2 - 8 * tm.tmAveCharWidth,
			exhibitRect1.bottom + tm.tmHeight + 40,
			_T("双通道原始数据"), 7);
		dc.GetDC().TextOutW(exhibitRect2.left + realwidth / 4 - 5 * tm.tmAveCharWidth,
			exhibitRect1.bottom + tm.tmHeight + 40,
			_T("切面位移"), 4);
		dc.GetDC().TextOutW(exhibitRect3.left + realwidth / 4 - 5 * tm.tmAveCharWidth,
			exhibitRect1.bottom + tm.tmHeight + 40,
			_T("离面位移"), 4);

		//标题
		dc.GetDC().TextOut((clientRect.right - clientRect.left) / 2 - dataInfo.title.GetLength()*1.5 * tm.tmAveCharWidth,
			clientSizeInfo.borderY1 - 50, dataInfo.title, dataInfo.title.GetLength());
		
		double partWidth = realwidth / 3.0;
		for (int i = 0; i < 4; i++)
		{
			//标x轴刻度
			dc.GetDC().Rectangle(exhibitRect1.left + partWidth * i, exhibitRect1.bottom + 1, 
				exhibitRect1.left + partWidth * i + 2, exhibitRect1.bottom + length + 1);

			//标刻度值
			ch.Format(_T("%.2g"), double(dataInfo.minX + i*1.0*(dataInfo.maxX - dataInfo.minX)/3.0));
			chWidth = ch.GetLength()*tm.tmAveCharWidth / 2;
			dc.GetDC().TextOut(exhibitRect1.left + partWidth * i - chWidth, 
				exhibitRect1.bottom + length + 2, ch, ch.GetLength());
		}
		double partWidth2 = (exhibitRect2.right - exhibitRect2.left)*1.0 / 2.0;
		for (int i = 0; i < 3; i++)
		{
			dc.GetDC().Rectangle(exhibitRect2.left + partWidth2 * i, 
				exhibitRect2.bottom + 1, exhibitRect2.left + partWidth2 * i + 2, 
				exhibitRect2.bottom + length + 1);
			dc.GetDC().Rectangle(exhibitRect3.left + partWidth2 * i, 
				exhibitRect3.bottom + 1, exhibitRect3.left + partWidth2 * i + 2, 
				exhibitRect3.bottom + length + 1);

			ch.Format(_T("%.2g"), double(dataInfo.minX + i*(dataInfo.maxX / 2.0 - dataInfo.minX)/2.0));
			chWidth = ch.GetLength()*tm.tmAveCharWidth / 2;

			dc.GetDC().TextOut(exhibitRect2.left + partWidth2 * i - chWidth, exhibitRect2.bottom + length + 2, ch, ch.GetLength());
			dc.GetDC().TextOut(exhibitRect3.left + partWidth2 * i - chWidth, exhibitRect3.bottom + length + 2, ch, ch.GetLength());
		}
		dc.GetDC().SelectObject(pOldFont);
	}
}

void CShowDlg::saveImg(CString picformat)
{
	CString PicName;
	switch (status)
	{
	case CShowDlg::NONE:
		break;
	case CShowDlg::SGLCOLOR:
	{
		PicName.Format(_T("%d&%d %s%s"), sglData.img1num, sglData.img2num, _T("单组彩色结果图."), picformat);
		CDC *pdc = GetDC();
		CDC dstDC;
		dstDC.CreateCompatibleDC(pdc);
		CImage img;
		img.Create(clientRect.right, clientRect.bottom, 24);
		dstDC.SelectObject((HBITMAP)img);
		dstDC.BitBlt(0, 0, clientRect.right, clientRect.bottom, pdc, 0, 0, SRCCOPY);
		img.Save(PicName);
		MessageBox(_T("保存成功"));
		break;
	}

	case CShowDlg::SGLGRAY:
	{
		PicName.Format(_T("%d&%d %s%s"), sglData.img1num, sglData.img2num, _T("单组灰度结果图."), picformat);
		Graphics g();
		CDC *pdc = GetDC();
		CDC dstDC;
		dstDC.CreateCompatibleDC(pdc);
		CImage img;
		img.Create(clientRect.right, clientRect.bottom, 24);
		dstDC.SelectObject((HBITMAP)img);
		dstDC.BitBlt(0, 0, clientRect.right, clientRect.bottom, pdc, 0, 0, SRCCOPY);
		img.Save(PicName);
		MessageBox(_T("保存成功"));
		break;
	}
	case CShowDlg::MLTUNCOLOR:
	{
		CString filename;
		PicName.Format(_T("%s\\%d_%d_动态解卷绕彩色图.gif"), folder_dir, multiData.imgStartNum, multiData.imgEndNum);
		CGifEncoder gifEncoder;
		gifEncoder.SetFrameSize(clientRect.right, clientRect.bottom);
		gifEncoder.SetDelayTime(kImagefreshrate);
		filename.Format(_T("%s\\%d_%d_动态解卷绕彩色图.gif"), folder_dir, multiData.imgStartNum, multiData.imgEndNum);
		wstring str = filename.GetString();
		gifEncoder.StartEncoder(str);
		for (int i = 0; i < unPhaseColorVec1.size(); i++)
		{
			gifEncoder.AddFrame(&Colorimg[i]);
		}
		gifEncoder.FinishEncoder();
		MessageBox(_T("保存成功"));
		break;
	}
	case CShowDlg::MLTUNGRAY:
	{
		CString filename;
		PicName.Format(_T("%s\\%d_%d_动态解卷绕灰度图.gif"), folder_dir, multiData.imgStartNum, multiData.imgEndNum);
		CGifEncoder gifEncoder;
		gifEncoder.SetFrameSize(clientRect.right, clientRect.bottom);
		gifEncoder.SetDelayTime(kImagefreshrate);
		filename.Format(_T("%s\\%d_%d_动态解卷绕灰度图.gif"), folder_dir, multiData.imgStartNum, multiData.imgEndNum);
		wstring str = filename.GetString();
		gifEncoder.StartEncoder(str);
		for (int i = 0; i < unPhaseGrayVec1.size(); i++)
		{
			gifEncoder.AddFrame(&Grayimg[i]);
		}
		gifEncoder.FinishEncoder();
		MessageBox(_T("保存成功"));
		break;
	}
	case CShowDlg::MLTCOLOR:
	{
		CString filename;
		CGifEncoder gifEncoder;
		gifEncoder.SetFrameSize(clientRect.right, clientRect.bottom);
		gifEncoder.SetDelayTime(kImagefreshrate);
		filename.Format(_T("%s\\%d_%d_动态卷绕彩色图.gif"), folder_dir, multiData.imgStartNum, multiData.imgEndNum);
		wstring str = filename.GetString();
		gifEncoder.StartEncoder(str);
		for (int i = 0; i < PhaseColorVec1.size(); i++)
		{
			//gifEncoder.AddFrame((BITMAPINFO*)&Colorbmphead, unPhaseColorImg[i]);
			gifEncoder.AddFrame(&Colorimg[i]);
		}
		gifEncoder.FinishEncoder();
		MessageBox(_T("保存成功"));
		break;
	}
	case CShowDlg::MLTGRAY:
	{
		CString filename;
		CGifEncoder gifEncoder;
		gifEncoder.SetFrameSize(clientRect.right, clientRect.bottom);
		gifEncoder.SetDelayTime(kImagefreshrate);
		filename.Format(_T("%s\\%d_%d_动态卷绕灰度图.gif"), folder_dir, multiData.imgStartNum, multiData.imgEndNum);
		wstring str = filename.GetString();
		gifEncoder.StartEncoder(str);
		for (int i = 0; i < PhaseGrayVec1.size(); i++)
		{
			gifEncoder.AddFrame(&Grayimg[i]);
		}
		gifEncoder.FinishEncoder();
		MessageBox(_T("保存成功"));
		break;
	}
	default:
		break;
	}
}

void CShowDlg::showSglColor()
{
	if (singleColorImg1.IsNull())
	{
		MessageBox(_T("不存在此组数据彩色图片！请先执行分析"), _T("警告"));
		return;
	}
	CShowDlg::getInstance()->setColorbarrange(-PI, PI);
	CShowDlg::getInstance()->set_X_Y_range(9.65*sglData.freCount / 694 * ((area.startCol-1)*1.0 / sglData.freCount),
		9.65*sglData.freCount / 694 * (area.endCol*1.0 / sglData.freCount),
		8.4*(1 - area.endRow*1.0 / 1038),
		8.4*(1 - (area.startRow - 1)*1.0 / 1038));	
	CShowDlg::getInstance()->setTitle(CString(_T("卷绕相位静态彩图")));

	status = SGLCOLOR;
	RedrawWindow();
}
void CShowDlg::showSglGray()
{
	if (singleGrayImg1.IsNull())
	{
		MessageBox(_T("不存在此组数据灰度图片！请先执行分析~"), _T("警告"));
		return;
	}
	CShowDlg::getInstance()->setColorbarrange(-PI, PI);
	CShowDlg::getInstance()->set_X_Y_range(9.65*sglData.freCount / 694 * ((area.startCol-1)*1.0 / sglData.freCount),
		9.65*sglData.freCount / 694 * (area.endCol*1.0 / sglData.freCount),
		8.4*(1 - area.endRow*1.0 / 1038),
		8.4*(1 - (area.startRow - 1)*1.0 / 1038));
	CShowDlg::getInstance()->setTitle(CString(_T("卷绕相位静态灰度图")));
	CShowDlg::getInstance()->loadGrayBar();

	status = SGLGRAY;
	RedrawWindow();
}

void CShowDlg::showPhaseColor(int i)
{
	CMemDC dcmem(*(this->GetDC()), destrc);
	dcmem.GetDC().SetStretchBltMode(COLORONCOLOR);
	dcmem.GetDC().FillSolidRect(&clientRect, RGB(255, 255, 255));

	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect1.left,
		exhibitRect1.top,
		exhibitRect1.right - exhibitRect1.left,
		exhibitRect1.bottom - exhibitRect1.top,
		0, 0, imgRect1.right, imgRect1.bottom,
		PhaseColorImg1[i], (BITMAPINFO*)&Colorbmphead1,
		DIB_RGB_COLORS, SRCCOPY
	);// 1
	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect2.left,
		exhibitRect2.top,
		exhibitRect2.right - exhibitRect2.left,
		exhibitRect2.bottom - exhibitRect2.top,
		0, 0, imgRect2.right, imgRect2.bottom,
		PhaseColorImg2[i], (BITMAPINFO*)&Colorbmphead2,
		DIB_RGB_COLORS, SRCCOPY
	);// 2
	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect3.left,
		exhibitRect3.top,
		exhibitRect3.right - exhibitRect3.left,
		exhibitRect3.bottom - exhibitRect3.top,
		0, 0, imgRect3.right, imgRect3.bottom,
		PhaseColorImg3[i], (BITMAPINFO*)&Colorbmphead2,
		DIB_RGB_COLORS, SRCCOPY
	);// 3
	PaintImageCoordinate(dcmem);
	Sleep(kImagefreshrate);
	CDC *pdc = GetDC();
	CDC dstDC;
	dstDC.CreateCompatibleDC(pdc);
	if (!Colorimg[i].IsNull())
	{
		Colorimg[i].Destroy();
	}
	Colorimg[i].Create(clientRect.right, clientRect.bottom, 24);
	dstDC.SelectObject((HBITMAP)Colorimg[i]);
	dstDC.BitBlt(0, 0, clientRect.right, clientRect.bottom, pdc, 0, 0, SRCCOPY);
}
void CShowDlg::showUnPhaseGray(int i)
{
	CMemDC dcmem(*(this->GetDC()), destrc);
	dcmem.GetDC().SetStretchBltMode(COLORONCOLOR);
	dcmem.GetDC().FillSolidRect(&clientRect, RGB(255, 255, 255));

	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect1.left,
		exhibitRect1.top,
		exhibitRect1.right - exhibitRect1.left,
		exhibitRect1.bottom - exhibitRect1.top,
		0, 0, imgRect1.right, imgRect1.bottom,
		unPhaseGrayImg1[i], (BITMAPINFO*)&GrayBitmapInfo1,
		DIB_RGB_COLORS, SRCCOPY
	);// 1
	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect2.left,
		exhibitRect2.top,
		exhibitRect2.right - exhibitRect2.left,
		exhibitRect2.bottom - exhibitRect2.top,
		0, 0, imgRect2.right, imgRect2.bottom,
		unPhaseGrayImg2[i], (BITMAPINFO*)&GrayBitmapInfo2,
		DIB_RGB_COLORS, SRCCOPY
	);// 2
	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect3.left,
		exhibitRect3.top,
		exhibitRect3.right - exhibitRect3.left,
		exhibitRect3.bottom - exhibitRect3.top,
		0, 0, imgRect3.right, imgRect3.bottom,
		unPhaseGrayImg3[i], (BITMAPINFO*)&GrayBitmapInfo2,
		DIB_RGB_COLORS, SRCCOPY
	);// 3
	PaintImageCoordinate(dcmem);
	Sleep(kImagefreshrate);
	CDC *pdc = GetDC();
	CDC dstDC;
	dstDC.CreateCompatibleDC(pdc);
	if (!Grayimg[i].IsNull())
	{
		Grayimg[i].Destroy();
	}
	Grayimg[i].Create(clientRect.right, clientRect.bottom, 24);
	dstDC.SelectObject((HBITMAP)Grayimg[i]);
	dstDC.BitBlt(0, 0, clientRect.right, clientRect.bottom, pdc, 0, 0, SRCCOPY);
}
void CShowDlg::showUnPhaseColor(int i)
{
	CMemDC dcmem(*(this->GetDC()), destrc);
	dcmem.GetDC().SetStretchBltMode(COLORONCOLOR);
	dcmem.GetDC().FillSolidRect(&clientRect, RGB(255, 255, 255));
	PaintImageCoordinate(dcmem);

	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect1.left,
		exhibitRect1.top,
		exhibitRect1.right - exhibitRect1.left,
		exhibitRect1.bottom - exhibitRect1.top,
		0, 0, imgRect1.right, imgRect1.bottom,
		unPhaseColorImg1[i], (BITMAPINFO*)&Colorbmphead1,
		DIB_RGB_COLORS, SRCCOPY
	);// 1
	//PaintImageCoordinate(dcmem);
	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect2.left,
		exhibitRect2.top,
		exhibitRect2.right - exhibitRect2.left,
		exhibitRect2.bottom - exhibitRect2.top,
		0, 0, imgRect2.right, imgRect2.bottom,
		unPhaseColorImg2[i], (BITMAPINFO*)&Colorbmphead2,
		DIB_RGB_COLORS, SRCCOPY
	);// 2
	//PaintImageCoordinate(dcmem);
	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect3.left,
		exhibitRect3.top,
		exhibitRect3.right - exhibitRect3.left,
		exhibitRect3.bottom - exhibitRect3.top,
		0, 0, imgRect3.right, imgRect3.bottom,
		unPhaseColorImg3[i], (BITMAPINFO*)&Colorbmphead2,
		DIB_RGB_COLORS, SRCCOPY
	);// 3
	
	Sleep(kImagefreshrate);
	CDC *pdc = GetDC();
	CDC dstDC;
	dstDC.CreateCompatibleDC(pdc);
	if (!Colorimg[i].IsNull())
	{
		Colorimg[i].Destroy();
	}
	Colorimg[i].Create(clientRect.right, clientRect.bottom, 24);
	dstDC.SelectObject((HBITMAP)Colorimg[i]);
	dstDC.BitBlt(0, 0, clientRect.right, clientRect.bottom, pdc, 0, 0, SRCCOPY);
}
void CShowDlg::showPhaseGray(int i)
{
	CMemDC dcmem(*(this->GetDC()), destrc);
	dcmem.GetDC().SetStretchBltMode(COLORONCOLOR);
	dcmem.GetDC().FillSolidRect(&clientRect, RGB(255, 255, 255));

	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect1.left,
		exhibitRect1.top,
		exhibitRect1.right - exhibitRect1.left,
		exhibitRect1.bottom - exhibitRect1.top,
		0, 0, imgRect1.right, imgRect1.bottom,
		PhaseGrayImg1[i], (BITMAPINFO*)&GrayBitmapInfo1,
		DIB_RGB_COLORS, SRCCOPY
	);// 1
	//PaintImageCoordinate(dcmem);
	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect2.left,
		exhibitRect2.top,
		exhibitRect2.right - exhibitRect2.left,
		exhibitRect2.bottom - exhibitRect2.top,
		0, 0, imgRect2.right, imgRect2.bottom,
		PhaseGrayImg2[i], (BITMAPINFO*)&GrayBitmapInfo2,
		DIB_RGB_COLORS, SRCCOPY
	);// 2
	//PaintImageCoordinate(dcmem);
	::StretchDIBits
	(dcmem.GetDC().GetSafeHdc(),
		exhibitRect3.left,
		exhibitRect3.top,
		exhibitRect3.right - exhibitRect3.left,
		exhibitRect3.bottom - exhibitRect3.top,
		0, 0, imgRect3.right, imgRect3.bottom,
		PhaseGrayImg3[i], (BITMAPINFO*)&GrayBitmapInfo2,
		DIB_RGB_COLORS, SRCCOPY
	);// 3
	PaintImageCoordinate(dcmem);
	Sleep(kImagefreshrate);
	CDC *pdc = GetDC();
	CDC dstDC;
	dstDC.CreateCompatibleDC(pdc);
	if (!Grayimg[i].IsNull())
	{
		Grayimg[i].Destroy();
	}
	Grayimg[i].Create(clientRect.right, clientRect.bottom, 24);
	dstDC.SelectObject((HBITMAP)Grayimg[i]);
	dstDC.BitBlt(0, 0, clientRect.right, clientRect.bottom, pdc, 0, 0, SRCCOPY);
}

UINT showPhaseColorThead(LPVOID pParam)
{
	CShowDlg::getInstance()->status = CShowDlg::MLTCOLOR;
	CShowDlg::getInstance()->loadColorBar();
	CShowDlg::getInstance()->setColorbarrange(-PI, PI);
	CShowDlg::getInstance()->set_X_Y_range(9.65*multiData.freCount / 694 * ((area.startCol-1)*1.0 / multiData.freCount),
		9.65*multiData.freCount / 694 * (area.endCol*1.0 / multiData.freCount),
		8.4*(1 - area.endRow*1.0 / 1038),
		8.4*(1 - (area.startRow - 1)*1.0 / 1038));
	CShowDlg::getInstance()->setTitle(CString(_T("卷绕相位动态彩图")));
	CShowDlg::getInstance()->RedrawWindow();


	for (int i = 0; i < PhaseColorVec1.size(); i++)
	{
		if (CShowDlg::getInstance()->status != CShowDlg::MLTCOLOR)
			return 0;
		CShowDlg::getInstance()->showPhaseColor(i);
	}
	
	return 0;
}
UINT showUnPhaseGrayThead(LPVOID pParam)
{
	CShowDlg::getInstance()->status = CShowDlg::MLTUNGRAY;
	CShowDlg::getInstance()->loadGrayBar();
	CShowDlg::getInstance()->setColorbarrange(multiData.umMin, multiData.umMax);
	CShowDlg::getInstance()->set_X_Y_range(9.65*multiData.freCount / 694 * ((area.startCol-1)*1.0 / multiData.freCount),
		9.65*multiData.freCount / 694 * (area.endCol*1.0 / multiData.freCount),
		8.4*(1 - area.endRow*1.0 / 1038),
		8.4*(1 - (area.startRow - 1)*1.0 / 1038)); 
	CShowDlg::getInstance()->setTitle(CString(_T("变形场动态灰度图")));
	CShowDlg::getInstance()->RedrawWindow();
	
	for (int i = 0; i < unPhaseGrayVec1.size(); i++)
	{
		if (CShowDlg::getInstance()->status != CShowDlg::MLTUNGRAY)
			return 0;
		CShowDlg::getInstance()->showUnPhaseGray(i);
	}
	return 0;
}
UINT showUnPhaseColorThead(LPVOID pParam)
{
	CShowDlg::getInstance()->status = CShowDlg::getInstance()->MLTUNCOLOR;
	CShowDlg::getInstance()->loadColorBar();
	CShowDlg::getInstance()->setColorbarrange(multiData.umMin, multiData.umMax);
	CShowDlg::getInstance()->set_X_Y_range(9.65*multiData.freCount / 694 * ((area.startCol-1)*1.0 / multiData.freCount),
		9.65*multiData.freCount / 694 * (area.endCol*1.0 / multiData.freCount),
		8.4*(1 - area.endRow*1.0 / 1038),
		8.4*(1 - (area.startRow - 1)*1.0 / 1038));
	CShowDlg::getInstance()->setTitle(CString(_T("变形场动态彩图")));
	CShowDlg::getInstance()->RedrawWindow();
	

	for (int i = 0; i < unPhaseColorVec1.size(); i++)
	{
		if (CShowDlg::getInstance()->status != CShowDlg::MLTUNCOLOR)
			return 0;
		CShowDlg::getInstance()->showUnPhaseColor(i);
	}
	
	return 0;
}
UINT showPhaseGrayThead(LPVOID pParam)
{
	CShowDlg::getInstance()->status = CShowDlg::MLTGRAY;
	CShowDlg::getInstance()->loadGrayBar();
	CShowDlg::getInstance()->setColorbarrange(-PI, PI);
	CShowDlg::getInstance()->set_X_Y_range(9.65*multiData.freCount / 694 * ((area.startCol-1)*1.0 / multiData.freCount),
		9.65*multiData.freCount / 694 * (area.endCol*1.0 / multiData.freCount),
		8.4*(1 - area.endRow*1.0 / 1038),
		8.4*(1 - (area.startRow - 1)*1.0 / 1038));
	CShowDlg::getInstance()->setTitle(CString(_T("卷绕相位动态灰度图")));
	CShowDlg::getInstance()->RedrawWindow();

	for (int i = 0; i < PhaseGrayVec1.size(); i++)
	{
		if (CShowDlg::getInstance()->status != CShowDlg::MLTGRAY)
			return 0;
		CShowDlg::getInstance()->showPhaseGray(i);
	}
	
	return 0;
}

void CShowDlg::MenuCheck()
{
	if (!singleColorImg1.IsNull())
		bSglMenuShow = true;
	else
		bSglMenuShow = false;
	if (!unPhaseColorVec1.empty())
		bMultiUnwrapMenuShow = true;
	if (!PhaseColorVec1.empty())
		bMultiWrapMenuShow = true;
	if (status == SGLCOLOR || status == SGLGRAY)
		bSaveSgl = true;
	else
		bSaveSgl = false;
	if (status == MLTCOLOR || status == MLTGRAY || status == MLTUNCOLOR || status == MLTUNGRAY)
		bSaveMulti = true;
	else
		bSaveMulti = false;
}

void CShowDlg::PlayOnce()
{
	switch (status)
	{
	case CShowDlg::NONE:
		break;
	case CShowDlg::SGLCOLOR:
		showSglColor();
		break;
	case CShowDlg::SGLGRAY:
		showSglGray();
		break;
	case CShowDlg::MLTUNCOLOR:
		showMultiUnwrapColor();
		break;
	case CShowDlg::MLTUNGRAY:
		showMultiUnwrapGray();
		break;
	case CShowDlg::MLTCOLOR:
		showMultiWrapColor();
		break;
	case CShowDlg::MLTGRAY:
		showMultiWrapGray();
		break;
	default:
		break;
	}
}

void CShowDlg::UpdateRect()
{
	GetClientRect(&clientRect);
	RedrawWindow();
}

void CShowDlg::showMultiUnwrapColor()
{
	status = MLTUNCOLOR;
	AfxBeginThread(showUnPhaseColorThead, NULL);
}
void CShowDlg::showMultiUnwrapGray()
{
	status = MLTUNGRAY;
	AfxBeginThread(showUnPhaseGrayThead, NULL);
}

void CShowDlg::showMultiWrapColor()
{
	status = MLTCOLOR;
	AfxBeginThread(showPhaseColorThead, NULL);
}
void CShowDlg::showMultiWrapGray()
{
	status = MLTGRAY;
	AfxBeginThread(showPhaseGrayThead, NULL);
}

void CShowDlg::reset(bool is_unwrap)
{
	setHeader();
	if (is_unwrap)
	{
		unPhaseGrayImgNum1 = 0;
		unPhaseColorImgNum1 = 0;
		unPhaseGrayImgNum2 = 0;
		unPhaseColorImgNum2 = 0;
		unPhaseGrayImgNum3 = 0;
		unPhaseColorImgNum3 = 0;
		status = MLTUNCOLOR;
		if (!unPhaseColorVec1.empty())
		{
			for (int i = 0; i < unPhaseColorVec1.size(); i++)
			{
				delete[]unPhaseColorVec1[i];
			}
			for (int i = 0; i < unPhaseColorVec2.size(); i++)
			{
				delete[]unPhaseColorVec2[i];
			}
			for (int i = 0; i < unPhaseColorVec3.size(); i++)
			{
				delete[]unPhaseColorVec3[i];
			}
		}
		unPhaseColorVec1.clear();
		unPhaseColorVec2.clear();
		unPhaseColorVec3.clear();

		if (!unPhaseGrayVec1.empty())
		{
			for (int i = 0; i < unPhaseGrayVec1.size(); i++)
			{
				delete[] unPhaseGrayVec1[i];
			}
			for (int i = 0; i < unPhaseGrayVec2.size(); i++)
			{
				delete[] unPhaseGrayVec2[i];
			}
			for (int i = 0; i < unPhaseGrayVec3.size(); i++)
			{
				delete[] unPhaseGrayVec3[i];
			}
		}
		unPhaseGrayVec1.clear();
		unPhaseGrayVec2.clear();
		unPhaseGrayVec3.clear();
	}
	else
	{
		PhaseGrayImgNum1 = 0;
		PhaseColorImgNum1 = 0;
		PhaseGrayImgNum2 = 0;
		PhaseColorImgNum2 = 0;
		PhaseGrayImgNum3 = 0;
		PhaseColorImgNum3 = 0;
		status = MLTCOLOR;
		if (!PhaseColorVec1.empty())
		{
			for (int i = 0; i < PhaseColorVec1.size(); i++)
			{
				delete[]PhaseColorVec1[i];
			}
			for (int i = 0; i < PhaseColorVec2.size(); i++)
			{
				delete[]PhaseColorVec2[i];
			}
			for (int i = 0; i < PhaseColorVec3.size(); i++)
			{
				delete[]PhaseColorVec3[i];
			}
		}
		PhaseColorVec1.clear();
		PhaseColorVec2.clear();
		PhaseColorVec3.clear();

		if (!PhaseGrayVec1.empty())
		{
			for (int i = 0; i < PhaseGrayVec1.size(); i++)
			{
				delete[] PhaseGrayVec1[i];
			}
			for (int i = 0; i < PhaseGrayVec2.size(); i++)
			{
				delete[] PhaseGrayVec2[i];
			}
			for (int i = 0; i < PhaseGrayVec3.size(); i++)
			{
				delete[] PhaseGrayVec3[i];
			}
		}
		PhaseGrayVec1.clear();
		PhaseGrayVec2.clear();
		PhaseGrayVec3.clear();
	}

	
}
void CShowDlg::sglreset()
{
	setHeader();
	if (!singleColorImg1.IsNull())
	{
		singleColorImg1.Destroy();
		singleColorImg2.Destroy();
		singleColorImg3.Destroy();
	}
	if (!singleGrayImg1.IsNull())
	{
		singleGrayImg1.Destroy();
		singleGrayImg2.Destroy();
		singleGrayImg3.Destroy();
	}
	status = SGLCOLOR;
}

afx_msg void CShowDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_colorBar.IsNull())
		return;
	if (status!=SGLCOLOR&&status!=SGLGRAY)
	{
		return;
	}
	m_pXY = GetDlgItem(IDC_SHOW_XY);
	m_pXY->GetWindowRect(&xyWndRect);
	m_pXY->ShowWindow(SW_HIDE);
	//判断坐标是否在图片上面
	POINT ptOrg1 = { exhibitRect1.left,exhibitRect1.top };
	POINT ptOrg2 = { exhibitRect2.left,exhibitRect2.top };
	POINT ptOrg3 = { exhibitRect3.left,exhibitRect3.top };
	if (point.x < ptOrg1.x || (point.x > exhibitRect1.right&&point.x < exhibitRect2.left) ||
		(point.x > exhibitRect2.right&&point.x < exhibitRect3.left) || point.x > exhibitRect3.right)
		return;
	if (point.y < ptOrg1.y || point.y >exhibitRect1.bottom)
		return;

	LONG width = xyWndRect.right - xyWndRect.left;
	LONG height = xyWndRect.bottom - xyWndRect.top;
	POINT pt = point;
	if (((point.x + width > exhibitRect1.right) && (point.x <= exhibitRect1.right)) ||
		((point.x + width > exhibitRect2.right) && (point.x <= exhibitRect2.right)) ||
		((point.x + width > exhibitRect3.right) && (point.x >= exhibitRect3.left)))
		pt.x -= width;

	if (point.y + height > exhibitRect1.bottom)
		pt.y -= height;
	m_pXY->MoveWindow(pt.x, pt.y, width, height);
	double dataX;
	if (point.x >= exhibitRect1.left&&point.x <= exhibitRect1.right)
	{
		dataX = dataInfo.minX+(point.x - ptOrg1.x)*1.0*(dataInfo.maxX - dataInfo.minX) / double((exhibitRect1.right - exhibitRect1.left));
	}
	else if (point.x >= exhibitRect2.left&&point.x <= exhibitRect2.right)
	{
		dataX = dataInfo.minX + (point.x - ptOrg2.x)*1.0*(dataInfo.maxX / 2 - dataInfo.minX) / double((exhibitRect2.right - exhibitRect2.left));
	}
	else
	{
		dataX = dataInfo.minX + (point.x - ptOrg3.x)*1.0*(dataInfo.maxX / 2 - dataInfo.minX) / double((exhibitRect3.right - exhibitRect3.left));
	}
	double dataY = (exhibitRect1.bottom - exhibitRect1.top - point.y + ptOrg1.y)*1.0*(dataInfo.maxY - dataInfo.minY) / double((exhibitRect1.bottom - exhibitRect1.top))+ dataInfo.minY;

	CString strXY;
	strXY.Format(_T("X = %.3lf\nY = %.3lf"), dataX, dataY);

	//m_pXY->SetWindowTextW(strXY);
	GetDlgItem(IDC_SHOW_XY)->SetWindowText(strXY);
	m_pXY->ShowWindow(SW_SHOW);
	CDialog::OnLButtonDown(nFlags, point);
}
afx_msg void CShowDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_pXY = GetDlgItem(IDC_SHOW_XY);
	m_pXY->GetWindowRect(&xyWndRect);
	m_pXY->ShowWindow(SW_HIDE);
	CDialog::OnLButtonDblClk(nFlags, point);
}