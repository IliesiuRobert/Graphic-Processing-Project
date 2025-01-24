//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include <iostream>

int glWindowWidth = 1600;
int glWindowHeight = 1200;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

// Lumina directionala
glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

// Lumina punctiforma
bool enablePointLight = false;
glm::vec3 lightPos1 = glm::vec3(-1.41646f, -0.661937f, -3.34227f);
glm::vec3 lightPos2 = glm::vec3(-1.87724f, -0.657924f, 0.528539f);
glm::vec3 lightPos3 = glm::vec3(-2.66135f, -0.684016f, 3.64273f);
glm::vec3 lightPos4 = glm::vec3(-3.83808f, -0.684016f, 7.0469f);
GLuint lightPos1Loc;
GLuint lightPos2Loc;
GLuint lightPos3Loc;
GLuint lightPos4Loc;

gps::Camera myCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.1f;

// Rotati cu axele
float pitch = 0.0f;
float yaw = -90.0f;
float lastX = 400, lastY = 300;
bool firstMoveMouse = true;

// Animatie 
bool isPresentationActive = false;
float aux = 0.0f;
float aux2 = 0.0f;
float aux3 = 0.0f;

// Vizualizare scena
GLenum polygonMode = GL_FILL;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D scena;
gps::Model3D screenQuad;
gps::Model3D bottle1;
gps::Model3D bottle2;
gps::Model3D bottle3; 
gps::Model3D bottle4;
gps::Model3D bottle5;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool enableShadow = false;
GLuint shadowsEnabledLoc;
bool showDepthMap;

// Ceata
bool enableFog = false;
GLint enableFogLoc;

// Noapte
bool enableNight = false;
GLint isNightLoc;
GLint nightLightColorLoc;
glm::vec3 nightLightColor = glm::vec3(0.02f, 0.02f, 0.02f);

// Ploaie
bool enableRain = false;
gps::Model3D RainDrop;
struct RainDrops {
	glm::vec3 position;
	glm::vec3 velocity;
	float size;
};
std::vector<RainDrops> raindrops;

// Animation
bool enableAnimation = false;
GLfloat angleElice = 0.0f;
glm::mat4 modelElice;
gps::Model3D eliceMoara1;

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	// Prezentarea
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		isPresentationActive = !isPresentationActive;
	}

	// Solid
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		polygonMode = GL_FILL;
	}

	// Wireframe
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		polygonMode = GL_LINE;
	}

	// Poligonal
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		polygonMode = GL_POINT;
	}

	// Activeaza ceata
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		enableFog = !enableFog;
	}

	// Activeaza noaptea
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		enableNight = !enableNight;
	}

	// Activeaza umbra
	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
		enableShadow = !enableShadow;
	}

	// Activeaza lumina punctiforma
	if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
		enablePointLight = !enablePointLight;
	}

	// Activeaza ploaia
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		enableRain = !enableRain;
	}

	// Activeaza animatia
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		enableAnimation = !enableAnimation;
	}

	// Returneaza pozitia camerei de vizualizare
	if (key == GLFW_KEY_K && action == GLFW_PRESS) {
		std::cout << "Camera Position: " << myCamera.getCameraPosition().x << " " << myCamera.getCameraPosition().y << " " << myCamera.getCameraPosition().z << std::endl;
		std::cout << "Camera Target: " << myCamera.getCameraTarget().x << " " << myCamera.getCameraTarget().y << " " << myCamera.getCameraTarget().z << std::endl;
		pressedKeys[GLFW_KEY_K] = false;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMoveMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMoveMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.05f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	myCamera.rotate(yOffset, xOffset);
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_Z]) {
		myCamera.rotate(0.0f, -1.0f);
	}

	if (pressedKeys[GLFW_KEY_C]) {
		myCamera.rotate(0.0f, 1.0f);
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	//for sRBG framebuffer
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	//for antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);
}

