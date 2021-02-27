// CMultiDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Integratedbak.h"
#include "DASDlg.h"
#include "MultiDlg.h"
#include "ShowDlg.h"
#include "afxdialogex.h"
#include "Myresource.h"
#include "MyAlgorithm.h"
#include <vector>
#include "Mymatrix.h"
#include "TipsDlg.h"
#define offset 0
#define filternum 5

#pragma warning(once:4244)

CWnd* mMultiWnd;




using namespace std;
extern AREA area;
bool CMultiDlg::bRunning = false;
extern bool is_exit1;
extern bool is_exit2;
extern bool is_exit3;
extern bool is_exit4;
extern bool is_Unwrap;
extern bool is_Wrap;
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
extern CString folder_dir;
extern CString folder_name;

extern vector<Mymatrix<double>*>p_directUnPhaseDataVec;
extern vector<Mymatrix<double>*>p_directPhaseDataVec;
extern int phaseProcess;
extern int unphaseProcess;
extern Mymatrix<double>* Amp;

extern CString folder_dir;
extern CString folder_name;
extern CString filename_prefix;
CString temp_width_m;
CString temp_height_m;
MULTIDATA multiData;


// CMultiDlg 对话框

IMPLEMENT_DYNAMIC(CMultiDlg, CDialogEx)

CMultiDlg::CMultiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_Analysis_Multi, pParent)
{

}

CMultiDlg::~CMultiDlg()
{
}

void CMultiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMultiDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BN_UNWRAP, &CMultiDlg::OnBnClickedBnUnwrap)
	ON_BN_CLICKED(IDC_BN_WRAP, &CMultiDlg::OnBnClickedBnWrap)
	ON_BN_CLICKED(IDC_BN_LOAD3, &CMultiDlg::OnBnClickedBnLoadBgM)
	ON_BN_CLICKED(IDC_BN_LOAD5, &CMultiDlg::OnBnClickedBnLoadFolderM)
END_MESSAGE_MAP()


// CMultiDlg 消息处理程序


BOOL CMultiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetBackgroundColor(RGB(255, 255, 255));
	mMultiWnd = this;

	GetDlgItem(IDC_EDIT_STARTIMG)->SetWindowText(_T("1"));
	GetDlgItem(IDC_EDIT_ENDIMG)->SetWindowText(_T("100"));
	GetDlgItem(IDC_EDIT_MDWAVELEN)->SetWindowText(_T("840"));
	GetDlgItem(IDC_EDIT_FRECOUNT)->SetWindowText(_T("694"));
	GetDlgItem(IDC_EDIT_BANDWIDTH)->SetWindowText(_T("25"));
	GetDlgItem(IDC_EDIT_BG_M)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_MINAMP)->SetWindowText(_T("0.03"));
	GetDlgItem(IDC_EDIT_MAXAMP)->SetWindowText(_T("0.08"));
	GetDlgItem(IDC_EDIT_MLTSR)->SetWindowText(_T("1"));
	GetDlgItem(IDC_EDIT_MLTSC)->SetWindowText(_T("20"));
	GetDlgItem(IDC_EDIT_MLTER)->SetWindowText(_T("800"));
	GetDlgItem(IDC_EDIT_MLTEC)->SetWindowText(_T("299"));
	GetDlgItem(IDC_EDIT_MLTOFFSET)->SetWindowText(_T("-5"));

	bg_dir = GetWorkingDir() + _T("\\Data\\");

	return TRUE;  
}

