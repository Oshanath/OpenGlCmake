#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices):
	vertices(vertices), 
	indices(indices) {

	vao.Bind();
	VBO vbo(vertices);
	EBO ebo(indices);

	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	vao.LinkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));

	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();
}

void Mesh::draw(Camera& camera, Shader& shader, 
	std::vector<Texture>& textures,
	glm::mat4 localTransformation, 
	glm::mat4 globalTransformation) {

	shader.Activate();
	vao.Bind();

	for (Texture& texture : textures) {

		texture.texUnit(shader, texture.type, texture.unit);
		texture.Bind();

	}

	glUniform3f(glGetUniformLocation(shader.ID, "cameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
	glm::mat4 cameraMatrix = camera.getMatrix();
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));

	glm::mat4 modelMatrix = globalTransformation * localTransformation;
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
