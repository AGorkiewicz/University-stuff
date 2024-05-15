#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#pragma once

struct MyWindow
{
	GLFWwindow* ptr = NULL;

	int mx, my;

	MyWindow(int width, int height, const char* name) : mx(width), my(height)
	{
		glfwInit();
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
		ptr = glfwCreateWindow(mx, my, name, nullptr, nullptr);
	
		glfwMakeContextCurrent(ptr);

		glfwSetWindowUserPointer(ptr, this);
		
		glfwSetWindowSizeCallback(ptr, CallbackResize);

		//glfwSetKeyCallback(        ptr, AGLWindow::CallbackKey );
		//glfwSetMouseButtonCallback(ptr, AGLWindow::CallbackMouseButton );
		//glfwSetScrollCallback(     ptr, AGLWindow::CallbackScroll );
		//glfwSetCursorPosCallback(  ptr, AGLWindow::CallbackMousePos );
	
		glEnable(GL_MULTISAMPLE);
	}
	
	void resize(int width, int height) {
		mx = width;
		my = height;
		
		glViewport(0, 0, mx, my);
	}
	
	static void CallbackResize(GLFWwindow* window, int cx, int cy) {
		static_cast<MyWindow*>(glfwGetWindowUserPointer(window))->resize(cx, cy);
	}
};
