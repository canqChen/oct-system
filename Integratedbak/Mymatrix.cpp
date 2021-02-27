#include "stdafx.h"
#include "Mymatrix.h"
#include <WINDOWSX.H>
#include"Myresource.h"

extern MULTIDATA multiData;
extern SINGLEDATA sglData; 
extern IMGINFO DataInfo;


template<typename T>
Mymatrix<T>::Mymatrix(BYTE *fp, int height, int width, int index)
{
	Row = height;
	Col = width;
	Data = new T*[Row];
	for (int i = 0; i < Row; i++)
	{
		Data[i] = new T[Col];
		for (int j = 0; j < Col; j++)
		{
			Data[i][j] = T((fp[Col*i*2 + 2 * j + 1] * 256 + fp[Col*i*2 + 2 * j]) * 16);
		}
	}
}
template<typename T>
Mymatrix<T>::Mymatrix(FILE *fp, int height, int width, int index, int index1)
{
	Row = height;
	Col = width;
	Data = new T*[Row];
	WORD *temp = new WORD[DataInfo.targetWidth];
	for (int i = 0; i < Row; i++)
	{
		Data[i] = new T[Col];
		fread(temp, sizeof(WORD), DataInfo.targetWidth, fp);
		for (int j = 0; j < Col; j++)
		{
			Data[i][j] = T(temp[j] * 16);
		}
		
	}
	delete[] temp;
}
template<typename T>
Mymatrix<T>::Mymatrix(BYTE* data)
{
	this->Row = DataInfo.targetHeight;
	this->Col = DataInfo.targetWidth;
	Data = new T*[Row];
	for (int i = 0; i < Row; i++)
	{
		Data[i] = new T[Col];
		for (int j = 0; j < Col; j++)
		{
			Data[i][j] = T(data[i*Row + j] * 128);
		}
	}
}
template<typename T>
Mymatrix<T>::Mymatrix(FILE *fp, int height, int width)
{
	Row = height;
	Col = width;
	Data = new T*[Row];

	for (int i = 0; i < Row; i++)
	{
		Data[i] = new T[Col];
		fread(Data[i], sizeof(T), Col, fp);
	}
}
template<typename T>
Mymatrix<T>::Mymatrix(Mymatrix<UINT16>& mat)
{
	this->Row = mat.Row;
	this->Col = mat.Col;
	Data = new T*[Row];
	for (int i = 0; i < Row; i++)
	{
		Data[i] = new T[Col];
		for (int j = 0; j < Col; j++)
		{
			Data[i][j] = T(mat(i, j));
			Data[i][j] *= 16.0;
		}
	}
}


template<typename T>
Mymatrix<T>::Mymatrix(int sr, int sc, int rows, int cols, const Mymatrix<T>&mat)
{
	this->Row = rows;
	this->Col = cols;

	Data = new T*[rows];
	for (int i = sr, k = 0; k < rows; i++, k++)
	{
		Data[k] = new T[cols];

		for (int j = sc, l = 0; l < cols; j++, l++)
		{
			Data[k][l] = T(mat.Data[i][j]);
		}
	}
}

template<typename T>
Mymatrix<T>::Mymatrix(int height, int width, T value)
{
	Row = height;
	Col = width;
	Data = new T *[Row];
	/*if (value != T(0))
	{
*/
		for (int i = 0; i < Row; i++)
		{
			Data[i] = new T[Col];
			for (int j = 0; j < Col; j++)
			{
				Data[i][j] = value;
			}
		}
		/*}
		else
		{
			for (int i = 0; i < Row; i++)
			{
				Data[i] = new T[Col];

			}
		}*/
}



template<typename T>
Mymatrix<T>::~Mymatrix()
{
		if (0 != Data)
		{
			for (int i = 0; i < Row; i++)
				delete[] Data[i];
			delete[] Data;
		}
}

template<typename T>
double Mymatrix<T>::GetMean(int h1, int h2, int w1, int w2)
{
	long col_sum = 0;
	double sum = 0.0;
	for (int i = h1; i < h2; i++)
	{
		col_sum = 0;
		for (int j = w1; j < w2; j++)
			col_sum += Data[i][j];
		sum += (col_sum / (double)(w2 - w1));
	}
	return (double)(sum / (h2 - h1));
}

