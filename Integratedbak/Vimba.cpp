#include "stdafx.h"
#include "Vimba.h"
#include "Camera.h"
#include "DASDlg.h"
#include "MyAlgorithm.h"

VimbaSystem &ApiController = VimbaSystem::GetInstance();		// 获取单例

#pragma warning(disable:4244)


extern IMGINFO DataInfo;
extern CRect PictureCRect;
extern CString XmlFile;
extern UINT Count;
BYTE *p_Img = nullptr;
std::vector<VmbUint8_t*> vecData;
enum { NUM_FRAMES = 1 };
extern CString filename_prefix;

typedef struct data
{
	CString path_;
	int samplingnum_;
}DATA;


Vimba::Vimba()
{
	pBuffer = NULL;
	ImageBuffer = NULL;
	BitmapInfoMono = NULL;
	offsetX = 50;
	offsetY = 50;
	bIsCameraGet = false;
	bAcquistion = false;
	bSavingData = false;
	scale = 1.0f;
	nSize = 0;
	colormode = MONO;

	BitmapInfoMono = (LPBITMAPINFO)GlobalAllocPtr(GHND, sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
	SetBitmapInfoMono(1388, 1038, BitmapInfoMono);

	BitmapInfoRGB.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfoRGB.biWidth = 1388;
	BitmapInfoRGB.biHeight = 0 - 1038;
	BitmapInfoRGB.biPlanes = 1;
	BitmapInfoRGB.biBitCount = 24;
	BitmapInfoRGB.biCompression = BI_RGB;
	BitmapInfoRGB.biSizeImage = (((1388 / 2 * 24 + 31) & ~31)) / 8 * 1038;
	BitmapInfoRGB.biXPelsPerMeter = 0;
	BitmapInfoRGB.biYPelsPerMeter = 0;
	BitmapInfoRGB.biClrUsed = 0;
	BitmapInfoRGB.biClrImportant = 0;
}

Vimba::~Vimba()
{
	ImageBuffer = NULL;
	/*if (pLastBuffer != NULL)
		delete[] pLastBuffer;*/
	GlobalFree(BitmapInfoMono);
}

static UINT Thread_DataSave(LPVOID lpParam)
{
	Vimba::GetInstance()->bSavingData = true;
	DATA *pt = (DATA*)lpParam;
	CString path = pt->path_;
	int samplingnum = pt->samplingnum_;

	FILE *fp;
	fp = NULL;
	CString cfile;
	CreateDirectory(path, NULL);
	for (int i = 0; i < samplingnum; i++)
	{
		fp = NULL;
		cfile.Format(_T("%s%s%d%s"), path, filename_prefix, i + 1, _T(".bin"));
		_wfopen_s(&fp, cfile, _T("wb"));

		fwrite(vecData[i], sizeof(VmbUint8_t), Vimba::GetInstance()->nSize, fp);
		fclose(fp);
		DoEvents();
	}
	for (auto &i : vecData)
	{
		delete i;
	}
	vecData.clear();

	Vimba::GetInstance()->bSavingData = false;
	return 0;
}

void Vimba::Acquiring(double exposure, double gain, int interval, int samplingnum, CString path, int &ProgressValue)
{
	bAcquistion = true;
	acquisition_count = 1;
	VmbUint32_t	timeout;

	err = SetPixelFormat(VmbPixelFormatMono12);
	if (VmbErrorSuccess != err)
	{
		TRACE("PixelFormatNotSet!");
	}
	err = GetImageSize(nSize);

	SetExposure(exposure);
	SetGain(gain);

	if (exposure > 1000)
	{
		timeout = exposure / 1000 + TO_offset * 2;
	}
	else
	{
		timeout = TO_offset;
	}

	long t1, t2;
	for (int i = 0; i < samplingnum; i++)
	{
		DoEvents();
		err = VmbErrorOther;
		UINT trycount = 0;
		AdjustPacketSize();
		while (1)
		{
			err = SetStreamBytesPerSecond(115000000);
			trycount++;
			if (VmbErrorSuccess == err)
				break;
			else {
				//CString cstr;
				//cstr.Format(_T("%d"), err);
				//AfxMessageBox(cstr);
				if (trycount > 20)
				{
					AfxGetMainWnd()->MessageBox(_T("Acquisition Failed"), _T("Warning!"), MB_OK | MB_ICONWARNING);
					return;
				}
			}
		}
		t1 = GetTickCount();
		acquisition_count = i + 1;
		cameras[0]->AcquireSingleImage(pFrameAcquire, timeout);
		pFrameAcquire->GetBuffer(pBuffer);

		VmbUint8_t* tmpdata = new VmbUint8_t[nSize];
		memcpy(tmpdata, pBuffer, nSize);
		vecData.push_back(tmpdata);
		pBuffer = NULL;
		ProgressValue = i + 1;
		do
		{
			DoEvents();
			t2 = GetTickCount();
		} while (t2 - t1 < interval);
	}

	DATA pt = { path, samplingnum };
	AfxBeginThread(Thread_DataSave, &pt);

	bIsCameraGet = false;
	bAcquistion = false;

	acquisition_count = 0;

	AfxGetMainWnd()->MessageBox(_T("采样完毕"), _T("提示"), MB_OK | MB_ICONWARNING);

	return;
}

void Vimba::BgShot(double exposure, double gain, CString filename)
{
	VmbUint32_t timeout;
	SetStreamBytesPerSecond(115000000);
	err = SetPixelFormat(VmbPixelFormatMono12);
	err = GetImageSize(nSize);

	VmbUint8_t* Bgdata = new VmbUint8_t[nSize];
	SetExposure(exposure);
	SetGain(gain);

	if (exposure > 1000)
	{
		timeout = exposure / 1000 + TO_offset * 2;
	}
	else
	{
		timeout = TO_offset;
	}

	cameras[0]->AcquireSingleImage(pFrame, timeout);
	pFrame->GetBuffer(pBuffer);
	memcpy(Bgdata, pBuffer, nSize);
	pBuffer = NULL;

	FILE* fp;
	fp = NULL;
	_wfopen_s(&fp, filename, _T("wb"));
	fwrite(Bgdata, sizeof(VmbUint8_t), nSize, fp);
	fclose(fp);
	delete[] Bgdata;
	Bgdata = NULL;
	err = SetPixelFormat(VmbPixelFormatMono8);
	return;
}

bool Vimba::InitCamera()
{
	if (!bIsCameraGet)									// 检查相机是否被获取
	{
		if (ApiController.Startup() != VmbErrorSuccess)
		{
			AfxGetMainWnd()->MessageBox(_T("API初始化失败"), _T("错误"), MB_OK | MB_ICONERROR);
			return false;
		}
		ApiController.GetCameras(cameras);				// 获取相机
		if (!cameras.empty())							// 检查相机列表是否为空
		{

			err = cameras[0]->Open(VmbAccessModeFull);	// 启动相机
			AdjustPacketSize();
			if (VmbErrorSuccess == err)
			{
				bIsCameraGet = true;
				if (VmbErrorSuccess != err)
				{
					if (VmbErrorTimeout == err)
						AfxGetMainWnd()->MessageBox(_T("回调超时"), _T("错误"), MB_OK | MB_ICONERROR);
					return false;
				}

				// Load default settings
				size_t len = XmlFile.GetLength();
				size_t i;
				char* xml = new char[len + 1];
				wcstombs_s(&i, xml, len + 1, XmlFile, _TRUNCATE);

				std::string XML_str;
				XML_str = xml;

				VmbFeaturePersistSettings_t settingsStruct;
				settingsStruct.loggingLevel = 4;
				settingsStruct.maxIterations = 5;
				settingsStruct.persistType = VmbFeaturePersistNoLUT;

				cameras[0]->LoadCameraSettings(xml);
				delete xml;

				if (m_mode == NORMAL)
				{
					err = SetPixelFormat(VmbPixelFormatMono8);
					SetParam();
				}
				else
				{
					err = SetPixelFormat(VmbPixelFormatMono12);

					SetParam();
				}

				err = GetHeight(nHeight);
				err = GetWidth(nWidth);
				err = GetImageSize(nSize);

				return true;
			}
			if (VmbErrorNotFound == err)
			{
				AfxGetMainWnd()->MessageBox(_T("未找到相机，请检查相机连接并重试"), _T("提示"), MB_OK | MB_ICONWARNING);
				return false;
			}
			else
			{
				CString cstr;
				cstr.Format(_T("%s%d"), _T("相机启动失败，错误代码"), (int)err);
				AfxGetMainWnd()->MessageBox(cstr, _T("错误"), MB_OK | MB_ICONERROR);
				return false;
			}

		}
		else
		{
			AfxGetMainWnd()->MessageBox(_T("未找到相机，请检查相机连接并重试"), _T("提示"), MB_OK | MB_ICONWARNING);
			return false;
		}
	}
	return true;
}

bool run=false;

void Vimba::Draw(CWnd *pWnd)
{
	if (pBuffer == NULL||pWnd==nullptr)
		return;
	if (CDASDlg::GetInstance()->bMonitor == true)
	{
		CMemDC DCMem(*(pWnd->GetDC()), PictureCRect);
		if (DCMem.GetDC() == nullptr ||DCMem.GetDC().m_hDC==nullptr)
		{
			return;
		}
		
		int RectW = PictureCRect.right - PictureCRect.left;
		int RectH = PictureCRect.Height();
		int SrcW = 0;
		int SrcH = 0;
		int bmpWidthBytes = 0;
		switch (m_mode)
		{
		case Vimba::NORMAL:
			switch (colormode)
			{
			case Vimba::MONO:
				DCMem.GetDC().FillSolidRect(PictureCRect, RGB(205, 205, 205));
				BitmapInfoMono->bmiHeader.biWidth = nWidth;
				BitmapInfoMono->bmiHeader.biHeight = -nHeight;
				BitmapInfoMono->bmiHeader.biSizeImage = nHeight*nWidth;
				SrcW = nWidth * scale;
				SrcH = nHeight * scale;
				DCMem.GetDC().SetStretchBltMode(HALFTONE);
				::StretchDIBits(DCMem.GetDC().GetSafeHdc(),
					PictureCRect.left, PictureCRect.top,
					RectW, RectH,
					(nWidth - SrcW)*0.01*(offsetX - 50) + (nWidth - SrcW) / 2,
					-(nHeight - SrcH)*0.01*(offsetY - 50) + (nHeight - SrcH) / 2,
					SrcW, SrcH,
					pBuffer, BitmapInfoMono,
					DIB_RGB_COLORS, SRCCOPY);
				break;
			case Vimba::RGB:
				DCMem.GetDC().FillSolidRect(PictureCRect, RGB(205, 205, 205));
				pDrawBuffer = new VmbUchar_t[nSize];
				memcpy(pDrawBuffer, pBuffer, nSize);
				SrcW = nWidth * scale;
				SrcH = nHeight * scale;
				BitmapInfoRGB.biWidth = nWidth;
				BitmapInfoRGB.biHeight = -nHeight;
				BitmapInfoRGB.biSizeImage = ((nWidth * 8 + 31) & ~31) / 8 * nHeight;
				ColorDataTransform();
				delete[] pDrawBuffer;
				pDrawBuffer = NULL;
				DCMem.GetDC().SetStretchBltMode(HALFTONE);
				::StretchDIBits(DCMem.GetDC().GetSafeHdc(),
					PictureCRect.left, PictureCRect.top,
					RectW, RectH,
					(nWidth - SrcW)*0.01*(offsetX - 50) + (nWidth - SrcW) / 2,
					-(nHeight - SrcH)*0.01*(offsetY - 50) + (nHeight - SrcH) / 2,
					SrcW, SrcH,
					mImage, (BITMAPINFO*)&BitmapInfoRGB,
					DIB_RGB_COLORS, SRCCOPY);
				delete[] mBuffer;
				delete[] mImage;
				mBuffer = NULL;
				mImage = NULL;
				break;
			default:
				break;
			}
			break;
		case Vimba::AMP_FREQ:
			if (p_Img != nullptr)
			{
				DCMem.GetDC().FillSolidRect(PictureCRect, RGB(255, 255, 255));
				bmpWidthBytes = (((DataInfo.endCol-DataInfo.startCol+1) * 24 + 31) & ~31) / 8;
				BITMAPINFOHEADER BitmapInfo;
				BitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
				BitmapInfo.biWidth = (DataInfo.endCol - DataInfo.startCol + 1);
				BitmapInfo.biHeight = -(DataInfo.endRow - DataInfo.startRow + 1);
				BitmapInfo.biSizeImage = bmpWidthBytes*(DataInfo.endRow - DataInfo.startRow + 1);
				BitmapInfo.biPlanes = 1;
				BitmapInfo.biBitCount = 24;
				BitmapInfo.biCompression = BI_RGB;
				BitmapInfo.biXPelsPerMeter = 0;
				BitmapInfo.biYPelsPerMeter = 0;
				BitmapInfo.biClrUsed = 0;
				BitmapInfo.biClrImportant = 0;
				
				SrcW = (DataInfo.endCol - DataInfo.startCol + 1) * scale;
				SrcH = (DataInfo.endRow - DataInfo.startRow + 1) * scale;

				DCMem.GetDC().SetStretchBltMode(HALFTONE);
				::StretchDIBits(DCMem.GetDC().GetSafeHdc(),
					PictureCRect.left+150, PictureCRect.top,
					RectW-300, RectH,
					((DataInfo.endCol - DataInfo.startCol + 1) - SrcW)*0.01*(offsetX - 50) + ((DataInfo.endCol - DataInfo.startCol + 1) - SrcW) / 2,
					-((DataInfo.endRow - DataInfo.startRow + 1) - SrcH)*0.01*(offsetY - 50) + ((DataInfo.endRow - DataInfo.startRow + 1) - SrcH) / 2,
					SrcW, SrcH,
					p_Img, (BITMAPINFO *)&BitmapInfo,
					DIB_RGB_COLORS, SRCCOPY);
			}
			break;
		case Vimba::PHASE_FREQ:
			if (p_Img != nullptr)
			{
				DCMem.GetDC().FillSolidRect(PictureCRect, RGB(255, 255, 255));
				bmpWidthBytes = (((DataInfo.endCol - DataInfo.startCol + 1) * 24 + 31) & ~31) / 8;
				BITMAPINFOHEADER BitmapInfo;
				BitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
				BitmapInfo.biWidth = (DataInfo.endCol - DataInfo.startCol + 1);
				BitmapInfo.biHeight = -(DataInfo.endRow - DataInfo.startRow + 1);
				BitmapInfo.biSizeImage = bmpWidthBytes*(DataInfo.endRow - DataInfo.startRow + 1);
				BitmapInfo.biPlanes = 1;
				BitmapInfo.biBitCount = 24;
				BitmapInfo.biCompression = BI_RGB;
				BitmapInfo.biXPelsPerMeter = 0;
				BitmapInfo.biYPelsPerMeter = 0;
				BitmapInfo.biClrUsed = 0;
				BitmapInfo.biClrImportant = 0;
				
				SrcW = (DataInfo.endCol - DataInfo.startCol + 1)*scale;
				SrcH = (DataInfo.endRow - DataInfo.startRow + 1)*scale;
				DCMem.GetDC().SetStretchBltMode(HALFTONE);
				::StretchDIBits(DCMem.GetDC().GetSafeHdc(),
					PictureCRect.left+150, PictureCRect.top,
					RectW-300, RectH,
					((DataInfo.endCol - DataInfo.startCol + 1) - SrcW)*0.01*(offsetX - 50) + ((DataInfo.endCol - DataInfo.startCol + 1) - SrcW) / 2,
					-((DataInfo.endRow-DataInfo.startRow+1) - SrcH)*0.01*(offsetY - 50) + ((DataInfo.endRow - DataInfo.startRow + 1) - SrcH) / 2,
					SrcW, SrcH,
					p_Img, (BITMAPINFO *)&BitmapInfo,
					DIB_RGB_COLORS, SRCCOPY);

			}

			break;
		default:
			break;

		}
	}
	else
	{
		// If we receive an incomplete image we do nothing but logging
		TRACE("Failure in receiving image");
	}
}


void Vimba::DataProcess()
{
	if (pBuffer == NULL)
		return;
	static UINT flag = 0;

	if (Count <= flag)
		return;

	Count = flag;
	pProcessBuffer = new VmbUchar_t[nSize];
	memcpy(pProcessBuffer, pBuffer, nSize);

	if (CDASDlg::GetInstance()->bMonitor == true)
	{
		VmbUint32_t timeout = 0;
		DoEvents();
		switch (m_mode)
		{
		case Vimba::AMP_FREQ:

			if (VmbErrorSuccess == err)
			{
				AMP_FREQ_DataProcess();
			}
			
			DoEvents();
			break;
		case Vimba::PHASE_FREQ:

			if (VmbErrorSuccess == err)
			{

				pLastBuffer = new VmbUchar_t[nSize];
				memcpy(pLastBuffer, pProcessBuffer, nSize);

				PHASE_FREQ_DataProcess();
				
			}
			DoEvents();
			break;
		default:
			break;
		}
	}
	delete[] pProcessBuffer;
	pProcessBuffer = NULL;
}


void Vimba::ImageAcuqisition(WPARAM status)
{
	if (VmbFrameStatusComplete == status)
	{
		// Pick up frame
		pFrame = GetFrame();
		if (SP_ISNULL(pFrame))
		{
			TRACE("frame ptr is NULL, late call");
			return;
		}
		err = pFrame->GetImage(pBuffer);
	}
	QueueFrame(pFrame);
}

void Vimba::ColorDataTransform()
{
	double pmin, pmax;
	pmin = pmax = (double)pDrawBuffer[0];
	for (int i = 0; i < nSize; ++i)
	{
		if (pmax < (double)pDrawBuffer[i])
			pmax = (double)pDrawBuffer[i];
		if (pmin > (double)pDrawBuffer[i])
			pmin = (double)pDrawBuffer[i];
	}
	mBuffer = new double[nSize];
	for (int i = 0; i < nSize; ++i)
	{
		mBuffer[i] = double(((double)pDrawBuffer[i] - pmin) / (pmax - pmin));
	}
	int bmpWidthBytes = ((nWidth * 24 + 31) & ~31) / 8;
	mImage = new BYTE[nHeight*bmpWidthBytes];

	for (int i = 0; i < nHeight; i++)
	{
		int imgLineIndex = i*bmpWidthBytes;
		for (unsigned int j = 0; j < nWidth; j++)
		{
			switch (int(ceil(mBuffer[i*nWidth + j] * 72)))
			{
			case 0:
			case 1:
			{
				mImage[imgLineIndex + j * 3] = 127;
				mImage[imgLineIndex + j * 3 + 1] = 0;
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 2:case 3:case 4:case 5:
			{
				mImage[imgLineIndex + j * 3] = BYTE(127 + 14 * 72.0*(mBuffer[i*nWidth + j] - 1 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 0;
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 6:
			{
				mImage[imgLineIndex + j * 3] = BYTE(183 + 15 * 72.0*(mBuffer[i*nWidth + j] - 5 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 0;
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 7:case 8:case 9:
			{
				mImage[imgLineIndex + j * 3] = BYTE(198 + 14 * 72.0*(mBuffer[i*nWidth + j] - 6 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 0;
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 10:
			{
				mImage[imgLineIndex + j * 3] = BYTE(240 + 15 * 72.0*(mBuffer[i*nWidth + j] - 9 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 0;
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:
			{
				mImage[imgLineIndex + j * 3] = 255;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(14 * 72.0*(mBuffer[i*nWidth + j] - 10 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 19:
			{
				mImage[imgLineIndex + j * 3] = 255;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(112 + 15 * 72.0*(mBuffer[i*nWidth + j] - 18 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 20:case 21:case 22:case 23:
			{
				mImage[imgLineIndex + j * 3] = 255;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(127 + 14 * 72.0*(mBuffer[i*nWidth + j] - 19 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 24:
			{
				mImage[imgLineIndex + j * 3] = 255;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(183 + 15 * 72.0*(mBuffer[i*nWidth + j] - 23 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 25:case 26:case 27:
			{
				mImage[imgLineIndex + j * 3] = 255;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(198 + 14 * 72.0*(mBuffer[i*nWidth + j] - 24 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 28:
			{
				mImage[imgLineIndex + j * 3] = 255;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(240 + 15 * 72.0*(mBuffer[i*nWidth + j] - 27 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 29:
			{
				mImage[imgLineIndex + j * 3] = BYTE(255 - 15 * 72.0*(mBuffer[i*nWidth + j] - 28 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 255;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(14 * 72.0*(mBuffer[i*nWidth + j] - 28 * 1.0 / 72));
				break;
			}
			case 30:case 31:case 32:
			{
				mImage[imgLineIndex + j * 3] = BYTE(240 - 14 * 72.0*(mBuffer[i*nWidth + j] - 29 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 255;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(14 + 14 * 72.0*(mBuffer[i*nWidth + j] - 29 * 1.0 / 72));
				break;
			}
			case 33:
			{
				mImage[imgLineIndex + j * 3] = BYTE(198 - 15 * 72.0*(mBuffer[i*nWidth + j] - 32 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 255;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(56 + 14 * 72.0*(mBuffer[i*nWidth + j] - 32 * 1.0 / 72));
				break;
			}
			case 34:case 35:case 36:
			{
				mImage[imgLineIndex + j * 3] = BYTE(183 - 14 * 72.0*(mBuffer[i*nWidth + j] - 33 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 255;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(70 + 14 * 72.0*(mBuffer[i*nWidth + j] - 33 * 1.0 / 72));
				break;
			}
			case 37:
			{
				mImage[imgLineIndex + j * 3] = BYTE(141 - 14 * 72.0*(mBuffer[i*nWidth + j] - 36 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 255;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(112 + 15 * 72.0*(mBuffer[i*nWidth + j] - 36 * 1.0 / 72));
				break;
			}
			case 38:
			{
				mImage[imgLineIndex + j * 3] = BYTE(127 - 15 * 72.0*(mBuffer[i*nWidth + j] - 37 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 255;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(127 + 14 * 72.0*(mBuffer[i*nWidth + j] - 37 * 1.0 / 72));
				break;
			}
			case 39:case 40:case 41:
			{
				mImage[imgLineIndex + j * 3] = BYTE(112 - 14 * 72.0*(mBuffer[i*nWidth + j] - 38 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 255;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(141 + 14 * 72.0*(mBuffer[i*nWidth + j] - 38 * 1.0 / 72));
				break;
			}
			case 42:
			{
				mImage[imgLineIndex + j * 3] = BYTE(70 - 14 * 72.0*(mBuffer[i*nWidth + j] - 41 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 255;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(183 + 15 * 72.0*(mBuffer[i*nWidth + j] - 41 * 1.0 / 72));
				break;
			}
			case 43:case 44:case 45:
			{
				mImage[imgLineIndex + j * 3] = BYTE(56 - 14 * 72.0*(mBuffer[i*nWidth + j] - 42 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 255;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(198 + 15 * 72.0*(mBuffer[i*nWidth + j] - 42 * 1.0 / 72));
				break;
			}
			case 46:
			{
				mImage[imgLineIndex + j * 3] = BYTE(14 - 14 * 72.0*(mBuffer[i*nWidth + j] - 45 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 1] = 255;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(14 - 14 * 72.0*(mBuffer[i*nWidth + j] - 45 * 1.0 / 72));
				break;
			}
			case 47:
			{
				mImage[imgLineIndex + j * 3] = 0;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(255 - 15 * 72.0*(mBuffer[i*nWidth + j] - 46 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 48:case 49:case 50:
			{
				mImage[imgLineIndex + j * 3] = 0;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(240 - 14 * 72.0*(mBuffer[i*nWidth + j] - 47 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 51:
			{
				mImage[imgLineIndex + j * 3] = 0;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(198 - 15 * 72.0*(mBuffer[i*nWidth + j] - 50 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 52:case 53:case 54:case 55:
			{
				mImage[imgLineIndex + j * 3] = 0;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(183 - 14 * 72.0*(mBuffer[i*nWidth + j] - 51 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 56:
			{
				mImage[imgLineIndex + j * 3] = 0;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(127 - 15 * 72.0*(mBuffer[i*nWidth + j] - 55 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 57:case 58:case 59:case 60:case 61:case 62:case 63:case 64:
			{
				mImage[imgLineIndex + j * 3] = 0;
				mImage[imgLineIndex + j * 3 + 1] = BYTE(112 - 14 * 72.0*(mBuffer[i*nWidth + j] - 56 * 1.0 / 72));
				mImage[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 65:
			{
				mImage[imgLineIndex + j * 3] = 0;
				mImage[imgLineIndex + j * 3 + 1] = 0;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(255 - 15 * 72.0*(mBuffer[i*nWidth + j] - 64 * 1.0 / 72));
				break;
			}
			case 66:case 67:case 68:
			{
				mImage[imgLineIndex + j * 3] = 0;
				mImage[imgLineIndex + j * 3 + 1] = 0;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(240 - 14 * 72.0*(mBuffer[i*nWidth + j] - 65 * 1.0 / 72));
				break;
			}
			case 69:
			{
				mImage[imgLineIndex + j * 3] = 0;
				mImage[imgLineIndex + j * 3 + 1] = 0;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(198 - 15 * 72.0*(mBuffer[i*nWidth + j] - 68 * 1.0 / 72));
				break;
			}
			case 70:case 71:case 72:
			{
				mImage[imgLineIndex + j * 3] = 0;
				mImage[imgLineIndex + j * 3 + 1] = 0;
				mImage[imgLineIndex + j * 3 + 2] = BYTE(183 - 14 * 72.0*(mBuffer[i*nWidth + j] - 69 * 1.0 / 72));
				break;
			}
			}
		}
		for (int j = nWidth * 3; j < bmpWidthBytes; j++)
		{
			mImage[imgLineIndex + j] = 0;
		}
	}
}
extern Complex<double>* tmp;
extern Complex<double>* temp;
extern Complex<double>* hn;
Mymatrix<double>*bg = nullptr;
Mymatrix<double>*ones = nullptr;
Complex<double>*W=nullptr;
Complex<double>*A=nullptr;
int bmpWidthBytes;
Mymatrix<double>*imgAmp=nullptr;
void Vimba::ApiShutdown()
{
	bIsCameraGet = false;
	run = false;
	if (!cameras.empty())
	{
		cameras[0]->Close();
	}
	ApiController.Shutdown();

	if (!phaseVec.empty())
	{
		for (auto &i : phaseVec)
		{
			delete[] i;
			i = nullptr;
		}
		phaseVec.clear();
	}
	if (tmp != nullptr)
	{
		delete[] hn;
		hn = nullptr;
		delete[] tmp;
		tmp = nullptr;
		delete[] temp;
		temp = nullptr;
		delete bg;
		bg = nullptr;
		delete ones;
		ones = nullptr;
		delete W;
		delete A;
		delete []p_Img;
		p_Img = nullptr;
		delete imgAmp;
		imgAmp = nullptr;
	}

	return;
}
void Vimba::AMP_FREQ_DataProcess()
{
	
	if (bg == nullptr)
	{
		FILE* f_r = NULL;
		int tempnum1 = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_HOR);//6
		int tempnum2 = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_VER);
		CString bk;
		bk.Format(_T("Data\\BK_%d_%d.bin"), tempnum1, tempnum2);
		int ret;
		ret = _wfopen_s(&f_r, bk, _T("rb"));
		if (ret != 0)
		{
			ret = GetLastError();
			CString error;
			error.Format(_T("Error occurs when openning BK.bin.\nErrorCode:%d"), ret);
			AfxGetMainWnd()->MessageBox(error, _T("Error"), MB_OK);
			return;
		}
		bg = new Mymatrix<double>(f_r, DataInfo.targetHeight, DataInfo.targetWidth, 0, 0);
		fclose(f_r);
		f_r = NULL;
		ones = new Mymatrix<double>(1, DataInfo.targetWidth, 1.0);
		W=new Complex<double>(cos(PI2*(DataInfo.freCount - 1) / (DataInfo.targetWidth*DataInfo.freCount)),
			-1.0*sin(PI2*(DataInfo.freCount - 1) / (DataInfo.targetWidth*DataInfo.freCount)));

		A=new Complex<double>(cos(PI2 / (DataInfo.targetWidth)), sin(PI2 / (DataInfo.targetWidth)));
	}
	Mymatrix<double>img(pProcessBuffer, DataInfo.targetHeight, DataInfo.targetWidth, 0);
	img -= *bg;
	
	Mymatrix<double>*tempmean = mean(img);
	Mymatrix<double>*temp = matMultiply(*tempmean, *ones);
	delete tempmean;
	img -= *temp;
	delete temp;
	img.Fliplr();
	Mymatrix<double> dbAfterimg(DataInfo.targetHeight, DataInfo.targetWidth);
	interpolation(img, dbAfterimg);	//线性插值
	hanning(dbAfterimg);		//对列加窗（hanning）
	Mymatrix_i<double>dbCZTimg(DataInfo.targetHeight, DataInfo.freCount);

	//CZT变换
	CZT(dbAfterimg, dbCZTimg, A, W);
	if (imgAmp == nullptr)
	{
		imgAmp=new Mymatrix<double> (DataInfo.targetHeight, dbCZTimg.Col); //干涉图1幅值
	}
	for (int i = 0; i < imgAmp->Row; i++)
	{
		for (int j = 0; j < imgAmp->Col; j++)
		{
			imgAmp->Data[i][j] = log(dbCZTimg.Data[i][j].mod());
			//imgAmp->Data[i][j] = log(imgAmp->Data[i][j]);
		}
	}

	/*for (int i = 0; i < imgAmp->Row; i++)
	{
		for (int j = 0; j < imgAmp->Col; j++)
		{
			imgAmp->Data[i][j] = log(imgAmp->Data[i][j]);
		}
	}*/

	double Max = imgAmp->Data[0][0];
	double Min = imgAmp->Data[0][0];
	for (int i = 0; i < imgAmp->Row; i++)
	{
		for (int j = 0; j < imgAmp->Col; j++)
		{
			if (imgAmp->Data[i][j] > Max)
			{
				Max = imgAmp->Data[i][j];
			}
			else if (imgAmp->Data[i][j] < Min)
			{
				Min = imgAmp->Data[i][j];
			}
		}
	}
	if (DataInfo.targetHeight == 1038)
	{
		if (Min < 0)
		{
			Min = 0;
		}
	}
	//归一化
	for (int i = 0; i < imgAmp->Row; i++)
	{
		for (int j = 0; j < imgAmp->Col; j++)
		{
			imgAmp->Data[i][j] = (imgAmp->Data[i][j] - Min) / (Max - Min);
		}
	}

	//imgAmp->resize(DataInfo.startRow - 1, DataInfo.startCol - 1, DataInfo.endRow-DataInfo.startRow+1, DataInfo.endCol-DataInfo.startCol+1);

	
	if (p_Img == nullptr)
	{
		/*	delete[] p_Img;
			p_Img = nullptr;*/
		bmpWidthBytes = ((imgAmp->Col * 24 + 31) & ~31) / 8;
		p_Img = new BYTE[bmpWidthBytes * imgAmp->Row];
	}
	
	for (int i = 0; i < imgAmp->Row; i++)
	{
		int imgLineIndex = i*bmpWidthBytes;
		for (int j = 0; j < imgAmp->Col; j++)
		{
			switch (int(ceil(imgAmp->Data[i][j] * 72)))
			{
			case 0:
			case 1:
			{
				p_Img[imgLineIndex + j * 3] = 127;
				p_Img[imgLineIndex + j * 3 + 1] = 0;
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 2:case 3:case 4:case 5:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(127 + 14 * 72.0*(imgAmp->Data[i][j] - 1 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 0;
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 6:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(183 + 15 * 72.0*(imgAmp->Data[i][j] - 5 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 0;
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 7:case 8:case 9:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(198 + 14 * 72.0*(imgAmp->Data[i][j] - 6 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 0;
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 10:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(240 + 15 * 72.0*(imgAmp->Data[i][j] - 9 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 0;
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:
			{
				p_Img[imgLineIndex + j * 3] = 255;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(14 * 72.0*(imgAmp->Data[i][j] - 10 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 19:
			{
				p_Img[imgLineIndex + j * 3] = 255;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(112 + 15 * 72.0*(imgAmp->Data[i][j] - 18 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 20:case 21:case 22:case 23:
			{
				p_Img[imgLineIndex + j * 3] = 255;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(127 + 14 * 72.0*(imgAmp->Data[i][j] - 19 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 24:
			{
				p_Img[imgLineIndex + j * 3] = 255;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(183 + 15 * 72.0*(imgAmp->Data[i][j] - 23 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 25:case 26:case 27:
			{
				p_Img[imgLineIndex + j * 3] = 255;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(198 + 14 * 72.0*(imgAmp->Data[i][j] - 24 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 28:
			{
				p_Img[imgLineIndex + j * 3] = 255;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(240 + 15 * 72.0*(imgAmp->Data[i][j] - 27 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 0;
				break;
			}
			case 29:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(255 - 15 * 72.0*(imgAmp->Data[i][j] - 28 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 255;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(14 * 72.0*(imgAmp->Data[i][j] - 28 * 1.0 / 72));
				break;
			}
			case 30:case 31:case 32:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(240 - 14 * 72.0*(imgAmp->Data[i][j] - 29 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 255;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(14 + 14 * 72.0*(imgAmp->Data[i][j] - 29 * 1.0 / 72));
				break;
			}
			case 33:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(198 - 15 * 72.0*(imgAmp->Data[i][j] - 32 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 255;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(56 + 14 * 72.0*(imgAmp->Data[i][j] - 32 * 1.0 / 72));
				break;
			}
			case 34:case 35:case 36:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(183 - 14 * 72.0*(imgAmp->Data[i][j] - 33 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 255;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(70 + 14 * 72.0*(imgAmp->Data[i][j] - 33 * 1.0 / 72));
				break;
			}
			case 37:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(141 - 14 * 72.0*(imgAmp->Data[i][j] - 36 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 255;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(112 + 15 * 72.0*(imgAmp->Data[i][j] - 36 * 1.0 / 72));
				break;
			}
			case 38:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(127 - 15 * 72.0*(imgAmp->Data[i][j] - 37 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 255;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(127 + 14 * 72.0*(imgAmp->Data[i][j] - 37 * 1.0 / 72));
				break;
			}
			case 39:case 40:case 41:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(112 - 14 * 72.0*(imgAmp->Data[i][j] - 38 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 255;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(141 + 14 * 72.0*(imgAmp->Data[i][j] - 38 * 1.0 / 72));
				break;
			}
			case 42:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(70 - 14 * 72.0*(imgAmp->Data[i][j] - 41 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 255;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(183 + 15 * 72.0*(imgAmp->Data[i][j] - 41 * 1.0 / 72));
				break;
			}
			case 43:case 44:case 45:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(56 - 14 * 72.0*(imgAmp->Data[i][j] - 42 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 255;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(198 + 15 * 72.0*(imgAmp->Data[i][j] - 42 * 1.0 / 72));
				break;
			}
			case 46:
			{
				p_Img[imgLineIndex + j * 3] = BYTE(14 - 14 * 72.0*(imgAmp->Data[i][j] - 45 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 1] = 255;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(240 + 15 * 72.0*(imgAmp->Data[i][j] - 45 * 1.0 / 72));
				break;
			}
			case 47:
			{
				p_Img[imgLineIndex + j * 3] = 0;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(255 - 15 * 72.0*(imgAmp->Data[i][j] - 46 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 48:case 49:case 50:
			{
				p_Img[imgLineIndex + j * 3] = 0;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(240 - 14 * 72.0*(imgAmp->Data[i][j] - 47 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 51:
			{
				p_Img[imgLineIndex + j * 3] = 0;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(198 - 15 * 72.0*(imgAmp->Data[i][j] - 50 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 52:case 53:case 54:case 55:
			{
				p_Img[imgLineIndex + j * 3] = 0;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(183 - 14 * 72.0*(imgAmp->Data[i][j] - 51 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 56:
			{
				p_Img[imgLineIndex + j * 3] = 0;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(127 - 15 * 72.0*(imgAmp->Data[i][j] - 55 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 57:case 58:case 59:case 60:case 61:case 62:case 63:case 64:
			{
				p_Img[imgLineIndex + j * 3] = 0;
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(112 - 14 * 72.0*(imgAmp->Data[i][j] - 56 * 1.0 / 72));
				p_Img[imgLineIndex + j * 3 + 2] = 255;
				break;
			}
			case 65:
			{
				p_Img[imgLineIndex + j * 3] = 0;
				p_Img[imgLineIndex + j * 3 + 1] = 0;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(255 - 15 * 72.0*(imgAmp->Data[i][j] - 64 * 1.0 / 72));
				break;
			}
			case 66:case 67:case 68:
			{
				p_Img[imgLineIndex + j * 3] = 0;
				p_Img[imgLineIndex + j * 3 + 1] = 0;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(240 - 14 * 72.0*(imgAmp->Data[i][j] - 65 * 1.0 / 72));
				break;
			}
			case 69:
			{
				p_Img[imgLineIndex + j * 3] = 0;
				p_Img[imgLineIndex + j * 3 + 1] = 0;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(198 - 15 * 72.0*(imgAmp->Data[i][j] - 68 * 1.0 / 72));
				break;
			}
			case 70:case 71:case 72:
			{
				p_Img[imgLineIndex + j * 3] = 0;
				p_Img[imgLineIndex + j * 3 + 1] = 0;
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(183 - 14 * 72.0*(imgAmp->Data[i][j] - 69 * 1.0 / 72));
				break;
			}
			}

		}
		for (int k = imgAmp->Col * 3 - 1; k < bmpWidthBytes; k++)
		{
			p_Img[imgLineIndex + k] = 0;
		}
	}
	//delete imgAmp;
}
void Vimba::PHASE_FREQ_DataProcess()
{
	if (pLastBuffer == NULL)
	{
		return;
	}

	phaseVec.push_back(pLastBuffer);
	if (phaseVec.size() > phaseStep)
	{
		if (bg == nullptr)
		{
			FILE* f_r = NULL;
			int tempnum1 = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_HOR);
			int tempnum2 = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_VER);
			CString bk;
			bk.Format(_T("Data\\BK_%d_%d.bin"), tempnum1, tempnum2);
			int ret;
			ret = _wfopen_s(&f_r, bk, _T("rb"));
			if (ret != 0)
			{
				ret = GetLastError();
				CString error;
				error.Format(_T("Error occurs when openning BK.bin.\nErrorCode:%d"), ret);
				AfxGetMainWnd()->MessageBox(error, _T("Error"), MB_OK);
				return;
			}

			bg=new Mymatrix<double>(f_r, DataInfo.targetHeight, DataInfo.targetWidth, 0, 0);
			fclose(f_r);
			f_r = NULL;
			ones=new Mymatrix<double>(1, DataInfo.targetWidth, 1.0);
			W = new Complex<double>(cos(PI2*(DataInfo.freCount - 1) / (DataInfo.targetWidth*DataInfo.freCount)),
				-1.0*sin(PI2*(DataInfo.freCount - 1) / (DataInfo.targetWidth*DataInfo.freCount)));

			A = new Complex<double>(cos(PI2 / (DataInfo.targetWidth)), sin(PI2 / (DataInfo.targetWidth)));
			
			
		}
		Mymatrix<double>Lastimg(phaseVec[0], DataInfo.targetHeight, DataInfo.targetWidth, 0);
		Mymatrix<double>Currentimg(phaseVec[phaseStep], DataInfo.targetHeight, DataInfo.targetWidth, 0);
		Lastimg -= *bg;
		Currentimg -= *bg;
		
		Mymatrix<double>*tempmean1 = mean(Lastimg);
		Mymatrix<double>*tempmean2 = mean(Currentimg);
		Mymatrix<double>*temp1 = matMultiply(*tempmean1, *ones);
		Mymatrix<double>*temp2 = matMultiply(*tempmean2, *ones);
		delete tempmean1;
		delete tempmean2;
		Lastimg -= *temp1;
		Currentimg -= *temp2;
		delete temp1;
		delete temp2;
		Lastimg.Fliplr();
		Currentimg.Fliplr();
		Mymatrix<double> dbAfterimg1(DataInfo.targetHeight, DataInfo.targetWidth);
		Mymatrix<double> dbAfterimg2(DataInfo.targetHeight, DataInfo.targetWidth);
		interpolation(Lastimg, dbAfterimg1);	//线性插值
		interpolation(Currentimg, dbAfterimg2);	//线性插值
		hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）
		
		Mymatrix_i<double>dbCZTimg1(DataInfo.targetHeight, DataInfo.freCount);
		//CZT变换
		CZT(dbAfterimg1, dbCZTimg1, A, W);
		Mymatrix_i<double>dbCZTimg2(DataInfo.targetHeight, DataInfo.freCount);
		//CZT变换
		CZT(dbAfterimg2, dbCZTimg2, A, W);
		Mymatrix<double>img1Phase(dbCZTimg1.Row, dbCZTimg1.Col); //干涉图1相位
		Mymatrix<double>img2Phase(dbCZTimg1.Row, dbCZTimg1.Col); //干涉图1相位
		if (imgAmp == nullptr)
		{
			imgAmp=new Mymatrix<double> (dbCZTimg1.Row, dbCZTimg1.Col);
		}
		for (int i = 0; i < imgAmp->Row; i++)
		{
			for (int j = 0; j < imgAmp->Col; j++)
			{
				img1Phase.Data[i][j] = dbCZTimg1.Data[i][j].arg();
				img2Phase.Data[i][j] = dbCZTimg2.Data[i][j].arg();
				imgAmp->Data[i][j] = dbCZTimg2.Data[i][j].mod();
			}
		}


		double data1, data2;
		Mymatrix<double>targetPhase(DataInfo.targetHeight, DataInfo.freCount);
		for (int i = 0; i < img1Phase.Row; i++)
		{
			for (int j = 0; j < img1Phase.Col; j++)
			{
				data1 = sin((img2Phase.Data[i][j] - img1Phase.Data[i][j]));
				data2 = cos((img2Phase.Data[i][j] - img1Phase.Data[i][j]));
				targetPhase.Data[i][j] = atan2(data1, data2);
			}
		}


		double amMax = imgAmp->Data[0][0], amMin = imgAmp->Data[0][0];
		double max_A = phaseMax;
		double min_A = phaseMin;
		double a = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
		double B = -1.5*(max_A + min_A)*a;
		double C = -3.0*a*(pow(max_A, 2)) - 2 * B*max_A;
		double D = 1.0 - a*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;
		for (int i = 0; i < targetPhase.Row; i++)
		{
			for (int j = 0; j < targetPhase.Col; j++)
			{

				targetPhase.Data[i][j] = (targetPhase.Data[i][j] + PI) / PI2;

				if (imgAmp->Data[i][j] > amMax)
				{
					amMax = imgAmp->Data[i][j];
				}
				else if (imgAmp->Data[i][j] < amMin)
				{
					amMin = imgAmp->Data[i][j];
				}

			}
		}
		for (int i = 0; i < imgAmp->Row; i++)
		{
			for (int j = 0; j < imgAmp->Col; j++)
			{
				imgAmp->Data[i][j] = (imgAmp->Data[i][j] - amMin) / (amMax - amMin);
				if (imgAmp->Data[i][j] >= max_A)
				{
					imgAmp->Data[i][j] = 1.0;
				}
				else if (imgAmp->Data[i][j] <= min_A)
				{
					imgAmp->Data[i][j] = 0;
				}
				else
				{
					imgAmp->Data[i][j] = a*pow(imgAmp->Data[i][j], 3) + B*pow(imgAmp->Data[i][j], 2) + C*imgAmp->Data[i][j] + D;
				}
			}
		}

		for (int i = 0; i < imgAmp->Row; i++)
		{
			for (int j = 0; j < imgAmp->Col; j++)
			{
				if (imgAmp->Data[i][j] > 1.0)
				{
					imgAmp->Data[i][j] = 1.0;
				}
				else if (imgAmp->Data[i][j] < 0.0)
				{
					imgAmp->Data[i][j] = 0.0;
				}
			}
		}

		medFilt2(*imgAmp);
		medFilt2(targetPhase, 5);
		//targetPhase->Flipud();
		//img1Amp->Flipud();
		//targetPhase->resize(DataInfo.startRow - 1, DataInfo.startCol - 1, DataInfo.endRow - DataInfo.startRow + 1, DataInfo.endCol - DataInfo.startCol + 1);
		//imgAmp->resize(DataInfo.startRow - 1, DataInfo.startCol - 1, DataInfo.endRow - DataInfo.startRow + 1, DataInfo.endCol - DataInfo.startCol + 1);


		if (p_Img == nullptr)
		{
			/*	delete[] p_Img;
			p_Img = nullptr;*/
			bmpWidthBytes = ((imgAmp->Col * 24 + 31) & ~31) / 8;
			p_Img = new BYTE[bmpWidthBytes * imgAmp->Row];
		}
		p_Img = new BYTE[bmpWidthBytes * (DataInfo.endRow - DataInfo.startRow + 1)];

		for (int i = 0; i < (DataInfo.endRow - DataInfo.startRow + 1); i++)
		{
			//int imgLineIndex = (height - i - 1)*bmpWidthBytes;
			int imgLineIndex = i*bmpWidthBytes;
			for (int j = 0; j < targetPhase.Col; j++)
			{
				switch (int(ceil(targetPhase.Data[i][j] * 72)))
				{
				case 0:
				case 1:
				{
					p_Img[imgLineIndex + j * 3] = 127;
					p_Img[imgLineIndex + j * 3 + 1] = 0;
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 2:case 3:case 4:case 5:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(127 + 14 * 72.0*(targetPhase.Data[i][j] - 1 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 0;
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 6:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(183 + 15 * 72.0*(targetPhase.Data[i][j] - 5 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 0;
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 7:case 8:case 9:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(198 + 14 * 72.0*(targetPhase.Data[i][j] - 6 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 0;
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 10:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(240 + 15 * 72.0*(targetPhase.Data[i][j] - 9 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 0;
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:
				{
					p_Img[imgLineIndex + j * 3] = 255;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(14 * 72.0*(targetPhase.Data[i][j] - 10 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 19:
				{
					p_Img[imgLineIndex + j * 3] = 255;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(112 + 15 * 72.0*(targetPhase.Data[i][j] - 18 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 20:case 21:case 22:case 23:
				{
					p_Img[imgLineIndex + j * 3] = 255;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(127 + 14 * 72.0*(targetPhase.Data[i][j] - 19 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 24:
				{
					p_Img[imgLineIndex + j * 3] = 255;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(183 + 15 * 72.0*(targetPhase.Data[i][j] - 23 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 25:case 26:case 27:
				{
					p_Img[imgLineIndex + j * 3] = 255;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(198 + 14 * 72.0*(targetPhase.Data[i][j] - 24 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 28:
				{
					p_Img[imgLineIndex + j * 3] = 255;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(240 + 15 * 72.0*(targetPhase.Data[i][j] - 27 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 0;
					break;
				}
				case 29:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(255 - 15 * 72.0*(targetPhase.Data[i][j] - 28 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 255;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(14 * 72.0*(targetPhase.Data[i][j] - 28 * 1.0 / 72));
					break;
				}
				case 30:case 31:case 32:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(240 - 14 * 72.0*(targetPhase.Data[i][j] - 29 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 255;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(14 + 14 * 72.0*(targetPhase.Data[i][j] - 29 * 1.0 / 72));
					break;
				}
				case 33:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(198 - 15 * 72.0*(targetPhase.Data[i][j] - 32 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 255;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(56 + 14 * 72.0*(targetPhase.Data[i][j] - 32 * 1.0 / 72));
					break;
				}
				case 34:case 35:case 36:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(183 - 14 * 72.0*(targetPhase.Data[i][j] - 33 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 255;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(70 + 14 * 72.0*(targetPhase.Data[i][j] - 33 * 1.0 / 72));
					break;
				}
				case 37:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(141 - 14 * 72.0*(targetPhase.Data[i][j] - 36 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 255;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(112 + 15 * 72.0*(targetPhase.Data[i][j] - 36 * 1.0 / 72));
					break;
				}
				case 38:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(127 - 15 * 72.0*(targetPhase.Data[i][j] - 37 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 255;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(127 + 14 * 72.0*(targetPhase.Data[i][j] - 37 * 1.0 / 72));
					break;
				}
				case 39:case 40:case 41:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(112 - 14 * 72.0*(targetPhase.Data[i][j] - 38 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 255;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(141 + 14 * 72.0*(targetPhase.Data[i][j] - 38 * 1.0 / 72));
					break;
				}
				case 42:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(70 - 14 * 72.0*(targetPhase.Data[i][j] - 41 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 255;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(183 + 15 * 72.0*(targetPhase.Data[i][j] - 41 * 1.0 / 72));
					break;
				}
				case 43:case 44:case 45:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(56 - 14 * 72.0*(targetPhase.Data[i][j] - 42 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 255;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(198 + 15 * 72.0*(targetPhase.Data[i][j] - 42 * 1.0 / 72));
					break;
				}
				case 46:
				{
					p_Img[imgLineIndex + j * 3] = BYTE(14 - 14 * 72.0*(targetPhase.Data[i][j] - 45 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 1] = 255;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(240 + 15 * 72.0*(targetPhase.Data[i][j] - 45 * 1.0 / 72));
					break;
				}
				case 47:
				{
					p_Img[imgLineIndex + j * 3] = 0;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(255 - 15 * 72.0*(targetPhase.Data[i][j] - 46 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 255;
					break;
				}
				case 48:case 49:case 50:
				{
					p_Img[imgLineIndex + j * 3] = 0;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(240 - 14 * 72.0*(targetPhase.Data[i][j] - 47 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 255;
					break;
				}
				case 51:
				{
					p_Img[imgLineIndex + j * 3] = 0;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(198 - 15 * 72.0*(targetPhase.Data[i][j] - 50 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 255;
					break;
				}
				case 52:case 53:case 54:case 55:
				{
					p_Img[imgLineIndex + j * 3] = 0;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(183 - 14 * 72.0*(targetPhase.Data[i][j] - 51 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 255;
					break;
				}
				case 56:
				{
					p_Img[imgLineIndex + j * 3] = 0;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(127 - 15 * 72.0*(targetPhase.Data[i][j] - 55 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 255;
					break;
				}
				case 57:case 58:case 59:case 60:case 61:case 62:case 63:case 64:
				{
					p_Img[imgLineIndex + j * 3] = 0;
					p_Img[imgLineIndex + j * 3 + 1] = BYTE(112 - 14 * 72.0*(targetPhase.Data[i][j] - 56 * 1.0 / 72));
					p_Img[imgLineIndex + j * 3 + 2] = 255;
					break;
				}
				case 65:
				{
					p_Img[imgLineIndex + j * 3] = 0;
					p_Img[imgLineIndex + j * 3 + 1] = 0;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(255 - 15 * 72.0*(targetPhase.Data[i][j] - 64 * 1.0 / 72));
					break;
				}
				case 66:case 67:case 68:
				{
					p_Img[imgLineIndex + j * 3] = 0;
					p_Img[imgLineIndex + j * 3 + 1] = 0;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(240 - 14 * 72.0*(targetPhase.Data[i][j] - 65 * 1.0 / 72));
					break;
				}
				case 69:
				{
					p_Img[imgLineIndex + j * 3] = 0;
					p_Img[imgLineIndex + j * 3 + 1] = 0;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(198 - 15 * 72.0*(targetPhase.Data[i][j] - 68 * 1.0 / 72));
					break;
				}
				case 70:case 71:case 72:
				{
					p_Img[imgLineIndex + j * 3] = 0;
					p_Img[imgLineIndex + j * 3 + 1] = 0;
					p_Img[imgLineIndex + j * 3 + 2] = BYTE(183 - 14 * 72.0*(targetPhase.Data[i][j] - 69 * 1.0 / 72));
					break;
				}
				}
				
			}
			for (int j = targetPhase.Col * 3; j < bmpWidthBytes; j++)
			{
				p_Img[imgLineIndex + j] = 0;
			}
		}
		for (int i = 0; i < (DataInfo.endRow - DataInfo.startRow + 1); i++)
		{
			int imgLineIndex = i*bmpWidthBytes;
			for (int j = 0; j < targetPhase.Col; j++)
			{
				p_Img[imgLineIndex + j * 3] = BYTE(p_Img[imgLineIndex + j * 3] * imgAmp->Data[i][j]);
				p_Img[imgLineIndex + j * 3 + 1] = BYTE(p_Img[imgLineIndex + j * 3 + 1] * imgAmp->Data[i][j]);
				p_Img[imgLineIndex + j * 3 + 2] = BYTE(p_Img[imgLineIndex + j * 3 + 2] * imgAmp->Data[i][j]);
			}
		}
		delete[] phaseVec[0];
		phaseVec[0] = nullptr;
		phaseVec.erase(phaseVec.begin());
	}
}

//void Vimba::ApiShutdown()
//{
//	bIsCameraGet = false;
//	if (!cameras.empty())
//	{
//		cameras[0]->Close();
//	}
//	ApiController.Shutdown();
//
//	if (!phaseVec.empty())
//	{
//		for (auto &i : phaseVec)
//		{
//			delete[] i;
//			i = nullptr;
//		}
//		phaseVec.clear();
//	}
//
//	return;
//}
//
//void Vimba::AMP_FREQ_DataProcess()
//{
//	Mymatrix<double>img(pProcessBuffer, DataInfo.targetHeight, DataInfo.targetWidth, 0);
//	FILE* f_r = NULL;
//	int tempnum1 = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_HOR);
//	int tempnum2 = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_VER);
//	CString bk;
//	bk.Format(_T("Data\\BK_%d_%d.bin"), tempnum1, tempnum2);
//	int ret;
//	ret = _wfopen_s(&f_r, bk, _T("rb"));
//	if (ret != 0)
//	{
//		ret = GetLastError();
//		CString error;
//		error.Format(_T("Error occurs when openning BK.bin.\nErrorCode:%d"), ret);
//		AfxGetMainWnd()->MessageBox(error, _T("Error"), MB_OK);
//		return;
//	}
//	Mymatrix<double>bg(f_r, DataInfo.targetHeight, DataInfo.targetWidth, 0, 0);
//	fclose(f_r);
//	f_r = NULL;
//
//	img -= bg;
//	Mymatrix<double>ones(1, DataInfo.targetWidth, 1.0);
//	Mymatrix<double>*tempmean = mean(img);
//	Mymatrix<double>*temp = matMultiply(*tempmean, ones);
//	delete tempmean;
//	img -= *temp;
//	delete temp;
//	img.Fliplr();
//	Mymatrix<double> dbAfterimg(DataInfo.targetHeight, DataInfo.targetWidth);
//	interpolation(img, dbAfterimg);	//线性插值
//	hanning(dbAfterimg);		//对列加窗（hanning）
//
//	int start = 1; //细化频率段起点
//	int end = DataInfo.freCount; //细化频率段终点
//	int fs = DataInfo.targetWidth;  //采样频率
//	int M = DataInfo.freCount;	//细化频段的频点数   变换的长度
//								//细化频段的跨度（步长）
//	Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
//	//细化频段的起始点  变换起始点
//	Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));
//	Mymatrix_i<double>dbCZTimg(DataInfo.targetHeight, M);
//	//CZT变换
//	CZT(dbAfterimg, dbCZTimg, A, W);
//
//	Mymatrix<double>*imgAmp = new Mymatrix<double>(dbCZTimg.Row, dbCZTimg.Col); //干涉图1幅值
//	for (int i = 0; i < dbCZTimg.Row; i++)
//	{
//		for (int j = 0; j < dbCZTimg.Col; j++)
//		{
//			imgAmp->Data[i][j] = (dbCZTimg.Data[i][j].mod());
//		}
//	}
//
//
//
//	for (int i = 0; i < imgAmp->Row; i++)
//	{
//		for (int j = 0; j < imgAmp->Col; j++)
//		{
//			imgAmp->Data[i][j] = log(imgAmp->Data[i][j]);
//		}
//	}
//
//	double Max = imgAmp->Data[0][0];
//	double Min = imgAmp->Data[0][0];
//	for (int i = 0; i < imgAmp->Row; i++)
//	{
//		for (int j = 0; j < imgAmp->Col; j++)
//		{
//			if (imgAmp->Data[i][j] > Max)
//			{
//				Max = imgAmp->Data[i][j];
//			}
//			else if (imgAmp->Data[i][j] < Min)
//			{
//				Min = imgAmp->Data[i][j];
//			}
//		}
//	}
//	if (DataInfo.targetHeight == 1038)
//	{
//		if (Min < 0)
//		{
//			Min = 0;
//		}
//	}
//
//	for (int i = 0; i < imgAmp->Row; i++)
//	{
//		for (int j = 0; j < imgAmp->Col; j++)
//		{
//			imgAmp->Data[i][j] = (imgAmp->Data[i][j] - Min) / (Max - Min);
//		}
//	}
//	//imgAmp->Flipud();
//	imgAmp->resize(DataInfo.startRow - 1, DataInfo.startCol - 1, DataInfo.endRow-DataInfo.startRow+1, DataInfo.endCol-DataInfo.startCol+1);
//
//	int bmpWidthBytes = ((imgAmp->Col * 24 + 31) & ~31) / 8;
//	if (p_Img != nullptr)
//	{
//		delete[] p_Img;
//		p_Img = nullptr;
//	}
//	p_Img = new BYTE[bmpWidthBytes * imgAmp->Row];
//
//	for (int i = 0; i < imgAmp->Row; i++)
//	{
//		//int imgLineIndex = (height - i - 1)*bmpWidthBytes;
//		int imgLineIndex = i*bmpWidthBytes;
//		for (int j = 0; j < imgAmp->Col; j++)
//		{
//			switch (int(ceil(imgAmp->Data[i][j] * 72)))
//			{
//			case 0:
//			case 1:
//			{
//				p_Img[imgLineIndex + j * 3] = 127;
//				p_Img[imgLineIndex + j * 3 + 1] = 0;
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 2:case 3:case 4:case 5:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(127 + 14 * 72.0*(imgAmp->Data[i][j] - 1 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 0;
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 6:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(183 + 15 * 72.0*(imgAmp->Data[i][j] - 5 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 0;
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 7:case 8:case 9:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(198 + 14 * 72.0*(imgAmp->Data[i][j] - 6 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 0;
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 10:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(240 + 15 * 72.0*(imgAmp->Data[i][j] - 9 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 0;
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:
//			{
//				p_Img[imgLineIndex + j * 3] = 255;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(14 * 72.0*(imgAmp->Data[i][j] - 10 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 19:
//			{
//				p_Img[imgLineIndex + j * 3] = 255;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(112 + 15 * 72.0*(imgAmp->Data[i][j] - 18 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 20:case 21:case 22:case 23:
//			{
//				p_Img[imgLineIndex + j * 3] = 255;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(127 + 14 * 72.0*(imgAmp->Data[i][j] - 19 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 24:
//			{
//				p_Img[imgLineIndex + j * 3] = 255;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(183 + 15 * 72.0*(imgAmp->Data[i][j] - 23 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 25:case 26:case 27:
//			{
//				p_Img[imgLineIndex + j * 3] = 255;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(198 + 14 * 72.0*(imgAmp->Data[i][j] - 24 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 28:
//			{
//				p_Img[imgLineIndex + j * 3] = 255;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(240 + 15 * 72.0*(imgAmp->Data[i][j] - 27 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 0;
//				break;
//			}
//			case 29:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(255 - 15 * 72.0*(imgAmp->Data[i][j] - 28 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 255;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(14 * 72.0*(imgAmp->Data[i][j] - 28 * 1.0 / 72));
//				break;
//			}
//			case 30:case 31:case 32:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(240 - 14 * 72.0*(imgAmp->Data[i][j] - 29 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 255;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(14 + 14 * 72.0*(imgAmp->Data[i][j] - 29 * 1.0 / 72));
//				break;
//			}
//			case 33:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(198 - 15 * 72.0*(imgAmp->Data[i][j] - 32 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 255;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(56 + 14 * 72.0*(imgAmp->Data[i][j] - 32 * 1.0 / 72));
//				break;
//			}
//			case 34:case 35:case 36:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(183 - 14 * 72.0*(imgAmp->Data[i][j] - 33 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 255;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(70 + 14 * 72.0*(imgAmp->Data[i][j] - 33 * 1.0 / 72));
//				break;
//			}
//			case 37:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(141 - 14 * 72.0*(imgAmp->Data[i][j] - 36 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 255;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(112 + 15 * 72.0*(imgAmp->Data[i][j] - 36 * 1.0 / 72));
//				break;
//			}
//			case 38:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(127 - 15 * 72.0*(imgAmp->Data[i][j] - 37 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 255;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(127 + 14 * 72.0*(imgAmp->Data[i][j] - 37 * 1.0 / 72));
//				break;
//			}
//			case 39:case 40:case 41:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(112 - 14 * 72.0*(imgAmp->Data[i][j] - 38 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 255;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(141 + 14 * 72.0*(imgAmp->Data[i][j] - 38 * 1.0 / 72));
//				break;
//			}
//			case 42:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(70 - 14 * 72.0*(imgAmp->Data[i][j] - 41 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 255;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(183 + 15 * 72.0*(imgAmp->Data[i][j] - 41 * 1.0 / 72));
//				break;
//			}
//			case 43:case 44:case 45:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(56 - 14 * 72.0*(imgAmp->Data[i][j] - 42 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 255;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(198 + 15 * 72.0*(imgAmp->Data[i][j] - 42 * 1.0 / 72));
//				break;
//			}
//			case 46:
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(14 - 14 * 72.0*(imgAmp->Data[i][j] - 45 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 1] = 255;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(240 + 15 * 72.0*(imgAmp->Data[i][j] - 45 * 1.0 / 72));
//				break;
//			}
//			case 47:
//			{
//				p_Img[imgLineIndex + j * 3] = 0;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(255 - 15 * 72.0*(imgAmp->Data[i][j] - 46 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 255;
//				break;
//			}
//			case 48:case 49:case 50:
//			{
//				p_Img[imgLineIndex + j * 3] = 0;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(240 - 14 * 72.0*(imgAmp->Data[i][j] - 47 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 255;
//				break;
//			}
//			case 51:
//			{
//				p_Img[imgLineIndex + j * 3] = 0;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(198 - 15 * 72.0*(imgAmp->Data[i][j] - 50 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 255;
//				break;
//			}
//			case 52:case 53:case 54:case 55:
//			{
//				p_Img[imgLineIndex + j * 3] = 0;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(183 - 14 * 72.0*(imgAmp->Data[i][j] - 51 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 255;
//				break;
//			}
//			case 56:
//			{
//				p_Img[imgLineIndex + j * 3] = 0;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(127 - 15 * 72.0*(imgAmp->Data[i][j] - 55 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 255;
//				break;
//			}
//			case 57:case 58:case 59:case 60:case 61:case 62:case 63:case 64:
//			{
//				p_Img[imgLineIndex + j * 3] = 0;
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(112 - 14 * 72.0*(imgAmp->Data[i][j] - 56 * 1.0 / 72));
//				p_Img[imgLineIndex + j * 3 + 2] = 255;
//				break;
//			}
//			case 65:
//			{
//				p_Img[imgLineIndex + j * 3] = 0;
//				p_Img[imgLineIndex + j * 3 + 1] = 0;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(255 - 15 * 72.0*(imgAmp->Data[i][j] - 64 * 1.0 / 72));
//				break;
//			}
//			case 66:case 67:case 68:
//			{
//				p_Img[imgLineIndex + j * 3] = 0;
//				p_Img[imgLineIndex + j * 3 + 1] = 0;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(240 - 14 * 72.0*(imgAmp->Data[i][j] - 65 * 1.0 / 72));
//				break;
//			}
//			case 69:
//			{
//				p_Img[imgLineIndex + j * 3] = 0;
//				p_Img[imgLineIndex + j * 3 + 1] = 0;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(198 - 15 * 72.0*(imgAmp->Data[i][j] - 68 * 1.0 / 72));
//				break;
//			}
//			case 70:case 71:case 72:
//			{
//				p_Img[imgLineIndex + j * 3] = 0;
//				p_Img[imgLineIndex + j * 3 + 1] = 0;
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(183 - 14 * 72.0*(imgAmp->Data[i][j] - 69 * 1.0 / 72));
//				break;
//			}
//			}
//
//		}
//		for (int k = imgAmp->Col * 3 - 1; k < bmpWidthBytes; k++)
//		{
//			p_Img[imgLineIndex + k] = 0;
//		}
//	}
//	delete imgAmp;
//}
//
//void Vimba::PHASE_FREQ_DataProcess()
//{
//	if (pLastBuffer == NULL)
//	{
//		return;
//	}
//	phaseVec.push_back(pLastBuffer);
//	if (phaseVec.size() > phaseStep)
//	{
//		Mymatrix<double>Lastimg(phaseVec[0], DataInfo.targetHeight, DataInfo.targetWidth, 0);
//		Mymatrix<double>Currentimg(phaseVec[phaseStep], DataInfo.targetHeight, DataInfo.targetWidth, 0);
//
//
//		FILE* f_r = NULL;
//		int tempnum1 = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_HOR);
//		int tempnum2 = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_VER);
//		CString bk;
//		bk.Format(_T("Data\\BK_%d_%d.bin"), tempnum1, tempnum2);
//		int ret;
//		ret = _wfopen_s(&f_r, bk, _T("rb"));
//		if (ret != 0)
//		{
//			ret = GetLastError();
//			CString error;
//			error.Format(_T("Error occurs when openning BK.bin.\nErrorCode:%d"), ret);
//			AfxGetMainWnd()->MessageBox(error, _T("Error"), MB_OK);
//			return;
//		}
//
//		Mymatrix<double>bg(f_r, DataInfo.targetHeight, DataInfo.targetWidth, 0, 0);
//		fclose(f_r);
//		f_r = NULL;
//
//		Lastimg -= bg;
//		Currentimg -= bg;
//		Mymatrix<double>ones(1, DataInfo.targetWidth, 1.0);
//		Mymatrix<double>*tempmean1 = mean(Lastimg);
//		Mymatrix<double>*tempmean2 = mean(Currentimg);
//		Mymatrix<double>*temp1 = matMultiply(*tempmean1, ones);
//		Mymatrix<double>*temp2 = matMultiply(*tempmean2, ones);
//		delete tempmean1;
//		delete tempmean2;
//		Lastimg -= *temp1;
//		Currentimg -= *temp2;
//		delete temp1;
//		delete temp2;
//		Lastimg.Fliplr();
//		Currentimg.Fliplr();
//		Mymatrix<double> dbAfterimg1(DataInfo.targetHeight, DataInfo.targetWidth);
//		Mymatrix<double> dbAfterimg2(DataInfo.targetHeight, DataInfo.targetWidth);
//		interpolation(Lastimg, dbAfterimg1);	//线性插值
//		interpolation(Currentimg, dbAfterimg2);	//线性插值
//		hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）
//		int start = 1; //细化频率段起点
//		int end = DataInfo.freCount; //细化频率段终点
//		int fs = DataInfo.targetWidth;  //采样频率
//		int M = DataInfo.freCount;	//细化频段的频点数   变换的长度
//									//细化频段的跨度（步长）
//		Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
//		//细化频段的起始点  变换起始点
//		Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));
//		Mymatrix_i<double>dbCZTimg1(DataInfo.targetHeight, M);
//		//CZT变换
//		CZT(dbAfterimg1, dbCZTimg1, A, W);
//		Mymatrix_i<double>dbCZTimg2(DataInfo.targetHeight, M);
//		//CZT变换
//		CZT(dbAfterimg2, dbCZTimg2, A, W);
//		Mymatrix<double>img1Phase(dbCZTimg1.Row, dbCZTimg1.Col); //干涉图1相位
//		Mymatrix<double>img2Phase(dbCZTimg1.Row, dbCZTimg1.Col); //干涉图1相位
//		Mymatrix<double>*img1Amp = new Mymatrix<double>(dbCZTimg1.Row, dbCZTimg1.Col);
//		for (int i = 0; i < img1Amp->Row; i++)
//		{
//			for (int j = 0; j < img1Amp->Col; j++)
//			{
//				img1Phase.Data[i][j] = dbCZTimg1.Data[i][j].arg();
//				img2Phase.Data[i][j] = dbCZTimg2.Data[i][j].arg();
//				img1Amp->Data[i][j] = dbCZTimg2.Data[i][j].mod();
//			}
//		}
//
//
//		double data1, data2;
//		Mymatrix<double>*targetPhase = new Mymatrix<double>(img1Phase.Row, img1Phase.Col);
//		for (int i = 0; i < img1Phase.Row; i++)
//		{
//			for (int j = 0; j < img1Phase.Col; j++)
//			{
//				data1 = sin((img2Phase.Data[i][j] - img1Phase.Data[i][j]));
//				data2 = cos((img2Phase.Data[i][j] - img1Phase.Data[i][j]));
//				targetPhase->Data[i][j] = atan2(data1, data2);
//			}
//		}
//
//
//		double amMax = img1Amp->Data[0][0], amMin = img1Amp->Data[0][0];
//		double max_A = phaseMax;
//		double min_A = phaseMin;
//		double a = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
//		double B = -1.5*(max_A + min_A)*a;
//		double C = -3.0*a*(pow(max_A, 2)) - 2 * B*max_A;
//		double D = 1.0 - a*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;
//
//		for (int i = 0; i < targetPhase->Row; i++)
//		{
//			for (int j = 0; j < targetPhase->Col; j++)
//			{
//
//				targetPhase->Data[i][j] = (targetPhase->Data[i][j] + PI) / PI2;
//
//				if (img1Amp->Data[i][j] > amMax)
//				{
//					amMax = img1Amp->Data[i][j];
//				}
//				else if (img1Amp->Data[i][j] < amMin)
//				{
//					amMin = img1Amp->Data[i][j];
//				}
//
//			}
//		}
//		for (int i = 0; i < img1Amp->Row; i++)
//		{
//			for (int j = 0; j < img1Amp->Col; j++)
//			{
//				img1Amp->Data[i][j] = (img1Amp->Data[i][j] - amMin) / (amMax - amMin);
//				if (img1Amp->Data[i][j] >= max_A)
//				{
//					img1Amp->Data[i][j] = 1.0;
//				}
//				else if (img1Amp->Data[i][j] <= min_A)
//				{
//					img1Amp->Data[i][j] = 0;
//				}
//				else
//				{
//					img1Amp->Data[i][j] = a*pow(img1Amp->Data[i][j], 3) + B*pow(img1Amp->Data[i][j], 2) + C*img1Amp->Data[i][j] + D;
//				}
//			}
//		}
//
//		for (int i = 0; i < img1Amp->Row; i++)
//		{
//			for (int j = 0; j < img1Amp->Col; j++)
//			{
//				if (img1Amp->Data[i][j] > 1.0)
//				{
//					img1Amp->Data[i][j] = 1.0;
//				}
//				else if (img1Amp->Data[i][j] < 0.0)
//				{
//					img1Amp->Data[i][j] = 0.0;
//				}
//			}
//		}
//
//		medFilt2(*img1Amp);
//		medFilt2(*targetPhase, 5);
//		//targetPhase->Flipud();
//		//img1Amp->Flipud();
//		//targetPhase->resize(DataInfo.startRow - 1, DataInfo.startCol - 1, DataInfo.endRow, DataInfo.freCount);
//		//img1Amp->resize(DataInfo.startRow - 1, DataInfo.startCol - 1, DataInfo.endRow, DataInfo.freCount);
//
//
//		int bmpWidthBytes = ((targetPhase->Col * 24 + 31) & ~31) / 8;
//		if (p_Img != nullptr)
//		{
//			delete[] p_Img;
//			p_Img = nullptr;
//		}
//		p_Img = new BYTE[bmpWidthBytes * targetPhase->Row];
//
//		for (int i = 0; i < targetPhase->Row; i++)
//		{
//			//int imgLineIndex = (height - i - 1)*bmpWidthBytes;
//			int imgLineIndex = i*bmpWidthBytes;
//			for (int j = 0; j < targetPhase->Col; j++)
//			{
//				switch (int(ceil(targetPhase->Data[i][j] * 72)))
//				{
//				case 0:
//				case 1:
//				{
//					p_Img[imgLineIndex + j * 3] = 127;
//					p_Img[imgLineIndex + j * 3 + 1] = 0;
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 2:case 3:case 4:case 5:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(127 + 14 * 72.0*(targetPhase->Data[i][j] - 1 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 0;
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 6:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(183 + 15 * 72.0*(targetPhase->Data[i][j] - 5 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 0;
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 7:case 8:case 9:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(198 + 14 * 72.0*(targetPhase->Data[i][j] - 6 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 0;
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 10:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(240 + 15 * 72.0*(targetPhase->Data[i][j] - 9 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 0;
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:
//				{
//					p_Img[imgLineIndex + j * 3] = 255;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(14 * 72.0*(targetPhase->Data[i][j] - 10 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 19:
//				{
//					p_Img[imgLineIndex + j * 3] = 255;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(112 + 15 * 72.0*(targetPhase->Data[i][j] - 18 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 20:case 21:case 22:case 23:
//				{
//					p_Img[imgLineIndex + j * 3] = 255;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(127 + 14 * 72.0*(targetPhase->Data[i][j] - 19 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 24:
//				{
//					p_Img[imgLineIndex + j * 3] = 255;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(183 + 15 * 72.0*(targetPhase->Data[i][j] - 23 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 25:case 26:case 27:
//				{
//					p_Img[imgLineIndex + j * 3] = 255;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(198 + 14 * 72.0*(targetPhase->Data[i][j] - 24 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 28:
//				{
//					p_Img[imgLineIndex + j * 3] = 255;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(240 + 15 * 72.0*(targetPhase->Data[i][j] - 27 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 0;
//					break;
//				}
//				case 29:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(255 - 15 * 72.0*(targetPhase->Data[i][j] - 28 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 255;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(14 * 72.0*(targetPhase->Data[i][j] - 28 * 1.0 / 72));
//					break;
//				}
//				case 30:case 31:case 32:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(240 - 14 * 72.0*(targetPhase->Data[i][j] - 29 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 255;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(14 + 14 * 72.0*(targetPhase->Data[i][j] - 29 * 1.0 / 72));
//					break;
//				}
//				case 33:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(198 - 15 * 72.0*(targetPhase->Data[i][j] - 32 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 255;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(56 + 14 * 72.0*(targetPhase->Data[i][j] - 32 * 1.0 / 72));
//					break;
//				}
//				case 34:case 35:case 36:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(183 - 14 * 72.0*(targetPhase->Data[i][j] - 33 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 255;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(70 + 14 * 72.0*(targetPhase->Data[i][j] - 33 * 1.0 / 72));
//					break;
//				}
//				case 37:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(141 - 14 * 72.0*(targetPhase->Data[i][j] - 36 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 255;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(112 + 15 * 72.0*(targetPhase->Data[i][j] - 36 * 1.0 / 72));
//					break;
//				}
//				case 38:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(127 - 15 * 72.0*(targetPhase->Data[i][j] - 37 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 255;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(127 + 14 * 72.0*(targetPhase->Data[i][j] - 37 * 1.0 / 72));
//					break;
//				}
//				case 39:case 40:case 41:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(112 - 14 * 72.0*(targetPhase->Data[i][j] - 38 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 255;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(141 + 14 * 72.0*(targetPhase->Data[i][j] - 38 * 1.0 / 72));
//					break;
//				}
//				case 42:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(70 - 14 * 72.0*(targetPhase->Data[i][j] - 41 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 255;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(183 + 15 * 72.0*(targetPhase->Data[i][j] - 41 * 1.0 / 72));
//					break;
//				}
//				case 43:case 44:case 45:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(56 - 14 * 72.0*(targetPhase->Data[i][j] - 42 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 255;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(198 + 15 * 72.0*(targetPhase->Data[i][j] - 42 * 1.0 / 72));
//					break;
//				}
//				case 46:
//				{
//					p_Img[imgLineIndex + j * 3] = BYTE(14 - 14 * 72.0*(targetPhase->Data[i][j] - 45 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 1] = 255;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(240 + 15 * 72.0*(targetPhase->Data[i][j] - 45 * 1.0 / 72));
//					break;
//				}
//				case 47:
//				{
//					p_Img[imgLineIndex + j * 3] = 0;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(255 - 15 * 72.0*(targetPhase->Data[i][j] - 46 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 255;
//					break;
//				}
//				case 48:case 49:case 50:
//				{
//					p_Img[imgLineIndex + j * 3] = 0;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(240 - 14 * 72.0*(targetPhase->Data[i][j] - 47 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 255;
//					break;
//				}
//				case 51:
//				{
//					p_Img[imgLineIndex + j * 3] = 0;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(198 - 15 * 72.0*(targetPhase->Data[i][j] - 50 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 255;
//					break;
//				}
//				case 52:case 53:case 54:case 55:
//				{
//					p_Img[imgLineIndex + j * 3] = 0;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(183 - 14 * 72.0*(targetPhase->Data[i][j] - 51 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 255;
//					break;
//				}
//				case 56:
//				{
//					p_Img[imgLineIndex + j * 3] = 0;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(127 - 15 * 72.0*(targetPhase->Data[i][j] - 55 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 255;
//					break;
//				}
//				case 57:case 58:case 59:case 60:case 61:case 62:case 63:case 64:
//				{
//					p_Img[imgLineIndex + j * 3] = 0;
//					p_Img[imgLineIndex + j * 3 + 1] = BYTE(112 - 14 * 72.0*(targetPhase->Data[i][j] - 56 * 1.0 / 72));
//					p_Img[imgLineIndex + j * 3 + 2] = 255;
//					break;
//				}
//				case 65:
//				{
//					p_Img[imgLineIndex + j * 3] = 0;
//					p_Img[imgLineIndex + j * 3 + 1] = 0;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(255 - 15 * 72.0*(targetPhase->Data[i][j] - 64 * 1.0 / 72));
//					break;
//				}
//				case 66:case 67:case 68:
//				{
//					p_Img[imgLineIndex + j * 3] = 0;
//					p_Img[imgLineIndex + j * 3 + 1] = 0;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(240 - 14 * 72.0*(targetPhase->Data[i][j] - 65 * 1.0 / 72));
//					break;
//				}
//				case 69:
//				{
//					p_Img[imgLineIndex + j * 3] = 0;
//					p_Img[imgLineIndex + j * 3 + 1] = 0;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(198 - 15 * 72.0*(targetPhase->Data[i][j] - 68 * 1.0 / 72));
//					break;
//				}
//				case 70:case 71:case 72:
//				{
//					p_Img[imgLineIndex + j * 3] = 0;
//					p_Img[imgLineIndex + j * 3 + 1] = 0;
//					p_Img[imgLineIndex + j * 3 + 2] = BYTE(183 - 14 * 72.0*(targetPhase->Data[i][j] - 69 * 1.0 / 72));
//					break;
//				}
//				}
//				
//			}
//			for (int j = targetPhase->Col * 3; j < bmpWidthBytes; j++)
//			{
//				p_Img[imgLineIndex + j] = 0;
//			}
//		}
//		for (int i = 0; i < targetPhase->Row; i++)
//		{
//			int imgLineIndex = i*bmpWidthBytes;
//			for (int j = 0; j < targetPhase->Col; j++)
//			{
//				p_Img[imgLineIndex + j * 3] = BYTE(p_Img[imgLineIndex + j * 3] * img1Amp->Data[i][j]);
//				p_Img[imgLineIndex + j * 3 + 1] = BYTE(p_Img[imgLineIndex + j * 3 + 1] * img1Amp->Data[i][j]);
//				p_Img[imgLineIndex + j * 3 + 2] = BYTE(p_Img[imgLineIndex + j * 3 + 2] * img1Amp->Data[i][j]);
//			}
//		}
//		delete[] phaseVec[0];
//		phaseVec[0] = nullptr;
//		phaseVec.erase(phaseVec.begin());
//		delete targetPhase;
//		delete img1Amp;
//	}
//}

void Vimba::SetParam()
{

	BinningHor = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_HOR);
	BinningVer = CDASDlg::GetInstance()->GetDlgItemInt(IDC_EDIT_BINNING_VER);

	err = SetBinningHorizontal(BinningHor);
	err = SetBinningVertical(BinningVer);
	switch (BinningHor)
	{
	case 1:err = SetWidth(1388); break;
	case 2:err = SetWidth(694);  break;
	case 3:err = SetWidth(462);  break;
	case 4: err = SetWidth(346); break;
	default:
		break;
	}
	switch (BinningVer)
	{
	case 1:err = SetHeight(1038); break;
	case 2:err = SetHeight(518);  break;
	case 3:err = SetHeight(346);  break;
	case 4: err = SetHeight(258); break;
	default:
		break;
	}
	VmbInt64_t tmp;
	GetHeight(tmp);
	GetWidth(tmp);
	GetImageSize(nSize);
}

FramePtr Vimba::GetFrame()
{
	return SP_DYN_CAST(m_pFrameObserver, FrameObserver)->GetFrame();
}

bool Vimba::SetExposure(double exposure)
{
	if (bIsCameraGet)
	{
		CString cstr_error;
		err = SetExposureTimeAbs(exposure);
		if (VmbErrorSuccess == err)
		{
			return true;
		}
		else
		{
			cstr_error.Format(_T("%s%d"), _T("无法设置曝光值，错误代码"), (int)err);
			AfxGetMainWnd()->MessageBox(cstr_error, _T("错误"), MB_OK | MB_ICONERROR);
			return false;
		}
	}
	return true;
}
bool Vimba::SetGain(double gain)
{
	if (bIsCameraGet)
	{
		CString cstr_error;
		err = SetGainValue(gain);
		if (VmbErrorSuccess == err)
		{
			return true;
		}
		else
		{
			cstr_error.Format(_T("%s%d"), _T("无法设置增益，错误代码"), (int)err);
			AfxGetMainWnd()->MessageBox(cstr_error, _T("错误"), MB_OK | MB_ICONERROR);
			return false;
		}
	}
	return true;
}
void Vimba::AdjustPacketSize()
{
	FeaturePtr pCommandFeature;
	if (VmbErrorSuccess == cameras[0]->GetFeatureByName("GVSPAdjustPacketSize", pCommandFeature))
	{
		if (VmbErrorSuccess == pCommandFeature->RunCommand())
		{
			bool bIsCommandDone = false;
			do
			{
				if (VmbErrorSuccess != pCommandFeature->IsCommandDone(bIsCommandDone))
				{
					break;
				}
			} while (false == bIsCommandDone);
		}
	}
}

VmbErrorType Vimba::QueueFrame(FramePtr pFrame)
{
	return SP_ACCESS(cameras[0])->QueueFrame(pFrame);
}
VmbErrorType Vimba::StartContinuousImageAcquisition()
{
	if (InitCamera())
	{
		SP_SET(m_pFrameObserver, new FrameObserver(cameras[0]));
		err = SP_ACCESS(cameras[0])->StartContinuousImageAcquisition(NUM_FRAMES, m_pFrameObserver);
		if (VmbErrorSuccess != err)
		{
			// If anything fails after opening the camera we close it
			SP_ACCESS(cameras[0])->Close();
		}
		return err;
	}
	else
	{
		return VmbErrorOther;
	}
}

VmbErrorType Vimba::StopContinuousImageAcquisition()
{
	// Stop streaming
	SP_ACCESS(cameras[0])->StopContinuousImageAcquisition();
	// Close camera
	return  SP_ACCESS(cameras[0])->Close();
}

void Vimba::ClearFrameQueue()
{
	SP_DYN_CAST(m_pFrameObserver, FrameObserver)->ClearFrameQueue();
}

VmbErrorType Vimba::SetExposureTimeAbs(double value)
{
	if (bIsCameraGet) {
		VmbErrorType result;
		FeaturePtr pFeature;
		FeaturePtr m_ExposureTimeAbsFeature;
		if (m_ExposureTimeAbsFeature.get() == NULL)
		{
			result = cameras[0]->GetFeatureByName("ExposureTimeAbs", m_ExposureTimeAbsFeature);
			if (result != VmbErrorSuccess)
			{
				m_ExposureTimeAbsFeature.reset();
				return result;
			}
		}
		pFeature = m_ExposureTimeAbsFeature;
		result = pFeature->SetValue(value);
		return result;
	}
	else
	{
		return VmbErrorOther;
	}
}
VmbErrorType Vimba::SetGainValue(double value)
{
	if (bIsCameraGet) {
		VmbErrorType result;
		FeaturePtr pFeature;
		FeaturePtr m_GainFeature;
		if (m_GainFeature.get() == NULL)
		{
			result = cameras[0]->GetFeatureByName("Gain", m_GainFeature);
			if (result != VmbErrorSuccess)
			{
				m_GainFeature.reset();
				return result;
			}
		}
		pFeature = m_GainFeature;
		result = pFeature->SetValue(value);
		return result;
	}
	else
	{
		return VmbErrorOther;
	}
}
VmbErrorType Vimba::SetPixelFormat(VmbInt64_t value)
{
	VmbErrorType result;
	FeaturePtr m_PixelFormatFeature;
	if (m_PixelFormatFeature.get() == NULL)
	{
		result = cameras[0]->GetFeatureByName("PixelFormat", m_PixelFormatFeature);
		if (result != VmbErrorSuccess)
		{
			m_PixelFormatFeature.reset();
			return result;
		}
	}
	result = m_PixelFormatFeature->SetValue(value);
	VmbInt64_t nValue;
	while (1)
	{
		if (VmbErrorSuccess == GetPixelFormat(nValue))
			if (nValue == value)
				break;
	}
	return result;
}

VmbErrorType Vimba::SetWidth(VmbInt64_t value)
{
	VmbErrorType result;
	FeaturePtr m_WidthFeature;
	if (m_WidthFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("Width", m_WidthFeature);
		if (result != VmbErrorSuccess)
		{
			m_WidthFeature.reset();
			return result;
		}
	}
	result = m_WidthFeature->SetValue(value);
	return result;
}
VmbErrorType Vimba::SetHeight(VmbInt64_t value)
{
	VmbErrorType result;
	FeaturePtr m_HeightFeature;
	if (m_HeightFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("Height", m_HeightFeature);
		if (result != VmbErrorSuccess)
		{
			m_HeightFeature.reset();
			return result;
		}
	}
	result = m_HeightFeature->SetValue(value);
	return result;
}

VmbErrorType Vimba::GetPixelFormat(VmbInt64_t & value)
{
	VmbErrorType result;
	FeaturePtr m_PixelFormatFeature;
	if (m_PixelFormatFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("PixelFormat", m_PixelFormatFeature);
		if (result != VmbErrorSuccess)
		{
			m_PixelFormatFeature.reset();
			return result;
		}
	}
	result = m_PixelFormatFeature->GetValue(value);
	return result;
}
VmbErrorType Vimba::GetWidth(VmbInt64_t & value)
{
	VmbErrorType result;
	FeaturePtr m_WidthFeature;
	if (m_WidthFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("Width", m_WidthFeature);
		if (result != VmbErrorSuccess)
		{
			m_WidthFeature.reset();
			return result;
		}
	}
	result = m_WidthFeature->GetValue(value);
	return result;
}
VmbErrorType Vimba::GetHeight(VmbInt64_t & value)
{
	VmbErrorType result;
	FeaturePtr m_HeightFeature;
	if (m_HeightFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("Height", m_HeightFeature);
		if (result != VmbErrorSuccess)
		{
			m_HeightFeature.reset();
			return result;
		}
	}
	result = m_HeightFeature->GetValue(value);
	return result;
}
VmbErrorType Vimba::GetImageSize(VmbInt64_t & value)
{
	VmbErrorType result;
	FeaturePtr m_ImageSizeFeature;
	if (m_ImageSizeFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("ImageSize", m_ImageSizeFeature);
		if (result != VmbErrorSuccess)
		{
			m_ImageSizeFeature.reset();
			return result;
		}
	}
	result = m_ImageSizeFeature->GetValue(value);
	return result;
}

VmbErrorType Vimba::SetStreamBytesPerSecond(VmbInt64_t value)
{
	VmbErrorType result;
	FeaturePtr m_StreamBytesPerSecondFeature;
	if (m_StreamBytesPerSecondFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("StreamBytesPerSecond", m_StreamBytesPerSecondFeature);
		if (result != VmbErrorSuccess)
		{
			m_StreamBytesPerSecondFeature.reset();
			return result;
		}
	}
	result = m_StreamBytesPerSecondFeature->SetValue(value);
	return result;
}

VmbErrorType Vimba::SetBinningHorizontal(VmbInt64_t value)
{
	VmbErrorType result;
	FeaturePtr m_BinningHorizontalFeature;
	if (m_BinningHorizontalFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("BinningHorizontal", m_BinningHorizontalFeature);
		if (result != VmbErrorSuccess)
		{
			m_BinningHorizontalFeature.reset();
			return result;
		}
	}
	result = m_BinningHorizontalFeature->SetValue(value);
	return result;
}
VmbErrorType Vimba::SetBinningVertical(VmbInt64_t value)
{
	VmbErrorType result;
	FeaturePtr m_BinningVerticalFeature;
	if (m_BinningVerticalFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("BinningVertical", m_BinningVerticalFeature);
		if (result != VmbErrorSuccess)
		{
			m_BinningVerticalFeature.reset();
			return result;
		}
	}
	result = m_BinningVerticalFeature->SetValue(value);
	return result;
}

VmbErrorType Vimba::SetOffestX(VmbInt64_t value)
{
	VmbErrorType result;
	FeaturePtr m_OffsetXFeature;
	if (m_OffsetXFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("OffsetX", m_OffsetXFeature);
		if (result != VmbErrorSuccess)
		{
			m_OffsetXFeature.reset();
			return result;
		}
	}
	result = m_OffsetXFeature->SetValue(value);
	return result;
}
VmbErrorType Vimba::SetOffestY(VmbInt64_t value)
{
	VmbErrorType result;
	FeaturePtr m_OffsetYFeature;
	if (m_OffsetYFeature.get() == NULL)
	{
		VmbErrorType result;
		result = cameras[0]->GetFeatureByName("OffsetY", m_OffsetYFeature);
		if (result != VmbErrorSuccess)
		{
			m_OffsetYFeature.reset();
			return result;
		}
	}
	result = m_OffsetYFeature->SetValue(value);
	return result;
}

