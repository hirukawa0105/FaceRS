#pragma once

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
	MainGL();
	~MainGL();

	int GLmain();
	bool enableCalc=true;
	void SetAngle(Point3f xAxis, Point3f yAxis, Point3f zAxis);
	void SetTrans(Point3f trans);
	void SetRightEye(std::vector<Point3f> &trans);
	bool GetCalcDuring();
	//float CalcDot(Point3f vec1, Point3f vec2);
	//Point3f CalcCross(Point3f vec1, Point3f vec2);
};