// Initializam ploaia
void initRainDrop(int numRaindrops, float groundHeight) {
	for (int i = 0; i < numRaindrops; i++) {
		RainDrops drops;
		drops.position = glm::vec3(rand() % 25 - 13,
			                       rand() % 20 + 5,
			                       rand() % 25 - 13);
		
		drops.velocity = glm::vec3(0.0f, -9.8f, 0.0f);
		drops.size = 0.7;
		
		raindrops.push_back(drops);
	}
}	

void initObjects() {
	scena.LoadModel("objects/ProiectFinal.obj", "textures/");
	screenQuad.LoadModel("objects/quad/quad.obj");
	RainDrop.LoadModel("objects/PicaturaPloaie.obj", "textures/");
	bottle1.LoadModel("objects/BeerBottle.obj", "textures/");
	bottle2.LoadModel("objects/BeerBottle.obj", "textures/");
	bottle3.LoadModel("objects/BeerBottle.obj", "textures/");
	bottle4.LoadModel("objects/BeerBottle.obj", "textures/");
	bottle5.LoadModel("objects/BeerBottle.obj", "textures/");
	eliceMoara1.LoadModel("objects/elice.obj", "textures/");
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();
	depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
	depthMapShader.useShaderProgram();
}

glm::mat4 computeLightSpaceTrMatrix() {
	glm::mat4 lightView = glm::lookAt(glm::inverseTranspose(glm::mat3(lightRotation)) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = -6.0f, far_plane = 20.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	//initializeaza ceata
	enableFogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "enableFog");
	glUniform1i(enableFogLoc, enableFog);

	//initializeaza nopatea
	nightLightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "nightColor");
	glUniform3fv(nightLightColorLoc, 1, glm::value_ptr(nightLightColor));

	isNightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "isNight");
	glUniform1i(isNightLoc, enableNight);

	//initializeaza umbra
	shadowsEnabledLoc = glGetUniformLocation(myCustomShader.shaderProgram, "enableShadow");
	glUniform1i(shadowsEnabledLoc, enableShadow);

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction 
	lightDir = glm::vec3(0.0f, 1.0f, 2.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//set point light
	lightPos1Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos1");
	glUniform3fv(lightPos1Loc, 1, glm::value_ptr(lightPos1));

	lightPos2Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos2");
	glUniform3fv(lightPos2Loc, 1, glm::value_ptr(lightPos2));

	lightPos3Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos3");
	glUniform3fv(lightPos3Loc, 1, glm::value_ptr(lightPos3));

	lightPos4Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos4");
	glUniform3fv(lightPos4Loc, 1, glm::value_ptr(lightPos4));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUseProgram(depthMapShader.shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix())); // Uniform pentru shaderul de vertex (depthMap.vert)
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model)); // Uniform pentru model în depthMap.vert
}

void initFBO() {
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);
	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,
		0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Modificam datele picaturilor
void updateRainDrops(float deltaTime, float groundHeight) {
	for (RainDrops& drops : raindrops) {
		drops.velocity.y -= 9.8f * deltaTime;
		drops.position += drops.velocity * deltaTime;

		if (drops.position.y < groundHeight) {
			drops.position.y = rand() % 20 + 5;
			drops.position.x = rand() % 25 - 13;
			drops.position.z = rand() % 25 - 13;

			// Resetez viteza de cadere
			drops.velocity.y = -9.8f; 
		}
	}
}

void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	scena.Draw(shader);

	modelElice = glm::mat4(1.0f);
	modelElice = glm::translate(modelElice, glm::vec3(0.0f, -1.0f, 0.0f));
	modelElice = glm::scale(modelElice, glm::vec3(0.5f));
	modelElice = glm::translate(modelElice, glm::vec3(-1.62485f, 1.35772f, 6.14168f));
	modelElice = glm::rotate(modelElice, glm::radians(angleElice), glm::vec3(1.0f, 0.0f, 0.0f));
	modelElice = glm::translate(modelElice, glm::vec3(1.62485f, -1.35772f, -6.14168f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelElice));

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * modelElice));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	eliceMoara1.Draw(shader);

	//Activeaza/dezactiveaza ploaia
	if (enableRain) {
		for (RainDrops& drops : raindrops) {
			glm::mat4 model = glm::translate(glm::mat4(1.0f), drops.position);
			model = glm::scale(model, glm::vec3(drops.size));
			glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

			RainDrop.Draw(shader);
		}
	}
}

