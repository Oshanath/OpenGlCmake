#include "Model.h"
#include <cctype>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

Model::Model(const char* file): 
	translation(glm::vec3(0.0f, 0.0f, 0.0f)), 
	rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
	scalar(1.0f){

	if (file != "") {
		std::string stdFileName = std::string(file);
		std::string filePath = stdFileName.substr(0, stdFileName.find_last_of("/") + 1);

		std::string gltfFile = get_file_contents(file);
		json = Json::parse(gltfFile);

		unsigned int numberOfBuffers = json["buffers"].size();
		std::vector<std::string> buffers;

		for (int i = 0; i < numberOfBuffers; i++) {
			std::string buffer = get_file_contents((filePath + std::string(json["buffers"][i]["uri"])).c_str());
			buffers.push_back(buffer);
		}

		loadMeshes(buffers);

		if (json.find("scene") == json.end()) {
			std::cout << "scene not found" << std::endl;
			throw(errno);
		}

		unsigned int sceneIndex = json["scene"];
		std::vector<unsigned int> nodeIndices = json["scenes"][sceneIndex]["nodes"];

		for (unsigned int nodeIndex : nodeIndices) {
			traverseNode(nodeIndex);
		}

		loadTextures(filePath);
	}

}

void Model::draw(Camera& camera, Shader& shader) {

	for (Entity entity : entities) {
		Mesh& mesh = meshes[entity.meshIndex];
		mesh.draw(camera, shader, textures, entity.transform, getTransform());
	}

}

void Model::loadMeshes(std::vector<std::string> buffers) {

	unsigned int numberOfMeshes = json["meshes"].size();
	bool firstTime = true;
	
	for (int i = 0; i < numberOfMeshes; i++) {

		unsigned int positionAccessorIndex = json["meshes"][i]["primitives"][0]["attributes"]["POSITION"];
		std::vector<unsigned char> positionData = getDataFromAccessor(json["accessors"][positionAccessorIndex], buffers);
		std::vector<glm::vec3> positions = makeVec3(positionData);

		unsigned int normalAccessorIndex = json["meshes"][i]["primitives"][0]["attributes"]["NORMAL"];
		std::vector<unsigned char> normalData = getDataFromAccessor(json["accessors"][normalAccessorIndex], buffers);
		std::vector<glm::vec3> normals = makeVec3(normalData);

		unsigned int texUVAccessorIndex = json["meshes"][i]["primitives"][0]["attributes"]["TEXCOORD_0"];
		std::vector<unsigned char> texUVData = getDataFromAccessor(json["accessors"][texUVAccessorIndex], buffers);
		std::vector<glm::vec2> texUVs = makeVec2(texUVData);

		unsigned int indicesAccessorIndex = json["meshes"][i]["primitives"][0]["indices"];
		std::vector<unsigned char> indicesData = getDataFromAccessor(json["accessors"][indicesAccessorIndex], buffers);
		std::vector<GLuint> indices = groupIndices(json["accessors"][indicesAccessorIndex], indicesData);

		std::vector<Vertex> vertices;

		for (unsigned int i = 0; i < positions.size(); i++) {
			vertices.push_back(Vertex{ positions[i], normals[i], texUVs[i]});
		}

		meshes.push_back(Mesh(vertices, indices));

	}

}

std::vector<GLuint> Model::groupIndices(Json accessor, std::vector<unsigned char> data) {

	unsigned int componentType = accessor["componentType"];
	unsigned int numberOfBytesPerElement;

	if (componentType == 5120 || componentType == 5121) numberOfBytesPerElement = 1;
	else if (componentType == 5122 || componentType == 5123) numberOfBytesPerElement = 2;
	else if (componentType == 5125) numberOfBytesPerElement = 4;
	else {
		std::cout << "Component type error" << std::endl;
		throw(errno);
	}

	std::vector<GLuint> indices;
	unsigned char* temp = new unsigned char[numberOfBytesPerElement];
	unsigned int currentIndex = 0;

	while (currentIndex < data.size()) {
		for (unsigned int i = 0; i < numberOfBytesPerElement; i++) {
			temp[i] = data[currentIndex++];
		}

		GLuint value = 0;
		memcpy(&value, temp, numberOfBytesPerElement);
		indices.push_back(value);
	}

	delete temp;
	return indices;

}

