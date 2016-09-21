#include "Engine\Graphics\Renderer.hpp"
#include <windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include "Engine\Graphics\GL\glext.h"
#include "Engine\Utilities\Time.hpp"
#include "..\Math\raw_vector.hpp"
#include "..\Math\Vertex.hpp"
#include "..\Utilities\EngineCommon.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "Engine/Graphics/Camera3D.hpp"

Renderer::Renderer(void)
{
	m_matrixStack.push_back(Matrix4());
}


Renderer::~Renderer(void)
{

}

// Globals "function pointer" variables
PFNGLGENBUFFERSPROC		glGenBuffers		= nullptr;
PFNGLBINDBUFFERPROC		glBindBuffer		= nullptr;
PFNGLBUFFERDATAPROC		glBufferData		= nullptr;
PFNGLGENERATEMIPMAPPROC	glGenerateMipmap	= nullptr;
PFNGLDELETEBUFFERSPROC	glDeleteBuffers     = nullptr;
PFNGLACTIVETEXTUREPROC  glActiveTexture     = nullptr;

PFNGLSHADERSOURCEPROC	glShaderSource		= nullptr;
PFNGLGETSHADERIVPROC	glGetShaderiv       = nullptr;
PFNGLCOMPILESHADERPROC  glCompileShader	    = nullptr;
PFNGLCREATESHADERPROC   glCreateShader	    = nullptr;
PFNGLGETUNIFORMLOCATIONPROC   glGetUniformLocation = nullptr;
PFNGLLINKPROGRAMPROC       glLinkProgram = nullptr;
PFNGLUSEPROGRAMPROC        glUseProgram = nullptr;
PFNGLCREATEPROGRAMPROC        glCreateProgram = nullptr;
PFNGLUNIFORM1IPROC			  glUniform1i = nullptr;
PFNGLUNIFORM1FPROC			  glUniform1f = nullptr;
PFNGLUNIFORM3FPROC			  glUniform3f = nullptr;
PFNGLGETPROGRAMIVPROC		  glGetProgramiv = nullptr;
PFNGLISSHADERPROC			  glIsShader = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f = nullptr;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = nullptr;
PFNGLUNIFORMMATRIX4FVPROC  glUniformMatrix4fv = nullptr;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;
PFNGLUNIFORM3FVPROC glUniform3fv = nullptr;
PFNGLUNIFORM4FVPROC glUniform4fv = nullptr;
PFNGLUNIFORM4FPROC glUniform4f = nullptr;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = nullptr;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = nullptr;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = nullptr;
PFNGLPATCHPARAMETERIPROC glPatchParameteri = nullptr;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;


void Renderer::SetIdentity() {
	//glLoadIdentity();
	m_matrixStack.back() = m_matrixStack.back().Identity();
}

void Renderer::SetPerspective(float fovY, float aspect, float zNear, float zFar ) {
	//gluPerspective(fovY, aspect, zNear, zFar);
	m_matrixStack.back() = m_matrixStack.back().BuildPerspective(fovY, aspect, zNear, zFar);
}

void Renderer::SetOrtho(double l, double r, double b, double t, double n, double f) {
	//glOrtho(l, r, b, t, n, f);
	m_matrixStack.back() = m_matrixStack.back().SetOrtho(l, r, b, t, n, f);
}

void Renderer::PopMatrix() {
	//glPopMatrix();
	if (m_matrixStack.size() > 1) {
		m_matrixStack.pop_back();
	}
}

void Renderer::RotateMatrix(float angle, float x, float y, float z) {
	//glRotatef(angle, x, y, z);
	m_matrixStack.back() = m_matrixStack.back().Rotate(angle, x, y, z);
}

void Renderer::RotateMatrix(float angle, const Vec3& axis) {
	//glRotatef(angle, axis.x(), axis.y(), axis.z());
	m_matrixStack.back() = m_matrixStack.back().Rotate(angle, axis);
}

void Renderer::TranslateMatrix(float x, float y, float z, bool isObjectSpace/*=false*/) {
	//glTranslatef(x, y, z);
	m_matrixStack.back() = m_matrixStack.back().Translate(x, y, z);

	if (isObjectSpace)
		m_objectToWorld = m_objectToWorld.Translate(x, y, z);
}

