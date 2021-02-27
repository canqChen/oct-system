#include "stdafx.h"
#include "GifEncoder.h"

ULONG_PTR m_gdiplusToken;
Gdiplus::GdiplusStartupInput StartupInput;
CGifEncoder::CGifEncoder()
{
	GdiplusStartup(&m_gdiplusToken, &StartupInput, NULL); // GDI+初始化
	m_started = false;
	m_width = 320;
	m_height = 240;
	m_delayTime = 100;
	m_repeatNum = 0;
	m_haveFrame = false;
	m_pStrSavePath = NULL;
	m_pImage = NULL;
}

CGifEncoder::~CGifEncoder()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken); // GDI+卸载
	if (NULL != m_pStrSavePath)
	{
		delete m_pStrSavePath;
	}

	if (NULL != m_pImage)
	{
		delete m_pImage;
	}

	size_t size = m_pBitMapVec.size();
	if (!m_pBitMapVec.empty())
	{
		for (size_t ix = 0; ix < size; ix++)
		{
			delete m_pBitMapVec[ix];
			m_pBitMapVec[ix] = NULL;
		}
	}
}
Bitmap* CImage2Image(const CImage* pImage)
{
	if (!pImage)
		return NULL;

	Gdiplus::Bitmap* image = new Gdiplus::Bitmap(pImage->GetWidth(), pImage->GetHeight());
	Gdiplus::Rect bound(0, 0, pImage->GetWidth(), pImage->GetHeight());
	Gdiplus::BitmapData lockedBitmapData;
	int bpp = pImage->GetBPP();
	int imageRowSize = pImage->GetWidth() * (bpp / 8);

	if (bpp == 24)
	{
		image->LockBits(&bound, Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &lockedBitmapData);
	}
	else if (bpp == 32)
	{
		image->LockBits(&bound, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &lockedBitmapData);
	}
	else
	{
		AfxDebugBreak();
		return NULL;
	}

	BYTE* pSrcPointer = (BYTE*)pImage->GetBits();
	BYTE* pDstPointer = (BYTE*)lockedBitmapData.Scan0;

	for (int i = 0; i < pImage->GetHeight(); i++)
	{
		memcpy(pDstPointer, pSrcPointer, imageRowSize);
		pSrcPointer += pImage->GetPitch();
		pDstPointer += lockedBitmapData.Stride;
	}

	image->UnlockBits(&lockedBitmapData);
	return image;
}

bool CGifEncoder::StartEncoder(wstring &saveFilePath)
{
	bool flag = true;

	if (NULL != m_pStrSavePath)
	{
		delete m_pStrSavePath;
		m_pStrSavePath = NULL;
	}

	m_pStrSavePath = new wstring(saveFilePath);
	m_started = true;

	return(flag);
}
bool CGifEncoder::AddFrame(BITMAPINFO* bitmapinfo, BYTE *pImage)
{
	if (!m_started || NULL == pImage)
	{
		return(false);
	}

	bool flag = true;
	if (!m_haveFrame)
	{
		m_pImage = new Gdiplus::Bitmap(bitmapinfo, pImage);
		m_haveFrame = true;
		return(true);
	}

	Gdiplus::Bitmap *pBitMap = new Gdiplus::Bitmap(bitmapinfo, pImage);

	m_pBitMapVec.push_back(pBitMap);

	return(flag);
}
bool CGifEncoder::AddFrame(CImage *pImage)
{
	if (!m_started || NULL == pImage)
	{
		return(false);
	}

	bool flag = true;
	if (!m_haveFrame)
	{
		m_pImage = CImage2Image(pImage);

		m_haveFrame = true;
		return(true);
	}

	m_pBitMapVec.push_back(CImage2Image(pImage));

	return(flag);
}

bool CGifEncoder::AddFrame(wstring &framePath)
{
	if (!m_started)
	{
		return(false);
	}

	bool flag = true;
	Gdiplus::Status statue;
	if (!m_haveFrame)
	{
		m_pImage = new Gdiplus::Bitmap(m_width, m_height);
		Gdiplus::Graphics g(m_pImage);

		Gdiplus::Bitmap bitmap(framePath.c_str());
		g.DrawImage(&bitmap, 0, 0, m_width, m_height);

		m_haveFrame = true;
		return(true);
	}

	Gdiplus::Bitmap     *pBitMap = new Gdiplus::Bitmap(m_width, m_height);
	Gdiplus::Graphics   g(pBitMap);

	Gdiplus::Bitmap bitmap(framePath.c_str());
	statue = g.DrawImage(&bitmap, 0, 0, m_width, m_height);

	m_pBitMapVec.push_back(pBitMap);

	return(flag);
}