std::vector<glm::vec3> Model::makeVec3(std::vector<unsigned char> data) {

	std::vector<glm::vec3> vec3List;

	for (unsigned int i = 0; i < data.size(); i) {
		
		float floats[3];

		for (unsigned int j = 0; j < 3; j++) {
			unsigned char temp[] = { data[i++], data[i++], data[i++], data[i++] };
			memcpy(&floats[j], temp, sizeof(float));
		}

		vec3List.push_back(glm::vec3(floats[0], floats[1], floats[2]));
	}

	return vec3List;

}

std::vector<glm::vec2> Model::makeVec2(std::vector<unsigned char> data) {

	std::vector<glm::vec2> vec2List;

	for (unsigned int i = 0; i < data.size(); i) {

		float floats[2];

		for (unsigned int j = 0; j < 2; j++) {
			unsigned char temp[] = { data[i++], data[i++], data[i++], data[i++] };
			memcpy(&floats[j], temp, sizeof(float));
		}

		vec2List.push_back(glm::vec2(floats[0], floats[1]));
	}

	return vec2List;

}

std::vector<unsigned char> Model::getDataFromAccessor(Json accessor, std::vector<std::string> buffers) {

	unsigned int bufferViewIndex = accessor["bufferView"];
	unsigned int bufferViewByteOffset = json["bufferViews"][bufferViewIndex].value("byteOffset", 0);

	unsigned int componentType = accessor["componentType"];
	unsigned int numberOfBytesPerElement;

	if (componentType == 5120 || componentType == 5121) numberOfBytesPerElement = 1;
	else if(componentType == 5122 || componentType == 5123) numberOfBytesPerElement = 2;
	else if (componentType == 5125 || componentType == 5126) numberOfBytesPerElement = 4;
	else {
		std::cout << "Component type error" << std::endl;
		throw(errno);
	}

	std::string accessorType = accessor["type"];
	unsigned int numberOfComponents;

	if (accessorType == "SCALAR") numberOfComponents = 1;
	else if (accessorType == "VEC2") numberOfComponents = 2;
	else if (accessorType == "VEC3") numberOfComponents = 3;
	else if (accessorType == "VEC4") numberOfComponents = 4;
	else if (accessorType == "MAT2") numberOfComponents = 4;
	else if (accessorType == "MAT3") numberOfComponents = 9;
	else if (accessorType == "MAT4") numberOfComponents = 16;
	else {
		std::cout << "Accessor type error" << std::endl;
		throw(errno);
	}

	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	unsigned int bufferIndex = json["bufferViews"][bufferViewIndex]["buffer"];

	std::string buffer = buffers[bufferIndex];
	std::vector<unsigned char> accessorData;

	for (unsigned int i = accessorByteOffset + bufferViewByteOffset; i < accessorByteOffset + bufferViewByteOffset + count * numberOfBytesPerElement * numberOfComponents; i++) {
		accessorData.push_back(buffer[i]);
	}

	return accessorData;
	
}

glm::mat4 Model::getTransform() {

	glm::mat4 translationMatrix = glm::mat4(1.0f);
	translationMatrix = glm::translate(translationMatrix, translation);

	glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

	glm::vec3 scaleVector = glm::vec3(scalar, scalar, scalar);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	scaleMatrix = glm::scale(scaleMatrix, scaleVector);

	return translationMatrix * rotationMatrix * scaleMatrix;

}


