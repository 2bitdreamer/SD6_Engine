#include "TriangleMesh.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Material.hpp"
#include <stddef.h>


TriangleMesh::TriangleMesh(Material* meshMaterial) :
m_vboID(0),
m_numVertices(0),
m_numIndices(0),
m_primitiveDrawMode(GL_TRIANGLES),
m_material(meshMaterial)
{
}

TriangleMesh::TriangleMesh() :
m_vboID(0),
m_numVertices(0),
m_numIndices(0),
m_primitiveDrawMode(GL_TRIANGLES),
m_material(nullptr)
{

}

//#TODO: Renderer should know about current camera and light position

void TriangleMesh::Render(const Vec3& lightPosition, const Vec3& cameraPosition)
{

	glEnable(GL_DEPTH_TEST);
	Renderer myRenderer = Renderer::GetInstance();

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID);
	
	m_material->Activate();
	int shaderID = m_material->GetShaderProgramID();
	glUseProgram(shaderID);

	Matrix4 objectToWorldMatrix;

	int worldToClipMatrixLocation = glGetUniformLocation(shaderID, "u_worldToClipMatrix");
	glUniformMatrix4fv(worldToClipMatrixLocation, 1, false, &myRenderer.m_matrixStack.back().Get()[0]);

	int objectToWorldLocation = glGetUniformLocation(shaderID, "u_objectToWorldMatrix");
	glUniformMatrix4fv(objectToWorldLocation, 1, false, &objectToWorldMatrix.Get()[0]);

	int lightLocation = glGetUniformLocation(shaderID, "u_lightWorldPos");
	glUniform3fv(lightLocation, 1, &lightPosition.x());

	int cameraLocation = glGetUniformLocation(shaderID, "u_cameraWorldPos");
	glUniform3fv(cameraLocation, 1, &cameraPosition.x());


	int positionLocation = glGetAttribLocation(shaderID, "a_position");
	int colorLocation = glGetAttribLocation(shaderID, "a_color");
	int texCoordsLocation = glGetAttribLocation(shaderID, "a_texCoords");
	int normalsLocation = glGetAttribLocation(shaderID, "a_normal");
	int tangentLocation = glGetAttribLocation(shaderID, "a_tangent");
	int bitangentLocation = glGetAttribLocation(shaderID, "a_bitangent");


	glEnableVertexAttribArray(positionLocation);
	glEnableVertexAttribArray(colorLocation);
	glEnableVertexAttribArray(texCoordsLocation);
	glEnableVertexAttribArray(normalsLocation);
	glEnableVertexAttribArray(tangentLocation);
	glEnableVertexAttribArray(bitangentLocation);

	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_position));
	glVertexAttribPointer(colorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, m_color));
	glVertexAttribPointer(texCoordsLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_texCoords));
	glVertexAttribPointer(normalsLocation, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal));
	glVertexAttribPointer(tangentLocation, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, m_tangent));
	glVertexAttribPointer(bitangentLocation, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, m_bitangent));

	glDrawElements(
		GL_TRIANGLES,
		m_numIndices, //numIndices
		GL_UNSIGNED_INT,
		(void*)0
		);

	//glDrawArrays(GL_TRIANGLES, 0, m_numVertices); //m_numVertices

	glDisableVertexAttribArray(positionLocation);
	glDisableVertexAttribArray(colorLocation);
	glDisableVertexAttribArray(texCoordsLocation);
	glDisableVertexAttribArray(normalsLocation);
	glDisableVertexAttribArray(tangentLocation);
	glDisableVertexAttribArray(bitangentLocation);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


TriangleMesh::~TriangleMesh(void)
{
}