void drawTransparentObjects(gps::Shader shader, bool depthPass) {

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-2.45107f, -1.03997f, 0.524735f));
	model = glm::scale(model, glm::vec3(0.001f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	bottle1.Draw(shader);

	glm::mat4 model1 = glm::mat4(1.0f);
	model1 = glm::translate(model1, glm::vec3(-2.45107f, -1.03997f, 0.490122f));
	model1 = glm::scale(model1, glm::vec3(0.001f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model1));

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model1));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	bottle2.Draw(shader);

	glm::mat4 model2 = glm::mat4(1.0f);
	model2 = glm::translate(model2, glm::vec3(-2.45107f, -1.03997f, 0.40315f));
	model2 = glm::scale(model2, glm::vec3(0.001f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model2));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	bottle3.Draw(shader);

	glm::mat4 model3 = glm::mat4(1.0f);
	model3 = glm::translate(model3, glm::vec3(-2.56188f, -1.03997f, 0.716445f));
	model3 = glm::scale(model3, glm::vec3(0.001f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model3));

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model3));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	bottle4.Draw(shader);

	glm::mat4 model4 = glm::mat4(1.0f);
	model4 = glm::translate(model4, glm::vec3(-2.56188f, -1.03997f, 0.813824f));
	model4 = glm::scale(model4, glm::vec3(0.001f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model4));

	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model4));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	bottle5.Draw(shader);
}

void renderScene() {

	// Modul prezentare
	if (isPresentationActive) {
		lightAngle++;
		if (aux < 10.0f) {
			myCamera = gps::Camera(glm::vec3(-1.0f, 2.0f, aux), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			aux = aux + 0.1f;
		}
		else {
			if (aux2 < 10.0f) {
				myCamera = gps::Camera(glm::vec3(-1.0f, aux2, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				aux2 = aux2 + 0.1f;
			}
			else {
				if (aux3 < 10.0f) {
					myCamera = gps::Camera(glm::vec3(aux3, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					aux3 = aux3 + 0.1f;
				}
				else {
					myCamera = gps::Camera(glm::vec3(0.0f, 2.0f, 5.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					aux = 0.0f;
					aux2 = 0.0f;
					aux3 = 0.0f;
				}
			}
		}
	}

	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, showDepthMap);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {
		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//Daca e activata ceata 
		glUniform1i(enableFogLoc, enableFog);

		//Activeaza/dezactiveaza noaptea
		if (enableNight) {
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(nightLightColor));
			glUniform1i(isNightLoc, enableNight);
		}
		else {
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
		}

		//Daca e activa umbra
		glUniform1i(shadowsEnabledLoc, enableShadow);

		//Activeaza/dezactiveaza lumina punctiforma
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "enablePointLight"), enablePointLight);

		//Activeaza/dezactiveaza animatia
		if (enableAnimation) {
			angleElice += 3.6f;
			if (angleElice >= 360.0f) angleElice -= 360.0f; 
		}

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		drawTransparentObjects(myCustomShader, false);
		glDisable(GL_BLEND);

		lightShader.useShaderProgram();

		// Pozitionare si scalare cub de lumina (soare)

		// Reseteaza matricea model
		model = glm::mat4(1.0f);
		// Rotatie pentru a simula miscarea soarelui
		model = glm::rotate(model, glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		// Pozitioneaza "soarele" pe cer
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, -10.0f));
		// Dimensiune mai mare a soarelui
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	}
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char* argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
	initRainDrop(300, -3.0f);

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		updateRainDrops(0.3f, -3.0f);
		processMovement();
		
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
