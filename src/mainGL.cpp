//#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#include <freeglut.h>
#include "OBJ.h"
#include "mouse.h"
#include "mainGL.h"

#define WIDTH 640
#define HEIGHT 480
#define PI  3.14159265358979323846264338327

MODEL* model;
ViewCamera camera(1.0);

void Keyboard(unsigned char key, int x, int y);
void Special(int key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void PassiveMotion(int x, int y);
void SetLighting();
float VectoScalar(float vec[3]);

void SetAxis(Point3f xAxis, Point3f yAxis, Point3f zAxis);
float VectoScalar(Point3f vec);
float CalcDot(Point3f vec1, Point3f vec2);
Point3f CalcCross(Point3f vec1, Point3f vec2);
float CalcTwoVectorAngle(Point3f vec1, Point3f vec2);
Point3f CalcUnitVec(Point3f vec);

int hoge;
float position[3];
float angle[3];
Point3f rsAngle;//物体の回転角度
Point3f rsTrans;//物体の座標
Point3f rsAngle_;//引き渡し回転角度
Point3f rsAxis[3];//RSから得られた顔の軸方向ベクトル

Point3f glXAxis(1, 0, 0);
Point3f glYAxis(0, 1, 0);
Point3f glZAxis(0, 0, 1);

MainGL::MainGL(){
}

MainGL::~MainGL()
{
}

Point3f::Point3f(float _x, float _y, float _z){
	x = _x;
	y = _y;
	z = _z;
}

Point3f::Point3f(){
	x = 0;
	y = 0;
	z = 0;
}

float DegToRad(float deg){

	return (deg)*(PI / 180.0);
}

void SetDist(float dist)
{
	double zoom = dist;
	zoom += camera.right.current.y / 5.0;

	
	/*angle[0] = camera.angle[0];
	angle[1] = camera.angle[1];
	angle[2] = camera.angle[2];*/
	position[0] = sin(angle[0]) * cos(angle[1]) * zoom;
	position[1] = sin(angle[1]) * zoom;
	position[2] = cos(angle[0]) * cos(angle[1]) * zoom;

	/*cout << "x:" << position[0];
	cout << "y:" << position[1];
	cout << "z:" << position[2] << endl;*/
	//　視点位置の設定
	gluLookAt(
		0.0, 0.0, 1.4,//カメラ位置は固定
		model->centerPoint.x, model->centerPoint.y, model->centerPoint.z,
		0,1,0);

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, WIDTH / HEIGHT, 0.1, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	SetDist(1);

	hoge++;
	glPushMatrix();
	glTranslated(model->centerPoint.x,model->centerPoint.y, model->centerPoint.z);

	if (hoge % 2 == 0){

		//cout << (int)rsAngle.x << " " << (int)rsAngle.y << " " << (int)rsAngle.z << endl;
		//cout << model->centerPoint.x << " " << model->centerPoint.y << " " << model->centerPoint.z << endl;
		/*cout << (int)CalcTwoVectorAngle(rsAxis[0], rsAxis[1]) << " " << (int)CalcTwoVectorAngle(rsAxis[1], rsAxis[2]) << " " 
			<< (int)CalcTwoVectorAngle(rsAxis[0], rsAxis[2]) << endl;*/
		//cout <<"Axis:"<< rsAxis[1].x << " " << rsAxis[1].y << " " << rsAxis[1].z << endl;
	}

	if (hoge % 4 == 0){//1frame毎に回転させると挙動不審な動きをするため間隔をあけて更新

		rsAngle_.x = rsAngle.x;
		rsAngle_.y = rsAngle.y;
		rsAngle_.z = rsAngle.z;
	}
	
	glRotated((int)rsAngle_.z * 2.5, 1, 0, 0);//振幅調整
	glRotated((int)rsAngle_.x, 0, 1, 0);
	glRotated(-(int)rsAngle_.y, 0, 0, 1);
	
	glRotated(-90, 1, 0, 0);//realsenseのobj出力角度の調整

	glTranslated(-model->centerPoint.x, -model->centerPoint.y, -model->centerPoint.z);
	glTranslated(rsTrans.x/1000, rsTrans.y/1000, 0);
	model->Draw();
	glPopMatrix();
	//　補助軸の描画
	glPushMatrix();
	//camera.RenderSubAxis(WIDTH, HEIGHT);
	glPopMatrix();

	glutSwapBuffers();
}
void idle(void)
{
	//angle += 2.0f;
	Sleep(1);
	glutPostRedisplay();
}
void Init(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	angle[0] = DegToRad(45.0);
	angle[1] = DegToRad(45.0);
	angle[2] = 0.0;
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;
	//model = new MODEL("3DScan2.obj");
	model = new MODEL("3DScan1.obj");
}

int MainGL::GLmain()
{
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	//glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("OBJローダ");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(PassiveMotion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	Init();
	glutMainLoop();
	return 0;
}



void MainGL::SetAngle(Point3f xAxis, Point3f yAxis, Point3f zAxis){

	//cout << xAxis.x << " " << xAxis.y << " " << xAxis.z << endl;
	//cout << zAxis.x << " " << zAxis.y << " " << zAxis.z << endl;
	//cout << "-----"<< endl;
	SetAxis(xAxis, yAxis, zAxis);

	//回転方向の正負の決定
	Point3f Cross = CalcCross(glXAxis, rsAxis[0]);
	float CrossAngle = CalcTwoVectorAngle(Cross, glYAxis);
	if (CrossAngle >= 0 && CrossAngle < 90){
		rsAngle.x = -CalcTwoVectorAngle(rsAxis[0], glXAxis);
	}
	else{
		rsAngle.x = CalcTwoVectorAngle(rsAxis[0], glXAxis);
	}

	Cross = CalcCross(glYAxis, rsAxis[1]);
	CrossAngle = CalcTwoVectorAngle(Cross, glZAxis);
	if (CrossAngle >= 0 && CrossAngle < 90){
		rsAngle.y = -CalcTwoVectorAngle(rsAxis[1], glYAxis);
	}
	else{
		rsAngle.y = CalcTwoVectorAngle(rsAxis[1], glYAxis);
	}

	Cross = CalcCross(glZAxis, rsAxis[2]);
	CrossAngle = CalcTwoVectorAngle(Cross, glXAxis);
	if (CrossAngle >= 0 && CrossAngle < 90){
		rsAngle.z = -CalcTwoVectorAngle(rsAxis[2], glZAxis);
	}
	else{
		rsAngle.z = CalcTwoVectorAngle(rsAxis[2], glZAxis);
	}
		
}

void MainGL::SetTrans(Point3f trans){
	rsTrans = trans;
}


void SetAxis(Point3f xAxis, Point3f yAxis,Point3f zAxis){

	xAxis.y = 0;
	yAxis.z = 0;
	zAxis.x = 0;
	Point3f _xAxis = CalcUnitVec(xAxis);
	Point3f _yAxis = CalcUnitVec(yAxis);
	//Point3f _zAxis = CalcCross(xAxis, yAxis);//z軸のブレ補正
	Point3f _zAxis = CalcUnitVec(zAxis);
	

	rsAxis[0] = _xAxis;
	rsAxis[1] = _yAxis;
	rsAxis[2] = _zAxis;

}

//ベクトルをスカラーへ
float VectoScalar(Point3f vec){

	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

//2ベクトルの内積計算
float CalcDot(Point3f vec1, Point3f vec2){

	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

//2ベクトルの外積計算
Point3f CalcCross(Point3f vec1, Point3f vec2){
	float xVec = vec1.y*vec2.z - vec1.z*vec2.y;
	float yVec = vec1.z*vec2.x - vec1.x*vec2.z;
	float zVec = vec1.x*vec2.y - vec1.y*vec2.x;
	return Point3f(xVec, yVec, zVec);
}

//単位ベクトルへ変換
Point3f CalcUnitVec(Point3f vec){

	Point3f temp;
	temp.x = vec.x / VectoScalar(vec);
	temp.y = vec.y / VectoScalar(vec);
	temp.z = vec.z / VectoScalar(vec);
	return temp;
}

//2つのベクトル間の角度を求める(符号なし)
float CalcTwoVectorAngle(Point3f vec1,Point3f vec2){

	float cos= CalcDot(vec1, vec2) / (VectoScalar(vec1)*VectoScalar(vec2));
	float sita = acos(cos);//ラジアンを求める
	sita = sita * 180 / PI;//角度を求める
	return sita;
}


//---------------------------------------------------------------------------------------------------
// Name : Mouse()
// Desc : マウス処理
//---------------------------------------------------------------------------------------------------
void Mouse(int button, int state, int x, int y)
{
	camera.MouseInput(button, state, x, y);
}

//--------------------------------------------------------------------------------------------------
// Name : Motion()
// Desc : マウスドラッグ時
//--------------------------------------------------------------------------------------------------
void Motion(int x, int y)
{
	camera.MouseMotion(x, y);
}

//--------------------------------------------------------------------------------------------------
// Name : PassiveMotion()
// Desc : マウス移動時
//--------------------------------------------------------------------------------------------------
void PassiveMotion(int x, int y)
{
}

//--------------------------------------------------------------------------------------------------
// Name : Keyboard()
// Desc : キーボード処理
//--------------------------------------------------------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '\033':
		exit(0);
		break;

	default:
		break;
	}
}

//--------------------------------------------------------------------------------------------------
// Name : Special()
// Desc : 特殊キー処理
//--------------------------------------------------------------------------------------------------
void Special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		break;

	case GLUT_KEY_F2:
		break;

	case GLUT_KEY_F3:
		break;

	case GLUT_KEY_F4:
		break;

	case GLUT_KEY_F5:
		break;

	case GLUT_KEY_F6:
		break;

	case GLUT_KEY_F7:
		break;

	case GLUT_KEY_F8:
		break;

	case GLUT_KEY_F9:
		break;

	case GLUT_KEY_F10:
		break;

	case GLUT_KEY_F11:
		break;

	case GLUT_KEY_F12:
		break;

	case GLUT_KEY_LEFT:
		break;

	case GLUT_KEY_RIGHT:
		break;

	case GLUT_KEY_UP:
		break;

	case GLUT_KEY_DOWN:
		break;

	case GLUT_KEY_PAGE_UP:
		break;

	case GLUT_KEY_PAGE_DOWN:
		break;

	case GLUT_KEY_HOME:
		break;

	case GLUT_KEY_END:
		break;

	case GLUT_KEY_INSERT:
		break;
	}
}