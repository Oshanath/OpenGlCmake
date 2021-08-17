#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include "VAO.h"
#include "Camera.h"
#include "shaderClass.h"
#include "EBO.h"
#include "Texture.h"

class Mesh {

public:
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
	void draw(Camera& camera, Shader& shader, 
		std::vector<Texture>& textures,
		glm::mat4 localTransformation = glm::mat4(1.0f),
		glm::mat4 globalTransformation = glm::mat4(1.0f));

private:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	VAO vao;
};
