#ifndef __SamplingPara__
#define __SamplingPara__

#pragma once
#include "DASDlg.h"
#include "SamplingProg.h"

// SamplingPara �Ի���

class CSamplingPara : public CDialogEx
{
	DECLARE_DYNAMIC(CSamplingPara)

public:
	CSamplingPara(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSamplingPara();
	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SamplePara_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
private:
	CString	cSamplingNum;				// �ؼ�ֵ
	CString	CurrentDate;				// ����ʱ��
	CString	foldername;					// �ļ�������
	void	BrowseCurrentAllFile(CString strDir);

public:
	afx_msg void OnBnClickedButtonReturn();
	afx_msg void OnBnClickedButtonContinue();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual void OnOK();
	virtual BOOL OnInitDialog();
};


#endif // !__SamplingPara__