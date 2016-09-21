#pragma once
#include "Engine/Math/raw_vector.hpp"
#include <Windows.h>
#include <Gl\GL.h>
#include "Engine/Graphics/gl/glext.h"
#include "Engine/Math/Vertex.hpp"
#include "Engine/Math/Matrices.h"
#include <stack>
#include <vector>
#include "Engine/Graphics/Framebuffer.hpp"
#include "Engine/Math/Vertex3D_PCT.hpp"
#include "Texture.hpp"
#include "../TriangleMesh.hpp"
#include <string>
class ShaderProgram;
class Camera3D;

extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
extern PFNGLPATCHPARAMETERIPROC glPatchParameteri;
extern PFNGLGENBUFFERSPROC		glGenBuffers;
extern PFNGLBINDBUFFERPROC		glBindBuffer;
extern PFNGLBUFFERDATAPROC		glBufferData;		
extern PFNGLGENERATEMIPMAPPROC	glGenerateMipmap;	
extern PFNGLDELETEBUFFERSPROC	glDeleteBuffers;     
extern PFNGLACTIVETEXTUREPROC  glActiveTexture;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLSHADERSOURCEPROC	glShaderSource;	
extern PFNGLGETSHADERIVPROC	glGetShaderiv;
extern PFNGLCOMPILESHADERPROC  glCompileShader;	    
extern PFNGLCREATESHADERPROC   glCreateShader;	    
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLGETUNIFORMLOCATIONPROC   glGetUniformLocation;
extern PFNGLLINKPROGRAMPROC       glLinkProgram;
extern PFNGLUSEPROGRAMPROC  glUseProgram;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLUNIFORM1IPROC	  glUniform1i;
extern PFNGLUNIFORM1FPROC     glUniform1f;
extern PFNGLUNIFORM3FPROC     glUniform3f;
extern PFNGLUNIFORM3FVPROC	  glUniform3fv;
extern PFNGLGETPROGRAMIVPROC  glGetProgramiv;
extern PFNGLISSHADERPROC glIsShader;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;


#define WGL_CURRENT_BIT                    0x00000001

/* TextureMagFilter */
#define WGL_NEAREST                        0x2600
#define WGL_LINEAR                         0x2601

/* TextureParameterName */
#define WGL_TEXTURE_MAG_FILTER             0x2800
#define WGL_TEXTURE_MIN_FILTER             0x2801
#define WGL_TEXTURE_WRAP_S                 0x2802
#define WGL_TEXTURE_WRAP_T                 0x2803

/* TextureMinFilter */
#define WGL_NEAREST_MIPMAP_NEAREST         0x2700
#define WGL_LINEAR_MIPMAP_NEAREST          0x2701
#define WGL_NEAREST_MIPMAP_LINEAR          0x2702
#define WGL_LINEAR_MIPMAP_LINEAR           0x2703

/*POLYGON STUFF */
#define WGL_POINTS                         0x0000
#define WGL_LINES                          0x0001
#define WGL_QUADS                          0x0007
#define WGL_POLYGON                        0x0009

/* DataType */
#define WGL_BYTE                           0x1400
#define WGL_UNSIGNED_BYTE                  0x1401
#define WGL_SHORT                          0x1402
#define WGL_INT                            0x1404
#define WGL_UNSIGNED_INT                   0x1405
#define WGL_FLOAT                          0x1406

/* PixelFormat */
#define WGL_RGB                            0x1907
#define WGL_RGBA                           0x1908

/* Tex stuff */
#define WGL_TEXTURE_2D                     0x0DE1
#define WGL_UNPACK_ALIGNMENT               0x0CF5

/* TextureWrapMode */
#define WGL_CLAMP                          0x2900
#define WGL_REPEAT                         0x2901
#define WGL_TEXTURE_MAX_LEVEL              0x813D
#define WGL_ARRAY_BUFFER                   0x8892
/* Array */
#define WGL_VERTEX_ARRAY                   0x8074
#define WGL_COLOR_ARRAY                    0x8076
#define WGL_TEXTURE_COORD_ARRAY            0x8078

/* draw modes */
#define WGL_STATIC_DRAW                    0x88E4

/* depth data */

#define WGL_DEPTH_TEST                     0x0B71
#define WGL_ALPHA_TEST                     0x0BC0

/* Boolean */
#define WGL_TRUE                           1
#define WGL_FALSE                          0

#define WGL_CULL_FACE                      0x0B44
#define WGL_LEQUAL                         0x0203
#define WGL_GREATER                        0x0204

/* FrontFaceDirection */
#define WGL_CW                             0x0900
#define WGL_CCW                            0x0901

#define WGL_FRONT                          0x0404
#define WGL_BACK                           0x0405
#define WGL_FRONT_AND_BACK                 0x0408

#define WGL_PROJECTION                     0x1701
#define WGL_MODELVIEW                      0x1700


class Renderer
{
public:
	Renderer(void);
	~Renderer(void);
	void InitializeAdvancedOpenGLFunctions();
	void SetIdentity();

	void SetPerspective(float fovY, float aspect, float zNear, float zFar );
	void SetOrtho(double l, double r, double b, double t, double n, double f);

	void PopMatrix();
	void PushMatrix();

	void RenderPrimitives(int primitiveType, const std::vector<Vertex>& vertexData, const std::string& texturePath = "", const std::string& spn = "");
	void RenderPrimitives(int primitiveType, const Vertex* vertexData, size_t numVertices, const std::string& texturePath = "", const std::string& spn = "");
	void CreateIcosahedron();
	void RotateMatrix(float angle, float x, float y, float z);
	void RotateMatrix(float angle, const Vec3& axis);

	void TranslateMatrix(float x, float y, float z, bool isObjectSpace=false);

	void SetScale(float xScale, float yScale, float zScale);

	void RenderTexturedAABB(const Vec2& mins, const Vec2& maxs, int colorTextureID, int depthTextureID, int shaderID);
	void RenderPointCloud(const std::vector<Vertex>& verts);
	void RenderTexturedQuadList(const std::vector<Vertex3D_PCT>& vertexData, const std::string& texturePath = "");

	bool ComputeSurfaceTangentsAtVertex( Vec3& surfaceTangentAtVertex_out, Vec3& surfaceBitangentAtVertex_out, const Vec3& normalAtThisVertex, const Vec3& positionOfThisVertex, const Vec2& texCoordsOfThisVertex, const Vec3& positionOfPreviousAdjacentVertex, const Vec2& texCoordsOfPreviousAdjacentVertex, const Vec3& positionOfNextAdjacentVertex, const Vec2& texCoordsOfNextAdjacentVertex );
	
	static Renderer& GetInstance();


public:
	std::vector<Matrix4> m_matrixStack;
	Matrix4 m_objectToWorld;
	FrameBuffer* m_frameBuffer;
};