template<typename T>
void Mymatrix<T>::Fliplr()
{
		T** temp = new T*[Row];
		for (int i = 0; i < Row; i++)
		{
			temp[i] = new T[Col];
			for (int j = 0; j < Col; j++)
			{
				temp[i][j] = Data[i][Col - j - 1];
			}
		}
		for (int i = 0; i < Row; i++)
			delete[] Data[i];
		delete[] Data;
		Data = temp;

}
template<typename T>
void Mymatrix<T>::Flipud()
{
		T** temp = new T*[Row];
		for (int i = 0; i < Row; i++)
		{
			temp[i] = new T[Col];
			int k = Row - 1 - i;
			for (int j = 0; j < Col; j++)
			{
				temp[i][j] = Data[k][j];
			}
		}
		for (int i = 0; i < Row; i++)
			delete[] Data[i];
		delete[] Data;
		Data = temp;
}

template<typename T>
void Mymatrix<T>::operator  -= (Mymatrix<T> &right)
{
	if ((this->Row != right.Row) || (this->Col != right.Col))
	{
		return;
	}
	for (int i = 0; i < this->Row; i++)
	{
		for (int j = 0; j < this->Col; j++)
		{
			Data[i][j] -= right.Data[i][j];
		}
	}
}
template<typename T>
void Mymatrix<T>::operator += (Mymatrix<T> &right)
{
	if ((this->Row != right.Row) || (this->Col != right.Col))
	{
		return;
	}
	for (int i = 0; i < this->Row; i++)
	{
		for (int j = 0; j < this->Col; j++)
		{
			Data[i][j] += right.Data[i][j];
		}
	}
}

template<typename T>
void Mymatrix<T>::operator = (Mymatrix<T> &right)
{
	if ((this->Row != right.Row) || (this->Col != right.Col))
	{
		return;
	}
	/*for (size_t i = 0; i < Row; i++)
	delete[] Data[i];
	delete Data;*/
	/*Data = right.Data;*/
	for (int i = 0; i < this->Row; i++)
	{
		for (int j = 0; j < this->Col; j++)
		{
			Data[i][j] = right.Data[i][j];
		}
	}
}

template<typename T>
void Mymatrix<T>::Transform()
{
		T ** p = new T*[Col];
		for (int i = 0; i < Col; i++)
		{
			p[i] = new T[Row];
			for (int j = 0; j < Row; j++)
				p[i][j] = Data[j][i];
		}

		for (int i = 0; i < Row; i++)
			delete[] Data[i];
		delete[] Data;

		Data = p;
		int c = Row;
		Row = Col;
		Col = c;
}
template<typename T>
void  Mymatrix<T>::Max_Min(T& max, T& min)
{
	max = Data[0][0];
	min = Data[0][0];
	for (int i = 0; i < Row; i++)
	{
		for (int j = 0; j < Col; j++)
		{
			if (Data[i][j] > max)
			{
				max = Data[i][j];
			}
			else if (Data[i][j] < min)
			{
				min = Data[i][j];
			}
		}
	}
}

template<typename T>
void Mymatrix<T>::resize(int startrow, int startcol, int row, int col)
{
	if (row == Row&&Col == col)
	{
		return;
	}
		T **newData = new T *[row];
		for (int i = startrow, l = 0; l < row; i++, l++)
		{
			newData[l] = new T[col];
			for (int j = startcol, m = 0; m < col; j++, m++)
			{
				newData[l][m] = Data[i][j];
			}
		}
		for (int i = 0; i < Row; i++)
		{
			delete[] Data[i];
		}
		delete[] Data;

		Data = newData;
		Row = row;
		Col = col;
		
}

//template<typename T>
//void Mymatrix<T>::WriteFile(char * fileName, int mode, int way)
//{
//	FILE* fp;
//	fopen_s(&fp, fileName, "wb");
//	IMGDATAHEAD sd;
//	sd.width = Col;
//	sd.height = Row;
//	sd.depth = 1;
//	sd.ProcMode = mode;
//	sd.ProcWay = way;
//	fwrite(&sd, sizeof(IMGDATAHEAD), 1, fp);
//	WriteToFile(fp);
//	fclose(fp);
//}
//
//template<typename T>
//void Mymatrix<T>::WriteToFile(FILE * fp)
//{
//	for (size_t i = 0; i < Row; i++)
//		fwrite(m_data[i], sizeof(T), Col, fp);
//}

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

