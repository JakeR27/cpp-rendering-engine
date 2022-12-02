#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

/** Mat44f: 4x4 matrix with floats
 *
 * See vec2f.hpp for discussion. Similar to the implementation, the Mat44f is
 * intentionally kept simple and somewhat bare bones.
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator () allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m(1,2);
 *    m(0,3) = 3.f;
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

constexpr
Mat44f operator*( Mat44f const& aLeft, Mat44f const& aRight ) noexcept
{
	// This approach is SLOW. Should really write it out fully as 16 lines. Will do for now.
	Mat44f result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			float num = 0.f;
			for (int k = 0; k < 4; k++) {
				num += aLeft(i, k) * aRight(k, j);
			}
			result(i, j) = float(num);
		}
	}
	return result;
}

constexpr
Vec4f operator*( Mat44f const& aLeft, Vec4f const& aRight ) noexcept
{
	Vec4f result = {};
	result.x = (aLeft(0, 0) * aRight.x) + (aLeft(0, 1) * aRight.y) + (aLeft(0, 2) * aRight.z) + (aLeft(0, 3) * aRight.w);
	result.y = (aLeft(1, 0) * aRight.x) + (aLeft(1, 1) * aRight.y) + (aLeft(1, 2) * aRight.z) + (aLeft(1, 3) * aRight.w);
	result.z = (aLeft(2, 0) * aRight.x) + (aLeft(2, 1) * aRight.y) + (aLeft(2, 2) * aRight.z) + (aLeft(2, 3) * aRight.w);
	result.w = (aLeft(3, 0) * aRight.x) + (aLeft(3, 1) * aRight.y) + (aLeft(3, 2) * aRight.z) + (aLeft(3, 3) * aRight.w);
	return result;
}

inline
Mat44f make_rotation_x( float aAngle ) noexcept
{
	Mat44f result = {};
	result(0, 0) = 1;
	result(1, 1) = cos(aAngle);
	result(1, 2) = -sin(aAngle);
	result(2, 1) = sin(aAngle);
	result(2, 2) = cos(aAngle);
	result(3, 3) = 1;
	return result;
}


inline
Mat44f make_rotation_y( float aAngle ) noexcept
{
	Mat44f result = {};
	result(0, 0) = cos(aAngle);
	result(0, 2) = sin(aAngle);
	result(1, 1) = 1;
	result(2, 0) = -sin(aAngle);
	result(2, 2) = cos(aAngle);
	result(3, 3) = 1;
	return result;
}

inline
Mat44f make_rotation_z( float aAngle ) noexcept
{
	Mat44f result = {};
	result(0, 0) = cos(aAngle);
	result(0, 1) = -sin(aAngle);
	result(1, 0) = sin(aAngle);
	result(1, 1) = cos(aAngle);
	result(2, 2) = 1;
	result(3, 3) = 1;
	return result;
}

inline
Mat44f make_translation( Vec3f aTranslation ) noexcept
{
	Mat44f output = kIdentity44f;
	output(0, 3) = aTranslation.x;
	output(1, 3) = aTranslation.y;
	output(2, 3) = aTranslation.z;
	return output;
}

inline
Mat44f make_scaling( float aSX, float aSY, float aSZ ) noexcept
{
	//TODO: your implementation goes here
	//TODO: remove the following when you start your implementation
	(void)aSX;  // Avoid warnings about unused arguments until the function
	(void)aSY;  // is properly implemented.
	(void)aSZ;
	return kIdentity44f;
}


inline
Mat44f make_perspective_projection( float aFovInRadians, float aAspect, float aNear, float aFar ) noexcept
{
	Mat44f result = {};
	float s = 1 / (tan(aFovInRadians/2));
	result(0, 0) = s / aAspect;
	result(1, 1) = s;
	result(2, 2) = -((aFar + aNear) / (aFar - aNear));
	result(2, 3) = (-2 * aFar * aNear) / (aFar - aNear);
	result(3, 2) = -1;
	result(3, 3) = 1;
	return result;
}



#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
