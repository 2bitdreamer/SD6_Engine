#pragma once
#include "Math/Vertex.hpp"
#include <string>
#include <vector>

class TriangleMesh;
class Material;
class BinaryFileParser;

void GenerateTangentData(std::vector<Vertex>& triangleVerts);
void GenerateNormalData(std::vector<Vertex>& triangleVerts);

std::string GetPathForModelName(const std::string& name);
Material* CreateNewMaterialFromModelName(const std::string& modelName);
TriangleMesh* CreateNewMeshFromModelName(const std::string& modelName);

bool ReadVertexListFromBuffer(std::vector<Vertex>& out_vertexList, unsigned int numVertices, BinaryFileParser& bfp);
bool ReadVertexFromBuffer(Vertex& out_vertex, BinaryFileParser& bfp);
bool ReadC23(std::vector<Vertex>& out_vertexList, std::vector<unsigned int>& out_indexList, const std::string& filePath);