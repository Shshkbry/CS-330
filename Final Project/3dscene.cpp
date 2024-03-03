/*
* Written by Mary Sanders
* CS 330
* February 2024
*/
//
// KEYBOARD COMMANDS
//
//  UP            E
//  DOWN          Q
//  LEFT          A
//  RIGHT         D
//  FORWARD       W
//  BACKWARD      S
//  PERSPECTIVE   P
//  ORTHO         O
//


//inclusions for errors/outputs and GL libraries
#include <iostream>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>



//inclusions for math functions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "camera.h"
#include "meshes.h"

using namespace std;

#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif // GLSL

namespace {

	const char* const WINDOW_TITLE = "~~~Mary's 3D scene~~~"; //window title
	const int WINDOW_WIDTH = 800; //variables to set window size
	const int WINDOW_HEIGHT = 600;

	struct GLMesh
	{
		GLuint vao;
		GLuint vbos[2];
		GLuint nIndices;
		GLuint nVertices;
	};

	// Main GLFW window
	GLFWwindow* gWindow = nullptr;

	// Shader program
	GLuint gProgramId;
	GLuint gLightId;

	// Camera location
	Camera gCamera(glm::vec3(0.0f, 2.0f, 12.0f));
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;
	bool isPerspective = true;

	// Window frame timing
	float gDeltaTime = 0.0f;
	float gLastFrame = 0.0f;

	//Shape Meshes from Professor Brian Battersby
	Meshes meshes;

	//texture variables here
	GLuint gTextureFloor;
	GLuint gTexturePBody;
	GLuint gTexturePStem;
	GLuint gTextureCandyBar;
	GLuint gTextureCandyCorn;
	GLuint gTextureCandle;
	GLuint gTextureCandlewax;

	glm::vec2 gUVScale(1.0f, 1.0f);
	GLint gTexWrapMode = GL_REPEAT;

	//positions
	// 


	// pumpkin
	// glm::vec3 gPumpkinPosition(0.0f, 0.0f, 0.0f);
	// glm::vec3 gPumpkinScale(1.0f);
	
	// pumpkin stem
	// glm::vec3 gPumpStemPosition(0.0f, 0.0f, 0.0f);
	// glm::vec3 gPumpStemScale(1.0f);

	// Floor
	// glm::vec3 gFloorPosition(0.0f, 0.0f, 0.0f);
	// glm::vec3 gFloorScale(1.0f);

	// candlestick
	// glm::vec3 gCandleStickPosition(0.0f, 0.0f, 0.0f);
	// glm::vec3 gCandleStickScale(1.0f);

	// candle 
	// glm::vec3 gCandlePosition(0.0f, 0.0f, 0.0f);
	// glm::vec3 gCandleScale(1.0f);

	// flame
	// glm::vec3 gFlamePosition(0.0f, 0.0f, 0.0f);
	// glm::vec3 gFlameScale(1.0f);

	// candy bar
	// glm::vec3 gCandyBarPosition(0.0f, 0.0f, 0.0f);
	// glm::vec3 gCandyBarScale(1.0f);

	// candy corn
	// glm::vec3 gCandyCornPosition(0.0f, 0.0f, 0.0f);
	// glm::vec3 gCandyCornScale(1.0f);

	// bool gIsLampOrbiting = false;

	// Cube and light color
	glm::vec3 gObjectColor(1.f, 0.2f, 0.0f);
	glm::vec3 gLightColor1(1.0f, 0.0f, 0.0f);
	glm::vec3 gLightColor2(1.0f, 1.0f, 1.0f); 
	glm::vec3 gLightColor3(1.0f, 1.0f, 1.0f);

	// Light position and scale
	glm::vec3 gLightPosition1(0.0f, 12.0f, 0.0f);
	glm::vec3 gLightScale1(0.8f);
	glm::vec3 gLightPosition2(-5.0f, 12.0f, 0.0f);
	glm::vec3 gLightScale2(0.8f);
	glm::vec3 gLightPosition3(5.0f, 6.0f, 0.0f);
	glm::vec3 gLightScale3(0.8f);
}

//functions for handling window, mesh, and shader program
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void Render();
bool createShaderProg(const char* vShaderSource, const char* fShaderSource, GLuint& programId);
void destroyShaderProg(GLuint programId);


