/*
* Float Arts
* By Matin Afzal Asr
* 27/9/2024
*
* ERROR Categories.
*	PVE: Pre-Visual Error.
*	AVE: After-Visual Error.
* ERROR Types.
*	INIT: Initializing Error.
*	MATH: Arithmetic Error.
*	SHAD: Shader Error.
*	PROG: Program Error.
*	LOAD: Loading Error.
*/
#include <iostream>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#define GLM_FORCE_RADIANS // [https://stackoverflow.com/questions/79054516/fps-camera-system-cant-look-up-or-down]
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

float FOV = glm::radians(45.0f);
float NEAR_PLANE = 0.1f;
float FAR_PLANE = 400.0f;

// Functions
void display(GLFWwindow* window, GLuint lightShader, GLuint program, GLuint VAO, GLuint LVAO, double currentTime, 
	GLuint uniformCamMatrix);

GLuint shaderProgramInit(void);
void terminateShaderProgram(GLuint program);

GLuint shaderLightInit();
void terminateShaderLight(GLuint shader);

std::tuple<GLuint, GLuint, GLuint> programBindingInit();
void terminateProgramBinding(GLuint VAO, GLuint VBO, GLuint EBO);

std::tuple<GLuint, GLuint, GLuint> lightBindingInit();
void terminateLightBinding(GLuint VAO, GLuint VBO, GLuint EBO);

void inputs(GLFWwindow* window);

void checkShaderCompileErrors(GLuint shader);
void checkProgramLinkErrors(GLuint program);

void errorLog(std::string category, std::string type, std::string massage, std::string comment="");


// Global Variables
glm::mat4 model = glm::mat4(1.0f);
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
glm::mat4 lightModel = glm::mat4(1.0f);

glm::vec3 cubePos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 cubeModel = glm::mat4(1.0f);

float rotation = 0.0f;
float rotatingSpeed = 1.0f;
float camSpeed = 0.1;
float sensitivity = 100.0;
bool firstClick = true;

double startTime = glfwGetTime();
double lastTime = startTime;

GLuint textureFloatArts;
GLfloat scale = 1.0f;

float screenColor[4] = { 0.1f, 0.2f, 0.3f, 1.f };

const char* vertexShaderCode =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTex;\n"
"uniform mat4 camMatrix;\n"
"uniform mat4 model;\n"
"out vec3 color;\n"
"out vec2 texCoord;\n"
"out vec3 crnt_pos;\n"
"void main()\n"
"{\n"
"   crnt_pos = vec3(model * vec4(aPos, 1.0f));\n"
"	gl_Position = camMatrix * vec4(crnt_pos, 1.0f);\n"
"	color = aColor;\n"
"	texCoord = aTex;\n"
"}\0;"
;
const char* fragmentShaderCode =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 color;\n"
"in vec2 texCoord;\n"
"uniform sampler2D tex0;\n"
"uniform vec4 lightColor;\n"
"uniform vec3 lightPos;\n"
"void main()\n"
"{\n"
"	FragColor = texture(tex0, texCoord) * lightColor;\n"
"}\n\0;"
;
const char* vertexShaderLightCode =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 Color;\n"
"uniform mat4 model;\n"
"uniform mat4 camMatrix;\n"
"void main()\n"
"{\n"
"	gl_Position = camMatrix * model * vec4(aPos, 1.0);\n"
"}\n\0;"
;
const char* fragmentShaderLightCode =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 lightColor;\n"
"void main()\n"
"{\n"
"	FragColor = lightColor;\n"
"}\n\0;"
;

