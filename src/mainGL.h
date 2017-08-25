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
	MainGL();
	~MainGL();

	int GLmain();
	void SetAngle(Point3f xAxis, Point3f yAxis, Point3f zAxis);
	void SetTrans(Point3f trans);
	void SetRightEye(std::vector<Point3f> trans);
	//float CalcDot(Point3f vec1, Point3f vec2);
	//Point3f CalcCross(Point3f vec1, Point3f vec2);
};