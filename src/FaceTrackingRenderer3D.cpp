#include "FaceTrackingRenderer3D.h"
#include "FaceTrackingUtilities.h"
#include "pxcprojection.h"
#include <iostream>
#include <vector>

using namespace std;

Point2::Point2(int _x, int _y){
	x = _x;
	y = _y;
}

Point2::Point2(){
	x = 0;
	y = 0;
}

inline int my_round(double x) {
	return int(floor(x+0.5)+0.01);
}

void DrawPoint(HDC dc, COLORREF color, int x, int y, int rad)
{
	HBRUSH hBrush = CreateSolidBrush(color);
	if (hBrush == NULL)
	{
		return;
	}
	HBRUSH hOld = (HBRUSH)SelectObject(dc, hBrush);
	Ellipse(dc, x-rad, y-rad, x+rad, y+rad);
	SelectObject(dc, hOld);
	DeleteObject(hBrush);
}

FaceTrackingRenderer3D::~FaceTrackingRenderer3D()
{
}

FaceTrackingRenderer3D::FaceTrackingRenderer3D(HWND window, PXCSession* session, MainGL* GL) : FaceTrackingRenderer(window), m_session(session)
{
	throwGL = *GL;
	//目や鼻のリストにに空ポイントを挿入
	for (int i = 0; i < 8; i++){
		leftEyeList.push_back(Point2(0, 0));
		rightEyeList.push_back(Point2(0, 0));
	}
	for (int i = 0; i < 3; i++){
		noseList.push_back(Point2(0, 0));
	}
	for (int i = 0; i < 12; i++){
		mouthList.push_back(Point2(0, 0));
	}
}

bool FaceTrackingRenderer3D::ProjectVertex(const PXCPoint3DF32 &v, double &x, double &y, int radius)
{
	x = m_outputImageInfo.width  * (0.5 + 0.001 * v.x);
	y = m_outputImageInfo.height * (0.5 - 0.001 * v.y);

	return ((radius <= my_round(x)) && (my_round(x) < m_outputImageInfo.width-radius) && (radius <= my_round(y)) && (my_round(y) < m_outputImageInfo.height-radius));
}

void FaceTrackingRenderer3D::CalcCenterOfMass(PXCFaceData::LandmarkPoint &centerOfMass,PXCFaceData::LandmarkPoint* points)
{
	centerOfMass.world.x = 0.0;
	centerOfMass.world.y = 0.0;
	centerOfMass.world.z = 0.0;

	int numStartPointsTodevied = 62;
	for (int j=0; j<78; j++)
	{
		if (j < 53 || j >69 ||j  == 61) 
		{			
			if (points[j].confidenceWorld > 0)
			{
				centerOfMass.world.x += points[j].world.x;
				centerOfMass.world.y += points[j].world.y;
				centerOfMass.world.z += points[j].world.z;
			}
			else
			{
				numStartPointsTodevied--;
			}
		}
	}

	centerOfMass.world.x /= numStartPointsTodevied;
	centerOfMass.world.y /= numStartPointsTodevied;
	centerOfMass.world.z /= numStartPointsTodevied;
}

void FaceTrackingRenderer3D::DrawGraphics(PXCFaceData* faceOutput)
{
	assert(faceOutput != NULL);
	if (!m_bitmap) return;

	const int numFaces = faceOutput->QueryNumberOfDetectedFaces();
	for (int i = 0; i < numFaces; ++i) 
	{
		PXCFaceData::Face* trackedFace = faceOutput->QueryFaceByIndex(i);		
		assert(trackedFace != NULL);
		if (FaceTrackingUtilities::IsModuleSelected(m_window, IDC_LANDMARK) && trackedFace->QueryLandmarks() != NULL) 
			DrawLandmark(trackedFace);
		if (FaceTrackingUtilities::IsModuleSelected(m_window, IDC_POSE))
			DrawPose(trackedFace);
		if (FaceTrackingUtilities::IsModuleSelected(m_window, IDC_LINE) && trackedFace->QueryLandmarks() != NULL)
			DrawLine(trackedFace);
	}
}

