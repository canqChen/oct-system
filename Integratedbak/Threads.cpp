#include "stdafx.h"
#include "Myresource.h"
#include "Mymatrix.h"
#include "MyAlgorithm.h"
#include "SingleDlg.h"
#include "MultiDlg.h"
#include <vector>
#include <windows.h>
#include "TipsDlg.h"

#pragma warning(once:4018)

#define offset 0

using namespace std;
extern SINGLEDATA sglData;
extern MULTIDATA multiData;
extern CString filename_prefix;
AREA area;
vector<Mymatrix<double>*>p_UnPhaseDataVec1;
vector<Mymatrix<double>*>p_UnPhaseDataVec2;
vector<Mymatrix<double>*>p_UnPhaseDataVec3;
vector<Mymatrix<double>*>p_UnPhaseDataVec4;

vector<Mymatrix<double>*>p_directUnPhaseDataVec;
vector<Mymatrix<double>*>p_directPhaseDataVec;

vector<Mymatrix<double>*>p_PhaseDataVec1;
vector<Mymatrix<double>*>p_PhaseDataVec2;
vector<Mymatrix<double>*>p_PhaseDataVec3;
vector<Mymatrix<double>*>p_PhaseDataVec4;

Mymatrix<double>** unPhaseData1 = nullptr;
Mymatrix<double>** unPhaseData2 = nullptr;
Mymatrix<double>** unPhaseData3 = nullptr;
Mymatrix<double>** unPhaseData4 = nullptr;

Mymatrix<double>** PhaseData1 = nullptr;
Mymatrix<double>** PhaseData2 = nullptr;
Mymatrix<double>** PhaseData3 = nullptr;
Mymatrix<double>** PhaseData4 = nullptr;

Mymatrix<double>* Amp = NULL; //1038*694   目标694*1038

extern CString dir;

extern CString folder_dir;

bool is_exit1 = false;
bool is_exit2 = false;
bool is_exit3 = false;
bool is_exit4 = false;

bool is_Wrap = false;
bool is_Unwrap = false;

bool phase_exit = false;
bool unphase_exit = false;
bool phase_exit1 = false;
bool unphase_exit1 = false;
bool phase_exit2 = false;
bool phase_exit3 = false;
bool phase_exit4 = false;
bool unphase_exit2 = false;
bool unphase_exit3 = false;
bool unphase_exit4 = false;

bool sgl_exit = false;

int phaseProcess = 0;
int unphaseProcess = 0;
int singleProcess = 0;

UINT sglRun(LPVOID pParam)
{
	CSingleDlg::bRunning = true;
	FILE* fp_r1 = nullptr, *fp_r2 = nullptr, *fp_rbg = NULL;
	_wfopen_s(&fp_r1, sglData.img1, _T("rb"));
	_wfopen_s(&fp_r2, sglData.img2, _T("rb"));
	_wfopen_s(&fp_rbg, sglData.backGround, _T("rb"));

	Mymatrix<UINT16>* uint16img1 = new Mymatrix<UINT16>(fp_r1, sglData.imgRow, sglData.imgCol);
	Mymatrix<UINT16>* uint16img2 = new Mymatrix<UINT16>(fp_r2, sglData.imgRow, sglData.imgCol);
	Mymatrix<UINT16>* uint16Bgimg = new Mymatrix<UINT16>(fp_rbg, sglData.imgRow, sglData.imgCol);
	fclose(fp_r1);
	fclose(fp_r2);
	fclose(fp_rbg);
	Mymatrix<double> dbimg1(*uint16img1);	//双精度干涉图1
	Mymatrix<double>dbimg2(*uint16img2);	//双精度干涉图2
	Mymatrix<double>dbBgimg(*uint16Bgimg);	//双精度背景图
	delete uint16img1;
	delete uint16img2;
	delete uint16Bgimg;

	dbimg1 -= dbBgimg;
	dbimg2 -= dbBgimg;

	Mymatrix<double> *ones = new Mymatrix<double>(1, sglData.imgCol, 1.0);
	Mymatrix<double>*tempmean1 = mean(dbimg1);
	Mymatrix<double>*tempmean2 = mean(dbimg2);

	Mymatrix<double>*temp1 = matMultiply(*tempmean1, *ones);
	Mymatrix<double>*temp2 = matMultiply(*tempmean2, *ones);

	dbimg1 -= *temp1;
	dbimg2 -= *temp2;

	delete tempmean1;
	delete tempmean2;
	delete temp1;
	delete temp2;
	delete ones;
	if (sgl_exit)
	{
		CSingleDlg::bRunning = false;
		return 0;
	}
	dbimg1.Fliplr();
	dbimg2.Fliplr();

	Mymatrix<double> dbAfterimg1(sglData.imgRow, sglData.imgCol);
	Mymatrix<double> dbAfterimg2(sglData.imgRow, sglData.imgCol);
	dbAfterimg1 = dbimg1;
	dbAfterimg2 = dbimg2;
	singleProcess++;
	CTipsDlg::getInstance()->update(singleProcess);

	interpolation(dbimg1, dbimg2, dbAfterimg1, dbAfterimg2, 0);	//线性插值
	hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）

	singleProcess++;
	CTipsDlg::getInstance()->update(singleProcess);
	int start = 1; //细化频率段起点
	int end = sglData.freCount; //细化频率段终点
	int fs = sglData.imgCol;  //采样频率
	int M = sglData.freCount;	//细化频段的频点数   变换的长度
								//细化频段的跨度（步长）
	Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
	//细化频段的起始点  变换起始点
	Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));

	Mymatrix_i<double>dbCZTimg1(sglData.imgRow, M);
	Mymatrix_i<double>dbCZTimg2(sglData.imgRow, M);
	if (sgl_exit)
	{
		CSingleDlg::bRunning = false;
		return 0;
	}
	//CZT变换
	CZT(dbAfterimg1, dbAfterimg2, dbCZTimg1, dbCZTimg2, A, W, 0);
	if (sgl_exit)
	{
		CSingleDlg::bRunning = false;
		return 0;
	}
	singleProcess++;
	CTipsDlg::getInstance()->update(singleProcess);
	Mymatrix<double>*img1Amp = new Mymatrix<double>(dbCZTimg1.Row, dbCZTimg1.Col); //干涉图1幅值
	Mymatrix<double>img1Phase(dbCZTimg1.Row, dbCZTimg1.Col);  //干涉图1相位
	Mymatrix<double>img2Phase(dbCZTimg1.Row, dbCZTimg1.Col);	//干涉图2相位

	for (int i = 0; i < dbCZTimg1.Row; i++)
	{
		for (int j = 0; j < dbCZTimg1.Col; j++)
		{
			img1Phase.Data[i][j] = (dbCZTimg1.Data[i][j].arg());
			img2Phase.Data[i][j] = (dbCZTimg2.Data[i][j].arg());
			img1Amp->Data[i][j] = (dbCZTimg1.Data[i][j].mod());
		}
	}


#pragma region 幅值处理
	medFilt2(*img1Amp);
	//img1Amp->Flipud();
	//偏移
	if (area.PixelOffset < 0)
	{
		for (int i = area.signalWidth() / 2; i < area.signalWidth() + area.PixelOffset; i++)   //850*276
		{
			for (int j = 0; j < area.signalHeight(); j++)
			{
				img1Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] = img1Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i - area.PixelOffset];
			}
		}
	}
	else if (area.PixelOffset > 0)
	{
		for (int i = area.startCol - 1 + area.signalWidth() + area.PixelOffset; i > area.startCol - 1 + area.signalWidth() / 2 + area.PixelOffset; i--)   //850*276
		{
			for (int j = area.startRow - 1; j < area.startRow - 1 + area.signalHeight(); j++)
			{
				img1Amp->Data[j][i] = img1Amp->Data[j][i - area.PixelOffset];
			}
		}
	}

	Mymatrix<double>*Am0 = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth(), *img1Amp);
	//左右相加
	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
	{
		for (int j = 0; j < Am0->Row; j++)
		{
			Am0->Data[j][i] = img1Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + img1Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
		}
	}
	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
	{
		for (int j = 0; j < Am0->Row; j++)
		{
			Am0->Data[j][i] = img1Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + img1Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
		}
	}
	Mymatrix<double>*Am = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col / 2, *Am0);
	delete img1Amp;
	//medFilt2(*Am);
	//medFilt2(*Am0);

	/************** 归一化 *****************/
	//找出am的最大最小值
	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] > maxAm0)
			{
				maxAm0 = Am0->Data[i][j];
			}
			else if (Am0->Data[i][j] < minAm0)
			{
				minAm0 = Am0->Data[i][j];
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
		}
	}
	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] > maxAm)
			{
				maxAm = Am->Data[i][j];
			}
			else if (Am->Data[i][j] < minAm)
			{
				minAm = Am->Data[i][j];
			}
		}
	}
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
		}
	}

	/********************** 幅值处理 ************************/
	double max_A = sglData.ampMax;
	double min_A = sglData.ampMin;
	double a = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
	double B = -1.5*(max_A + min_A)*a;
	double C = -3.0*a*(pow(max_A, 2)) - 2 * B*max_A;
	double D = 1.0 - a*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;

	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] >= max_A)
			{
				Am->Data[i][j] = 1.0;
			}
			else if (Am->Data[i][j] <= min_A)
			{
				Am->Data[i][j] = 0;
			}
			else
			{
				Am->Data[i][j] = a*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] >= max_A)
			{
				Am0->Data[i][j] = 1.0;
			}
			else if (Am0->Data[i][j] <= min_A)
			{
				Am0->Data[i][j] = 0;
			}
			else
			{
				Am0->Data[i][j] = a*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
			}
		}
	}
	//Am->Flipud();
	//Am0->Flipud();
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] >= 1)
			{
				Am->Data[i][j] = 1;
			}
			else if (Am->Data[i][j] <= 0)
			{
				Am->Data[i][j] = 0;
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] >= 1)
			{
				Am0->Data[i][j] = 1;
			}
			else if (Am0->Data[i][j] <= 0)
			{
				Am0->Data[i][j] = 0;
			}
		}
	}
#pragma endregion

	double data1, data2;
	Mymatrix<double>*singlePhase = new Mymatrix<double>(img1Phase.Row, img1Phase.Col);
	for (int i = 0; i < img1Phase.Row; i++)
	{
		for (int j = 0; j < img1Phase.Col; j++)
		{
			data1 = sin((img2Phase.Data[i][j] - img1Phase.Data[i][j]));
			data2 = cos((img2Phase.Data[i][j] - img1Phase.Data[i][j]));
			singlePhase->Data[i][j] = atan2(data1, data2);		//1038*694
		}
	}
	
	if (area.PixelOffset < 0)
	{
		for (int i = area.signalWidth() / 2; i < area.signalWidth() + area.PixelOffset; i++)   //850*276
		{
			for (int j = 0; j < area.signalHeight(); j++)
			{
				singlePhase->Data[area.startRow - 1 + j][area.startCol - 1 + i] = singlePhase->Data[area.startRow - 1 + j][area.startCol - 1 + i - area.PixelOffset];
			}
		}
	}
	else if (area.PixelOffset > 0)
	{
		for (int i = area.startCol - 1 + area.signalWidth() + area.PixelOffset; i > area.startCol - 1 + area.signalWidth() / 2 + area.PixelOffset; i--)   //850*276
		{
			for (int j = area.startRow - 1; j <area.startRow - 1 + area.signalHeight(); j++)
			{
				singlePhase->Data[j][i] = singlePhase->Data[j][i - area.PixelOffset];
			}
		}
	}
	medFilt2(*singlePhase);
	//singlePhase->Flipud();
	singlePhase->resize(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth());
	
	
	if (sgl_exit)
	{
		delete singlePhase;
		delete Am;
		delete Am0;
		CSingleDlg::bRunning = false;
		return 0;
	}

	singleProcess++;
	CTipsDlg::getInstance()->update(singleProcess);


	HSI_Disp(singlePhase, Am, Am0, 0);

	delete singlePhase;
	delete Am;
	delete Am0;

	singleProcess = 0;
	CSingleDlg::bRunning = false;
	return 0;
}

