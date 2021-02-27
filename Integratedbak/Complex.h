#pragma once

#include <math.h>
template<typename T>
class Complex
{
public:
	Complex()
	{
		re = 0; im = 0;
	}
	Complex(T r, T i)
	{
		re = r; im = i;
	}
	Complex(const Complex<T> &a)
	{
		re = a.re; im = a.im;
	}
	~Complex()
	{
	}

	Complex operator+(const Complex &a)
	{
		Complex z;
		z.re = re + a.re;
		z.im = im + a.im;
		return z;
	}
	Complex operator-(const Complex &a)
	{
		Complex z; z.re = re - a.re;
		z.im = im - a.im;
		return z;
	}
	Complex operator*(const Complex &a)
	{
		Complex z;
		z.re = re * a.re - im * a.im;
		z.im = re * a.im + a.re * im;
		return z;
	}
	Complex operator/(const Complex &a)
	{
		Complex z;
		T m = a.re*a.re + a.im*a.im;
		z.re = (re * a.re + im * a.im) / m;
		z.im = (re * a.im - a.re * im) / m;
		return z;
	}
	Complex operator+(const T a)
	{
		Complex z;
		z.re = re + a;
		z.im = im;
		return z;
	}
	Complex operator-(const T a)
	{
		Complex z;
		z.re = re - a;
		z.im = im;
		return z;
	}
	Complex operator*(const T a)
	{
		Complex z;
		z.re = re * a;
		z.im = im * a;
		return z;
	}
	Complex operator/(const T a)
	{
		Complex z;
		z.re = re / a;
		z.im = -im / a;
		return z;
	}

	void operator+=(const Complex &a)
	{
		re += a.re;
		im += a.im;
	}

	void operator=(const Complex &a)
	{
		re = a.re;
		im = a.im;
	}
	void operator-=(const Complex &a)
	{
		re -= a.re;
		im -= a.im;
	}
	void operator*=(const Complex &a)
	{
		Complex z;
		z.re = re * a.re - im * a.im;
		z.im = re * a.im + a.re * im;
		*this = z;
	}
	void operator/=(const Complex &a)
	{
		Complex z; T m = a.re*a.re + a.im*a.im;
		z.re = (re * a.re + im * a.im) / m;
		z.im = (re * a.im - a.re * im) / m;
		*this = z;
	}

	//返回共轭复数
	Complex conjg()
	{
		Complex z;
		z.re = re;
		z.im = -im;
		return z;
	}
	//把自身变成共轭复数
	void  conjg_self()
	{
		im = -im;
	}
	//复数值置0
	void  czero()
	{
		re = 0; im = 0;
	}

	//返回复数的模（幅度）
	T mod()
	{
		return sqrt(re*re + im*im);
	}
	//返回复数幅角
	T arg()
	{
		return atan2(im, re);
	}

	//复数乘幂
	Complex& cpow(double n)
	{
		T amp = mod();
		T theta = arg();
		amp = (T)pow((double)amp, n);
		theta = (T)(theta * n);
		Complex b;
		b.re = amp * cos(theta);
		b.im = amp * sin(theta);
		return b;
	}
public:
	T re;
	T im;
};
