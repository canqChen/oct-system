#pragma once
#include <vector>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma  comment(lib, "gdiplus.lib")

using namespace std;
class CGifEncoder
{
public:
	CGifEncoder();
	~CGifEncoder();
public:
	//=================================StartEncoder()=================================
	/// @brief 开始gif编码
	///
	/// @param [in] saveFilePath gif图像保存的全路径
	///
	/// @return 成功返回true
	//================================================================================
	bool StartEncoder(wstring &saveFilePath);
	//===================================AddFrame()===================================
	/// @brief 添加图像
	///
	/// @param [in] im  Image对象
	///
	/// @return 成功返回true
	//================================================================================
	bool AddFrame(CImage *pImage);
	bool AddFrame(BITMAPINFO *bitmapinfo, BYTE *pImage);
	//===================================AddFrame()===================================
	/// @brief 添加图像
	///
	/// @param [in] framePath 图像的全路径
	///
	/// @return  成功返回true
	//================================================================================
	bool AddFrame(wstring &framePath);
	//================================FinishEncoder()===============================
	/// @brief 结束gif的编码
	///
	/// @return  成功返回true
	//================================================================================
	bool FinishEncoder();
	//=================================SetDelayTime()=================================
	/// @brief 设置两幅图像切换的时间间隔
	///
	/// @param [in] ms 时间,以毫秒为单位
	///
	/// @return 无
	//================================================================================
	void SetDelayTime(int ms);
	//=================================SetRepeatNum()=================================
	/// @brief 设置gif动画播放的次数
	///
	/// @param [in] num 次数，0表示无限次
	///
	/// @return 无
	//================================================================================
	void SetRepeatNum(int num);
	//=================================SetFrameRate()=================================
	/// @brief 设置图像的帧率
	///
	/// @param [in] fps 帧率，每秒播放图像的数目
	///
	/// @return 无
	//================================================================================
	void SetFrameRate(float fps);
	//=================================SetFrameSize()=================================
	/// @brief 设置图像的尺寸
	///
	/// @param [in] width  图像的宽度
	/// @param [in] height 图像的高度
	///
	/// @return 无
	//================================================================================
	void SetFrameSize(int width, int height);

private:
	void SetImagePropertyItem();
	bool GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

private:
	int                 m_width;
	int                 m_height;
	int                 m_repeatNum;
	int                 m_delayTime;
	bool                m_started;
	bool                m_haveFrame;

	wstring             *m_pStrSavePath;
	Gdiplus::Bitmap     *m_pImage;
	vector<Gdiplus::Bitmap *> m_pBitMapVec;
};