bool FaceTrackingRenderer3D::PointInPolygon(Point2 v, std::vector<Point2> points) {
	Point2 p1, p2;
	bool inside = false;
	Point2 oldPoint = points[points.size() - 1];
	if (points.at(0).x!=0){
		int fafa = 0;

	}
	for (int i = 0; i < points.size(); i++) {
		Point2 newPoint = points[i];
		if (newPoint.x > oldPoint.x) { p1 = oldPoint; p2 = newPoint; }
		else { p1 = newPoint; p2 = oldPoint; }
		if ((p1.x<v.x) == (v.x <= p2.x) && (v.y - p1.y)*(p2.x - p1.x) < (p2.y - p1.y)*(v.x - p1.x)) {
			inside = !inside;
		}
		oldPoint = newPoint;
	}
	return inside;
}

void FaceTrackingRenderer3D::DrawBitmap(PXCCapture::Sample* sample, bool ir)
{
	PXCImage *imageDepth = sample->depth;
	assert(imageDepth);
	PXCImage::ImageInfo imageDepthInfo = imageDepth->QueryInfo();

	m_outputImageInfo.width = 1024;
	m_outputImageInfo.height = 1024;
	m_outputImageInfo.format = PXCImage::PIXEL_FORMAT_RGB32;
	m_outputImageInfo.reserved = 0;

	m_outputImage = m_session->CreateImage(&m_outputImageInfo);
	assert(m_outputImage);

	PXCImage::ImageData imageDepthData;
	if (imageDepth->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &imageDepthData) >= PXC_STATUS_NO_ERROR)
	{
		memset(&m_outputImageData, 0, sizeof(m_outputImageData));
		pxcStatus status = m_outputImage->AcquireAccess(PXCImage::ACCESS_WRITE, PXCImage::PIXEL_FORMAT_RGB32, &m_outputImageData);
		if (status < PXC_STATUS_NO_ERROR) return;

		int stridePixels = m_outputImageData.pitches[0];
		pxcBYTE *pixels = reinterpret_cast<pxcBYTE*> (m_outputImageData.planes[0]);
		memset(pixels, 0, stridePixels * m_outputImageInfo.height);

		// get access to depth data
		PXCPoint3DF32* vertices = new PXCPoint3DF32[imageDepthInfo.width * imageDepthInfo.height];
		PXCProjection* projection(m_senseManager->QueryCaptureManager()->QueryDevice()->CreateProjection());
		if (!projection)
		{
			if (vertices) delete[] vertices;
			return;
		}

		projection->QueryVertices(imageDepth, vertices);
		projection->Release();
		int strideVertices = imageDepthInfo.width;

		// render vertices
		int numVertices = 0;

		//一番近い＆遠いレンジを検索
		int highZ=0;
		int lowZ = 0;


		/*std::vector<Point2> temp;
		temp.push_back(Point2(400, 500));
		temp.push_back(Point2(450, 500));
		temp.push_back(Point2(420, 450));
		temp.push_back(Point2(460, 400));
		bool test=PointInPolygon(Point2(530, 470), temp);
		cout << test << endl;*/

		for (int y = 0; y < imageDepthInfo.height; y++)
		{
			const PXCPoint3DF32 *verticesRow = vertices + y * strideVertices;
			for (int x = 0; x < imageDepthInfo.width; x++)
			{
				const PXCPoint3DF32 &v = verticesRow[x];

				
				if (y == 0 && x == 0){
					highZ = v.z;
					lowZ = 10000;
				}

				if (v.z <= 0.0f)
				{
					continue;
				}

				//一番遠い点を探索
				if (v.z > highZ){
					highZ = v.z;
				}
				//一番近い点を探索
				if (v.z<lowZ){
					lowZ = v.z;
				}
				

				double ix = 0, iy = 0;
				if (ProjectVertex(v, ix, iy))
				{
					pxcBYTE *ptr = m_outputImageData.planes[0];
					ptr += my_round(iy) * m_outputImageData.pitches[0];
					ptr += my_round(ix) * 4;
					/*ptr[0] = pxcBYTE(255.0f * 0.5f);
					ptr[1] = pxcBYTE(255.0f * 0.5f);
					ptr[2] = pxcBYTE(255.0f * 0.5f);*/

					double depth;

					//depth範囲は約50～700程、3で割れば255内に収まる
					if (PointInPolygon(Point2(ix, iy), rightEyeList) || PointInPolygon(Point2(ix, iy), leftEyeList)||
						PointInPolygon(Point2(ix, iy), noseList) || PointInPolygon(Point2(ix, iy), mouthList)){
						depth =v.z / 3;
					}
					else{
						depth = 0;
					}
					ptr[0] = pxcBYTE(-depth);
					ptr[1] = pxcBYTE(-depth);
					ptr[2] = pxcBYTE(-depth);
					ptr[3] = pxcBYTE(255.0f);
				}

				numVertices++;
			}
		}

		if (vertices) delete[] vertices;

		if (m_bitmap)
		{
			DeleteObject(m_bitmap);
			m_bitmap = 0;
		}

		HWND hwndPanel = GetDlgItem(m_window, IDC_PANEL);
		HDC dc = GetDC(hwndPanel);
		BITMAPINFO binfo;
		memset(&binfo, 0, sizeof(binfo));
		binfo.bmiHeader.biWidth = m_outputImageData.pitches[0] / 4;
		binfo.bmiHeader.biHeight = -(int)m_outputImageInfo.height;
		binfo.bmiHeader.biBitCount = 32;
		binfo.bmiHeader.biPlanes = 1;
		binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		binfo.bmiHeader.biCompression = BI_RGB;
		Sleep(1);
		m_bitmap = CreateDIBitmap(dc, &binfo.bmiHeader, CBM_INIT, m_outputImageData.planes[0], &binfo, DIB_RGB_COLORS);

		ReleaseDC(hwndPanel, dc);

		m_outputImage->ReleaseAccess(&m_outputImageData);
		imageDepth->ReleaseAccess(&imageDepthData);
		m_outputImage->Release();
	}
}

