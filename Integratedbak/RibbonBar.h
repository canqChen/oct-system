#pragma once

// CRibbonBar

class CRibbonBar : public CMFCRibbonBar
{
	DECLARE_DYNAMIC(CRibbonBar)

public:
	CRibbonBar();
	virtual ~CRibbonBar();

protected:
	DECLARE_MESSAGE_MAP()

private:
	BOOL OnShowRibbonQATMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit);
	BOOL OnShowRibbonContextMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit);
};


