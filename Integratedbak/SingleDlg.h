#pragma once

// CSingleDlg 对话框

class CSingleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSingleDlg)

public:
	CSingleDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSingleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_Analysis_Single};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	virtual void OnOK() { OnBnClickedBnRun(); }
	CString img1_dir;
	CString img2_dir;
	CString bg_dir;
public:
	static bool bRunning;
	afx_msg void OnBnClickedBnLoadImg1();
	afx_msg void OnBnClickedBnLoadImg2();
	afx_msg void OnBnClickedBnLoadBgS();
	afx_msg void OnBnClickedBnRun();
	afx_msg void OnBnClickedBnLoadFolderS();

	virtual void OnCancel();
};
