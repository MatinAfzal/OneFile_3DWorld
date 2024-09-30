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
*/
#include <iostream>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

GLuint textureFloatArts;

void display(GLFWwindow* window, GLuint program, GLuint VAO, double currentTime, GLuint uniformScale);
GLuint shaderProgramInit();
void terminateShaderProgram(GLuint program);
std::tuple<GLuint, GLuint, GLuint> bindingInit();
void terminateBinding(GLuint VAO, GLuint VBO, GLuint EBO);
void checkShaderCompileErrors(GLuint shader);
void checkProgramLinkErrors(GLuint program);

float screenColor[4] = { 1.f, 1.f, 1.f, 1.f };
const char* vertexShaderCode =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTex;\n"
"uniform float scale;\n"
"out vec3 color;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);\n"
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

GLfloat vertices[] =
{
	// Vertices               Colors                Texture Coordinates
	-0.5f, -0.75f, 0.0f,     1.0f, 0.5f, 1.0f,     0.0f, 0.0f,  // Bottom-left
	-0.5f,  0.75f, 0.0f,     1.0f, 1.0f, 1.0f,     0.0f, 1.0f,  // Top-left
	 0.5f,  0.75f, 0.0f,     1.0f, 0.5f, 1.0f,     1.0f, 1.0f,  // Top-right
	 0.5f, -0.75f, 0.0f,     1.0f, 1.0f, 1.0f,     1.0f, 0.0f   // Bottom-right
};

GLuint indices[] =
{
	0, 1, 2,  // First triangle
	0, 2, 3   // Second triangle
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
	GLuint uniformScale = glGetUniformLocation(program, "scale");
	GLuint uniformTexture0 = glGetUniformLocation(program, "tex0");

	// Textures
	int textureFA_Height, textureFA_Width, textureFA_Col;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* lastLoadedTexture = stbi_load("FloatArts.png", &textureFA_Width, &textureFA_Height, &textureFA_Col, 0);
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
		display(window, program, VAO, glfwGetTime(), uniformScale);
	}

	terminateBinding(VAO, VBO, EBO);
	terminateShaderProgram(program);
	glDeleteTextures(1, &textureFloatArts);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

//******************************
void display(GLFWwindow* window, GLuint program, GLuint VAO, double currentTime, GLuint uniformScale) {
	glClearColor(screenColor[0], screenColor[1], screenColor[2], screenColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glUniform1f(uniformScale, -0.5f);
	glBindTexture(GL_TEXTURE_2D, textureFloatArts);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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