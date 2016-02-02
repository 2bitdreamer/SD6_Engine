#pragma once

#include <iostream>
#include <iomanip>
#include "Engine/Math/raw_vector.hpp"
#include "Engine/Math/raw_vector.hpp"
#include "Engine/Math/raw_vector.hpp"

const float DEG2RAD = 3.141593f / 180;
const float EPSILON = 0.00001f;

class Matrix4
{
public:
    Matrix4();
    Matrix4(const float src[16]);

    Matrix4(float m00, float m01, float m02, float m03,
            float m04, float m05, float m06, float m07,
            float m08, float m09, float m10, float m11,
            float m12, float m13, float m14, float m15);

    void        Set(const float src[16]);
    void        Set(float m00, float m01, float m02, float m03,
                    float m04, float m05, float m06, float m07,
                    float m08, float m09, float m10, float m11,
                    float m12, float m13, float m14, float m15);

    void        SetRow(int index, const float row[4]);
    void        SetRow(int index, const Vec4& v);
    void        SetRow(int index, const Vec3& v);

    void        SetColumn(int index, const float col[4]);
    void        SetColumn(int index, const Vec4& v);
    void        SetColumn(int index, const Vec3& v);

    const float* Get() const;

    Matrix4&    Identity();                                       

    Matrix4&    Translate(float x, float y, float z);
    Matrix4&    Translate(const Vec3& v);

    Matrix4&    Rotate(float angle, const Vec3& axis);
    Matrix4&    Rotate(float angle, float x, float y, float z);

    Matrix4&    Scale(float scale);
    Matrix4&    Scale(float sx, float sy, float sz);

    Matrix4     operator+(const Matrix4& rhs) const;
    Matrix4     operator-(const Matrix4& rhs) const;
    Matrix4&    operator+=(const Matrix4& rhs);
    Matrix4&    operator-=(const Matrix4& rhs);
	Vec4        operator*(const Vec4& rhs) const;
    Vec3        operator*(const Vec3& rhs) const;
    Matrix4     operator*(const Matrix4& rhs) const;
    Matrix4&    operator*=(const Matrix4& rhs);
    bool        operator==(const Matrix4& rhs) const;
    bool        operator!=(const Matrix4& rhs) const;
    float       operator[](int index) const;
    float&      operator[](int index);

    friend Matrix4 operator-(const Matrix4& m);
    friend Matrix4 operator*(float scalar, const Matrix4& m);
    friend Vec3 operator*(const Vec3& vec, const Matrix4& m);
    friend Vec4 operator*(const Vec4& vec, const Matrix4& m);
	inline Matrix4 SetOrtho(double l, double r, double b, double t, double n, double f);
	inline Matrix4 BuildPerspective(float fovY, float aspect, float front, float back);
	inline Matrix4 SetFrustum(float l, float r, float b, float t, float n, float f);
	Vec4 GetRow(int row) const;
protected:

private:
	
	float m[16];

};

inline Matrix4::Matrix4()
{
	Identity();
}



inline Matrix4::Matrix4(const float src[16])
{
	Set(src);
}

inline Matrix4::Matrix4(float m00, float m01, float m02, float m03,
	float m04, float m05, float m06, float m07,
	float m08, float m09, float m10, float m11,
	float m12, float m13, float m14, float m15)
{
	Set(m00, m01, m02, m03,  m04, m05, m06, m07,  m08, m09, m10, m11,  m12, m13, m14, m15);
}

inline void Matrix4::Set(const float src[16])
{
	m[0] = src[0];  m[1] = src[1];  m[2] = src[2];  m[3] = src[3];
	m[4] = src[4];  m[5] = src[5];  m[6] = src[6];  m[7] = src[7];
	m[8] = src[8];  m[9] = src[9];  m[10]= src[10]; m[11]= src[11];
	m[12]= src[12]; m[13]= src[13]; m[14]= src[14]; m[15]= src[15];
}

