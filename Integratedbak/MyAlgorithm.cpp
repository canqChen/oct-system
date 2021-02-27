#include "stdafx.h"
#include "MyAlgorithm.h"
#include "TipsDlg.h"

extern MULTIDATA multiData;
extern SINGLEDATA sglData;

extern int singleProcess;
extern AREA area;
using namespace std;
IMGINFO DataInfo;

/***************************
返回矩阵每一行的平均值组成的列向量
**************************/
Mymatrix<double>* mean(Mymatrix<double>& mat)
{
	Mymatrix<double> *temp = new Mymatrix<double>(mat.Row, 1);
	for (int i = 0; i < mat.Row; i++)
	{
		double sum = 0.0;
		for (int j = 0; j < mat.Col; j++)
		{
			sum += mat(i, j);
		}
		temp->Data[i][0] = sum / double(mat.Col);
	}
	return temp;
}

/*************************
矩阵乘法
***********************/
Mymatrix<double>* matMultiply(Mymatrix<double> &left, Mymatrix<double> &right)
{
	if (left.Col != right.Row)
	{
		return NULL;
	}
	Mymatrix<double> *temp = new Mymatrix<double>(left.Row, right.Col);
	for (int i = 0; i < left.Row; i++)
	{
		for (int j = 0; j < left.Col; j++)
		{
			double r = left.Data[i][j];
			for (int k = 0; k < right.Col; k++)
			{
				temp->Data[i][k] += r*right.Data[j][k];
			}
		}
	}
	return temp;
}

void quickSort(double a[], int left, int right)
{
	int i = left;
	int j = right;
	double temp = a[left];
	if (left >= right)
		return;
	while (i != j)
	{
		while (i < j&&a[j] >= temp)
			j--;
		if (j > i)
			a[i] = a[j];//a[i]已经赋值给temp,所以直接将a[j]赋值给a[i],赋值完之后a[j],有空位
		while (i < j&&a[i] <= temp)
			i++;
		if (i < j)
			a[j] = a[i];
	}
	a[i] = temp;//把基准插入,此时i与j已经相等R[low..pivotpos-1].keys≤R[pivotpos].key≤R[pivotpos+1..high].keys
	quickSort(a, left, i - 1);/*递归左边*/
	quickSort(a, i + 1, right);/*递归右边*/
}