GLfloat objectFloatArtsvertices[] = {
	// Positions          // Colors               // Texture Coordinates
	// Back Face
	-0.5f * scale, -0.5f * scale, -0.5f * scale,  1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  // Back-left-bottom
	 0.5f * scale, -0.5f * scale, -0.5f * scale,  1.0f, 0.0f, 0.0f,     0.0f, 0.0f,  // Back-right-bottom
	 0.5f * scale,  0.5f * scale, -0.5f * scale,  1.0f, 0.0f, 0.0f,     0.0f, 1.0f,  // Back-right-top
	-0.5f * scale,  0.5f * scale, -0.5f * scale,  1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  // Back-left-top

	// Front Face
	-0.5f * scale, -0.5f * scale,  0.5f * scale,  1.0f, 1.0f, 1.0f,    0.0f, 0.0f,  // Front-left-bottom
	 0.5f * scale, -0.5f * scale,  0.5f * scale,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,  // Front-right-bottom
	 0.5f * scale,  0.5f * scale,  0.5f * scale,  1.0f, 1.0f, 1.0f,     1.0f, 1.0f,  // Front-right-top
	-0.5f * scale,  0.5f * scale,  0.5f * scale,  1.0f, 1.0f, 1.0f,    0.0f, 1.0f,  // Front-left-top

	// Left Face (no texture)
	-0.5f * scale, -0.5f * scale, -0.5f * scale,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Back-left-bottom
	-0.5f * scale, -0.5f * scale,  0.5f * scale,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Front-left-bottom
	-0.5f * scale,  0.5f * scale,  0.5f * scale,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Front-left-top
	-0.5f * scale,  0.5f * scale, -0.5f * scale,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Back-left-top

	// Right Face (no texture)
	 0.5f * scale, -0.5f * scale, -0.5f * scale,  0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // Back-right-bottom
	 0.5f * scale, -0.5f * scale,  0.5f * scale,  0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // Front-right-bottom
	 0.5f * scale,  0.5f * scale,  0.5f * scale,  0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // Front-right-top
	 0.5f * scale,  0.5f * scale, -0.5f * scale,  0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // Back-right-top

	 // Top Face (no texture)
	 -0.5f * scale,  0.5f * scale, -0.5f * scale,  1.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Back-left-top
	  0.5f * scale,  0.5f * scale, -0.5f * scale,  1.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Back-right-top
	  0.5f * scale,  0.5f * scale,  0.5f * scale,  1.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Front-right-top
	 -0.5f * scale,  0.5f * scale,  0.5f * scale,  1.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Front-left-top

	 // Bottom Face (no texture)
	 -0.5f * scale, -0.5f * scale, -0.5f * scale,  0.5f, 0.5f, 0.5f,    0.0f, 0.0f,  // Back-left-bottom
	  0.5f * scale, -0.5f * scale, -0.5f * scale,  0.5f, 0.5f, 0.5f,    0.0f, 0.0f,  // Back-right-bottom
	  0.5f * scale, -0.5f * scale,  0.5f * scale,  0.5f, 0.5f, 0.5f,    0.0f, 0.0f,  // Front-right-bottom
	 -0.5f * scale, -0.5f * scale,  0.5f * scale,  0.5f, 0.5f, 0.5f,    0.0f, 0.0f   // Front-left-bottom
};


GLuint objectFloatArtsindices[] = {
	// Back face
	0, 1, 2,
	0, 2, 3,
	// Front face
	4, 5, 6,
	4, 6, 7,
	// Left face
	8, 9, 10,
	8, 10, 11,
	// Right face
	12, 13, 14,
	12, 14, 15,
	// Top face
	16, 17, 18,
	16, 18, 19,
	// Bottom face
	20, 21, 22,
	20, 22, 23
};

GLfloat objectLightVertices[] = {
	//     COORDINATES        
	-0.1f, -0.1f,  0.1f,      
	-0.1f, -0.1f, -0.1f,      
	 0.1f, -0.1f, -0.1f,      
	 0.1f, -0.1f,  0.1f,      
	-0.1f,  0.1f,  0.1f,      
	-0.1f,  0.1f, -0.1f,     
	 0.1f,  0.1f, -0.1f,    
	 0.1f,  0.1f,  0.1f
};

GLuint objectLightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