const GLchar* vertexShaderSource = GLSL(440,
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 textureCoordinate;

out vec3 vertexNormal;
out vec3 vertexFragmentPos;
out vec2 vertexTextureCoordinate;

uniform mat4 model; //variables for transform matrices
uniform mat4 view;
uniform mat4 projection;

void main() {

	gl_Position = projection * view * model * vec4(position, 1.0f);
	vertexFragmentPos = vec3(model * vec4(position, 1.0f));
	vertexNormal = mat3(transpose(inverse(model))) * normal;
	vertexTextureCoordinate = textureCoordinate;
}
);

const GLchar* fragmentShaderSource = GLSL(440,
	in vec3 vertexNormal;
in vec3 vertexFragmentPos;
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor;

uniform vec3 objectColor;
uniform vec3 lightColor1;
uniform vec3 lightPos1;
uniform vec3 lightColor2;
uniform vec3 lightPos2;
uniform vec3 lightColor3;
uniform vec3 lightPos3;
uniform vec3 viewPosition1;
uniform vec3 viewPosition2;
uniform vec3 viewPosition3;
uniform sampler2D uTexture;
uniform vec2 uvScale;


void main()
{
	//Phong lighting model calculations

	//calculate ambient lighting light #1
	float ambientStrength1 = 0.01f;
	vec3 ambient1 = ambientStrength1 * lightColor1;

	// ambient lighting, light #2
	float ambientStrength2 = 0.002f;
	vec3 ambient2 = ambientStrength2 * lightColor2;

	// ambient lighting, light #3
	float ambientStrength3 = 0.002f;
	vec3 ambient3 = ambientStrength3 * lightColor3;

	//calculate diffuse lighting light #1
	vec3 norm1 = normalize(vertexNormal); //normalize vectors
	vec3 lightDirection1 = normalize(lightPos1 - vertexFragmentPos); 
	float impact1 = max(dot(norm1, lightDirection1), 0.0);
	vec3 diffuse1 = impact1 * lightColor1;

	// diffuse lighting, light #2
	vec3 norm2 = normalize(vertexNormal);
	vec3 lightDirection2 = normalize(lightPos2 - vertexFragmentPos);
	float impact2 = max(dot(norm2, lightDirection2), 0.0);
	vec3 diffuse2 = impact2 * lightColor2;

	// diffuse lighting, light #3
	vec3 norm3 = normalize(vertexNormal);
	vec3 lightDirection3 = normalize(lightPos3 - vertexFragmentPos);
	float impact3 = max(dot(norm3, lightDirection3), 0.0);
	vec3 diffuse3 = impact3 * lightColor3;

	//calculate specular lighting #1
	float specularIntensity1 = 0.02f;
	float highlightSize1 = 16.0f;
	vec3 viewDir1 = normalize(viewPosition1 - vertexFragmentPos);
	vec3 reflectDir1 = reflect(-lightDirection1, norm1);
	//calculate specular component #1
	float specularComponent1 = pow(max(dot(viewDir1, reflectDir1), 0.0), highlightSize1);
	vec3 specular1 = specularIntensity1 * specularComponent1 * lightColor1;

	//calculate specular lighting #2
	float specularIntensity2 = 0.02f;
	float highlightSize2 = 16.0f;
	vec3 viewDir2 = normalize(viewPosition2 - vertexFragmentPos);
	vec3 reflectDir2 = reflect(-lightDirection2, norm2);
	// calculate specular component #2
	float specularComponent2 = pow(max(dot(viewDir2, reflectDir2), 0.0), highlightSize2);
	vec3 specular2 = specularIntensity2 * specularComponent2 * lightColor2;

	//calculate specular lighting #3
	float specularIntensity3 = 0.2f;
	float highlightSize3 = 6.0f;
	vec3 viewDir3 = normalize(viewPosition3 - vertexFragmentPos);
	vec3 reflectDir3 = reflect(-lightDirection3, norm3);
	//calculate specular component #3
	float specularComponent3 = pow(max(dot(viewDir3, reflectDir3), 0.0), highlightSize3);
	vec3 specular3 = specularIntensity3 * specularComponent3 * lightColor3;


	//texture holds the color
	vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

	vec3 phong = (ambient1 + ambient2 + ambient3 + diffuse1 + diffuse2 + diffuse3 + specular1 + specular2 + specular3) * textureColor.xyz;
	fragmentColor = vec4(phong, 1.0);

}
);

