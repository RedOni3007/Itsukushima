#include "MathHelper.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define CLEAN_THRESHOLD 0.00001f
#define ALMOST_EQUAL_MAX_DIF 0.0001f


MathHelper::MathHelper(void)
{
}


MathHelper::~MathHelper(void)
{
}

void 
MathHelper::ApplyMatrix_w0(Matrix44 &mat, Vector3 &vec, Vector3 &out)
{
	//column  major
	out.x = vec.x * mat[0][0] + vec.y * mat[1][0] + vec.z * mat[2][0];
	out.y = vec.x * mat[0][1] + vec.y * mat[1][1] + vec.z * mat[2][1];
	out.z = vec.x * mat[0][2] + vec.y * mat[1][2] + vec.z * mat[2][2];

}


void 
MathHelper::ApplyMatrix_w1(Matrix44 &mat, Vector3 &vec, Vector3 &out)
{
	//column  major
	out.x =  vec.x * mat[0][0] + vec.y * mat[1][0] + vec.z * mat[2][0] + mat[0][3];
	out.y =  vec.x * mat[0][1] + vec.y * mat[1][1] + vec.z * mat[2][1] + mat[1][3];
	out.z =  vec.x * mat[0][2] + vec.y * mat[1][2] + vec.z * mat[2][2] + mat[2][3];
}


//the most important function in my engine
//I really don't want to admit it
void 
MathHelper::Clean(Vector3 &v3)
{
	float32 decimal_x = v3.x - (int32)v3.x;
	float32 decimal_y = v3.y - (int32)v3.y;
	float32 decimal_z = v3.z - (int32)v3.z;

	//clean anything tiny
	if(decimal_x == 0)
	{
		//do nothing
	}
	else if(decimal_x > -CLEAN_THRESHOLD && decimal_x <  CLEAN_THRESHOLD)
	{
		decimal_x = 0;
		v3.x =  (int32)v3.x;
	}
	else
	{
		if(decimal_x > 0)
		{
			decimal_x = 1.0f - decimal_x;
			if(decimal_x <  CLEAN_THRESHOLD)
				v3.x =  (int32)v3.x + 1;

		}
		else
		{
			decimal_x = -1.0f - decimal_x;
			if(decimal_x > -CLEAN_THRESHOLD)
				v3.x =  (int32)v3.x - 1;
		}
	}

	
	if(decimal_y == 0)
	{
		//do nothing
	}
	else if(decimal_y > -CLEAN_THRESHOLD && decimal_y <  CLEAN_THRESHOLD)
	{
		decimal_y = 0;
		v3.y =  (int32)v3.y;
	}
	else
	{
		if(decimal_y > 0)
		{
			decimal_y = 1.0f - decimal_y;
			if(decimal_y <  CLEAN_THRESHOLD)
				v3.y =  (int32)v3.y + 1;

		}
		else
		{
			decimal_y = -1.0f - decimal_y;
			if(decimal_y > -CLEAN_THRESHOLD)
				v3.y =  (int32)v3.y - 1;
		}
	}

	if(decimal_z == 0)
	{
		//do nothing
	}
	else if(decimal_z > -CLEAN_THRESHOLD && decimal_z <  CLEAN_THRESHOLD)
	{
		decimal_z = 0;
		v3.z =  (int32)v3.z;
	}
	else
	{
		if(decimal_z > 0)
		{
			decimal_z = 1.0f - decimal_z;
			if(decimal_z <  CLEAN_THRESHOLD)
				v3.z =  (int32)v3.z + 1;

		}
		else
		{
			decimal_z = -1.0f - decimal_z;
			if(decimal_z > -CLEAN_THRESHOLD)
				v3.z =  (int32)v3.z - 1;
		}
	}
}

