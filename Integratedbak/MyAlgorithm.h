#ifndef __MYALGORITHM_H__
#define __MYALGORITHM_H__

#include "Myresource.h"
#include "Mymatrix.h"
#include "Complex.h"
#include <math.h>
#include "ShowDlg.h"

/********************
获得矩阵每行平均值组成的列向量
*************************/
Mymatrix<double>* mean(Mymatrix<double>& mat);

/*************
矩阵乘法运算
**************/
Mymatrix<double>* matMultiply(Mymatrix<double> &left, Mymatrix<double> &right);

/*********************************
线性插值
*************************************/
void interpolation(const Mymatrix<double> &dbimg1,		//线性插值前干涉图1矩阵
	const Mymatrix<double> &dbimg2,		//线性插值前干涉图2矩阵
	Mymatrix<double> &dbAfterimg1,	//线性插值后干涉图1矩阵
	Mymatrix<double> &dbAfterimg2, int sign);	//线性插值后干涉图2矩阵
void interpolation(const Mymatrix<double> &dbimg,
	Mymatrix<double> &dbAfterimg);
												//对列加窗（hanning）
void hanning(Mymatrix<double>& mat1, Mymatrix<double>&mat2);
void hanning(Mymatrix<double>& mat);
/*********************************************************************
函数说明：CZT行变换
**********************************************************************/
void CZT(const Mymatrix<double> &dbAfterimg1, const Mymatrix<double> &dbAfterimg2,
	Mymatrix_i<double> &dbCZTout1, Mymatrix_i<double> &dbCZTout2,
	Complex<double>& A, Complex<double>& W, const int sign);
void CZT( Mymatrix<double> &dbAfterimg,
	Mymatrix_i<double> &dbCZTout,
	Complex<double>* A, Complex<double>* W);
//void CZT(const Mymatrix<double> &dbAfterimg,
//	Mymatrix_i<double> &dbCZTout,
//	Complex<double>& A, Complex<double>& W);
/**************************************************
* 快速傅里叶行变换函数	data:要进行变换的数据  FFTn:信号长度  inverse=1正变换  =-1逆变换
***************************************************/
void FFT(Complex<double> * &data1, Complex<double> * &data2, int FFTN, int inverse);

void FFT(Complex<double>* &data, int FFTN, int inverse);

/**************************************
二维中值滤波算法   参数传入矩阵  滤波窗口大小 nxn  n默认为3
******************************************/
void medFilt2(Mymatrix<double>&mat, int n = 3);

void HSI_Disp(Mymatrix<double>* & phase, Mymatrix<double>* & Am, Mymatrix<double>* & Am0, int flag = 1);
#endif