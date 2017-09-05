#include "PNG.h"
#include "mainGL.h"

using namespace std;
//3�̃x�N�g��
struct Vector3f{
	float x;
	float y;
	float z;
}vec3d;
//4�̃x�N�g��
struct Vector4f{
	float x;
	float y;
	float z;
	float w;
}vec4d;
//4�̃J���[
struct Color4{
	float r;
	float g;
	float b;
	float a;
};
//4�̔���
struct Reflection4{
	Color4 diffuse;
	Color4 ambient;
	Color4 emission;
	Color4 specular;
};
//UV���W
struct UV{
	float u;//u�l
	float v;//v�l
}vec2d;
struct Vector4I{
	int x;
	int y;
	int z;
	int w;
};
//�|���S���f�[�^
struct Triangle{
	Vector3f TriVer;
	Vector3f TriNor;
	Color4 TriColor;
	UV TriUV;
}Tri;
//�|���S���f�[�^
struct Quadrangle{
	Vector3f QuadVer;
	Vector3f QuadNor;
	UV QuadUV;
}Quad;
//�}�e���A���\����
struct MATERIAL{
	string MaterialName;//�}�e���A����
	Reflection4 MaterialColor;//����
	float Shininess;//shininess
	string TextureName;//�e�N�X�`����
	int TexNo;//�e�N�X�`��NO.
	vector <Triangle> Tridata;//�O�p�ʃf�[�^
	vector <Quadrangle> Quaddata;//�l�p�ʃf�[�^
	vector <unsigned int> TriVerID;//�e��C���f�b�N�X
	vector <unsigned int> TriNorID;
	vector <unsigned int> TriColorID;
	vector <unsigned int> TriUVID;
	vector <unsigned int> QuadVerID;
	vector <unsigned int> QuadNorID;
	vector <unsigned int> QuadUVID;
}mtl;
//���f���N���X
class MODEL{
protected:
	bool LoadMaterialFromFile(char* FileName);//�}�e���A�����[�h
	vector <MATERIAL> Material;
	vector <TEXTURE*> TexData;//�e�N�X�`���f�[�^
	vector<GLuint> TexID;//�e�N�X�`��ID
	GLuint TexID2;
	TEXTURE* tex;

public:
	MODEL();
	MODEL(char* FileName);//�R���X�g���N�^
	bool OBJ_Load(char* FileName);//���[�h
	bool VertexLoad(std::vector<Point3f> &points);//�_�Q���[�h
	void Draw();
	void RealSenseDraw();
	vector <Vector3f> objVertex;//3�����_
	Vector3f centerPoint;
	void calcCenter();
	void CalcFacePoint();
	std::vector<Point3f> rsVertex;
	// �J���[�z����

	float minXPoint = 9999999;
	float maxXPoint = -9999999;
	float minYPoint = 9999999;
	float maxYPoint = -9999999;
	float minZPoint = 9999999;
	float maxZPoint = -9999999;
	
	Vector3f rightEyePoint;
	Vector3f leftEyePoint;
	Vector3f nosePoint;
	Vector3f mouthPoint;

};
MODEL::MODEL(){
}
MODEL::MODEL(char* FileName){
	OBJ_Load(FileName);
}
//�`��
void MODEL::Draw(){
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	for (int i = 0; i<(signed)Material.size(); i++){

		glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (const GLfloat *)&Material[i].MaterialColor.ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (const GLfloat *)&Material[i].MaterialColor.diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (const GLfloat *)&Material[i].MaterialColor.specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Material[i].Shininess);
		
		if (Material[i].TexNo>0){
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, TexID[Material[i].TexNo - 1]);
			
		}
		else{
			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		if (Material[i].Tridata.size()>1){

			glNormalPointer(GL_FLOAT, sizeof(Tri), &Material[i].Tridata[0].TriNor.x);
			glVertexPointer(3, GL_FLOAT, sizeof(Tri), &Material[i].Tridata[0].TriVer.x);
			//glColorPointer(4, GL_FLOAT, sizeof(Tri), &Material[i].Tridata[0].TriColor);
			if (Material[i].TexNo>0)glTexCoordPointer(2, GL_FLOAT, sizeof(Tri), &Material[i].Tridata[0].TriUV.u);
			glDrawArrays(GL_TRIANGLES, 0, Material[i].Tridata.size());
		}
		if (Material[i].Quaddata.size()>1){
			glVertexPointer(3, GL_FLOAT, sizeof(Quad), &Material[i].Quaddata[0].QuadVer.x);
			glNormalPointer(GL_FLOAT, sizeof(Quad), &Material[i].Quaddata[0].QuadNor.x);
			if (Material[i].TexNo>0)glTexCoordPointer(2, GL_FLOAT, sizeof(Quad), &Material[i].Quaddata[0].QuadUV.u);
			glDrawArrays(GL_QUADS, 0, Material[i].Quaddata.size());
		}
		glPopMatrix();
	}

	//glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void MODEL::RealSenseDraw(){

	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);

	GLfloat temp[3];

	for (int i=0; i < rsVertex.size(); ++i){
		temp[0] = rsVertex.at(i).x; temp[1] = rsVertex.at(i).y; temp[2] = rsVertex.at(i).z;

		//printf("%f\n", temp[1]);
		glVertexPointer(3, GL_FLOAT, sizeof(Point3f), temp);
		//glColorPointer(4, GL_FLOAT, sizeof(Tri), &Material[i].Tridata[0].TriColor);
		glDrawArrays(GL_POINTS,0, 1);
	}

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