//輪郭線描画
void FaceTrackingRenderer3D::DrawLine(PXCFaceData::Face* trackedFace){
	const PXCFaceData::LandmarksData *landmarkData = trackedFace->QueryLandmarks();

	if (!landmarkData)
		return;

	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);
	HDC dc2 = CreateCompatibleDC(dc1);

	if (!dc2)
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	HFONT hFont = CreateFont(16, 8, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"MONOSPACE");

	if (!hFont)
	{
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}

	SetBkMode(dc2, TRANSPARENT);

	SelectObject(dc2, m_bitmap);
	SelectObject(dc2, hFont);

	BITMAP bitmap;
	GetObject(m_bitmap, sizeof(bitmap), &bitmap);

	pxcI32 numPoints = landmarkData->QueryNumPoints();
	if (numPoints != m_numLandmarks)
	{
		DeleteObject(hFont);
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}
	
	PXCFaceData::LandmarkPoint* points = new PXCFaceData::LandmarkPoint[numPoints];
	
	for (int l = 0; l < numPoints; l++) //initialize Array
	{
		points[l].world.x = 0.0;
		points[l].world.y = 0.0;
		points[l].world.z = 0.0;
	}

	landmarkData->QueryPoints(points); //data set for all landmarks in frame

	//convert depth data is to millimeters
	for (int l = 0; l < numPoints; l++)
	{
		points[l].world.x *= 1000.0f;
		points[l].world.y *= 1000.0f;
		points[l].world.z *= 1000.0f;
	}

	for (int l = 0; l < numPoints; l++) //initialize Array
	{
		points[l].world.x = 0.0;
		points[l].world.y = 0.0;
		points[l].world.z = 0.0;
	}

	HPEN cyan = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	if (!cyan)
	{
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}
	SelectObject(dc2, cyan);

	landmarkData->QueryPoints(points); //data set for all landmarks in frame

	//描画線ループ用
	int _x;
	int _y;

	//convert depth data is to millimeters
	for (int l = 0; l < numPoints; l++)
	{
		points[l].world.x *= 1000.0f;
		points[l].world.y *= 1000.0f;
		points[l].world.z *= 1000.0f;
	}

	for (int i = 0; i < numPoints; i++)
	{
		double ix = 0, iy = 0;

		if (ProjectVertex(points[i].world, ix, iy, 1))
		{
			int x = ix;
			int y = iy;

			//右目の描画
			if (i >= 10 && i <= 17){
				/*if (i == 10){
					MoveToEx(dc2, x , y , 0);
					_x = x;
					_y = y;
				}
				else{
					LineTo(dc2, x, y);
				}
				if (i == 17){
					LineTo(dc2, _x, _y);
				}
*/
				//点群追加
				rightEyeList.at(i-10) = Point2(x, y);
			}
			//左目の描画
			if (i >= 18 && i <= 25){
				/*if (i == 18){
					MoveToEx(dc2, x, y, 0);
					_x = x ;
					_y = y ;
				}
				else{
					LineTo(dc2, x , y );
				}
				if (i == 25){
					LineTo(dc2, _x, _y);
				}*/
				leftEyeList.at(i - 18) = Point2(x, y);
			}
			//鼻の描画
			if (i >= 26 && i <= 32){
				if (i == 26){
					/*MoveToEx(dc2, x , y , 0);
					_x = x ;
					_y = y ;*/
					noseList.at(0) = Point2(x, y);
				}
				else if (i == 30){
					//LineTo(dc2, x , y );
					noseList.at(1) = Point2(x, y);
				}
				if (i == 32){
					//LineTo(dc2, x, y);
					//LineTo(dc2, _x, _y);
					noseList.at(2) = Point2(x, y);
				}
				
			}

			//口の描画
			if (i >= 33 && i <= 44){
				/*if (i == 33){
					MoveToEx(dc2, x , y , 0);
					_x = x ;
					_y = y ;
				}
				else{
					LineTo(dc2, x , y );
				}
				if (i == 44){
					LineTo(dc2, _x, _y);
				}*/
				mouthList.at(i - 33) = Point2(x, y);
			}
		}
	}

	if (points) delete[] points;

	DeleteObject(cyan);
	DeleteObject(hFont);
	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);

}