const GLchar* lampVertexShaderSource = GLSL(440,

	layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


//----------------------
//LAMP FRAGMENT SHADER SOURCE CODE
//----------------------
const GLchar* lampFragmentShaderSource = GLSL(440,

	out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
	fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);

/*
* WRITTEN BY @SNHU TUTORIALS
*/
// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
	for (int j = 0; j < height / 2; ++j)
	{
		int index1 = j * width * channels;
		int index2 = (height - 1 - j) * width * channels;

		for (int i = width * channels; i > 0; --i)
		{
			unsigned char tmp = image[index1];
			image[index1] = image[index2];
			image[index2] = tmp;
			++index1;
			++index2;
		}
	}
}



int main(int argc, char* argv[]) {

	if (!UInitialize(argc, argv, &gWindow)) { //exits when initialization fails
		return EXIT_FAILURE;
	}

	meshes.CreateMeshes();
	if (!createShaderProg(vertexShaderSource, fragmentShaderSource, gProgramId)) {
		return EXIT_FAILURE;
	}
	if (!createShaderProg(lampVertexShaderSource, lampFragmentShaderSource, gLightId)) {
		return EXIT_FAILURE;
	}

	//load textures
	const char* texFilename = "pumpkintexture.jpg";
	if (!UCreateTexture(texFilename, gTexturePBody))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "pstemtexture.jpg";
	if (!UCreateTexture(texFilename, gTexturePStem)) {
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "checkerfloor.jpg";
	if (!UCreateTexture(texFilename, gTextureFloor)) {
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "chocolatebar.jpg";
	if (!UCreateTexture(texFilename, gTextureCandyBar))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "candycorn.jpg";
	if (!UCreateTexture(texFilename, gTextureCandyCorn))
	{
		cout << "Failed to load texture " << texFilename << endl;
		return EXIT_FAILURE;
	}
	texFilename = "candle.jpg";
	if (!UCreateTexture(texFilename, gTextureCandle))
	{
		cout << "Failed to load texture " << texFilename << endl;
	}
	texFilename = "candlewax.jpg";
	if (!UCreateTexture(texFilename, gTextureCandlewax))
	{
		cout << "Failed to load texture " << texFilename << endl;
	}

	glUseProgram(gProgramId);
	glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0);

	while (!glfwWindowShouldClose(gWindow)) {

		float currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;

		UProcessInput(gWindow);
		Render();
		glfwPollEvents();
	}

	meshes.DestroyMeshes();
	UDestroyTexture(gTextureFloor);
	UDestroyTexture(gTexturePBody);
	UDestroyTexture(gTexturePStem);
	UDestroyTexture(gTextureCandle);
	UDestroyTexture(gTextureCandyBar);
	UDestroyTexture(gTextureCandyCorn);
	UDestroyTexture(gTextureCandlewax);


	destroyShaderProg(gProgramId);

	exit(EXIT_SUCCESS);
}

bool UInitialize(int argc, char* argv[], GLFWwindow** window) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

	if (*window == NULL) {

		std::cout << "Window creation failed" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, UResizeWindow);
	glfwSetCursorPosCallback(*window, UMousePositionCallback);
	glfwSetScrollCallback(*window, UMouseScrollCallback);
	glfwSetMouseButtonCallback(*window, UMouseButtonCallback);


	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glewExperimental = GL_TRUE;
	GLenum GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {

		std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
		return false;
	}

	cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;
	return true;
}

void UProcessInput(GLFWwindow* window) {

	static const float cameraSpeed = 2.5f;


	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		isPerspective = !isPerspective;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		isPerspective = true;
	}
	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) //closes window
		glfwSetWindowShouldClose(window, true);

	float cameraOffset = cameraSpeed * gDeltaTime;


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		gCamera.ProcessKeyboard(LEFT, gDeltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		gCamera.ProcessKeyboard(UP, gDeltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		gCamera.ProcessKeyboard(DOWN, gDeltaTime);
	}
}


