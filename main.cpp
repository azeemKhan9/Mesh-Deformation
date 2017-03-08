#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <vector>
//#include <stdio.h>
#include <string>
#include <cstring>

#define GLEW_STATIC
#include <GL/GLEW.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "SOIL2/SOIL2.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

bool meshReader(const char * path, std::vector<glm::vec3> & out_vertices);

const GLuint WIDTH = 800, HEIGHT = 600;

int main()
{
	glfwInit();

	//Uncomment these if on Mac OSX
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Gingerbread Man", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight); //so size of window is gauged depending on size of screen

	if (nullptr == window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialise GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader ourShader("verShader.txt", "fragShader.txt"); //Build and compile shader program

	std::vector<glm::vec3> vertices;
	bool res = meshReader("man.obj", vertices); //Read the mesh file and output the data as array of vertices

	//Load and create texture
	GLuint texture;
	int texWidth, texHeight;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char *image = SOIL_load_image("texture_42.jpg", &texWidth, &texHeight, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Attribute pointers
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);

		//draw opengl stuff
		ourShader.Use();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::lookAt(glm::vec3(0,0,4), glm::vec3(0,0,0), glm::vec3(0,7,0));
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		/*transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0f));
		transform = glm::rotate(transform, (GLfloat)glfwGetTime() * 3.0f, glm::vec3(0.0f, 0.0f, 1.0f)); //Axis of rotation
		GLint transformLocation = glGetUniformLocation(ourShader.Program, "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));*/

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	//Terminate GLFW to clear any resources allocated by it
	glfwTerminate();
	return EXIT_SUCCESS;
}

bool meshReader(const char * path, std::vector<glm::vec3> & out_vertices)
{
	printf("Loading obj file %s........\n", path);

	std::vector<unsigned int> vertexIndices;
	std::vector<glm::vec3> temp_vertices;

	FILE * file = fopen(path, "r");
	if (file == NULL) {
		std::cout << "Can't open the file!" << std::endl;
		return false;
	}

	while (1) {
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) {
			break;
		}
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f \n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3];
			int matches = fscanf(file, "%d %d %d \n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			if (matches != 3) {
				std::cout << "File cannot be read :(" << std::endl;
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
		}
	}

	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);
	}

	fclose(file);
	std::cout << "File has been read" << std::endl;
	return true;
}