/************************************************
线性插值  sign为0：单组数据线性插值   为1：多组数据线性插值
***************************************************/
void interpolation(const Mymatrix<double> &dbimg1,		//线性插值前干涉图1矩阵
				   const Mymatrix<double> &dbimg2,		//线性插值前干涉图2矩阵
				   Mymatrix<double> &dbAfterimg1,	//线性插值后干涉图1矩阵
				   Mymatrix<double> &dbAfterimg2, int sign)	//线性插值后干涉图2矩阵
{
	if (sign == 0)
	{
		//设置插值参数
		double k_min = PI2 / (sglData.midWaveLen + 0.5*sglData.bandWidth) / pow(10, -6.0);
		double k_max = PI2 / (sglData.midWaveLen - 0.5*sglData.bandWidth) / pow(10, -6.0);
		double *k = new double[sglData.imgCol];

		double step = (k_max - k_min) / (sglData.imgCol - 1);
		for (int i = 0; i < sglData.imgCol; i++)
		{
			k[i] = i*1.0*step + k_min;
		}
		//插入位置
		double *xi = new double[sglData.imgCol];

		for (int i = 0; i < sglData.imgCol; i++)
		{
			xi[i] = (PI2*pow(10, 6) / k[i] - (sglData.midWaveLen - 0.5*sglData.bandWidth))*sglData.imgCol / sglData.bandWidth;
		}
		/*double *x = new double[sglData.imgCol];
		for (size_t i = 0; i < sglData.imgCol; i++)
		{
		x[i] = i;
		}*/

		//开始插值

		for (int i = 0; i < sglData.imgRow; i++)
		{
			for (int j = 1; j < sglData.imgCol - 1; j++)
			{
				if ((int(xi[j]) + 1 < sglData.imgCol) || (int(xi[j]) < 0))
				{
					//干涉图1插值
					dbAfterimg1(i, j) = dbimg1.Data[i][int(xi[j])] + ((xi[j] - int(xi[j]))*(dbimg1.Data[i][int(xi[j]) + 1] - dbimg1.Data[i][int(xi[j])]));
					//干涉图2插值
					dbAfterimg2(i, j) = dbimg2.Data[i][int(xi[j])] + ((xi[j] - int(xi[j]))*(dbimg2.Data[i][int(xi[j]) + 1] - dbimg2.Data[i][int(xi[j])]));
				}
				else   //数组越界
				{
					if (int(xi[j]) + 1 == sglData.imgCol) //边界
					{
						//干涉图1插值
						dbAfterimg1(i, j) = dbimg1.Data[i][int(xi[j])] + ((xi[j] - int(xi[j]))*(dbimg1.Data[i][int(xi[j])] - dbimg1.Data[i][int(xi[j])]));
						//干涉图2插值
						dbAfterimg2(i, j) = dbimg2.Data[i][int(xi[j])] + ((xi[j] - int(xi[j]))*(dbimg2.Data[i][int(xi[j])] - dbimg2.Data[i][int(xi[j])]));
					}
					else if (int(xi[j]) + 1 > sglData.imgCol)  //上越界
					{
						//干涉图1插值
						dbAfterimg1(i, j) = dbimg1.Data[i][sglData.imgCol - 1];
						//干涉图2插值
						dbAfterimg2(i, j) = dbimg2[i][sglData.imgCol - 1];
					}
					else    //下越界
					{
						//干涉图1插值
						dbAfterimg1(i, j) = dbimg1[i][0];
						//干涉图2插值
						dbAfterimg2(i, j) = dbimg2[i][0];
					}
				}
			}
		}

		delete[] k;
		delete[] xi;
		//delete[] x;
	}
	else
	{
		//设置插值参数
		double k_min = PI2 / (multiData.midWaveLen + 0.5*multiData.bandWidth) / pow(10, -6.0);
		double k_max = PI2 / (multiData.midWaveLen - 0.5*multiData.bandWidth) / pow(10, -6.0);
		double *k = new double[multiData.imgCol];

		double step = (k_max - k_min) / (multiData.imgCol - 1);
		for (int i = 0; i < multiData.imgCol; i++)
		{
			k[i] = i*1.0*step + k_min;
		}
		//插入位置
		double *xi = new double[multiData.imgCol];

		for (int i = 0; i < multiData.imgCol; i++)
		{
			xi[i] = (PI2*pow(10, 6) / k[i] - (multiData.midWaveLen - 0.5*multiData.bandWidth))*multiData.imgCol / multiData.bandWidth;
		}
		/*double *x = new double[CommonData.imgCol];
		for (size_t i = 0; i < CommonData.imgCol; i++)
		{
		x[i] = i;
		}*/

		//开始插值

		for (int i = 0; i < multiData.imgRow; i++)
		{
			for (int j = 1; j < multiData.imgCol - 1; j++)
			{
				if ((int(xi[j]) + 1 < multiData.imgCol) || (int(xi[j]) < 0))
				{
					//干涉图1插值
					dbAfterimg1(i, j) = dbimg1.Data[i][int(xi[j])] + ((xi[j] - int(xi[j]))*(dbimg1.Data[i][int(xi[j]) + 1] - dbimg1.Data[i][int(xi[j])]));
					//干涉图2插值
					dbAfterimg2(i, j) = dbimg2.Data[i][int(xi[j])] + ((xi[j] - int(xi[j]))*(dbimg2.Data[i][int(xi[j]) + 1] - dbimg2.Data[i][int(xi[j])]));
				}
				else   //数组越界
				{
					if (int(xi[j]) + 1 == multiData.imgCol) //边界
					{
						//干涉图1插值
						dbAfterimg1(i, j) = dbimg1.Data[i][int(xi[j])] + ((xi[j] - int(xi[j]))*(dbimg1.Data[i][int(xi[j])] - dbimg1.Data[i][int(xi[j])]));
						//干涉图2插值
						dbAfterimg2(i, j) = dbimg2.Data[i][int(xi[j])] + ((xi[j] - int(xi[j]))*(dbimg2.Data[i][int(xi[j])] - dbimg2.Data[i][int(xi[j])]));
					}
					else if (int(xi[j]) + 1 > multiData.imgCol)  //上越界
					{
						//干涉图1插值
						dbAfterimg1(i, j) = dbimg1.Data[i][sglData.imgCol - 1];
						//干涉图2插值
						dbAfterimg2(i, j) = dbimg2[i][sglData.imgCol - 1];
					}
					else    //下越界
					{
						//干涉图1插值
						dbAfterimg1(i, j) = dbimg1[i][0];
						//干涉图2插值
						dbAfterimg2(i, j) = dbimg2[i][0];
					}
				}
			}
		}

		delete[] k;
		delete[] xi;
		//delete[] x;
	}
}
void interpolation(const Mymatrix<double> &dbimg,		//线性插值前干涉图矩阵
				   Mymatrix<double> &dbAfterimg)	//线性插值后干涉图矩阵
{
	//设置插值参数
	double k_min = PI2 / (DataInfo.midWaveLen + 0.5*DataInfo.bandWidth) / pow(10, -6.0);
	double k_max = PI2 / (DataInfo.midWaveLen - 0.5*DataInfo.bandWidth) / pow(10, -6.0);
	double *k = new double[dbimg.Col];

	double step = (k_max - k_min) / (dbimg.Col - 1);
	for (int i = 0; i < dbimg.Col; i++)
	{
		k[i] = i*1.0*step + k_min;
	}
	//插入位置
	double *xi = new double[dbimg.Col];

	for (int i = 0; i < dbimg.Col; i++)
	{
		xi[i] = (PI2*pow(10, 6) / k[i] - (DataInfo.midWaveLen - 0.5*DataInfo.bandWidth))*dbimg.Col / DataInfo.bandWidth;
	}
	/*double *x = new double[DataInfo.targetWidth];
	for (size_t i = 0; i < DataInfo.targetWidth; i++)
	{
	x[i] = i;
	}*/

	//开始插值

	for (int i = 0; i < dbimg.Row; i++)
	{
		for (int j = 1; j < dbimg.Col - 1; j++)
		{
			if ((int(xi[j]) + 1 < dbimg.Col) || (int(xi[j]) < 0))
			{
				//干涉图1插值
				dbAfterimg(i, j) = dbimg.Data[i][int(xi[j])] + ((xi[j] - int(xi[j]))*(dbimg.Data[i][int(xi[j]) + 1] - dbimg.Data[i][int(xi[j])]));
			}
			else   //数组越界
			{
				if (int(xi[j]) + 1 == dbimg.Col) //边界
				{
					//干涉图1插值
					dbAfterimg(i, j) = dbimg.Data[i][int(xi[j])] + ((xi[j] - int(xi[j]))*(dbimg.Data[i][int(xi[j])] - dbimg.Data[i][int(xi[j])]));
				}
				else if (int(xi[j]) + 1 > dbimg.Col)  //上越界
				{
					//干涉图1插值
					dbAfterimg(i, j) = dbimg.Data[i][dbimg.Col - 1];
				}
				else    //下越界
				{
					//干涉图1插值
					dbAfterimg(i, j) = dbimg[i][0];
				}
			}
		}
	}

	delete[] k;
	delete[] xi;
	//delete[] x;
}
/******************************
对矩阵逐行对列加窗（hanning窗）
*************************/
void hanning(Mymatrix<double>& mat1, Mymatrix<double>&mat2)
{
	for (int i = 0; i < mat1.Row; i++)
	{
		for (int j = 1; j <= mat1.Col; j++)
		{
			mat1.Data[i][j - 1] *= (0.5*(1 - cos(PI2*j / double(mat1.Col + 1))));
			mat2.Data[i][j - 1] *= (0.5*(1 - cos(PI2*j / double(mat2.Col + 1))));
		}
	}
}
void hanning(Mymatrix<double>& mat)
{
	for (int i = 0; i < mat.Row; i++)
	{
		for (int j = 1; j <= mat.Col; j++)
		{
			mat.Data[i][j - 1] *= (0.5*(1 - cos(PI2*j / double(mat.Col + 1))));
		}
	}
}
/**************************************************
* 快速傅里叶行变换函数	data:要进行变换的数据  FFTn:信号长度  inverse=1正变换  =-1逆变换
***************************************************/
void FFT(Complex<double>* &data1, Complex<double>* &data2, int FFTN, int inverse)
{
	Complex<double> W, deltaW, t;

	//蝶形图级数
	int M = int(log(double(FFTN)) / log(2.0));
	//while (FFTn != (0x0001 << M)) M++;

	//序列变换
	for (int i = 0, j = 0; i < FFTN - 1; i++)
	{
		if (i < j)
		{
			t = data1[j];
			data1[j] = data1[i];
			data1[i] = t;
			t = data2[j];
			data2[j] = data2[i];
			data2[i] = t;
		}
		int k = FFTN / 2;
		while (k <= j)
		{
			j -= k;
			k /= 2;
		}
		j += k;
	}

	int  space = 1;//两组同因子结构的距离
	int Wnum;		//一级中W因子个数
	double temp1, temp2;
	for (int l = 0; l < M; l++)  //级数遍历
	{
		space *= 2;
		Wnum = space / 2;		//2^l
		W.re = 1.0;
		W.im = 0.0;
		deltaW.re = (double)cos(PI / Wnum);
		deltaW.im = (double)((-1)*inverse* sin(PI / Wnum));
		for (int j = 0; j < Wnum; j++)
		{
			for (int i = j; i < FFTN; i += space)
			{
				int partner = i + Wnum;
				t.re = data1[partner].re*W.re - data1[partner].im*W.im; //data1[partner]*W
				t.im = data1[partner].re*W.im + data1[partner].im*W.re;
				data1[partner].re = data1[i].re - t.re;
				data1[partner].im = data1[i].im - t.im;
				data1[i].re += t.re;
				data1[i].im += t.im;

				t.re = data2[partner].re*W.re - data2[partner].im*W.im; //data2[partner]*W
				t.im = data2[partner].re*W.im + data2[partner].im*W.re;
				data2[partner].re = data2[i].re - t.re;
				data2[partner].im = data2[i].im - t.im;
				data2[i].re += t.re;
				data2[i].im += t.im;
			}
			temp1 = W.re;
			temp2 = W.im;
			W.re = (double)(temp1*deltaW.re - temp2*deltaW.im);
			W.im = (double)(temp1*deltaW.im + temp2*deltaW.re);
			/*W.re = (double)(W.re*deltaW.re - W.im*deltaW.im);
			W.im = (double)(W.re*deltaW.im + W.im*deltaW.re);*/
		}
	}
	if (inverse > 0) return;
	for (int i = 0; i < FFTN; i++)
	{
		data1[i].re /= FFTN;
		data1[i].im /= FFTN;

		data2[i].re /= FFTN;
		data2[i].im /= FFTN;
	}
	return;
}

/**************************************************
* 快速傅里叶行变换函数	data:要进行变换的数据  FFTn:信号长度  inverse=1正变换  =-1逆变换
***************************************************/
void FFT(Complex<double>* &data, int FFTN, int inverse)
{
	Complex<double> W, deltaW, t;

	//蝶形图级数
	int M = int(log(double(FFTN)) / log(2.0));

	//序列变换
	for (int i = 0, j = 0; i < FFTN - 1; i++)
	{
		if (i < j)
		{
			t = data[j];
			data[j] = data[i];
			data[i] = t;
		}
		int k = FFTN / 2;
		while (k <= j)
		{
			j -= k;
			k /= 2;
		}
		j += k;
	}

	int  space = 1;//两组同因子结构的距离
	int Wnum;		//一级中W因子个数
	double temp1, temp2;
	for (int l = 0; l < M; l++)
	{
		space *= 2;
		Wnum = space / 2;		//2^l
		W.re = 1.0;
		W.im = 0.0;
		deltaW.re = double(cos(PI / Wnum));
		deltaW.im = double((-1)*inverse * sin(PI / Wnum));
		for (int j = 0; j < Wnum; j++)
		{
			for (int i = j; i < FFTN; i += space)
			{
				int partner = i + Wnum;
				t.re = data[partner].re*W.re - data[partner].im*W.im; //data[partner]*W
				t.im = data[partner].re*W.im + data[partner].im*W.re;
				data[partner].re = data[i].re - t.re;
				data[partner].im = data[i].im - t.im;
				data[i].re += t.re;
				data[i].im += t.im;
			}
			temp1 = W.re;
			temp2 = W.im;
			W.re = (double)(temp1*deltaW.re - temp2*deltaW.im);
			W.im = (double)(temp1*deltaW.im + temp2*deltaW.re);
		}
	}
	if (inverse > 0) return;
	for (int i = 0; i < FFTN; i++)
	{
		data[i].re /= FFTN;
		data[i].im /= FFTN;
	}
	return;
}

