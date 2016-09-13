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


std::vector<glm::vec4> GenerateCircleVerts(int np, float radius)
{

	std::vector<glm::vec4> points;// = new std::vector<glm::vec3>();
	for (int i = 0; i < np; i++)
	{
		//get the current x_angle
		float theta = glm::pi<float>() * (float)i / (float)(np - 1);
		float newX = radius * sin(theta);
		float newY = radius * cos(theta);
		
		points.push_back(vec4(newX, newY, 0.f, 1));
	}

	return points;
}

std::vector<glm::vec4> RotatePoints(std::vector<glm::vec4> &start, int nm)
{

	std::vector<glm::vec4> verts;
	
	for (int i = 0; i <= nm; i++) //nm must be at least 3 to get 4 rotations
	{
		float phi = 2 * glm::pi<float>() * float(i) / float(nm);

		for (int j = 0; j < start.size(); j++) //np times
		{
			float newX = start[j].x  * cos(phi) + start[j].z * sin(phi);
			float newY = start[j].y;
			float newZ = start[j].z * cos(phi) - start[j].x * sin(phi);			

			verts.push_back(vec4(newX, newY, newZ, 1.f));
		}
	}

	return verts;
}

std::vector<unsigned int> GenerateIndices(int nm, int np)
{
	std::vector<unsigned int> indices;

	//j=np-1
	//		
	//2		5	8	11	14	17
	//1     4	7	10	13	16
	//0		3	6	9	12	15		
	//	

	for (unsigned int i = 0; i < nm; i++) //nm = 4
	{
		unsigned int start = i * np;
		for (int j = 0; j < np; j++) //np = 3
		{
			unsigned int botR = (start + np + j);
			unsigned int botL = (start + j);
			indices.push_back(botL);
			indices.push_back(botR);
		}
		indices.push_back(0xFFFF);

	} //we copied the origin whenever we rotated around nm + 1 times so we dont need to get the end again
	return indices;

}
unsigned int indexCount;
RenderingGeometryApplication::RenderingGeometryApplication()
	: m_camera(nullptr) {

}

RenderingGeometryApplication::~RenderingGeometryApplication() {

}

bool RenderingGeometryApplication::startup() {

	// create a basic window
	createWindow("AIE OpenGL Application", 1280, 720);

	// start the gizmo system that can draw basic immediate-mode shapes
	//Gizmos::create();

	// setup camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(10, 10, 10), vec3(0));

	// create vertex and index data for a quad
	//Vertex vertices[4];

	/*
	2 -5,5 ___ 5,5 3
	|\  |
	| \ |
	0-5,-5|__\|5,-5 1


	*/
	unsigned int numPoints = 25;
	unsigned int numMeridians = 25;
	std::vector<vec4> halfCircle = GenerateCircleVerts(numPoints, 5.f);
	std::vector<vec4> sphereVertices = RotatePoints(halfCircle, numMeridians);	
	std::vector<unsigned int> sphereIndices = GenerateIndices(numMeridians, numPoints);
	//
	indexCount = sphereIndices.size();
	int vertOffset = sphereVertices.size() * sizeof(vec4);
	int indexOffset = sphereIndices.size() * sizeof(unsigned int);
	//vertices[0].position = vec4(-5, 0, -5, 1);
	//vertices[1].position = vec4(5, 0, -5, 1);
	//vertices[2].position = vec4(-5, 0, 5, 1);
	//vertices[3].position = vec4(5, 0, 5, 1);

	//vertices[0].colour = vec4(1, 0, 0, 1);
	//vertices[1].colour = vec4(0, 1, 0, 1);
	//vertices[2].colour = vec4(0, 0, 1, 1);
	//vertices[3].colour = vec4(1, 1, 1, 1);

	//unsigned int indices[4] = { 0,2,1,3 };
	// create opengl data

	// generate buffers
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	// generate vertex array object (descriptors)
	glGenVertexArrays(1, &m_vao);

	// all changes will apply to this handle
	glBindVertexArray(m_vao);

	// set vertex buffer data
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	
	glBufferData(GL_ARRAY_BUFFER, vertOffset, sphereVertices.data(), GL_STATIC_DRAW);

	// index data

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexOffset, sphereIndices.data(), GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//// colour
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// safety
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind shader
	glUseProgram(m_program);

	// where to send the matrix
	int matUniform = glGetUniformLocation(m_program, "ProjectionViewWorld");

	// send the matrix
	glUniformMatrix4fv(matUniform, 1, GL_FALSE, glm::value_ptr(m_camera->getProjectionView()));
	glBindVertexArray(m_vao);


	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);// draw quad		
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, (void*)0);
	
}