void Renderer::SetScale(float xScale, float yScale, float zScale)
{
	//glScalef(xScale, xScale, zScale);
	m_matrixStack.back() = m_matrixStack.back().Scale(xScale, yScale, zScale);
}

void Renderer::PushMatrix()
{
	//glPushMatrix();
	m_matrixStack.push_back(m_matrixStack.back());
}

void Renderer::RenderPrimitives(int primitiveType, const Vertex* vertexData, size_t numVertices, const std::string& texturePath /*= ""*/, const std::string& shaderProgName) {
	Material testMat(shaderProgName);
	testMat.AddTexture(texturePath, "u_diffuseMap");
	int shaderID = testMat.GetShaderProgramID();
	glUseProgram(shaderID);

	Matrix4 objectToWorldMatrix;
	int worldToClipMatrixLocation = glGetUniformLocation(shaderID, "u_worldToClipMatrix");
	glUniformMatrix4fv(worldToClipMatrixLocation, 1, false, &m_matrixStack.back().Get()[0]);

	int objectToWorldLocation = glGetUniformLocation(shaderID, "u_objectToWorldMatrix");
	glUniformMatrix4fv(objectToWorldLocation, 1, false, &objectToWorldMatrix.Get()[0]);

	int timeLocation = glGetUniformLocation(shaderID, "u_time");
	glUniform1f(timeLocation, (float)GetAbsoluteTimeSeconds());

	int positionLocation = glGetAttribLocation(shaderID, "a_position");
	int colorLocation = glGetAttribLocation(shaderID, "a_color");
	int texCoordsLocation = glGetAttribLocation(shaderID, "a_texCoords");

	testMat.Activate();

	glEnableVertexAttribArray(positionLocation);
	glEnableVertexAttribArray(colorLocation);
	glEnableVertexAttribArray(texCoordsLocation);

	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)&vertexData[0].m_position);
	glVertexAttribPointer(colorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)&vertexData[0].m_color);
	glVertexAttribPointer(texCoordsLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)&vertexData[0].m_texCoords);

	glDrawArrays(primitiveType, 0, numVertices);

	glDisableVertexAttribArray(positionLocation);
	glDisableVertexAttribArray(colorLocation);
	glDisableVertexAttribArray(texCoordsLocation);
}

