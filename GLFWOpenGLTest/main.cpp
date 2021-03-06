// VS 2015 includes
#include <iostream>
#include <vector>
#include <cmath>
// GLEW (NOTICE: GLEW MUST BE ALWAYS INCLUDED BEFORE GLFW)
#define GLEW_STATIC
#include <GL\glew.h>
// GLFW
#include <GLFW\glfw3.h>
// GLM Mathematics
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

// 3rdparty
#include "lodepng.h"

// Project includes
#include "Shader.h"
#include "Camera.h"

// Window dimensions
const GLuint screenWidth = 1280, screenHeight = 720;
// Functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

// Camera
GLfloat lastX = screenWidth / 2.0, lastY = screenHeight / 2.0;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
bool firstMouse = true;

// Position of the light/lamp
glm::vec3 lightPos(1.2f, 0.5f, 1.0f);

// Deltatime
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
	std::cout << "Starting GLFW context, OpenGL3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Setting all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create a GLFWwindows object that we can use for GLFW's funtions
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Tutorial", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
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

	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	//Setup OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Build and compile our shader program
	Shader ourShader("VertexShader.txt", "FragmentShader.txt");
	Shader lampShader("LampVertexShader.txt", "LampFragmentShader.txt");
	

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions          // Normals          // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f, 0.0f,   1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f, 0.0f,   1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f, 0.0f,   0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f, 0.0f,   1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f, 0.0f,   1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f, 0.0f,   1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f, 0.0f,   0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f, 0.0f,   1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,   0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f,
		 0.5f, 0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 1.0f,
		 0.5f, 0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
		 0.5f, 0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   1.0f, 0.0f,
		-0.5f, 0.5f,  0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f,   0.0f,  1.0f, 0.0f,   0.0f, 1.0f
	};
	GLuint indices[] = {
		0, 1, 3,	// First Triangle
		1, 2, 3		// Second Triangle
	};
	
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	glm::vec3 cubePositions2[] = {
		glm::vec3(0.0f, 0.0f, -2.0f),
		glm::vec3(0.0f, 1.0f, -4.0f),
		glm::vec3(0.0f, 2.0f, -6.0f),
		glm::vec3(0.0f, 3.0f, -8.0f),
		glm::vec3(0.0f, 4.0f, -10.0f),
		glm::vec3(0.0f, 5.0f, -12.0f),
		glm::vec3(0.0f, 6.0f, -14.0f),
		glm::vec3(0.0f, 7.0f, -16.0f),
		glm::vec3(0.0f, 8.0f, -18.0f),
		glm::vec3(0.0f, 9.0f, -20.0f)
	};

	GLfloat lightVertices[] = {
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,

		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,

		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f
	};
	glm::vec3 pointLightPositions[] =
	{
		glm::vec3( 0.7f, 0.2f, 2.0f),
		glm::vec3( 2.3f,-3.3f,-4.0f),
		glm::vec3(-4.0f, 2.0f,-12.0f),
		glm::vec3( 0.0f, 0.0f,-3.0f)
	};

	glm::vec3 pointLightPositions2[] =
	{
		glm::vec3(0.5f, 0.5f, 2.0f),
		glm::vec3(2.5f, 2.0f, -6.0f),
		glm::vec3(-2.5f, 6.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	// Vertex Buffer Objects
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Element Buffer Objects
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal attribpointer
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0);

	//-------------------------------------------------------------------
	// LIGHTNING
	GLuint lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	// Adding textures
	std::vector<unsigned char> png, pixels;
	
	GLuint width, height;
	GLuint texture;
	GLuint texture2;

	GLuint diffuseMap;

	glGenTextures(1, &diffuseMap);
	glBindTexture(GL_TEXTURE_2D, diffuseMap);

	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loading the image, creating a texture and generate mipmaps
	lodepng::load_file(png, "woodbox.png");
	lodepng::decode(pixels, width, height, png.data(), png.size());

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Second texture
	png.clear();
	pixels.clear();

	GLuint specularMap;

	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	//// Set our texture parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// Set texture filtering
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	lodepng::load_file(png, "smiley.png");
	lodepng::decode(pixels, width, height, png.data(), png.size());

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculating the deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();

		// Render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Load shaders
		ourShader.Use();
		
		// Binding texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glUniform1i(glGetUniformLocation(ourShader.Program, "material.diffuse"), 0);

		//.... texture2
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glUniform1i(glGetUniformLocation(ourShader.Program, "material.specular"), 1);
		
		//View position
		glUniform3f(glGetUniformLocation(ourShader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		// Material properties
		glUniform1f(glGetUniformLocation(ourShader.Program, "material.shininess"), 32.0f);

		// Directional light
		glUniform3f(glGetUniformLocation(ourShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(ourShader.Program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(ourShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(ourShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
		
		// Light materials
		glUniform3f(glGetUniformLocation(ourShader.Program, "light.ambient"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(ourShader.Program, "light.diffuse"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(ourShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(ourShader.Program, "light.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(ourShader.Program, "light.linear"), 0.09);
		glUniform1f(glGetUniformLocation(ourShader.Program, "light.quadratic"), 0.032);
		glUniform1f(glGetUniformLocation(ourShader.Program, "material.shininess"), 32.0f);

		// 4 point lights
		for (GLuint i = 0; i < 4; i++)
		{
			std::string number = std::to_string(i);

			glUniform3f(glGetUniformLocation(ourShader.Program, ("pointLights[" + number + "].position").c_str()), pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
			glUniform3f(glGetUniformLocation(ourShader.Program, ("pointLights[" + number + "].ambient").c_str()), 0.05f, 0.05f, 0.05f);
			glUniform3f(glGetUniformLocation(ourShader.Program, ("pointLights[" + number + "].diffuse").c_str()), 0.8f, 0.8f, 0.8f);
			glUniform3f(glGetUniformLocation(ourShader.Program, ("pointLights[" + number + "].specular").c_str()), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(ourShader.Program, ("pointLights[" + number + "].constant").c_str()), 1.0f);
			glUniform1f(glGetUniformLocation(ourShader.Program, ("pointLights[" + number + "].linear").c_str()), 0.09f);
			glUniform1f(glGetUniformLocation(ourShader.Program, ("pointLights[" + number + "].quadratic").c_str()), 0.032f);
		}
		// SpotLight
		glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
		glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(ourShader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(ourShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(ourShader.Program, "spotLight.linear"), 0.09);
		glUniform1f(glGetUniformLocation(ourShader.Program, "spotLight.quadratic"), 0.032);
		glUniform1f(glGetUniformLocation(ourShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(ourShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Camera/View Transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Projection
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);

		// Get their uniform locations
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projectionLoc = glGetUniformLocation(ourShader.Program, "projection");
		
		// Passing them to shaders
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		// Draw box
		glBindVertexArray(VAO);
		glm::mat4 model;
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glBindVertexArray(0);

		for (GLuint i = 0; i < 10; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, cubePositions2[i]);
			GLfloat angle = glm::radians(20.0f) * i;
			model = glm::rotate(model, (GLfloat)glfwGetTime() * angle, glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Lamps
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projectionLoc = glGetUniformLocation(lampShader.Program, "projection");
		// Matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//Drawing light object using light's vertex attributes
		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, pointLightPositions2[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);
		
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Deleting all resources, what have been rendered
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// Clearing any resources allocated by GLFW
	glfwTerminate();
	return 0;
}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key >=  0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
	
}

void do_movement()
{
	// Camera controls
	//GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;		// Reversed since y-coordinates go from bottom to left
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}