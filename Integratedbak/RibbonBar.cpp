// RibbonBar.cpp : 实现文件
//

#include "stdafx.h"
#include "Integratedbak.h"
#include "RibbonBar.h"


// CRibbonBar

IMPLEMENT_DYNAMIC(CRibbonBar, CMFCRibbonBar)

CRibbonBar::CRibbonBar()
{

}

CRibbonBar::~CRibbonBar()
{
}

BOOL CRibbonBar::OnShowRibbonQATMenu(CWnd * pWnd, int x, int y, CMFCRibbonBaseElement * pHit)
{

	return 0;
}

BOOL CRibbonBar::OnShowRibbonContextMenu(CWnd * pWnd, int x, int y, CMFCRibbonBaseElement * pHit)
{
	// do nothing
	return 0;
}



BEGIN_MESSAGE_MAP(CRibbonBar, CMFCRibbonBar)
END_MESSAGE_MAP()



// CRibbonBar 消息处理程序

