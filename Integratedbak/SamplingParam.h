#ifndef __SamplingPara__
#define __SamplingPara__

#pragma once
#include "DASDlg.h"
#include "SamplingProg.h"

// SamplingPara 对话框

class CSamplingPara : public CDialogEx
{
	DECLARE_DYNAMIC(CSamplingPara)

public:
	CSamplingPara(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSamplingPara();
	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SamplePara_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
private:
	CString	cSamplingNum;				// 控件值
	CString	CurrentDate;				// 采样时间
	CString	foldername;					// 文件夹名称
	void	BrowseCurrentAllFile(CString strDir);

public:
	afx_msg void OnBnClickedButtonReturn();
	afx_msg void OnBnClickedButtonContinue();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual void OnOK();
	virtual BOOL OnInitDialog();
};


#endif // !__SamplingPara__