#pragma once
#include <bits/stdc++.h>
#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>
#include <drawables.cpp>

using lf = GLfloat;
using namespace std;
#define ss(x) ((int)x.size())
#define pb push_back

class point {
	public:
	lf x, y;
	point() {}
	point(lf x, lf y) : x(x), y(y) {}
};

point operator*(const lf& a, const point& b) { return point(a * b.x, a * b.y); }
point operator+(const point& a, const point& b) { return point(a.x + b.x, a.y + b.y); }
point operator-(const point& a, const point& b) { return point(a.x - b.x, a.y - b.y); }

lf scale;

class segment {
	public:
	point center, a, b;
	lf color[2][3];
	void set_slope(lf slope){
		a = point(scale * cos(slope), scale * sin(slope));
		b = point(-a.x, -a.y);
		a = a + center;
		b = b + center;
	}
	void set_color(int which, float r, float g, float b){
		color[which][0] = r;
		color[which][1] = g;
		color[which][2] = b;
	}
	void set_center(point c){
		center = c;
	}
};

class Segments : public AGLDrawable {
	public:
	Segments() : AGLDrawable(0) {
		setShaders();
	}
	void setShaders() {
		compileShadersFromFile(
			"shaders/segments.vertexshader",
			"shaders/segments.fragmentshader"
		);
	}
	void setBuffers(vector <segment>& segments) {
		bindBuffers();
		
		vector <lf> pos, col, buf;

		for (segment seg : segments){
			pos.pb(seg.a.x);
			pos.pb(seg.a.y);
			pos.pb(seg.b.x);
			pos.pb(seg.b.y);
			col.pb(seg.color[0][0]);
			col.pb(seg.color[0][1]);
			col.pb(seg.color[0][2]);
			col.pb(seg.color[1][0]);
			col.pb(seg.color[1][1]);
			col.pb(seg.color[1][2]);
		}
		for (lf x : pos) buf.pb(x);
		for (lf x : col) buf.pb(x);

		glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(float), &buf[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			0,                 // attribute 0, must match the layout in the shader.
			2,                 // size
			GL_FLOAT,          // type
			GL_FALSE,          // normalized?
			0,                 // stride
			(void*)0           // array buffer offset
			);
		glVertexAttribPointer(
			1,                 // attribute 0, must match the layout in the shader.
			3,                 // size
			GL_FLOAT,          // type
			GL_FALSE,          // normalized?
			0,                 // stride
			(void*)(ss(pos) * sizeof(lf))     // array buffer offset
			);
	}
	void draw(vector <segment>& segments, int mx, int my) {
		bindProgram();
		setBuffers(segments);
		glUniform1f(2, lf(mx) / lf(my));
		glDrawArrays(GL_LINES, 0, ss(segments) * 2);
	}
};

class Background : public AGLDrawable {
public:
   Background() : AGLDrawable(0) {
      setShaders();
   }
   void setShaders() {
		compileShadersFromFile(
			"shaders/background.vertexshader",
			"shaders/background.fragmentshader"
		);
   }
   void draw(lf timer, int mx, int my){
      bindProgram();
	  bindBuffers();
	  glUniform1f(0, timer);
	  glUniform1f(1, lf(mx) / lf(my));
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
   }
};