inline void Matrix4::Set(float m00, float m01, float m02, float m03,
	float m04, float m05, float m06, float m07,
	float m08, float m09, float m10, float m11,
	float m12, float m13, float m14, float m15)
{
	m[0] = m00;  m[1] = m01;  m[2] = m02;  m[3] = m03;
	m[4] = m04;  m[5] = m05;  m[6] = m06;  m[7] = m07;
	m[8] = m08;  m[9] = m09;  m[10]= m10;  m[11]= m11;
	m[12]= m12;  m[13]= m13;  m[14]= m14;  m[15]= m15;
}

inline void Matrix4::SetRow(int index, const float row[4])
{
	m[index] = row[0]; 
	m[index + 4] = row[1]; 
	m[index + 8] = row[2]; 
	m[index + 12] = row[3];
}

inline void Matrix4::SetRow(int index, const Vec4& v)
{
	m[index] = v.x();  
	m[index + 4] = v.y();
	m[index + 8] = v.z();
	m[index + 12] = v.w();
}

inline void Matrix4::SetRow(int index, const Vec3& v)
{
	m[index] = v.x();  m[index + 4] = v.y();  m[index + 8] = v.z();
}

inline void Matrix4::SetColumn(int index, const float col[4])
{
	m[index*4] = col[0]; 
	m[index*4 + 1] = col[1]; 
	m[index*4 + 2] = col[2];
	m[index*4 + 3] = col[3];
}

inline void Matrix4::SetColumn(int index, const Vec4& v)
{
	m[index*4] = v.x();  
	m[index*4 + 1] = v.y(); 
	m[index*4 + 2] = v.z(); 
	m[index*4 + 3] = v.w();
}

inline void Matrix4::SetColumn(int index, const Vec3& v)
{
	m[index*4] = v.x(); 
	m[index*4 + 1] = v.y();
	m[index*4 + 2] = v.z();
}

inline const float* Matrix4::Get() const
{
	return m;
}

inline Matrix4& Matrix4::Identity()
{
	m[0] = m[5] = m[10] = m[15] = 1.0f;
	m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
	return *this;
}

inline Matrix4 Matrix4::operator+(const Matrix4& rhs) const
{
	return Matrix4(m[0]+rhs[0],   m[1]+rhs[1],   m[2]+rhs[2],   m[3]+rhs[3],
		m[4]+rhs[4],   m[5]+rhs[5],   m[6]+rhs[6],   m[7]+rhs[7],
		m[8]+rhs[8],   m[9]+rhs[9],   m[10]+rhs[10], m[11]+rhs[11],
		m[12]+rhs[12], m[13]+rhs[13], m[14]+rhs[14], m[15]+rhs[15]);
}

inline Matrix4 Matrix4::operator-(const Matrix4& rhs) const
{
	return Matrix4(m[0]-rhs[0],   m[1]-rhs[1],   m[2]-rhs[2],   m[3]-rhs[3],
		m[4]-rhs[4],   m[5]-rhs[5],   m[6]-rhs[6],   m[7]-rhs[7],
		m[8]-rhs[8],   m[9]-rhs[9],   m[10]-rhs[10], m[11]-rhs[11],
		m[12]-rhs[12], m[13]-rhs[13], m[14]-rhs[14], m[15]-rhs[15]);
}

inline Matrix4& Matrix4::operator+=(const Matrix4& rhs)
{
	m[0] += rhs[0];   m[1] += rhs[1];   m[2] += rhs[2];   m[3] += rhs[3];
	m[4] += rhs[4];   m[5] += rhs[5];   m[6] += rhs[6];   m[7] += rhs[7];
	m[8] += rhs[8];   m[9] += rhs[9];   m[10]+= rhs[10];  m[11]+= rhs[11];
	m[12]+= rhs[12];  m[13]+= rhs[13];  m[14]+= rhs[14];  m[15]+= rhs[15];
	return *this;
}

