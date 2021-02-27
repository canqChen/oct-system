#ifndef __Vimba__
#define __Vimba__

#include "DASDlg.h"
#include <VimbaCPP/Include/VimbaCPP.h>
#include "FrameObserver.h"
#include "Mymatrix.h"
#include <vector>
using namespace std;



class Vimba
{
public:
	static Vimba* GetInstance()
	{
		static Vimba pInstance;
		return &pInstance;
	}
	int phaseStep;
	double phaseMax;
	double phaseMin;
private:
	Vimba();
	~Vimba();
	VmbErrorType	err;
	CameraPtrVector	cameras;
	FramePtr		pFrame;
	FramePtr		pFrameAcquire;
	FramePtr		GetFrame();
	IFrameObserverPtr m_pFrameObserver;
	VmbUchar_t		*pBuffer;
	VmbUchar_t		*pProcessBuffer;
	VmbUchar_t		*pDrawBuffer;
	VmbUchar_t		*pLastBuffer;
	VmbInt64_t		m_nPixelFormat;
	VmbInt64_t		nWidth;
	VmbInt64_t		nHeight;
	BYTE			**ImageBuffer;
	BYTE			*mImage;
	vector<VmbUchar_t*> phaseVec;
	VmbUint32_t		TO_offset = 35;
	
	double			*mBuffer;
	void			ClearFrameQueue();
	void			AdjustPacketSize();
	void			ColorDataTransform();
	void			AMP_FREQ_DataProcess();
	void			PHASE_FREQ_DataProcess();
	
	VmbErrorType	SetStreamBytesPerSecond(VmbInt64_t value);
	VmbErrorType	SetExposureTimeAbs(double value);
	VmbErrorType	SetGainValue(double value);
	VmbErrorType	SetPixelFormat(VmbInt64_t value);
	VmbErrorType	SetWidth(VmbInt64_t value);
	VmbErrorType	SetHeight(VmbInt64_t value);
	VmbErrorType	SetBinningHorizontal(VmbInt64_t value);
	VmbErrorType	SetBinningVertical(VmbInt64_t value);
	VmbErrorType	SetOffestX(VmbInt64_t value);
	VmbErrorType	SetOffestY(VmbInt64_t value);

	VmbErrorType	GetPixelFormat(VmbInt64_t &value);
	VmbErrorType	GetWidth(VmbInt64_t & value);
	VmbErrorType	GetHeight(VmbInt64_t & value);
	VmbErrorType	GetImageSize(VmbInt64_t & value);
	VmbErrorType	QueueFrame(FramePtr pFrame);

	LPBITMAPINFO	BitmapInfoMono;
	BITMAPINFOHEADER BitmapInfoRGB;

	struct {
		BITMAPINFOHEADER  bmiHeader;
		RGBQUAD			  bmiColors[256];
	}GrayBitmapInfo;

public:
	int		offsetX;
	int		offsetY;
	int		acquisition_count;
	double  scale;
	bool	bIsCameraGet;
	bool	bAcquistion;
	bool	bSavingData;
	bool	InitCamera();
	bool	SetExposure(double exposure);
	bool	SetGain(double gain);
	void	Acquiring(double exposure, double gain, int interval, int samplingnum, CString path, int &ProgressValue);
	void	BgShot(double exposure, double gain, CString filename);
	void	Draw(CWnd *pWnd);
	//void	Draw(CDC *dc);
	void	DataProcess();
	void	ImageAcuqisition(WPARAM status);
	void	ApiShutdown();

	enum COLORMODE { MONO, RGB };
	enum MONITORMODE { NORMAL, AMP_FREQ, PHASE_FREQ };
	COLORMODE colormode;
	MONITORMODE m_mode;

	VmbErrorType    StartContinuousImageAcquisition();
	VmbErrorType    StopContinuousImageAcquisition();

	VmbInt64_t	nSize;

	//////////////////////////////////////////////////////
	void SetParam();
	int BinningHor;
	int BinningVer;
	////////////////////////////////////////////////////
};

#endif // !__Vimba__

