// SamplingProg.cpp : 实现文件
//

#include "stdafx.h"
#include "Integratedbak.h"
#include "SamplingProg.h"
#include "SingleDlg.h"
#include "MultiDlg.h"
#include "afxdialogex.h"

#pragma warning(disable:4244)

extern CString folder_dir;
extern CWnd* mSingleWnd;
extern CWnd* mMultiWnd;

int s1, s2 = 0;
bool bThreadExit = false;
CWnd* DlgWnd;
CString folder_name;
int tempnum_;
int progress_value = 0;

// SamplingProg 对话框

IMPLEMENT_DYNAMIC(CSamplingProg, CDialogEx)

CSamplingProg::CSamplingProg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SampleProg, pParent)
{
}

CSamplingProg::~CSamplingProg()
{

}

void CSamplingProg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_PROGRESS_Acquiring, ProgressSampling);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSamplingProg, CDialogEx)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// CSamplingProg 消息处理程序



BOOL CSamplingProg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DlgWnd = this;
	s1 = GetTickCount();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CSamplingProg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (bAcquiring)
	{
		SetCursor(LoadCursor(NULL, IDC_WAIT));
		return true;
	}
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}

static UINT Thread_Update(LPVOID lpParam)
{
	bThreadExit = false;
	CString cstr;
	CProgressCtrl* progress = (CProgressCtrl*)DlgWnd->GetDlgItem(IDC_PROGRESS_Acquiring);
	while (Vimba::GetInstance()->acquisition_count != 0)
	{
		cstr.Format(_T("%d%s%d"), Vimba::GetInstance()->acquisition_count, _T("/"), tempnum_);
		DlgWnd->GetDlgItem(IDC_STATIC_Percent)->SetWindowTextW(cstr);
		progress->SetPos(progress_value);
		DoEvents();
	}
	bThreadExit = true;
	return 0;
}

BOOL CSamplingProg::PreTranslateMessage(MSG* pMsg)
{
	do
	{
		s2 = GetTickCount();
	} while (s2 - s1 < 100);

	if (!Vimba::GetInstance()->InitCamera())
	{
		OnCancel();
		Vimba::GetInstance()->ApiShutdown();
		return CDialogEx::PreTranslateMessage(pMsg);
	}

	/////////////////////////////
	Vimba::GetInstance()->SetParam();
	/////////////////////////////

	bAcquiring = true;
	Vimba::GetInstance()->acquisition_count = -1;
	tempnum_ = param_->SamplingNumber_;
	AfxBeginThread(Thread_Update, this);
	ProgressSampling.SetRange(0, param_->SamplingNumber_);
	Vimba::GetInstance()->Acquiring(param_->exposure_,
		param_->gain_,
		param_->interval_,
		param_->SamplingNumber_,
		param_->path_,
		progress_value);
	Vimba::GetInstance()->ApiShutdown();
	bAcquiring = false;

	while (bThreadExit == false)
	{
		DoEvents();
	}
	folder_dir = param_->path_;
	DlgWnd->GetDlgItem(IDC_STATIC_Acquiring)->SetWindowTextW(_T("采样完毕"));
	folder_name = folder_dir;
	folder_name.TrimRight(_T("\\"));
	folder_name = folder_name.Right(11);
	mSingleWnd->GetDlgItem(IDC_EDIT_FolderNameS)->SetWindowTextW(folder_name);
	mMultiWnd->GetDlgItem(IDC_EDIT_FolderNameM)->SetWindowTextW(folder_name);
	CString tmp;
	tmp.Format(_T("%s%d"), _T("im_"), 1);
	mSingleWnd->GetDlgItem(IDC_EDIT_IMG1)->SetWindowTextW(tmp);
	tmp.Format(_T("%s%d"), _T("im_"), 2);
	mSingleWnd->GetDlgItem(IDC_EDIT_IMG2)->SetWindowTextW(tmp);
	Sleep(250);
	ProgressSampling.SetPos(0);
	OnCancel();

	return CDialogEx::PreTranslateMessage(pMsg);
}
