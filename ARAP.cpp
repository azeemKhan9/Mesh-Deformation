#include <iostream>
#include <Eigen/Dense>

#define GLEW_STATIC
#include <GL/GLEW.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

struct Point
{
	GLfloat x;
	GLfloat y;
};

Point p;
bool clicked;
int num_clicks = 0;
GLint ver_pos[3] = { 0, 0, 0 };

void mouseClickCallBack(GLFWwindow * window, int button, int action, int mode)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (num_clicks == 3) {
			return;
		}
		clicked = true;
		num_clicks += 1;
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		p.x = xpos;
		p.y = ypos;
	}
}

void mouseToWorld(double inputx, double inputy, int width, int height, glm::mat4 invProjMat) 
{
	float x = (float)(-2.0f*inputx) / (float)(width) + 1, y = (float)(2.0f*inputy) / (float)width + 1;
	glm::vec4 V = glm::vec4(x, y, 0, 1);
	glm::vec4 world = invProjMat * V;
	p.x = (world.x * world.z) / world.w;
	p.y = (world.y * world.z) / world.w;
}

void nearestVertex(double inputx, double inputy, glm::vec3 vertices, size_t num_vertices, double scale)
{
	double d_min = 1.0e5, d = 1.0e5;
	int min_index = 0;
	for (int i = 0; i < num_vertices; i++) {
		float x = vertices[3 * i], y = vertices[3 * i + 1];
		d = sqrt((inputx / scale - x)*(inputx / scale - x) + (inputy / scale - y)*(inputy / scale - y));
		if (d < d_min) {
			d_min = d;
			min_index = 3 * i;
		}
	}
	p.x = vertices[min_index];
	p.y = vertices[min_index + 1];
	ver_pos[num_clicks - 1] = min_index / 3;
}