/*********************************************************************
函数说明：CZT行变换   A起点   W 步长
**********************************************************************/
void CZT(const Mymatrix<double> &dbAfterimg1, const Mymatrix<double> &dbAfterimg2,
		 Mymatrix_i<double> &dbCZTout1, Mymatrix_i<double> &dbCZTout2,
		 Complex<double> &A, Complex<double> &W, const int sign)
{
	int L = 1; //CZT变换所需长度
	int N = dbAfterimg1.Col; //列数（x（n）信号长度）
	int M;  //频点数，最终变换的长度
	if (sign == 0)
	{
		M = sglData.freCount;
	}
	else
	{
		M = multiData.freCount;	//频点数，最终变换的长度
	}
	//使L>=N+M-1,并且为2的幂
	do
	{
		L *= 2;
	} while (L < N + M - 1);

	Complex<double>* hn = new Complex<double>[L];
	Complex<double>* tmp = new Complex<double>[N];

	Complex<double> tmp1, tmp2;
	double ampA = A.mod();		//A的模（幅值）
	double thetaA = A.arg();	//A的幅角
	double ampW = W.mod();		//W的模（幅值）
	double thetaW = W.arg();		//W的幅角
	double tempAmp, tempTheta;

	//计算 A^-n * W^n*n/2
	for (int i = 0; i < N; i++)
	{
		tempAmp = (double)pow(ampA, -i);
		tempTheta = (double)(thetaA * (-i));
		tmp1.re = tempAmp * cos(tempTheta);
		tmp1.im = tempAmp * sin(tempTheta);

		tempAmp = (double)pow(ampW, double(i*i*0.5));
		tempTheta = (double)(thetaW * (i*i*0.5));
		tmp2.re = tempAmp * cos(tempTheta);
		tmp2.im = tempAmp * sin(tempTheta);

		//tmp[i] = tmp1*tmp2;
		tmp[i].re = tmp1.re*tmp2.re - tmp1.im*tmp2.im;
		tmp[i].im = tmp1.re*tmp2.im + tmp1.im*tmp2.re;
	}

	//计算W^-n*n/2
	for (int i = 0; i < M; i++)
	{
		tempAmp = (double)pow(ampW, double(-i*i*0.5));
		tempTheta = (double)(thetaW * (-i*i*0.5));
		hn[i].re = tempAmp * cos(tempTheta);
		hn[i].im = tempAmp * sin(tempTheta);
	}

	//计算W^-(L-n)^2/2
	for (int i = L - N + 1; i < L; i++)
	{
		tempAmp = (double)pow(ampW, double(-(L - i)*(L - i)*0.5));
		tempTheta = (double)(thetaW * (-(L - i)*(L - i)*0.5));
		hn[i].re = tempAmp * cos(tempTheta);
		hn[i].im = tempAmp * sin(tempTheta);
	}

	FFT(hn, L, 1);

	Complex<double>* gn1 = new Complex<double>[L];
	Complex<double>* gn2 = new Complex<double>[L];
	Complex<double>* yr1 = new Complex<double>[L];
	Complex<double>* yr2 = new Complex<double>[L];
	for (int idy = 0; idy < dbAfterimg1.Row; idy++)
	{
		//计算gn  x(n)*A^-n*W^n*n/2
		for (int i = 0; i < N; i++)
		{
			gn1[i].re = tmp[i].re*dbAfterimg1.Data[idy][i];
			gn1[i].im = tmp[i].im*dbAfterimg1.Data[idy][i];

			gn2[i].re = tmp[i].re*dbAfterimg2.Data[idy][i];
			gn2[i].im = tmp[i].im*dbAfterimg2.Data[idy][i];
			/*gn1[i] = tmp[i] * dbAfterimg1(idy, i);
			gn2[i] = tmp[i] * dbAfterimg2(idy, i);*/
		}

		//FFT(gn1, L, 1);
		FFT(gn1, gn2, L, 1);
		//FFT(gn2, L, 1);

		//计算hn*gn
		for (int i = 0; i < L; i++)
		{
			yr1[i].re = hn[i].re * gn1[i].re - hn[i].im * gn1[i].im;
			yr1[i].im = hn[i].re * gn1[i].im + gn1[i].re * hn[i].im;

			yr2[i].re = hn[i].re * gn2[i].re - hn[i].im * gn2[i].im;
			yr2[i].im = hn[i].re * gn2[i].im + gn2[i].re * hn[i].im;

			/*pComp1[i] = gn1[i] * hn[i];
			pComp2[i] = gn2[i] * hn[i];*/
		}

		//FFT(yr1, L, -1);
		FFT(yr1, yr2, L, -1); //IDFT  逆变换
							  //FFT(yr2, L, -1);

		Complex<double> temp;
		for (int i = 0; i < M; i++)  //取前M个变换结果
		{
			tempAmp = (double)pow(ampW, double(i*i*0.5));
			tempTheta = (double)(thetaW * (i*i*0.5));
			temp.re = tempAmp * cos(tempTheta);
			temp.im = tempAmp * sin(tempTheta);

			dbCZTout1.Data[idy][i].re = temp.re * yr1[i].re - temp.im * yr1[i].im;
			dbCZTout1.Data[idy][i].im = temp.re *yr1[i].im + yr1[i].re * temp.im;

			dbCZTout2.Data[idy][i].re = temp.re * yr2[i].re - temp.im * yr2[i].im;
			dbCZTout2.Data[idy][i].im = temp.re *yr2[i].im + yr2[i].re * temp.im;

			/*dbCZTout1(idy, i) = W.cpow(i*i*0.5)*pComp1[i];
			dbCZTout2(idy, i) = W.cpow(i*i*0.5)*pComp2[i];*/
		}
		for (int i = 0; i < L; i++)
		{
			gn1[i].re = 0;
			gn1[i].im = 0;
			gn2[i].re = 0;
			gn2[i].im = 0;
			yr1[i].re = 0;
			yr1[i].im = 0;
			yr2[i].re = 0;
			yr2[i].im = 0;
		}
	}
	delete[] gn1;
	delete[] yr1;
	delete[] gn2;
	delete[] yr2;
	delete[] hn;
	delete[] tmp;
}
double ** inData=nullptr;
Complex<double> **outData=nullptr;
Complex<double>* hn=nullptr;
Complex<double>* tmp = nullptr;
int row;
double ampW=0;//W的模（幅值）
double thetaW=0;//W的幅角
double ampA = 0;		//A的模（幅值）
double thetaA = 0;	//A的幅角
int N, L;
bool isDone1, isDone2, isDone3, isDone4, isDone5, isDone6, isDone7, isDone8
, isDone9, isDone10, isDone11, isDone12, isDone13, isDone14, isDone15, isDone16;
Complex<double> *temp = nullptr;
UINT process1(LPVOID pParam)
{
	isDone1 = false;
	Complex<double>* gn = new Complex<double>[L];
	Complex<double>* yr = new Complex<double>[L];
	for (int idy = 0; idy < row / 8; idy++)
	{
		//计算gn  x(n)*A^-n*W^n*n/2
		for (int i = 0; i < N; i++)
		{
			gn[i].re = tmp[i].re*inData[idy][i];
			gn[i].im = tmp[i].im*inData[idy][i];
		}

		FFT(gn, L, 1);

		//计算hn*gn
		for (int i = 0; i < L; i++)
		{
			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
		}

		FFT(yr, L, -1);

		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
		{
			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
		}
		for (int i = N; i < L; i++)
		{
			gn[i].re = 0;
			gn[i].im = 0;
		}
	}
	delete[] gn;
	delete[] yr;
	isDone1 = true;
	return 0;
}
UINT process2(LPVOID pParam)
{
	isDone2 = false;
	Complex<double>* gn = new Complex<double>[L];
	Complex<double>* yr = new Complex<double>[L];
	for (int idy = row / 8; idy < row / 4; idy++)
	{
		//计算gn  x(n)*A^-n*W^n*n/2
		for (int i = 0; i < N; i++)
		{
			gn[i].re = tmp[i].re*inData[idy][i];
			gn[i].im = tmp[i].im*inData[idy][i];
		}

		FFT(gn, L, 1);

		//计算hn*gn
		for (int i = 0; i < L; i++)
		{
			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
		}

		FFT(yr, L, -1);

		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
		{
			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
		}
		for (int i = N; i < L; i++)
		{
			gn[i].re = 0;
			gn[i].im = 0;
		}
	}
	delete[] gn;
	delete[] yr;
	isDone2 = true;
	return 0;
}
UINT process3(LPVOID pParam)
{
	isDone3 = false;
	Complex<double>* gn = new Complex<double>[L];
	Complex<double>* yr = new Complex<double>[L];
	for (int idy = row / 4; idy < row * 3 / 8; idy++)
	{
		//计算gn  x(n)*A^-n*W^n*n/2
		for (int i = 0; i < N; i++)
		{
			gn[i].re = tmp[i].re*inData[idy][i];
			gn[i].im = tmp[i].im*inData[idy][i];
		}

		FFT(gn, L, 1);

		//计算hn*gn
		for (int i = 0; i < L; i++)
		{
			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
		}

		FFT(yr, L, -1);

		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
		{
			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
		}
		for (int i = N; i < L; i++)
		{
			gn[i].re = 0;
			gn[i].im = 0;
		}
	}
	delete[] gn;
	delete[] yr;
	isDone3 = true;
	return 0;
}
UINT process4(LPVOID pParam)
{
	isDone4 = false;
	Complex<double>* gn = new Complex<double>[L];
	Complex<double>* yr = new Complex<double>[L];
	for (int idy = row * 3 / 8; idy < row / 2; idy++)
	{
		//计算gn  x(n)*A^-n*W^n*n/2
		for (int i = 0; i < N; i++)
		{
			gn[i].re = tmp[i].re*inData[idy][i];
			gn[i].im = tmp[i].im*inData[idy][i];
		}

		FFT(gn, L, 1);

		//计算hn*gn
		for (int i = 0; i < L; i++)
		{
			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
		}

		FFT(yr, L, -1);

		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
		{
			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
		}
		for (int i = N; i < L; i++)
		{
			gn[i].re = 0;
			gn[i].im = 0;
		}
	}
	delete[] gn;
	delete[] yr;
	isDone4 = true;
	return 0;
}
UINT process5(LPVOID pParam)
{
	isDone5 = false;
	Complex<double>* gn = new Complex<double>[L];
	Complex<double>* yr = new Complex<double>[L];
	for (int idy = row / 2; idy < row * 5 / 8; idy++)
	{
		//计算gn  x(n)*A^-n*W^n*n/2
		for (int i = 0; i < N; i++)
		{
			gn[i].re = tmp[i].re*inData[idy][i];
			gn[i].im = tmp[i].im*inData[idy][i];
		}

		FFT(gn, L, 1);

		//计算hn*gn
		for (int i = 0; i < L; i++)
		{
			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
		}

		FFT(yr, L, -1);

		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
		{
			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
		}
		for (int i = N; i < L; i++)
		{
			gn[i].re = 0;
			gn[i].im = 0;
		}
	}
	delete[] gn;
	delete[] yr;
	isDone5 = true;
	return 0;
}
UINT process6(LPVOID pParam)
{
	isDone6 = false;
	Complex<double>* gn = new Complex<double>[L];
	Complex<double>* yr = new Complex<double>[L];
	for (int idy = row * 5 / 8; idy < row * 3 / 4; idy++)
	{
		//计算gn  x(n)*A^-n*W^n*n/2
		for (int i = 0; i < N; i++)
		{
			gn[i].re = tmp[i].re*inData[idy][i];
			gn[i].im = tmp[i].im*inData[idy][i];
		}

		FFT(gn, L, 1);

		//计算hn*gn
		for (int i = 0; i < L; i++)
		{
			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
		}

		FFT(yr, L, -1);

		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
		{
			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
		}
		for (int i = N; i < L; i++)
		{
			gn[i].re = 0;
			gn[i].im = 0;
		}
	}
	delete[] gn;
	delete[] yr;
	isDone6 = true;
	return 0;
}
UINT process7(LPVOID pParam)
{
	isDone7 = false;
	Complex<double>* gn = new Complex<double>[L];
	Complex<double>* yr = new Complex<double>[L];
	for (int idy = row * 3 / 4; idy < row * 7 / 8; idy++)
	{
		//计算gn  x(n)*A^-n*W^n*n/2
		for (int i = 0; i < N; i++)
		{
			gn[i].re = tmp[i].re*inData[idy][i];
			gn[i].im = tmp[i].im*inData[idy][i];
		}

		FFT(gn, L, 1);

		//计算hn*gn
		for (int i = 0; i < L; i++)
		{
			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
		}

		FFT(yr, L, -1);

		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
		{
			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
		}
		for (int i = N; i < L; i++)
		{
			gn[i].re = 0;
			gn[i].im = 0;
		}
	}
	delete[] gn;
	delete[] yr;
	isDone7 = true;
	return 0;
}UINT process8(LPVOID pParam)
{
	isDone8 = false;
	Complex<double>* gn = new Complex<double>[L];
	Complex<double>* yr = new Complex<double>[L];
	for (int idy = row * 7 / 8; idy < row; idy++)
	{
		//计算gn  x(n)*A^-n*W^n*n/2
		for (int i = 0; i < N; i++)
		{
			gn[i].re = tmp[i].re*inData[idy][i];
			gn[i].im = tmp[i].im*inData[idy][i];
		}

		FFT(gn, L, 1);

		//计算hn*gn
		for (int i = 0; i < L; i++)
		{
			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
		}

		FFT(yr, L, -1);

		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
		{
			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
		}
		for (int i = N; i < L; i++)
		{
			gn[i].re = 0;
			gn[i].im = 0;
		}
	}
	delete[] gn;
	delete[] yr;
	isDone8 = true;
	return 0;
}
//UINT process9(LPVOID pParam)
//{
//	isDone9 = false;
//	Complex<double>* gn = new Complex<double>[L];
//	Complex<double>* yr = new Complex<double>[L];
//	for (int idy = row/2; idy < row *9/ 16; idy++)
//	{
//		//计算gn  x(n)*A^-n*W^n*n/2
//		for (int i = 0; i < N; i++)
//		{
//			gn[i].re = tmp[i].re*inData[idy][i];
//			gn[i].im = tmp[i].im*inData[idy][i];
//		}
//
//		FFT(gn, L, 1);
//
//		//计算hn*gn
//		for (int i = 0; i < L; i++)
//		{
//			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
//			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
//		}
//
//		FFT(yr, L, -1);
//
//		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
//		{
//			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
//			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
//		}
//		for (int i = 0; i < L; i++)
//		{
//			gn[i].re = 0;
//			gn[i].im = 0;
//		}
//	}
//	delete[] gn;
//	delete[] yr;
//	isDone9 = true;
//	return 0;
//}
//UINT process10(LPVOID pParam)
//{
//	isDone10 = false;
//	Complex<double>* gn = new Complex<double>[L];
//	Complex<double>* yr = new Complex<double>[L];
//	for (int idy = row*9 / 16; idy < row*5 / 8; idy++)
//	{
//		//计算gn  x(n)*A^-n*W^n*n/2
//		for (int i = 0; i < N; i++)
//		{
//			gn[i].re = tmp[i].re*inData[idy][i];
//			gn[i].im = tmp[i].im*inData[idy][i];
//		}
//
//		FFT(gn, L, 1);
//
//		//计算hn*gn
//		for (int i = 0; i < L; i++)
//		{
//			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
//			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
//		}
//
//		FFT(yr, L, -1);
//
//		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
//		{
//			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
//			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
//		}
//		for (int i = 0; i < L; i++)
//		{
//			gn[i].re = 0;
//			gn[i].im = 0;
//		}
//	}
//	delete[] gn;
//	delete[] yr;
//	isDone10 = true;
//	return 0;
//}
//UINT process11(LPVOID pParam)
//{
//	isDone11 = false;
//	Complex<double>* gn = new Complex<double>[L];
//	Complex<double>* yr = new Complex<double>[L];
//	for (int idy = row *5/ 8; idy < row * 11 / 16; idy++)
//	{
//		//计算gn  x(n)*A^-n*W^n*n/2
//		for (int i = 0; i < N; i++)
//		{
//			gn[i].re = tmp[i].re*inData[idy][i];
//			gn[i].im = tmp[i].im*inData[idy][i];
//		}
//
//		FFT(gn, L, 1);
//
//		//计算hn*gn
//		for (int i = 0; i < L; i++)
//		{
//			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
//			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
//		}
//
//		FFT(yr, L, -1);
//
//		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
//		{
//			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
//			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
//		}
//		for (int i = 0; i < L; i++)
//		{
//			gn[i].re = 0;
//			gn[i].im = 0;
//		}
//	}
//	delete[] gn;
//	delete[] yr;
//	isDone11 = true;
//	return 0;
//}
//UINT process12(LPVOID pParam)
//{
//	isDone12 = false;
//	Complex<double>* gn = new Complex<double>[L];
//	Complex<double>* yr = new Complex<double>[L];
//	for (int idy = row * 11 / 16; idy < row*3 / 4; idy++)
//	{
//		//计算gn  x(n)*A^-n*W^n*n/2
//		for (int i = 0; i < N; i++)
//		{
//			gn[i].re = tmp[i].re*inData[idy][i];
//			gn[i].im = tmp[i].im*inData[idy][i];
//		}
//
//		FFT(gn, L, 1);
//
//		//计算hn*gn
//		for (int i = 0; i < L; i++)
//		{
//			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
//			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
//		}
//
//		FFT(yr, L, -1);
//
//		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
//		{
//			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
//			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
//		}
//		for (int i = 0; i < L; i++)
//		{
//			gn[i].re = 0;
//			gn[i].im = 0;
//		}
//	}
//	delete[] gn;
//	delete[] yr;
//	isDone12 = true;
//	return 0;
//}
//UINT process13(LPVOID pParam)
//{
//	isDone13 = false;
//	Complex<double>* gn = new Complex<double>[L];
//	Complex<double>* yr = new Complex<double>[L];
//	for (int idy = row *3/ 4; idy < row * 13 / 16; idy++)
//	{
//		//计算gn  x(n)*A^-n*W^n*n/2
//		for (int i = 0; i < N; i++)
//		{
//			gn[i].re = tmp[i].re*inData[idy][i];
//			gn[i].im = tmp[i].im*inData[idy][i];
//		}
//
//		FFT(gn, L, 1);
//
//		//计算hn*gn
//		for (int i = 0; i < L; i++)
//		{
//			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
//			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
//		}
//
//		FFT(yr, L, -1);
//
//		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
//		{
//			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
//			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
//		}
//		for (int i = 0; i < L; i++)
//		{
//			gn[i].re = 0;
//			gn[i].im = 0;
//		}
//		
//	}
//	delete[] gn;
//	delete[] yr;
//	isDone13 = true;
//	return 0;
//}
//UINT process14(LPVOID pParam)
//{
//	isDone14 = false;
//	Complex<double>* gn = new Complex<double>[L];
//	Complex<double>* yr = new Complex<double>[L];
//	for (int idy = row * 13 / 16; idy < row * 7 / 8; idy++)
//	{
//		//计算gn  x(n)*A^-n*W^n*n/2
//		for (int i = 0; i < N; i++)
//		{
//			gn[i].re = tmp[i].re*inData[idy][i];
//			gn[i].im = tmp[i].im*inData[idy][i];
//		}
//
//		FFT(gn, L, 1);
//
//		//计算hn*gn
//		for (int i = 0; i < L; i++)
//		{
//			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
//			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
//		}
//
//		FFT(yr, L, -1);
//
//		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
//		{
//			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
//			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
//		}
//		for (int i = 0; i < L; i++)
//		{
//			gn[i].re = 0;
//			gn[i].im = 0;
//		}
//	}
//	delete[] gn;
//	delete[] yr;
//	isDone14 = true;
//	return 0;
//}
//UINT process15(LPVOID pParam)
//{
//	isDone15 = false;
//	Complex<double>* gn = new Complex<double>[L];
//	Complex<double>* yr = new Complex<double>[L];
//	for (int idy = row * 7 / 8; idy < row * 15 / 16; idy++)
//	{
//		//计算gn  x(n)*A^-n*W^n*n/2
//		for (int i = 0; i < N; i++)
//		{
//			gn[i].re = tmp[i].re*inData[idy][i];
//			gn[i].im = tmp[i].im*inData[idy][i];
//		}
//
//		FFT(gn, L, 1);
//
//		//计算hn*gn
//		for (int i = 0; i < L; i++)
//		{
//			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
//			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
//		}
//
//		FFT(yr, L, -1);
//
//		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
//		{
//			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
//			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
//		}
//		for (int i = 0; i < L; i++)
//		{
//			gn[i].re = 0;
//			gn[i].im = 0;
//		}
//	}
//	delete[] gn;
//	delete[] yr;
//	isDone15 = true;
//	return 0;
//}UINT process16(LPVOID pParam)
//{
//	isDone16 = false;
//	Complex<double>* gn = new Complex<double>[L];
//	Complex<double>* yr = new Complex<double>[L];
//	for (int idy = row * 15 / 16; idy < row; idy++)
//	{
//		//计算gn  x(n)*A^-n*W^n*n/2
//		for (int i = 0; i < N; i++)
//		{
//			gn[i].re = tmp[i].re*inData[idy][i];
//			gn[i].im = tmp[i].im*inData[idy][i];
//		}
//
//		FFT(gn, L, 1);
//
//		//计算hn*gn
//		for (int i = 0; i < L; i++)
//		{
//			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
//			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
//		}
//
//		FFT(yr, L, -1);
//
//		for (int i = 0; i < DataInfo.freCount; i++)  //取前M个变换结果
//		{
//			outData[idy][i].re = temp[i].re * yr[i].re - temp[i].im * yr[i].im;
//			outData[idy][i].im = temp[i].re *yr[i].im + yr[i].re * temp[i].im;
//		}
//		for (int i = 0; i < L; i++)
//		{
//			gn[i].re = 0;
//			gn[i].im = 0;
//		}
//	}
//	delete[] gn;
//	delete[] yr;
//	isDone16 = true;
//	return 0;
//}

