// SingleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Integratedbak.h"
#include "SingleDlg.h"
#include "ShowDlg.h"
#include "TipsDlg.h"
#include "afxdialogex.h"
#include "Myresource.h"
#include <io.h>

CWnd* mSingleWnd;
SINGLEDATA sglData;
extern AREA area;
bool CSingleDlg::bRunning = false;
CString dir = GetWorkingDir();
CString img_init_path;
CString folder_dir;

CString temp_filename1;
CString temp_filename2;
CString temp_width;
CString temp_height;
CString temp_foldername;

extern CString folder_name;
extern CString filename_prefix;

// CSingleDlg 对话框

IMPLEMENT_DYNAMIC(CSingleDlg, CDialogEx)

CSingleDlg::CSingleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_Analysis_Single, pParent)
{
	img_init_path.Format(_T("%s\\Data\\"), dir);
}

CSingleDlg::~CSingleDlg()
{
}

void CSingleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSingleDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BN_LOAD1, &CSingleDlg::OnBnClickedBnLoadImg1)
	ON_BN_CLICKED(IDC_BN_LOAD2, &CSingleDlg::OnBnClickedBnLoadImg2)
	ON_BN_CLICKED(IDC_BN_LOAD3, &CSingleDlg::OnBnClickedBnLoadBgS)
	ON_BN_CLICKED(IDC_BN_RUN, &CSingleDlg::OnBnClickedBnRun)
	ON_BN_CLICKED(IDC_BN_LOAD4, &CSingleDlg::OnBnClickedBnLoadFolderS)

END_MESSAGE_MAP()


// CSingleDlg 消息处理程序


BOOL CSingleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetBackgroundColor(RGB(255, 255, 255));

	mSingleWnd = this;

	GetDlgItem(IDC_EDIT_BANDWIDTH)->SetWindowText(_T("25"));
	GetDlgItem(IDC_EDIT_MDWAVELENGTH)->SetWindowText(_T("840"));
	GetDlgItem(IDC_EDIT_COLORMIN)->SetWindowText(_T("0.03"));
	GetDlgItem(IDC_EDIT_COLORMAX)->SetWindowText(_T("0.08"));
	GetDlgItem(IDC_EDIT_SGLFRECOUNT)->SetWindowText(_T("694"));
	GetDlgItem(IDC_EDIT_BG_S)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_SGLSR)->SetWindowText(_T("1"));
	GetDlgItem(IDC_EDIT_SGLSC)->SetWindowText(_T("20"));
	GetDlgItem(IDC_EDIT_SGLER)->SetWindowText(_T("800"));
	GetDlgItem(IDC_EDIT_SGLEC)->SetWindowText(_T("299"));
	GetDlgItem(IDC_EDIT_SGLOFFSET)->SetWindowText(_T("-5"));

	bg_dir = GetWorkingDir() + _T("\\Data\\");
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CSingleDlg::OnBnClickedBnLoadImg1()
{
	CString filename;
	CFileDialog filedlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR(_T("二进制文件(*.bin)|*.bin|All Files(*.*)|*.*||")), NULL);
	filedlg.m_ofn.lpstrInitialDir = img_init_path;
	if (filedlg.DoModal() == IDOK)
	{
		filename = filedlg.GetFileName();
		img1_dir = filename;
		filename = filename.Left(filename.GetLength() - 4);
		GetDlgItem(IDC_EDIT_IMG1)->SetWindowText(filename);
	}
}

void CSingleDlg::OnBnClickedBnLoadImg2()
{
	CString filename;
	CFileDialog filedlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR(_T("二进制文件(*.bin)|*.bin|All Files(*.*)|*.*||")), NULL);
	filedlg.m_ofn.lpstrInitialDir = img_init_path;
	if (filedlg.DoModal() == IDOK)
	{
		filename = filedlg.GetFileName();
		img2_dir = filename;
		filename = filename.Left(filename.GetLength() - 4);
		GetDlgItem(IDC_EDIT_IMG2)->SetWindowText(filename);
	}
}