int main() {
	const char* screenTitle = "Float Arts";
	int viewPortX1 = 0, viewPortY1 = 0, viewPortX2 = SCREEN_WIDTH, viewPortY2 = SCREEN_HEIGHT;

	if (!glfwInit()) {
		errorLog("PVE", "INIT", "can't initilize glfw.", "");
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, screenTitle, NULL, NULL);
	if (window == NULL) {
		errorLog("PVE", "INIT", "can't create window.", "");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(viewPortX1, viewPortY1, viewPortX2, viewPortY2);

	// Shader program and Bindings
	GLuint program = shaderProgramInit();
	GLuint VAO, VBO, EBO;
	std::tie(VAO, VBO, EBO) = programBindingInit();

	GLuint lightShader = shaderLightInit();
	GLuint LVAO, LVBO, LEBO;
	std::tie(LVAO, LVBO, LEBO) = lightBindingInit();

	// Uniforms
	GLuint uniformTexture0 = glGetUniformLocation(program, "tex0");
	GLuint uniformCamMatrix = glGetUniformLocation(program, "camMatrix");

	GLuint uniformLightColor_L = glGetUniformLocation(lightShader, "lightColor");
	GLuint uniformCamMatrix_L = glGetUniformLocation(lightShader, "camMatrix");
	GLuint uniformModel_L = glGetUniformLocation(lightShader, "model");

	// Light
	lightModel = glm::translate(lightModel, lightPos);
	cubeModel = glm::translate(cubeModel, cubePos);

	glUseProgram(lightShader);
	glUniform4fv(uniformModel_L, 1, glm::value_ptr(cubeModel));
	glUniform4f(uniformLightColor_L, lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
	glUniform4f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Textures
	int textureFA_Height, textureFA_Width, textureFA_Col;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* lastLoadedTexture = stbi_load("FloatArts.png", &textureFA_Width, &textureFA_Height, &textureFA_Col, 0);
	if (!lastLoadedTexture) {
		errorLog("AVE", "LOAD", "can't load (FloatArts.png) texture.", "");
		glfwTerminate();
		return -1;
	}
	glGenTextures(1, &textureFloatArts);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureFloatArts);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST, GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S R T : X Y Z
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureFA_Width, textureFA_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, lastLoadedTexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(lastLoadedTexture);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(program);
	glUniform1i(uniformTexture0, 0);

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		display(window, program, lightShader, VAO, LVAO, glfwGetTime(), uniformCamMatrix);
	}

	terminateProgramBinding(VAO, VBO, EBO);
	terminateShaderProgram(program);
	terminateShaderLight(lightShader);
	terminateLightBinding(LVAO, LVBO, LEBO);

	glDeleteTextures(1, &textureFloatArts);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

//******************************
void display(GLFWwindow* window, GLuint program, GLuint lightShader, GLuint VAO, GLuint LVAO, double currentTime, 
	GLuint uniformCamMatrix) {
	glClearColor(screenColor[0], screenColor[1], screenColor[2], screenColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	inputs(window);
	// Camera
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	view = glm::lookAt(camPos, camPos + orientation, up);
	proj = glm::perspective(FOV, (float)SCREEN_WIDTH / SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);

	glUseProgram(program);
	glUniformMatrix4fv(uniformCamMatrix, 1, GL_FALSE, glm::value_ptr(proj * view * model));

	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	rotation = rotatingSpeed; // static rotation
	double crntTime = glfwGetTime();
	if (crntTime - lastTime >= 1 / 60) {
		if (rotation <= 10400)
			rotation += rotatingSpeed;
		lastTime = crntTime;
	}

	glBindTexture(GL_TEXTURE_2D, textureFloatArts);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, sizeof(objectFloatArtsindices)/sizeof(int), GL_UNSIGNED_INT, 0);


	glUseProgram(lightShader);
	glUniformMatrix4fv(glGetUniformLocation(lightShader, "camMatrix"), 1, GL_FALSE, glm::value_ptr(proj * view));
	glUniformMatrix4fv(glGetUniformLocation(lightShader, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glBindVertexArray(LVAO);
	glDrawElements(GL_TRIANGLES, sizeof(objectLightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);


	glfwSwapBuffers(window);
	glfwPollEvents();
}

GLuint shaderProgramInit() {
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER),
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER),
		shaderProgram = glCreateProgram();

	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexShader);
	checkShaderCompileErrors(vertexShader);

	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);
	checkShaderCompileErrors(fragmentShader);

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkProgramLinkErrors(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

void terminateShaderProgram(GLuint program) {
	glDeleteProgram(program);
}

GLuint shaderLightInit() {
	GLuint vertexShaderLight = glCreateShader(GL_VERTEX_SHADER),
		fragmentShaderLight = glCreateShader(GL_FRAGMENT_SHADER),
		shaderLight = glCreateProgram();

	glShaderSource(vertexShaderLight, 1, &vertexShaderLightCode, NULL);
	glCompileShader(vertexShaderLight);
	checkShaderCompileErrors(vertexShaderLight);

	glShaderSource(fragmentShaderLight, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShaderLight);
	checkShaderCompileErrors(fragmentShaderLight);

	glAttachShader(shaderLight, vertexShaderLight);
	glAttachShader(shaderLight, fragmentShaderLight);
	glLinkProgram(shaderLight);
	checkProgramLinkErrors(shaderLight);

	glDeleteShader(vertexShaderLight);
	glDeleteShader(fragmentShaderLight);

	return shaderLight;
}

void terminateShaderLight(GLuint shaderLight) {
	glDeleteProgram(shaderLight);
}

std::tuple<GLuint, GLuint, GLuint> programBindingInit() {
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(objectFloatArtsvertices), objectFloatArtsvertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(objectFloatArtsindices), objectFloatArtsindices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // vertices
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // colors
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Textures
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return std::make_tuple(VAO, VBO, EBO);
}

void terminateProgramBinding(GLuint VAO, GLuint VBO, GLuint EBO) {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

std::tuple<GLuint, GLuint, GLuint> lightBindingInit() {
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(objectLightVertices), objectLightVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(objectLightIndices), objectLightIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // vertices
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return std::make_tuple(VAO, VBO, EBO);
}

void terminateLightBinding(GLuint VAO, GLuint VBO, GLuint EBO) {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void inputs(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camPos += camSpeed * orientation;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camPos += camSpeed * -glm::normalize(glm::cross(orientation, up));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camPos += camSpeed * -orientation;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camPos += camSpeed * glm::normalize(glm::cross(orientation, up));
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camPos += camSpeed * up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camPos += camSpeed * -up;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camSpeed = 0.4f;

	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camSpeed = 0.1f;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick)
		{
			glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));
			firstClick = false;
		}

		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (SCREEN_HEIGHT / 2)) / SCREEN_HEIGHT;
		float rotY = sensitivity * (float)(mouseX - (SCREEN_WIDTH / 2)) / SCREEN_WIDTH;

		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

		//std::cout << "abs(glm::angle(newOrientation, up) - glm::radians(90.0f): " << abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) << std::endl;
		//std::cout << "orientation X/Y/Z" << orientation.x << "/" << orientation.y << "/" << orientation.z << std::endl;
		if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = newOrientation;
		}

		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

		glfwSetCursorPos(window, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}

void checkShaderCompileErrors(GLuint shader) {
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		errorLog("AVE", "SHAD", infoLog, "");
	}
}

void checkProgramLinkErrors(GLuint program) {
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		errorLog("AVE", "PROG", infoLog, "");
	}
}

void errorLog(std::string category, std::string type, std::string massage, std::string comment) {
	if (comment.empty())
		std::cerr << "ERROR" << " [" << category << "], " << "TYPE" << " [" << type << "]: " << massage;
	else
		std::cerr << "ERROR" << " [" << category << "], " << "TYPE" << " [" << type << "] " << "(" << comment << ")" << massage;
}