void CZT(Mymatrix<double> &dbAfterimg,
		 Mymatrix_i<double> &dbCZTout,
		 Complex<double> *A, Complex<double> *W)
{
	
	inData = dbAfterimg.Data;
	outData = dbCZTout.Data;
	int M = DataInfo.freCount;//频点数，最终变换的长度
	
	if (ampA == 0)
	{
		L = 1; //CZT变换所需长度
		N = dbAfterimg.Col; //列数（x（n）信号长度）

		row = dbAfterimg.Row;
		do
		{
			L *= 2;
		} while (L < N + M - 1);//使L>=N+M-1,并且为2的幂
		ampA = (*A).mod();		//A的模（幅值）
		thetaA = (*A).arg();	//A的幅角
		ampW = (*W).mod();		//W的模（幅值）
		thetaW = (*W).arg();		//W的幅角
	}
	double tempAmp, tempTheta;

	if (temp == nullptr)
	{
		temp = new Complex<double>[M];
		for (int i = 0; i < M; i++)
		{
			tempAmp = (double)pow(ampW, double(i*i*0.5));
			tempTheta = (double)(thetaW * (i*i*0.5));
			temp[i].re = tempAmp * cos(tempTheta);
			temp[i].im = tempAmp * sin(tempTheta);
		}
		Complex<double> tmp1, tmp2;
		tmp = new Complex<double>[N];
		
		//计算 A^-n * W^n*n/2
		for (int i = 0; i < N; i++)
		{
			tempAmp = (double)pow(ampA, -i);
			tempTheta = (double)(thetaA * (-i));
			tmp1.re = tempAmp * cos(tempTheta);
			tmp1.im = tempAmp * sin(tempTheta);

			tempAmp = (double)pow(ampW, double(i*i*0.5));
			tempTheta = (double)(thetaW * (i*i*0.5));
			tmp2.re = tempAmp * cos(tempTheta);
			tmp2.im = tempAmp * sin(tempTheta);

			tmp[i].re = tmp1.re*tmp2.re - tmp1.im*tmp2.im;
			tmp[i].im = tmp1.re*tmp2.im + tmp1.im*tmp2.re;
		}

		hn = new Complex<double>[L];
		//计算W^-n*n/2
		for (int i = 0; i < M; i++)
		{
			tempAmp = (double)pow(ampW, double(-i*i*0.5));
			tempTheta = (double)(thetaW * (-i*i*0.5));
			hn[i].re = tempAmp * cos(tempTheta);
			hn[i].im = tempAmp * sin(tempTheta);
		}
		//计算W^-(L-n)^2/2
		for (int i = L - N + 1; i < L; i++)
		{
			tempAmp = (double)pow(ampW, double(-(L - i)*(L - i)*0.5));
			tempTheta = (double)(thetaW * (-(L - i)*(L - i)*0.5));
			hn[i].re = tempAmp * cos(tempTheta);
			hn[i].im = tempAmp * sin(tempTheta);
		}
		FFT(hn, L, 1);
	}
	
	Complex<double>* gn = new Complex<double>[L];
	Complex<double>* yr = new Complex<double>[L];
	for (int idy = 0; idy < dbAfterimg.Row; idy++)
	{
		//计算gn  x(n)*A^-n*W^n*n/2
		for (int i = 0; i < N; i++)
		{
			gn[i].re = tmp[i].re*dbAfterimg.Data[idy][i];
			gn[i].im = tmp[i].im*dbAfterimg.Data[idy][i];
		}
		FFT(gn, L, 1);
		//计算hn*gn
		for (int i = 0; i < L; i++)
		{
			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
		}
		FFT(yr, L, -1);
		Complex<double> temp;
		for (int i = 0; i < M; i++)  //取前M个变换结果
		{
			tempAmp = (double)pow(ampW, double(i*i*0.5));
			tempTheta = (double)(thetaW * (i*i*0.5));
			temp.re = tempAmp * cos(tempTheta);
			temp.im = tempAmp * sin(tempTheta);
			dbCZTout.Data[idy][i].re = temp.re * yr[i].re - temp.im * yr[i].im;
			dbCZTout.Data[idy][i].im = temp.re *yr[i].im + yr[i].re * temp.im;
		}
		for (int i = 0; i < L; i++)
		{
			gn[i].re = 0;
			gn[i].im = 0;
			yr[i].re = 0;
			yr[i].im = 0;
		}
	}
	delete[] gn;
	delete[] yr;

	//AfxBeginThread(process1, NULL);
	//AfxBeginThread(process2, NULL);
	//AfxBeginThread(process3, NULL);
	//AfxBeginThread(process4, NULL);
	//AfxBeginThread(process5, NULL);
	//AfxBeginThread(process6, NULL);
	//AfxBeginThread(process7, NULL);
	//AfxBeginThread(process8, NULL);

	///*AfxBeginThread(process9, NULL);
	//AfxBeginThread(process10, NULL);
	//AfxBeginThread(process11, NULL);
	//AfxBeginThread(process12, NULL);
	//AfxBeginThread(process13, NULL);
	//AfxBeginThread(process14, NULL);
	//AfxBeginThread(process15, NULL);
	//AfxBeginThread(process16, NULL);*/

	//while ((!isDone1) || (!isDone2) || (!isDone3) || (!isDone4)||
	//	(!isDone5) || (!isDone6) || (!isDone7) || (!isDone8)/*||
	//	(!isDone9) || (!isDone10) || (!isDone11) || (!isDone12) ||
	//	(!isDone13) || (!isDone14) || (!isDone15) || (!isDone16)*/)
	//{
	//	DoEvents();
	//	Sleep(10);
	//}

}