void Renderer::CreateIcosahedron()
{
	const int Faces[] = {
		2, 1, 0,
		3, 2, 0,
		4, 3, 0,
		5, 4, 0,
		1, 5, 0,

		11, 6,  7,
		11, 7,  8,
		11, 8,  9,
		11, 9,  10,
		11, 10, 6,

		1, 2, 6,
		2, 3, 7,
		3, 4, 8,
		4, 5, 9,
		5, 1, 10,

		2,  7, 6,
		3,  8, 7,
		4,  9, 8,
		5, 10, 9,
		1, 6, 10 };

	const float Verts[] = {
		0.000f,  0.000f,  1.000f,
		0.894f,  0.000f,  0.447f,
		0.276f,  0.851f,  0.447f,
		-0.724f,  0.526f,  0.447f,
		-0.724f, -0.526f,  0.447f,
		0.276f, -0.851f,  0.447f,
		0.724f,  0.526f, -0.447f,
		-0.276f,  0.851f, -0.447f,
		-0.894f,  0.000f, -0.447f,
		-0.276f, -0.851f, -0.447f,
		0.724f, -0.526f, -0.447f,
		0.000f,  0.000f, -1.000f };

	GLsizei IndexCount = sizeof(Faces) / sizeof(Faces[0]);
	
	// Create the VAO:
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create the VBO for positions:
	GLuint positions;
	GLsizei stride = 3 * sizeof(float);
	glGenBuffers(1, &positions);
	glBindBuffer(GL_ARRAY_BUFFER, positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

	// Create the VBO for indices:
	GLuint indices;
	glGenBuffers(1, &indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);

	glDrawElements(GL_PATCHES, IndexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Renderer::RenderPrimitives(int primitiveType, const std::vector<Vertex>& vertexData, const std::string& texturePath /*= ""*/, const std::string& progName) {
	RenderPrimitives(primitiveType, &vertexData[0], vertexData.size(), texturePath, progName);
}

void Renderer::RenderTexturedAABB(const Vec2& mins, const Vec2& maxs, int colorTextureID, int depthTextureID, int shaderID) {
	glUseProgram(shaderID);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, colorTextureID);

	struct TestVertex {
		Vec3 position;
		RGBA color;
		Vec2 texCoords;
	};

	const int NUM_VERTEXES = 4;
	std::vector<TestVertex> testVertices;
	testVertices.resize( NUM_VERTEXES );
	Vec2 minTexCoords = Vec2(0.f, 0.f);
	Vec2 maxTexCoords = Vec2(1.f, 1.f);

	testVertices[0].position = Vec3(mins.x(), mins.y(), 0.f);
	testVertices[0].texCoords = Vec2(minTexCoords.x(), minTexCoords.y());
	testVertices[0].color = RGBA(255, 255, 255, 255);

	testVertices[1].position = Vec3(maxs.x(), mins.y(), 0.f);
	testVertices[1].texCoords = Vec2(maxTexCoords.x(), minTexCoords.y());
	testVertices[1].color = RGBA(255, 255, 255, 255);

	testVertices[2].position = Vec3(maxs.x(), maxs.y(), 0.f);
	testVertices[2].texCoords = Vec2(maxTexCoords.x(), maxTexCoords.y());
	testVertices[2].color = RGBA(255, 255, 255, 255);

	testVertices[3].position = Vec3(mins.x(), maxs.y(), 0.f);
	testVertices[3].texCoords = Vec2(minTexCoords.x(), maxTexCoords.y());
	testVertices[3].color = RGBA(255, 255, 255, 255);


	Matrix4 objectToWorld;
	int objectToWorldLocation = glGetUniformLocation(shaderID, "u_objectToWorldMatrix" );
	glUniformMatrix4fv(objectToWorldLocation, 1, false, &objectToWorld.Get()[0]);

	int worldToClipMatrixLocation = glGetUniformLocation(shaderID, "u_worldToClipMatrix" );
	glUniformMatrix4fv(worldToClipMatrixLocation, 1, false, &m_matrixStack.back().Get()[0]); 

	int timeLocation = glGetUniformLocation(shaderID, "u_time" );
	glUniform1f(timeLocation, (float)(GetAbsoluteTimeSeconds()));

	int positionLocation = glGetAttribLocation(shaderID, "a_position");
	int colorLocation = glGetAttribLocation(shaderID, "a_color");
	int texCoordsLocation = glGetAttribLocation(shaderID, "a_texCoords");

	glEnableVertexAttribArray(positionLocation);
	glEnableVertexAttribArray(colorLocation);
	glEnableVertexAttribArray(texCoordsLocation);

	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(TestVertex), &testVertices[0].position);
	glVertexAttribPointer(colorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TestVertex), &testVertices[0].color);
	glVertexAttribPointer(texCoordsLocation, 2, GL_FLOAT, GL_FALSE, sizeof(TestVertex), &testVertices[0].texCoords);

	glDrawArrays(GL_QUADS, 0, testVertices.size());

	glDisableVertexAttribArray(positionLocation);
	glDisableVertexAttribArray(colorLocation);
	glDisableVertexAttribArray(texCoordsLocation);
}


void Renderer::RenderPointCloud(const std::vector<Vertex>& verts) {

	Material defaultMaterial;
	defaultMaterial.Activate();
	int shaderID = defaultMaterial.GetShaderProgramID();
	glUseProgram(shaderID);

	Matrix4 objectToWorldMatrix;

	int worldToClipMatrixLocation = glGetUniformLocation(shaderID, "u_worldToClipMatrix" );
	glUniformMatrix4fv(worldToClipMatrixLocation, 1, false, &m_matrixStack.back().Get()[0]); 

	int objectToWorldLocation = glGetUniformLocation(shaderID, "u_objectToWorldMatrix" );
	glUniformMatrix4fv(objectToWorldLocation, 1, false, &objectToWorldMatrix.Get()[0]); 

	int positionLocation = glGetAttribLocation(shaderID, "a_position");
	int colorLocation = glGetAttribLocation(shaderID, "a_color");
	int texCoordsLocation = glGetAttribLocation(shaderID, "a_texCoords");

	glEnableVertexAttribArray(positionLocation);
	glEnableVertexAttribArray(colorLocation);
	glEnableVertexAttribArray(texCoordsLocation);

	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)&verts[0].m_position);
	glVertexAttribPointer(colorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)&verts[0].m_color);
	glVertexAttribPointer(texCoordsLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)&verts[0].m_texCoords);

	glDrawArrays(GL_POINTS, 0, verts.size());

	glDisableVertexAttribArray(positionLocation);
	glDisableVertexAttribArray(colorLocation);
	glDisableVertexAttribArray(texCoordsLocation);
}