UINT UnPhaseDataProcess1(LPVOID pParam)
{
	CMultiDlg::bRunning = true;
	if (Amp != NULL)
	{
		delete Amp;
		Amp = NULL;
	}
	FILE * fp_r1 = nullptr, *fp_r2 = nullptr, *fp_rbg = NULL;
	_wfopen_s(&fp_rbg, multiData.backGround, _T("rb"));
	Mymatrix<UINT16>* uint16Bgimg = new Mymatrix<UINT16>(fp_rbg, multiData.imgRow, multiData.imgCol);
	Mymatrix<double>dbBgimg(*uint16Bgimg);	//双精度背景图
	fclose(fp_rbg);
	delete uint16Bgimg;
	CString dataname1, dataname2;

	Mymatrix<double> *ones = new Mymatrix<double>(1, multiData.imgCol, 1.0);

	Mymatrix<UINT16>* uint16img1 = NULL;
	Mymatrix<UINT16>* uint16img2 = NULL;

	Mymatrix<double>*tempmean1 = NULL;
	Mymatrix<double>*tempmean2 = NULL;

	Mymatrix<double>*temp1 = NULL;
	Mymatrix<double>*temp2 = NULL;

	if (unphase_exit1)
	{
		CMultiDlg::bRunning = false;
		delete ones;
		is_exit1 = true;
		return 0;
	}

	int start = 1; //细化频率段起点
	int end = multiData.freCount; //细化频率段终点
	int fs = multiData.imgCol;  //采样频率
	int M = multiData.freCount;	//细化频段的频点数   变换的长度
								//细化频段的跨度（步长）
	Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
	//细化频段的起始点  变换起始点
	Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));
	Mymatrix<double> dbAfterimg1(multiData.imgRow, multiData.imgCol);
	Mymatrix<double> dbAfterimg2(multiData.imgRow, multiData.imgCol);
	Mymatrix_i<double>dbCZTimg1(multiData.imgRow, M);
	Mymatrix_i<double>dbCZTimg2(multiData.imgRow, M);
	Mymatrix<double>img1Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double>img2Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double>*dbimg1 = NULL;
	Mymatrix<double>*dbimg2 = NULL;

	int count = multiData.imgEndNum - multiData.imgStartNum;
	unPhaseData1 = new Mymatrix<double>*[int(count / 4)];
	for (int i = 0; i<int(count / 4); i++)
	{
		unPhaseData1[i] = new Mymatrix<double>(img1Phase.Row, img1Phase.Col);
	}
	for (int i = multiData.imgStartNum, index = 0; i < multiData.imgStartNum + int(count / 4); i += multiData.stepLen, index++)
	{
		if (unphase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit1 = true;
			return 0;
		}
		dataname1.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i);
		_wfopen_s(&fp_r1, dataname1, _T("rb"));
		dataname2.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i + multiData.stepLen);
		_wfopen_s(&fp_r2, dataname2, _T("rb"));

		uint16img1 = new Mymatrix<UINT16>(fp_r1, multiData.imgRow, multiData.imgCol);
		uint16img2 = new Mymatrix<UINT16>(fp_r2, multiData.imgRow, multiData.imgCol);
		fclose(fp_r1);
		fclose(fp_r2);

		dbimg1 = new Mymatrix<double>(*uint16img1);	//双精度干涉图1
		dbimg2 = new Mymatrix<double>(*uint16img2);	//双精度干涉图2

		delete uint16img1;
		delete uint16img2;

		*dbimg1 -= dbBgimg;
		*dbimg2 -= dbBgimg;

		tempmean1 = mean(*dbimg1);
		tempmean2 = mean(*dbimg2);

		temp1 = matMultiply(*tempmean1, *ones);
		temp2 = matMultiply(*tempmean2, *ones);

		*dbimg1 -= *temp1;
		*dbimg2 -= *temp2;

		delete tempmean1;
		delete tempmean2;
		delete temp1;
		delete temp2;

		(*dbimg1).Fliplr();
		(*dbimg2).Fliplr();

		dbAfterimg1 = *dbimg1;
		dbAfterimg2 = *dbimg2;
		if (unphase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete dbimg1;
			delete dbimg2;
			delete ones;
			is_exit1 = true;
			return 0;
		}
		interpolation(*dbimg1, *dbimg2, dbAfterimg1, dbAfterimg2, 1);	//线性插值
		hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）
		delete dbimg1;
		delete dbimg2;
		if (unphase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit1 = true;
			return 0;
		}
		//CZT变换
		CZT(dbAfterimg1, dbAfterimg2, dbCZTimg1, dbCZTimg2, A, W, 1);

		if (unphase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit1 = true;
			return 0;
		}
		if (Amp == NULL)
		{
			//Amp = dbCZTimg1.getAmp();
			Amp = new Mymatrix<double>(dbCZTimg1.Row, dbCZTimg1.Col);  //1038*694
			for (int i = 0; i < dbCZTimg1.Row; i++)
			{
				for (int j = 0; j < dbCZTimg1.Col; j++)
				{
					Amp->Data[i][j] = (dbCZTimg1.Data[i][j].mod());
				}
			}
			medFilt2(*Amp);
			//Amp->Flipud();
			if (area.PixelOffset < 0)
			{
				for (int i = area.signalWidth() / 2; i < area.signalWidth() + area.PixelOffset; i++)   //850*276
				{
					for (int j = 0; j < area.signalHeight(); j++)
					{
						Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i - area.PixelOffset];
					}
				}
			}
			else if (area.PixelOffset > 0)
			{
				for (int i = area.startCol - 1 + area.signalWidth() + area.PixelOffset; i > area.startCol - 1 + area.signalWidth() / 2 + area.PixelOffset; i--)   //850*276
				{
					for (int j = area.startRow - 1; j <area.startRow - 1 + area.signalHeight(); j++)
					{
						Amp->Data[j][i] = Amp->Data[j][i - area.PixelOffset];
					}
				}
			}
		}
		for (int i = 0; i < dbCZTimg1.Row; i++)
		{
			for (int j = 0; j < dbCZTimg1.Col; j++)
			{
				img1Phase.Data[i][j] = (dbCZTimg1.Data[i][j].arg());
				img2Phase.Data[i][j] = (dbCZTimg2.Data[i][j].arg());
			}
		}
		double data1, data2;
		for (int j = 0; j < img1Phase.Row; j++)
		{
			for (int k = 0; k < img1Phase.Col; k++)
			{
				data1 = sin((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				data2 = cos((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				unPhaseData1[index]->Data[j][k] = atan2(data1, data2);		//1038*694
			}
		}
		if (unphase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit1 = true;
			return 0;
		}
		p_UnPhaseDataVec1.push_back((unPhaseData1[index]));
		unphaseProcess++;
		CTipsDlg::getInstance()->update(unphaseProcess);
	}

	delete ones;
	CMultiDlg::bRunning = false;
	is_exit1 = true;
	return 0;
}
UINT UnPhaseDataProcess2(LPVOID pParam)
{
	CMultiDlg::bRunning = true;
	FILE * fp_r1 = nullptr, *fp_r2 = nullptr, *fp_rbg = NULL;
	_wfopen_s(&fp_rbg, multiData.backGround, _T("rb"));
	Mymatrix<UINT16>* uint16Bgimg = new Mymatrix<UINT16>(fp_rbg, multiData.imgRow, multiData.imgCol);
	Mymatrix<double>dbBgimg(*uint16Bgimg);	//双精度背景图
	fclose(fp_rbg);
	delete uint16Bgimg;
	CString dataname1, dataname2;

	Mymatrix<double> *ones = new Mymatrix<double>(1, multiData.imgCol, 1.0);

	Mymatrix<UINT16>* uint16img1;
	Mymatrix<UINT16>* uint16img2;

	Mymatrix<double>*tempmean1;
	Mymatrix<double>*tempmean2;

	Mymatrix<double>*temp1;
	Mymatrix<double>*temp2;

	if (unphase_exit2)
	{
		CMultiDlg::bRunning = false;
		delete ones;
		is_exit2 = true;
		return 0;
	}

	int start = 1; //细化频率段起点
	int end = multiData.freCount; //细化频率段终点
	int fs = multiData.imgCol;  //采样频率
	int M = multiData.freCount;	//细化频段的频点数   变换的长度
								//细化频段的跨度（步长）
	Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
	//细化频段的起始点  变换起始点
	Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));
	Mymatrix<double> dbAfterimg1(multiData.imgRow, multiData.imgCol);
	Mymatrix<double> dbAfterimg2(multiData.imgRow, multiData.imgCol);
	Mymatrix_i<double>dbCZTimg1(multiData.imgRow, M);
	Mymatrix_i<double>dbCZTimg2(multiData.imgRow, M);
	Mymatrix<double>img1Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double>img2Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double>*dbimg1 = NULL;
	Mymatrix<double>*dbimg2 = NULL;

	int count = multiData.imgEndNum - multiData.imgStartNum;
	unPhaseData2 = new Mymatrix<double>*[int(count / 4)];
	for (int i = 0; i<int(count / 4); i++)
	{
		unPhaseData2[i] = new Mymatrix<double>(img1Phase.Row, img1Phase.Col);
	}

	for (int i = int(count / 4) + multiData.imgStartNum, index = 0; i < multiData.imgStartNum + (int)(count / 4) * 2; i += multiData.stepLen, index++)
	{
		if (unphase_exit2)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit2 = true;
			return 0;
		}
		dataname1.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i);
		_wfopen_s(&fp_r1, dataname1, _T("rb"));
		dataname2.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i + multiData.stepLen);
		_wfopen_s(&fp_r2, dataname2, _T("rb"));

		uint16img1 = new Mymatrix<UINT16>(fp_r1, multiData.imgRow, multiData.imgCol);
		uint16img2 = new Mymatrix<UINT16>(fp_r2, multiData.imgRow, multiData.imgCol);
		fclose(fp_r1);
		fclose(fp_r2);

		dbimg1 = new Mymatrix<double>(*uint16img1);	//双精度干涉图1
		dbimg2 = new Mymatrix<double>(*uint16img2);	//双精度干涉图2

		delete uint16img1;
		delete uint16img2;

		*dbimg1 -= dbBgimg;
		*dbimg2 -= dbBgimg;

		tempmean1 = mean(*dbimg1);
		tempmean2 = mean(*dbimg2);

		temp1 = matMultiply(*tempmean1, *ones);
		temp2 = matMultiply(*tempmean2, *ones);

		*dbimg1 -= *temp1;
		*dbimg2 -= *temp2;

		delete tempmean1;
		delete tempmean2;
		delete temp1;
		delete temp2;

		(*dbimg1).Fliplr();
		(*dbimg2).Fliplr();

		dbAfterimg1 = *dbimg1;
		dbAfterimg2 = *dbimg2;
		if (unphase_exit2)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			delete dbimg2;
			is_exit2 = true;
			return 0;
		}
		interpolation(*dbimg1, *dbimg2, dbAfterimg1, dbAfterimg2, 1);	//线性插值
		hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）
		delete dbimg1;
		delete dbimg2;
		if (unphase_exit2)
		{
			delete ones;
			is_exit2 = true;
			return 0;
		}
		//CZT变换
		CZT(dbAfterimg1, dbAfterimg2, dbCZTimg1, dbCZTimg2, A, W, 1);
		if (unphase_exit2)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit2 = true;
			return 0;
		}
		for (int i = 0; i < dbCZTimg1.Row; i++)
		{
			for (int j = 0; j < dbCZTimg1.Col; j++)
			{
				img1Phase.Data[i][j] = (dbCZTimg1.Data[i][j].arg());
				img2Phase.Data[i][j] = (dbCZTimg2.Data[i][j].arg());
			}
		}
		if (unphase_exit2)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit2 = true;
			return 0;
		}
		double data1, data2;
		for (int j = 0; j < img1Phase.Row; j++)
		{
			for (int k = 0; k < img1Phase.Col; k++)
			{
				data1 = sin((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				data2 = cos((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				unPhaseData2[index]->Data[j][k] = atan2(data1, data2);		//1038*694
			}
		}
		if (unphase_exit2)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit2 = true;
			return 0;
		}
		p_UnPhaseDataVec2.push_back((unPhaseData2[index]));
		unphaseProcess++;
		CTipsDlg::getInstance()->update(unphaseProcess);
	}

	delete ones;
	CMultiDlg::bRunning = false;
	is_exit2 = true;
	return 0;
}

UINT UnPhaseDataProcess3(LPVOID pParam)
{
	CMultiDlg::bRunning = true;

	FILE * fp_r1 = nullptr, *fp_r2 = nullptr, *fp_rbg = NULL;
	_wfopen_s(&fp_rbg, multiData.backGround, _T("rb"));
	Mymatrix<UINT16>* uint16Bgimg = new Mymatrix<UINT16>(fp_rbg, multiData.imgRow, multiData.imgCol);
	Mymatrix<double>dbBgimg(*uint16Bgimg);	//双精度背景图
	fclose(fp_rbg);
	delete uint16Bgimg;
	CString dataname1, dataname2;

	Mymatrix<double> *ones = new Mymatrix<double>(1, multiData.imgCol, 1.0);

	Mymatrix<UINT16>* uint16img1;
	Mymatrix<UINT16>* uint16img2;

	Mymatrix<double>*tempmean1;
	Mymatrix<double>*tempmean2;

	Mymatrix<double>*temp1;
	Mymatrix<double>*temp2;

	if (unphase_exit3)
	{
		CMultiDlg::bRunning = false;
		delete ones;
		is_exit3 = true;
		return 0;
	}

	int start = 1; //细化频率段起点
	int end = multiData.freCount; //细化频率段终点
	int fs = multiData.imgCol;  //采样频率
	int M = multiData.freCount;	//细化频段的频点数   变换的长度
								//细化频段的跨度（步长）
	Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
	//细化频段的起始点  变换起始点
	Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));
	Mymatrix<double> dbAfterimg1(multiData.imgRow, multiData.imgCol);
	Mymatrix<double> dbAfterimg2(multiData.imgRow, multiData.imgCol);
	Mymatrix_i<double>dbCZTimg1(multiData.imgRow, M);
	Mymatrix_i<double>dbCZTimg2(multiData.imgRow, M);
	Mymatrix<double> img1Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double> img2Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double>*dbimg1 = NULL;
	Mymatrix<double>*dbimg2 = NULL;

	int count = multiData.imgEndNum - multiData.imgStartNum;
	unPhaseData3 = new Mymatrix<double>*[int(count / 4)];

	for (int i = 0; i<int(count / 4); i++)
	{
		unPhaseData3[i] = new Mymatrix<double>(img1Phase.Row, img1Phase.Col);
	}

	for (int i = int(count / 4) * 2 + multiData.imgStartNum, index = 0; i < int(count / 4) * 3 + multiData.imgStartNum; i += multiData.stepLen, index++)
	{
		if (unphase_exit3)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit3 = true;
			return 0;
		}
		dataname1.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i);
		_wfopen_s(&fp_r1, dataname1, _T("rb"));
		dataname2.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i + multiData.stepLen);
		_wfopen_s(&fp_r2, dataname2, _T("rb"));

		uint16img1 = new Mymatrix<UINT16>(fp_r1, multiData.imgRow, multiData.imgCol);
		uint16img2 = new Mymatrix<UINT16>(fp_r2, multiData.imgRow, multiData.imgCol);
		fclose(fp_r1);
		fclose(fp_r2);

		dbimg1 = new Mymatrix<double>(*uint16img1);	//双精度干涉图1
		dbimg2 = new Mymatrix<double>(*uint16img2);	//双精度干涉图2

		delete uint16img1;
		delete uint16img2;

		*dbimg1 -= dbBgimg;
		*dbimg2 -= dbBgimg;

		tempmean1 = mean(*dbimg1);
		tempmean2 = mean(*dbimg2);

		temp1 = matMultiply(*tempmean1, *ones);
		temp2 = matMultiply(*tempmean2, *ones);

		*dbimg1 -= *temp1;
		*dbimg2 -= *temp2;

		delete tempmean1;
		delete tempmean2;
		delete temp1;
		delete temp2;

		(*dbimg1).Fliplr();
		(*dbimg2).Fliplr();

		dbAfterimg1 = *dbimg1;
		dbAfterimg2 = *dbimg2;
		if (unphase_exit3)
		{
			CMultiDlg::bRunning = false;
			delete dbimg1;
			delete dbimg2;
			delete ones;
			is_exit3 = true;
			return 0;
		}
		interpolation(*dbimg1, *dbimg2, dbAfterimg1, dbAfterimg2, 1);	//线性插值
		hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）
		delete dbimg1;
		delete dbimg2;
		if (unphase_exit3)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit3 = true;
			return 0;
		}
		//CZT变换
		CZT(dbAfterimg1, dbAfterimg2, dbCZTimg1, dbCZTimg2, A, W, 1);
		if (unphase_exit3)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit3 = true;
			return 0;
		}
		for (int i = 0; i < dbCZTimg1.Row; i++)
		{
			for (int j = 0; j < dbCZTimg1.Col; j++)
			{
				img1Phase.Data[i][j] = (dbCZTimg1.Data[i][j].arg());
				img2Phase.Data[i][j] = (dbCZTimg2.Data[i][j].arg());
			}
		}
		if (unphase_exit3)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit3 = true;
			return 0;
		}
		double data1, data2;
		for (int j = 0; j < img1Phase.Row; j++)
		{
			for (int k = 0; k < img1Phase.Col; k++)
			{
				data1 = sin((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				data2 = cos((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				unPhaseData3[index]->Data[j][k] = atan2(data1, data2);		//1038*694
			}
		}
		if (unphase_exit3)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit3 = true;
			return 0;
		}
		p_UnPhaseDataVec3.push_back((unPhaseData3[index]));
		unphaseProcess++;
		CTipsDlg::getInstance()->update(unphaseProcess);
	}

	delete ones;
	CMultiDlg::bRunning = false;
	is_exit3 = true;
	return 0;
}
UINT UnPhaseDataProcess4(LPVOID pParam)
{
	CMultiDlg::bRunning = true;

	FILE * fp_r1 = nullptr, *fp_r2 = nullptr, *fp_rbg = NULL;
	_wfopen_s(&fp_rbg, multiData.backGround, _T("rb"));
	Mymatrix<UINT16>* uint16Bgimg = new Mymatrix<UINT16>(fp_rbg, multiData.imgRow, multiData.imgCol);
	Mymatrix<double>dbBgimg(*uint16Bgimg);	//双精度背景图
	fclose(fp_rbg);
	delete uint16Bgimg;
	CString dataname1, dataname2;

	Mymatrix<double> *ones = new Mymatrix<double>(1, multiData.imgCol, 1.0);

	Mymatrix<UINT16>* uint16img1;
	Mymatrix<UINT16>* uint16img2;

	Mymatrix<double>*tempmean1;
	Mymatrix<double>*tempmean2;

	Mymatrix<double>*temp1;
	Mymatrix<double>*temp2;

	if (unphase_exit4)
	{
		CMultiDlg::bRunning = false;
		delete ones;
		is_exit4 = true;
		return 0;
	}

	int start = 1; //细化频率段起点
	int end = multiData.freCount; //细化频率段终点
	int fs = multiData.imgCol;  //采样频率
	int M = multiData.freCount;	//细化频段的频点数   变换的长度
								//细化频段的跨度（步长）
	Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
	//细化频段的起始点  变换起始点
	Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));
	Mymatrix<double> dbAfterimg1(multiData.imgRow, multiData.imgCol);
	Mymatrix<double> dbAfterimg2(multiData.imgRow, multiData.imgCol);
	Mymatrix_i<double>dbCZTimg1(multiData.imgRow, M);
	Mymatrix_i<double>dbCZTimg2(multiData.imgRow, M);
	Mymatrix<double> img1Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double> img2Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double>*dbimg1;
	Mymatrix<double>*dbimg2;

	int count = multiData.imgEndNum - multiData.imgStartNum;
	unPhaseData4 = new Mymatrix<double>*[int(count / 4) + count % 4];
	for (int i = 0; i<int(count / 4) + count % 4; i++)
	{
		unPhaseData4[i] = new Mymatrix<double>(img1Phase.Row, img1Phase.Col);
	}
	for (int i = int(count / 4) * 3 + multiData.imgStartNum, index = 0; i < multiData.imgEndNum; i += multiData.stepLen, index++)
	{
		if (unphase_exit4)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit4 = true;
			return 0;
		}
		dataname1.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i);
		_wfopen_s(&fp_r1, dataname1, _T("rb"));
		dataname2.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i + multiData.stepLen);
		_wfopen_s(&fp_r2, dataname2, _T("rb"));

		uint16img1 = new Mymatrix<UINT16>(fp_r1, multiData.imgRow, multiData.imgCol);
		uint16img2 = new Mymatrix<UINT16>(fp_r2, multiData.imgRow, multiData.imgCol);
		fclose(fp_r1);
		fclose(fp_r2);

		dbimg1 = new Mymatrix<double>(*uint16img1);	//双精度干涉图1
		dbimg2 = new Mymatrix<double>(*uint16img2);	//双精度干涉图2

		delete uint16img1;
		delete uint16img2;

		*dbimg1 -= dbBgimg;
		*dbimg2 -= dbBgimg;

		tempmean1 = mean(*dbimg1);
		tempmean2 = mean(*dbimg2);

		temp1 = matMultiply(*tempmean1, *ones);
		temp2 = matMultiply(*tempmean2, *ones);

		*dbimg1 -= *temp1;
		*dbimg2 -= *temp2;

		delete tempmean1;
		delete tempmean2;
		delete temp1;
		delete temp2;

		(*dbimg1).Fliplr();
		(*dbimg2).Fliplr();

		dbAfterimg1 = *dbimg1;
		dbAfterimg2 = *dbimg2;
		if (unphase_exit4)
		{
			CMultiDlg::bRunning = false;
			delete dbimg1;
			delete dbimg2;
			delete ones;
			is_exit4 = true;
			return 0;
		}
		interpolation(*dbimg1, *dbimg2, dbAfterimg1, dbAfterimg2, 1);	//线性插值
		hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）
		delete dbimg1;
		delete dbimg2;
		if (unphase_exit)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit4 = true;
			return 0;
		}
		//CZT变换
		CZT(dbAfterimg1, dbAfterimg2, dbCZTimg1, dbCZTimg2, A, W, 1);
		if (unphase_exit4)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit4 = true;
			return 0;
		}
		for (int i = 0; i < dbCZTimg1.Row; i++)
		{
			for (int j = 0; j < dbCZTimg1.Col; j++)
			{
				img2Phase.Data[i][j] = (dbCZTimg2.Data[i][j].arg());
				img1Phase.Data[i][j] = (dbCZTimg1.Data[i][j].arg());
			}
		}
		double data1, data2;
		for (int j = 0; j < img1Phase.Row; j++)
		{
			for (int k = 0; k < img1Phase.Col; k++)
			{
				data1 = sin((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				data2 = cos((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				unPhaseData4[index]->Data[j][k] = atan2(data1, data2);		//1038*694
			}
		}
		if (unphase_exit4)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			is_exit4 = true;
			return 0;
		}
		p_UnPhaseDataVec4.push_back((unPhaseData4[index]));
		unphaseProcess++;
		CTipsDlg::getInstance()->update(unphaseProcess);
	}

	delete ones;

	CMultiDlg::bRunning = false;
	is_exit4 = true;
	return 0;
}

UINT PhaseDataProcess1(LPVOID pParam)
{
	CMultiDlg::bRunning = true;
	if (Amp != NULL)
	{
		delete Amp;
		Amp = NULL;
	}
	FILE * fp_r1 = nullptr, *fp_r2 = nullptr, *fp_rbg = NULL;
	_wfopen_s(&fp_rbg, multiData.backGround, _T("rb"));
	Mymatrix<UINT16>* uint16Bgimg = new Mymatrix<UINT16>(fp_rbg, multiData.imgRow, multiData.imgCol);
	Mymatrix<double>dbBgimg(*uint16Bgimg);	//双精度背景图
	fclose(fp_rbg);
	delete uint16Bgimg;
	CString dataname1, dataname2;

	Mymatrix<double> *ones = new Mymatrix<double>(1, multiData.imgCol, 1.0);
	if (phase_exit1)
	{
		CMultiDlg::bRunning = false;
		delete ones;

		is_exit1 = true;
		return 0;
	}
	dataname1.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgStartNum);
	_wfopen_s(&fp_r1, dataname1, _T("rb"));
	Mymatrix<UINT16>*  uint16img1 = new Mymatrix<UINT16>(fp_r1, multiData.imgRow, multiData.imgCol);
	fclose(fp_r1);
	Mymatrix<double>*dbimg1 = new Mymatrix<double>(*uint16img1);	//双精度干涉图1
	delete uint16img1;
	*dbimg1 -= dbBgimg;
	Mymatrix<double>*tempmean1 = mean(*dbimg1);
	Mymatrix<double>*temp1 = matMultiply(*tempmean1, *ones);
	*dbimg1 -= *temp1;
	(*dbimg1).Fliplr();

	delete tempmean1;
	delete temp1;

	Mymatrix<UINT16>* uint16img2 = NULL;

	Mymatrix<double>*tempmean2 = NULL;

	Mymatrix<double>*temp2 = NULL;

	if (phase_exit1)
	{
		CMultiDlg::bRunning = false;
		delete ones;
		delete dbimg1;
		is_exit1 = true;
		return 0;
	}

	int start = 1; //细化频率段起点
	int end = multiData.freCount; //细化频率段终点
	int fs = multiData.imgCol;  //采样频率
	int M = multiData.freCount;	//细化频段的频点数   变换的长度
								//细化频段的跨度（步长）
	Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
	//细化频段的起始点  变换起始点
	Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));
	Mymatrix<double> dbAfterimg1(multiData.imgRow, multiData.imgCol);
	Mymatrix<double> dbAfterimg2(multiData.imgRow, multiData.imgCol);
	Mymatrix_i<double>dbCZTimg1(multiData.imgRow, M);
	Mymatrix_i<double>dbCZTimg2(multiData.imgRow, M);
	Mymatrix<double>img1Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double>img2Phase(multiData.imgRow, multiData.freCount);

	Mymatrix<double>*dbimg2 = NULL;
	int count = multiData.imgEndNum - multiData.imgStartNum+1;
	PhaseData1 = new Mymatrix<double>*[int(count / 4)];
	for (int i = 0; i<int(count / 4); i++)
	{
		PhaseData1[i] = new Mymatrix<double>(img1Phase.Row, img1Phase.Col);
	}
	for (int i = multiData.imgStartNum+1 , index = 0; i < multiData.imgStartNum + int(count / 4)+1 ; i += multiData.stepLen, index++)
	{
		if (phase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit1 = true;
			return 0;
		}
		dataname2.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i);
		_wfopen_s(&fp_r2, dataname2, _T("rb"));

		uint16img2 = new Mymatrix<UINT16>(fp_r2, multiData.imgRow, multiData.imgCol);

		fclose(fp_r2);

		dbimg2 = new Mymatrix<double>(*uint16img2);	//双精度干涉图2

		delete uint16img2;

		*dbimg2 -= dbBgimg;

		tempmean2 = mean(*dbimg2);

		temp2 = matMultiply(*tempmean2, *ones);

		*dbimg2 -= *temp2;

		delete tempmean2;

		delete temp2;

		(*dbimg2).Fliplr();

		dbAfterimg1 = *dbimg1;
		dbAfterimg2 = *dbimg2;
		if (phase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete dbimg2;
			delete ones;
			delete dbimg1;
			is_exit1 = true;
			return 0;
		}
		interpolation(*dbimg1, *dbimg2, dbAfterimg1, dbAfterimg2, 1);	//线性插值
		hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）

		delete dbimg2;
		if (phase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit1 = true;
			return 0;
		}
		//CZT变换
		CZT(dbAfterimg1, dbAfterimg2, dbCZTimg1, dbCZTimg2, A, W, 1);
		if (phase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit1 = true;
			return 0;
		}
		if (Amp == NULL)
		{
			Amp = new Mymatrix<double>(dbCZTimg1.Row, dbCZTimg1.Col);  //1038*694
			for (int i = 0; i < dbCZTimg1.Row; i++)
			{
				for (int j = 0; j < dbCZTimg1.Col; j++)
				{
					Amp->Data[i][j] = (dbCZTimg1.Data[i][j].mod());
				}
			}

			medFilt2(*Amp);
		//	Amp->Flipud();
			if (area.PixelOffset < 0)
			{
				for (int i = area.signalWidth() / 2; i < area.signalWidth() + area.PixelOffset; i++)   //850*276
				{
					for (int j = 0; j < area.signalHeight(); j++)
					{
						Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i - area.PixelOffset];
					}
				}
			}
			else if (area.PixelOffset > 0)
			{
				for (int i = area.startCol - 1 + area.signalWidth()+ area.PixelOffset; i > area.startCol - 1 + area.signalWidth()/2 + area.PixelOffset; i--)   //850*276
				{
					for (int j = area.startRow - 1; j <area.startRow - 1+ area.signalHeight(); j++)
					{
						Amp->Data[j][i] = Amp->Data[j][i - area.PixelOffset];
					}
				}
			}
		}
		if (phase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit1 = true;
			return 0;
		}
		for (int i = 0; i < dbCZTimg1.Row; i++)
		{
			for (int j = 0; j < dbCZTimg1.Col; j++)
			{
				img1Phase.Data[i][j] = (dbCZTimg1.Data[i][j].arg());
				img2Phase.Data[i][j] = (dbCZTimg2.Data[i][j].arg());
			}
		}
		if (phase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit1 = true;
			return 0;
		}

		double data1, data2;
		for (int j = 0; j < img1Phase.Row; j++)
		{
			for (int k = 0; k < img1Phase.Col; k++)
			{
				data1 = sin((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				data2 = cos((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				PhaseData1[index]->Data[j][k] = atan2(data1, data2);		//1038*694
			}
		}
		if (phase_exit1)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit1 = true;
			return 0;
		}
		p_PhaseDataVec1.push_back((PhaseData1[index]));
		phaseProcess++;
		CTipsDlg::getInstance()->update(phaseProcess);
	}
	delete ones;
	delete dbimg1;
	CMultiDlg::bRunning = false;
	is_exit1 = true;
	return 0;
}

UINT PhaseDataProcess2(LPVOID pParam)
{
	CMultiDlg::bRunning = true;

	FILE * fp_r1 = nullptr, *fp_r2 = nullptr, *fp_rbg = NULL;
	_wfopen_s(&fp_rbg, multiData.backGround, _T("rb"));
	Mymatrix<UINT16>* uint16Bgimg = new Mymatrix<UINT16>(fp_rbg, multiData.imgRow, multiData.imgCol);
	Mymatrix<double>dbBgimg(*uint16Bgimg);	//双精度背景图
	fclose(fp_rbg);
	delete uint16Bgimg;
	CString dataname1, dataname2;
	if (phase_exit2)
	{
		CMultiDlg::bRunning = false;
		is_exit2 = true;
		return 0;
	}
	Mymatrix<double> *ones = new Mymatrix<double>(1, multiData.imgCol, 1.0);

	dataname1.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgStartNum);
	_wfopen_s(&fp_r1, dataname1, _T("rb"));
	Mymatrix<UINT16>*  uint16img1 = new Mymatrix<UINT16>(fp_r1, multiData.imgRow, multiData.imgCol);
	fclose(fp_r1);
	Mymatrix<double>*dbimg1 = new Mymatrix<double>(*uint16img1);	//双精度干涉图1
	delete uint16img1;
	*dbimg1 -= dbBgimg;
	Mymatrix<double>*tempmean1 = mean(*dbimg1);
	Mymatrix<double>*temp1 = matMultiply(*tempmean1, *ones);
	*dbimg1 -= *temp1;
	(*dbimg1).Fliplr();

	delete tempmean1;
	delete temp1;

	Mymatrix<UINT16>* uint16img2 = NULL;

	Mymatrix<double>*tempmean2 = NULL;

	Mymatrix<double>*temp2 = NULL;

	if (phase_exit2)
	{
		CMultiDlg::bRunning = false;
		delete ones;
		delete dbimg1;
		is_exit2 = true;
		return 0;
	}

	int start = 1; //细化频率段起点
	int end = multiData.freCount; //细化频率段终点
	int fs = multiData.imgCol;  //采样频率
	int M = multiData.freCount;	//细化频段的频点数   变换的长度
								//细化频段的跨度（步长）
	Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
	//细化频段的起始点  变换起始点
	Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));
	Mymatrix<double> dbAfterimg1(multiData.imgRow, multiData.imgCol);
	Mymatrix<double> dbAfterimg2(multiData.imgRow, multiData.imgCol);
	Mymatrix_i<double>dbCZTimg1(multiData.imgRow, M);
	Mymatrix_i<double>dbCZTimg2(multiData.imgRow, M);
	Mymatrix<double>img1Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double>img2Phase(multiData.imgRow, multiData.freCount);

	Mymatrix<double>*dbimg2 = NULL;

	int count = multiData.imgEndNum - multiData.imgStartNum+1;
	PhaseData2 = new Mymatrix<double>*[int(count / 4)];
	for (int i = 0; i<int(count / 4); i++)
	{
		PhaseData2[i] = new Mymatrix<double>(img1Phase.Row, img1Phase.Col);
	}
	for (int i = int(count / 4) + multiData.imgStartNum + 1, index = 0; i < multiData.imgStartNum + (int)(count / 4) * 2 + 1; i += multiData.stepLen, index++)
	{
		if (phase_exit2)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			return 0;
		}

		dataname2.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i);
		_wfopen_s(&fp_r2, dataname2, _T("rb"));

		uint16img2 = new Mymatrix<UINT16>(fp_r2, multiData.imgRow, multiData.imgCol);

		fclose(fp_r2);

		dbimg2 = new Mymatrix<double>(*uint16img2);	//双精度干涉图2

		delete uint16img2;

		*dbimg2 -= dbBgimg;

		tempmean2 = mean(*dbimg2);

		temp2 = matMultiply(*tempmean2, *ones);

		*dbimg2 -= *temp2;

		delete tempmean2;

		delete temp2;

		(*dbimg2).Fliplr();

		dbAfterimg1 = *dbimg1;
		dbAfterimg2 = *dbimg2;
		if (phase_exit2)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			delete dbimg2;
			is_exit2 = true;
			return 0;
		}
		interpolation(*dbimg1, *dbimg2, dbAfterimg1, dbAfterimg2, 1);	//线性插值
		hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）

		delete dbimg2;
		if (phase_exit2)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit2 = true;
			return 0;
		}
		//CZT变换
		CZT(dbAfterimg1, dbAfterimg2, dbCZTimg1, dbCZTimg2, A, W, 1);
		for (int i = 0; i < dbCZTimg1.Row; i++)
		{
			for (int j = 0; j < dbCZTimg1.Col; j++)
			{
				img1Phase.Data[i][j] = (dbCZTimg1.Data[i][j].arg());
				img2Phase.Data[i][j] = (dbCZTimg2.Data[i][j].arg());
			}
		}
		if (phase_exit2)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit2 = true;
			return 0;
		}
		double data1, data2;
		for (int j = 0; j < img1Phase.Row; j++)
		{
			for (int k = 0; k < img1Phase.Col; k++)
			{
				data1 = sin((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				data2 = cos((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				PhaseData2[index]->Data[j][k] = atan2(data1, data2);		//1038*694
			}
		}
		if (phase_exit2)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit2 = true;
			return 0;
		}
		p_PhaseDataVec2.push_back((PhaseData2[index]));
		phaseProcess++;
		CTipsDlg::getInstance()->update(phaseProcess);
	}

	delete ones;
	delete dbimg1;
	CMultiDlg::bRunning = false;
	is_exit2 = true;
	return 0;
}

UINT PhaseDataProcess3(LPVOID pParam)
{
	CMultiDlg::bRunning = true;

	FILE * fp_r1 = nullptr, *fp_r2 = nullptr, *fp_rbg = NULL;
	_wfopen_s(&fp_rbg, multiData.backGround, _T("rb"));
	Mymatrix<UINT16>* uint16Bgimg = new Mymatrix<UINT16>(fp_rbg, multiData.imgRow, multiData.imgCol);
	Mymatrix<double>dbBgimg(*uint16Bgimg);	//双精度背景图
	fclose(fp_rbg);
	delete uint16Bgimg;
	CString dataname1, dataname2;

	Mymatrix<double> *ones = new Mymatrix<double>(1, multiData.imgCol, 1.0);

	dataname1.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgStartNum);
	_wfopen_s(&fp_r1, dataname1, _T("rb"));
	Mymatrix<UINT16>*  uint16img1 = new Mymatrix<UINT16>(fp_r1, multiData.imgRow, multiData.imgCol);
	fclose(fp_r1);
	Mymatrix<double>*dbimg1 = new Mymatrix<double>(*uint16img1);	//双精度干涉图1
	delete uint16img1;
	*dbimg1 -= dbBgimg;
	Mymatrix<double>*tempmean1 = mean(*dbimg1);
	Mymatrix<double>*temp1 = matMultiply(*tempmean1, *ones);
	*dbimg1 -= *temp1;
	(*dbimg1).Fliplr();

	delete tempmean1;
	delete temp1;

	Mymatrix<UINT16>* uint16img2 = NULL;

	Mymatrix<double>*tempmean2 = NULL;

	Mymatrix<double>*temp2 = NULL;

	if (phase_exit3)
	{
		CMultiDlg::bRunning = false;
		delete ones;
		delete dbimg1;
		is_exit3 = true;
		return 0;
	}
	int start = 1; //细化频率段起点
	int end = multiData.freCount; //细化频率段终点
	int fs = multiData.imgCol;  //采样频率
	int M = multiData.freCount;	//细化频段的频点数   变换的长度
								//细化频段的跨度（步长）
	Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
	//细化频段的起始点  变换起始点
	Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));
	Mymatrix<double> dbAfterimg1(multiData.imgRow, multiData.imgCol);
	Mymatrix<double> dbAfterimg2(multiData.imgRow, multiData.imgCol);
	Mymatrix_i<double>dbCZTimg1(multiData.imgRow, M);
	Mymatrix_i<double>dbCZTimg2(multiData.imgRow, M);
	Mymatrix<double>img1Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double>img2Phase(multiData.imgRow, multiData.freCount);

	Mymatrix<double>*dbimg2 = NULL;

	int count = multiData.imgEndNum - multiData.imgStartNum+1;
	PhaseData3 = new Mymatrix<double>*[int(count / 4)];

	for (int i = 0; i<int(count / 4); i++)
	{
		PhaseData3[i] = new Mymatrix<double>(img1Phase.Row, img1Phase.Col);
	}
	for (int i = int(count / 4) * 2 + multiData.imgStartNum + 1, index = 0; i < int(count / 4) * 3 + multiData.imgStartNum + 1; i += multiData.stepLen, index++)
	{
		if (phase_exit3)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit3 = true;
			return 0;
		}

		dataname2.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i);
		_wfopen_s(&fp_r2, dataname2, _T("rb"));

		uint16img2 = new Mymatrix<UINT16>(fp_r2, multiData.imgRow, multiData.imgCol);

		fclose(fp_r2);

		dbimg2 = new Mymatrix<double>(*uint16img2);	//双精度干涉图2

		delete uint16img2;

		*dbimg2 -= dbBgimg;

		tempmean2 = mean(*dbimg2);

		temp2 = matMultiply(*tempmean2, *ones);

		*dbimg2 -= *temp2;

		delete tempmean2;

		delete temp2;

		(*dbimg2).Fliplr();

		dbAfterimg1 = *dbimg1;
		dbAfterimg2 = *dbimg2;
		if (phase_exit3)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			delete dbimg2;
			is_exit3 = true;
			return 0;
		}
		interpolation(*dbimg1, *dbimg2, dbAfterimg1, dbAfterimg2, 1);	//线性插值
		hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）

		delete dbimg2;
		if (phase_exit3)
		{
			CMultiDlg::bRunning = false;
			is_exit3 = true;
			return 0;
		}
		//CZT变换
		CZT(dbAfterimg1, dbAfterimg2, dbCZTimg1, dbCZTimg2, A, W, 1);
		if (phase_exit3)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit3 = true;
			return 0;
		}
		for (int i = 0; i < dbCZTimg1.Row; i++)
		{
			for (int j = 0; j < dbCZTimg1.Col; j++)
			{
				img1Phase.Data[i][j] = (dbCZTimg1.Data[i][j].arg());
				img2Phase.Data[i][j] = (dbCZTimg2.Data[i][j].arg());
			}
		}
		double data1, data2;
		for (int j = 0; j < img1Phase.Row; j++)
		{
			for (int k = 0; k < img1Phase.Col; k++)
			{
				data1 = sin((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				data2 = cos((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				PhaseData3[index]->Data[j][k] = atan2(data1, data2);		//1038*694
			}
		}
		if (phase_exit3)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit3 = true;
			return 0;
		}
		p_PhaseDataVec3.push_back((PhaseData3[index]));
		phaseProcess++;
		CTipsDlg::getInstance()->update(phaseProcess);
	}
	delete ones;
	delete dbimg1;
	CMultiDlg::bRunning = false;
	is_exit3 = true;
	return 0;
}

UINT PhaseDataProcess4(LPVOID pParam)
{
	CMultiDlg::bRunning = true;
	FILE * fp_r1 = nullptr, *fp_r2 = nullptr, *fp_rbg = NULL;
	_wfopen_s(&fp_rbg, multiData.backGround, _T("rb"));
	Mymatrix<UINT16>* uint16Bgimg = new Mymatrix<UINT16>(fp_rbg, multiData.imgRow, multiData.imgCol);
	Mymatrix<double>dbBgimg(*uint16Bgimg);	//双精度背景图
	fclose(fp_rbg);
	delete uint16Bgimg;
	CString dataname1, dataname2;

	Mymatrix<double> *ones = new Mymatrix<double>(1, multiData.imgCol, 1.0);

	dataname1.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, multiData.imgStartNum);
	_wfopen_s(&fp_r1, dataname1, _T("rb"));
	Mymatrix<UINT16>*  uint16img1 = new Mymatrix<UINT16>(fp_r1, multiData.imgRow, multiData.imgCol);
	fclose(fp_r1);
	Mymatrix<double>*dbimg1 = new Mymatrix<double>(*uint16img1);	//双精度干涉图1
	delete uint16img1;
	*dbimg1 -= dbBgimg;
	Mymatrix<double>*tempmean1 = mean(*dbimg1);
	Mymatrix<double>*temp1 = matMultiply(*tempmean1, *ones);
	*dbimg1 -= *temp1;
	(*dbimg1).Fliplr();

	delete tempmean1;
	delete temp1;

	Mymatrix<UINT16>* uint16img2 = NULL;

	Mymatrix<double>*tempmean2 = NULL;

	Mymatrix<double>*temp2 = NULL;

	if (phase_exit4)
	{
		CMultiDlg::bRunning = false;
		delete ones;
		delete dbimg1;
		is_exit4 = true;
		return 0;
	}

	int start = 1; //细化频率段起点
	int end = multiData.freCount; //细化频率段终点
	int fs = multiData.imgCol;  //采样频率
	int M = multiData.freCount;	//细化频段的频点数   变换的长度
								//细化频段的跨度（步长）
	Complex<double>W(cos(PI2*(end - start) / (fs*M)), -1.0*sin(PI2*(end - start) / (fs*M)));
	//细化频段的起始点  变换起始点
	Complex<double>A(cos(PI2*start / (fs)), sin(PI2*start / (fs)));
	Mymatrix<double> dbAfterimg1(multiData.imgRow, multiData.imgCol);
	Mymatrix<double> dbAfterimg2(multiData.imgRow, multiData.imgCol);
	Mymatrix_i<double>dbCZTimg1(multiData.imgRow, M);
	Mymatrix_i<double>dbCZTimg2(multiData.imgRow, M);
	Mymatrix<double>img1Phase(multiData.imgRow, multiData.freCount);
	Mymatrix<double>img2Phase(multiData.imgRow, multiData.freCount);

	Mymatrix<double>*dbimg2 = NULL;
	int count = multiData.imgEndNum - multiData.imgStartNum+1;
	PhaseData4 = new Mymatrix<double>*[int(count / 4) + count % 4];

	for (int i = 0; i<int(count / 4) + count % 4; i++)
	{
		PhaseData4[i] = new Mymatrix<double>(img1Phase.Row, img1Phase.Col);
	}

	for (int i = int(count / 4) * 3 + multiData.imgStartNum + 1, index = 0; i <= multiData.imgEndNum; i += multiData.stepLen, index++)
	{
		if (phase_exit4)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit4 = true;
			return 0;
		}

		dataname2.Format(_T("%s\\%s%d.bin"), folder_dir, filename_prefix, i);
		_wfopen_s(&fp_r2, dataname2, _T("rb"));

		uint16img2 = new Mymatrix<UINT16>(fp_r2, multiData.imgRow, multiData.imgCol);

		fclose(fp_r2);

		dbimg2 = new Mymatrix<double>(*uint16img2);	//双精度干涉图2

		delete uint16img2;

		*dbimg2 -= dbBgimg;

		tempmean2 = mean(*dbimg2);

		temp2 = matMultiply(*tempmean2, *ones);

		*dbimg2 -= *temp2;

		delete tempmean2;

		delete temp2;

		(*dbimg2).Fliplr();

		dbAfterimg1 = *dbimg1;

		dbAfterimg2 = *dbimg2;
		if (phase_exit4)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			delete dbimg2;
			is_exit4 = true;
			return 0;
		}
		interpolation(*dbimg1, *dbimg2, dbAfterimg1, dbAfterimg2, 1);	//线性插值
		hanning(dbAfterimg1, dbAfterimg2);		//对列加窗（hanning）
		if (phase_exit4)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			delete dbimg2;
			is_exit4 = true;
			return 0;
		}
		delete dbimg2;
		//CZT变换
		CZT(dbAfterimg1, dbAfterimg2, dbCZTimg1, dbCZTimg2, A, W, 1);
		if (phase_exit4)
		{
			CMultiDlg::bRunning = false;
			is_exit4 = true;
			return 0;
		}
		//相位
		for (int i = 0; i < dbCZTimg1.Row; i++)
		{
			for (int j = 0; j < dbCZTimg1.Col; j++)
			{
				img1Phase.Data[i][j] = (dbCZTimg1.Data[i][j].arg());
				img2Phase.Data[i][j] = (dbCZTimg2.Data[i][j].arg());
			}
		}
		if (phase_exit4)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit4 = true;
			return 0;
		}
		double data1, data2;
		for (int j = 0; j < img1Phase.Row; j++)
		{
			for (int k = 0; k < img1Phase.Col; k++)
			{
				data1 = sin((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				data2 = cos((img2Phase.Data[j][k] - img1Phase.Data[j][k]));
				PhaseData4[index]->Data[j][k] = atan2(data1, data2);		//1038*694
			}
		}
		if (phase_exit4)
		{
			CMultiDlg::bRunning = false;
			delete ones;
			delete dbimg1;
			is_exit4 = true;
			return 0;
		}
		p_PhaseDataVec4.push_back((PhaseData4[index]));
		phaseProcess++;
		CTipsDlg::getInstance()->update(phaseProcess);
	}
	delete ones;
	delete dbimg1;
	CMultiDlg::bRunning = false;
	is_exit4 = true;
	return 0;
}