//void CZT(const Mymatrix<double> &dbAfterimg,
//	Mymatrix_i<double> &dbCZTout,
//	Complex<double> &A, Complex<double> &W)
//{
//	int L = 1; //CZT变换所需长度
//	int N = dbAfterimg.Col; //列数（x（n）信号长度）
//							//频点数，最终变换的长度
//
//	int M = DataInfo.freCount;
//
//	//使L>=N+M-1,并且为2的幂
//	do
//	{
//		L *= 2;
//	} while (L < N + M - 1);
//
//	Complex<double>* hn = new Complex<double>[L];
//	Complex<double>* tmp = new Complex<double>[N];
//
//	Complex<double> tmp1, tmp2;
//	double ampA = A.mod();		//A的模（幅值）
//	double thetaA = A.arg();	//A的幅角
//	double ampW = W.mod();		//W的模（幅值）
//	double thetaW = W.arg();		//W的幅角
//	double tempAmp, tempTheta;
//
//	//计算 A^-n * W^n*n/2
//	for (int i = 0; i < N; i++)
//	{
//		tempAmp = (double)pow(ampA, -i);
//		tempTheta = (double)(thetaA * (-i));
//		tmp1.re = tempAmp * cos(tempTheta);
//		tmp1.im = tempAmp * sin(tempTheta);
//
//		tempAmp = (double)pow(ampW, double(i*i*0.5));
//		tempTheta = (double)(thetaW * (i*i*0.5));
//		tmp2.re = tempAmp * cos(tempTheta);
//		tmp2.im = tempAmp * sin(tempTheta);
//
//		//tmp[i] = tmp1*tmp2;
//		tmp[i].re = tmp1.re*tmp2.re - tmp1.im*tmp2.im;
//		tmp[i].im = tmp1.re*tmp2.im + tmp1.im*tmp2.re;
//	}
//
//	//计算W^-n*n/2
//	for (int i = 0; i < M; i++)
//	{
//		tempAmp = (double)pow(ampW, double(-i*i*0.5));
//		tempTheta = (double)(thetaW * (-i*i*0.5));
//		hn[i].re = tempAmp * cos(tempTheta);
//		hn[i].im = tempAmp * sin(tempTheta);
//	}
//
//	//计算W^-(L-n)^2/2
//	for (int i = L - N + 1; i < L; i++)
//	{
//		tempAmp = (double)pow(ampW, double(-(L - i)*(L - i)*0.5));
//		tempTheta = (double)(thetaW * (-(L - i)*(L - i)*0.5));
//		hn[i].re = tempAmp * cos(tempTheta);
//		hn[i].im = tempAmp * sin(tempTheta);
//	}
//
//	FFT(hn, L, 1);
//
//	Complex<double>* gn = new Complex<double>[L];
//	//Complex<double>* gn2 = new Complex<double>[L];
//	Complex<double>* yr = new Complex<double>[L];
//	//Complex<double>* yr2 = new Complex<double>[L];
//	for (int idy = 0; idy < dbAfterimg.Row; idy++)
//	{
//		//计算gn  x(n)*A^-n*W^n*n/2
//		for (int i = 0; i < N; i++)
//		{
//			gn[i].re = tmp[i].re*dbAfterimg.Data[idy][i];
//			gn[i].im = tmp[i].im*dbAfterimg.Data[idy][i];
//		}
//
//		FFT(gn, L, 1);
//		//FFT(gn1, gn2, L, 1);
//		//FFT(gn2, L, 1);
//
//		//计算hn*gn
//		for (int i = 0; i < L; i++)
//		{
//			yr[i].re = hn[i].re * gn[i].re - hn[i].im * gn[i].im;
//			yr[i].im = hn[i].re * gn[i].im + gn[i].re * hn[i].im;
//		}
//
//		FFT(yr, L, -1);
//		//FFT(yr1, yr2, L, -1); //IDFT  逆变换
//		//FFT(yr2, L, -1);
//
//		Complex<double> temp;
//		for (int i = 0; i < M; i++)  //取前M个变换结果
//		{
//			tempAmp = (double)pow(ampW, double(i*i*0.5));
//			tempTheta = (double)(thetaW * (i*i*0.5));
//			temp.re = tempAmp * cos(tempTheta);
//			temp.im = tempAmp * sin(tempTheta);
//
//			dbCZTout.Data[idy][i].re = temp.re * yr[i].re - temp.im * yr[i].im;
//			dbCZTout.Data[idy][i].im = temp.re *yr[i].im + yr[i].re * temp.im;
//		}
//		for (int i = 0; i < L; i++)
//		{
//			gn[i].re = 0;
//			gn[i].im = 0;
//
//			yr[i].re = 0;
//			yr[i].im = 0;
//		}
//	}
//	delete[] gn;
//	delete[] yr;
//
//	delete[] hn;
//	delete[] tmp;
//}
/*************************************
获得数组中值
*************************************/
double getMidNum(double * array, int size)
{
	quickSort(array, 0, size - 1);

	double mid;
	if ((size & 1) > 0)
	{
		// 数组有奇数个元素，返回中间一个元素
		mid = array[size / 2];
	}
	else
	{
		// 数组有偶数个元素，返回中间两个元素平均值
		mid = (array[size / 2] + array[size / 2 - 1]) / 2;
	}

	return mid;
}
/**************************************
二维中值滤波算法   参数传入矩阵  滤波窗口大小 nxn  n默认为3
******************************************/
void medFilt2(Mymatrix<double> &mat, int n)
{
	double *temp = new double[n*n];
	Mymatrix<double>tempMat(mat.Row, mat.Col);
	tempMat = mat;
	for (int i = 0; i < mat.Row; i++)
	{
		for (int j = 0; j < mat.Col; j++)
		{
			//内部
			if ((i >= n / 2) && (i < mat.Row - n / 2) && (j >= n / 2) && (j < mat.Col - n / 2))
			{
				for (int l = i - n / 2, k = 0; l <= i + n / 2; l++)
				{
					for (int m = j - n / 2; m <= j + n / 2; m++)
					{
						temp[k] = tempMat.Data[l][m];
						k++;
					}
				}
				mat.Data[i][j] = getMidNum(temp, n*n);
			}
			else if ((i < n / 2) && (j < n / 2))  //左上边界
			{
				for (int k = 0; k < (n*n - (i + 1 + n / 2)*(j + 1 + n / 2)); k++)
				{
					temp[k] = 0;
				}
				for (int k = (n*n - (i + 1 + n / 2)*(j + 1 + n / 2)), l = 0; l <= i + n / 2; l++)
				{
					for (int m = 0; m <= j + n / 2; m++)
					{
						temp[k] = tempMat.Data[l][m];
						k++;
					}
				}
				mat.Data[i][j] = getMidNum(temp, n*n);
			}
			else if ((i < n / 2) && (j >= mat.Col - n / 2)) //右上边界
			{
				for (int k = 0; k < (n*n - (i + 1 + n / 2)*(mat.Col - j + n / 2)); k++)
				{
					temp[k] = 0;
				}
				for (int k = (n*n - (i + 1 + n / 2)*(mat.Col - j + n / 2)), l = 0; l <= i + n / 2; l++)
				{
					for (int m = j - n / 2; m < mat.Col; m++)
					{
						temp[k] = tempMat.Data[l][m];
						k++;
					}
				}
				mat.Data[i][j] = getMidNum(temp, n*n);
			}
			else if ((i < n / 2) && (j < mat.Col - n / 2) && (j >= n / 2))//上部边界
			{
				for (int k = 0; k < (n*n - n*(n / 2 + i + 1)); k++)
				{
					temp[k] = 0;
				}
				for (int k = (n*n - n*(n / 2 + i + 1)), l = 0; l <= i + n / 2; l++)
				{
					for (int m = j - n / 2; m <= j + n / 2; m++)
					{
						temp[k] = tempMat.Data[l][m];
						k++;
					}
				}
				mat.Data[i][j] = getMidNum(temp, n*n);
			}
			else if ((i >= mat.Row - n / 2) && (j < n / 2)) //左下角边界
			{
				for (int k = 0; k < (n*n - (j + 1 + n / 2)*(mat.Row - i + n / 2)); k++)
				{
					temp[k] = 0;
				}
				for (int k = (n*n - (j + 1 + n / 2)*(mat.Row - i + n / 2)), l = i - n / 2; l < mat.Row; l++)
				{
					for (int m = 0; m <= j + n / 2; m++)
					{
						temp[k] = tempMat.Data[l][m];
						k++;
					}
				}
				mat.Data[i][j] = getMidNum(temp, n*n);
			}
			else if ((i >= mat.Row - n / 2) && (j >= mat.Col - n / 2))//右下角边界
			{
				for (int k = 0; k < (n*n - (mat.Col - j + n / 2)*(mat.Row - i + n / 2)); k++)
				{
					temp[k] = 0;
				}
				for (int k = (n*n - (mat.Col - j + n / 2)*(mat.Row - i + n / 2)), l = i - n / 2; l < mat.Row; l++)
				{
					for (int m = j - n / 2; m < mat.Col; m++)
					{
						temp[k] = tempMat.Data[l][m];
						k++;
					}
				}
				mat.Data[i][j] = getMidNum(temp, n*n);
			}
			else if ((i >= mat.Row - n / 2) && (j < mat.Col - n / 2) && (j >= n / 2))//下部边界
			{
				for (int k = 0; k < (n*n - n*(mat.Row - i + n / 2)); k++)
				{
					temp[k] = 0;
				}
				for (int k = (n*n - n*(mat.Row - i + n / 2)), l = i - n / 2; l < mat.Row; l++)
				{
					for (int m = j - n / 2; m <= j + n / 2; m++)
					{
						temp[k] = tempMat.Data[l][m];
						k++;
					}
				}
				mat.Data[i][j] = getMidNum(temp, n*n);
			}
			else if ((i < mat.Row - n / 2) && (i >= n / 2) && (j < n / 2)) //左边边界
			{
				for (int k = 0; k < (n*n - n*(j + 1 + n / 2)); k++)
				{
					temp[k] = 0;
				}
				for (int k = (n*n - n*(j + 1 + n / 2)), l = i - n / 2; l <= i + n / 2; l++)
				{
					for (int m = 0; m <= j + n / 2; m++)
					{
						temp[k] = tempMat.Data[l][m];
						k++;
					}
				}
				mat.Data[i][j] = getMidNum(temp, n*n);
			}
			else if ((i < mat.Row - n / 2) && (i >= n / 2) && (j >= mat.Col - n / 2))//右边边界
			{
				for (int k = 0; k < (n*n - n*(mat.Col - j + n / 2)); k++)
				{
					temp[k] = 0;
				}
				for (int k = (n*n - n*(mat.Col - j + n / 2)), l = i - n / 2; l <= i + n / 2; l++)
				{
					for (int m = j - n / 2; m < mat.Col; m++)
					{
						temp[k] = tempMat.Data[l][m];
						k++;
					}
				}
				mat.Data[i][j] = getMidNum(temp, n*n);
			}
		}
	}
	delete[] temp;
}