//OBJ�t�@�C���ǂݍ���
bool MODEL::OBJ_Load(char* FileName){
	Vector4I Face[3];//�ꎞ����p
	vector <Vector3f> Vertex;//���_
	vector <Vector3f> Normal;//�@��
	vector <UV> uv;//UV

	int Material_No = 0;
	char key[255] = { 0 };
	//OBJ�t�@�C�����J���ē��e��ǂݍ���
	FILE* fp = NULL;
	fopen_s(&fp, FileName, "rt");

	//�ǂݍ���
	fseek(fp, SEEK_SET, 0);

	while (!feof(fp))
	{
		//�L�[���[�h
		ZeroMemory(key, sizeof(key));
		fscanf_s(fp, "%s ", key, sizeof(key));
		//�}�e���A��
		if (strcmp(key, "mtllib") == 0){
			fscanf_s(fp, "%s ", key, sizeof(key));
			LoadMaterialFromFile(key);
		}
		//���_
		if (strcmp(key, "v") == 0){
			fscanf_s(fp, "%f %f %f", &vec3d.x, &vec3d.y, &vec3d.z);
			Vertex.push_back(vec3d);

			//�ŏ��_�A�ő�_�����߂�
			if (minXPoint > vec3d.x){
				minXPoint = vec3d.x;
			}
			if (maxXPoint < vec3d.x){
				maxXPoint = vec3d.x;
			}

			if (minYPoint > vec3d.y){
				minYPoint = vec3d.y;
			}
			if (maxYPoint < vec3d.y){
				maxYPoint = vec3d.y;
			}

			if (minZPoint > vec3d.z){
				minZPoint = vec3d.z;
			}
			if (maxZPoint < vec3d.z){
				maxZPoint = vec3d.z;
			}
		}
		//�@��
		if (strcmp(key, "vn") == 0){
			//fscanf_s(fp, "%f %f %f", &vec3d.x, &vec3d.y, &vec3d.z);
			fscanf_s(fp, "%f %f %f", &vec3d.x, &vec3d.y, &vec3d.z);
			//vec3d.x = 0; vec3d.y = 0; vec3d.z = -1;
			Normal.push_back(vec3d);
		}
		//�e�N�X�`��
		if (strcmp(key, "vt") == 0){
			fscanf_s(fp, "%f %f", &vec2d.u, &vec2d.v);
			uv.push_back(vec2d);
		}
		//�}�e���A���Z�b�g
		if (strcmp(key, "usemtl") == 0){
			fscanf_s(fp, "%s ", key, sizeof(key));
			for (int i = 0; i<(signed)Material.size(); i++){
				if (strcmp(key, Material[i].MaterialName.c_str()) == 0)Material_No = i;
			}
		}
		//�ʂ̃C���f�b�N�X 0=���_ 1=UV 2=�@��
		if (strcmp(key, "f") == 0){
			Face[0].w = -1;
			Face[1].w = -1;
			Face[2].w = -1;
			fscanf_s(fp, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &Face[0].x, &Face[1].x, &Face[2].x, &Face[0].y, &Face[1].y, &Face[2].y, &Face[0].z, &Face[1].z, &Face[2].z, &Face[0].w, &Face[1].w, &Face[2].w);
			if ((Face[0].w == -1) && (Face[1].w == -1) && (Face[2].w == -1)){
				//�O�p��
				Material[Material_No].TriVerID.push_back(Face[0].x - 1);
				Material[Material_No].TriVerID.push_back(Face[0].y - 1);
				Material[Material_No].TriVerID.push_back(Face[0].z - 1);
				Material[Material_No].TriUVID.push_back(Face[1].x - 1);
				Material[Material_No].TriUVID.push_back(Face[1].y - 1);
				Material[Material_No].TriUVID.push_back(Face[1].z - 1);
				Material[Material_No].TriNorID.push_back(Face[2].x - 1);
				Material[Material_No].TriNorID.push_back(Face[2].y - 1);
				Material[Material_No].TriNorID.push_back(Face[2].z - 1);
			}
			else{
				//�l�p��
				Material[Material_No].QuadVerID.push_back(Face[0].x - 1);
				Material[Material_No].QuadVerID.push_back(Face[0].y - 1);
				Material[Material_No].QuadVerID.push_back(Face[0].z - 1);
				Material[Material_No].QuadVerID.push_back(Face[0].w - 1);
				Material[Material_No].QuadUVID.push_back(Face[1].x - 1);
				Material[Material_No].QuadUVID.push_back(Face[1].y - 1);
				Material[Material_No].QuadUVID.push_back(Face[1].z - 1);
				Material[Material_No].QuadUVID.push_back(Face[1].w - 1);
				Material[Material_No].QuadNorID.push_back(Face[2].x - 1);
				Material[Material_No].QuadNorID.push_back(Face[2].y - 1);
				Material[Material_No].QuadNorID.push_back(Face[2].z - 1);
				Material[Material_No].QuadNorID.push_back(Face[2].w - 1);
			}
		}
	}
	for (int j = 0; j<(signed)Material.size(); j++){
		for (int i = 0; i<(signed)Material[j].TriVerID.size(); i++){
			Tri.TriVer = Vertex[Material[j].TriVerID[i]];
			Tri.TriNor = Normal[Material[j].TriNorID[i]];
			Tri.TriUV = uv[Material[j].TriUVID[i]];
			Color4 temp; temp.r = 0.1f; temp.g = 1; temp.b = 1; temp.a = 1;
			Tri.TriColor = temp;
			Material[j].Tridata.push_back(Tri);
		}
		for (int i = 0; i<(signed)Material[j].QuadVerID.size(); i++){
			Quad.QuadVer = Vertex[Material[j].QuadVerID[i]];
			Quad.QuadNor = Normal[Material[j].QuadNorID[i]];
			Quad.QuadUV = uv[Material[j].QuadUVID[i]];
			Material[j].Quaddata.push_back(Quad);
		}
		Material[j].TriVerID.clear();
		Material[j].TriNorID.clear();
		Material[j].TriUVID.clear();
		Material[j].QuadVerID.clear();
		Material[j].QuadNorID.clear();
		Material[j].QuadUVID.clear();
	}

	copy(Vertex.begin(), Vertex.end(), back_inserter(objVertex));
	calcCenter();
	CalcFacePoint();
	Vertex.clear();
	Normal.clear();
	uv.clear();
	return true;
}