void CSingleDlg::OnBnClickedBnLoadBgS()
{
	CString filename;
	CFileDialog filedlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR(_T("二进制文件(*.bin)|*.bin|All Files(*.*)|*.*||")), NULL);
	CString path;
	path.Format(_T("%s\\Data\\"), dir);
	filedlg.m_ofn.lpstrInitialDir = path;
	if (filedlg.DoModal() == IDOK)
	{
		filename = filedlg.GetFileName();
		bg_dir = filedlg.GetPathName();
		bg_dir.Replace(filename, _T(""));
		filename = filename.Left(filename.GetLength() - 4);
		GetDlgItem(IDC_EDIT_BG_S)->SetWindowText(filename);
	}
}

void CSingleDlg::OnBnClickedBnLoadFolderS()
{
	CString filename;
	CString path;
	path.Format(_T("%s\\Data\\"), dir);
	CFolderPickerDialog folderdlg(path);
	filename = folderdlg.GetFolderPath();
	if (folderdlg.DoModal() == IDOK)
	{
		filename = folderdlg.GetFileName();
		folder_name = filename;
		folder_dir = folderdlg.GetPathName();
		GetDlgItem(IDC_EDIT_FolderNameS)->SetWindowText(filename);
		img_init_path.Format(_T("%s\\"), folder_dir);
		CString tmp;
		tmp.Format(_T("%s%d"), filename_prefix, 1);
		GetDlgItem(IDC_EDIT_IMG1)->SetWindowTextW(tmp);
		tmp.Format(_T("%s%d"), filename_prefix, 2);
		GetDlgItem(IDC_EDIT_IMG2)->SetWindowTextW(tmp);
	}
}