template<typename T>
Mymatrix_i<T>::Mymatrix_i(int height, int width)
{
	Row = height;
	Col = width;

	Data = new Complex<T>*[Row];
	for (int i = 0; i < Row; i++)
		Data[i] = new Complex<T>[Col];
}

template<typename T>
Mymatrix_i<T>::~Mymatrix_i()
{
	if (0 != Data)
	{
		for (int i = 0; i < Row; i++)
			delete[] Data[i];
		delete Data;
	}
}
/***********************************************************
函数说明：	获取最大峰值点坐标以及幅值
***********************************************************/
template<typename T>
T Mymatrix_i<T>::GetMaxMod(unsigned long * position)
{
	T max = 0;
	T mod = 0;
	for (int i = 0; i < Row; i++)
	{
		for (int j = 0; j < Col; j++)
		{
			mod = (T)sqrt((double)(Data[i][j].re * Data[i][j].re + Data[i][j].im * Data[i][j].im));
			{
				if (mod > max)
				{
					max = mod;
					position[0] = i;
					position[1] = j;
				}
			}
		}
	}
	return max;
}

template<typename T>
T Mymatrix_i<T>::GetMaxMod(int height, int width, unsigned long * position)
{
	T max = 0;
	T mod = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			mod = (T)sqrt((double)(Data[i][j].re * Data[i][j].re + Data[i][j].im * Data[i][j].im));
			{
				if (mod > max)
				{
					max = mod;
					position[0] = i;
					position[1] = j;
				}
			}
		}
	}
	return max;
}
template<typename T>
T Mymatrix_i<T>::GetMaxMod(int h1, int h2, int w1, int w2, unsigned long * position)
{
	T max = 0;
	T mod = 0;
	for (int i = h1; i < h2; i++)
	{
		for (int j = w1; j < w2; j++)
		{
			mod = (T)sqrt((double)(Data[i][j].re * Data[i][j].re + Data[i][j].im * Data[i][j].im));
			{
				if (mod > max)
				{
					max = mod;
					position[0] = i;
					position[1] = j;
				}
			}
		}
	}
	return max;
}

template<typename T>
void Mymatrix_i<T>::Transform()
{
	Complex<T> ** p = new Complex<T>*[Col];
	for (int i = 0; i < Col; i++)
	{
		p[i] = new Complex<T>[Row];
		for (int j = 0; j < Row; j++)
			p[i][j] = Data[j][i];
	}

	for (int i = 0; i < Row; i++)
		delete[] Data[i];
	delete Data;

	Data = p;
	int c = Row;
	Row = Col;
	Col = c;
}

template<typename T>
Mymatrix<double> * Mymatrix_i<T>::GetAmplitude()
{
	Mymatrix<double> * m_Out = new Mymatrix<double>(m_Row, m_Col);
	for (int i = 0; i < Row; i++)
		for (int j = 0; j < Col; j++)
			(*m_Out)(i, j) = (double)Data[i][j].mod();
	return m_Out;
}

template<typename T>
Mymatrix<double> * Mymatrix_i<T>::GetArg()
{
	Mymatrix<double> * m_Out = new Mymatrix<double>(Row, Col);
	for (int i = 0; i < Row; i++)
		for (int j = 0; j < Col; j++)
			(*m_Out)(i, j) = (double)Data[i][j].arg();
	return m_Out;
}

template<typename T>
void Mymatrix_i<T>::Clear()
{
	for (int i = 0; i < Row; i++)
		for (int j = 0; j < Col; j++)
			Data[i][j].czero();
}

//获得复数矩阵的幅值
template<typename T>
Mymatrix<double>* & Mymatrix_i<T>::getAmp()
{
	Mymatrix<double> *temp = new Mymatrix<double>(Row, Col);
	for (int i = 0; i < Row; i++)
	{
		for (int j = 0; j < Col; j++)
		{
			temp->Data[i][j] = Data[i][j].mod();
		}
	}
	return temp;
}
//获得复数矩阵的相位
template<typename T>
Mymatrix<double>* & Mymatrix_i<T>::getPhase()
{
	Mymatrix<double> *temp = new Mymatrix<double>(Row, Col);
	for (int i = 0; i < Row; i++)
	{
		for (int j = 0; j < Col; j++)
		{
			temp->Data[i][j] = Data[i][j].arg();
		}
	}
	return temp;
}