#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//3�����_�N���X
class Point3f{
public:
	Point3f(float _x, float _y, float _z);
	Point3f();
	float x;
	float y;
	float z;
};



//���C���N���X
class MainGL
{
public:

#define WIDTH 640
#define HEIGHT 480
	MainGL();
	~MainGL();

	int GLmain();
	bool enableCalc=true;
	void SetAngle(Point3f xAxis, Point3f yAxis, Point3f zAxis);
	void SetTrans(Point3f trans);
	void SetRightEye(std::vector<Point3f> &trans, Point3f center);
	void SetLeftEye(std::vector<Point3f> &trans, Point3f center);
	void SetNose(std::vector<Point3f> &trans, Point3f center);
	void SetMouth(std::vector<Point3f> &trans, Point3f center);
	void SetCameraMap(cv::Mat cameraMap_);
	bool GetCalcDuring();
	//float CalcDot(Point3f vec1, Point3f vec2);
	//Point3f CalcCross(Point3f vec1, Point3f vec2);
};