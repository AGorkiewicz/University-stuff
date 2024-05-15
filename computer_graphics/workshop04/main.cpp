#include <bits/stdc++.h>
#include <AGL3Window.hpp>
#include <AGL3Drawable.hpp>
#include <drawables.cpp>
#define rep(i, a, b) for (int i = (a); i <= (b); i++)

struct MyWin : public AGLWindow {
    MyWin() {};
    MyWin(int _wd, int _ht, const char *name, int vers, int fullscr=0) : AGLWindow(_wd, _ht, name, vers, fullscr) {};
    void KeyCB(int key, int scancode, int action, int mods);
	void CallbackResize(GLFWwindow* window, int cx, int cy);
		void MainLoop();
	};

void MyWin::KeyCB(int key, int scancode, int action, int mods) {
    AGLWindow::KeyCB(key,scancode, action, mods); // f-key full screen switch
}

bool collides(vec3 a, vec3 b){
	return glm::length(a - b) < Sphere::radius;
}

bool collides(vec3 position, Tetra tetra){
	if (glm::length(position - tetra.points[0]) > 2 * Sphere::radius + 2 * Tetra::radius) return false;
	for (int i = 0; i < 4; i++){
		if (collides(position, tetra.points[i])){
			return true;
		}
	}
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < i; j++){
			for (int tries = 0; tries < 10; tries++){
				lf w1 = rand() % 100;
				lf w2 = rand() % 100;
				vec3 pos = (tetra.points[i] * w1 + tetra.points[j] * w2) / (w1 + w2);
				if (collides(position, pos)) return true;
			}
		}
	}
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < i; j++){
			for (int k = 0; k < j; k++){
				for (int tries = 0; tries < 10; tries++){
					lf w1 = rand() % 100;
					lf w2 = rand() % 100;
					lf w3 = rand() % 100;
					vec3 pos = (tetra.points[i] * w1 + tetra.points[j] * w2 + tetra.points[k] * w3) / (w1 + w2 + w3);
					if (collides(position, pos)) return true;
				}
			}
		}
	}
	return false;
}

int N = 10;

void MyWin::MainLoop()
{

	Tetras tetras;

	for (int X = 0; X < N; X++){
		for (int Y = 0; Y < N; Y++){
			for (int Z = 0; Z < N; Z++){
				vec3 origin(lf(X) / lf(N - 1), lf(Y) / lf(N - 1), lf(Z) / lf(N - 1));
				tetras.vec.pb(Tetra(origin));
			}
		}	
	}

	tetras.setBuffers();
	
	Tetra* last = &tetras.vec.back();

	Background background;
	Sphere sphere(vec3(-0.1, 0.0, 0.0));
	
	lf prev_timer = 0.0;
	lf frame_len = 0.001;
	float horizontalAngle = PI / 4;
	float verticalAngle = 0.5f;
	float initialFoV = 45.0f;
	float speed = 1.0f;
	float mouseSpeed = 0.05f;
	float rotSpeed = 1.0f;
	float cameraDistance = 0.2;
	bool mouse_control = false;

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		AGLErrors("main-loopbegin");
		
		lf timer = glfwGetTime();
		lf deltaTime = timer - prev_timer;
		if (deltaTime < frame_len) continue;
		prev_timer = timer;

		AGLErrors("main-afterdraw");
		
		glm::vec3 direction(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);	
		
		auto display = [&](int mode)
		{
			if (mode == 0) glViewport(0, 0, wd / 2, ht);
			if (mode == 1) glViewport(wd / 2, 0, wd / 2, ht);

			vec3 position = sphere.position;

			if (mode == 1) position -= cameraDistance * direction;

			mat4 CameraMatrix = lookAt(position, position + direction, vec3(0.0, 1.0, 0.0));
			mat4 projectionMatrix = glm::perspective(lf(glm::radians(45.0)), lf(wd / 2) / lf(ht), 0.1f, 100.0f);
			mat4 MVP = projectionMatrix * CameraMatrix;
			background.draw(timer, MVP);
			tetras.draw(MVP);
			if (mode == 1) sphere.draw(MVP);
		};
		
		display(0);
		display(1);
		
		glfwPollEvents();
		glfwSwapBuffers(win());
		
		auto backup_position = sphere.position;

		if (glfwGetKey(win(), GLFW_KEY_A) == GLFW_PRESS) sphere.position += direction * deltaTime * speed;
		if (glfwGetKey(win(), GLFW_KEY_Z) == GLFW_PRESS) sphere.position -= direction * deltaTime * speed;
		if (glfwGetKey(win(), GLFW_KEY_LEFT)  == GLFW_PRESS) horizontalAngle += deltaTime * rotSpeed;
		if (glfwGetKey(win(), GLFW_KEY_RIGHT) == GLFW_PRESS) horizontalAngle -= deltaTime * rotSpeed;
		if (glfwGetKey(win(), GLFW_KEY_UP)    == GLFW_PRESS) verticalAngle += deltaTime * rotSpeed;
		if (glfwGetKey(win(), GLFW_KEY_DOWN)  == GLFW_PRESS) verticalAngle -= deltaTime * rotSpeed;
		if (glfwGetKey(win(), GLFW_KEY_M)  == GLFW_PRESS){
			if (mouse_control){
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}else{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			mouse_control ^= 1;
		}
		if (mouse_control){
			double xpos, ypos;
			double cx = wd / 2;
			double cy = ht / 2;
			glfwGetCursorPos(window, &xpos, &ypos);
			double dx = cx - xpos;
			double dy = cy - ypos;
			horizontalAngle += mouseSpeed * deltaTime * dx;
			verticalAngle   += mouseSpeed * deltaTime * dy;	
			glfwSetCursorPos(window, cx, cy);
		}

		verticalAngle = min(verticalAngle, +PI / 2.0f - 1e-5f);
		verticalAngle = max(verticalAngle, -PI / 2.0f + 1e-5f);
		
		if (collides(sphere.position, *last)){
			printf("YOU WIN!\n");
			printf("Time: %fs\n", timer);
			exit(0);
		}

		for (Tetra tetra : tetras.vec){
			if (collides(sphere.position, tetra)){
				sphere.position = backup_position;
				break;
			}
		}
	
	} while (glfwGetKey(win(), GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(win()) == 0);
}

int main(int argc, char *argv[]) {
	if (argc > 1){
		srand(atoi(argv[1]));
	}else{
		srand(6);
	}
	if (argc > 2){
		N = atoi(argv[2]);
	}
	MyWin win;
	win.Init(2048, 1024, "Pracownia 4", 0, 33);
	win.MainLoop();
	return 0;
}