bool Renderer::ComputeSurfaceTangentsAtVertex(
	Vec3& surfaceTangentAtVertex_out,
	Vec3& surfaceBitangentAtVertex_out,
	const Vec3& normalAtThisVertex,
	const Vec3& positionOfThisVertex,
	const Vec2& texCoordsOfThisVertex,
	const Vec3& positionOfPreviousAdjacentVertex,
	const Vec2& texCoordsOfPreviousAdjacentVertex,
	const Vec3& positionOfNextAdjacentVertex,
	const Vec2& texCoordsOfNextAdjacentVertex )
{
	Vec3 vecToPrevious	= positionOfPreviousAdjacentVertex - positionOfThisVertex;
	Vec3 vecToNext		= positionOfNextAdjacentVertex - positionOfThisVertex;

	Vec2 texToPrevious	= texCoordsOfPreviousAdjacentVertex - texCoordsOfThisVertex;
	Vec2 texToNext		= texCoordsOfNextAdjacentVertex - texCoordsOfThisVertex;

	float determinant = ((texToPrevious.x() * texToNext.y()) - (texToNext.x() * texToPrevious.y()));

	Vec3 uDirectionInWorldSpace(	
		(texToNext.y() * vecToPrevious.x() - texToPrevious.y() * vecToNext.x()),
		(texToNext.y() * vecToPrevious.y() - texToPrevious.y() * vecToNext.y()),
		(texToNext.y() * vecToPrevious.z() - texToPrevious.y() * vecToNext.z())
		);

	Vec3 vDirectionInWorldSpace(
		(texToPrevious.x() * vecToNext.x() - texToNext.x() * vecToPrevious.x()),
		(texToPrevious.x() * vecToNext.y() - texToNext.x() * vecToPrevious.y()),
		(texToPrevious.x() * vecToNext.z() - texToNext.x() * vecToPrevious.z())
		);

	float invDeterminant = 1.0f / determinant;
	uDirectionInWorldSpace *= invDeterminant;
	vDirectionInWorldSpace *= invDeterminant;

	surfaceTangentAtVertex_out = uDirectionInWorldSpace;
	surfaceBitangentAtVertex_out = vDirectionInWorldSpace * -1; // NOTE: You should remove this minus sign if your V texture coordinates are using the opposite convention of mine!

	// NOTE: You don't need the following code, unless you intend to reconstitute the Bitangent vector inside the vertex shader (and not pass it in as a vertex attribute)
	Vec3 naturalBitangentFromCross = normalAtThisVertex.Cross(surfaceTangentAtVertex_out) ;
	float computedBitangentDotNaturalBitangent = surfaceBitangentAtVertex_out.Dot(naturalBitangentFromCross);
	return( computedBitangentDotNaturalBitangent >= 0.f );
}

void Renderer::RenderTexturedQuadList(const std::vector<Vertex3D_PCT>& vertexData, const std::string& texturePath/*=""*/) {

	if (vertexData.empty())
		return;

	Material testMat;
	testMat.AddTexture(texturePath, "u_diffuseMap");
	int shaderID = testMat.GetShaderProgramID();
	glUseProgram(shaderID);

	Matrix4 objectToWorldMatrix;
	int worldToClipMatrixLocation = glGetUniformLocation(shaderID, "u_worldToClipMatrix");
	glUniformMatrix4fv(worldToClipMatrixLocation, 1, false, &m_matrixStack.back().Get()[0]);

	int objectToWorldLocation = glGetUniformLocation(shaderID, "u_objectToWorldMatrix");
	glUniformMatrix4fv(objectToWorldLocation, 1, false, &objectToWorldMatrix.Get()[0]);

	int timeLocation = glGetUniformLocation(shaderID, "u_time");
	glUniform1f(timeLocation, (float)GetAbsoluteTimeSeconds());

	int positionLocation = glGetAttribLocation(shaderID, "a_position");
	int colorLocation = glGetAttribLocation(shaderID, "a_color");
	int texCoordsLocation = glGetAttribLocation(shaderID, "a_texCoords");

	testMat.Activate();

	glEnableVertexAttribArray(positionLocation);
	glEnableVertexAttribArray(colorLocation);
	glEnableVertexAttribArray(texCoordsLocation);

	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D_PCT), (void*)&vertexData[0].m_position);
	glVertexAttribPointer(colorLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex3D_PCT), (void*)&vertexData[0].m_color);
	glVertexAttribPointer(texCoordsLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D_PCT), (void*)&vertexData[0].m_texCoords);

	glDrawArrays(GL_QUADS, 0, vertexData.size());

	glDisableVertexAttribArray(positionLocation);
	glDisableVertexAttribArray(colorLocation);
	glDisableVertexAttribArray(texCoordsLocation);
}