bool CMultiDlg::Uncheck()
{
	if (bRunning)
	{
		MessageBox(_T("数据正在分析中，请稍等"), _T("警告"));
		return false;
	}
	
	CString cstr[13];
	//CString tmp[2];
	/*GetDlgItem(IDC_EDIT_Target_width2)->GetWindowTextW(tmp[0]);
	GetDlgItem(IDC_EDIT_Target_Height2)->GetWindowTextW(tmp[1]);
	CDASDlg::GetInstance()->Target_Width = _wtoi(tmp[0]);
	CDASDlg::GetInstance()->Target_Height = _wtoi(tmp[1]);
	multiData.imgCol = CDASDlg::GetInstance()->Target_Width;
	multiData.imgRow = CDASDlg::GetInstance()->Target_Height;*/
	CString dir = GetWorkingDir();
	GetDlgItem(IDC_EDIT_STARTIMG)->GetWindowText(cstr[0]);
	GetDlgItem(IDC_EDIT_ENDIMG)->GetWindowText(cstr[1]);
	multiData.imgStartNum = _wtof(cstr[0]);
	multiData.imgEndNum = _wtof(cstr[1]);

	GetDlgItem(IDC_EDIT_MDWAVELEN)->GetWindowText(cstr[2]);
	multiData.midWaveLen = _wtof(cstr[2]);

	GetDlgItem(IDC_EDIT_BANDWIDTH)->GetWindowText(cstr[3]);
	multiData.bandWidth = _wtof(cstr[3]);

	GetDlgItem(IDC_EDIT_BG_M)->GetWindowText(cstr[4]);
	multiData.backGround.Format(_T("%s%s.bin"), bg_dir, cstr[4]);

	GetDlgItem(IDC_EDIT_FRECOUNT)->GetWindowText(cstr[5]);
	multiData.freCount = _wtof(cstr[5]);

	GetDlgItem(IDC_EDIT_MINAMP)->GetWindowText(cstr[6]);
	multiData.ampMin = _wtof(cstr[6]);

	GetDlgItem(IDC_EDIT_MAXAMP)->GetWindowText(cstr[7]);
	multiData.ampMax = double(_wtof(cstr[7]));

	GetDlgItem(IDC_EDIT_MLTSR)->GetWindowText(cstr[8]);
	area.startRow = int(_wtof(cstr[8]));

	GetDlgItem(IDC_EDIT_MLTER)->GetWindowText(cstr[9]);
	area.endRow = int(_wtof(cstr[9]));

	GetDlgItem(IDC_EDIT_MLTSC)->GetWindowText(cstr[10]);
	area.startCol = int(_wtof(cstr[10]));

	GetDlgItem(IDC_EDIT_MLTEC)->GetWindowText(cstr[11]);
	area.endCol = int(_wtof(cstr[11]));

	GetDlgItem(IDC_EDIT_MLTOFFSET)->GetWindowText(cstr[12]);
	area.PixelOffset = int(_wtof(cstr[12]));

	for (int i = 0; i < 13; i++)
	{
		if (cstr[i] == _T(""))
		{
			MessageBox(_T("信息未填写完整！"), _T("警告"));
			return false;
		}
	}
	if (multiData.imgStartNum <= 0 || multiData.imgEndNum <= 0)
	{
		MessageBox(_T("起始图像和结束图像号码必须为正数！"), _T("警告"));
		return false;
	}
	if (multiData.imgStartNum >= multiData.imgEndNum)
	{
		MessageBox(_T("起始图像号码必须小于结束图像号码！"), _T("警告"));
		return false;
	}
	CString dataname[2];
	dataname[0].Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgStartNum);
	dataname[1].Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgEndNum);
	if (!DataCheck(dataname[0]))
	{
		MessageBox(_T("起始图像不存在！"), _T("警告"));
		return false;
	}
	if (!DataCheck(dataname[1]))
	{
		MessageBox(_T("结束图像不存在！"), _T("警告"));
		return false;
	}
	if (multiData.midWaveLen <= 0)
	{
		MessageBox(_T("波长必须为正数！"), _T("警告"));
		return false;
	}
	if (multiData.bandWidth <= 0)
	{
		MessageBox(_T("带宽必须为正数！"), _T("警告"));
		return false;
	}

	if (multiData.freCount <= 0)
	{
		MessageBox(_T("频点数必须为正数！"), _T("警告"));
		return false;
	}
	if (multiData.ampMax <= 0 || multiData.ampMin <= 0)
	{
		MessageBox(_T("非线性幅值取值必须为正数！"), _T("警告"));
		return false;
	}

	if (multiData.ampMin >= multiData.ampMax)
	{
		MessageBox(_T("非线性幅值范围输入错误！"), _T("警告"));
		return false;
	}

	
	if (area.startRow<=0 || area.startCol<=0 || area.endCol>multiData.freCount || area.endRow>multiData.imgRow)
	{
		MessageBox(_T("显示区域选择不正确！"), _T("警告"));
		return false;
	}
	if (area.PixelOffset<-area.signalWidth()/2 || area.PixelOffset>area.signalWidth()/2)
	{
		MessageBox(_T("偏移列数选择超出范围！"), _T("警告"));
		return false;
	}
	/*area.startCol += area.PixelOffset;
	area.endCol += area.PixelOffset;*/
	/*CString ch1[2];
	ch1[0].Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgStartNum);
	ch1[1].Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgEndNum);
	if ((DataCheck(ch1[0])==false)||(DataCheck(ch1[1])==false))
	{
		MessageBox(_T("处理图像选择范围不正确，请确保图像存在！"), _T("警告"));
		return false;
	}*/
	if (!DataCheck(multiData.backGround))
	{
		MessageBox(_T("背景图不存在！"), _T("警告"));
		return false;
	}

	
	//MessageBox(_T("你已处理过该组数据！"), _T("警告"));
	return true;
}