void Model::traverseNode(unsigned int nodeIndex, glm::mat4 parentTransform) {

	glm::mat4 currentTransform = parentTransform;

	if (json["nodes"][nodeIndex].find("matrix") != json["nodes"][nodeIndex].end()) {
		std::vector<float> matrixValues = json["nodes"][nodeIndex]["matrix"];
		glm::mat4 matrix = glm::make_mat4(&matrixValues[0]);
		currentTransform = currentTransform * matrix;
	}
	else {
		glm::mat4 matrix = glm::mat4(1.0f);

		if (json["nodes"][nodeIndex].find("scale") != json["nodes"][nodeIndex].end()) {
			std::vector<float> scaleValues = json["nodes"][nodeIndex]["scale"];
			glm::vec3 scaleVector = glm::make_vec3(&scaleValues[0]);
			glm::mat4 scaleMatrix = glm::mat4(1.0f);
			scaleMatrix = glm::scale(scaleMatrix, scaleVector);
			matrix = scaleMatrix * matrix;
		}

		if (json["nodes"][nodeIndex].find("rotation") != json["nodes"][nodeIndex].end()) {
			std::vector<float> rotationValues = json["nodes"][nodeIndex]["rotation"];
			glm::quat rotationQuaternion = glm::make_quat(&rotationValues[0]);
			glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuaternion);
			matrix = rotationMatrix * matrix;
		}

		if (json["nodes"][nodeIndex].find("translation") != json["nodes"][nodeIndex].end()) {
			std::vector<float> translationValues = json["nodes"][nodeIndex]["translation"];
			glm::vec3 translationVector = glm::make_vec3(&translationValues[0]);
			glm::mat4 translationMatrix = glm::mat4(1.0f);
			translationMatrix = glm::translate(translationMatrix, translationVector);
			matrix = translationMatrix * matrix;
		}

		currentTransform = currentTransform * matrix;

	}

	if (json["nodes"][nodeIndex].find("mesh") != json["nodes"][nodeIndex].end()) {
		unsigned int meshIndex = json["nodes"][nodeIndex]["mesh"];

		entities.push_back(Entity{ meshIndex, currentTransform });
	}


	if (json["nodes"][nodeIndex].find("children") != json["nodes"][nodeIndex].end()) {
		std::vector<unsigned int> childrenNodeIndices = json["nodes"][nodeIndex]["children"];

		for (unsigned int childNodeIndex : childrenNodeIndices) {
			traverseNode(childNodeIndex, currentTransform);
		}
	}

}

void Model::translate(float x, float y, float z) {
	translation = glm::vec3(x, y, z);
}

void Model::rotate(float x, float y, float z) {
	rotation = glm::quat(glm::vec3(x, y, z));
}

void Model::scale(float scalar) {
	this->scalar = scalar;
}

std::string toLowerCase(std::string str) {

	std::string str2;

	for (char c : str) {
		str2 += tolower(c);
	}

	return str2;
}

void Model::loadTextures(std::string filePath) {

	unsigned int numberOfTextures = json["textures"].size();

	for (unsigned int i = 0; i < numberOfTextures; i++) {

		unsigned int imageIndex = json["textures"][i]["source"];
		std::string imageUri = json["images"][imageIndex]["uri"];
		std::string imagePath = filePath + imageUri;
		std::string imagePathLowerCase = toLowerCase(imagePath);
		std::string texType;
		GLuint slot;
		
		if (imagePathLowerCase.find("base") != std::string::npos) {
			texType = "diffuse";
			slot = DIFFUSE_TEXTURE_UNIT;
		}
		else if (imagePathLowerCase.find("metallic") != std::string::npos ||
			     imagePathLowerCase.find("roughness") != std::string::npos) {
			texType = "specular";
			slot = SPECULAR_TEXTURE_UNIT;
		}
		else {
			texType = "unknown";
			slot = 2;
		}

		Texture texture(imagePath.c_str(), texType.c_str(), slot);
		textures.push_back(texture);

	}

}