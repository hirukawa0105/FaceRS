#pragma once

#include <memory>
#include "FaceTrackingRenderer.h"
#include "service/pxcsessionservice.h"
#include <iostream>
#include <vector>
#include "mainGL.h"
//MainGL throwGL;

class Point2{
public:
	Point2(int _x, int _y);
	Point2();
	int x;
	int	y;
};

class FaceTrackingRenderer3D : public FaceTrackingRenderer
{
public:
	FaceTrackingRenderer3D(HWND window, PXCSession* session, MainGL* GL, int outputPanelID, HBITMAP* colorMap);
	virtual ~FaceTrackingRenderer3D();
	void DrawBitmap(PXCCapture::Sample* sample, bool ir);

	HBITMAP* _colorMap;

private:
	bool PointInPolygon(Point2 p, std::vector<Point2> points);
	void DrawGraphics(PXCFaceData* faceOutput);
	void DrawLine(PXCFaceData::Face* trackedFace);
	void DrawLandmark(PXCFaceData::Face* trackedFace);
	bool ProjectVertex(const PXCPoint3DF32 &v, double &x, double &y, int radius = 0);
	void CalcCenterOfMass(PXCFaceData::LandmarkPoint &centerOfMass,PXCFaceData::LandmarkPoint* points);
	void DrawPose(PXCFaceData::Face* trackedFace);
	Point3f CalcCrossRs(Point3f vec1, Point3f vec2);
	Point3f CalcUnitVecRs(Point3f vec);
	Point2 CalcUnitVecRs2D(Point2 vec);
	float VectoScalarRs(Point3f vec);
	float VectoScalarRs2D(Point2 vec);
	Point2 CalcGravity(std::vector<Point2> points);

	PXCSession* m_session;
	PXCImage::ImageInfo m_outputImageInfo;
	PXCImage* m_outputImage;
	PXCImage::ImageData m_outputImageData;

	std::vector<Point2> leftEyeList;
	std::vector<Point2> rightEyeList;
	std::vector<Point2> noseList;
	std::vector<Point2> mouthList;

	std::vector<Point3f> rightEyeDepth;
	std::vector<cv::Point2i> rightEyeColor;
	Point3f rightEyeCenter;
	std::vector<Point3f> leftEyeDepth;
	std::vector<cv::Point2i> leftEyeColor;
	Point3f leftEyeCenter;
	std::vector<Point3f> noseDepth;
	std::vector<cv::Point2i> noseColor;
	Point3f noseCenter;
	std::vector<Point3f> mouthDepth;
	std::vector<cv::Point2i> mouthColor;
	Point3f mouthCenter;

	MainGL throwGL;

};

