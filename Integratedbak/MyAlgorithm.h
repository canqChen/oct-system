#ifndef __MYALGORITHM_H__
#define __MYALGORITHM_H__

#include "Myresource.h"
#include "Mymatrix.h"
#include "Complex.h"
#include <math.h>
#include "ShowDlg.h"

/********************
��þ���ÿ��ƽ��ֵ��ɵ�������
*************************/
Mymatrix<double>* mean(Mymatrix<double>& mat);

/*************
����˷�����
**************/
Mymatrix<double>* matMultiply(Mymatrix<double> &left, Mymatrix<double> &right);

/*********************************
���Բ�ֵ
*************************************/
void interpolation(const Mymatrix<double> &dbimg1,		//���Բ�ֵǰ����ͼ1����
	const Mymatrix<double> &dbimg2,		//���Բ�ֵǰ����ͼ2����
	Mymatrix<double> &dbAfterimg1,	//���Բ�ֵ�����ͼ1����
	Mymatrix<double> &dbAfterimg2, int sign);	//���Բ�ֵ�����ͼ2����
void interpolation(const Mymatrix<double> &dbimg,
	Mymatrix<double> &dbAfterimg);
												//���мӴ���hanning��
void hanning(Mymatrix<double>& mat1, Mymatrix<double>&mat2);
void hanning(Mymatrix<double>& mat);
/*********************************************************************
����˵����CZT�б任
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
* ���ٸ���Ҷ�б任����	data:Ҫ���б任������  FFTn:�źų���  inverse=1���任  =-1��任
***************************************************/
void FFT(Complex<double> * &data1, Complex<double> * &data2, int FFTN, int inverse);

void FFT(Complex<double>* &data, int FFTN, int inverse);

/**************************************
��ά��ֵ�˲��㷨   �����������  �˲����ڴ�С nxn  nĬ��Ϊ3
******************************************/
void medFilt2(Mymatrix<double>&mat, int n = 3);

void HSI_Disp(Mymatrix<double>* & phase, Mymatrix<double>* & Am, Mymatrix<double>* & Am0, int flag = 1);
#endif