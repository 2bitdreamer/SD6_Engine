
#include <cmath>
#include <algorithm>
#include "Matrices.h"
#include "raw_vector.hpp"
#include <string>
#include <iostream>
#include <iomanip>

Matrix4& Matrix4::Translate(const Vec3& v)
{
	return Translate(v.x(), v.y(), v.z());
}

Matrix4& Matrix4::Translate(float x, float y, float z)
{
	float m0 = m[0],  m4 = m[4],  m8 = m[8],  m12 = m[12],
		m1 = m[1],  m5 = m[5],  m9 = m[9],  m13 = m[13],
		m2 = m[2],  m6 = m[6],  m10 = m[10], m14 = m[14], 
		m3 = m[3], m7 = m[7], m11 = m[11], m15 = m[15];
	
	m[12] = (m0 * x) + (m4 * y) + (m8 * z) + m12;
	m[13] = (m1 * x) + (m5 * y) + (m9 * z) + m13;
	m[14] = (m2 * x) + (m6 * y) + (m10 * z) + m14;
	m[15] = (m3 * x) + (m7 * y) + (m11 * z) + m15;

	return *this;
}

Matrix4& Matrix4::Scale(float s)
{
	return Scale(s, s, s);
}

Matrix4& Matrix4::Scale(float x, float y, float z)
{
	m[0] *= x;
	m[1] *= x;
	m[2] *= x;
	m[3] *= x;

	m[4] *= y;
	m[5] *= y;
	m[6] *= y;
	m[7] *= y;

	m[8] *= z;
	m[9] *= z;
	m[10] *= z;
	m[11] *= z;

	return *this;
}

Vec4 Matrix4::GetRow(int row) const
{
	Vec4 resultVec;
	int matrixIndex = row * 4;
	for (int vectorIndex = 0; vectorIndex < 4; vectorIndex++) {
		resultVec[vectorIndex] = m[matrixIndex];
		matrixIndex++;
	}
	return resultVec;
}

Matrix4& Matrix4::Rotate(float angle, const Vec3& axis)
{
	return Rotate(angle, axis.x(), axis.y(), axis.z());
}

Matrix4& Matrix4::Rotate(float angle, float x, float y, float z)
{
	float c = cosf(angle * DEG2RAD);
	float s = sinf(angle * DEG2RAD);

	float c1 = 1.0f - c;
	float m0 = m[0],  m4 = m[4],  m8 = m[8],  m12 = m[12],
		m1 = m[1],  m5 = m[5],  m9 = m[9],  m13 = m[13],
		m2 = m[2],  m6 = m[6],  m10 = m[10], m14 = m[14], 
		m3 = m[3], m7 = m[7], m11 = m[11], m15 = m[15];

	float r0 = x * x * c1 + c;
	float r1 = x * y * c1 + z * s;
	float r2 = x * z * c1 - y * s;
	float r3 = 0.f;
	float r4 = x * y * c1 - z * s;
	float r5 = y * y * c1 + c;
	float r6 = y * z * c1 + x * s;
	float r7 = 0.f;
	float r8 = x * z * c1 + y * s;
	float r9 = y * z * c1 - x * s;
	float r10= z * z * c1 + c;
	float r11 = 0.f;
	float r12 = 0.f;
	float r13 = 0.f;
	float r14 = 0.f;
	float r15 = 1.f;

	m[0] = (m0 * r0) + (m4 * r1) + (m8 * r2) + (m12 * r3);
	m[1] = (m1 * r0) + (m5 * r1) + (m9 * r2) + (m13 * r3);
	m[2] = (m2 * r0) + (m6 * r1) + (m10 * r2) + (m14 * r3);
	m[3] = (m3 * r0) + (m7 * r1) + (m11 * r2) + (m15 * r3);
	m[4] = (m0 * r4) + (m4 * r5) + (m8 * r6) + (m12 * r7);
	m[5] = (m1 * r4) + (m5 * r5) + (m9 * r6) + (m13 * r7);
	m[6] = (m2 * r4) + (m6 * r5) + (m10 * r6) + (m14 * r7);
	m[7] = (m3 * r4) + (m7 * r5) + (m11 * r6) + (m15 * r7);
	m[8] = (m0 * r8) + (m4 * r9) + (m8 * r10) + (m12 * r11);
	m[9] = (m1 * r8) + (m5 * r9) + (m9 * r10) + (m13 * r11);
	m[10] = (m2 * r8) + (m6* r9) + (m10 * r10) + (m14 * r11);
	m[11] = (m3 * r8) + (m7 * r9) + (m11 * r10) + (m15 * r11);
	m[12] = (m0 * r12) + (m4 * r13) + (m8 * r14) + (m12 * r15);
	m[13] = (m1 * r12) + (m5 * r13) + (m9 * r14) + (m13 * r15);
	m[14] = (m2 * r12) + (m6 * r13) + (m10 * r14) + (m14 * r15);
	m[15] = (m3 * r12) + (m7 * r13) + (m11 * r14) + (m15 * r15);

 	return *this;
}
