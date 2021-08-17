#include "Model.h"
#include "WindowInit.h"
#include "LightSource.h"

void addModelsAndLights(std::vector<Model*>& models, std::vector<LightSource*>& lightSources) {
	//Model* bunny = new Model("models/bunny/scene.gltf");
	Model* crate = new Model("../models/crate/crate.gltf");
	//Model* map = new Model("models/map/scene.gltf");
	//Model* sword = new Model("models/sword/scene.gltf");

	//Model* grindstone = new Model("models/grindstone/scene.gltf");
	//grindstone->scale(0.05f);
	//grindstone->translate(0.0f, 0.0f, -30.0f);

	//models.push_back(bunny);
	models.push_back(crate); 
	//models.push_back(map);
	//models.push_back(sword);
	//models.push_back(grindstone);

	Model* cube = new Model("../models/cube/cube.gltf");
	cube->translate(0.0f, 8.0f, 0.0f);
	LightSource* cubeLight = new LightSource(*cube, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), SPOT_LIGHT);
	cubeLight->direction = glm::vec3(0.0f, -1.0f, 0.0f);
	cubeLight->linear = 0.1;
	cubeLight->quadratic = 0.1;
	cubeLight->innerCutoffDegrees = 10;
	cubeLight->outerCutoffDegrees = 30;
	lightSources.push_back(cubeLight);
}

int main() {

	const unsigned int width = 1800;
	const unsigned int height = 1000;

	//Create window
	GLFWwindow* window = initWindow(width, height, "OpenGL Tutorial");
	if (window == NULL)
		return -1;

	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(0.0f, 3.0f, 15.0f));
	Shader shader("../shaders/default.vert", "../shaders/default.frag");

	std::vector<Model*> models;
	std::vector<LightSource*> lightSources;

	addModelsAndLights(models, lightSources);

	while (!glfwWindowShouldClose(window))
	{

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);

		camera.updateMatrix(45.0f, 0.1f, 100.0f);
		glUniform3fv(glGetUniformLocation(shader.ID, "cameraPosition"), 1, glm::value_ptr(camera.Position));

		sendLightData(shader, lightSources);

		glUniform1i(glGetUniformLocation(shader.ID, "isLightSource"), 0);
		for (Model* model : models) {
			model->draw(camera, shader);
		}

		glUniform1i(glGetUniformLocation(shader.ID, "isLightSource"), 1);
		for (LightSource* lightSource : lightSources) {
			lightSource->draw(camera, shader);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for (Model* model : models) {
		delete model;
	}

	shader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}