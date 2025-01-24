#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;
//uniform vec3 lightPointColor;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//Fog control
uniform bool enableFog;

//Night mode
uniform bool enableNight;

//Shadow mode
uniform bool enableShadow;

//Point light
uniform bool enablePointLight;
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightPos3;
uniform vec3 lightPos4;
uniform mat4 view;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
float shadow=1;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}
float computeShadow()
{
	// perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Transform to [0,1] range
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	// Get closest depth value from light's perspective
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	// Get depth of current fragment from light's perspective
	float currentDepth = normalizedCoords.z;
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	// Check whether current frag pos is in shadow
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
	return shadow;
}

float applyFog() {
    	float fogDensity = 0.1f;
    	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
	return clamp(fogFactor, 0.0f, 1.0f);
}

float constant = 1.0f;
float linear = 1.0f;
float quadratic = 1.0f;
vec3 computePointLight(vec4 lightPosEye)
{
	vec3 cameraPosEye = vec3(0.0f);
    	vec3 normalEye = normalize(fNormal);
    	vec3 lightDirN = normalize(lightPosEye.xyz - fPosEye.xyz);
    	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
    	vec3 ambient = ambient * lightColor;
    	vec3 diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
    	vec3 halfVector = normalize(lightDirN + viewDirN);
    	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
    	vec3 specular = specularStrength * specCoeff * lightColor;
    	float distance = length(lightPosEye.xyz - fPosEye.xyz);
    	float att = 1.0f / (constant + linear * distance + quadratic * distance * distance);
    	return (ambient + diffuse + specular) * att * vec3(1.0f, 1.0f, 1.0f);
}

void main() 
{
	//Fragment discarding
	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords); 
	if (colorFromTexture.a < 0.1) discard; 	

	computeLightComponents();
	
	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;
	
	//shadow = computeShadow();
	if (enableShadow) {
		shadow = computeShadow();
	} else {
		shadow = 0.0f;
	}

	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);  	

	if (enablePointLight == true) {
		vec4 lightPosEye1 = view * vec4(lightPos1, 1.0f);
		color+=computePointLight(lightPosEye1);	
		vec4 lightPosEye2 = view * vec4(lightPos2, 1.0f);
		color+=computePointLight(lightPosEye2);
		vec4 lightPosEye3 = view * vec4(lightPos3, 1.0f);
		color+=computePointLight(lightPosEye3);
		vec4 lightPosEye4 = view * vec4(lightPos4, 1.0f);
		color+=computePointLight(lightPosEye4);
	}

	if (enableFog) { 
		vec4 colorfinal = vec4(color, 0.3f);
        	float fogFactor = applyFog();
		vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
		fColor = fogColor * (1-fogFactor) + colorfinal * fogFactor;
     	} else {
		fColor = vec4(color, 0.3f);
	}

	if (enableNight) {
		fColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	} 	
}