void 
MathHelper::Clean(Quaternion &qQ)
{
	float32 decimal_x = qQ.x - (int32)qQ.x;
	float32 decimal_y = qQ.y - (int32)qQ.y;
	float32 decimal_z = qQ.z - (int32)qQ.z;
	float32 decimal_w = qQ.w - (int32)qQ.w;

	//clean anything tiny
	if(decimal_x == 0)
	{
		//do nothing
	}
	else if(decimal_x > -CLEAN_THRESHOLD && decimal_x <  CLEAN_THRESHOLD)
	{
		decimal_x = 0;
		qQ.x =  (int32)qQ.x;
	}
	else
	{
		if(decimal_x > 0)
		{
			decimal_x = 1.0f - decimal_x;
			if(decimal_x <  CLEAN_THRESHOLD)
				qQ.x =  (int32)qQ.x + 1;

		}
		else
		{
			decimal_x = -1.0f - decimal_x;
			if(decimal_x > -CLEAN_THRESHOLD)
				qQ.x =  (int32)qQ.x - 1;
		}
	}

	
	if(decimal_y == 0)
	{
		//do nothing
	}
	else if(decimal_y > -CLEAN_THRESHOLD && decimal_y <  CLEAN_THRESHOLD)
	{
		decimal_y = 0;
		qQ.y =  (int32)qQ.y;
	}
	else
	{
		if(decimal_y > 0)
		{
			decimal_y = 1.0f - decimal_y;
			if(decimal_y <  CLEAN_THRESHOLD)
				qQ.y =  (int32)qQ.y + 1;

		}
		else
		{
			decimal_y = -1.0f - decimal_y;
			if(decimal_y > -CLEAN_THRESHOLD)
				qQ.y =  (int32)qQ.y - 1;
		}
	}

	if(decimal_z == 0)
	{
		//do nothing
	}
	else if(decimal_z > -CLEAN_THRESHOLD && decimal_z <  CLEAN_THRESHOLD)
	{
		decimal_z = 0;
		qQ.z =  (int32)qQ.z;
	}
	else
	{
		if(decimal_z > 0)
		{
			decimal_z = 1.0f - decimal_z;
			if(decimal_z <  CLEAN_THRESHOLD)
				qQ.z =  (int32)qQ.z + 1;

		}
		else
		{
			decimal_z = -1.0f - decimal_z;
			if(decimal_z > -CLEAN_THRESHOLD)
				qQ.z =  (int32)qQ.z - 1;
		}
	}

	if(decimal_w == 0)
	{
		//do nothing
	}
	else if(decimal_w > -CLEAN_THRESHOLD && decimal_w <  CLEAN_THRESHOLD)
	{
		decimal_w = 0;
		qQ.w =  (int32)qQ.w;
	}
	else
	{
		if(decimal_w > 0)
		{
			decimal_w = 1.0f - decimal_w;
			if(decimal_w <  CLEAN_THRESHOLD)
				qQ.w =  (int32)qQ.w + 1;

		}
		else
		{
			decimal_w = -1.0f - decimal_w;
			if(decimal_w > -CLEAN_THRESHOLD)
				qQ.w =  (int32)qQ.w - 1;
		}
	}
}

float32 
MathHelper::Clean(float32 fValue)
{
	float32 decimal = fValue - (int32)fValue;

	//clean anything tiny
	if(decimal == 0)
	{
		//do nothing
		return fValue;
	}
	else if(decimal > -CLEAN_THRESHOLD && decimal <  CLEAN_THRESHOLD)
	{
		return (int32)fValue;
	}
	else
	{
		if(decimal > 0)
		{
			decimal = 1.0f - decimal;
			if(decimal <  CLEAN_THRESHOLD)
				return (int32)fValue + 1;

		}
		else
		{
			decimal = -1.0f - decimal;
			if(decimal > -CLEAN_THRESHOLD)
				return (int32)fValue - 1;
		}
	}

	return fValue;
}

void 
MathHelper::RemoveDuplicate(std::vector<Vector3>& list, uint32 &uCount)
{
	Vector3 *pV1 = nullptr;
	Vector3 *pV2 = nullptr;
	uint32 i = 0;
	uint32 j = 0;
	size_t uTypeSize = sizeof(Vector3);
	for(i = 0; i < uCount -1;++i)
	{
		pV1 = &list[i];
		for(j = i + 1; j < uCount; ++j)
		{
			pV2 = &list[j];

			if(pV1->x == pV2->x 
				&& pV1->y == pV2->y
				&& pV1->z == pV2->z)
			{
				if(j < uCount -1)
				{
					memcpy(pV2,&list[j + 1], uTypeSize * (uCount - j));
				}
				--j;
				--uCount;
			}
		}
	}
}

void 
MathHelper::RemoveDuplicate(std::vector<Vector3*>& list, uint32 &uCount)
{
	Vector3 *pV1 = nullptr;
	Vector3 *pV2 = nullptr;
	uint32 i = 0;
	uint32 j = 0;
	size_t uTypeSize = sizeof(Vector3*);
	for(i = 0; i < uCount -1;++i)
	{
		pV1 = list[i];
		for(j = i + 1; j < uCount; ++j)
		{
			pV2 = list[j];

			if(pV1->x == pV2->x 
				&& pV1->y == pV2->y
				&& pV1->z == pV2->z)
			{
				if(j < uCount -1)
				{
					memcpy(&list[j],&list[j + 1], uTypeSize * (uCount - j));
				}
				--j;
				--uCount;
			}
		}
	}
}

