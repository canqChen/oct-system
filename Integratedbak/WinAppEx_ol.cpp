// WinAppEx_ol.cpp : 实现文件
//

#include "stdafx.h"
#include "Integratedbak.h"
#include "WinAppEx_ol.h"


// CWinAppEx_ol

IMPLEMENT_DYNCREATE(CWinAppEx_ol, CWinAppEx)

CWinAppEx_ol::CWinAppEx_ol()
{
}

CWinAppEx_ol::~CWinAppEx_ol()
{
}

BOOL CWinAppEx_ol::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	return TRUE;
}

int CWinAppEx_ol::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinAppEx::ExitInstance();
}

BEGIN_MESSAGE_MAP(CWinAppEx_ol, CWinAppEx)
END_MESSAGE_MAP()


// CWinAppEx_ol 消息处理程序


BOOL CWinAppEx_ol::LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd)
{
	// TODO: 在此添加专用代码和/或调用基类

	//return CWinAppEx::LoadWindowPlacement(rectNormalPosition, nFflags, nShowCmd);
	return FALSE;
}


BOOL CWinAppEx_ol::StoreWindowPlacement(const CRect& rectNormalPosition, int nFflags, int nShowCmd)
{
	// TODO: 在此添加专用代码和/或调用基类

	//return CWinAppEx::StoreWindowPlacement(rectNormalPosition, nFflags, nShowCmd);
	return FALSE;
}