bool MODEL::VertexLoad(vector<Point3f> &points){
	//Vector4I Face[3];//�ꎞ����p
	//vector <Vector3f> Vertex;//���_
	//vector <Vector3f> Normal;//�@��
	//vector <UV> uv;//UV

	rsVertex.clear();

	for (int i = 0; i < points.size();++i){
		rsVertex.push_back(points.at(i));
	}

	/*fscanf_s(fp, "%f %f %f", &vec3d.x, &vec3d.y, &vec3d.z);
	Vertex.push_back(vec3d);*/
	//	}
	//	//�@��
	//	if (strcmp(key, "vn") == 0){
	//		//fscanf_s(fp, "%f %f %f", &vec3d.x, &vec3d.y, &vec3d.z);
	//		fscanf_s(fp, "%f %f %f", &vec3d.x, &vec3d.y, &vec3d.z);
	//		//vec3d.x = 0; vec3d.y = 0; vec3d.z = -1;
	//		Normal.push_back(vec3d);
	//	}
	//	//�e�N�X�`��
	//	if (strcmp(key, "vt") == 0){
	//		fscanf_s(fp, "%f %f", &vec2d.u, &vec2d.v);
	//		uv.push_back(vec2d);
	//	}
	//	//�}�e���A���Z�b�g
	//	if (strcmp(key, "usemtl") == 0){
	//		fscanf_s(fp, "%s ", key, sizeof(key));
	//		for (int i = 0; i<(signed)Material.size(); i++){
	//			if (strcmp(key, Material[i].MaterialName.c_str()) == 0)Material_No = i;
	//		}
	//	}
	//	//�ʂ̃C���f�b�N�X 0=���_ 1=UV 2=�@��
	//	if (strcmp(key, "f") == 0){
	//		Face[0].w = -1;
	//		Face[1].w = -1;
	//		Face[2].w = -1;
	//		fscanf_s(fp, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &Face[0].x, &Face[1].x, &Face[2].x, &Face[0].y, &Face[1].y, &Face[2].y, &Face[0].z, &Face[1].z, &Face[2].z, &Face[0].w, &Face[1].w, &Face[2].w);
	//		if ((Face[0].w == -1) && (Face[1].w == -1) && (Face[2].w == -1)){
	//			//�O�p��
	//			Material[Material_No].TriVerID.push_back(Face[0].x - 1);
	//			Material[Material_No].TriVerID.push_back(Face[0].y - 1);
	//			Material[Material_No].TriVerID.push_back(Face[0].z - 1);
	//			Material[Material_No].TriUVID.push_back(Face[1].x - 1);
	//			Material[Material_No].TriUVID.push_back(Face[1].y - 1);
	//			Material[Material_No].TriUVID.push_back(Face[1].z - 1);
	//			Material[Material_No].TriNorID.push_back(Face[2].x - 1);
	//			Material[Material_No].TriNorID.push_back(Face[2].y - 1);
	//			Material[Material_No].TriNorID.push_back(Face[2].z - 1);
	//		}
	//		else{
	//			//�l�p��
	//			Material[Material_No].QuadVerID.push_back(Face[0].x - 1);
	//			Material[Material_No].QuadVerID.push_back(Face[0].y - 1);
	//			Material[Material_No].QuadVerID.push_back(Face[0].z - 1);
	//			Material[Material_No].QuadVerID.push_back(Face[0].w - 1);
	//			Material[Material_No].QuadUVID.push_back(Face[1].x - 1);
	//			Material[Material_No].QuadUVID.push_back(Face[1].y - 1);
	//			Material[Material_No].QuadUVID.push_back(Face[1].z - 1);
	//			Material[Material_No].QuadUVID.push_back(Face[1].w - 1);
	//			Material[Material_No].QuadNorID.push_back(Face[2].x - 1);
	//			Material[Material_No].QuadNorID.push_back(Face[2].y - 1);
	//			Material[Material_No].QuadNorID.push_back(Face[2].z - 1);
	//			Material[Material_No].QuadNorID.push_back(Face[2].w - 1);
	//		}
	//	}
	//}
	//for (int j = 0; j<(signed)Material.size(); j++){
	//	for (int i = 0; i<(signed)Material[j].TriVerID.size(); i++){
	//		Tri.TriVer = Vertex[Material[j].TriVerID[i]];
	//		Tri.TriNor = Normal[Material[j].TriNorID[i]];
	//		Tri.TriUV = uv[Material[j].TriUVID[i]];
	//		Color4 temp; temp.r = 0.1f; temp.g = 1; temp.b = 1; temp.a = 1;
	//		Tri.TriColor = temp;
	//		Material[j].Tridata.push_back(Tri);
	//	}
	//	for (int i = 0; i<(signed)Material[j].QuadVerID.size(); i++){
	//		Quad.QuadVer = Vertex[Material[j].QuadVerID[i]];
	//		Quad.QuadNor = Normal[Material[j].QuadNorID[i]];
	//		Quad.QuadUV = uv[Material[j].QuadUVID[i]];
	//		Material[j].Quaddata.push_back(Quad);
	//	}
	//	Material[j].TriVerID.clear();
	//	Material[j].TriNorID.clear();
	//	Material[j].TriUVID.clear();
	//	Material[j].QuadVerID.clear();
	//	Material[j].QuadNorID.clear();
	//	Material[j].QuadUVID.clear();
	//}

	//copy(Vertex.begin(), Vertex.end(), back_inserter(objVertex));
	//calcCenter();
	//Vertex.clear();
	//Normal.clear();
	//uv.clear();
	return true;
}



