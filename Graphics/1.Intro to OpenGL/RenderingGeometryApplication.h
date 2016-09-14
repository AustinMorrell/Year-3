#pragma once

#include "BaseApplication.h"

#include <glm/vec4.hpp>

// forward declared to reduce dependencies
class Camera;

class RenderingGeometryApplication : public BaseApplication {
public:

	RenderingGeometryApplication();
	virtual ~RenderingGeometryApplication();

	virtual bool Sphere();
	virtual void createCube();
	virtual bool startup();
	virtual void shutdown();

	virtual bool update(float deltaTime);
	virtual void draw();

private:

	Camera*		m_camera;

	struct Vertex {
		glm::vec4 position;
		//glm::vec4 colour;
	};

	//vertex array object, vertex buffer object and index buffer object
	unsigned int	m_vao, m_vbo, m_ibo;
	unsigned int	m_indexCount;

	unsigned int	m_program;
};