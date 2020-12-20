
#include "Shader.h"
#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL.h>
// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes

#include "Camera.h"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera  camera(glm::vec3(0.0f, 2.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 2.0f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

							// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);


	// Build and compile our shader program
	Shader targetShader("shader/target.vs", "shader/target.fs");
	Shader lightShader("shader/light.vs", "shader/light.fs");
	// 边缘检测shader
	Shader outlineShader("shader/outline.vs", "shader/outline.fs", "shader/outline.gs", true);
	// Set up vertex data (and buffer(s)) and attribute pointers

	/*
	1  -0.5f, -0.5f,  0.5f,
	2   0.5f, -0.5f,  0.5f,
	3   0.5f,  0.5f,  0.5f,
	4  -0.5f,  0.5f,  0.5f,
	5  -0.5f, -0.5f, -0.5f,
	6   0.5f, -0.5f, -0.5f,
	7   0.5f,  0.5f, -0.5f,
	8  -0.5f,  0.5f, -0.5f,
	
	*/

	GLfloat vertices[] = {
		 -0.5f, -0.5f,  0.5f,// 0
		  0.5f, -0.5f,  0.5f,// 1
		  0.5f,  0.5f,  0.5f,// 2
		 -0.5f,  0.5f,  0.5f,// 3
		 -0.5f, -0.5f, -0.5f,// 4
		  0.5f, -0.5f, -0.5f,// 5
		  0.5f,  0.5f, -0.5f,// 6
		 -0.5f,  0.5f, -0.5f,// 7
	};


	/*
	GLint target_vertex_indices[] ={
		6, 5, 4,
		7, 6, 4,
		0, 1, 2,
		2, 3, 0,
		3, 7, 4,
		3, 4, 0,
		2, 5, 6,
		2, 1, 5,
		4, 5, 1,
		4, 1, 0,
		2, 6, 7,
		2, 7, 3,
	}
	*/

	GLint outline_vertex_indices[] =
	{
	//实点 虚点 实点 虚点 实点 虚点
		6, 2, 5, 1, 4, 7, 
		7, 2, 6, 5, 4, 3,
		0, 4, 1, 5, 2, 3,
		2, 7, 3, 4, 0, 1, 
		3, 2, 7, 6, 4, 0, 
		3, 7, 4, 1, 0, 2,
		2, 1, 5, 4, 6, 7,
		2, 0, 1, 4, 5, 6,
		4, 6, 5, 2, 1, 0, 
		4, 5, 1, 2, 0, 3,
		2, 5, 6, 4, 7, 3,
		2, 6, 7, 4, 3, 0,
	};

	GLfloat target_vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	// First, set the container's VAO (and VBO)
	GLuint targetVBO, targetVAO;
	glGenVertexArrays(1, &targetVAO);
	glGenBuffers(1, &targetVBO);

	
	glBindBuffer(GL_ARRAY_BUFFER, targetVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(target_vertices), target_vertices, GL_STATIC_DRAW);
	glBindVertexArray(targetVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint outlineVBO, outlineEBO, outlineVAO;
	glGenVertexArrays(1, &outlineVAO);
	glGenBuffers(1, &outlineVBO);
	glGenBuffers(1, &outlineEBO);

	glBindVertexArray(outlineVAO);

	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outlineEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(outline_vertex_indices), outline_vertex_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// 
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 
		glfwPollEvents();
		do_movement();

		// 
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// 绘制目标箱子
		targetShader.Use();
		glBindVertexArray(targetVAO);
		GLint objectColorLoc = glGetUniformLocation(targetShader.Program, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(targetShader.Program, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(targetShader.Program, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(targetShader.Program, "viewPos");
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(lightColorLoc, 1.0f, 0.5f, 1.0f);
		glUniform3f(lightPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		GLint modelLoc = glGetUniformLocation(targetShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(targetShader.Program, "view");
		GLint projLoc = glGetUniformLocation(targetShader.Program, "projection");
		//
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		
		glm::mat4 model(1.0f);  // 注意此处一定要手写1.0f，不同的库初始化方式不一样，可能就全是0了
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// 绘制光源体
		/*
		lightShader.Use();
		modelLoc = glGetUniformLocation(lightShader.Program, "model");
		viewLoc = glGetUniformLocation(lightShader.Program, "view");
		projLoc = glGetUniformLocation(lightShader.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // 缩小比例
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(outlineVAO);
		glDrawElements(GL_TRIANGLES_ADJACENCY, 72, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		*/

		// 使用轮廓shader
		outlineShader.Use();
		modelLoc = glGetUniformLocation(outlineShader.Program, "model");
		viewLoc = glGetUniformLocation(outlineShader.Program, "view");
		projLoc = glGetUniformLocation(outlineShader.Program, "projection");
		lightPosLoc = glGetUniformLocation(outlineShader.Program, "lightPos");
		glUniform3f(lightPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);  // 摄像机的位置是光源
		// 设置矩阵
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(1.2f)); // 放大比例
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(outlineVAO);
		glDrawElements(GL_TRIANGLES_ADJACENCY, 72, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		

		// Swap the screen buffers
		glfwSwapBuffers(window);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset*0.1, yoffset*0.1);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