bool MODEL::LoadMaterialFromFile(char* FileName){
	FILE* fp = NULL;
	fopen_s(&fp, FileName, "rt");
	char key[255] = { 0 };
	bool flag = false;
	bool flag2 = false;
	ZeroMemory(&vec4d, sizeof(Vector4f));
	vec4d.w = 1.0f;
	mtl.MaterialColor.emission = (const Color4 &)vec4d;
	mtl.Shininess = 0.0f;
	mtl.TexNo = 0;

	fseek(fp, SEEK_SET, 0);

	while (!feof(fp))
	{
		//�L�[���[�h�ǂݍ���
		fscanf_s(fp, "%s ", key, sizeof(key));
		//�}�e���A����
		if (strcmp(key, "newmtl") == 0)
		{
			if (flag){ Material.push_back(mtl); mtl.TexNo = 0; }
			flag = true;
			fscanf_s(fp, "%s ", key, sizeof(key));
			mtl.MaterialName = key;
			flag2 = false;
		}
		//Ka �A���r�G���g
		if (strcmp(key, "Ka") == 0)
		{
			fscanf_s(fp, "%f %f %f", &vec4d.x, &vec4d.y, &vec4d.z);
			mtl.MaterialColor.ambient = (const Color4 &)vec4d;
		}
		//Kd �f�B�t���[�Y
		if (strcmp(key, "Kd") == 0)
		{
			fscanf_s(fp, "%f %f %f", &vec4d.x, &vec4d.y, &vec4d.z);
			mtl.MaterialColor.diffuse = (const Color4 &)vec4d;
		}
		//Ks �X�y�L�����[
		if (strcmp(key, "Ks") == 0)
		{
			fscanf_s(fp, "%f %f %f", &vec4d.x, &vec4d.y, &vec4d.z);
			mtl.MaterialColor.specular = (const Color4 &)vec4d;
		}
		//SHININESS
		if (strcmp(key, "Ns") == 0)
		{
			fscanf_s(fp, "%f", &vec4d.x);
			mtl.Shininess = vec4d.x;
		}
		//map_Kd �e�N�X�`��
		if (strcmp(key, "map_Kd") == 0)
		{
			fscanf_s(fp, "%s ", key, sizeof(key));
			for (int i = 0; i<(signed)Material.size(); i++){
				if (strcmp(key, Material[i].TextureName.c_str()) == 0){
					flag2 = true;
					mtl.TexNo = Material[i].TexNo;
					break;
				}
			}
			if (flag2){
			}
			else{
				mtl.TextureName = key;
				//�e�N�X�`�����쐬
				TexData.push_back(tex);
				TexData[TexData.size() - 1] = new TEXTURE(mtl.TextureName.c_str());;
				mtl.TexNo = TexData.size();
				TexID.push_back(TexID2);
				glGenTextures(1, (GLuint *)&TexID[TexData.size() - 1]);
				glBindTexture(GL_TEXTURE_2D, TexID[TexData.size() - 1]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glEnable(GL_TEXTURE_2D);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TexData[TexData.size() - 1]->Width, TexData[TexData.size() - 1]->Height,
					0, GL_RGBA, GL_UNSIGNED_BYTE, TexData[TexData.size() - 1]->image);
				glDisable(GL_TEXTURE_2D);
			}
		}
	}
	fclose(fp);
	if (flag)Material.push_back(mtl);
	return true;
}

