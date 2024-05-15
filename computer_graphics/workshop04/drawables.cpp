#pragma once
#include <bits/stdc++.h>
#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>
#include <drawables.cpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using lf = GLfloat;
using mat4=glm::mat4;
using vec3=glm::vec3;
using vec4=glm::vec4;
#define ss(x) ((int)x.size())
#define pb push_back

const lf PI = acos(-1);

struct Tetra {
	static constexpr lf radius = 0.03;

	vec3 points[4];	

	vec3 randomPoint(vec3 origin = vec3(0.0, 0.0, 0.0)){
		vec3 v = vec3(
			rand() % 10001 - 5000,
			rand() % 10001 - 5000,
			rand() % 10001 - 5000
		);
		v /= glm::length(v);
		v *= radius;
		v += origin;
		return v;
	}

	Tetra(vec3 origin = vec3(0.0, 0.0, 0.0)){
		points[0] = randomPoint(origin);
		points[1] = randomPoint(origin);
		points[2] = randomPoint(origin);
		points[3] = randomPoint(origin);
	}
};

struct Tetras : public AGLDrawable {
	vector <Tetra> vec;

	GLuint MatrixID;

	Tetras() : AGLDrawable(0) {
		setShaders();
		MatrixID = glGetUniformLocation(pId, "MVP");
	}

	void setShaders() {
		compileShadersFromFile(
			"shaders/tetra.vertexshader",
			"shaders/tetra.fragmentshader"
		);
	}

	void setBuffers() {
		bindBuffers();
		
		vector <lf> buf;

		for (auto tet : vec){
			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 4; j++){
					if (j == i) continue;
					buf.pb(tet.points[j].x);
					buf.pb(tet.points[j].y);
					buf.pb(tet.points[j].z);
				}
			}
		}
		
		glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), &buf[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                 // attribute 0, must match the layout in the shader.
			3,                 // size
			GL_FLOAT,          // type
			GL_FALSE,          // normalized?
			0,                 // stride
			(void*)0           // array buffer offset
			);
	}
	void draw(mat4 MVP) {
		bindProgram();
		bindBuffers();
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, ss(vec) * 12);
	}
};

struct Sphere : public AGLDrawable {
	vector <vec3> triangles;
	vec3 position = vec3(0.0, 0.0, 0.0);
	static constexpr lf radius = 0.02;
	GLuint MatrixID;

	Sphere(vec3 pos) : position(pos), AGLDrawable(0) {
		setShaders();
		setBuffers();
		MatrixID = glGetUniformLocation(pId, "MVP");
	}

	void setShaders() {
		compileShadersFromFile(
			"shaders/sphere.vertexshader",
			"shaders/sphere.fragmentshader"
		);
	}

	void setBuffers() {
		bindBuffers();
		
		vector <lf> buf;

		int N = 30;

		auto spherical = [&](int X, int Y){
			lf alpha = 2 * PI / N * X;
			lf beta = PI / N * Y;
			lf y = sin(beta);
			lf x = cos(alpha) * cos(beta);
			lf z = sin(alpha) * cos(beta);
			return vec3(x, y, z);
		};

		triangles.clear();
		for (int x = 0; x < N; x++){
			for (int y = -N; y < N; y++){
				vec3 a = spherical(x, y);
				vec3 b = spherical(x, y + 1);
				vec3 c = spherical(x + 1, y);
				vec3 d = spherical(x + 1, y + 1);
				triangles.pb(a);
				triangles.pb(b);
				triangles.pb(d);
				triangles.pb(a);
				triangles.pb(c);
				triangles.pb(d);
			}
		}

		for (auto tri : triangles){
			buf.pb(tri.x);
			buf.pb(tri.y);
			buf.pb(tri.z);
		}
		
		glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), &buf[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                 // attribute 0, must match the layout in the shader.
			3,                 // size
			GL_FLOAT,          // type
			GL_FALSE,          // normalized?
			0,                 // stride
			(void*)0           // array buffer offset
			);
	}
	void draw(mat4 MVP) {
		bindProgram();
		bindBuffers();
		MVP = MVP * glm::translate(position) * glm::scale(mat4(1.0), vec3(radius, radius, radius));
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, ss(triangles) * 3);
	}
};

struct Background : public AGLDrawable {
	GLuint MatrixID;
	static constexpr lf scale = 1.5;
	//static constexpr glm::vec3 trans = vec3(0.25, 0.25, 0.25);
	Background() : AGLDrawable(0) {
		setShaders();
		MatrixID = glGetUniformLocation(pId, "MVP");
	}
	void setShaders() {
		compileShadersFromFile(
				"shaders/background.vertexshader",
				"shaders/background.fragmentshader"
				);
	}
	void draw(lf timer, mat4 MVP){
		bindProgram();
		bindBuffers();
		MVP = MVP * glm::translate(-vec3(0.25, 0.25, 0.25)) * glm::scale(mat4(1.0), vec3(scale, scale, scale));
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1f(0, timer);
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
	}
};