UINT Wrap(LPVOID pParam)
{
	is_Wrap = true;
	CMultiDlg::bRunning = true;
#pragma region 保存相位

	FILE *f_w;
	CString path;
	path.Format(_T("%s\\%d-%d,%d,%d,%d,%d-%d,%d-%d Wrap.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
		multiData.freCount, multiData.midWaveLen, multiData.bandWidth, area.startRow, area.endRow,
		area.startCol, area.endCol);

	/*path.Format(_T("Data\\%d-%d,%d,%d,%d,%d-%d,%g-%g Wrap.bin"), multiData.imgStartNum, multiData.imgEndNum,
	multiData.freCount, multiData.midWaveLen, multiData.bandWidth, area.startCol, area.endCol, multiData.ampMin, multiData.ampMax);*/
	_wfopen_s(&f_w, path, _T("wb"));
	for (int i = 0; i < p_PhaseDataVec1.size(); i++)
	{
		for (int j = 0; j < p_PhaseDataVec1[i]->Row; j++)
		{
			fwrite(p_PhaseDataVec1[i]->Data[j], sizeof(double), p_PhaseDataVec1[i]->Col, f_w);
		}
	}
	for (int i = 0; i < p_PhaseDataVec2.size(); i++)
	{
		for (int j = 0; j < p_PhaseDataVec2[i]->Row; j++)
		{
			fwrite(p_PhaseDataVec2[i]->Data[j], sizeof(double), p_PhaseDataVec2[i]->Col, f_w);
		}
	}
	for (int i = 0; i < p_PhaseDataVec3.size(); i++)
	{
		for (int j = 0; j < p_PhaseDataVec3[i]->Row; j++)
		{
			fwrite(p_PhaseDataVec3[i]->Data[j], sizeof(double), p_PhaseDataVec3[i]->Col, f_w);
		}
	}
	for (int i = 0; i < p_PhaseDataVec4.size(); i++)
	{
		for (int j = 0; j < p_PhaseDataVec4[i]->Row; j++)
		{
			fwrite(p_PhaseDataVec4[i]->Data[j], sizeof(double), p_PhaseDataVec4[i]->Col, f_w);
		}
	}
	fclose(f_w);

	path.Format(_T("%s\\%d-%d,%d,%d-%d Amp.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
		multiData.freCount, area.startCol, area.endCol);
	if (!DataCheck(path))
	{
		_wfopen_s(&f_w, path, _T("wb"));
		for (int j = 0; j < Amp->Row; j++)
		{
			fwrite(Amp->Data[j], sizeof(double), Amp->Col, f_w);
		}
		fclose(f_w);
	}
#pragma endregion
	if (phase_exit)
	{
		is_Wrap = false;
		return 0;
	}
#pragma region 幅值处理


	//Mymatrix<double>*Am = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth() / 2, *Amp);
	Mymatrix<double>*Am0 = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth(), *Amp);


	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
	{
		for (int j = 0; j < Am0->Row; j++)
		{
			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
		}
	}
	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
	{
		for (int j = 0; j < Am0->Row; j++)
		{
			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
		}
	}
	Mymatrix<double>*Am = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col / 2, *Am0);
	//medFilt2(*Am);

	/************** 归一化 *****************/
	//找出am的最大最小值
	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] > maxAm0)
			{
				maxAm0 = Am0->Data[i][j];
			}
			else if (Am0->Data[i][j] < minAm0)
			{
				minAm0 = Am0->Data[i][j];
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
		}
	}
	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] > maxAm)
			{
				maxAm = Am->Data[i][j];
			}
			else if (Am->Data[i][j] < minAm)
			{
				minAm = Am->Data[i][j];
			}
		}
	}
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
		}
	}

	/********************** 幅值处理 ************************/
	double max_A = multiData.ampMax;
	double min_A = multiData.ampMin;
	double A = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
	double B = -1.5*(max_A + min_A)*A;
	double C = -3.0*A*(pow(max_A, 2)) - 2 * B*max_A;
	double D = 1.0 - A*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;

	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] >= max_A)
			{
				Am->Data[i][j] = 1.0;
			}
			else if (Am->Data[i][j] <= min_A)
			{
				Am->Data[i][j] = 0;
			}
			else
			{
				Am->Data[i][j] = A*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] >= max_A)
			{
				Am0->Data[i][j] = 1.0;
			}
			else if (Am0->Data[i][j] <= min_A)
			{
				Am0->Data[i][j] = 0;
			}
			else
			{
				Am0->Data[i][j] = A*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
			}
		}
	}
	//Am->Flipud();
	//Am0->Flipud();
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] >= 1)
			{
				Am->Data[i][j] = 1;
			}
			else if (Am->Data[i][j] <= 0)
			{
				Am->Data[i][j] = 0;
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] >= 1)
			{
				Am0->Data[i][j] = 1;
			}
			else if (Am0->Data[i][j] <= 0)
			{
				Am0->Data[i][j] = 0;
			}
		}
	}
