#pragma once
#include "gl_core_4_4.h"
#define PI 3.14159265358979323846
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "Camera.h"
#include "Gizmos.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

#include "BaseApplication.h"

#include <glm/vec4.hpp>

// forward declared to reduce dependencies
class Camera;

class RenderingGeometryApplication : public BaseApplication {
public:
	struct Vertex {
		glm::vec4 position;
		glm::vec4 colour;
	};

	RenderingGeometryApplication();
	virtual ~RenderingGeometryApplication();
	virtual Vertex* generateSphereVertices(const unsigned int &sides, const unsigned int &mirid, Vertex* &halfSphere);
	virtual Vertex* generateHalfSphereVertices(unsigned int np, const int &rad);
	virtual unsigned int* generateSphereIndicies(const unsigned int &vertices, const unsigned int &mirid);
	virtual bool Sphere(const unsigned int radius, const unsigned int verts, const unsigned int halfSpheres);
	virtual void createCube();
	virtual bool startup();
	virtual void shutdown();
	std::vector<unsigned int> indicesHolder;
	virtual bool update(float deltaTime);
	virtual void draw();
	
private:

	Camera*		m_camera;

	//vertex array object, vertex buffer object and index buffer object
	unsigned int	m_vao, m_vbo, m_ibo;
	unsigned int	m_indexCount;

	unsigned int	m_program;
};