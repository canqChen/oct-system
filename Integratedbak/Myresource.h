#ifndef __MYRESOURCE_H__
#define __MYRESOURCE_H__

typedef struct datainfo
{
	int imgRow = 1038;
	int imgCol = 1388;
	int targetWidth;
	int targetHeight;
	int midWaveLen = 840;
	int freCount;
	int bandWidth = 25;
	int startRow = 1;
	int endRow;
	int startCol = 1;
	int endCol = 0;
}IMGINFO;

typedef struct sgldata
{
	int stepLen = 1;
	int imgRow = 1038;	//行数
	int imgCol = 1388;	//列数
	int img1num;
	int img2num;
	CString img1;
	CString img2;
	int midWaveLen;	//中心波长
	int bandWidth;	//带宽
	int freCount;		//频点数
	CString backGround; //背景图
	double ampMin;	//非线性最小幅值
	double ampMax;	//非线性最大幅值
}SINGLEDATA;//单组处理程序数据

typedef struct multidata
{
	int stepLen = 1;
	int imgRow = 1038;	//行数
	int imgCol = 1388;	//列数
	int imgStartNum;
	int imgEndNum;
	int midWaveLen;	//中心波长
	int bandWidth;	//带宽
	int freCount;		//频点数
	CString backGround; //背景图
	double ampMin;	//非线性最小幅值
	double ampMax;	//非线性最大幅值
	double unwrapMax;//解卷绕范围最大值
	double unwrapMin;//解卷绕范围最小值
	double umMax;
	double umMin;
	
}MULTIDATA;//单组处理程序数据

typedef struct zarea
{
	int startRow = 1;
	int startCol = 20;
	int endRow = 1038;
	int endCol = 299;
	int PixelOffset = 0;
	int signalWidth()
	{
		return (endCol - startCol + 1);
	}
	int signalHeight()
	{
		return (endRow - startRow + 1);
	}
}AREA;//单组处理程序数据

bool DataCheck(CString dataname);

UINT sglRun(LPVOID pParam);

UINT PhaseDataProcess1(LPVOID pParam);
UINT PhaseDataProcess2(LPVOID pParam);
UINT PhaseDataProcess3(LPVOID pParam);
UINT PhaseDataProcess4(LPVOID pParam);

UINT UnPhaseDataProcess1(LPVOID pParam);
UINT UnPhaseDataProcess2(LPVOID pParam);
UINT UnPhaseDataProcess3(LPVOID pParam);
UINT UnPhaseDataProcess4(LPVOID pParam);

UINT Wrap(LPVOID pParam);
//UINT WrapGray(LPVOID pParam);

UINT showPhaseColorThead(LPVOID pParam);
UINT showUnPhaseColorThead(LPVOID pParam);
UINT unWrap(LPVOID pParam);
//UINT unWrapGray(LPVOID pParam);

UINT directWrap(LPVOID pParam);
UINT directUnWrap(LPVOID pParam);

//UINT directWrapGray(LPVOID pParam);
//UINT directUnWrapGray(LPVOID pParam);

void DoEvents();
CString GetWorkingDir();

#endif