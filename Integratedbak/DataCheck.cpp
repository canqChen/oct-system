#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <io.h>
#include "Myresource.h"

bool DataCheck(CString dataname)
{
	//判断文件是否存在
	if (_waccess(dataname, 0) == -1)
	{
		return false;
	}
	return true;
}
