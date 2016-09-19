#include "RenderingGeometryApplication.h"

RenderingGeometryApplication::RenderingGeometryApplication()
	: m_camera(nullptr) {

}

RenderingGeometryApplication::~RenderingGeometryApplication() {

}

bool RenderingGeometryApplication::startup() {

	Sphere(5, 30, 30);

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

	vertices[0].colour = glm::vec4(1, 0, 0, 1);
	vertices[1].colour = glm::vec4(0, 1, 0, 1);
	vertices[2].colour = glm::vec4(0, 0, 1, 1);
	vertices[3].colour = glm::vec4(1, 1, 1, 1);

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

RenderingGeometryApplication::Vertex* RenderingGeometryApplication::generateHalfSphereVertices(unsigned int np, const int &rad)
{
	RenderingGeometryApplication::Vertex* vertices = new RenderingGeometryApplication::Vertex[np];

	for (int i = 0; i < np; i++)
	{
		float angle = PI * i / (np - 1);
		//vertices[i].position = glm::vec4(rad * sin(angle), rad * cos(angle), 0, 1);
		vertices[i].position = glm::vec4(rad * cos(angle), rad * sin(angle), 0, 1);
	}
	return vertices;
}

RenderingGeometryApplication::Vertex* RenderingGeometryApplication::generateSphereVertices(const unsigned int &sides, const unsigned int &mirid, Vertex* &halfSphere)
{
	int count = 0;
	RenderingGeometryApplication::Vertex* vertices = new RenderingGeometryApplication::Vertex[sides * mirid];

	for (int i = 0; i < mirid; i++)
	{
		float phi = 2.f * PI * ((float)i / (float)(mirid));
		for (int j = 0; j < sides; j++, count++)
		{
			//float x = halfSphere[j].position.x * cos(phi) + halfSphere[j].position.z * sin(phi);
			//float y = halfSphere[j].position.y;
			//float z = halfSphere[j].position.z * -sin(phi) + halfSphere[j].position.x * cos(phi);

			float x = halfSphere[j].position.x;
			float y = halfSphere[j].position.y * cos(phi) - halfSphere[j].position.z * sin(phi);
			float z = halfSphere[j].position.z * cos(phi) + halfSphere[j].position.y * sin(phi);


			vertices[count].position = glm::vec4(x, y, z, 1);
			vertices[count].colour = glm::vec4(1, 0, 0, 1);
		}
	}
	return vertices;
}

unsigned int* RenderingGeometryApplication::generateSphereIndicies(const unsigned int &vertices, const unsigned int &mirid)
{
	unsigned int* indices = new unsigned int[2 * (vertices * (mirid + 1))];
	m_indexCount = 2 * (vertices * (mirid + 1));

	for (unsigned int i = 0; i < mirid; i++)
	{
		unsigned int beginning = i * vertices;
		for (int j = 0; j < vertices; j++)
		{
			unsigned int botR = ((beginning + vertices + j) % (vertices * mirid));
			unsigned int botL = ((beginning + j) % (vertices * mirid));
			indicesHolder.push_back(botL);
			indicesHolder.push_back(botR);
		}
		indicesHolder.push_back(0xFFFF);
	}

	for (int i = 0; i < indicesHolder.size(); i++) {
		indices[i] = indicesHolder[i];
	}
	return indices;
}

bool RenderingGeometryApplication::Sphere(const unsigned int radius, const unsigned int verts, const unsigned int halfSpheres)
{
	createWindow("MakingASphere", 1280, 720);

	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(10, 10, 10), vec3(0));

	const unsigned int size = (verts) * (halfSpheres);

	Vertex* vertices = new Vertex[size];
	unsigned int* indices;

	Vertex* halfSpheresVerts = generateHalfSphereVertices(verts, radius);
	vertices = generateSphereVertices(verts, halfSpheres, halfSpheresVerts);
	indices = generateSphereIndicies(verts, halfSpheres);

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (m_indexCount * sizeof(unsigned int)), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

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