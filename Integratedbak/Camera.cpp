#include "stdafx.h"
#include "Camera.h"

// 参考http://blog.sina.com.cn/s/blog_673ccb5b01019za8.html
// 设置BitMap的参数
// 图像宽、图像高、图像文件头
void SetBitmapInfoMono(UINT Width, UINT Height, LPBITMAPINFO _lpBitsInfo)
{
	_lpBitsInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	_lpBitsInfo->bmiHeader.biWidth = Width;
	_lpBitsInfo->bmiHeader.biHeight = 0 - Height;//设置成负数，可以把图像原本倒转的还原成正常的
	_lpBitsInfo->bmiHeader.biPlanes = 1;
	_lpBitsInfo->bmiHeader.biSizeImage = Width*Height;
	_lpBitsInfo->bmiHeader.biXPelsPerMeter = 0;
	_lpBitsInfo->bmiHeader.biYPelsPerMeter = 0;
	_lpBitsInfo->bmiHeader.biClrUsed = 0;
	_lpBitsInfo->bmiHeader.biClrImportant = 0;
	_lpBitsInfo->bmiHeader.biBitCount = 8;
	_lpBitsInfo->bmiHeader.biCompression = 0;
	for (int k = 0;k < 256;k++)
	{
		_lpBitsInfo->bmiColors[k].rgbRed = _lpBitsInfo->bmiColors[k].rgbGreen = _lpBitsInfo->bmiColors[k].rgbBlue = (BYTE)k;
		_lpBitsInfo->bmiColors[k].rgbReserved = 0;
	}
}
