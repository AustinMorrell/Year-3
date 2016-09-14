#include "RenderingGeometryApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "Camera.h"
#include "Gizmos.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

RenderingGeometryApplication::RenderingGeometryApplication()
	: m_camera(nullptr) {

}

RenderingGeometryApplication::~RenderingGeometryApplication() {

}

bool RenderingGeometryApplication::startup() {

	Sphere();

	// create shader
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec4 Colour; \
							out vec4 vColour; \
							uniform mat4 ProjectionViewWorld; \
							void main() { vColour = Colour; \
							gl_Position = ProjectionViewWorld * Position; }";

	const char* fsSource = "#version 410\n \
							in vec4 vColour; \
							out vec4 FragColor; \
							void main() { FragColor = vColour; }";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);

	// check that it compiled and linked correctly
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength + 1];
		glGetProgramInfoLog(m_program, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	// we don't need to keep the individual shaders around
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	return true;
}

void RenderingGeometryApplication::createCube()
{
	createWindow("MakingASphere", 1280, 720);

	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(10, 10, 10), vec3(0));

	Vertex vertices[4];
	unsigned int indices[4] = { 0,2,1,3 };

	m_indexCount = 4;

	vertices[0].position = glm::vec4(-2, 0, -2, 1);
	vertices[1].position = glm::vec4(2, 0, -2, 1);
	vertices[2].position = glm::vec4(-2, 0, 2, 1);
	vertices[3].position = glm::vec4(2, 0, 2, 1);

	/*vertices[0].color = glm::vec4(1, 0, 0, 1);
	vertices[1].color = glm::vec4(0, 1, 0, 1);
	vertices[2].color = glm::vec4(0, 0, 1, 1);
	vertices[3].color = glm::vec4(1, 1, 1, 1);*/

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool RenderingGeometryApplication::Sphere()
{
	createWindow("MakingASphere", 1280, 720);

	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(10, 10, 10), vec3(0));

	Vertex vertices[6];

	vertices[0].position = vec4(-5, 0, -5, 1);
	vertices[1].position = vec4(5, 0, -5, 1);
	vertices[2].position = vec4(-5, 0, 5, 1);
	vertices[3].position = vec4(5, 0, 5, 1);
	vertices[4].position = vec4(-1, 0, 1, 1);
	vertices[5].position = vec4(1, 0, 1, 1);

	unsigned int indices[6] = { 0,2,1,3,5,4 };

	m_indexCount = 6;

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

void RenderingGeometryApplication::shutdown() {

	// cleanup render data
	glDeleteProgram(m_program);
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);

	// cleanup camera and gizmos
	delete m_camera;
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool RenderingGeometryApplication::update(float deltaTime) {

	// close the application if the window closes or we press escape
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	// update the camera's movement
	m_camera->update(deltaTime);



	// return true, else the application closes
	return true;
}

void RenderingGeometryApplication::draw() {
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind shader
	glUseProgram(m_program);

	// where to send the matrix
	int matUniform = glGetUniformLocation(m_program, "ProjectionViewWorld");

	// send the matrix
	glUniformMatrix4fv(matUniform, 1, GL_FALSE, glm::value_ptr(m_camera->getProjectionView()));
	glBindVertexArray(m_vao); 
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);

}