void CSingleDlg::OnBnClickedBnRun()
{
	if (bRunning)
	{
		MessageBox(_T("数据正在分析中，请稍等"), _T("警告"));
		return;
	}
	static CString bgStr("");
	static int num3 = 0, num4 = 0, num5 = 0,num8=0,num9=0,num10=0,num11=0,num12=0;
	static double num6 = 0, num7 = 0;
	CString cstr[13];
	GetDlgItem(IDC_EDIT_IMG1)->GetWindowText(cstr[0]);
	GetDlgItem(IDC_EDIT_IMG2)->GetWindowText(cstr[1]);
	CString tmp[2];
	/*GetDlgItem(IDC_EDIT_Target_width)->GetWindowTextW(tmp[0]);
	GetDlgItem(IDC_EDIT_Target_Height)->GetWindowTextW(tmp[1]);
	CDASDlg::GetInstance()->Target_Width = _wtoi(tmp[0]);
	CDASDlg::GetInstance()->Target_Height = _wtoi(tmp[1]);
	sglData.imgCol = CDASDlg::GetInstance()->Target_Width;
	sglData.imgRow = CDASDlg::GetInstance()->Target_Height;*/
	sglData.img1.Format(_T("%s\\%s.bin"), folder_dir, cstr[0]);
	sglData.img2.Format(_T("%s\\%s.bin"), folder_dir, cstr[1]);

	for (int i = 0; i < 2; i++)
	{
		cstr[i].TrimLeft(filename_prefix);
	}
	sglData.img1num = _wtoi(cstr[0]);
	sglData.img2num = _wtoi(cstr[1]);

	GetDlgItem(IDC_EDIT_MDWAVELENGTH)->GetWindowText(cstr[2]);
	sglData.midWaveLen = _wtof(cstr[2]);

	GetDlgItem(IDC_EDIT_BANDWIDTH)->GetWindowText(cstr[3]);
	sglData.bandWidth = _wtof(cstr[3]);

	GetDlgItem(IDC_EDIT_BG_S)->GetWindowText(cstr[4]);
	sglData.backGround.Format(_T("%s%s.bin"), bg_dir, cstr[4]);

	GetDlgItem(IDC_EDIT_SGLFRECOUNT)->GetWindowText(cstr[5]);
	sglData.freCount = _wtoi(cstr[5]);

	GetDlgItem(IDC_EDIT_COLORMIN)->GetWindowText(cstr[6]);
	sglData.ampMin = _wtof(cstr[6]);

	GetDlgItem(IDC_EDIT_COLORMAX)->GetWindowText(cstr[7]);
	sglData.ampMax = _wtof(cstr[7]);

	GetDlgItem(IDC_EDIT_SGLSR)->GetWindowText(cstr[8]);
	area.startRow = int(_wtof(cstr[8]));

	GetDlgItem(IDC_EDIT_SGLER)->GetWindowText(cstr[9]);
	area.endRow = int(_wtof(cstr[9]));

	GetDlgItem(IDC_EDIT_SGLSC)->GetWindowText(cstr[10]);
	area.startCol = int(_wtof(cstr[10]));

	GetDlgItem(IDC_EDIT_SGLEC)->GetWindowText(cstr[11]);
	area.endCol= int(_wtof(cstr[11]));

	GetDlgItem(IDC_EDIT_SGLOFFSET)->GetWindowText(cstr[12]);
	area.PixelOffset = int(_wtof(cstr[12]));


	for (int i = 0; i < 13; i++)
	{
		if (cstr[i] == _T(""))
		{
			MessageBox(_T("信息未填写完整！"), _T("警告"));
			return;
		}
	}
	if (sglData.midWaveLen <= 0)
	{
		MessageBox(_T("波长必须为正数！"), _T("警告"));
		return;
	}
	if (sglData.bandWidth <= 0)
	{
		MessageBox(_T("带宽必须为正数！"), _T("警告"));
		return;
	}
	if (sglData.freCount <= 0)
	{
		MessageBox(_T("频点数必须为正数！"), _T("警告"));
		return;
	}
	if (sglData.ampMax <= 0 || sglData.ampMin <= 0)
	{
		MessageBox(_T("非线性幅值取值必须为正数！"), _T("警告"));
		return;
	}

	if (sglData.ampMin >= sglData.ampMax)
	{
		MessageBox(_T("非线性幅值范围输入错误！"), _T("警告"));
		return;
	}
	if (!DataCheck(sglData.img1))
	{
		MessageBox(_T("干涉图1不存在！"), _T("警告"));
		return;
	}
	if (!DataCheck(sglData.img2))
	{
		MessageBox(_T("干涉图2不存在！"), _T("警告"));
		return;
	}
	if (!DataCheck(sglData.backGround))
	{
		MessageBox(_T("背景图不存在！"), _T("警告"));
		return;
	}
	
	
	if (area.startRow<=0 || area.startCol<=0 || area.endCol>sglData.imgCol || area.endRow>sglData.imgRow)
	{
		MessageBox(_T("显示区域选择不正确！"), _T("警告"));
		return;
	}
	if (area.PixelOffset<-area.signalWidth() / 2 || area.PixelOffset>area.signalWidth() / 2)
	{
		MessageBox(_T("偏移列数选择超出范围！"), _T("警告"));
		return;
	}
	//area.startCol += area.PixelOffset;
	//area.endCol += area.PixelOffset;

	CString temp_name1, temp_name2;
	GetDlgItem(IDC_EDIT_IMG1)->GetWindowText(temp_name1);
	GetDlgItem(IDC_EDIT_IMG2)->GetWindowText(temp_name2);

	if (temp_foldername != folder_name ||
		temp_filename1 != temp_name1 ||
		temp_filename2 != temp_name2 ||
		num3 != sglData.bandWidth ||
		num4 != sglData.freCount ||
		num5 != sglData.midWaveLen ||
		num6 != sglData.ampMax ||
		num7 != sglData.ampMin ||
		bgStr != sglData.backGround ||
		temp_width != tmp[0] ||
		temp_height != tmp[1]||
		num8!=area.startRow||
		num9!=area.endRow||
		num10!=area.startCol||
		num11!=area.endCol||
		num12!=area.PixelOffset)
	{
		CShowDlg::getInstance()->sglreset();
		CTipsDlg::initInstance(5);
		AfxBeginThread(sglRun, (LPVOID)this);
		temp_foldername = folder_name;
		temp_filename1 = temp_name1;
		temp_filename2 = temp_name2;
		temp_width = tmp[0];
		temp_height = tmp[1];
		num3 = sglData.bandWidth;
		num4 = sglData.freCount;
		num5 = sglData.midWaveLen;
		num6 = sglData.ampMax;
		num7 = sglData.ampMin;
		bgStr = sglData.backGround;
		num8 = area.startRow;
		num9 = area.endRow;
		num10 = area.startCol;
		num11 = area.endCol;
		num12 = area.PixelOffset;
		return;
	}
	MessageBox(_T("你已处理过该组数据！"), _T("警告"));
	return;
}



void CSingleDlg::OnCancel()
{
	return;
}
