/**
* math helper functions
*
* @author: Kai Yang
**/

#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#include <Core/CoreHeaders.h>
#include <vector>
#include <list>

class MathHelper
{
private:
	MathHelper(void);
	~MathHelper(void);

public:
	static void ApplyMatrix_w0(Matrix44 &mat, Vector3 &vec, Vector3 &out);
	static void ApplyMatrix_w1(Matrix44 &mat, Vector3 &vec, Vector3 &out);

	static void Clean(Vector3 &v3);
	static void Clean(Quaternion &qQ);

	static float32 Clean(float32 fValue);

	static void RemoveDuplicate(std::vector<Vector3>& list, uint32 &uCount);
	static void RemoveDuplicate(std::vector<Vector3*>& list, uint32 &uCount);
	static void RemoveDuplicate(std::vector<uint32>& list, uint32 &uCount);

	static bool IsZeroVector(Vector2 &v);
	static bool IsZeroVector(Vector3 &v);
	static bool IsZeroVector(Vector4 &v);

	//oh float float, wherefore art thou float, might need a verion for max diffence setting
	static bool AlmostEqual(float32 v1, float32 v2);
	static bool AlmostEqual(Vector2& v1, Vector2& v2);
	static bool AlmostEqual(Vector3& v1, Vector3& v2);
	static bool AlmostEqual(Vector4& v1, Vector4& v2);

	//todo:random pool
	static float32 RandF(float32 fMin, float32 fMax);
	static int32 RandI(int32 nMin, int32 nMax );

	static bool IsSameSign(int32 v1, int32 v2);
	static bool IsSameSign(float32 v1, float32 v2);

	static Quaternion RotateTo(Vector3& vOrigin, Vector3& vDest);
};

#endif