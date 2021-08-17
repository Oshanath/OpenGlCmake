#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

out vec2 texCoord;
out vec3 normal;
out vec3 fragmentPosition;

void main(){

	texCoord = aTex;
	normal = aNormal;
	fragmentPosition = vec3(modelMatrix * vec4(aPos, 1.0f));
	gl_Position = cameraMatrix * modelMatrix * vec4(aPos, 1.0f);

}
