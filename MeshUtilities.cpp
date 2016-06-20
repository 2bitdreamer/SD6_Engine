
#include "MeshUtilities.hpp"
#include "Math/Vertex.hpp"
#include <vector>
#include "TriangleMesh.hpp"
#include "Utilities/BinaryFileParser.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Material.hpp"
class TriangleMesh;


TriangleMesh* CreateNewMeshFromModelName(const std::string& modelName) {

	Material* modelMaterial = CreateNewMaterialFromModelName(modelName);
	TriangleMesh* tMesh = new TriangleMesh(modelMaterial);
	std::string c23path = GetPathForModelName(modelName) + modelName + ".c23";


	std::vector<Vertex> vertexList;
	std::vector<unsigned int> indexList;
	bool readCorrectly = ReadC23(vertexList, indexList, c23path);
	if (!readCorrectly)
		return nullptr;

	tMesh->m_numVertices = vertexList.size();
	tMesh->m_numIndices = indexList.size();

	if (tMesh->m_vboID == 0)
	{
		glGenBuffers(1, &tMesh->m_vboID);
		glGenBuffers(1, &tMesh->m_iboID);
	}

	glBindBuffer(GL_ARRAY_BUFFER, tMesh->m_vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexList.size(), vertexList.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tMesh->m_iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexList.size() * sizeof(unsigned int), indexList.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return tMesh;
}


void FillIndexData(std::vector<unsigned int>& out_indexList, unsigned int numVertices)
{
	out_indexList.reserve(numVertices);
	for (unsigned int index = 0; index < numVertices; index++)
		out_indexList.push_back(index);
}


void GenerateTangentData(std::vector<Vertex>& triangleVerts) {
	Vec3 surfaceTangentAtVertex;
	Vec3 surfaceBitangentAtVertex;
	Renderer& myRenderer = Renderer::GetInstance();

	for (size_t index = 0; index < triangleVerts.size(); index+=3) {
		Vertex& v0 = triangleVerts[index];
		Vertex& v1 = triangleVerts[index + 1];
		Vertex& v2 = triangleVerts[index + 2];

		myRenderer.ComputeSurfaceTangentsAtVertex(v0.m_tangent, v0.m_bitangent, v0.m_normal, v0.m_position, v0.m_texCoords, v2.m_position, v2.m_texCoords, v1.m_position, v1.m_texCoords);
		myRenderer.ComputeSurfaceTangentsAtVertex(v1.m_tangent, v1.m_bitangent, v1.m_normal, v1.m_position, v1.m_texCoords, v0.m_position, v0.m_texCoords, v2.m_position, v2.m_texCoords);
		myRenderer.ComputeSurfaceTangentsAtVertex(v2.m_tangent, v2.m_bitangent, v2.m_normal, v2.m_position, v2.m_texCoords, v1.m_position, v1.m_texCoords, v0.m_position, v0.m_texCoords);
	}

}

void GenerateNormalData(std::vector<Vertex>& triangleVerts) {
	Vec3 surfaceNormal;

	for (size_t index = 0; index < triangleVerts.size(); index += 3) {
		Vertex& v0 = triangleVerts[index];
		Vertex& v1 = triangleVerts[index + 1];
		Vertex& v2 = triangleVerts[index + 2];

		Vec3 a = v1.m_position - v0.m_position;
		Vec3 b = v2.m_position - v0.m_position;

		surfaceNormal = a.Cross(b);

		v0.m_normal = surfaceNormal;
		v1.m_normal = surfaceNormal;
		v2.m_normal = surfaceNormal;
	}
}


inline std::string GetPathForModelName(const std::string& name) {
	const std::string basePath = "Data/Models/" + name + "/";
	return basePath;
}


Material* CreateNewMaterialFromModelName(const std::string& modelName) {
	const std::string basePath = GetPathForModelName(modelName);

	const std::string diffusePath = basePath + modelName + "_" + "Diffuse.png";
	const std::string normalPath = basePath + modelName + "_" + "Normal.png";
	const std::string specGlossEmitPath = basePath + modelName + "_" + "SpecGlossEmit.png";

	Material* resultMaterial = new Material("PhongShader");
	resultMaterial->AddTexture(diffusePath, "u_diffuseMap");
	resultMaterial->AddTexture(normalPath, "u_normalMap");
	resultMaterial->AddTexture(specGlossEmitPath, "u_specularMap");
	return resultMaterial;
}

bool ReadVertexListFromBuffer(std::vector<Vertex>& out_vertexList, unsigned int numVertices, BinaryFileParser& bfp) {
	if (numVertices == 0)
		return true;

	out_vertexList.resize(numVertices);

	for (size_t index = 0; index < numVertices; index++) {
		Vertex outVertex;
		bool success = ReadVertexFromBuffer(outVertex, bfp);
		if (!success)
			return false;
		out_vertexList[index] = outVertex;
	}

	if (out_vertexList[0].m_normal.x() == 0.f && out_vertexList[0].m_normal.y() == 0.f && out_vertexList[0].m_normal.z() == 0.f)
		GenerateNormalData(out_vertexList);

	//if (out_vertexList[0].m_tangent.x() == 0.f && out_vertexList[0].m_tangent.y() == 0.f && out_vertexList[0].m_tangent.z() == 0.f)
		//GenerateTangentData(out_vertexList);

	return true;
}

bool ReadVertexFromBuffer(Vertex& out_vertex, BinaryFileParser& bfp) {
	bool readPosition = bfp.ReadVec3(out_vertex.m_position);
	bool readColor = bfp.ReadRGBA(out_vertex.m_color);
	bool readTexCoords = bfp.ReadVec2(out_vertex.m_texCoords);
	bool readNormal = bfp.ReadVec3(out_vertex.m_normal);
	bool readTangent = bfp.ReadVec3(out_vertex.m_tangent);
	bool readBitangent = bfp.ReadVec3(out_vertex.m_bitangent);
	return (readPosition && readColor && readTexCoords && readTangent && readBitangent && readNormal);
}

bool ReadC23(std::vector<Vertex>& out_vertexList, std::vector<unsigned int>& out_indexList, const std::string& filePath) {
	bool success = true;

	BinaryFileParser bfp = BinaryFileParser(filePath);

	std::vector<char> fourCC_code;
	bool fourCCRead = bfp.ReadCharArray(fourCC_code, 4);
	if (!(fourCCRead && fourCC_code[0] == 'G' && fourCC_code[1] == 'C' && fourCC_code[2] == '2' && fourCC_code[3] == '3'))
		return false;

	unsigned char subtype;
	success = bfp.ReadUnsignedChar(subtype);
	if (!success)
		return false;
 
	if (subtype != 2)
		return false;

	unsigned char versionNumber;
	success = bfp.ReadUnsignedChar(versionNumber);
	if (!success)
		return false;

	if (versionNumber != 1)
		return false;

	std::string commentText;
	success = bfp.ReadString(commentText);
	if (!success)
		return false;

	unsigned int numVertices;
	success = bfp.ReadUnsignedInt(numVertices);
	if (!success)
		return false;

	success = ReadVertexListFromBuffer(out_vertexList, numVertices, bfp);
	if (!success)
		return false;

	unsigned int numIndices;
	success = bfp.ReadUnsignedInt(numIndices);
	if (!success)
		return false;

	if (numIndices == 0) {
		FillIndexData(out_indexList, numVertices);
	}
	else {
		success = bfp.ReadPODList<unsigned int>(out_indexList, numIndices);
	}

	return success;

}