//顔輪郭などの点描画
void FaceTrackingRenderer3D::DrawLandmark(PXCFaceData::Face* trackedFace)
{
	const PXCFaceData::LandmarksData *landmarkData = trackedFace->QueryLandmarks();

	if (!landmarkData)
		return;

	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);
	HDC dc2 = CreateCompatibleDC(dc1);

	if (!dc2) 
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	HFONT hFont = CreateFont(16, 8, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"MONOSPACE");

	if (!hFont)
	{
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}

	SetBkMode(dc2, TRANSPARENT);

	SelectObject(dc2, m_bitmap);
	SelectObject(dc2, hFont);

	BITMAP bitmap;
	GetObject(m_bitmap, sizeof(bitmap), &bitmap);

	pxcI32 numPoints = landmarkData->QueryNumPoints();
	if (numPoints != m_numLandmarks)
	{
		DeleteObject(hFont);
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}

	PXCFaceData::LandmarkPoint* points = new PXCFaceData::LandmarkPoint[numPoints];

	for (int l = 0; l < numPoints; l++) //initialize Array
	{
		points[l].world.x = 0.0;
		points[l].world.y = 0.0;
		points[l].world.z = 0.0;
	}

	landmarkData->QueryPoints(points); //data set for all landmarks in frame

	//convert depth data is to millimeters
	for (int l = 0; l < numPoints; l++)
	{
		points[l].world.x *= 1000.0f;
		points[l].world.y *= 1000.0f;
		points[l].world.z *= 1000.0f;
	}

	for (int j = 0; j < numPoints; j++)
	{
		double ix = 0, iy = 0;

		if(ProjectVertex(points[j].world, ix, iy, 1))
		{
			if (points[j].confidenceWorld > 0)
			{
				DrawPoint(dc2, RGB(255, 255, 0), my_round(ix), my_round(iy), 3);
			}
			else
			{
				DrawPoint(dc2, RGB(255, 0, 0), my_round(ix), my_round(iy), 3);
			}
		}
	}
		
	if (points) delete[] points;

	DeleteObject(hFont);
	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);
}

