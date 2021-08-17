#pragma once

#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

GLFWwindow* initWindow(int width, int height, const char* name) {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);

	if (window == NULL) {
		std::cout << "Window failed to create" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);
	
	gladLoadGL();
	glViewport(0, 0, width, height);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapInterval(1);
	glfwSwapBuffers(window);

	return window;
}
