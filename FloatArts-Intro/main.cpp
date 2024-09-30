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
*	LOAD: Loading Error.
*/
#include <iostream>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define ASPECT_RATIO SCREEN_WIDTH / SCREEN_HEIGHT
#define FOV glm::radians(45.0f)
#define NEAR_PLANE 0.1f
#define FAR_PLANE 1000.0f

// Functions
void display(GLFWwindow* window, GLuint program, GLuint VAO, double currentTime, 
	GLuint uniformCamMatrix);

GLuint shaderProgramInit();
void terminateShaderProgram(GLuint program);

std::tuple<GLuint, GLuint, GLuint> bindingInit();
void terminateBinding(GLuint VAO, GLuint VBO, GLuint EBO);

void inputs(GLFWwindow* window);

void checkShaderCompileErrors(GLuint shader);
void checkProgramLinkErrors(GLuint program);


// Global Variables
glm::mat4 model = glm::mat4(1.0f);
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, -300.0f);
glm::vec3 orientation = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

float rotation = 0.0f;
float rotatingSpeed = 30.0f;
float camSpeed = 0.1;
float sensitivity = 100.0;
bool firstClick = true;

double startTime = glfwGetTime();
double lastTime = startTime;

GLuint textureFloatArts;

float screenColor[4] = { 1.f, 1.f, 1.f, 1.f };

const char* vertexShaderCode =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTex;\n"
"uniform mat4 camMatrix;\n"
"out vec3 color;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"	gl_Position = camMatrix * vec4(aPos, 1.0f);\n"
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
"void main()\n"
"{\n"
"	FragColor = texture(tex0, texCoord);\n"
"}\n\0;"
;

GLfloat vertices[] = {
	// Positions          // Colors               // Texture Coordinates
	// Back Face
	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,    1.0f, 0.0f,  // Back-left-bottom
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,     0.0f, 0.0f,  // Back-right-bottom
	0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,     0.0f, 1.0f,  // Back-right-top
	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  // Back-left-top

	// Front Face
	-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,    0.0f, 0.0f,  // Front-left-bottom
	0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 0.0f,  // Front-right-bottom
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,     1.0f, 1.0f,  // Front-right-top
	-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,    0.0f, 1.0f,  // Front-left-top

	// Left Face (no texture)
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Back-left-bottom
	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Front-left-bottom
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Front-left-top
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Back-left-top

	// Right Face (no texture)
	0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // Back-right-bottom
	0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // Front-right-bottom
	0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // Front-right-top
	0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // Back-right-top

	// Top Face (no texture)
	-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Back-left-top
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Back-right-top
	0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Front-right-top
	-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,    0.0f, 0.0f,  // Front-left-top

	// Bottom Face (no texture)
	-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f,    0.0f, 0.0f,  // Back-left-bottom
	0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f,    0.0f, 0.0f,  // Back-right-bottom
	0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f,    0.0f, 0.0f,  // Front-right-bottom
	-0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f,    0.0f, 0.0f   // Front-left-bottom
};

GLuint indices[] = {
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

int main() {
	const char* screenTitle = "Float Arts";
	int viewPortX1 = 0, viewPortY1 = 0, viewPortX2 = SCREEN_WIDTH, viewPortY2 = SCREEN_HEIGHT;

	if (!glfwInit()) {
		std::cerr << "ERROR [PVE], TYPE [INIT]: can't initilize glfw.";
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, screenTitle, NULL, NULL);
	if (window == NULL) {
		std::cerr << "ERROR [PVE], TYPE [INIT]: can't create window.";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(viewPortX1, viewPortY1, viewPortX2, viewPortY2);

	// Shader program and Bindings
	GLuint program = shaderProgramInit();
	GLuint VAO, VBO, EBO;
	std::tie(VAO, VBO, EBO) = bindingInit();

	// Uniforms
	GLuint uniformTexture0 = glGetUniformLocation(program, "tex0");
	GLuint uniformCamMatrix = glGetUniformLocation(program, "camMatrix");

	// Textures
	int textureFA_Height, textureFA_Width, textureFA_Col;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* lastLoadedTexture = stbi_load("FloatArts.png", &textureFA_Width, &textureFA_Height, &textureFA_Col, 0);
	if (!lastLoadedTexture) {
		std::cerr << "ERROR [AVE], TYPE [LOAD]: can't load (FloatArts.png) texture.";
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

	while (!glfwWindowShouldClose(window)) {
		display(window, program, VAO, glfwGetTime(), uniformCamMatrix);
		inputs(window);
	}

	terminateBinding(VAO, VBO, EBO);
	terminateShaderProgram(program);
	glDeleteTextures(1, &textureFloatArts);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

//******************************
void display(GLFWwindow* window, GLuint program, GLuint VAO, double currentTime, 
	GLuint uniformCamMatrix) {
	glEnable(GL_DEPTH_TEST);
	glClearColor(screenColor[0], screenColor[1], screenColor[2], screenColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	// Rotating 5400, 10423
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	//rotation = rotatingSpeed; // static rotation
	double crntTime = glfwGetTime();
	if (crntTime - lastTime >= 1 / 60) {
		if (rotation <= 10400)
			rotation += rotatingSpeed;
		lastTime = crntTime;
		//std::cout << rotation << std::endl;
	}

	// Camera
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	view = glm::lookAt(camPos, camPos + orientation, up);
	proj = glm::perspective(FOV, (float)ASPECT_RATIO, NEAR_PLANE, FAR_PLANE);
	glUniformMatrix4fv(uniformCamMatrix, 1, GL_FALSE, glm::value_ptr(proj * view * model));

	glBindTexture(GL_TEXTURE_2D, textureFloatArts);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

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

std::tuple<GLuint, GLuint, GLuint> bindingInit() {
	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // vertices
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // colors
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // colors
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return std::make_tuple(VAO, VBO, EBO);
}

void terminateBinding(GLuint VAO, GLuint VBO, GLuint EBO) {
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
		std::cerr << "ERROR [AVE], TYPE [SHAD]:\nERROR::SHADER_COMPILATION_ERROR of type: " << infoLog << std::endl;
	}
}

void checkProgramLinkErrors(GLuint program) {
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cerr << "ERROR [AVE], TYPE [SHAD]:\nERROR::PROGRAM_LINKING_ERROR of type: " << infoLog << std::endl;
	}
}