inline Matrix4& Matrix4::operator-=(const Matrix4& rhs)
{
	m[0] -= rhs[0];   m[1] -= rhs[1];   m[2] -= rhs[2];   m[3] -= rhs[3];
	m[4] -= rhs[4];   m[5] -= rhs[5];   m[6] -= rhs[6];   m[7] -= rhs[7];
	m[8] -= rhs[8];   m[9] -= rhs[9];   m[10]-= rhs[10];  m[11]-= rhs[11];
	m[12]-= rhs[12];  m[13]-= rhs[13];  m[14]-= rhs[14];  m[15]-= rhs[15];
	return *this;
}

inline Vec4 Matrix4::operator*(const Vec4& rhs) const
{
	return Vec4(m[0] * rhs.x() + m[4] * rhs.y() + m[8] * rhs.z() + m[12] * rhs.w(),
		m[1] * rhs.x() + m[5] * rhs.y() + m[9] * rhs.z() + m[13] * rhs.w(),
		m[2] * rhs.x() + m[6] * rhs.y() + m[10] * rhs.z() + m[14] * rhs.w(),
		m[3] * rhs.x() + m[7] * rhs.y() + m[11] * rhs.z() + m[15] * rhs.w());
}

inline Vec3 Matrix4::operator*(const Vec3& rhs) const
{
	return Vec3(m[0] * rhs.x() + m[4] * rhs.y() + m[8] * rhs.z(),
		m[1] * rhs.x() + m[5] * rhs.y() + m[9] * rhs.z(),
		m[2] * rhs.x() + m[6] * rhs.y() + m[10] * rhs.z());
}

inline Matrix4 Matrix4::operator*(const Matrix4& n) const
{
	return Matrix4(m[0]*n[0]  + m[4]*n[1]  + m[8]*n[2]  + m[12]*n[3],   m[1]*n[0]  + m[5]*n[1]  + m[9]*n[2]  + m[13]*n[3],   m[2]*n[0]  + m[6]*n[1]  + m[10]*n[2]  + m[14]*n[3],   m[3]*n[0]  + m[7]*n[1]  + m[11]*n[2]  + m[15]*n[3],
		m[0]*n[4]  + m[4]*n[5]  + m[8]*n[6]  + m[12]*n[7],   m[1]*n[4]  + m[5]*n[5]  + m[9]*n[6]  + m[13]*n[7],   m[2]*n[4]  + m[6]*n[5]  + m[10]*n[6]  + m[14]*n[7],   m[3]*n[4]  + m[7]*n[5]  + m[11]*n[6]  + m[15]*n[7],
		m[0]*n[8]  + m[4]*n[9]  + m[8]*n[10] + m[12]*n[11],  m[1]*n[8]  + m[5]*n[9]  + m[9]*n[10] + m[13]*n[11],  m[2]*n[8]  + m[6]*n[9]  + m[10]*n[10] + m[14]*n[11],  m[3]*n[8]  + m[7]*n[9]  + m[11]*n[10] + m[15]*n[11],
		m[0]*n[12] + m[4]*n[13] + m[8]*n[14] + m[12]*n[15],  m[1]*n[12] + m[5]*n[13] + m[9]*n[14] + m[13]*n[15],  m[2]*n[12] + m[6]*n[13] + m[10]*n[14] + m[14]*n[15],  m[3]*n[12] + m[7]*n[13] + m[11]*n[14] + m[15]*n[15]);
}

inline Matrix4& Matrix4::operator*=(const Matrix4& rhs)
{
	*this = *this * rhs;
	return *this;
}

inline bool Matrix4::operator==(const Matrix4& n) const
{
	return (m[0] == n[0])  && (m[1] == n[1])  && (m[2] == n[2])  && (m[3] == n[3])  &&
		(m[4] == n[4])  && (m[5] == n[5])  && (m[6] == n[6])  && (m[7] == n[7])  &&
		(m[8] == n[8])  && (m[9] == n[9])  && (m[10]== n[10]) && (m[11]== n[11]) &&
		(m[12]== n[12]) && (m[13]== n[13]) && (m[14]== n[14]) && (m[15]== n[15]);
}

