#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <windows.h>

namespace cvwin {

	HBITMAP IconToBitmap(HICON hIcon);
	HBITMAP CreateBlockBitmap(HBITMAP hbmSrc, int x, int y,
		int nWidth, int nHeight, HDC hDC = NULL, HDC hDCMem = NULL, HDC hDCMem2 = NULL);
	std::vector<uchar> BitmapToPng(HBITMAP hBitmap);
	cv::Mat BitmapToMat(HBITMAP hBitmap);

	std::vector<uchar> IconToPng(HICON hIcon);
	cv::Mat IconToMat(HICON hIcon);

}