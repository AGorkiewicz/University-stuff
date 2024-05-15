#pragma once
#include "utils.cpp"

struct MyWindow
{
	GLFWwindow* ptr = NULL;

	int mx, my;

	MyWindow(int width, int height, const char* name) : mx(width), my(height)
	{
		if (!glfwInit()){
			glfwTerminate();
			throw runtime_error("Error initializing glfw.");
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);

		ptr = glfwCreateWindow(mx, my, name, nullptr, nullptr);

		if (ptr == NULL){
			glfwTerminate();
			throw runtime_error("Error creating window.");
		}

		glfwMakeContextCurrent(ptr);

		glfwSetWindowUserPointer(ptr, this);

		glfwSetWindowSizeCallback(ptr, CallbackResize);
	}

	void resize(int width, int height) {
		mx = width, my = height;
		glViewport(0, 0, mx, my);
	}

	static void CallbackResize(GLFWwindow* window, int cx, int cy) {
		static_cast<MyWindow*>(glfwGetWindowUserPointer(window))->resize(cx, cy);
	}
};