//顔のベクトル
void FaceTrackingRenderer3D::DrawPose(PXCFaceData::Face* trackedFace)
{
	HWND panelWindow = GetDlgItem(m_window, IDC_PANEL);
	HDC dc1 = GetDC(panelWindow);
	HDC dc2 = CreateCompatibleDC(dc1);

	if (!dc2) 
	{
		ReleaseDC(panelWindow, dc1);
		return;
	}

	HFONT hFont = CreateFont(28, 18, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"MONOSPACE");

	if (!hFont)
	{
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}

	SetBkMode(dc2, TRANSPARENT);

	SelectObject(dc2, m_bitmap);
	SelectObject(dc2, hFont);

	BITMAP bitmap;
	GetObject(m_bitmap, sizeof(bitmap), &bitmap);

	const PXCFaceData::PoseData *poseData = trackedFace->QueryPose();

	if(poseData == NULL)
	{
		DeleteObject(hFont);
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}
	PXCFaceData::HeadPosition outFaceCenterPoint;
	poseData->QueryHeadPosition(&outFaceCenterPoint);
	if (outFaceCenterPoint.confidence == 0)
	{
		DeleteObject(hFont);
		DeleteDC(dc2);
		ReleaseDC(panelWindow, dc1);
		return;
	}

	double headCenter_x = 0, headCenter_y = 0;	
	if(ProjectVertex(outFaceCenterPoint.headCenter, headCenter_x, headCenter_y, 2))
	{
		if(poseData->QueryConfidence() > 0 && outFaceCenterPoint.confidence > 0)
		{
			DrawPoint(dc2, RGB(0, 0, 255), my_round(headCenter_x), my_round(headCenter_y), 8);
		}
	
		const PXCFaceData::LandmarksData *landmarkData = trackedFace->QueryLandmarks();

		if (!landmarkData)
		{
			DeleteObject(hFont);
			DeleteDC(dc2);
			ReleaseDC(panelWindow, dc1);
			return;
		}

		PXCFaceData::LandmarkPoint* points = new PXCFaceData::LandmarkPoint[landmarkData->QueryNumPoints()];
		landmarkData->QueryPoints(points); //data set for all landmarks in frame

		points[29].world.x *= 1000.0f;
		points[29].world.y *= 1000.0f;
		points[29].world.z *= 1000.0f;

		points[10].world.x *= 1000.0f;
		points[10].world.y *= 1000.0f;
		points[10].world.z *= 1000.0f;

		points[18].world.x *= 1000.0f;
		points[18].world.y *= 1000.0f;
		points[18].world.z *= 1000.0f;

		//顔のz軸方向のベクトルを顔の中心から鼻の先端までのベクトルとする
		//Point3f faceVecZ(points[29].world.x - outFaceCenterPoint.headCenter.x, points[29].world.y - outFaceCenterPoint.headCenter.y,points[29].world.z - outFaceCenterPoint.headCenter.z );
		Point3f faceVecZ(outFaceCenterPoint.headCenter.x - points[29].world.x , outFaceCenterPoint.headCenter.y - points[29].world.y, outFaceCenterPoint.headCenter.z - points[29].world.z);

		//顔のx軸方向のベクトルを目の端間のベクトルする
		//Point3f faceVecX(points[10].world.x - points[18].world.x, points[10].world.y - points[18].world.y,points[10].world.z - points[18].world.z);
		Point3f faceVecX(points[18].world.x - points[10].world.x, points[18].world.y - points[10].world.y, points[18].world.z - points[10].world.z);

		//Point3f faceVecY(points[12].world.x - points[16].world.x, points[12].world.y - points[16].world.y, points[12].world.z - points[16].world.z);

		//顔のy軸方向のベクトルをz,xベクトルの外積とする
		Point3f tempX = CalcUnitVecRs(faceVecX);
		Point3f tempZ = CalcUnitVecRs(faceVecZ);
		Point3f faceVecY=CalcCrossRs(faceVecZ, faceVecX);
		Point3f tempY = CalcUnitVecRs(faceVecY);

		//角度設定
		throwGL.SetAngle(tempX, tempY, tempZ);
		
		//座標設定
		Point3f trans(outFaceCenterPoint.headCenter.x, outFaceCenterPoint.headCenter.y, outFaceCenterPoint.headCenter.z);
		throwGL.SetTrans(trans);

		PXCPoint3DF32 axisXpoint;
		axisXpoint.x = outFaceCenterPoint.headCenter.x + tempX.x * 100;
		axisXpoint.y = outFaceCenterPoint.headCenter.y + tempX.y * 100;
		axisXpoint.z = outFaceCenterPoint.headCenter.z + tempX.z * 100;

		PXCPoint3DF32 axisYpoint;
		axisYpoint.x = outFaceCenterPoint.headCenter.x + tempY.x * 100;
		axisYpoint.y = outFaceCenterPoint.headCenter.y + tempY.y * 100;
		axisYpoint.z = outFaceCenterPoint.headCenter.z + tempY.z * 100;

		PXCPoint3DF32 axisZpoint;
		axisZpoint.x = outFaceCenterPoint.headCenter.x + tempZ.x * 100;
		axisZpoint.y = outFaceCenterPoint.headCenter.y + tempZ.y * 100;
		axisZpoint.z = outFaceCenterPoint.headCenter.z + tempZ.z * 100;

		//throwGL.SetAngle(glVecX, glVecY, glVecZ);

		double noseTip_x = 0, noseTip_y = 0;

		//z軸描画
		if(ProjectVertex(points[29].world, noseTip_x, noseTip_y, 1))
		{
			PXCPoint3DF32 direction;
			direction.x = (float)(noseTip_x - headCenter_x);
			direction.y = (float)(noseTip_y - headCenter_y);
			
			HPEN lineColor;
			
			if (poseData->QueryConfidence() > 0)
			{
				lineColor = CreatePen(PS_SOLID, 3, RGB(255 ,0 ,255));
			}
			else
			{
				lineColor = CreatePen(PS_SOLID, 3, RGB(255 ,0 , 0));
			}

			if (!lineColor)
			{
				DeleteObject(hFont);
				DeleteDC(dc2);
				ReleaseDC(panelWindow, dc1);
				return;
			}
			SelectObject(dc2, lineColor);

			MoveToEx(dc2, my_round(headCenter_x), my_round(headCenter_y), 0);
			LineTo(dc2, my_round(headCenter_x + 1.2 * direction.x), my_round(headCenter_y + 1.2 * direction.y));

			DeleteObject(lineColor);
		}

		double xAxis_x = 0, xAxis_y = 0;
		ProjectVertex(axisXpoint, xAxis_x, xAxis_y, 1);
		//x軸描画
		if (ProjectVertex(points[29].world, noseTip_x, noseTip_y, 1))
		{
			PXCPoint3DF32 direction;
			direction.x = (float)(xAxis_x - headCenter_x);
			direction.y = (float)(xAxis_y - headCenter_y);

			HPEN lineColor;

			if (poseData->QueryConfidence() > 0)
			{
				lineColor = CreatePen(PS_SOLID, 3, RGB(0, 255, 255));
			}
			else
			{
				lineColor = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
			}

			if (!lineColor)
			{
				DeleteObject(hFont);
				DeleteDC(dc2);
				ReleaseDC(panelWindow, dc1);
				return;
			}
			SelectObject(dc2, lineColor);

			MoveToEx(dc2, my_round(headCenter_x), my_round(headCenter_y), 0);
			LineTo(dc2, my_round(headCenter_x + 1.2 * direction.x), my_round(headCenter_y + 1.2 * direction.y));


			DeleteObject(lineColor);
		}

		double yAxis_x = 0, yAxis_y = 0;
		ProjectVertex(axisYpoint, yAxis_x, yAxis_y, 1);
		//y軸描画
		if (ProjectVertex(points[29].world, noseTip_x, noseTip_y, 1))
		{
			PXCPoint3DF32 direction;
			direction.x = (float)(yAxis_x - headCenter_x);
			direction.y = (float)(yAxis_y - headCenter_y);

			HPEN lineColor;

			if (poseData->QueryConfidence() > 0)
			{
				lineColor = CreatePen(PS_SOLID, 3, RGB(0, 255, 255));
			}
			else
			{
				lineColor = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
			}

			if (!lineColor)
			{
				DeleteObject(hFont);
				DeleteDC(dc2);
				ReleaseDC(panelWindow, dc1);
				return;
			}
			SelectObject(dc2, lineColor);

			MoveToEx(dc2, my_round(headCenter_x), my_round(headCenter_y), 0);
			LineTo(dc2, my_round(headCenter_x + 1.2 * direction.x), my_round(headCenter_y + 1.2 * direction.y));
			
			DeleteObject(lineColor);
		}
	}
	
	DeleteObject(hFont);
	DeleteDC(dc2);
	ReleaseDC(panelWindow, dc1);
}

Point3f FaceTrackingRenderer3D::CalcCrossRs(Point3f vec1, Point3f vec2){
	float xVec = vec1.y*vec2.z - vec1.z*vec2.y;
	float yVec = vec1.z*vec2.x - vec1.x*vec2.z;
	float zVec = vec1.x*vec2.y - vec1.y*vec2.x;
	return Point3f(xVec, yVec, zVec);
}

//ベクトルをスカラーへ
float FaceTrackingRenderer3D::VectoScalarRs(Point3f vec){

	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

//単位ベクトルへ変換
Point3f FaceTrackingRenderer3D::CalcUnitVecRs(Point3f vec){

	Point3f temp;
	temp.x = vec.x / VectoScalarRs(vec);
	temp.y = vec.y / VectoScalarRs(vec);
	temp.z = vec.z / VectoScalarRs(vec);
	return temp;
}

