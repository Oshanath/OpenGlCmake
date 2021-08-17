#include "LightSource.h"

LightSource::LightSource(Model model, glm::vec4 color, LightType type) :
	model(model), color(color), type(type), linear(0.14f), quadratic(0.07f), direction(glm::vec3(0.0f, -1.0f, 0.0f)),
	innerCutoffDegrees(10), outerCutoffDegrees(30){}

void LightSource::draw(Camera& camera, Shader& shader) {
	model.draw(camera, shader);
}
glm::vec3 LightSource::getPosition() {
	return model.translation;
}

void sendLightData(Shader& shader, std::vector<LightSource*> lightSources) {

	std::vector<glm::vec4> lightColors;
	for (LightSource* lightSource : lightSources) {
		lightColors.push_back(lightSource->color);
	}

	std::vector<glm::vec3> lightPositions;
	for (LightSource* lightSource : lightSources) {
		lightPositions.push_back(lightSource->getPosition());
	}

	std::vector<GLuint> lightTypes;
	for (LightSource* lightSource : lightSources) {
		lightTypes.push_back(lightSource->type);
	}

	std::vector<GLfloat> attenuationLinears;
	for (LightSource* lightSource : lightSources) {
		attenuationLinears.push_back(lightSource->linear);
	}

	std::vector<GLfloat> attenuationQuadratics;
	for (LightSource* lightSource : lightSources) {
		attenuationQuadratics.push_back(lightSource->quadratic);
	}

	std::vector<glm::vec3> lightPointingDirections;
	for (LightSource* lightSource : lightSources) {
		lightPointingDirections.push_back(lightSource->direction);
	}

	std::vector<GLfloat> innerCutoffDegrees;
	for (LightSource* lightSource : lightSources) {
		innerCutoffDegrees.push_back(lightSource->innerCutoffDegrees);
	}

	std::vector<GLfloat> outerCutoffDegrees;
	for (LightSource* lightSource : lightSources) {
		outerCutoffDegrees.push_back(lightSource->outerCutoffDegrees);
	}

	glUniform4fv(glGetUniformLocation(shader.ID, "lightColors"), lightSources.size(), glm::value_ptr(lightColors[0]));
	glUniform3fv(glGetUniformLocation(shader.ID, "lightPositions"), lightPositions.size(), glm::value_ptr(lightPositions[0]));
	glUniform1uiv(glGetUniformLocation(shader.ID, "lightTypes"), lightTypes.size(), &lightTypes[0]);
	glUniform1fv(glGetUniformLocation(shader.ID, "lightLinears"), attenuationLinears.size(), &attenuationLinears[0]);
	glUniform1fv(glGetUniformLocation(shader.ID, "lightQuadratics"), attenuationQuadratics.size(), &attenuationQuadratics[0]);
	glUniform3fv(glGetUniformLocation(shader.ID, "lightPointingDirections"), lightPointingDirections.size(), glm::value_ptr(lightPointingDirections[0]));
	glUniform1fv(glGetUniformLocation(shader.ID, "innerCutoffDegrees"), innerCutoffDegrees.size(), &innerCutoffDegrees[0]);
	glUniform1fv(glGetUniformLocation(shader.ID, "outerCutoffDegrees"), outerCutoffDegrees.size(), &outerCutoffDegrees[0]);

}
