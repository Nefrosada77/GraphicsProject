#ifndef OBJ3D_H
#define OBJ3D_H

#include "Mesh.h"

#include <glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>
#include <string>
using namespace std;

//CLASSE DO OBJETO
class Obj3D {
public:
	string name;
	bool quebravel = false;
	glm::vec3 Pos = glm::vec3(0.0f);
	Mesh mesh;
	glm::mat4 transform = glm::mat4(1.0f);
	glm::vec3 min = glm::vec3( 100000000.0f);
	glm::vec3 max = glm::vec3(-100000000.0f);

	Obj3D(string path, glm::vec3 translate = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), float angle = 0.0f , glm::vec3 axis = glm::vec3(0.0f))
	{
		//LEITURA DE ARQUIVO
		name = path;
		Group* g_atual = new Group;
		ifstream arq(path);
		while (!arq.eof())
		{
			string line;
			getline(arq, line);
			stringstream sline;
			sline << line;
			string temp;
			sline >> temp;
			//ADD POSI플O DA VERTICE
			if (temp == "v") {
				float x, y, z;
				sline >> x >> y >> z;
				glm::vec3 tempVert(x, y, z);
				//cout << "NEW VERTEX -> " << glm::to_string(tempVert) << endl;
				mesh.vertices.push_back(tempVert);
			}
			//ADD COORDENADA DE TEXTURA
			else if (temp == "vt") {
				float xt, yt;
				sline >> xt >> yt;
				glm::vec2 tempTex(xt, yt);
				//cout << "NEW TEXTURE COORD -> " << glm::to_string(tempTex) << endl;
				mesh.textures.push_back(tempTex);
			}
			//ADD NORMAL DA VERTICE
			else if (temp == "vn") {
				float xn, yn, zn;
				sline >> xn >> yn >> zn;
				glm::vec3 tempNorm(xn, yn, zn);
				//cout << "NEW NORMAL -> " << glm::to_string(tempNorm) << endl;
				mesh.normals.push_back(tempNorm);
			}
			//ADD FACE
			else if (temp == "f") {
				Face* f_atual = new Face;
				while (!sline.eof()) {
					string token;
					sline >> token;
					stringstream stoken;
					stoken << token;
					int point = 0;
					while (!stoken.eof()) {
						point++;
						string aux;
						getline(stoken, aux, '/');
						if (point == 1) {
							f_atual->indVerts.push_back(stoi(aux) - 1);
						}
						else if (point == 2) {
							if (aux != "") {
								f_atual->indTexCoord.push_back(stoi(aux) - 1);
							}
						}
						else if (point == 3) {
							if (aux != "") {
								f_atual->indNormal.push_back(stoi(aux) - 1);
							}
						}
					}
				}
				g_atual->faces.push_back(f_atual);
			}
		}
		//MUDAR POSI플O NA CRIA플O
		if (translate != glm::vec3(0.0f))
		{
			transform = glm::translate(transform, translate);
			Pos += translate;
		}
		//MUDAR TAMANHO NA CRIA플O
		if (scale != glm::vec3(1.0f))
		{
			transform = glm::scale(transform, scale);
		}
		//MUDAR ROTA플O NA CRIA플O
		if (angle != 0.0f)
		{
			transform = glm::rotate(transform, angle, axis);
		}
		mesh.groups.push_back(g_atual);
		//MUDAR O MINIMO E MAXIMO PARA A CAIXA DE COLIS홒
		for (glm::vec3 vert : mesh.vertices)
		{
			if ((vert.x * scale.x) + Pos.x <= min.x) {
 				min.x = (vert.x * scale.x) + Pos.x;
			}
			if ((vert.y * scale.y) + Pos.y <= min.y) {
				min.y = (vert.y * scale.y) + Pos.y;
			}
			if ((vert.z * scale.z) + Pos.z <= min.z) {
				min.z = (vert.z * scale.z) + Pos.z;
			}


			if ((vert.x * scale.x) + Pos.x >= max.x) {
				max.x = (vert.x * scale.x) + Pos.x;
			}
			if ((vert.y * scale.y) + Pos.y >= max.y) {
				max.y = (vert.y * scale.y) + Pos.y;
			}
			if ((vert.z * scale.z) + Pos.z >= max.z) {
				max.z = (vert.z * scale.z) + Pos.z;
			}
		}
		mesh.setupMesh();
	}
	//DESENHO
	void draw() {
		mesh.draw();
	}
	//MOVIMENTA플O
	void move(glm::vec3 pos) {
		transform = glm::translate(transform, pos);
		Pos += pos;
		min += pos;
		max += pos;
	}
};
#endif // !3DOBJ_H

