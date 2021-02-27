#ifndef __MYMATRIX_H__
#define __MYMATRIX_H__

#include "Complex.h"
#include "Myresource.h"
#define PI  3.141592653589793
#define PI2 6.283185307179586

template<typename T>
class Mymatrix
{
public:
	Mymatrix()
	{
		Data = 0;
		Row = 0;
		Col = 0;
	}
	Mymatrix(BYTE *fp, int height, int width, int index);
	Mymatrix(FILE *fp, int height, int width, int index, int index1);

	Mymatrix(int height, int width, T value=0);
	Mymatrix(Mymatrix<UINT16>&mat);
	Mymatrix(BYTE* data);
	Mymatrix(int sr, int sc, int rows, int cols, const Mymatrix<T>&mat);
	Mymatrix(FILE *fp, int height, int width);
	~Mymatrix();

	int getRowNum()
	{
		return Row;
	}
	int getColNum()
	{
		return Col;
	}
	//当数组元素(i, j)在运算符左边(被写)时，使用_Ty& operator ()
	T& operator () (int stRow, int stCol)
	{
		return Data[stRow][stCol];
	}
	//当数组元素(i, j)在运算符右边(被读)时，使用_T  operator ()
	const T operator () (int stRow, int stCol) const
	{
		return Data[stRow][stCol];
	}

	const T* operator[](const int i)const
	{
		return Data[i];
	}

	void operator -= (Mymatrix<T> &right);
	void operator += (Mymatrix<T> &right);
	void operator = (Mymatrix<T> &right);
	//friend Mymatrix<T>* operator * (Mymatrix<T> &left, Mymatrix<T> &right);

	double GetMean(int h1, int h2, int w1, int w2);	//获得矩阵数据的平均值
	void Fliplr();	//翻转矩阵每一行的数据
	void Flipud();

	void  Max_Min(T& max, T& min);

	void resize(int startrow, int startcol, int row, int col);

	void Transform();//矩阵转置
					 // 	void WriteFile(char * fileName, int mode, int way);
					 // 	void WriteToFile(FILE * fp);
					 // 	void ChangeRowNum(size_t newRowNum);
					 // 	int  SaveBitmap_24(char* dataname, char* datafix);
					 // 	int  SaveBitmap_8(char* dataname, char* datafix);

	T ** Data;
	int Row;
	int Col;

};

template<typename T>
class Mymatrix_i
{
public:
	Mymatrix_i()
	{
		Data = 0; row = 0; Col = 0;
	}
	Mymatrix_i(int height, int width);
	~Mymatrix_i();

	//当数组元素(i, j)在运算符左边(被写)时，使用_Ty& operator ()
	Complex<T>& operator () (int stRow, int stCol)
	{
		return Data[stRow][stCol];
	}
	//当数组元素(i, j)在运算符右边(被读)时，使用_Ty operator ()
	const Complex<T>& operator () (int stRow, int stCol) const
	{
		return Data[stRow][stCol];
	}
	const Complex<T>* operator[](const int i)
	{
		return Data[i];
	}

	T GetMaxMod(unsigned long* position);
	T GetMaxMod(int height, int width, unsigned long* position);
	T GetMaxMod(int h1, int h2, int w1, int w2, unsigned long * position);

	const Complex<T>* operator[](const int i)const
	{
		return Data[i];
	}

	//获得复数矩阵的幅值
	Mymatrix<double>*& getAmp();
	//获得复数矩阵的相位
	Mymatrix<double>*& getPhase();

	void Transform();
	Mymatrix<double>* GetAmplitude();
	Mymatrix<double>* GetArg();
	void Clear();

	Complex<T> ** Data;
	int Row;
	int Col;
};

#endif