void UResizeWindow(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
	
	if (gFirstMouse) {
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}

	float xoffset = xpos - gLastX;
	float yoffset = gLastY - ypos;

	gLastX = xpos;
	gLastY = ypos;

	gCamera.ProcessMouseMovement(xoffset, yoffset);
}




void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	
	gCamera.ProcessMouseScroll(yoffset);
}

void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT: {
		if (action == GLFW_PRESS)
			cout << "Left mouse button pressed" << endl;
		else
			cout << "Left mouse button released" << endl;
	}
							   break;

	case GLFW_MOUSE_BUTTON_MIDDLE: {
		if (action == GLFW_PRESS)
			cout << "Middle mouse button pressed" << endl;
		else
			cout << "Middle mouse button released" << endl;

	}
								 break;

	case GLFW_MOUSE_BUTTON_RIGHT: {
		if (action == GLFW_PRESS)
			cout << "Right mouse button pressed" << endl;
		else
			cout << "Right mouse button released" << endl;
	}
								break;

	default:
		cout << "Unhandled mouse button event" << endl;
		break;
	}

}
void Render() {

	GLint modelLoc,
		viewLoc,
		projLoc,
		objColorLoc,
		lightColor1Loc,
		lightColor2Loc,
		lightColor3Loc,
		lightPosition1Loc,
		lightPosition2Loc,
		lightPosition3Loc,
		viewPositionLoc,
		viewPosition2Loc,
		viewPosition3Loc;

	glm::mat4 scale,
		rotation,
		translation,
		model,
		view,
		projection;

	glEnable(GL_DEPTH_TEST); //enable z-depth

	//clear frame and z buffers
	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	view = gCamera.GetViewMatrix();

	if (isPerspective) {
		projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	}
	else {
		projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	}

	//---------------------------------
	// Plane for objects to rest on
	
	glBindVertexArray(meshes.gPlaneMesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(7.0f, 1.0f, 7.0f));
	rotation = glm::rotate(0.0f, glm::vec3(0.0, 1.0f, 0.0f));
	translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));
	model = translation * rotation * scale;

	// Retrieves and passes transform matrices to the Shader program
	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");
	lightColor1Loc = glGetUniformLocation(gProgramId, "lightColor1");
	lightPosition1Loc = glGetUniformLocation(gProgramId, "lightPos1");
	lightColor2Loc = glGetUniformLocation(gProgramId, "lightColor2");
	lightPosition2Loc = glGetUniformLocation(gProgramId, "lightPos2");
	lightColor3Loc = glGetUniformLocation(gProgramId, "lightColor3");
	lightPosition3Loc = glGetUniformLocation(gProgramId, "lightPos3");
	viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
	viewPosition2Loc = glGetUniformLocation(gProgramId, "viewPosition2");
	viewPosition3Loc = glGetUniformLocation(gProgramId, "viewPosition3");
	
	glUniform3f(objColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColor1Loc, gLightColor1.r, gLightColor1.g, gLightColor1.b);
	glUniform3f(lightPosition1Loc, gLightPosition1.x, gLightPosition1.y, gLightPosition1.z);
	glUniform3f(lightColor2Loc, gLightColor2.r, gLightColor2.g, gLightColor2.b);
	glUniform3f(lightPosition1Loc, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
	glUniform3f(lightColor3Loc, gLightColor3.r, gLightColor3.g, gLightColor3.b);
	glUniform3f(lightPosition3Loc, gLightPosition3.x, gLightPosition3.y, gLightPosition3.z);
	const glm::vec3 cameraPosition1 = gCamera.Position;
	glUniform3f(viewPositionLoc, cameraPosition1.x, cameraPosition1.y, cameraPosition1.z);

	GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
	glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureFloor);

	glDrawElements(GL_TRIANGLES, meshes.gPlaneMesh.nIndices, GL_UNSIGNED_INT, (void*)0);


	//---------------------------------
	// candy bar, made of a cube
	//---------------------------------

	glBindVertexArray(meshes.gBoxMesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(3.0, 0.2, 1.0));
	rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(4.0f, 0.2f, 1.0f));
	model = translation * rotation * scale;

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");
	lightColor1Loc = glGetUniformLocation(gProgramId, "lightColor1");
	lightPosition1Loc = glGetUniformLocation(gProgramId, "lightPos1");
	lightColor2Loc = glGetUniformLocation(gProgramId, "lightColor2");
	lightPosition2Loc = glGetUniformLocation(gProgramId, "lightPos2");
	lightColor3Loc = glGetUniformLocation(gProgramId, "lightColor3");
	lightPosition3Loc = glGetUniformLocation(gProgramId, "lightPos3");
	viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
	viewPosition2Loc = glGetUniformLocation(gProgramId, "viewPosition2");
	viewPosition3Loc = glGetUniformLocation(gProgramId, "viewPosition3");

	glUniform3f(objColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColor1Loc, gLightColor1.r, gLightColor1.g, gLightColor1.b);
	glUniform3f(lightPosition1Loc, gLightPosition1.x, gLightPosition1.y, gLightPosition1.z);
	glUniform3f(lightColor2Loc, gLightColor2.r, gLightColor2.g, gLightColor2.b);
	glUniform3f(lightPosition1Loc, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
	glUniform3f(lightColor3Loc, gLightColor3.r, gLightColor3.g, gLightColor3.b);
	glUniform3f(lightPosition3Loc, gLightPosition3.x, gLightPosition3.y, gLightPosition3.z);
	glUniform3f(viewPositionLoc, cameraPosition1.x, cameraPosition1.y, cameraPosition1.z);

	UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
	glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureCandyBar);

	glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);


	//---------------------------------
	//Body of pumpkin (made of a sphere)
	//---------------------------------

	glBindVertexArray(meshes.gSphereMesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(1.3f, 1.6f, 1.3f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(-4.0f, 1.6f, -1.0f));
	model = translation * rotation * scale;

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glProgramUniform4f(gProgramId, objColorLoc, 1.0f, 0.0f, 0.0f, 1.0f);
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");
	lightColor1Loc = glGetUniformLocation(gProgramId, "lightColor1");
	lightPosition1Loc = glGetUniformLocation(gProgramId, "lightPos1");
	lightColor2Loc = glGetUniformLocation(gProgramId, "lightColor2");
	lightPosition2Loc = glGetUniformLocation(gProgramId, "lightPos2");
	lightColor3Loc = glGetUniformLocation(gProgramId, "lightColor3");
	lightPosition3Loc = glGetUniformLocation(gProgramId, "lightPos3");
	viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
	viewPosition2Loc = glGetUniformLocation(gProgramId, "viewPosition2");
	viewPosition3Loc = glGetUniformLocation(gProgramId, "viewPosition3");

	glUniform3f(objColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
	glUniform3f(lightColor1Loc, gLightColor1.r, gLightColor1.g, gLightColor1.b);
	glUniform3f(lightPosition1Loc, gLightPosition1.x, gLightPosition1.y, gLightPosition1.z);
	glUniform3f(lightColor2Loc, gLightColor2.r, gLightColor2.g, gLightColor2.b);
	glUniform3f(lightPosition1Loc, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);
	glUniform3f(lightColor3Loc, gLightColor3.r, gLightColor3.g, gLightColor3.b);
	glUniform3f(lightPosition3Loc, gLightPosition3.x, gLightPosition3.y, gLightPosition3.z);
	glUniform3f(viewPositionLoc, cameraPosition1.x, cameraPosition1.y, cameraPosition1.z);

	UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
	glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexturePBody);

	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);


	//-------------------------------
	//Pumpkin stem, made of a tapered cylinder
	//---------------------------------

	glBindVertexArray(meshes.gTaperedCylinderMesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(0.25f, 0.8f, 0.25f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(-4.0f, 2.95f, -1.0f));
	model = translation * rotation * scale;

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glProgramUniform4f(gProgramId, objColorLoc, 1.0f, 0.0f, 0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexturePStem);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 72);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides
	
	// candy corn
	glBindVertexArray(meshes.gPyramid3Mesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(0.19f, 0.25f, 0.1f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(1.0f, 0.2f, -0.5f));
	model = translation * rotation * scale;

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glProgramUniform4f(gProgramId, objColorLoc, 1.0f, 0.0f, 0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureCandyCorn);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid3Mesh.nVertices);

	glBindVertexArray(meshes.gPyramid3Mesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(0.19f, 0.25f, 0.1f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(1.5f, 0.2f, -0.5f));
	model = translation * rotation * scale;

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glProgramUniform4f(gProgramId, objColorLoc, 1.0f, 0.0f, 0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureCandyCorn);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid3Mesh.nVertices);

	glBindVertexArray(meshes.gPyramid3Mesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(0.19f, 0.25f, 0.1f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(1.25f, 0.2f, -0.75f));
	model = translation * rotation * scale;

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glProgramUniform4f(gProgramId, objColorLoc, 1.0f, 0.0f, 0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureCandyCorn);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, meshes.gPyramid3Mesh.nVertices);
	
	
	// candle, spheres and cylinders

	glBindVertexArray(meshes.gSphereMesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(0.3f, 0.08f, 0.3f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, 0.2f, -2.5f));
	model = translation * rotation * scale;

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glProgramUniform4f(gProgramId, objColorLoc, 0.0f, 0.0f, 0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureCandle);

	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(meshes.gCylinderMesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(0.1f, 1.0f, 0.1f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, 0.2f, -2.5f));
	model = translation * rotation * scale;

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glProgramUniform4f(gProgramId, objColorLoc, 0.0f, 0.0f, 0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureCandle);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides

	glBindVertexArray(meshes.gSphereMesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(0.15f, 0.04f, 0.15f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, 1.2f, -2.5f));
	model = translation * rotation * scale;

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glProgramUniform4f(gProgramId, objColorLoc, 0.0f, 0.0f, 0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureCandle);

	glDrawElements(GL_TRIANGLES, meshes.gSphereMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(meshes.gCylinderMesh.vao);
	glUseProgram(gProgramId);
	scale = glm::scale(glm::vec3(0.065f, 0.8f, 0.065f));
	rotation = glm::rotate(0.0f, glm::vec3(1.0, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, 1.2f, -2.5f));
	model = translation * rotation * scale;

	modelLoc = glGetUniformLocation(gProgramId, "model");
	viewLoc = glGetUniformLocation(gProgramId, "view");
	projLoc = glGetUniformLocation(gProgramId, "projection");
	objColorLoc = glGetUniformLocation(gProgramId, "objectColor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glProgramUniform4f(gProgramId, objColorLoc, 0.0f, 0.0f, 0.0f, 1.0f);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTextureCandlewax);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 36);		//bottom
	glDrawArrays(GL_TRIANGLE_FAN, 36, 36);		//top
	glDrawArrays(GL_TRIANGLE_STRIP, 72, 146);	//sides


	// box light source

	glUseProgram(gLightId);
	glBindVertexArray(meshes.gBoxMesh.vao);
	scale = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	translation = glm::translate(glm::vec3(0.0f, 12.0f, 0.0f));
	model = translation * scale;

	modelLoc = glGetUniformLocation(gLightId, "model");
	viewLoc = glGetUniformLocation(gLightId, "view");
	projLoc = glGetUniformLocation(gLightId, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glDrawElements(GL_TRIANGLES, meshes.gBoxMesh.nIndices, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
	glUseProgram(0);

	glfwSwapBuffers(gWindow);

}


bool UCreateTexture(const char* filename, GLuint& textureId)
{
	int width, height, channels;
	unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
	if (image)
	{
		flipImageVertically(image, width, height, channels);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			cout << "Not implemented to handle image with " << channels << " channels" << endl;
			return false;
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		return true;
	}

	// Error loading the image
	return false;
}

void UDestroyTexture(GLuint textureId)
{
	glGenTextures(1, &textureId);
}

bool createShaderProg(const char* vShaderSource, const char* fShaderSource, GLuint& programId) {

	int success = 0;
	char infoLog[512];

	programId = glCreateProgram();

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShaderId, 1, &vShaderSource, NULL);
	glShaderSource(fragmentShaderId, 1, &fShaderSource, NULL);

	glCompileShader(vertexShaderId);
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);


	if (!success) {

		glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	glCompileShader(fragmentShaderId);
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

	if (!success) {

		glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return false;
	}

	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	glLinkProgram(programId);
	glGetProgramiv(programId, GL_LINK_STATUS, &success);


	if (!success) {

		glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return false;
	}

	glUseProgram(programId);
	return true;
}

void destroyShaderProg(GLuint programId) {

	glDeleteProgram(programId);
}
