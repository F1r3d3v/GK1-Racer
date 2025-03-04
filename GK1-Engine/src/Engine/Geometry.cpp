#include "Engine/Geometry.h"
#include <iostream>

Geometry::Geometry() : m_vao(0), m_vbo(0), m_ebo(0), m_vertexCount(0), m_indexCount(0)
{
}

Geometry::Geometry(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) : Geometry()
{
	SetData(vertices, indices);
}

Geometry::~Geometry()
{
	Cleanup();
}

void Geometry::Cleanup()
{
	if (m_ebo != 0)
	{
		glDeleteBuffers(1, &m_ebo);
		m_ebo = 0;
	}
	if (m_vbo != 0)
	{
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}
	if (m_vao != 0)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
}

void Geometry::SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
{
	Cleanup();

	m_vertices = vertices;
	m_indices = indices;
	m_vertexCount = vertices.size();
	m_indexCount = indices.size();

	SetupGeometry();
}

void Geometry::SetupGeometry()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);

	// Load vertex data
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
				 m_vertices.data(), GL_STATIC_DRAW);

	// Load index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t),
				 m_indices.data(), GL_STATIC_DRAW);

	// Set vertex attribute pointers
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void *)offsetof(Vertex, position));

	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void *)offsetof(Vertex, normal));

	// Texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void *)offsetof(Vertex, texCoords));

	// Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void *)offsetof(Vertex, tangent));

	// Bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
						  (void *)offsetof(Vertex, bitangent));

	glBindVertexArray(0);
}