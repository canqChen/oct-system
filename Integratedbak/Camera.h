// 相机公共函数
#include <afxcmn.h>
#include <queue>
#include <vector>
#include <io.h>
#include <share.h>
#include <Fcntl.h>
#include <sys/stat.h>
#include <windowsx.h>
#include <math.h>

// 设置BitMap的参数
// 图像宽、图像高、图像文件头
void SetBitmapInfoMono(UINT Width, UINT Height, LPBITMAPINFO _lpBitsInfo);