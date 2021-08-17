#pragma once

#include "Mesh.h"
#include "Helpers.h"
#include <json/json.h>

using Json = nlohmann::json;

struct Entity {
	unsigned int meshIndex;
	glm::mat4 transform;
};

class Model {
public:
	glm::vec3 translation;
	glm::quat rotation;
	float scalar;

	Model(const char* file);
	void draw(Camera& camera, Shader& shader);
	void translate(float x, float y, float z);
	void rotate(float x, float y, float z);
	void scale(float scalar);

private:
	std::vector<Mesh> meshes;
	std::vector<Entity> entities;
	std::vector<Texture> textures;
	Json json;

	void loadMeshes(std::vector<std::string> buffers);
	std::vector<unsigned char> getDataFromAccessor(Json accessor, std::vector<std::string> buffers);
	std::vector<glm::vec3> makeVec3(std::vector<unsigned char> data);
	std::vector<glm::vec2> makeVec2(std::vector<unsigned char> data);
	std::vector<GLuint> groupIndices(Json accessor, std::vector<unsigned char> data);
	glm::mat4 getTransform();
	void traverseNode(unsigned int nodeIndex = 0, glm::mat4 parentTransform = glm::mat4(1.0f));
	void loadTextures(std::string filePath);
};
