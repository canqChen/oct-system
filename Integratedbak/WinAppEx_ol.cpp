// WinAppEx_ol.cpp : ʵ���ļ�
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
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int CWinAppEx_ol::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinAppEx::ExitInstance();
}

BEGIN_MESSAGE_MAP(CWinAppEx_ol, CWinAppEx)
END_MESSAGE_MAP()


// CWinAppEx_ol ��Ϣ�������


BOOL CWinAppEx_ol::LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd)
{
	// TODO: �ڴ����ר�ô����/����û���

	//return CWinAppEx::LoadWindowPlacement(rectNormalPosition, nFflags, nShowCmd);
	return FALSE;
}


BOOL CWinAppEx_ol::StoreWindowPlacement(const CRect& rectNormalPosition, int nFflags, int nShowCmd)
{
	// TODO: �ڴ����ר�ô����/����û���

	//return CWinAppEx::StoreWindowPlacement(rectNormalPosition, nFflags, nShowCmd);
	return FALSE;
}
