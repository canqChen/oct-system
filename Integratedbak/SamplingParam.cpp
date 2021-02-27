// SamplingPara.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Integratedbak.h"
#include "SamplingParam.h"
#include "afxdialogex.h"


extern double exposure;
extern double gain;
extern CString SavePath;

CString	path;
int		filename_count = 1;
int		interval;					// �������
int		SamplingNumber;				// ��������

// CSamplingPara �Ի���

IMPLEMENT_DYNAMIC(CSamplingPara, CDialogEx)

CSamplingPara::CSamplingPara(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SampleParam, pParent)
{

}

CSamplingPara::~CSamplingPara()
{
}

void CSamplingPara::DoDataExchange(CDataExchange* pDX)
{
	CTime ct = CTime::GetCurrentTime();
	CurrentDate = ct.Format("20%y%m%d_");
	DDX_Text(pDX, IDC_EDIT_Camera_SamplingDate, CurrentDate);
	DDX_Text(pDX, IDC_EDIT_Camera_Interval, interval);
	DDX_Text(pDX, IDC_EDIT_Camera_SamplingNumber, SamplingNumber);
	DDX_Text(pDX, IDC_EDIT_Camera_Filename, foldername);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSamplingPara, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_RETURN, &CSamplingPara::OnBnClickedButtonReturn)
	ON_BN_CLICKED(IDC_BUTTON_CONTINUE, &CSamplingPara::OnBnClickedButtonContinue)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CSamplingPara::OnInitDialog()
{
	SetBackgroundColor(RGB(255, 255, 255));
	CDialogEx::OnInitDialog();
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
	SetIcon(m_hIcon, false);		// ����Сͼ��

	interval = 200;
	SamplingNumber = 10;
	foldername.Format(_T("%s%.2d"), CurrentDate, filename_count);
	GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->SetWindowTextW(_T("10"));
	GetDlgItem(IDC_EDIT_Camera_Interval)->SetWindowTextW(_T("200"));


	// �������ݱ���·��
	path.Format(_T("%s%s%s"), SavePath, foldername, _T("\\"));
	BrowseCurrentAllFile(SavePath);
	GetDlgItem(IDC_EDIT_Camera_Filename)->SetWindowTextW(foldername);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

// CSamplingPara ��Ϣ�������

void CSamplingPara::BrowseCurrentAllFile(CString strDir)
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
		//strPath������Ҫ��ȡTestĿ¼�µ��ļ��к��ļ�������·����
		CString tPath = path;
		tPath.Delete(path.GetLength() - 1);
		if (tPath == strPath)
		{
			filename_count ++;
			foldername.Format(_T("%s%.2d"), CurrentDate, filename_count);
			path.Format(_T("%s%s%s"), SavePath, foldername, _T("\\"));
		}
	}
}

void CSamplingPara::OnBnClickedButtonReturn()
{
	CDialogEx::OnCancel();
}
void CSamplingPara::OnBnClickedButtonContinue()
{
	if (MessageBox(_T("��ʼ������"), _T("��ʾ"), MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
		return;

	GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->GetWindowTextW(cSamplingNum);
	SamplingNumber = _wtoi(cSamplingNum);

	// �������Ƿ�����
	if (SamplingNumber == 0 || SamplingNumber > 500)
	{
		MessageBox(_T("����ȷ��д����������������ΧΪ1��500"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		return;
	}
	if (interval == 0)
	{
		MessageBox(_T("����ȷ��д����"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		return;
	}

	AcquiringParam param = { exposure,gain,path,interval,SamplingNumber };

	CSamplingProg SamplingProgDlg;
	SamplingProgDlg.param_ = &param;
	ShowWindow(false);
	SamplingProgDlg.DoModal();
	OnBnClickedButtonReturn();
}

BOOL CSamplingPara::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_TAB)
	{
		if (GetDlgItem(IDC_EDIT_Camera_SamplingNumber) == GetFocus())
		{
			GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->GetWindowTextW(cSamplingNum);
			SamplingNumber = _wtoi(cSamplingNum);
			if (SamplingNumber > 500 || SamplingNumber < 1)
			{
				MessageBox(_T("����������ΧΪ1��500"), _T("����"), MB_OK | MB_ICONWARNING);
				GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->SetWindowText(_T("1"));
				GetDlgItem(IDC_EDIT_Camera_SamplingNumber)->SetFocus();
				SamplingNumber = 0;
			}
			return FALSE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CSamplingPara::OnOK()
{
	// do nothing
}

