#ifndef MESH_H
#define MESH_H

#include "shader.h"

#include <GL/glew.h>
#include <glm.hpp>
#include <vector>
using namespace std;

struct Vertex {
	glm::vec3 aPos;
	glm::vec3 aNorm;
	glm::vec2 aTex;
};

class Face {
public:
	vector<int> indVerts;
	vector<int> indNormal;
	vector<int> indTexCoord;
};

class Group {
public:
	GLuint VAO, VBO;
	vector<Face*> faces;
	int numVert = 0;
};

class Mesh {
public:
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec2> textures;
	vector<Group*> groups;
	void setupMesh()
	{
		for (Group* g : groups) {
			vector<Vertex> vs;
			for (Face* f : g->faces)
			{
				Vertex v1, v2, v3, v4;
				if (f->indVerts.size() == 3)
				{
					v1.aPos = vertices[f->indVerts[0]];
					v2.aPos = vertices[f->indVerts[1]];
					v3.aPos = vertices[f->indVerts[2]];
				}
				else {
					v1.aPos = vertices[f->indVerts[0]];
					v2.aPos = vertices[f->indVerts[1]];
					v3.aPos = vertices[f->indVerts[2]];
					v4.aPos = vertices[f->indVerts[3]];
				}
				if (normals.size() != 0)
				{
					if (f->indVerts.size() == 3) {
						v1.aNorm = normals[f->indNormal[0]];
						v2.aNorm = normals[f->indNormal[1]];
						v3.aNorm = normals[f->indNormal[2]];
					}
					else {
						v1.aNorm = normals[f->indNormal[0]];
						v2.aNorm = normals[f->indNormal[1]];
						v3.aNorm = normals[f->indNormal[2]];
						v4.aNorm = normals[f->indNormal[3]];
					}
				}
				if (textures.size() != 0)
				{
					if (f->indVerts.size() == 3) {
						v1.aTex = textures[f->indTexCoord[0]];
						v2.aTex = textures[f->indTexCoord[1]];
						v3.aTex = textures[f->indTexCoord[2]];
					}
					else {
						v1.aTex = textures[f->indTexCoord[0]];
						v2.aTex = textures[f->indTexCoord[1]];
						v3.aTex = textures[f->indTexCoord[2]];
						v4.aTex = textures[f->indTexCoord[3]];
					}
				}
				if (f->indVerts.size() == 3) {
					vs.push_back(v1);
					vs.push_back(v2);
					vs.push_back(v3);
				}
				else {
					vs.push_back(v1);
					vs.push_back(v2);
					vs.push_back(v3);
					vs.push_back(v1);
					vs.push_back(v3);
					vs.push_back(v4);
				}
				g->numVert += 6;
				
			}

			glGenVertexArrays(1, &g->VAO);
			glGenBuffers(1, &g->VBO);

			glBindVertexArray(g->VAO);
			glBindBuffer(GL_ARRAY_BUFFER, g->VBO);

			glBufferData(GL_ARRAY_BUFFER, vs.size() * sizeof(Vertex), &vs[0], GL_STATIC_DRAW);

			// vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, aNorm));

			// tex coord
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, aTex));

		}
	}
	void draw() {
		for (Group* g : groups)
		{
			glBindVertexArray(g->VAO);
			glDrawArrays(GL_TRIANGLES, 0, g->numVert);
		}
	}
};

#endif // MESH_H