bool CMultiDlg::check()
{
	if (bRunning)
	{
		MessageBox(_T("数据正在分析中，请稍等"), _T("警告"));
		return false;
	}
	
	CString cstr[13];
	//CString tmp[2];
	/*GetDlgItem(IDC_EDIT_Target_width2)->GetWindowTextW(tmp[0]);
	GetDlgItem(IDC_EDIT_Target_Height2)->GetWindowTextW(tmp[1]);
	CDASDlg::GetInstance()->Target_Width = _wtoi(tmp[0]);
	CDASDlg::GetInstance()->Target_Height = _wtoi(tmp[1]);
	multiData.imgCol = CDASDlg::GetInstance()->Target_Width;
	multiData.imgRow = CDASDlg::GetInstance()->Target_Height;*/
	CString dir = GetWorkingDir();
	GetDlgItem(IDC_EDIT_STARTIMG)->GetWindowText(cstr[0]);
	GetDlgItem(IDC_EDIT_ENDIMG)->GetWindowText(cstr[1]);
	multiData.imgStartNum = _wtof(cstr[0]);
	multiData.imgEndNum = _wtof(cstr[1]);

	GetDlgItem(IDC_EDIT_MDWAVELEN)->GetWindowText(cstr[2]);
	multiData.midWaveLen = _wtof(cstr[2]);

	GetDlgItem(IDC_EDIT_BANDWIDTH)->GetWindowText(cstr[3]);
	multiData.bandWidth = _wtof(cstr[3]);

	GetDlgItem(IDC_EDIT_BG_M)->GetWindowText(cstr[4]);
	multiData.backGround.Format(_T("%s%s.bin"), bg_dir, cstr[4]);

	GetDlgItem(IDC_EDIT_FRECOUNT)->GetWindowText(cstr[5]);
	multiData.freCount = _wtof(cstr[5]);

	GetDlgItem(IDC_EDIT_MINAMP)->GetWindowText(cstr[6]);
	multiData.ampMin = _wtof(cstr[6]);

	GetDlgItem(IDC_EDIT_MAXAMP)->GetWindowText(cstr[7]);
	multiData.ampMax = double(_wtof(cstr[7]));

	GetDlgItem(IDC_EDIT_MLTSR)->GetWindowText(cstr[8]);
	area.startRow = int(_wtof(cstr[8]));

	GetDlgItem(IDC_EDIT_MLTER)->GetWindowText(cstr[9]);
	area.endRow = int(_wtof(cstr[9]));

	GetDlgItem(IDC_EDIT_MLTSC)->GetWindowText(cstr[10]);
	area.startCol = int(_wtof(cstr[10]));

	GetDlgItem(IDC_EDIT_MLTEC)->GetWindowText(cstr[11]);
	area.endCol = int(_wtof(cstr[11]));
	

	GetDlgItem(IDC_EDIT_MLTOFFSET)->GetWindowText(cstr[12]);
	area.PixelOffset = int(_wtof(cstr[12]));

	for (int i = 0; i < 13; i++)
	{
		if (cstr[i] == _T(""))
		{
			MessageBox(_T("信息未填写完整！"), _T("警告"));
			return false;
		}
	}
	if (multiData.imgStartNum <= 0 || multiData.imgEndNum <= 0)
	{
		MessageBox(_T("起始图像和结束图像号码必须为正数！"), _T("警告"));
		return false;
	}
	if (multiData.imgStartNum >= multiData.imgEndNum)
	{
		MessageBox(_T("起始图像号码必须小于结束图像号码！"), _T("警告"));
		return false;
	}
	CString dataname[2];
	dataname[0].Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgStartNum);
	dataname[1].Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgEndNum);
	if (!DataCheck(dataname[0]))
	{
		MessageBox(_T("起始图像不存在！"), _T("警告"));
		return false;
	}
	if (!DataCheck(dataname[1]))
	{
		MessageBox(_T("结束图像不存在！"), _T("警告"));
		return false;
	}
	if (multiData.midWaveLen <= 0)
	{
		MessageBox(_T("波长必须为正数！"), _T("警告"));
		return false;
	}
	if (multiData.bandWidth <= 0)
	{
		MessageBox(_T("带宽必须为正数！"), _T("警告"));
		return false;
	}

	if (multiData.freCount <= 0)
	{
		MessageBox(_T("频点数必须为正数！"), _T("警告"));
		return false;
	}
	if (multiData.ampMax <= 0 || multiData.ampMin <= 0)
	{
		MessageBox(_T("非线性幅值取值必须为正数！"), _T("警告"));
		return false;
	}

	if (multiData.ampMin >= multiData.ampMax)
	{
		MessageBox(_T("非线性幅值范围输入错误！"), _T("警告"));
		return false;
	}


	if (area.startRow <= 0 || area.startCol <= 0 || area.endCol>multiData.freCount || area.endRow>multiData.imgRow)
	{
		MessageBox(_T("显示区域选择不正确！"), _T("警告"));
		return false;
	}
	if (area.PixelOffset<-area.signalWidth() / 2 || area.PixelOffset>area.signalWidth() / 2)
	{
		MessageBox(_T("偏移列数选择超出范围！"), _T("警告"));
		return false;
	}
	/*area.startCol += area.PixelOffset;
	area.endCol += area.PixelOffset;*/
	/*CString ch1[2];
	ch1[0].Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgStartNum);
	ch1[1].Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgEndNum);
	if ((DataCheck(ch1[0])==false)||(DataCheck(ch1[1])==false))
	{
	MessageBox(_T("处理图像选择范围不正确，请确保图像存在！"), _T("警告"));
	return false;
	}*/
	if (!DataCheck(multiData.backGround))
	{
		MessageBox(_T("背景图不存在！"), _T("警告"));
		return false;
	}


	
	//MessageBox(_T("你已处理过该组数据！"), _T("警告"));
	return true;
}

