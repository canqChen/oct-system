#ifndef __SamplingProg__
#define __SamplingProg__

#pragma once
#include "DASDlg.h"
//#include "SamplingParam.h"

struct AcquiringParam
{
	double		exposure_;
	double		gain_;
	CString		path_;
	int			interval_;
	int			SamplingNumber_;
};

// SamplingProg 对话框

class CSamplingProg : public CDialogEx
{
	DECLARE_DYNAMIC(CSamplingProg)

public:
	CSamplingProg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSamplingProg();

	static CSamplingProg* GetInstance()
	{
		static CSamplingProg pInstance;
		return &pInstance;
	}

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SampleProg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	bool		bAcquiring;
	CProgressCtrl ProgressSampling;

public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	AcquiringParam* param_;
};

#endif // !__SamplingProg__