void 
MathHelper::RemoveDuplicate(std::vector<uint32>& list, uint32 &uCount)
{
	uint32 *pV1 = nullptr;
	uint32 *pV2 = nullptr;
	uint32 i = 0;
	uint32 j = 0;
	size_t uTypeSize = sizeof(uint32);
	for(i = 0; i < uCount -1;++i)
	{
		pV1 = &list[i];
		for(j = i + 1; j < uCount; ++j)
		{
			pV2 = &list[j];

			if((*pV1) == (*pV2))
			{
				if(j < uCount -1)
				{
					memcpy(pV2,&list[j + 1], uTypeSize * (uCount - j));
				}
				--j;
				--uCount;
			}
		}
	}
}

bool 
MathHelper::IsZeroVector(Vector2 &v)
{
	return v.x == 0 && v.y == 0;
}

bool 
MathHelper::IsZeroVector(Vector3 &v)
{
	return v.x == 0 && v.y == 0 && v.z == 0;
}

bool 
MathHelper::IsZeroVector(Vector4 &v)
{
	return v.x == 0 && v.y == 0 && v.z == 0 && v.w == 0;
}

//convert to int32? no, cos +0.0 and -0.0, and I am lazy
bool 
MathHelper::AlmostEqual(float32 v1, float32 v2)
{
	return fabsf(v1-v2) < ALMOST_EQUAL_MAX_DIF;
	
}

bool MathHelper::AlmostEqual(Vector2& v1, Vector2& v2)
{
	return fabsf(v1.x-v2.x) < ALMOST_EQUAL_MAX_DIF
			&& fabsf(v1.y-v2.y) < ALMOST_EQUAL_MAX_DIF;

}

bool MathHelper::AlmostEqual(Vector3& v1, Vector3& v2)
{
	return fabsf(v1.x-v2.x) < ALMOST_EQUAL_MAX_DIF
		&& fabsf(v1.y-v2.y) < ALMOST_EQUAL_MAX_DIF
		&& fabsf(v1.z-v2.z) < ALMOST_EQUAL_MAX_DIF;
}

bool MathHelper::AlmostEqual(Vector4& v1, Vector4& v2)
{
	return fabsf(v1.x-v2.x) < ALMOST_EQUAL_MAX_DIF
		&& fabsf(v1.y-v2.y) < ALMOST_EQUAL_MAX_DIF
		&& fabsf(v1.z-v2.z) < ALMOST_EQUAL_MAX_DIF
		&& fabsf(v1.w-v2.w) < ALMOST_EQUAL_MAX_DIF;
}

float32 MathHelper::RandF(float32 fMin, float32 fMax)
{
	return fMin + rand() % 100000 * 0.00001f * (fMax - fMin);
}

int32 MathHelper::RandI(int32 nMin, int32 nMax )
{
	return nMin + rand()%(nMax - nMin);
}

bool  
MathHelper::IsSameSign(int32 v1, int32 v2)
{
	return (v1 < 0 && v2 < 0) || (v1 > 0 && v2 > 0) || (v1 == v2);
}
	
bool  
MathHelper::IsSameSign(float32 v1, float32 v2)
{
	return (v1 < 0 && v2 < 0) || (v1 > 0 && v2 > 0) || (v1 == v2);
}

Quaternion 
MathHelper::RotateTo(Vector3& vOrigin, Vector3& vDest)
{
	vOrigin = MFD_Normalize(vOrigin);
	vDest =  MFD_Normalize(vDest);

	float32 fCos = MFD_Dot(vOrigin, vDest);
	Vector3 vAxis;

	if (fCos < -1 + 0.001f)
	{
		// in opposite directions
		vAxis = MFD_Cross(Vector3(0.0f, 0.0f, 1.0f), vOrigin);

		if( MFD_Length2(vAxis) < 0.01f ) //  parallel again
			vAxis = MFD_Cross(Vector3(1.0f, 0.0f, 0.0f), vOrigin);
 
		vAxis = MFD_Normalize(vAxis);
		return glm::angleAxis(180.0f, vAxis);
	}
 
	vAxis = MFD_Cross(vOrigin, vDest);
 
    float32 s = sqrt( (1+fCos)*2 );
    float32 invs = 1 / s;

	return Quaternion(s * 0.5f, vAxis.x * invs, vAxis.y * invs, vAxis.z * invs);
}
