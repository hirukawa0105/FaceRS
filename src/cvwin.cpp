#include "cvwin.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <windows.h>
#include <atlimage.h>

HBITMAP cvwin::IconToBitmap(HICON hIcon)
{
	BITMAP bm;
	ICONINFO iconInfo;
	GetIconInfo(hIcon, &iconInfo);
	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);
	HDC hDC = GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, bm.bmWidth, bm.bmHeight);
	HBITMAP hResultBmp = NULL;
	HGDIOBJ hOrgBMP = SelectObject(hMemDC, hMemBmp);
	DrawIconEx(hMemDC, 0, 0, hIcon, bm.bmWidth, bm.bmHeight, 0, NULL, DI_NORMAL);
	hResultBmp = hMemBmp;
	hMemBmp = NULL;
	SelectObject(hMemDC, hOrgBMP);
	DeleteDC(hMemDC);
	ReleaseDC(NULL, hDC);
	return hResultBmp;
}

HBITMAP cvwin::CreateBlockBitmap(HBITMAP hbmSrc, int x, int y,
	int nWidth, int nHeight, HDC hDC, HDC hDCMem, HDC hDCMem2)
{
	HDC _hDC;
	HDC _hDCMem, _hDCMem2;
	HGDIOBJ hgdiBmp, hgdiBmp2;
	HBITMAP hbmRet;

	if (hDC)
		_hDC = hDC;
	else
		_hDC = NULL;                         // スクリーン
	if (hDCMem)
		_hDCMem = hDCMem;
	else
		_hDCMem = CreateCompatibleDC(_hDC);  // 一時的な利用
	if (hDCMem2)
		_hDCMem2 = hDCMem2;
	else
		_hDCMem2 = CreateCompatibleDC(_hDC); // 一時的な利用

	hbmRet = CreateCompatibleBitmap(_hDC, nWidth, nHeight);
	hgdiBmp = SelectObject(_hDCMem, hbmRet);
	hgdiBmp2 = SelectObject(_hDCMem2, hbmSrc);
	BitBlt(_hDCMem, 0, 0, nWidth, nHeight, _hDCMem2, x, y, SRCCOPY);

	// クリーンアップ
	SelectObject(_hDCMem, hgdiBmp);
	SelectObject(_hDCMem2, hgdiBmp2);
	if (!hDCMem)
		DeleteDC(_hDCMem);
	if (!hDCMem2)
		DeleteDC(_hDCMem2);

	return hbmRet;
}

std::vector<uchar> cvwin::BitmapToPng(HBITMAP hBitmap)
{
	std::vector<uchar> png_data;
	CImage cimg;
	cimg.Attach(hBitmap);
	IStream* pIStream = NULL;
	if (CreateStreamOnHGlobal(NULL, TRUE, (LPSTREAM*)&pIStream) != S_OK)
	{
		cimg.Detach();
		DeleteObject(hBitmap);
		return png_data;
	}
	if (cimg.Save(pIStream, Gdiplus::ImageFormatPNG) != S_OK)
	{
		pIStream->Release();
		cimg.Detach();
		DeleteObject(hBitmap);
		return png_data;
	}
	cimg.Detach();
	DeleteObject(hBitmap);
	ULARGE_INTEGER ulnSize;
	LARGE_INTEGER lnOffset;
	lnOffset.QuadPart = 0;
	if (pIStream->Seek(lnOffset, STREAM_SEEK_END, &ulnSize) != S_OK)
	{
		pIStream->Release();
		return png_data;
	}
	if (pIStream->Seek(lnOffset, STREAM_SEEK_SET, NULL) != S_OK)
	{
		pIStream->Release();
		return png_data;
	}
	png_data.resize((unsigned int)ulnSize.QuadPart);
	ULONG ulBytesRead;
	if (pIStream->Read(&png_data[0], (ULONG)ulnSize.QuadPart, &ulBytesRead) != S_OK)
	{
		pIStream->Release();
		return png_data;
	}
	pIStream->Release();
	return png_data;
}

cv::Mat cvwin::BitmapToMat(HBITMAP hBitmap)
{
	std::vector<uchar> png = cvwin::BitmapToPng(hBitmap);
	cv::Mat mat = cv::imdecode(cv::Mat(png), 1);
	return mat;
}

std::vector<uchar> cvwin::IconToPng(HICON hIcon)
{
	HBITMAP hBitmap = cvwin::IconToBitmap(hIcon);
	std::vector<uchar> png = cvwin::BitmapToPng(hBitmap);
	DeleteObject(hBitmap);
	return png;
}

cv::Mat cvwin::IconToMat(HICON hIcon)
{
	HBITMAP hBitmap = cvwin::IconToBitmap(hIcon);
	cv::Mat mat = cvwin::BitmapToMat(hBitmap);
	DeleteObject(hBitmap);
	return mat;
}