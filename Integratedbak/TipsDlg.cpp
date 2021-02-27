// TipsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Integratedbak.h"
#include "TipsDlg.h"
#include "afxdialogex.h"
#include <vector>
#include "Mymatrix.h"
#include "resource.h"

// CTipsDlg 对话框

using namespace std;
extern int PhaseGrayImgNum1;
extern int PhaseColorImgNum1;
extern int PhaseGrayImgNum2;
extern int PhaseColorImgNum2;
extern int PhaseGrayImgNum3;
extern int PhaseColorImgNum3;

extern int unPhaseGrayImgNum1;
extern int unPhaseColorImgNum1;
extern int unPhaseGrayImgNum2;
extern int unPhaseColorImgNum2;
extern int unPhaseGrayImgNum3;
extern int unPhaseColorImgNum3;

extern vector<Mymatrix<double>*>p_PhaseDataVec1;
extern vector<Mymatrix<double>*>p_PhaseDataVec2;
extern vector<Mymatrix<double>*>p_PhaseDataVec3;
extern vector<Mymatrix<double>*>p_PhaseDataVec4;

extern Mymatrix<double>** PhaseData1;
extern Mymatrix<double>** PhaseData2;
extern Mymatrix<double>** PhaseData3;
extern Mymatrix<double>** PhaseData4;

extern vector<Mymatrix<double>*>p_UnPhaseDataVec1;
extern vector<Mymatrix<double>*>p_UnPhaseDataVec2;
extern vector<Mymatrix<double>*>p_UnPhaseDataVec3;
extern vector<Mymatrix<double>*>p_UnPhaseDataVec4;

extern vector<Mymatrix<double>*>p_directUnPhaseDataVec;
extern vector<Mymatrix<double>*>p_directPhaseDataVec;

extern Mymatrix<double>** unPhaseData1;
extern Mymatrix<double>** unPhaseData2;
extern Mymatrix<double>** unPhaseData3;
extern Mymatrix<double>** unPhaseData4;

extern bool phase_exit1;
extern bool unphase_exit1;
extern bool phase_exit2;
extern bool unphase_exit2;
extern bool phase_exit3;
extern bool unphase_exit3;
extern bool phase_exit4;
extern bool unphase_exit4;
extern bool phase_exit;
extern bool unphase_exit;

extern bool sgl_exit;

extern bool is_exit1;
extern bool is_exit2;
extern bool is_exit3;
extern bool is_exit4;

extern bool is_Wrap;
extern bool is_Unwrap;

extern int phaseProcess;
extern int unphaseProcess;
extern int singleProcess;

IMPLEMENT_DYNAMIC(CTipsDlg, CDialog)

CTipsDlg* CTipsDlg::_tipsdlg = NULL;

CTipsDlg::CTipsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_Tip, pParent)
{
}
CTipsDlg::CTipsDlg(int value)
	: PgLimit(value)
{
}

CTipsDlg::~CTipsDlg()
{
	/*if (_tipsdlg != NULL)
	{
	delete _tipsdlg;
	}*/
}

void CTipsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, ProgressBar);
	DDX_Control(pDX, IDC_PERCENT, Percent);
}

BEGIN_MESSAGE_MAP(CTipsDlg, CDialog)
END_MESSAGE_MAP()

void CTipsDlg::OnCancel()
{
	if (MessageBox(_T("确定要取消数据分析进程吗？"), _T("警告"), MB_OKCANCEL) == IDOK)
	{
		GetDlgItem(IDC_STATIC_TEXT)->SetWindowText(_T("正在取消中，请稍等~"));
		phase_exit1 = true;
		unphase_exit1 = true;
		phase_exit2 = true;
		unphase_exit2 = true;
		phase_exit3 = true;
		unphase_exit3 = true;
		phase_exit4 = true;
		unphase_exit4 = true;
		phase_exit = true;
		unphase_exit = true;
		sgl_exit = true;
		phaseProcess = 0;
		unphaseProcess = 0;
		singleProcess = 0;
		if (is_Wrap)
		{
			while ((is_Wrap) || (!is_exit1) || (!is_exit2) || (!is_exit3) || (!is_exit4))
			{
				DoEvents();
				Sleep(40);
			}
		}
		else if (is_Unwrap)
		{
			while ((is_Unwrap) || (!is_exit1) || (!is_exit2) || (!is_exit3) || (!is_exit4))
			{
				DoEvents();
				Sleep(40);
			}
		}
		else
		{
			while ((!is_exit1) || (!is_exit2) || (!is_exit3) || (!is_exit4))
			{
				DoEvents();
				Sleep(40);
			}
		}
		is_Wrap = is_Unwrap = is_exit1 = is_exit2 = is_exit3 = is_exit4 = false;
		phase_exit = unphase_exit = phase_exit1 = unphase_exit1 = phase_exit2 = unphase_exit2 = phase_exit3 = unphase_exit3 = phase_exit4 = unphase_exit4 = false;

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
		}
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
		}

		if (p_directUnPhaseDataVec.size() > 0)
		{
			for (auto &i : p_directUnPhaseDataVec)
			{
				delete i;
			}
			p_directUnPhaseDataVec.clear();
		}
		if (p_directPhaseDataVec.size() > 0)
		{
			for (auto &i : p_directPhaseDataVec)
			{
				delete i;
			}
			p_directPhaseDataVec.clear();
		}
		PhaseGrayImgNum1 = 0;
		PhaseColorImgNum1 = 0;
		PhaseGrayImgNum2 = 0;
		PhaseColorImgNum2 = 0;
		PhaseGrayImgNum3 = 0;
		PhaseColorImgNum3 = 0;

		unPhaseGrayImgNum1 = 0;
		unPhaseColorImgNum1 = 0;
		unPhaseGrayImgNum2 = 0;
		unPhaseColorImgNum2 = 0;
		unPhaseGrayImgNum3 = 0;
		unPhaseColorImgNum3 = 0;
		CDialog::OnCancel();
	}
	else
		return;
}

BOOL CTipsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	ProgressBar.SetRange(0, PgLimit);
	SetDlgItemText(IDC_PERCENT, _T("0%"));
	ProgressBar.SetStep(1);
	ProgressBar.SetPos(0);

	return TRUE;
}