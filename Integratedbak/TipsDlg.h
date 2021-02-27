#ifndef __TIPSDLG_H__
#define __TIPSDLG_H__

#include "resource.h"
#include "afxwin.h"

extern int unphaseProcess;
extern int phaseProcess;
extern int singleProcess;

// CTipsDlg 对话框

class CTipsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTipsDlg)

public:
	CTipsDlg(CWnd* pParent = NULL);   // 标准构造函数
	CTipsDlg(int value);
	virtual ~CTipsDlg();
	 void destroyInstance()
	{
		if (_tipsdlg != nullptr)
		{
			Sleep(300);
			_tipsdlg->CDialog::OnCancel();
			/*delete _tipsdlg;
			_tipsdlg = nullptr;*/
		}
	}
	static void initInstance(int value)
	{
		if (_tipsdlg!=nullptr)
		{
			delete _tipsdlg;
			_tipsdlg = nullptr;
		}
		if (_tipsdlg == nullptr)
		{
			unphaseProcess = 0;
			phaseProcess = 0;
			singleProcess = 0;
			CWnd *wnd = AfxGetMainWnd();
			_tipsdlg = new CTipsDlg(value);
			_tipsdlg->Create(IDD_DIALOG_Tip, wnd);
			_tipsdlg->GetDlgItem(IDC_STATIC_TEXT)->SetWindowText(_T("数据正在处理中，请稍等~"));
			_tipsdlg->ShowWindow(SW_SHOW);
		}
	}
	static CTipsDlg* getInstance()
	{
		return _tipsdlg;
	}
	void update(int value)
	{
		ProgressBar.SetPos(value);
		CString ch;
		ch.Format(_T("%d%s"), int(value*1.0 / PgLimit * 100.0), _T("%"));
		SetDlgItemText(IDC_PERCENT, ch);
	}

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_Tip };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	CProgressCtrl ProgressBar;
	int PgLimit;
	CStatic Percent;

	static CTipsDlg * _tipsdlg;
	virtual void OnOK()
	{
		return;
	}
private:
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (CTipsDlg::_tipsdlg)
				delete CTipsDlg::_tipsdlg;
		}
	};
	static CGarbo Garbo;

};

#endif // !__TIPSDLG_H__