bool CGifEncoder::FinishEncoder()
{
	if (!m_started || !m_haveFrame)
	{
		return(false);
	}

	bool    flag = true;
	Gdiplus::Status statue;

	//1.0 设置图像的属性
	SetImagePropertyItem();

	//2.0 保存第一副图像
	GUID                        gifGUID;
	Gdiplus::EncoderParameters  encoderParams;
	GetEncoderClsid(L"image/gif", &gifGUID);

	encoderParams.Count = 1;
	encoderParams.Parameter[0].Guid = Gdiplus::EncoderSaveFlag;
	encoderParams.Parameter[0].NumberOfValues = 1;
	encoderParams.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;//第一帧需要设置为MultiFrame

	long firstValue = Gdiplus::EncoderValueMultiFrame;
	encoderParams.Parameter[0].Value = &firstValue;

	m_pImage->Save(m_pStrSavePath->c_str(), &gifGUID, &encoderParams);

	//3.0 保存剩余的图像
	size_t size = m_pBitMapVec.size();
	firstValue = Gdiplus::EncoderValueFrameDimensionTime;
	encoderParams.Parameter[0].Value = &firstValue;
	for (size_t ix = 0; ix < size; ix++)
	{
		statue = m_pImage->SaveAdd(m_pBitMapVec[ix], &encoderParams);
	}

	delete m_pImage;
	m_pImage = NULL;
	for (int i = 0; i < m_pBitMapVec.size(); i++)
	{
		delete m_pBitMapVec[i];
	}
	m_pBitMapVec.clear();
	m_started = false;
	m_haveFrame = false;
	return(flag);
}

void CGifEncoder::SetDelayTime(int ms)
{
	if (ms > 0)
	{
		m_delayTime = ms / 10.0f;
	}
}

void CGifEncoder::SetRepeatNum(int num)
{
	if (num >= 0)
	{
		m_repeatNum = num;
	}
}

void CGifEncoder::SetFrameRate(float fps)
{
	if (fps > 0)
	{
		m_delayTime = 100.0f / fps;
	}
}

void CGifEncoder::SetFrameSize(int width, int height)
{
	if (!m_haveFrame)
	{
		m_width = width;
		m_height = height;

		if (m_width < 1)
			m_width = 320;

		if (m_height < 1)
			height = 240;
	}
}

void CGifEncoder::SetImagePropertyItem()
{
	if (!m_started || NULL == m_pImage)
	{
		return;
	}

	Gdiplus::PropertyItem propertyItem;

	//1.0 设置动画循环的次数
	short sValue = m_repeatNum; //0 -- 不限次数
	propertyItem.id = PropertyTagLoopCount;
	propertyItem.length = 2; //字节数
	propertyItem.type = PropertyTagTypeShort;
	propertyItem.value = &sValue;
	m_pImage->SetPropertyItem(&propertyItem);

	//2.0 设置每副图像延迟的时间，从第一副开始
	long lImageNum = 1 + m_pBitMapVec.size();
	long *plValue = new long[lImageNum];
	for (int ix = 0; ix < lImageNum; ix++)
	{
		plValue[ix] = m_delayTime; //可以设置不一样
	}
	propertyItem.id = PropertyTagFrameDelay;
	propertyItem.length = 4 * lImageNum;//字节数
	propertyItem.type = PropertyTagTypeLong;
	propertyItem.value = plValue; //不限次数
	m_pImage->SetPropertyItem(&propertyItem);

	delete[]plValue;
	plValue = NULL;
}

bool CGifEncoder::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0, size = 0;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return false;  // Failure

	Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
	bool found = false;
	for (UINT ix = 0; !found && ix < num; ++ix)
	{
		if (_wcsicmp(pImageCodecInfo[ix].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[ix].Clsid;
			found = true;
			break;
		}
	}

	free(pImageCodecInfo);
	return found;
}