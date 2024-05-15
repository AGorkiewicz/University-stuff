#pragma once
#include "utils.cpp"
#include "shader.cpp"
using namespace std;

struct MyDrawable
{
	Shader shader;
	GLuint VBO, VAO, EBO;

	MyDrawable(const char* shader_name) : shader(shader_name)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
	}
	
	~MyDrawable() {
		//fprintf(stderr,"AGL: destroying Drawable\n");
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO);
		if (shader.id) glDeleteProgram(shader.id);
	}

	void bind()
	{
		shader.use();
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	}
};

struct Sphere
{
	vec3 position;
	lf radius;

	Sphere(vec3 position = vec3(), lf radius = 1.0) : position(position), radius(radius) {}
};

vec3 spherical(int inx, int N)
{
	int X = inx % N;
	int Y = inx / N;
	lf alpha = 2 * PI / N * X;
	lf beta = PI / N * Y;
	lf x = sin(beta) * cos(alpha);
	lf y = sin(beta) * sin(alpha);
	lf z = cos(beta);
	return vec3(x, y, -z);
};

struct Bubbles : MyDrawable
{
	static const int N = 30;
	GLuint IVBO;

	Bubbles() : MyDrawable("shaders/bubbles")
	{
		glGenBuffers(1, &IVBO);
		
		bind();
		
		static lf vertices[3 * N * (N + 1)];
		
		rep(i, 0, N * (N + 1) - 1){
			vec3 p = spherical(i, N);
			vertices[3 * i + 0] = p.x;
			vertices[3 * i + 1] = p.y;
			vertices[3 * i + 2] = p.z;
		}
		
		static GLuint indices[6 * N * N];

		rep(i, 0, N * N - 1){
			int j = (i + 1) % N == 0 ? i + 1 - N : i + 1;
			indices[6 * i + 0] = i;
			indices[6 * i + 1] = j;
			indices[6 * i + 2] = j + N;
			indices[6 * i + 3] = i;
			indices[6 * i + 4] = i + N;
			indices[6 * i + 5] = j + N;
		}
		
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW); 
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
	}

	void draw(const vector <Sphere>& instances, mat4 perspective, mat4 unview, vec3 light_pos, vec3 player_pos, vec3 camera_pos)
	{
		bind();
		
		vector <lf> vertices;

		for (Sphere instance : instances){
			vertices.pb(instance.position.x);
			vertices.pb(instance.position.y);
			vertices.pb(instance.position.z);
			vertices.pb(instance.radius);
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, IVBO);
		glBufferData(GL_ARRAY_BUFFER, ss(vertices) * sizeof(lf), &vertices[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(lf), (void*)(0 * sizeof(lf)));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(lf), (void*)(3 * sizeof(lf)));
		glVertexAttribDivisor(1, 1); glEnableVertexAttribArray(1);
		glVertexAttribDivisor(2, 1); glEnableVertexAttribArray(2);
		
		shader.setMatrix("perspective", perspective);
		shader.setMatrix("unview", unview);
		shader.setVec3("light_pos1", light_pos);
		shader.setVec3("light_pos2", player_pos);
		shader.setVec3("camera_pos", camera_pos);
		
		glDrawElementsInstanced(GL_TRIANGLES, 6 * N * N, GL_UNSIGNED_INT, 0, ss(instances));
	}
};

struct Player : MyDrawable, Sphere
{
	static const int N = 50;

	Player(vec3 position = vec3(), lf radius = 1.0) : MyDrawable("shaders/player"), Sphere(position, radius)
	{
		bind();
		
		static lf vertices[3 * N * (N + 1)];
		
		rep(i, 0, N * (N + 1) - 1){
			vec3 p = spherical(i, N);
			vertices[3 * i + 0] = p.x;
			vertices[3 * i + 1] = p.y;
			vertices[3 * i + 2] = p.z;
		}
		
		static GLuint indices[6 * N * N];

		rep(i, 0, N * N - 1){
			int j = (i + 1) % N == 0 ? i + 1 - N : i + 1;
			indices[6 * i + 0] = i;
			indices[6 * i + 1] = j;
			indices[6 * i + 2] = j + N;
			indices[6 * i + 3] = i;
			indices[6 * i + 4] = i + N;
			indices[6 * i + 5] = j + N;
		}
		
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
	}

	void draw(mat4 perspective, mat4 unview, vec3 light_pos, vec3 camera_pos, int control_mode)
	{
		bind();
		
		shader.setMatrix("perspective", perspective);
		shader.setMatrix("unview", unview);
		shader.setVec3("offset", position);
		shader.setFloat("scale", radius);
		shader.setVec3("light_pos", light_pos);
		shader.setVec3("camera_pos", camera_pos);
		shader.setBool("control_mode", control_mode);

		glDrawElements(GL_TRIANGLES, 6 * N * N, GL_UNSIGNED_INT, 0);
	}
};

struct Box : MyDrawable
{
	static constexpr lf mx = 3.0;
	static constexpr lf my = 2.0;
	static constexpr lf mz = 5.0;

	Box() : MyDrawable("shaders/box")
	{
		bind();
		
		shader.setFloat("mx", mx);
		shader.setFloat("my", my);
		shader.setFloat("mz", mz);
	}
	
	void draw(mat4 perspective, vec3 light_pos, vec3 player_pos, vec3 camera_pos, int camera_mode)
	{
		bind();
		
		shader.setMatrix("perspective", perspective);
		shader.setVec3("light_pos1", light_pos);
		shader.setVec3("light_pos2", player_pos);
		shader.setVec3("camera_pos", camera_pos);
		
		if (camera_mode == 0) glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
		if (camera_mode == 1) glDrawArrays(GL_TRIANGLES, 0, 10 * 3);
	}
};