#pragma endregion
	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i += multiData.stepLen)
	{
		if (phase_exit)
		{
			CMultiDlg::bRunning = false;
			delete Am;
			delete Am0;
			is_Wrap = false;
			return 0;
		}

		
			if (i < p_PhaseDataVec1.size())
			{
				Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
				Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);

				HSI_Disp(p_PhaseDataVec1[i], temp1, temp2, 2);

				delete temp1;
				delete temp2;
				
			}
			else if ((p_PhaseDataVec1.size() <= i) && (i < (p_PhaseDataVec1.size() + p_PhaseDataVec2.size())))
			{
				Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
				Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);

				HSI_Disp(p_PhaseDataVec2[i- p_PhaseDataVec1.size()], temp1, temp2, 2);

				delete temp1;
				delete temp2;
			}
			else if ((p_PhaseDataVec1.size() + p_PhaseDataVec2.size() <= i) && (i < p_PhaseDataVec1.size() + p_PhaseDataVec2.size() + p_PhaseDataVec3.size()))
			{
				Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
				Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);

				HSI_Disp(p_PhaseDataVec3[i- p_PhaseDataVec1.size() - p_PhaseDataVec2.size()], temp1, temp2, 2);

				delete temp1;
				delete temp2;
			}
			else
			{
				Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
				Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);

				HSI_Disp(p_PhaseDataVec4[i- p_PhaseDataVec1.size() - p_PhaseDataVec2.size()- p_PhaseDataVec3.size()], temp1, temp2, 2);

				delete temp1;
				delete temp2;
			}
		
			phaseProcess++;
			CTipsDlg::getInstance()->update(phaseProcess);

	}

	if (phase_exit)
	{
		CMultiDlg::bRunning = false;
		is_Wrap = false;
		delete Am;
		delete Am0;
		return 0;
	}
	CTipsDlg::getInstance()->destroyInstance();

	AfxBeginThread(showPhaseColorThead, NULL);
	delete Am;
	delete Am0;
	CMultiDlg::bRunning = false;
	is_Wrap = false;
	return 0;
}