//////////////////////////////////////////////////////////////////////////
Renderer& Renderer::GetInstance() {
	static Renderer instance;
	return instance;
}

//////////////////////////////////////////////////////////////////////////
void Renderer::InitializeAdvancedOpenGLFunctions()
{
	glGenBuffers		= (PFNGLGENBUFFERSPROC)		wglGetProcAddress( "glGenBuffers" );
	glBindBuffer		= (PFNGLBINDBUFFERPROC)		wglGetProcAddress( "glBindBuffer" );
	glBufferData		= (PFNGLBUFFERDATAPROC)		wglGetProcAddress( "glBufferData" );
	glGenerateMipmap	= (PFNGLGENERATEMIPMAPPROC)	wglGetProcAddress( "glGenerateMipmap" );
	glDeleteBuffers     = (PFNGLDELETEBUFFERSPROC)	wglGetProcAddress("glDeleteBuffers");
	glActiveTexture     = (PFNGLACTIVETEXTUREPROC)  wglGetProcAddress("glActiveTexture");

	glCreateShader   = (PFNGLCREATESHADERPROC)  wglGetProcAddress("glCreateShader");
	glShaderSource     = (PFNGLSHADERSOURCEPROC)  wglGetProcAddress("glShaderSource");
	glGetShaderiv		= (PFNGLGETSHADERIVPROC)  wglGetProcAddress("glGetShaderiv");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) wglGetProcAddress("glGetUniformLocation");
	glAttachShader = (PFNGLATTACHSHADERPROC) wglGetProcAddress("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC) wglGetProcAddress("glLinkProgram");
	glUseProgram = 	(PFNGLUSEPROGRAMPROC) wglGetProcAddress("glUseProgram");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC) wglGetProcAddress("glCreateProgram");
	glUniform1i = (PFNGLUNIFORM1IPROC)	  wglGetProcAddress("glUniform1i");
	glUniform1f = (PFNGLUNIFORM1FPROC) wglGetProcAddress("glUniform1f");
	glUniform3f = (PFNGLUNIFORM3FPROC) wglGetProcAddress("glUniform3f");
	glUniform3fv = (PFNGLUNIFORM3FVPROC) wglGetProcAddress("glUniform3fv");
	glIsShader = (PFNGLISSHADERPROC) wglGetProcAddress("glIsShader");

	glGetShaderInfoLog =  (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) wglGetProcAddress("glGetProgramInfoLog");
	glCompileShader = (PFNGLCOMPILESHADERPROC) wglGetProcAddress("glCompileShader");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC) wglGetProcAddress("glGetProgramiv");
	glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)wglGetProcAddress("glGetActiveAttrib");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) wglGetProcAddress("glVertexAttribPointer");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) wglGetProcAddress("glGetAttribLocation");
	glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC) wglGetProcAddress("glVertexAttrib4f");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) wglGetProcAddress("glUniformMatrix4fv");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) wglGetProcAddress("glBindAttribLocation");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glDisableVertexAttribArray");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) wglGetProcAddress("glEnableVertexAttribArray");
	glUniform4fv = (PFNGLUNIFORM4FVPROC) wglGetProcAddress("glUniform4fv");
	glUniform4f = (PFNGLUNIFORM4FPROC) wglGetProcAddress("glUniform4f");
	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffers");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebuffer");
	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2D");
	glPatchParameteri = (PFNGLPATCHPARAMETERIPROC) wglGetProcAddress("glPatchParameteri");
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
}  

