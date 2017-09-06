#pragma once

#include "FaceTrackingRenderer.h"
#include <vector>
#include "mainGL.h"

class FaceTrackingRenderer2D : public FaceTrackingRenderer
{

#define WIDTH 640
#define HEIGHT 480
public:
	FaceTrackingRenderer2D(HWND window, MainGL* GL, int outputPanelID, HBITMAP* colorMap);
	virtual ~FaceTrackingRenderer2D();

	void DrawBitmap(PXCCapture::Sample* sample, bool ir);
	void CalcDistances();
	void Reset();
	void SetActivateEyeCenterCalculations(bool bValue) {bActivateEyeCenterCalculations = bValue;}

	HBITMAP _colorMap;

protected:
	static const int sizeData    = 100;
	static const int sizeSample  =  10;

private:
	void DrawDistances();
	void DrawGraphics(PXCFaceData* faceOutput);
	void DrawLine(PXCFaceData::Face* trackedFace);
	void DrawLandmark(PXCFaceData::Face* trackedFace);
	void DrawLocation(PXCFaceData::Face* trackedFace);
	void DrawPoseAndPulse(PXCFaceData::Face* trackedFace, const int faceId);
	void DrawExpressions(PXCFaceData::Face* trackedFace, const int faceId);		
	void DrawRecognition(PXCFaceData::Face* trackedFace, const int faceId);

	template <int dim> double CalculateDistance(PXCPoint3DF32 p1, PXCPoint3DF32 p2);

	double currHeadWidthAvg;
	double currNoseBridgeAvg;
	double currEyesCenterAvg;
	double currEyesCenterSqrAvg;

	double arrEyesCenter[sizeData];
	double arrEyesCenterAveSample   [sizeSample];
	double arrEyesCenterSqrAveSample[sizeSample];

	double headWidthAvg;
	double noseBridgeAvg;
	double eyesCenterAvg;
	double eyesCenterSqrAvg;
	double eyesCenterSTD;

	int frameNum;
	int fn; //sample frame number (this starts from when frameNum == sizeData 

	MainGL throwGL;
	bool bActivateEyeCenterCalculations;
	float cameraMap[WIDTH][HEIGHT][3];

};

