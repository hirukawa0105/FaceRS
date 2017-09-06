//#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#include <freeglut.h>
#include "OBJ.h"
#include "mouse.h"
#include "mainGL.h"

// �E�B���h�E�֘A�̏���
#include "Window.h"

#define WIDTH 640
#define HEIGHT 480
#define PI  3.14159265358979323846264338327

MODEL* model;
MODEL* rightEye;
MODEL* leftEye;
MODEL* nose;
MODEL* mouth;

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

int hoge=0;
float position[3];
float angle[3];
Point3f rsAngle;//���̂̉�]�p�x
Point3f rsTrans;//���̂̍��W
Point3f rsAngle_;//�����n����]�p�x
Point3f rsAxis[3];//RS���瓾��ꂽ��̎������x�N�g��

Point3f glXAxis(1, 0, 0);
Point3f glYAxis(0, 1, 0);
Point3f glZAxis(0, 0, 1);

//RealSense�̃J�����摜
cv::Mat cameraMap = cv::Mat::zeros(cv::Size(160, 100), CV_8U);

// ���b�V���̗񐔂ƍs��
const auto slices(16), stacks(12);

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
	//�@���_�ʒu�̐ݒ�
	


	//gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

}

void drawPoint(void){


}

//���_�f�[�^ 3�ʂ�4���_�ō쐬���� 1���_��x,y,z��3�v�f
GLfloat Vertex[3][4][3] = {
	{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.5f }, { 0.0f, 1.0f, 0.5f }, { 0.0f, 1.0f, 0.0f } },//1����
	{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.5f }, { -1.0f, 0.0f, 0.5f }, { -1.0f, 0.0f, 0.0f } },//2����
	{ { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.5f }, { 0.0f, 1.0f, 0.5f }, { 0.0f, 1.0f, 0.0f } },//3����
};
GLfloat Normal[6][4][3] = {
	{ { 1.0f, 0.5f, 0.0f }, { 1.0f, 0.5f, 0.0f }, { 1.0f, 0.5f, 0.0f }, { 1.0f, 0.5f, 0.0f } },//�E��
	{ { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f } },//��
	{ { -1.0f, 0.5f, 0.0f }, { -1.0f, 0.5f, 0.0f }, { -1.0f, 0.5f, 0.0f }, { -1.0f, 0.5f, 0.0f } },//����
};
GLfloat Color[6][4][3] = {
	{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },//��
	{ { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },//��
	{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },//��
};

GLfloat Vertex2[3] = { 0, 0, 0 };

void DrawArray(void)
{
	//�L����
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	//�f�[�^�̊֘A�t��
	glVertexPointer(3, GL_FLOAT, 0, Vertex);//���W
	glNormalPointer(GL_FLOAT, 0, Normal);//�@��
	glColorPointer(3, GL_FLOAT, 0, Color);//�F

	glDrawArrays(GL_TRIANGLES, 0, 4 * 3); //�`��(4���_*3��)

	//������
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glEnableClientState(GL_VERTEX_ARRAY);
	std::vector<Point3f> temp;
	float a = -0.1f;
	for (int i = 0; i < 100; ++i){

		a += 0.1f;
		GLfloat Vertex2[3] = { a, a, a };

		glVertexPointer(3, GL_FLOAT, 0, Vertex2);//���W
		glDrawArrays(GL_POINTS, 0, 1);
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);


}
void display2(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, WIDTH / HEIGHT, 0.1, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//hoge += 0.1;
	//glRotatef(hoge, 1.0f, 0.0f, 0.0f);//��]
	//glRotatef(hoge, 0.0f, 1.0f, 0.0f);//��]

	//DrawArray();
	model->Draw();
	rightEye->RealSenseDraw();
	glutSwapBuffers();
}

void drawAxis(){

	glPushMatrix();
	glTranslated(-0.3, 0, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0.1, 0, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0.1, 0, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0.1, 0, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0.1, 0, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0.1, 0, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0.1, 0, 0);
	glutSolidSphere(0.005, 8, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0.1, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0, 0.1, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0, 0.1, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0, 0.1, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0, 0.1, 0);
	glutSolidSphere(0.005, 8, 8);
	glTranslated(0, 0.1, 0);
	glutSolidSphere(0.005, 8, 8);
	glPopMatrix();
}

void drawMinMaxPoint(){
	//�ŏ��_�A�ő�_
	glPushMatrix();
	glTranslated(model->minXPoint, model->centerPoint.y, model->centerPoint.z);
	glutSolidSphere(0.01, 8, 8);
	glPopMatrix();
	glPushMatrix();
	glTranslated(model->maxXPoint, model->centerPoint.y, model->centerPoint.z);
	glutSolidSphere(0.01, 8, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(model->centerPoint.x, model->centerPoint.y, model->minZPoint);
	glutSolidSphere(0.01, 8, 8);
	glPopMatrix();
	glPushMatrix();
	glTranslated(model->centerPoint.x, model->centerPoint.y, model->maxZPoint);
	glutSolidSphere(0.01, 8, 8);
	glPopMatrix();
}


void drawFaceTest(){

	//��
	glPushMatrix();
	glTranslated(model->mouthPoint.x, model->mouthPoint.y, model->mouthPoint.z);
	glutSolidSphere(0.02, 8, 8);
	glPopMatrix();

	//��
	glPushMatrix();
	glTranslated(model->rightEyePoint.x, model->rightEyePoint.y, model->rightEyePoint.z);
	glutSolidSphere(0.02, 8, 8);
	glPopMatrix();

	glPushMatrix();
	glTranslated(model->leftEyePoint.x, model->leftEyePoint.y, model->leftEyePoint.z);
	glutSolidSphere(0.02, 8, 8);
	glPopMatrix();

	//�@(�␳�t��)
	glPushMatrix();
	glTranslated(model->nosePoint.x, model->nosePoint.y - 0.02, model->nosePoint.z);
	glutSolidSphere(0.02, 8, 8);
	glPopMatrix();
}
void drawObject(){

	glPushMatrix();
	glTranslated(model->centerPoint.x, model->centerPoint.y, model->centerPoint.z);

	//SetDist(1);
	if (hoge % 2 == 0){

		//cout << (int)rsAngle.x << " " << (int)rsAngle.y << " " << (int)rsAngle.z << endl;
		//cout << model->centerPoint.x << " " << model->centerPoint.y << " " << model->centerPoint.z << endl;
		/*cout << (int)CalcTwoVectorAngle(rsAxis[0], rsAxis[1]) << " " << (int)CalcTwoVectorAngle(rsAxis[1], rsAxis[2]) << " "
		<< (int)CalcTwoVectorAngle(rsAxis[0], rsAxis[2]) << endl;*/
		//cout <<"Axis:"<< rsAxis[1].x << " " << rsAxis[1].y << " " << rsAxis[1].z << endl;
	}

	if (hoge % 3 == 0){//1frame���ɉ�]������Ƌ����s�R�ȓ��������邽�ߊԊu�������čX�V

		rsAngle_.x = rsAngle.x;
		rsAngle_.y = rsAngle.y;
		rsAngle_.z = rsAngle.z;
	}

	glRotated((int)rsAngle_.z * 2.2, 1, 0, 0);//�U������
	glRotated((int)rsAngle_.x, 0, 1, 0);
	glRotated(-(int)rsAngle_.y, 0, 0, 1);

	glRotated(-90, 1, 0, 0);//realsense��obj�o�͊p�x�̒���

	glTranslated(-model->centerPoint.x, -model->centerPoint.y, -model->centerPoint.z);
	glTranslated(rsTrans.x / 1000, rsTrans.y / 1000, 0);

	glPushMatrix();
	//glutSolidSphere(0.01, 16, 16);
	//glTranslated(-model->minXPoint, -model->minYPoint, -model->centerPoint.z);
	glPopMatrix();

	//drawFaceTest();

	//�E�ڕ`��
	glPushMatrix();
	glTranslated(model->rightEyePoint.x, model->rightEyePoint.y - 0.01, model->rightEyePoint.z);
	glRotated(60, 1, 0, 0);//���ʕ����̒���
	glScaled(1.2, 1.2, 1.2);
	glTranslated(-rightEye->centerPoint.x, -rightEye->centerPoint.y, -rightEye->centerPoint.z);
	rightEye->RealSenseDraw();
	glPopMatrix();

	//����
	glPushMatrix();
	glTranslated(model->leftEyePoint.x, model->leftEyePoint.y - 0.01, model->leftEyePoint.z);
	glRotated(60, 1, 0, 0);//���ʕ����̒���
	glScaled(1.2, 1.2, 1.2);
	glTranslated(-leftEye->centerPoint.x, -leftEye->centerPoint.y, -leftEye->centerPoint.z);
	leftEye->RealSenseDraw();
	glPopMatrix();

	//�@
	glPushMatrix();
	glTranslated(model->nosePoint.x, model->nosePoint.y - 0.02, model->nosePoint.z);
	glRotated(70, 1, 0, 0);//���ʕ����̒���
	glScaled(1.2, 1.2, 1.2);
	glTranslated(-nose->centerPoint.x, -nose->centerPoint.y, -nose->centerPoint.z);
	nose->RealSenseDraw();
	glPopMatrix();

	//�@
	glPushMatrix();
	glTranslated(model->mouthPoint.x, model->mouthPoint.y - 0.02, model->mouthPoint.z);
	glRotated(70, 1, 0, 0);//���ʕ����̒���
	glScaled(1.2, 1.2, 1.2);
	glTranslated(-mouth->centerPoint.x, -mouth->centerPoint.y, -mouth->centerPoint.z);
	mouth->RealSenseDraw();
	glPopMatrix();

	//model->Draw();

	glPopMatrix();
}

void display(void)
{


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, WIDTH / HEIGHT, 0.1, 200.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	gluLookAt(
		0.0, 0.0, 1.4,//�J�����ʒu�͌Œ�
		model->centerPoint.x, model->centerPoint.y, model->centerPoint.z,
		0, 1, 0);
	hoge++;

	//���W�n(�킩��ɂ�������)
	glPushMatrix();
	//drawAxis();
	glPopMatrix();
	
	//�e�X�g�p
	glPushMatrix();
	//model->Draw();

	//if (cameraMap)
	//{
	//	HDC dc3;
	//	SetBkMode(dc3, TRANSPARENT);
	//	SelectObject(dc3, cameraMap);
	//	DeleteObject(cameraMap);
	//	COLORREF color = GetPixel(dc3, 0, 0);
	//	int R = GetRValue(color);//�ԏ����o��
	//	int G = GetGValue(color);//�Ώ����o��
	//	int B = GetBValue(color);//�����o��

	//	printf("%d,%d,%d\n", R, G, B);

	//}
	//printf("%f,%f,%f\n", cameraMap[0][0][0], cameraMap[0][0][1], cameraMap[0][0][2]);
	cv::imshow("aaa", cameraMap);
	cv::waitKey(1);

	

		
	
	glPopMatrix();

	//�I�u�W�F�N�g�`��
	drawObject();

	
	//std::cout << " " << -rightEye->centerPoint.x << " " << -rightEye->centerPoint.y << " " << -rightEye->centerPoint.z <<endl;
	glutSwapBuffers();
}
void idle(void)
{
	//angle += 2.0f;
	hoge += 2;
	Sleep(1);
	glutPostRedisplay();
}
void Init(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	/*glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);*/
	glEnable(GL_CULL_FACE);
	
	//glCullFace(GL_BACK);
	angle[0] = DegToRad(45.0);
	angle[1] = DegToRad(45.0);
	angle[2] = 0.0;
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;
	//model = new MODEL("3DScan2.obj");
	model = new MODEL("kettleTest.obj");

	rightEye = new MODEL();
	leftEye = new MODEL();
	nose = new MODEL();
	mouth = new MODEL();
	/*std::vector<Point3f> temp;
	float a=-2.1f;
	for (int i = 0; i < 100;++i){

		a += 0.1f;
		temp.push_back(Point3f(a,a,-a));
	}
	*/
}

//
// ���C���v���O����
//
int FWmain()
{
	// GLFW ������������
	if (glfwInit() == GL_FALSE)
	{
		// GLFW �̏������Ɏ��s����
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return EXIT_FAILURE;
	}

	// �v���O�����I�����ɂ� GLFW ���I������
	atexit(glfwTerminate);

	// OpenGL Version 3.2 Core Profile ��I������
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// �E�B���h�E���J��
	Window window(640, 480, "Mesh Sample");
	if (!window.get())
	{
		// �E�B���h�E���쐬�ł��Ȃ�����
		std::cerr << "Can't open GLFW window." << std::endl;
		return EXIT_FAILURE;
	}

	// ���_��
	const auto vertices(slices * stacks);

	// ���_�ʒu
	GLfloat position[stacks][slices][3];
	for (auto v = 0; v < stacks; ++v)
	{
		for (auto u = 0; u < slices; ++u)
		{
			const auto x((GLfloat(u) / GLfloat(slices - 1) - 0.5f) * GLfloat(slices) / GLfloat(stacks));
			const auto y((GLfloat(v) / GLfloat(stacks - 1) - 0.5f));

			position[v][u][0] = x;
			position[v][u][1] = y;
			position[v][u][2] = 0.0f;
		}
	}

	// ���_�z��I�u�W�F�N�g
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// ���_�ʒu���i�[���钸�_�o�b�t�@�I�u�W�F�N�g
	GLuint positionBuffer;
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);

	// ���̒��_�o�b�t�@�I�u�W�F�N�g�̃��������m�ۂ���
	glBufferData(GL_ARRAY_BUFFER, sizeof position, position, GL_STATIC_DRAW);

	// ���̒��_�o�b�t�@�I�u�W�F�N�g�� 0 �Ԃ� attribute �ϐ�������o��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// ���̒��_�z��I�u�W�F�N�g�̌�������������
	glBindVertexArray(0);

	// ���b�V���`��p�̃V�F�[�_
	const auto point(ggLoadShader("point.vert", "point.frag"));
	const auto mcLoc(glGetUniformLocation(point, "mc"));

	// �w�i�F��ݒ肷��
	glClearColor(background[0], background[1], background[2], background[3]);

	// �B�ʏ���������L���ɂ���
	glEnable(GL_DEPTH_TEST);

	Init();

	// �E�B���h�E���J���Ă���Ԃ���Ԃ��`�悷��
	while (!window.shouldClose())
	{
		// ��ʏ���
		window.clear();

		// �V�F�[�_�̎w��
		glUseProgram(point);
		glUniformMatrix4fv(mcLoc, 1, GL_FALSE, (window.getMp() * window.getMv()).get());

		// �`��
		glPopMatrix();
		glBindVertexArray(vao);

		hoge++;
		glDrawArrays(GL_POINTS, 0, vertices);

		glPushMatrix();
		
		//display();
		// �o�b�t�@�����ւ���
		window.swapBuffers();
	}

	// ���_�z��I�u�W�F�N�g���폜����
	glDeleteVertexArrays(1, &vao);

	// ���_�o�b�t�@�I�u�W�F�N�g���폜����
	glDeleteBuffers(1, &positionBuffer);
}

int MainGL::GLmain()
{
	//FWmain();
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WIDTH, HEIGHT);
	//glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("OBJ���[�_");
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

void MainGL::SetRightEye(std::vector<Point3f> &points,Point3f center){

	rightEye->VertexLoad(points);
	points.clear();
	rightEye->centerPoint.x = center.x;
	rightEye->centerPoint.y = center.y;
	rightEye->centerPoint.z = center.z;

}

void MainGL::SetLeftEye(std::vector<Point3f> &points, Point3f center){

	leftEye->VertexLoad(points);
	points.clear();
	leftEye->centerPoint.x = center.x;
	leftEye->centerPoint.y = center.y;
	leftEye->centerPoint.z = center.z;

}

void MainGL::SetNose(std::vector<Point3f> &points, Point3f center){

	nose->VertexLoad(points);
	points.clear();
	nose->centerPoint.x = center.x;
	nose->centerPoint.y = center.y;
	nose->centerPoint.z = center.z;

}

void MainGL::SetMouth(std::vector<Point3f> &points, Point3f center){

	mouth->VertexLoad(points);
	points.clear();
	mouth->centerPoint.x = center.x;
	mouth->centerPoint.y = center.y;
	mouth->centerPoint.z = center.z;

}


void MainGL::SetAngle(Point3f xAxis, Point3f yAxis, Point3f zAxis){

	//cout << xAxis.x << " " << xAxis.y << " " << xAxis.z << endl;
	//cout << zAxis.x << " " << zAxis.y << " " << zAxis.z << endl;
	//cout << "-----"<< endl;
	SetAxis(xAxis, yAxis, zAxis);

	//��]�����̐����̌���
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

bool MainGL::GetCalcDuring(){
	return enableCalc;
}

void MainGL::SetCameraMap(cv::Mat cameraMap_){

	cameraMap = cameraMap_.clone();
};


void SetAxis(Point3f xAxis, Point3f yAxis,Point3f zAxis){

	xAxis.y = 0;
	yAxis.z = 0;
	zAxis.x = 0;
	Point3f _xAxis = CalcUnitVec(xAxis);
	Point3f _yAxis = CalcUnitVec(yAxis);
	//Point3f _zAxis = CalcCross(xAxis, yAxis);//z���̃u���␳
	Point3f _zAxis = CalcUnitVec(zAxis);
	

	rsAxis[0] = _xAxis;
	rsAxis[1] = _yAxis;
	rsAxis[2] = _zAxis;

}

//�x�N�g�����X�J���[��
float VectoScalar(Point3f vec){

	return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

//2�x�N�g���̓��όv�Z
float CalcDot(Point3f vec1, Point3f vec2){

	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

//2�x�N�g���̊O�όv�Z
Point3f CalcCross(Point3f vec1, Point3f vec2){
	float xVec = vec1.y*vec2.z - vec1.z*vec2.y;
	float yVec = vec1.z*vec2.x - vec1.x*vec2.z;
	float zVec = vec1.x*vec2.y - vec1.y*vec2.x;
	return Point3f(xVec, yVec, zVec);
}

//�P�ʃx�N�g���֕ϊ�
Point3f CalcUnitVec(Point3f vec){

	Point3f temp;
	temp.x = vec.x / VectoScalar(vec);
	temp.y = vec.y / VectoScalar(vec);
	temp.z = vec.z / VectoScalar(vec);
	return temp;
}

//2�̃x�N�g���Ԃ̊p�x�����߂�(�����Ȃ�)
float CalcTwoVectorAngle(Point3f vec1,Point3f vec2){

	float cos= CalcDot(vec1, vec2) / (VectoScalar(vec1)*VectoScalar(vec2));
	float sita = acos(cos);//���W�A�������߂�
	sita = sita * 180 / PI;//�p�x�����߂�
	return sita;
}


//---------------------------------------------------------------------------------------------------
// Name : Mouse()
// Desc : �}�E�X����
//---------------------------------------------------------------------------------------------------
void Mouse(int button, int state, int x, int y)
{
	camera.MouseInput(button, state, x, y);
}

//--------------------------------------------------------------------------------------------------
// Name : Motion()
// Desc : �}�E�X�h���b�O��
//--------------------------------------------------------------------------------------------------
void Motion(int x, int y)
{
	camera.MouseMotion(x, y);
}

//--------------------------------------------------------------------------------------------------
// Name : PassiveMotion()
// Desc : �}�E�X�ړ���
//--------------------------------------------------------------------------------------------------
void PassiveMotion(int x, int y)
{
}

//--------------------------------------------------------------------------------------------------
// Name : Keyboard()
// Desc : �L�[�{�[�h����
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
// Desc : ����L�[����
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