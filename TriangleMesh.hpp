#pragma once
#include <vector>
#include "Math\raw_vector.hpp"
#include "Math\Vertex.hpp"

class Material;

class TriangleMeshData
{
public:
	TriangleMeshData();

public:
	std::vector< Vertex >	m_vertices;
	std::vector< unsigned int >		m_indices;
};


class TriangleMesh
{
public:
	TriangleMesh(Material* meshMaterial);
	TriangleMesh();
	~TriangleMesh(void);

	void Render(const Vec3& lightPosition, const Vec3& cameraPosition);
	void GenerateVertexArrayAndVBO();
public:
	unsigned int m_vboID;
	unsigned int m_iboID;
	unsigned int m_numVertices;
	unsigned int m_numIndices;

	unsigned int m_primitiveDrawMode;
	static bool s_staticDrawModeEnabled;

	Material* m_material;
	
};