UINT unWrap(LPVOID pParam)
{
	CMultiDlg::bRunning = true;
	is_Unwrap = true;
	if (unphase_exit)
	{
		CMultiDlg::bRunning = false;
		is_Unwrap = false;
		return 0;
	}
#pragma region 保存相位
	/*for (int i = 0; i < p_UnPhaseDataVec1.size(); i++)
	{
		for (int j = 0; j < p_UnPhaseDataVec1[i]->Row; j++)
		{
			for (int k = 0; k < p_UnPhaseDataVec1[i]->Col; k++)
			{

				p_UnPhaseDataVec1[i]->Data[j][k]/=PI2;
			}
		}
	}
	for (int i = 0; i < p_UnPhaseDataVec2.size(); i++)
	{
		for (int j = 0; j < p_UnPhaseDataVec2[i]->Row; j++)
		{
			for (int k = 0; k < p_UnPhaseDataVec2[i]->Col; k++)
			{

				p_UnPhaseDataVec2[i]->Data[j][k] /= PI2;
			}
		}
	}
	for (int i = 0; i < p_UnPhaseDataVec3.size(); i++)
	{
		for (int j = 0; j < p_UnPhaseDataVec3[i]->Row; j++)
		{
			for (int k = 0; k < p_UnPhaseDataVec3[i]->Col; k++)
			{

				p_UnPhaseDataVec3[i]->Data[j][k] /= PI2;
			}
		}
	}
	for (int i = 0; i < p_UnPhaseDataVec4.size(); i++)
	{
		for (int j = 0; j < p_UnPhaseDataVec4[i]->Row; j++)
		{
			for (int k = 0; k < p_UnPhaseDataVec4[i]->Col; k++)
			{

				p_UnPhaseDataVec4[i]->Data[j][k] /= PI2;
			}
		}
	}*/


	FILE *f_w;
	CString path;
	path.Format(_T("%s\\%d-%d,%d,%d,%d,%d-%d,%d-%d unWrap.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
		multiData.freCount, multiData.midWaveLen, multiData.bandWidth, area.startRow, area.endRow,
		area.startCol, area.endCol);
	_wfopen_s(&f_w, path, _T("wb"));
	for (int i = 0; i < p_UnPhaseDataVec1.size(); i++)
	{
		for (int j = 0; j < p_UnPhaseDataVec1[i]->Row; j++)
		{
			fwrite(p_UnPhaseDataVec1[i]->Data[j], sizeof(double), p_UnPhaseDataVec1[i]->Col, f_w);
		}
	}
	for (int i = 0; i < p_UnPhaseDataVec2.size(); i++)
	{
		for (int j = 0; j < p_UnPhaseDataVec2[i]->Row; j++)
		{
			fwrite(p_UnPhaseDataVec2[i]->Data[j], sizeof(double), p_UnPhaseDataVec2[i]->Col, f_w);
		}
	}
	for (int i = 0; i < p_UnPhaseDataVec3.size(); i++)
	{
		for (int j = 0; j < p_UnPhaseDataVec3[i]->Row; j++)
		{
			fwrite(p_UnPhaseDataVec3[i]->Data[j], sizeof(double), p_UnPhaseDataVec3[i]->Col, f_w);
		}
	}
	for (int i = 0; i < p_UnPhaseDataVec4.size(); i++)
	{
		for (int j = 0; j < p_UnPhaseDataVec4[i]->Row; j++)
		{
			fwrite(p_UnPhaseDataVec4[i]->Data[j], sizeof(double), p_UnPhaseDataVec4[i]->Col, f_w);
		}
	}
	fclose(f_w);
	path.Format(_T("%s\\%d-%d,%d,%d-%d Amp.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
		multiData.freCount, area.startCol, area.endCol);
	if (!DataCheck(path))
	{
		_wfopen_s(&f_w, path, _T("wb"));
		for (int j = 0; j < Amp->Row; j++)
		{
			fwrite(Amp->Data[j], sizeof(double), Amp->Col, f_w);
		}
		fclose(f_w);
	}
#pragma endregion
#pragma region 拷贝数据

	//拷贝一份数据
		Mymatrix<double>**tempdata = new Mymatrix<double>*[multiData.imgEndNum - multiData.imgStartNum];
	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
	{
		if (i < p_UnPhaseDataVec1.size())
		{
			tempdata[i] = new Mymatrix<double>(0, 0, p_UnPhaseDataVec1[i]->Row, p_UnPhaseDataVec1[i]->Col, *p_UnPhaseDataVec1[i]);
		}
		else if ((p_UnPhaseDataVec1.size() <= i) && (i < (p_UnPhaseDataVec1.size() + p_UnPhaseDataVec2.size())))
		{
			tempdata[i] = new Mymatrix<double>(0, 0, p_UnPhaseDataVec2[i - p_UnPhaseDataVec1.size()]->Row,
				p_UnPhaseDataVec2[i - p_UnPhaseDataVec1.size()]->Col, *p_UnPhaseDataVec2[i - p_UnPhaseDataVec1.size()]);
		}
		else if ((p_UnPhaseDataVec1.size() + p_UnPhaseDataVec2.size() <= i) && (i < p_UnPhaseDataVec1.size() + p_UnPhaseDataVec2.size() + p_UnPhaseDataVec3.size()))
		{
			tempdata[i] = new Mymatrix<double>(0, 0, p_UnPhaseDataVec3[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size()]->Row,
				p_UnPhaseDataVec3[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size()]->Col,
				*p_UnPhaseDataVec3[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size()]);
		}
		else
		{
			tempdata[i] = new Mymatrix<double>(0, 0, p_UnPhaseDataVec4[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size() - p_UnPhaseDataVec3.size()]->Row,
				p_UnPhaseDataVec4[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size() - p_UnPhaseDataVec3.size()]->Col,
				*p_UnPhaseDataVec4[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size() - p_UnPhaseDataVec3.size()]);
		}
	}
	/*multiData.unwrapMax = tempdata[0]->Data[0][0] / PI2;
	multiData.unwrapMin = tempdata[0]->Data[0][0] / PI2;

	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
	{
		for (int j = 0; j < tempdata[i]->Row; j++)
		{
			for (int k = 0; k < tempdata[i]->Col; k++)
			{
				tempdata[i]->Data[j][k] = tempdata[i]->Data[j][k] / PI2;
				if (multiData.unwrapMax < tempdata[i]->Data[j][k])
				{
					multiData.unwrapMax = tempdata[i]->Data[j][k];
				}
				else if (multiData.unwrapMin > tempdata[i]->Data[j][k])
				{
					multiData.unwrapMin = tempdata[i]->Data[j][k];
				}
			}
		}
	}*/
#pragma endregion
	if (unphase_exit)
	{
		CMultiDlg::bRunning = false;
		if (tempdata != NULL)
		{
			for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
			{
				delete tempdata[i];
			}
			delete[]tempdata;
		}
		is_Unwrap = false;
		return 0;
	}
#pragma region 幅值处理
	
	Mymatrix<double>*Am0 = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth(), *Amp);
	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
	{
		for (int j = 0; j < Am0->Row; j++)
		{
			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
		}
	}
	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
	{
		for (int j = 0; j < Am0->Row; j++)
		{
			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
		}
	}
	Mymatrix<double>*Am = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col / 2, *Am0);
	//medFilt2(*Am);
	//找出am的最大最小值
	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] > maxAm0)
			{
				maxAm0 = Am0->Data[i][j];
			}
			else if (Am0->Data[i][j] < minAm0)
			{
				minAm0 = Am0->Data[i][j];
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
		}
	}
	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] > maxAm)
			{
				maxAm = Am->Data[i][j];
			}
			else if (Am->Data[i][j] < minAm)
			{
				minAm = Am->Data[i][j];
			}
		}
	}
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
		}
	}
	double max_A = multiData.ampMax;
	double min_A = multiData.ampMin;
	double A = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
	double B = -1.5*(max_A + min_A)*A;
	double C = -3.0*A*(pow(max_A, 2)) - 2 * B*max_A;
	double D = 1.0 - A*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;

	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] >= max_A)
			{
				Am->Data[i][j] = 1.0;
			}
			else if (Am->Data[i][j] <= min_A)
			{
				Am->Data[i][j] = 0;
			}
			else
			{
				Am->Data[i][j] = A*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] >= max_A)
			{
				Am0->Data[i][j] = 1.0;
			}
			else if (Am0->Data[i][j] <= min_A)
			{
				Am0->Data[i][j] = 0;
			}
			else
			{
				Am0->Data[i][j] = A*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
			}
		}
	}
	//Am->Flipud();
	//Am0->Flipud();
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] >= 1)
			{
				Am->Data[i][j] = 1;
			}
			else if (Am->Data[i][j] <= 0)
			{
				Am->Data[i][j] = 0;
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] >= 1)
			{
				Am0->Data[i][j] = 1;
			}
			else if (Am0->Data[i][j] <= 0)
			{
				Am0->Data[i][j] = 0;
			}
		}
	}
