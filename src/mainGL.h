#pragma once

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
	void SetRightEye(std::vector<Point3f> &trans, Point3f center);
	void SetLeftEye(std::vector<Point3f> &trans, Point3f center);
	void SetNose(std::vector<Point3f> &trans, Point3f center);
	void SetMouth(std::vector<Point3f> &trans, Point3f center);
	void SetCameraMap(float cameraMap[640][480][3]);
	bool GetCalcDuring();
	//float CalcDot(Point3f vec1, Point3f vec2);
	//Point3f CalcCross(Point3f vec1, Point3f vec2);
};