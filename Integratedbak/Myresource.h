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
	int imgRow = 1038;	//����
	int imgCol = 1388;	//����
	int img1num;
	int img2num;
	CString img1;
	CString img2;
	int midWaveLen;	//���Ĳ���
	int bandWidth;	//����
	int freCount;		//Ƶ����
	CString backGround; //����ͼ
	double ampMin;	//��������С��ֵ
	double ampMax;	//����������ֵ
}SINGLEDATA;//���鴦���������

typedef struct multidata
{
	int stepLen = 1;
	int imgRow = 1038;	//����
	int imgCol = 1388;	//����
	int imgStartNum;
	int imgEndNum;
	int midWaveLen;	//���Ĳ���
	int bandWidth;	//����
	int freCount;		//Ƶ����
	CString backGround; //����ͼ
	double ampMin;	//��������С��ֵ
	double ampMax;	//����������ֵ
	double unwrapMax;//����Ʒ�Χ���ֵ
	double unwrapMin;//����Ʒ�Χ��Сֵ
	double umMax;
	double umMin;
	
}MULTIDATA;//���鴦���������

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
}AREA;//���鴦���������

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