inline bool Matrix4::operator!=(const Matrix4& n) const
{
	return (m[0] != n[0])  || (m[1] != n[1])  || (m[2] != n[2])  || (m[3] != n[3])  ||
		(m[4] != n[4])  || (m[5] != n[5])  || (m[6] != n[6])  || (m[7] != n[7])  ||
		(m[8] != n[8])  || (m[9] != n[9])  || (m[10]!= n[10]) || (m[11]!= n[11]) ||
		(m[12]!= n[12]) || (m[13]!= n[13]) || (m[14]!= n[14]) || (m[15]!= n[15]);
}

inline float Matrix4::operator[](int index) const
{
	return m[index];
}

inline float& Matrix4::operator[](int index)
{
	return m[index];
}

inline Matrix4 operator-(const Matrix4& rhs)
{
	return Matrix4(-rhs[0], -rhs[1], -rhs[2], -rhs[3], -rhs[4], -rhs[5], -rhs[6], -rhs[7], -rhs[8], -rhs[9], -rhs[10], -rhs[11], -rhs[12], -rhs[13], -rhs[14], -rhs[15]);
}

inline Matrix4 operator*(float s, const Matrix4& rhs)
{
	return Matrix4(s*rhs[0], s*rhs[1], s*rhs[2], s*rhs[3], s*rhs[4], s*rhs[5], s*rhs[6], s*rhs[7], s*rhs[8], s*rhs[9], s*rhs[10], s*rhs[11], s*rhs[12], s*rhs[13], s*rhs[14], s*rhs[15]);
}

inline Vec4 operator*(const Vec4& v, const Matrix4& m)
{
	return Vec4(v.x()*m[0] + v.y()*m[1] + v.z()*m[2] + v.w()*m[3], v.x()*m[4] + v.y()*m[5] + v.z()*m[6] + v.w()*m[7], v.x()*m[8] + v.y()*m[9] + v.z()*m[10] + v.w()*m[11], v.x()*m[12] + v.y()*m[13] + v.z()*m[14] + v.w()*m[15]);
}

inline Vec3 operator*(const Vec3& v, const Matrix4& m)
{
	return Vec3(v.x()*m[0] + v.y()*m[1] + v.z()*m[2], v.x()*m[4] + v.y()*m[5] + v.z()*m[6], v.x()*m[8] + v.y()*m[9] + v.z()*m[10]);
}

inline Matrix4 Matrix4::SetFrustum(float l, float r, float b, float t, float n, float f)
{
	Matrix4 mat;
	mat[0]  = 2 * n / (r - l);
	mat[5]  = 2 * n / (t - b);
	mat[8]  = (r + l) / (r - l);
	mat[9]  = (t + b) / (t - b);
	mat[10] = -(f + n) / (f - n);
	mat[11] = -1;
	mat[14] = -(2 * f * n) / (f - n);
	mat[15] = 0;
	return mat;
}

// gluPerspective()
inline Matrix4 Matrix4::BuildPerspective(float fovY, float aspect, float front, float back)
{
	float tangent = tanf(fovY/2 * DEG2RAD);
	float height = front * tangent;
	float width = height * aspect;

	return SetFrustum(-width, width, -height, height, front, back);
}

// glOrtho()
inline Matrix4 Matrix4::SetOrtho(double l, double r, double b, double t, double n, double f)
{
	Matrix4 mat;
	mat[0]  = (float)(2 / (r - l));
	mat[5]  = (float)(2 / (t - b));
	mat[10] = (float)(-2 / (f - n));
	mat[12] = (float)(-(r + l) / (r - l));
	mat[13] = (float)(-(t + b) / (t - b));
	mat[14] = (float)(-(f + n) / (f - n));
	return mat;
}
