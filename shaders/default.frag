#version 330 core

const uint DIRECT_LIGHT = uint(0);
const uint POINT_LIGHT = uint(1);
const uint SPOT_LIGHT = uint(2);

out vec4 FragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragmentPosition;

const int numberOfLightSources = 1;

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform vec4 lightColors[numberOfLightSources];
uniform vec3 lightPositions[numberOfLightSources];
uniform vec3 cameraPosition;
uniform int isLightSource;
uniform uint lightTypes[numberOfLightSources];
uniform float lightLinears[numberOfLightSources];
uniform float lightQuadratics[numberOfLightSources];
uniform vec3 lightPointingDirections[numberOfLightSources];
uniform float innerCutoffDegrees[numberOfLightSources];
uniform float outerCutoffDegrees[numberOfLightSources];

void main(){

	vec4 pixelColor = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 diffuseTextureColor = texture(diffuse, texCoord);
	vec4 specularTextureColor = texture(specular, texCoord);
	vec3 Normal = normalize(normal);
	vec3 lightDirections[numberOfLightSources];
	float attenuations[numberOfLightSources];

	//calculate light directions and attenuations

	for(int i = 0; i < numberOfLightSources; i++){
		vec3 lightDirection;

		if(lightTypes[i] == DIRECT_LIGHT){
			lightDirection = normalize(lightPositions[i]);
		}
		else{
			lightDirection = normalize(lightPositions[i] - fragmentPosition);
		}

		lightDirections[i] = lightDirection;

		float distance = length(lightPositions[i] - fragmentPosition);
		attenuations[i] = 1.0/(0.1 + distance * lightLinears[i] + lightQuadratics[i] * distance * distance);
	}

	//Ambient Lighting
	float ambientIntensity = 0.2;
	vec4 ambientColor = vec4(1.0, 1.0, 1.0, 1.0) * ambientIntensity;

	for(int i = 1; i < numberOfLightSources; i++){
		ambientColor *= lightColors[i];
	}

	if(isLightSource == 1){
		ambientColor = vec4(1.0, 1.0, 1.0, 1.0);
	}

	pixelColor = diffuseTextureColor * ambientColor;




	//Diffuse lighting
	for(int i = 0; i < numberOfLightSources; i++){
		float diffuseIntensity = max(dot(Normal, lightDirections[i]), 0.0);
		vec4 diffuse = diffuseIntensity * lightColors[i];

		if(isLightSource == 0){
			if(lightTypes[i] == DIRECT_LIGHT){
				pixelColor += diffuseTextureColor * diffuse;
			}
			else if(lightTypes[i] == POINT_LIGHT){
				pixelColor += diffuseTextureColor * diffuse * attenuations[i];
			}
			else if(lightTypes[i] == SPOT_LIGHT){
				float theta = dot(lightDirections[i], normalize(-lightPointingDirections[i]));
				float innerCutoff = cos(radians(innerCutoffDegrees[i]));
				float outerCutoff = cos(radians(outerCutoffDegrees[i]));
				float epsilon = (innerCutoff - outerCutoff);
				float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
			
				pixelColor += diffuseTextureColor * diffuse * intensity * attenuations[i];
			}
			
		}
	}

	//Specular Lighting
	float specularIntensity = 0.5;
	vec3 viewDirection = normalize(cameraPosition - fragmentPosition);

	for(int i = 0; i < numberOfLightSources; i++){
		vec3 reflectionDirection = reflect(normalize(-lightDirections[i]), Normal);
		float specularvalue = pow(max(dot(reflectionDirection, viewDirection), 0.0), 16);
		vec4 specular = specularIntensity * specularvalue * lightColors[i];

		if(lightTypes[i] == DIRECT_LIGHT){
			pixelColor += specularTextureColor * specular;
		}
		else if(lightTypes[i] == POINT_LIGHT){
			pixelColor += specularTextureColor * specular * attenuations[i];
		}
		else if(lightTypes[i] == SPOT_LIGHT){
			float theta = dot(lightDirections[i], normalize(-lightPointingDirections[i]));
			float innerCutoff = cos(radians(innerCutoffDegrees[i]));
			float outerCutoff = cos(radians(outerCutoffDegrees[i]));
			float epsilon = (innerCutoff - outerCutoff);
			float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

			pixelColor += specularTextureColor * specular * intensity * attenuations[i];
		}
	}



	FragColor = pixelColor;
}
