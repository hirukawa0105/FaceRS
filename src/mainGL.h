#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//3次元点クラス
class Point3f{
public:
	Point3f(float _x, float _y, float _z);
	Point3f();
	float x;
	float y;
	float z;
};



//メインクラス
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
	void SetRightEye(std::vector<Point3f> &trans, Point3f center, std::vector<cv::Point2i> &color);
	void SetLeftEye(std::vector<Point3f> &trans, Point3f center, std::vector<cv::Point2i> &color);
	void SetNose(std::vector<Point3f> &trans, Point3f center, std::vector<cv::Point2i> &color);
	void SetMouth(std::vector<Point3f> &trans, Point3f center, std::vector<cv::Point2i> &color);
	void SetCameraMap(cv::Mat cameraMap_);
	bool GetCalcDuring();
	//float CalcDot(Point3f vec1, Point3f vec2);
	//Point3f CalcCross(Point3f vec1, Point3f vec2);
};