bool runover = false;
UINT UnPhaseProcess(LPVOID pParam)
{
	//叠加
	for (int i = 1; i < p_UnPhaseDataVec1.size(); i++)
	{
		*p_UnPhaseDataVec1[i] += *p_UnPhaseDataVec1[i - 1];
	}
	*p_UnPhaseDataVec2[0] += *p_UnPhaseDataVec1[p_UnPhaseDataVec1.size() - 1];
	for (int i = 1; i < p_UnPhaseDataVec2.size(); i++)
	{
		*p_UnPhaseDataVec2[i] += *p_UnPhaseDataVec2[i - 1];
	}
	*p_UnPhaseDataVec3[0] += *p_UnPhaseDataVec2[p_UnPhaseDataVec2.size() - 1];
	for (int i = 1; i < p_UnPhaseDataVec3.size(); i++)
	{
		*p_UnPhaseDataVec3[i] += *p_UnPhaseDataVec3[i - 1];
	}
	*p_UnPhaseDataVec4[0] += *p_UnPhaseDataVec3[p_UnPhaseDataVec3.size() - 1];
	for (int i = 1; i < p_UnPhaseDataVec4.size(); i++)
	{
		*p_UnPhaseDataVec4[i] += *p_UnPhaseDataVec4[i - 1];
	}
	unphaseProcess++;
	CTipsDlg::getInstance()->update(unphaseProcess);

	if (area.PixelOffset < 0)//左移
	{
		//相位偏移
		for (int k = 0; k < p_UnPhaseDataVec1.size(); k++)
		{
			for (int i = area.signalWidth() / 2; i < area.signalWidth() + area.PixelOffset; i++)
			{
				for (int j = 0; j < area.signalHeight(); j++)
				{
					p_UnPhaseDataVec1[k]->Data[area.startRow - 1 + j][area.startCol - 1 + i] = p_UnPhaseDataVec1[k]->Data[area.startRow - 1 + j][area.startCol - 1 + i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_UnPhaseDataVec2.size(); k++)
		{
			for (int i = area.signalWidth() / 2; i < area.signalWidth() + area.PixelOffset; i++)
			{
				for (int j = 0; j < area.signalHeight(); j++)
				{
					p_UnPhaseDataVec2[k]->Data[area.startRow - 1 + j][area.startCol - 1 + i] = p_UnPhaseDataVec2[k]->Data[area.startRow - 1 + j][area.startCol - 1 + i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_UnPhaseDataVec3.size(); k++)
		{
			for (int i = area.signalWidth() / 2; i < area.signalWidth() + area.PixelOffset; i++)
			{
				for (int j = 0; j < area.signalHeight(); j++)
				{
					p_UnPhaseDataVec3[k]->Data[area.startRow - 1 + j][area.startCol - 1 + i] = p_UnPhaseDataVec3[k]->Data[area.startRow - 1 + j][area.startCol - 1 + i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_UnPhaseDataVec4.size(); k++)
		{
			for (int i = area.signalWidth() / 2; i < area.signalWidth() + area.PixelOffset; i++)
			{
				for (int j = 0; j < area.signalHeight(); j++)
				{
					p_UnPhaseDataVec4[k]->Data[area.startRow - 1 + j][area.startCol - 1 + i] = p_UnPhaseDataVec4[k]->Data[area.startRow - 1 + j][area.startCol - 1 + i - area.PixelOffset];
				}
			}
		}
	}
	else if (area.PixelOffset > 0)//右移
	{
		//相位偏移
		for (int k = 0; k < p_UnPhaseDataVec1.size(); k++)
		{
			for (int i = area.startCol - 1+area.signalWidth()+ area.PixelOffset; i > area.startCol - 1 + area.signalWidth() + area.PixelOffset; i--)
			{
				for (int j = area.startRow - 1; j < area.startRow - 1+area.signalHeight(); j++)
				{
					p_UnPhaseDataVec1[k]->Data[j][i] = p_UnPhaseDataVec1[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_UnPhaseDataVec2.size(); k++)
		{
			for (int i = area.startCol - 1 + area.signalWidth() + area.PixelOffset; i > area.startCol - 1 + area.signalWidth() + area.PixelOffset; i--)
			{
				for (int j = area.startRow - 1; j < area.startRow - 1 + area.signalHeight(); j++)
				{
					p_UnPhaseDataVec2[k]->Data[j][i] = p_UnPhaseDataVec2[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_UnPhaseDataVec3.size(); k++)
		{
			for (int i = area.startCol - 1 + area.signalWidth() + area.PixelOffset; i > area.startCol - 1 + area.signalWidth() + area.PixelOffset; i--)
			{
				for (int j = area.startRow - 1; j < area.startRow - 1 + area.signalHeight(); j++)
				{
					p_UnPhaseDataVec3[k]->Data[j][i] = p_UnPhaseDataVec3[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_UnPhaseDataVec4.size(); k++)
		{
			for (int i = area.startCol - 1 + area.signalWidth() + area.PixelOffset; i > area.startCol - 1 + area.signalWidth() + area.PixelOffset; i--)
			{
				for (int j = area.startRow - 1; j < area.startRow - 1 + area.signalHeight(); j++)
				{
					p_UnPhaseDataVec4[k]->Data[j][i] = p_UnPhaseDataVec4[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
	}
	unphaseProcess++;
	CTipsDlg::getInstance()->update(unphaseProcess);

	//相位处理
	for (int i = 0; i < p_UnPhaseDataVec1.size(); i++)
	{
		//p_UnPhaseDataVec1[i]->Flipud();
		p_UnPhaseDataVec1[i]->resize(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth());
		medFilt2(*p_UnPhaseDataVec1[i], filternum);
		
	}
	unphaseProcess++;
	CTipsDlg::getInstance()->update(unphaseProcess);
	for (int i = 0; i < p_UnPhaseDataVec2.size(); i++)
	{
		//p_UnPhaseDataVec2[i]->Flipud();
		p_UnPhaseDataVec2[i]->resize(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth());
		medFilt2(*p_UnPhaseDataVec2[i], filternum);
		
	}
	unphaseProcess++;
	CTipsDlg::getInstance()->update(unphaseProcess);

	for (int i = 0; i < p_UnPhaseDataVec3.size(); i++)
	{
		//p_UnPhaseDataVec3[i]->Flipud();
		p_UnPhaseDataVec3[i]->resize(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth());
		medFilt2(*p_UnPhaseDataVec3[i], filternum);
		
	}
	unphaseProcess++;
	CTipsDlg::getInstance()->update(unphaseProcess);
	for (int i = 0; i < p_UnPhaseDataVec4.size(); i++)
	{
		//p_UnPhaseDataVec4[i]->Flipud();
		p_UnPhaseDataVec4[i]->resize(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth());
		medFilt2(*p_UnPhaseDataVec4[i], filternum);
		
	}
	unphaseProcess++;
	CTipsDlg::getInstance()->update(unphaseProcess);

	runover = true;
	return 0;
}

UINT PhaseProcess(LPVOID pParam)
{
	for (int i = 0; i < p_PhaseDataVec1.size(); i++)
	{
		//p_PhaseDataVec1[i]->Flipud();
		p_PhaseDataVec1[i]->resize(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth());
		medFilt2(*p_PhaseDataVec1[i], filternum);
		
	}
	for (int i = 0; i < p_PhaseDataVec2.size(); i++)
	{
		//p_PhaseDataVec2[i]->Flipud();
		p_PhaseDataVec2[i]->resize(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth());
		medFilt2(*p_PhaseDataVec2[i], filternum);
		
	}
	for (int i = 0; i < p_PhaseDataVec3.size(); i++)
	{
		//p_PhaseDataVec3[i]->Flipud();
		p_PhaseDataVec3[i]->resize(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth());
		medFilt2(*p_PhaseDataVec3[i], filternum);
		
	}
	for (int i = 0; i < p_PhaseDataVec4.size(); i++)
	{
		//p_PhaseDataVec4[i]->Flipud();
		p_PhaseDataVec4[i]->resize(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth());
		medFilt2(*p_PhaseDataVec4[i], filternum);
		
	}
	phaseProcess++;
	CTipsDlg::getInstance()->update(phaseProcess);

	if (area.PixelOffset < 0)//左移
	{
		//相位偏移
		for (int k = 0; k < p_PhaseDataVec1.size(); k++)
		{
			for (int i = p_PhaseDataVec1[k]->Col/2; i < p_PhaseDataVec1[k]->Col + area.PixelOffset; i++)
			{
				for (int j = 0  ; j < p_PhaseDataVec1[k]->Row; j++)
				{
					p_PhaseDataVec1[k]->Data[ j][i] = p_PhaseDataVec1[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_PhaseDataVec2.size(); k++)
		{
			for (int i = p_PhaseDataVec2[k]->Col / 2; i < p_PhaseDataVec2[k]->Col + area.PixelOffset; i++)
			{
				for (int j = 0; j < p_PhaseDataVec1[k]->Row; j++)
				{
					p_PhaseDataVec2[k]->Data[j][i] = p_PhaseDataVec2[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_PhaseDataVec3.size(); k++)
		{
			for (int i = p_PhaseDataVec3[k]->Col / 2; i < p_PhaseDataVec3[k]->Col + area.PixelOffset; i++)
			{
				for (int j = 0; j < p_PhaseDataVec3[k]->Row; j++)
				{
					p_PhaseDataVec3[k]->Data[j][i] = p_PhaseDataVec3[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_PhaseDataVec4.size(); k++)
		{
			for (int i = p_PhaseDataVec4[k]->Col / 2; i < p_PhaseDataVec4[k]->Col + area.PixelOffset; i++)
			{
				for (int j = 0; j < p_PhaseDataVec4[k]->Row; j++)
				{
					p_PhaseDataVec4[k]->Data[j][i] = p_PhaseDataVec4[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
	}
	else if (area.PixelOffset > 0)//右移
	{
		//相位偏移
		for (int k = 0; k < p_PhaseDataVec1.size(); k++)
		{
			for (int i = p_PhaseDataVec1[k]->Col - 1; i > p_PhaseDataVec1[k]->Col/2+ area.PixelOffset; i--)
			{
				for (int j = 0; j <p_PhaseDataVec1[k]->Row; j++)
				{
					p_PhaseDataVec1[k]->Data[j][i] = p_PhaseDataVec1[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_PhaseDataVec2.size(); k++)
		{
			for (int i = p_PhaseDataVec2[k]->Col - 1; i > p_PhaseDataVec2[k]->Col / 2 + area.PixelOffset; i--)
			{
				for (int j = 0; j < p_PhaseDataVec2[k]->Row; j++)
				{
					p_PhaseDataVec2[k]->Data[j][i] = p_PhaseDataVec2[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_PhaseDataVec3.size(); k++)
		{
			for (int i = p_PhaseDataVec3[k]->Col - 1; i > p_PhaseDataVec3[k]->Col / 2 + area.PixelOffset; i--)
			{
				for (int j = 0; j < p_PhaseDataVec3[k]->Row; j++)
				{
					p_PhaseDataVec3[k]->Data[j][i] = p_PhaseDataVec3[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
		for (int k = 0; k < p_PhaseDataVec4.size(); k++)
		{
			for (int i = p_PhaseDataVec4[k]->Col - 1; i > p_PhaseDataVec4[k]->Col / 2 + area.PixelOffset; i--)
			{
				for (int j = 0; j < p_PhaseDataVec4[k]->Row; j++)
				{
					p_PhaseDataVec4[k]->Data[j][i] = p_PhaseDataVec4[k]->Data[j][i - area.PixelOffset];
				}
			}
		}
	}

// 	phaseProcess++;
// 	CTipsDlg::getInstance()->update(phaseProcess);

	runover = true;
	return 0;
}

void CMultiDlg::OnBnClickedBnUnwrap()
{
	if (!Uncheck())
	{
		return;
	}

	static CString bgStr("");
	static int num1 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0, num8 = 0, num9 = 0, num10 = 0, num11 = 0, num12 = 0;
	static double num6 = 0, num7 = 0;
	if (num1 != multiData.imgStartNum ||
		num2 != multiData.imgEndNum ||
		num3 != multiData.bandWidth ||
		num4 != multiData.freCount ||
		num5 != multiData.midWaveLen ||
		num6 != multiData.ampMax ||
		num7 != multiData.ampMin ||
		bgStr != multiData.backGround ||
		num8 != area.startRow ||
		num9 != area.endRow ||
		num10 != area.startCol ||
		num11 != area.endCol ||
		num12 != area.PixelOffset)
	{
		CShowDlg::getInstance()->reset(true);
	
		unphaseProcess = 0;

		CString path;
		path.Format(_T("%s\\%d-%d,%d,%d,%d,%d-%d,%d-%d unWrap.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
			multiData.freCount, multiData.midWaveLen, multiData.bandWidth, area.startRow, area.endRow,
			area.startCol, area.endCol);
		if (DataCheck(path))
		{
			if (p_directUnPhaseDataVec.size() > 0)
			{
				for (auto &i : p_directUnPhaseDataVec)
				{
					delete i;
				}
			}
			p_directUnPhaseDataVec.clear();
		}
		else
		{
			if (p_UnPhaseDataVec1.size() > 0)
			{
				for (auto &i : p_UnPhaseDataVec4)
				{
					delete i;
				}
				p_UnPhaseDataVec4.clear();
				for (auto &i : p_UnPhaseDataVec3)
				{
					delete i;
				}
				p_UnPhaseDataVec3.clear();
				for (auto &i : p_UnPhaseDataVec2)
				{
					delete i;
				}
				p_UnPhaseDataVec2.clear();
				for (auto &i : p_UnPhaseDataVec1)
				{
					delete i;
				}
				p_UnPhaseDataVec1.clear();
				delete[] unPhaseData1;

				delete[] unPhaseData2;
				delete[] unPhaseData3;
				delete[] unPhaseData4;
				unPhaseData1 = NULL;
				unPhaseData2 = NULL;
				unPhaseData3 = NULL;
				unPhaseData4 = NULL;
			}
		}
	}
	else
	{
		MessageBox(_T("已处理过该组数据！"), _T("警告"));
		return;
	}

	CString path;
	path.Format(_T("%s\\%d-%d,%d,%d,%d,%d-%d,%d-%d unWrap.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
		multiData.freCount, multiData.midWaveLen, multiData.bandWidth, area.startRow, area.endRow, 
		area.startCol, area.endCol);
	if (DataCheck(path))
	{
		if (p_directUnPhaseDataVec.empty())
		{
			FILE *f_r1;
			_wfopen_s(&f_r1, path, _T("rb"));
			if (f_r1 != NULL)
			{
				CTipsDlg::initInstance((multiData.imgEndNum - multiData.imgStartNum)*2 + 2);
				if (Amp != NULL)
				{
					delete Amp;
					Amp = NULL;
				}

				for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
				{
					Mymatrix<double>*tempdata = new Mymatrix<double>(area.signalHeight(), area.signalWidth());
					for (int j = 0; j < tempdata->Row; j++)
					{
						fread(tempdata->Data[j], sizeof(double), tempdata->Col, f_r1);
					}
					p_directUnPhaseDataVec.push_back(tempdata);
				}

				unphaseProcess++;
				CTipsDlg::getInstance()->update(unphaseProcess);

				fclose(f_r1);

				FILE *f_r2;
				CString path1;
				path1.Format(_T("%s\\%d-%d,%d,%d-%d Amp.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
					multiData.freCount, area.startCol, area.endCol);
				_wfopen_s(&f_r2, path1, _T("rb"));
				if (f_r2 != NULL)
				{
					Amp = new Mymatrix<double>(f_r2, multiData.imgRow, multiData.freCount);
					fclose(f_r2);
					unphaseProcess++;
					CTipsDlg::getInstance()->update(unphaseProcess);
					AfxBeginThread(directUnWrap, NULL);
					return;
				}
				if (p_directUnPhaseDataVec.size() > 0)
				{
					for (auto &i : p_directUnPhaseDataVec)
					{
						delete i;
					}
				}
				p_directUnPhaseDataVec.clear();
			}
		}
	}
	if (p_UnPhaseDataVec1.empty())
	{
		CTipsDlg::initInstance((multiData.imgEndNum - multiData.imgStartNum) * 3 + 6);
		AfxBeginThread(UnPhaseDataProcess1, LPVOID(this));
		AfxBeginThread(UnPhaseDataProcess2, LPVOID(this));
		AfxBeginThread(UnPhaseDataProcess3, LPVOID(this));
		AfxBeginThread(UnPhaseDataProcess4, LPVOID(this));

		while ((!is_exit1) || (!is_exit2) || (!is_exit3) || (!is_exit4))
		{
			DoEvents();
			Sleep(40);
		}
		is_exit1 = is_exit2 = is_exit3 = is_exit4 = false;
		runover = false;
		AfxBeginThread(UnPhaseProcess, NULL);

		while (!runover)
		{
			DoEvents();
			Sleep(40);
		}
		
		//AfxBeginThread(unWrapGray, NULL);
		AfxBeginThread(unWrap, NULL);
		num1 = multiData.imgStartNum;
		num2 = multiData.imgEndNum;
		num3 = multiData.bandWidth;
		num4 = multiData.freCount;
		num5 = multiData.midWaveLen;
		num6 = multiData.ampMax;
		num7 = multiData.ampMin;
		bgStr = multiData.backGround;
		num8 = area.startRow;
		num9 = area.endRow;
		num10 = area.startCol;
		num11 = area.endCol;
		num12 = area.PixelOffset;
		return;
	}

	MessageBox(_T("已处理过该组数据！"), _T("警告"));
	return;
}

void CMultiDlg::OnBnClickedBnWrap()
{
	if (!check())
	{
		return;
	}

	static CString bgStr("");
	static int num1 = 0, num2 = 0, num3 = 0, num4 = 0, num5 = 0, num8 = 0, num9 = 0, num10 = 0, num11 = 0, num12 = 0;
	static double num6 = 0, num7 = 0;

	if (num1 != multiData.imgStartNum ||
		num2 != multiData.imgEndNum ||
		num3 != multiData.bandWidth ||
		num4 != multiData.freCount ||
		num5 != multiData.midWaveLen ||
		num6 != multiData.ampMax ||
		num7 != multiData.ampMin ||
		bgStr != multiData.backGround ||
		num8 != area.startRow ||
		num9 != area.endRow ||
		num10 != area.startCol ||
		num11 != area.endCol ||
		num12 != area.PixelOffset)
	{
		CShowDlg::getInstance()->reset(false);
		phaseProcess = 0;

		

		CString path;
		path.Format(_T("%s\\%d-%d,%d,%d,%d,%d-%d,%d-%d Wrap.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
			multiData.freCount, multiData.midWaveLen, multiData.bandWidth, area.startRow, area.endRow,
			area.startCol, area.endCol);
		if (DataCheck(path))
		{
			if (p_directPhaseDataVec.size() > 0)
			{
				for (auto &i : p_directPhaseDataVec)
				{
					delete i;
				}
			}
			p_directPhaseDataVec.clear();
		}
		else
		{
			if (p_PhaseDataVec1.size() > 0)
			{
				for (auto &i : p_PhaseDataVec4)
				{
					delete i;
				}
				p_PhaseDataVec4.clear();
				for (auto &i : p_PhaseDataVec3)
				{
					delete i;
				}
				p_PhaseDataVec3.clear();
				for (auto &i : p_PhaseDataVec2)
				{
					delete i;
				}
				p_PhaseDataVec2.clear();
				for (auto &i : p_PhaseDataVec1)
				{
					delete i;
				}
				p_PhaseDataVec1.clear();
				delete[] PhaseData1;
				delete[] PhaseData2;
				delete[] PhaseData3;
				delete[] PhaseData4;
				PhaseData1 = NULL;
				PhaseData2 = NULL;
				PhaseData3 = NULL;
				PhaseData4 = NULL;
			}
		}
	}
	else
	{
		MessageBox(_T("已处理过该组数据！"), _T("警告"));
		return;
	}

	CString path;
	path.Format(_T("%s\\%d-%d,%d,%d,%d,%d-%d,%d-%d Wrap.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
		multiData.freCount, multiData.midWaveLen, multiData.bandWidth, area.startRow, area.endRow,
		area.startCol, area.endCol);
	if (DataCheck(path))
	{
		if (p_directPhaseDataVec.empty())
		{
			FILE *f_r1;
			_wfopen_s(&f_r1, path, _T("rb"));
			if (f_r1 != NULL)
			{
				CTipsDlg::initInstance(multiData.imgEndNum - multiData.imgStartNum + 2);
				if (Amp != NULL)
				{
					delete Amp;
					Amp = NULL;
				}

				for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
				{
					Mymatrix<double>*tempdata = new Mymatrix<double>(area.signalHeight(), area.signalWidth());
					for (int j = 0; j < tempdata->Row; j++)
					{
						fread(tempdata->Data[j], sizeof(double), tempdata->Col, f_r1);
					}
					p_directPhaseDataVec.push_back(tempdata);
				}

				phaseProcess++;
				CTipsDlg::getInstance()->update(phaseProcess);

				fclose(f_r1);

				FILE *f_r2;
				CString path1;
				path1.Format(_T("%s\\%d-%d,%d,%d-%d Amp.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
					multiData.freCount, area.startCol, area.endCol);
				_wfopen_s(&f_r2, path1, _T("rb"));
				if (f_r2 != NULL)
				{
					Amp = new Mymatrix<double>(f_r2, multiData.imgRow, multiData.freCount);
					fclose(f_r2);
					phaseProcess++;
					CTipsDlg::getInstance()->update(phaseProcess);

					AfxBeginThread(directWrap, NULL);
					num1 = multiData.imgStartNum;
					num2 = multiData.imgEndNum;
					num3 = multiData.bandWidth;
					num4 = multiData.freCount;
					num5 = multiData.midWaveLen;
					num6 = multiData.ampMax;
					num7 = multiData.ampMin;
					bgStr = multiData.backGround;
					num8 = area.startRow;
					num9 = area.endRow;
					num10 = area.startCol;
					num11 = area.endCol;
					num12 = area.PixelOffset;
					return;
				}
				if (p_directPhaseDataVec.size() > 0)
				{
					for (auto &i : p_directPhaseDataVec)
					{
						delete i;
					}
				}
				p_directPhaseDataVec.clear();
			}
		}
	}
	if (p_PhaseDataVec1.empty())
	{
		CTipsDlg::initInstance((multiData.imgEndNum - multiData.imgStartNum) * 2 + 2);
		AfxBeginThread(PhaseDataProcess1, LPVOID(this));
		AfxBeginThread(PhaseDataProcess2, LPVOID(this));
		AfxBeginThread(PhaseDataProcess3, LPVOID(this));
		AfxBeginThread(PhaseDataProcess4, LPVOID(this));

		while ((!is_exit1) || (!is_exit2) || (!is_exit3) || (!is_exit4))
		{
			DoEvents();
			Sleep(40);
		}
		is_exit1 = is_exit2 = is_exit3 = is_exit4 = false;

		runover = false;

		AfxBeginThread(PhaseProcess, NULL);

		while (!runover)
		{
			DoEvents();
			Sleep(40);
		}

		AfxBeginThread(Wrap, NULL);
		num1 = multiData.imgStartNum;
		num2 = multiData.imgEndNum;
		num3 = multiData.bandWidth;
		num4 = multiData.freCount;
		num5 = multiData.midWaveLen;
		num6 = multiData.ampMax;
		num7 = multiData.ampMin;
		bgStr = multiData.backGround;
		num8 = area.startRow;
		num9 = area.endRow;
		num10 = area.startCol;
		num11 = area.endCol;
		num12 = area.PixelOffset;
		return;
	}
	MessageBox(_T("已处理过该组数据！"), _T("警告"));
	return;
}

void CMultiDlg::OnBnClickedBnLoadBgM()
{
	CString filename;
	CFileDialog filedlg(true, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR(_T("二进制文件(*.bin)|*.bin|All Files(*.*)|*.*||")), NULL);
	CString path;
	path.Format(_T("%s\\Data\\"), GetWorkingDir());
	filedlg.m_ofn.lpstrInitialDir = path;
	if (filedlg.DoModal() == IDOK)
	{
		filename = filedlg.GetFileName();
		bg_dir = filedlg.GetPathName();
		bg_dir.Replace(filename, _T(""));
		filename = filename.Left(filename.GetLength() - 4);
		GetDlgItem(IDC_EDIT_BG_M)->SetWindowText(filename);
	}
}

void CMultiDlg::OnBnClickedBnLoadFolderM()
{
	CString filename;
	CString path;
	path.Format(_T("%s\\Data\\"), GetWorkingDir());
	CFolderPickerDialog folderdlg(path);
	if (folderdlg.DoModal() == IDOK)
	{
		folder_dir = folderdlg.GetPathName();
		filename = folderdlg.GetFileName();
		folder_name = filename;
		GetDlgItem(IDC_EDIT_FolderNameM)->SetWindowText(filename);
	}
}


void CMultiDlg::OnCancel()
{
	return;
}
