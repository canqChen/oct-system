#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <io.h>
#include "Myresource.h"

bool DataCheck(CString dataname)
{
	//�ж��ļ��Ƿ����
	if (_waccess(dataname, 0) == -1)
	{
		return false;
	}
	return true;
}
