#ifndef __MULTI_H__
#define __MULTI_H__

// CMultiDlg �Ի���

class CMultiDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMultiDlg)

public:
	CMultiDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMultiDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_Analysis_Multi };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	bool Uncheck();
	bool check();
	virtual void OnOK()	{	return;	}
	CString bg_dir;
public:
	static bool bRunning;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBnUnwrap();
	afx_msg void OnBnClickedBnWrap();
	afx_msg void OnBnClickedBnLoadBgM();
	afx_msg void OnBnClickedBnLoadFolderM();
	virtual void OnCancel();
};

#endif