void MODEL::calcCenter(){
	
	Vector3f temp;
	for (int i = 0; i < objVertex.size();++i){
		temp.x += objVertex.at(i).x;
		temp.y += objVertex.at(i).y;
		temp.z += objVertex.at(i).z;
	}

	temp.x = temp.x / objVertex.size();
	temp.y = temp.y / objVertex.size();
	temp.z = temp.z / objVertex.size();

	centerPoint = temp;
}

void MODEL::CalcFacePoint(){

	//���̌v�Z
	mouthPoint.x = minXPoint + ((maxXPoint - minXPoint) / 2);
	mouthPoint.y = centerPoint.y;
	mouthPoint.z = minZPoint + ((maxZPoint - minZPoint)*(2.0 / 9.0));
	
	//�ڂ̌v�Z
	rightEyePoint.x = minXPoint + ((maxXPoint - minXPoint) *(2.0 / 5.0));
	rightEyePoint.y = centerPoint.y;
	rightEyePoint.z = mouthPoint.z + ((maxZPoint - mouthPoint.z) / 2);

	leftEyePoint.x = maxXPoint - ((maxXPoint - minXPoint) *(2.0 / 5.0));
	leftEyePoint.y = centerPoint.y;
	leftEyePoint.z = mouthPoint.z + ((maxZPoint - mouthPoint.z) / 2);

	//�@�̌v�Z
	nosePoint.x = minXPoint + ((maxXPoint - minXPoint) / 2);
	nosePoint.y = centerPoint.y;
	nosePoint.z = mouthPoint.z + ((maxZPoint - mouthPoint.z) / 4);
}