#pragma endregion
	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
	{
		if (unphase_exit)
		{
			CMultiDlg::bRunning = false;
			if (tempdata != NULL)
			{
				for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
				{
					delete tempdata[i];
				}
				delete[]tempdata;

			}
			is_Unwrap = false;
			delete Am;
			delete Am0;
			return 0;
		}
		

		HSI_Disp(tempdata[i], Am, Am0, 1);


		unphaseProcess++;
		CTipsDlg::getInstance()->update(unphaseProcess);
	}
	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
	{
		delete tempdata[i];
	}
	delete[]tempdata;
	if (unphase_exit)
	{
		CMultiDlg::bRunning = false;
		is_Unwrap = false;
		delete Am;
		delete Am0;
		return 0;
	}

	delete Am;
	delete Am0;

	CMultiDlg::bRunning = false;
	is_Unwrap = false;
	return 0;
}


UINT directWrap(LPVOID pParam)
{
	CMultiDlg::bRunning = true;

#pragma region 拷贝数据

	//拷贝一份数据
	Mymatrix<double>**tempdata = new Mymatrix<double>*[multiData.imgEndNum - multiData.imgStartNum];
	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
	{
		tempdata[i] = new Mymatrix<double>(0, 0, p_directPhaseDataVec[i]->Row, p_directPhaseDataVec[i]->Col, *p_directPhaseDataVec[i]);
	}

#pragma endregion
	if (phase_exit)
	{
		CMultiDlg::bRunning = false;
		if (tempdata != NULL)
		{
			for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
			{
				delete tempdata[i];
			}
			delete[]tempdata;
		}
		is_Wrap = false;
		return 0;
	}
#pragma region 幅值处理
	//Mymatrix<double>*Am = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth() / 2, *Amp);

	Mymatrix<double>*Am0 = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth(), *Amp);
	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
	{
		for (int j = 0; j < Am0->Row; j++)
		{
			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
		}
	}
	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
	{
		for (int j = 0; j < Am0->Row; j++)
		{
			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
		}
	}
	Mymatrix<double>*Am = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col / 2, *Am0);
	//medFilt2(*Am);

	/************** 归一化 *****************/
	//找出am的最大最小值
	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] > maxAm0)
			{
				maxAm0 = Am0->Data[i][j];
			}
			else if (Am0->Data[i][j] < minAm0)
			{
				minAm0 = Am0->Data[i][j];
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
		}
	}
	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] > maxAm)
			{
				maxAm = Am->Data[i][j];
			}
			else if (Am->Data[i][j] < minAm)
			{
				minAm = Am->Data[i][j];
			}
		}
	}
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
		}
	}

	/********************** 幅值处理 ************************/
	double max_A = multiData.ampMax;
	double min_A = multiData.ampMin;
	double A = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
	double B = -1.5*(max_A + min_A)*A;
	double C = -3.0*A*(pow(max_A, 2)) - 2 * B*max_A;
	double D = 1.0 - A*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;

	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] >= max_A)
			{
				Am->Data[i][j] = 1.0;
			}
			else if (Am->Data[i][j] <= min_A)
			{
				Am->Data[i][j] = 0;
			}
			else
			{
				Am->Data[i][j] = A*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] >= max_A)
			{
				Am0->Data[i][j] = 1.0;
			}
			else if (Am0->Data[i][j] <= min_A)
			{
				Am0->Data[i][j] = 0;
			}
			else
			{
				Am0->Data[i][j] = A*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
			}
		}
	}
	//Am->Flipud();
	//Am0->Flipud();
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] >= 1)
			{
				Am->Data[i][j] = 1;
			}
			else if (Am->Data[i][j] <= 0)
			{
				Am->Data[i][j] = 0;
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] >= 1)
			{
				Am0->Data[i][j] = 1;
			}
			else if (Am0->Data[i][j] <= 0)
			{
				Am0->Data[i][j] = 0;
			}
		}
	}
#pragma endregion
	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i += multiData.stepLen)
	{
		if (phase_exit)
		{
			if (tempdata != NULL)
			{
				for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
				{
					delete tempdata[i];
				}
				delete[]tempdata;
			}
			is_Wrap = false;
			delete Am;
			delete Am0;
			return 0;
		}
	
		HSI_Disp(tempdata[i], Am, Am0, 2);


		phaseProcess++;
		CTipsDlg::getInstance()->update(phaseProcess);
	}
	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
	{
		delete tempdata[i];
	}
	delete[]tempdata;
	if (phase_exit)
	{
		CMultiDlg::bRunning = false;
		is_Wrap = false;
		delete Am;
		delete Am0;
		return 0;
	}
	CTipsDlg::getInstance()->destroyInstance();
	AfxBeginThread(showPhaseColorThead, NULL);
	delete Am;
	delete Am0;
	CMultiDlg::bRunning = false;
	return 0;
}

//UINT directUnWrap(LPVOID pParam)
//{
//	CMultiDlg::bRunning = true;
//#pragma region 拷贝数据
//
//	//拷贝一份数据
//	Mymatrix<double>**tempdata = new Mymatrix<double>*[multiData.imgEndNum - multiData.imgStartNum];
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		tempdata[i] = new Mymatrix<double>(0, 0, p_directUnPhaseDataVec[i]->Row, p_directUnPhaseDataVec[i]->Col, *p_directUnPhaseDataVec[i]);
//	}
//	multiData.unwrapMax = tempdata[0]->Data[0][0] / PI2;
//	multiData.unwrapMin = tempdata[0]->Data[0][0]  / PI2;
//
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		for (int j = 0; j < tempdata[i]->Row; j++)
//		{
//			for (int k = 0; k < tempdata[i]->Col; k++)
//			{
//				tempdata[i]->Data[j][k] = tempdata[i]->Data[j][k]  / PI2;
//
//				if (multiData.unwrapMax < tempdata[i]->Data[j][k])
//				{
//					multiData.unwrapMax = tempdata[i]->Data[j][k];
//				}
//				else if (multiData.unwrapMin > tempdata[i]->Data[j][k])
//				{
//					multiData.unwrapMin = tempdata[i]->Data[j][k];
//				}
//			}
//		}
//	}
//#pragma endregion
//	if (unphase_exit)
//	{
//		CMultiDlg::bRunning = false;
//		if (tempdata != NULL)
//		{
//			for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//			{
//				delete tempdata[i];
//			}
//			delete[]tempdata;
//		}
//		is_Unwrap = false;
//		return 0;
//	}
//
//#pragma region 幅值处理
//	Mymatrix<double>*Am0 = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth(), *Amp);
//	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
//		}
//	}
//	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
//		}
//	}
//	Mymatrix<double>*Am = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col / 2, *Am0);
//	//medFilt2(*Am);
//
//	/************** 归一化 *****************/
//	//找出am的最大最小值
//	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] > maxAm0)
//			{
//				maxAm0 = Am0->Data[i][j];
//			}
//			else if (Am0->Data[i][j] < minAm0)
//			{
//				minAm0 = Am0->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
//		}
//	}
//	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] > maxAm)
//			{
//				maxAm = Am->Data[i][j];
//			}
//			else if (Am->Data[i][j] < minAm)
//			{
//				minAm = Am->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
//		}
//	}
//
//	/********************** 幅值处理 ************************/
//	double max_A = multiData.ampMax;
//	double min_A = multiData.ampMin;
//	double A = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
//	double B = -1.5*(max_A + min_A)*A;
//	double C = -3.0*A*(pow(max_A, 2)) - 2 * B*max_A;
//	double D = 1.0 - A*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;
//
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] >= max_A)
//			{
//				Am->Data[i][j] = 1.0;
//			}
//			else if (Am->Data[i][j] <= min_A)
//			{
//				Am->Data[i][j] = 0;
//			}
//			else
//			{
//				Am->Data[i][j] = A*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] >= max_A)
//			{
//				Am0->Data[i][j] = 1.0;
//			}
//			else if (Am0->Data[i][j] <= min_A)
//			{
//				Am0->Data[i][j] = 0;
//			}
//			else
//			{
//				Am0->Data[i][j] = A*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
//			}
//		}
//	}
//	//Am->Flipud();
//	//Am0->Flipud();
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] >= 1)
//			{
//				Am->Data[i][j] = 1;
//			}
//			else if (Am->Data[i][j] <= 0)
//			{
//				Am->Data[i][j] = 0;
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] >= 1)
//			{
//				Am0->Data[i][j] = 1;
//			}
//			else if (Am0->Data[i][j] <= 0)
//			{
//				Am0->Data[i][j] = 0;
//			}
//		}
//	}
//#pragma endregion
//
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		if (unphase_exit)
//		{
//			CMultiDlg::bRunning = false;
//			if (tempdata != NULL)
//			{
//				for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//				{
//					delete tempdata[i];
//				}
//				delete[]tempdata;
//			}
//			is_Unwrap = false;
//			delete Am;
//			delete Am0;
//			return 0;
//		}
//
//		HSI_Disp(tempdata[i], Am, Am0, 1);
//
//		unphaseProcess++;
//		CTipsDlg::getInstance()->update(unphaseProcess);
//	}
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		delete tempdata[i];
//	}
//	delete[]tempdata;
//	if (unphase_exit)
//	{
//		CMultiDlg::bRunning = false;
//		is_Unwrap = false;
//		delete Am;
//		delete Am0;
//		return 0;
//	}
//	
//	CMultiDlg::bRunning = false;
//	delete Am;
//	delete Am0;
//	return 0;
//}

UINT directUnWrap(LPVOID pParam)
{
	CMultiDlg::bRunning = true;
	if (unphase_exit)
	{
		CMultiDlg::bRunning = false;
		is_Unwrap = false;
		return 0;
	}

#pragma region 幅值处理
	Mymatrix<double>*Am0 = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth(), *Amp);
	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
	{
		for (int j = 0; j < Am0->Row; j++)
		{
			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
		}
	}
	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
	{
		for (int j = 0; j < Am0->Row; j++)
		{
			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
		}
	}
	Mymatrix<double>*Am = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col / 2, *Am0);
	//medFilt2(*Am);

	/************** 归一化 *****************/
	//找出am的最大最小值
	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] > maxAm0)
			{
				maxAm0 = Am0->Data[i][j];
			}
			else if (Am0->Data[i][j] < minAm0)
			{
				minAm0 = Am0->Data[i][j];
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
		}
	}
	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] > maxAm)
			{
				maxAm = Am->Data[i][j];
			}
			else if (Am->Data[i][j] < minAm)
			{
				minAm = Am->Data[i][j];
			}
		}
	}
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
		}
	}

	/********************** 幅值处理 ************************/
	double max_A = multiData.ampMax;
	double min_A = multiData.ampMin;
	double A = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
	double B = -1.5*(max_A + min_A)*A;
	double C = -3.0*A*(pow(max_A, 2)) - 2 * B*max_A;
	double D = 1.0 - A*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;

	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] >= max_A)
			{
				Am->Data[i][j] = 1.0;
			}
			else if (Am->Data[i][j] <= min_A)
			{
				Am->Data[i][j] = 0;
			}
			else
			{
				Am->Data[i][j] = A*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] >= max_A)
			{
				Am0->Data[i][j] = 1.0;
			}
			else if (Am0->Data[i][j] <= min_A)
			{
				Am0->Data[i][j] = 0;
			}
			else
			{
				Am0->Data[i][j] = A*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
			}
		}
	}
	//Am->Flipud();
	//Am0->Flipud();
	for (int i = 0; i < Am->Row; i++)
	{
		for (int j = 0; j < Am->Col; j++)
		{
			if (Am->Data[i][j] >= 1)
			{
				Am->Data[i][j] = 1;
			}
			else if (Am->Data[i][j] <= 0)
			{
				Am->Data[i][j] = 0;
			}
		}
	}
	for (int i = 0; i < Am0->Row; i++)
	{
		for (int j = 0; j < Am0->Col; j++)
		{
			if (Am0->Data[i][j] >= 1)
			{
				Am0->Data[i][j] = 1;
			}
			else if (Am0->Data[i][j] <= 0)
			{
				Am0->Data[i][j] = 0;
			}
		}
	}