vector<Mymatrix<double>*>Ang0Vec;
vector<Mymatrix<double>*>Phase1Vec;
vector<Mymatrix<double>*>Phase2Vec;


//Am0 第一张幅值     Am  第二第三幅值
void HSI_Disp(Mymatrix<double>* & phase, Mymatrix<double>* & Am, Mymatrix<double>* & Am0, int flag)
{

	Mymatrix<double>*Ang0 = new Mymatrix<double>(0, 0, phase->Row, phase->Col, *phase);
	medFilt2(*Ang0, 3); //第一
	Mymatrix<double>*Ang = new Mymatrix<double>(0, 0, phase->Row, phase->Col / 2, *phase);
	Mymatrix<double>*Ang_As = new Mymatrix<double>(0, phase->Col / 2, phase->Row, phase->Col / 2, *phase);

	Mymatrix<double>*phase1 = new Mymatrix<double>(area.signalHeight(), area.signalWidth() / 2),
		*phase2 = new Mymatrix<double>(area.signalHeight(), area.signalWidth() / 2);

	if (flag == 0)
	{
		for (int i = 0; i < area.signalHeight(); i++)
		{
			for (int j = 0; j < area.signalWidth() / 2; j++)
			{
				phase1->Data[i][j] = (Ang_As->Data[i][j] - Ang->Data[i][j]);
				phase2->Data[i][j] = (Ang_As->Data[i][j] + Ang->Data[i][j]);
			}
		}
		//中值滤波
		medFilt2(*phase1);	//第二
		medFilt2(*phase2);	//第三

		Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Ang0->Row, Ang0->Col, *Ang0);
		Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, phase1->Row, phase1->Col, *phase1);
		Mymatrix<double>*temp3 = new Mymatrix<double>(0, 0, phase2->Row, phase2->Col, *phase2);

		singleProcess++;
		CTipsDlg::getInstance()->update(singleProcess);
		CTipsDlg::getInstance()->destroyInstance();

		CShowDlg::getInstance()->loadColorImg(Ang0, Am0, 0, 1);
		CShowDlg::getInstance()->loadColorImg(phase1, Am, 0, 2);
		CShowDlg::getInstance()->loadColorImg(phase2, Am, 0, 3);

		CShowDlg::getInstance()->loadGrayBar();
		CShowDlg::getInstance()->loadGrayImg(temp1, Am0, 0, 1);
		CShowDlg::getInstance()->loadGrayImg(temp2, Am, 0, 2);
		CShowDlg::getInstance()->loadGrayImg(temp3, Am, 0, 3);

		delete phase1;
		delete phase2;

		delete Ang0;
		delete Ang;
		delete Ang_As;

		delete temp3;
		delete temp2;
		delete temp1;

		return;

	}
	else if (flag == 1)
	{

		for (int i = 0; i < area.signalHeight(); i++)
		{
			for (int j = 0; j < area.signalWidth() / 2; j++)
			{
				phase1->Data[i][j] = (Ang_As->Data[i][j] - Ang->Data[i][j])*1.3 / PI2;
				phase2->Data[i][j] = (Ang_As->Data[i][j] + Ang->Data[i][j]) *0.14 / PI2;
			}
		}
		medFilt2(*phase1);
		medFilt2(*phase2);

		Ang0Vec.push_back(Ang0);
		Phase1Vec.push_back(phase1);
		Phase2Vec.push_back(phase2);
		if (Ang0Vec.size() < multiData.imgEndNum - multiData.imgStartNum)
		{
			return;
		}
		/*multiData.unwrapMax = Ang0Vec[0]->Data[0][0], multiData.unwrapMin = Ang0Vec[0]->Data[0][0];
		//for (int i = 0; i < Ang0Vec.size(); i++)
		//{
		//	for (int j = 0; j < Ang0Vec[i]->Row; j++)
		//	{
		//		for (int k = 0; k < Ang0Vec[i]->Col; k++)
		//		{
		//			if (multiData.unwrapMax<Ang0Vec[i]->Data[j][k])
		//			{
		//				multiData.unwrapMax = Ang0Vec[i]->Data[j][k];
		//			}
		//			else if (multiData.unwrapMin>Ang0Vec[i]->Data[j][k])
		//			{
		//				multiData.unwrapMin = Ang0Vec[i]->Data[j][k];
		//			}
		//		}
		//	}
		//}*/
		multiData.unwrapMax = Ang0Vec[0]->Data[0][20], multiData.unwrapMin = Ang0Vec[0]->Data[0][20];
		for (int i = 0; i < Ang0Vec.size(); i++)
		{
			for (int j = 30; j < 70; j++)
			{
				for (int k = 0; k < Ang0Vec[i]->Row; k++)
				{
					if (multiData.unwrapMax<Ang0Vec[i]->Data[k][j])
					{
						multiData.unwrapMax = Ang0Vec[i]->Data[k][j];
					}
					else if (multiData.unwrapMin>Ang0Vec[i]->Data[k][j])
					{
						multiData.unwrapMin = Ang0Vec[i]->Data[k][j];
					}
				}
			}
		}
		for (int i = 0; i < Ang0Vec.size(); i++)
		{
			for (int j = 170; j < 220; j++)
			{
				for (int k = 0; k < Ang0Vec[i]->Row; k++)
				{
					if (multiData.unwrapMax<Ang0Vec[i]->Data[k][j])
					{
						multiData.unwrapMax = Ang0Vec[i]->Data[k][j];
					}
					else if (multiData.unwrapMin>Ang0Vec[i]->Data[k][j])
					{
						multiData.unwrapMin = Ang0Vec[i]->Data[k][j];
					}
				}
			}
		}
		multiData.umMax = Phase1Vec[0]->Data[0][20];
		multiData.umMin = Phase1Vec[0]->Data[0][20];
		for (int i = 0; i < Phase1Vec.size(); i++)
		{
			for (int j = 30; j < 70; j++)
			{
				for (int k = 0; k < Phase1Vec[i]->Row; k++)
				{
					if (multiData.umMax<Phase1Vec[i]->Data[k][j])
					{
						multiData.umMax = Phase1Vec[i]->Data[k][j];
					}
					else if (multiData.umMin>Phase1Vec[i]->Data[k][j])
					{
						multiData.umMin = Phase1Vec[i]->Data[k][j];
					}

					if (multiData.umMax < Phase2Vec[i]->Data[k][j])
					{
						multiData.umMax = Phase2Vec[i]->Data[k][j];
					}
					else if (multiData.umMin>Phase2Vec[i]->Data[k][j])
					{
						multiData.umMin = Phase2Vec[i]->Data[k][j];
					}
				}
			}
		}
		//multiData.unwrapMax *= 0.98;
		//multiData.unwrapMin *= 0.95;
		multiData.umMin *= 0.3;
		multiData.umMax *= 0.6;
		/*multiData.umMax = Phase1Vec[0]->Data[0][0];
		multiData.umMin = Phase1Vec[0]->Data[0][0];
		for (int i = 0; i < Phase1Vec.size(); i++)
		{
		for (int j = 0; j < Phase1Vec[i]->Row; j++)
		{
		for (int k = 0; k < Phase1Vec[i]->Col; k++)
		{
		if (multiData.umMax<Phase1Vec[i]->Data[j][k])
		{
		multiData.umMax = Phase1Vec[i]->Data[j][k];
		}
		else if (multiData.umMin>Phase1Vec[i]->Data[j][k])
		{
		multiData.umMin = Phase1Vec[i]->Data[j][k];
		}

		if (multiData.umMax < Phase2Vec[i]->Data[j][k])
		{
		multiData.umMax = Phase2Vec[i]->Data[j][k];
		}
		else if (multiData.umMin>Phase2Vec[i]->Data[j][k])
		{
		multiData.umMin = Phase2Vec[i]->Data[j][k];
		}
		}
		}
		}*/

		for (int i = 0; i < Ang0Vec.size(); i++)
		{
			Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Ang0Vec[i]->Row, Ang0Vec[i]->Col, *Ang0Vec[i]);
			Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, Phase1Vec[i]->Row, Phase1Vec[i]->Col, *Phase1Vec[i]);
			Mymatrix<double>*temp3 = new Mymatrix<double>(0, 0, Phase2Vec[i]->Row, Phase2Vec[i]->Col, *Phase2Vec[i]);
			CShowDlg::getInstance()->loadColorImg(temp1, Am0, 1, 1);
			CShowDlg::getInstance()->loadColorImg(temp2, Am, 1, 2);
			CShowDlg::getInstance()->loadColorImg(temp3, Am, 1, 3);

			CShowDlg::getInstance()->loadGrayImg(Ang0Vec[i], Am0, 1, 1);
			CShowDlg::getInstance()->loadGrayImg(Phase1Vec[i], Am, 1, 2);
			CShowDlg::getInstance()->loadGrayImg(Phase2Vec[i], Am, 1, 3);

			delete temp3;
			delete temp2;
			delete temp1;

			unphaseProcess++;
			CTipsDlg::getInstance()->update(unphaseProcess);

		}
		for (auto &i : Ang0Vec)
		{
			delete i;
		}
		Ang0Vec.clear();
		for (auto &i : Phase1Vec)
		{
			delete i;
		}
		Phase1Vec.clear();
		for (auto &i : Phase2Vec)
		{
			delete i;
		}
		Phase2Vec.clear();
		CTipsDlg::getInstance()->destroyInstance();
		AfxBeginThread(showUnPhaseColorThead, NULL);
		return;
	}
	else
	{
		for (int i = 0; i < area.signalHeight(); i++)
		{
			for (int j = 0; j < area.signalWidth() / 2; j++)
			{
				phase1->Data[i][j] = Ang_As->Data[i][j] - Ang->Data[i][j];
				phase2->Data[i][j] = Ang_As->Data[i][j] + Ang->Data[i][j];
			}
		}
		medFilt2(*phase1);
		medFilt2(*phase2);


		Mymatrix<double>*temp1 = new Mymatrix<double>(0, 0, Ang0->Row, Ang0->Col, *Ang0);
		Mymatrix<double>*temp2 = new Mymatrix<double>(0, 0, phase1->Row, phase1->Col, *phase1);
		Mymatrix<double>*temp3 = new Mymatrix<double>(0, 0, phase2->Row, phase2->Col, *phase2);
		CShowDlg::getInstance()->loadColorImg(Ang0, Am0, 2, 1);
		CShowDlg::getInstance()->loadColorImg(phase1, Am, 2, 2);
		CShowDlg::getInstance()->loadColorImg(phase2, Am, 2, 3);

		CShowDlg::getInstance()->loadGrayImg(Ang0, Am0, 2, 1);
		CShowDlg::getInstance()->loadGrayImg(phase1, Am, 2, 2);
		CShowDlg::getInstance()->loadGrayImg(phase2, Am, 2, 3);

		delete phase1;
		delete phase2;

		delete Ang0;
		delete Ang;
		delete Ang_As;

		delete temp3;
		delete temp2;
		delete temp1;
		return;
	}
}