#pragma once



// CWinAppEx_ol

class CWinAppEx_ol : public CWinAppEx
{
	DECLARE_DYNCREATE(CWinAppEx_ol)

protected:
	CWinAppEx_ol();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CWinAppEx_ol();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd);
	virtual BOOL StoreWindowPlacement(const CRect& rectNormalPosition, int nFflags, int nShowCmd);
};