#pragma endregion

	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
	{
		if (unphase_exit)
		{
			CMultiDlg::bRunning = false;
			is_Unwrap = false;
			delete Am;
			delete Am0;
			return 0;
		}

		HSI_Disp(p_directUnPhaseDataVec[i], Am, Am0, 1);

		unphaseProcess++;
		CTipsDlg::getInstance()->update(unphaseProcess);
	}
	
	if (unphase_exit)
	{
		CMultiDlg::bRunning = false;
		is_Unwrap = false;
		delete Am;
		delete Am0;
		return 0;
	}

	CMultiDlg::bRunning = false;
	delete Am;
	delete Am0;
	return 0;
}



//UINT Wrap(LPVOID pParam)
//{
//	is_Wrap = true;
//	CMultiDlg::bRunning = true;
//#pragma region 保存相位
//
//	FILE *f_w;
//	CString path;
//	path.Format(_T("%s\\%d-%d,%d,%d,%d,%d-%d,%d-%d Wrap.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
//		multiData.freCount, multiData.midWaveLen, multiData.bandWidth, area.startRow, area.endRow,
//		area.startCol, area.endCol);
//
//	/*path.Format(_T("Data\\%d-%d,%d,%d,%d,%d-%d,%g-%g Wrap.bin"), multiData.imgStartNum, multiData.imgEndNum,
//	multiData.freCount, multiData.midWaveLen, multiData.bandWidth, area.startCol, area.endCol, multiData.ampMin, multiData.ampMax);*/
//	_wfopen_s(&f_w, path, _T("wb"));
//	for (int i = 0; i < p_PhaseDataVec1.size(); i++)
//	{
//		for (int j = 0; j < p_PhaseDataVec1[i]->Row; j++)
//		{
//			fwrite(p_PhaseDataVec1[i]->Data[j], sizeof(double), p_PhaseDataVec1[i]->Col, f_w);
//		}
//	}
//	for (int i = 0; i < p_PhaseDataVec2.size(); i++)
//	{
//		for (int j = 0; j < p_PhaseDataVec2[i]->Row; j++)
//		{
//			fwrite(p_PhaseDataVec2[i]->Data[j], sizeof(double), p_PhaseDataVec2[i]->Col, f_w);
//		}
//	}
//	for (int i = 0; i < p_PhaseDataVec3.size(); i++)
//	{
//		for (int j = 0; j < p_PhaseDataVec3[i]->Row; j++)
//		{
//			fwrite(p_PhaseDataVec3[i]->Data[j], sizeof(double), p_PhaseDataVec3[i]->Col, f_w);
//		}
//	}
//	for (int i = 0; i < p_PhaseDataVec4.size(); i++)
//	{
//		for (int j = 0; j < p_PhaseDataVec4[i]->Row; j++)
//		{
//			fwrite(p_PhaseDataVec4[i]->Data[j], sizeof(double), p_PhaseDataVec4[i]->Col, f_w);
//		}
//	}
//	fclose(f_w);
//
//	path.Format(_T("%s\\%d-%d,%d,%d-%d Amp.bin"), folder_dir, multiData.imgStartNum, multiData.imgEndNum,
//		multiData.freCount, area.startCol, area.endCol);
//	if (!DataCheck(path))
//	{
//		_wfopen_s(&f_w, path, _T("wb"));
//		for (int j = 0; j < Amp->Row; j++)
//		{
//			fwrite(Amp->Data[j], sizeof(double), Amp->Col, f_w);
//		}
//		fclose(f_w);
//	}
//#pragma endregion
//	 #pragma region 拷贝数据
//	 
//	 	//拷贝一份数据
//	 	Mymatrix<double>**tempdata = new Mymatrix<double>*[multiData.imgEndNum - multiData.imgStartNum];
//	 	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	 	{
//	 		if (i < p_PhaseDataVec1.size())
//	 		{
//	 			tempdata[i] = new Mymatrix<double>(0, 0, p_PhaseDataVec1[i]->Row, p_PhaseDataVec1[i]->Col, *p_PhaseDataVec1[i]);
//	 		}
//	 		else if ((p_PhaseDataVec1.size() <= i) && (i < (p_PhaseDataVec1.size() + p_PhaseDataVec2.size())))
//	 		{
//	 			tempdata[i] = new Mymatrix<double>(0, 0, p_PhaseDataVec2[i - p_PhaseDataVec1.size()]->Row,
//	 				p_PhaseDataVec2[i - p_PhaseDataVec1.size()]->Col, *p_PhaseDataVec2[i - p_PhaseDataVec1.size()]);
//	 		}
//	 		else if ((p_PhaseDataVec1.size() + p_PhaseDataVec2.size() <= i) && (i < p_PhaseDataVec1.size() + p_PhaseDataVec2.size() + p_PhaseDataVec3.size()))
//	 		{
//	 			tempdata[i] = new Mymatrix<double>(0, 0, p_PhaseDataVec3[i - p_PhaseDataVec1.size() - p_PhaseDataVec2.size()]->Row,
//	 				p_PhaseDataVec3[i - p_PhaseDataVec1.size() - p_PhaseDataVec2.size()]->Col,
//	 				*p_PhaseDataVec3[i - p_PhaseDataVec1.size() - p_PhaseDataVec2.size()]);
//	 		}
//	 		else
//	 		{
//	 			tempdata[i] = new Mymatrix<double>(0, 0, p_PhaseDataVec4[i - p_PhaseDataVec1.size() - p_PhaseDataVec2.size() - p_PhaseDataVec3.size()]->Row,
//	 				p_PhaseDataVec4[i - p_PhaseDataVec1.size() - p_PhaseDataVec2.size() - p_PhaseDataVec3.size()]->Col,
//	 				*p_PhaseDataVec4[i - p_PhaseDataVec1.size() - p_PhaseDataVec2.size() - p_PhaseDataVec3.size()]);
//	 		}
//	 	}
//	 
//	 #pragma endregion
//	if (phase_exit)
//	{
//		if (tempdata != NULL)
//		{
//			for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//			{
//				delete tempdata[i];
//			}
//			delete[]tempdata;
//		}
//		is_Wrap = false;
//		return 0;
//	}
//#pragma region 幅值处理
//
//
//	Mymatrix<double>*Am = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth() / 2, *Amp);
//	Mymatrix<double>*Am0 = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth(), *Amp);
//
//
//	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
//		}
//	}
//	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
//		}
//	}
//	medFilt2(*Am);
//
//	/************** 归一化 *****************/
//	//找出am的最大最小值
//	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] > maxAm0)
//			{
//				maxAm0 = Am0->Data[i][j];
//			}
//			else if (Am0->Data[i][j] < minAm0)
//			{
//				minAm0 = Am0->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
//		}
//	}
//	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] > maxAm)
//			{
//				maxAm = Am->Data[i][j];
//			}
//			else if (Am->Data[i][j] < minAm)
//			{
//				minAm = Am->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
//		}
//	}
//
//	/********************** 幅值处理 ************************/
//	double max_A = multiData.ampMax;
//	double min_A = multiData.ampMin;
//	double A = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
//	double B = -1.5*(max_A + min_A)*A;
//	double C = -3.0*A*(pow(max_A, 2)) - 2 * B*max_A;
//	double D = 1.0 - A*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;
//
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] >= max_A)
//			{
//				Am->Data[i][j] = 1.0;
//			}
//			else if (Am->Data[i][j] <= min_A)
//			{
//				Am->Data[i][j] = 0;
//			}
//			else
//			{
//				Am->Data[i][j] = A*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] >= max_A)
//			{
//				Am0->Data[i][j] = 1.0;
//			}
//			else if (Am0->Data[i][j] <= min_A)
//			{
//				Am0->Data[i][j] = 0;
//			}
//			else
//			{
//				Am0->Data[i][j] = A*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
//			}
//		}
//	}
//	Am->Flipud();
//	Am0->Flipud();
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] >= 1)
//			{
//				Am->Data[i][j] = 1;
//			}
//			else if (Am->Data[i][j] <= 0)
//			{
//				Am->Data[i][j] = 0;
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] >= 1)
//			{
//				Am0->Data[i][j] = 1;
//			}
//			else if (Am0->Data[i][j] <= 0)
//			{
//				Am0->Data[i][j] = 0;
//			}
//		}
//	}
//
//#pragma endregion
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i += multiData.stepLen)
//	{
//		if (phase_exit)
//		{
//			CMultiDlg::bRunning = false;
//			if (tempdata != NULL)
//			{
//				for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//				{
//					delete tempdata[i];
//				}
//				delete[]tempdata;
//			}
//			is_Wrap = false;
//			return 0;
//		}
//		Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
//		Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);
//
//		HSI_Disp(tempdata[i], temp1, temp2, 2);
//
//		delete temp1;
//		delete temp2;
//		phaseProcess++;
//		CTipsDlg::getInstance()->update(phaseProcess);
//	}
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		delete tempdata[i];
//	}
//	delete[]tempdata;
//	if (phase_exit)
//	{
//		CMultiDlg::bRunning = false;
//		is_Wrap = false;
//		return 0;
//	}
//	CTipsDlg::getInstance()->destroyInstance();
//	AfxBeginThread(showPhaseColorThead, NULL);
//	delete Am;
//	delete Am0;
//	CMultiDlg::bRunning = false;
//	return 0;
//}
//UINT WrapGray(LPVOID pParam)
//{
//	is_Wrap = true;
//
//	if (phase_exit)
//	{
//		is_Wrap = false;
//		return 0;
//	}
//
//#pragma region 幅值处理
//	//Mymatrix<double>*Am = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth() / 2, *Amp);
//
//	Mymatrix<double>*Am0 = new Mymatrix<double>( area.signalHeight(), area.signalWidth());
//	for (int i = 0; i < Am0->Row;i++)
//	{
//		for (int j = 0; j < Am0->Col;j++)
//		{
//			Am0->Data[i][j] = Amp->Data[area.startRow - 1+i][area.startCol - 1+j];
//		}
//	}
//
//	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
//		}
//	}
//	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
//		}
//	}
//	Mymatrix<double>*Am = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col / 2, *Am0);
//	//medFilt2(*Am);
//	//找出am的最大最小值
//	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] > maxAm0)
//			{
//				maxAm0 = Am0->Data[i][j];
//			}
//			else if (Am0->Data[i][j] < minAm0)
//			{
//				minAm0 = Am0->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
//		}
//	}
//	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] > maxAm)
//			{
//				maxAm = Am->Data[i][j];
//			}
//			else if (Am->Data[i][j] < minAm)
//			{
//				minAm = Am->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
//		}
//	}
//	double max_A = multiData.ampMax;
//	double min_A = multiData.ampMin;
//	double A = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
//	double B = -1.5*(max_A + min_A)*A;
//	double C = -3.0*A*(pow(max_A, 2)) - 2 * B*max_A;
//	double D = 1.0 - A*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;
//
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] >= max_A)
//			{
//				Am->Data[i][j] = 1.0;
//			}
//			else if (Am->Data[i][j] <= min_A)
//			{
//				Am->Data[i][j] = 0;
//			}
//			else
//			{
//				Am->Data[i][j] = A*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] >= max_A)
//			{
//				Am0->Data[i][j] = 1.0;
//			}
//			else if (Am0->Data[i][j] <= min_A)
//			{
//				Am0->Data[i][j] = 0;
//			}
//			else
//			{
//				Am0->Data[i][j] = A*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
//			}
//		}
//	}
//	Am->Flipud();
//	Am0->Flipud();
//	
//#pragma endregion
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i += multiData.stepLen)
//	{
//		if (phase_exit)
//		{
//			CMultiDlg::bRunning = false;
//
//			is_Wrap = false;
//			return 0;
//		}
//
//
//		if (i < p_PhaseDataVec1.size())
//		{
//			Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
//			Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);
//
//			HSI_Disp(p_PhaseDataVec1[i], temp1, temp2, 2, false);
//
//			delete temp1;
//			delete temp2;
//
//		}
//		else if ((p_PhaseDataVec1.size() <= i) && (i < (p_PhaseDataVec1.size() + p_PhaseDataVec2.size())))
//		{
//			Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
//			Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);
//
//			HSI_Disp(p_PhaseDataVec2[i - p_PhaseDataVec1.size()], temp1, temp2, 2, false);
//
//			delete temp1;
//			delete temp2;
//		}
//		else if ((p_PhaseDataVec1.size() + p_PhaseDataVec2.size() <= i) && (i < p_PhaseDataVec1.size() + p_PhaseDataVec2.size() + p_PhaseDataVec3.size()))
//		{
//			Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
//			Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);
//
//			HSI_Disp(p_PhaseDataVec3[i - p_PhaseDataVec1.size() - p_PhaseDataVec2.size()], temp1, temp2, 2, false);
//
//			delete temp1;
//			delete temp2;
//		}
//		else
//		{
//			Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
//			Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);
//
//			HSI_Disp(p_PhaseDataVec4[i - p_PhaseDataVec1.size() - p_PhaseDataVec2.size() - p_PhaseDataVec3.size()], temp1, temp2, 2, false);
//
//			delete temp1;
//			delete temp2;
//		}
//	}
//	
//	if (phase_exit)
//	{
//		is_Wrap = false;
//		return 0;
//	}
//
//	delete Am;
//	delete Am0;
//
//	return 0;
//}
//UINT directUnWrapGray(LPVOID pParam)
//{
//	CMultiDlg::bRunning = true;
//#pragma region 拷贝数据
//
//	//拷贝一份数据
//	Mymatrix<double>**tempdata = new Mymatrix<double>*[multiData.imgEndNum - multiData.imgStartNum];
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		tempdata[i] = new Mymatrix<double>(0, 0, p_directUnPhaseDataVec[i]->Row, p_directUnPhaseDataVec[i]->Col, *p_directUnPhaseDataVec[i]);
//	}
//
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		for (int j = 0; j < tempdata[i]->Row; j++)
//		{
//			for (int k = 0; k < tempdata[i]->Col; k++)
//			{
//				tempdata[i]->Data[j][k] = tempdata[i]->Data[j][k] * 1.3 / PI2;
//			}
//		}
//	}
//
//	if (unphase_exit)
//	{
//		if (tempdata != NULL)
//		{
//			for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//			{
//				delete tempdata[i];
//			}
//			delete[]tempdata;
//		}
//		is_Unwrap = false;
//		return 0;
//	}
//#pragma endregion
//#pragma region 幅值处理
//	//Mymatrix<double>*Am = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth() / 2, *Amp);
//
//	Mymatrix<double>*Am0 = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth(), *Amp);
//	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
//		}
//	}
//	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
//		}
//	}
//	Mymatrix<double>*Am = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col / 2, *Am0);
//	//medFilt2(*Am);
//
//	/************** 归一化 *****************/
//	//找出am的最大最小值
//	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] > maxAm0)
//			{
//				maxAm0 = Am0->Data[i][j];
//			}
//			else if (Am0->Data[i][j] < minAm0)
//			{
//				minAm0 = Am0->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
//		}
//	}
//	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] > maxAm)
//			{
//				maxAm = Am->Data[i][j];
//			}
//			else if (Am->Data[i][j] < minAm)
//			{
//				minAm = Am->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
//		}
//	}
//
//	/********************** 幅值处理 ************************/
//	double max_A = multiData.ampMax;
//	double min_A = multiData.ampMin;
//	double A = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
//	double B = -1.5*(max_A + min_A)*A;
//	double C = -3.0*A*(pow(max_A, 2)) - 2 * B*max_A;
//	double D = 1.0 - A*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;
//
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] >= max_A)
//			{
//				Am->Data[i][j] = 1.0;
//			}
//			else if (Am->Data[i][j] <= min_A)
//			{
//				Am->Data[i][j] = 0;
//			}
//			else
//			{
//				Am->Data[i][j] = A*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] >= max_A)
//			{
//				Am0->Data[i][j] = 1.0;
//			}
//			else if (Am0->Data[i][j] <= min_A)
//			{
//				Am0->Data[i][j] = 0;
//			}
//			else
//			{
//				Am0->Data[i][j] = A*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
//			}
//		}
//	}
//	Am->Flipud();
//	Am0->Flipud();
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] >= 1)
//			{
//				Am->Data[i][j] = 1;
//			}
//			else if (Am->Data[i][j] <= 0)
//			{
//				Am->Data[i][j] = 0;
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] >= 1)
//			{
//				Am0->Data[i][j] = 1;
//			}
//			else if (Am0->Data[i][j] <= 0)
//			{
//				Am0->Data[i][j] = 0;
//			}
//		}
//	}
//#pragma endregion
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		if (unphase_exit)
//		{
//			if (tempdata != NULL)
//			{
//				for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//				{
//					delete tempdata[i];
//				}
//				delete[]tempdata;
//			}
//			is_Unwrap = false;
//			return 0;
//		}
//		Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
//		Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);
//
//		HSI_Disp(tempdata[i], temp1, temp2, 1, false);
//
//		delete temp1;
//		delete temp2;
//	}
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		delete tempdata[i];
//	}
//	delete[]tempdata;
//	if (unphase_exit)
//	{
//		is_Unwrap = false;
//		return 0;
//	}
//
//	CMultiDlg::bRunning = false;
//	return 0;
//}
//UINT directWrapGray(LPVOID pParam)
//{
//	CMultiDlg::bRunning = true;
//	if (phase_exit)
//	{
//		is_Wrap = false;
//		return 0;
//	}
//#pragma region 拷贝数据
//
//	//拷贝一份数据
//	Mymatrix<double>**tempdata = new Mymatrix<double>*[multiData.imgEndNum - multiData.imgStartNum];
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		tempdata[i] = new Mymatrix<double>(0, 0, p_directPhaseDataVec[i]->Row, p_directPhaseDataVec[i]->Col, *p_directPhaseDataVec[i]);
//	}
//
//#pragma endregion
//	if (phase_exit)
//	{
//		if (tempdata != NULL)
//		{
//			for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//			{
//				delete tempdata[i];
//			}
//			delete[]tempdata;
//		}
//		is_Wrap = false;
//		return 0;
//	}
//#pragma region 幅值处理
////	Mymatrix<double>*Am = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth() / 2, *Amp);
//
//	Mymatrix<double>*Am0 = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth(), *Amp);
//	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
//		}
//	}
//	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
//		}
//	}
//	Mymatrix<double>*Am = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col / 2, *Am0);
//	//medFilt2(*Am);
//	//找出am的最大最小值
//	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] > maxAm0)
//			{
//				maxAm0 = Am0->Data[i][j];
//			}
//			else if (Am0->Data[i][j] < minAm0)
//			{
//				minAm0 = Am0->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
//		}
//	}
//	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] > maxAm)
//			{
//				maxAm = Am->Data[i][j];
//			}
//			else if (Am->Data[i][j] < minAm)
//			{
//				minAm = Am->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
//		}
//	}
//	double max_A = multiData.ampMax;
//	double min_A = multiData.ampMin;
//	double A = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
//	double B = -1.5*(max_A + min_A)*A;
//	double C = -3.0*A*(pow(max_A, 2)) - 2 * B*max_A;
//	double D = 1.0 - A*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;
//
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] >= max_A)
//			{
//				Am->Data[i][j] = 1.0;
//			}
//			else if (Am->Data[i][j] <= min_A)
//			{
//				Am->Data[i][j] = 0;
//			}
//			else
//			{
//				Am->Data[i][j] = A*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] >= max_A)
//			{
//				Am0->Data[i][j] = 1.0;
//			}
//			else if (Am0->Data[i][j] <= min_A)
//			{
//				Am0->Data[i][j] = 0;
//			}
//			else
//			{
//				Am0->Data[i][j] = A*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
//			}
//		}
//	}
//	Am->Flipud();
//	Am0->Flipud();
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] >= 1)
//			{
//				Am->Data[i][j] = 1;
//			}
//			else if (Am->Data[i][j] <= 0)
//			{
//				Am->Data[i][j] = 0;
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] >= 1)
//			{
//				Am0->Data[i][j] = 1;
//			}
//			else if (Am0->Data[i][j] <= 0)
//			{
//				Am0->Data[i][j] = 0;
//			}
//		}
//	}
//#pragma endregion
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i += multiData.stepLen)
//	{
//		if (phase_exit)
//		{
//			if (tempdata != NULL)
//			{
//				for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//				{
//					delete tempdata[i];
//				}
//				delete[]tempdata;
//			}
//			is_Wrap = false;
//			return 0;
//		}
//		Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
//		Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);
//
//		HSI_Disp(tempdata[i], temp1, temp2, 2, false);
//
//		delete temp1;
//		delete temp2;
//	}
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		delete tempdata[i];
//	}
//	delete[]tempdata;
//	if (phase_exit)
//	{
//		is_Wrap = false;
//		return 0;
//	}
//
//	delete Am;
//	delete Am0;
//
//	return 0;
//}
//UINT unWrapGray(LPVOID pParam)
//{
//	CMultiDlg::bRunning = true;
//	is_Unwrap = true;
//
//	if (unphase_exit)
//	{
//		is_Unwrap = false;
//		return 0;
//	}
//#pragma region 拷贝数据
//
//	//拷贝一份数据
//	Mymatrix<double>**tempdata = new Mymatrix<double>*[multiData.imgEndNum - multiData.imgStartNum];
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		if (i < p_UnPhaseDataVec1.size())
//		{
//			tempdata[i] = new Mymatrix<double>(0, 0, p_UnPhaseDataVec1[i]->Row, p_UnPhaseDataVec1[i]->Col, *p_UnPhaseDataVec1[i]);
//		}
//		else if ((p_UnPhaseDataVec1.size() <= i) && (i < (p_UnPhaseDataVec1.size() + p_UnPhaseDataVec2.size())))
//		{
//			tempdata[i] = new Mymatrix<double>(0, 0, p_UnPhaseDataVec2[i - p_UnPhaseDataVec1.size()]->Row,
//				p_UnPhaseDataVec2[i - p_UnPhaseDataVec1.size()]->Col, 
//				*p_UnPhaseDataVec2[i - p_UnPhaseDataVec1.size()]);
//		}
//		else if ((p_UnPhaseDataVec1.size() + p_UnPhaseDataVec2.size() <= i) && (i < p_UnPhaseDataVec1.size() + p_UnPhaseDataVec2.size() + p_UnPhaseDataVec3.size()))
//		{
//			tempdata[i] = new Mymatrix<double>(0, 0, 
//				p_UnPhaseDataVec3[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size()]->Row,
//				p_UnPhaseDataVec3[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size()]->Col,
//				*p_UnPhaseDataVec3[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size()]);
//		}
//		else
//		{
//			tempdata[i] = new Mymatrix<double>(0, 0, 
//				p_UnPhaseDataVec4[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size() - p_UnPhaseDataVec3.size()]->Row,
//				p_UnPhaseDataVec4[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size() - p_UnPhaseDataVec3.size()]->Col,
//				*p_UnPhaseDataVec4[i - p_UnPhaseDataVec1.size() - p_UnPhaseDataVec2.size() - p_UnPhaseDataVec3.size()]);
//		}
//	}
//
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		for (int j = 0; j < tempdata[i]->Row; j++)
//		{
//			for (int k = 0; k < tempdata[i]->Col; k++)
//			{
//				tempdata[i]->Data[j][k] = tempdata[i]->Data[j][k] * 1.3 / PI2;
//			}
//		}
//	}
//
//#pragma endregion
//	if (unphase_exit)
//	{
//		if (tempdata != NULL)
//		{
//			for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//			{
//				delete tempdata[i];
//			}
//			delete[]tempdata;
//		}
//		is_Unwrap = false;
//		return 0;
//	}
//#pragma region 幅值处理
////	Mymatrix<double>*Am = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth() / 2, *Amp);
//
//	Mymatrix<double>*Am0 = new Mymatrix<double>(area.startRow - 1, area.startCol - 1, area.signalHeight(), area.signalWidth(), *Amp);
//	for (int i = 0; i < Am0->Col / 2; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 + area.signalWidth() / 2 + i];
//		}
//	}
//	for (int i = Am0->Col / 2; i < Am0->Col; i++)   //850*276
//	{
//		for (int j = 0; j < Am0->Row; j++)
//		{
//			Am0->Data[j][i] = Amp->Data[area.startRow - 1 + j][area.startCol - 1 + i] + Amp->Data[area.startRow - 1 + j][area.startCol - 1 - area.signalWidth() / 2 + i];
//		}
//	}
//	Mymatrix<double>*Am = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col / 2, *Am0);
//	//medFilt2(*Am);
//	//找出am的最大最小值
//	double maxAm0 = Am0->Data[0][0], minAm0 = Am0->Data[0][0];
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] > maxAm0)
//			{
//				maxAm0 = Am0->Data[i][j];
//			}
//			else if (Am0->Data[i][j] < minAm0)
//			{
//				minAm0 = Am0->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			Am0->Data[i][j] = (Am0->Data[i][j] - minAm0) / (maxAm0 - minAm0);
//		}
//	}
//	double maxAm = Am->Data[0][0], minAm = Am->Data[0][0];
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] > maxAm)
//			{
//				maxAm = Am->Data[i][j];
//			}
//			else if (Am->Data[i][j] < minAm)
//			{
//				minAm = Am->Data[i][j];
//			}
//		}
//	}
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			Am->Data[i][j] = (Am->Data[i][j] - minAm) / (maxAm - minAm);
//		}
//	}
//	double max_A = multiData.ampMax;
//	double min_A = multiData.ampMin;
//	double A = 1.0 / (((pow(max_A, 3)) - (pow(min_A, 3))) - 1.5*(max_A + min_A)*((pow(max_A, 2)) - (pow(min_A, 2))) - 3 * (max_A - min_A)*(pow(max_A, 2)) + 3 * ((pow(max_A, 2)) - (pow(min_A, 2)))*max_A);
//	double B = -1.5*(max_A + min_A)*A;
//	double C = -3.0*A*(pow(max_A, 2)) - 2 * B*max_A;
//	double D = 1.0 - A*(pow(max_A, 3)) - B*(pow(max_A, 2)) - C*max_A;
//
//	for (int i = 0; i < Am->Row; i++)
//	{
//		for (int j = 0; j < Am->Col; j++)
//		{
//			if (Am->Data[i][j] >= max_A)
//			{
//				Am->Data[i][j] = 1.0;
//			}
//			else if (Am->Data[i][j] <= min_A)
//			{
//				Am->Data[i][j] = 0;
//			}
//			else
//			{
//				Am->Data[i][j] = A*pow(Am->Data[i][j], 3) + B*pow(Am->Data[i][j], 2) + C*Am->Data[i][j] + D;
//			}
//		}
//	}
//	for (int i = 0; i < Am0->Row; i++)
//	{
//		for (int j = 0; j < Am0->Col; j++)
//		{
//			if (Am0->Data[i][j] >= max_A)
//			{
//				Am0->Data[i][j] = 1.0;
//			}
//			else if (Am0->Data[i][j] <= min_A)
//			{
//				Am0->Data[i][j] = 0;
//			}
//			else
//			{
//				Am0->Data[i][j] = A*pow(Am0->Data[i][j], 3) + B*pow(Am0->Data[i][j], 2) + C*Am0->Data[i][j] + D;
//			}
//		}
//	}
//	Am->Flipud();
//	Am0->Flipud();
//#pragma endregion
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		if (unphase_exit)
//		{
//			if (tempdata != NULL)
//			{
//				for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//				{
//					delete tempdata[i];
//				}
//				delete[]tempdata;
//			}
//			is_Unwrap = false;
//			return 0;
//		}
//		Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Am->Row, Am->Col, *Am);
//		Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Am0->Row, Am0->Col, *Am0);
//
//		HSI_Disp(tempdata[i], temp1, temp2, 1, false);
//
//		delete temp1;
//		delete temp2;
//	}
//	for (int i = 0; i < multiData.imgEndNum - multiData.imgStartNum; i++)
//	{
//		delete tempdata[i];
//	}
//	delete[]tempdata;
//	if (unphase_exit)
//	{
//		is_Unwrap = false;
//		return 0;
//	}
//
//	CMultiDlg::bRunning = false;
//	is_Unwrap = false;
//	return 0;
//}