#pragma once

#include "Model.h"

enum LightType {
	DIRECT_LIGHT = 0,
	POINT_LIGHT = 1,
	SPOT_LIGHT = 2
};

class LightSource {
public:
	glm::vec4 color;
	Model model;
	LightType type;

	//Attenuation values
	float linear;		// Brightness
	float quadratic;	//Maximum distance

	glm::vec3 direction;
	float innerCutoffDegrees;
	float outerCutoffDegrees;

	LightSource(Model model, glm::vec4 color, LightType type);
	void draw(Camera& camera, Shader& shader);
	glm::vec3 getPosition();

};

void sendLightData(Shader& shader, std::vector<LightSource*> lightSources);
