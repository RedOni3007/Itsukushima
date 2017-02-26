/**
* My type define
* todo:think a engine name
*
* @author: Kai Yang
**/

#ifndef TYPE_DEFINE_H
#define TYPE_DEFINE_H

#include "PlatformDetect.h"

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#if X_PC
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef float float32;
typedef double float64;

//for graphic
typedef uint16 VertexIndex;

//for math
typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;
typedef glm::vec4 Vector4;
typedef glm::mat3 Matrix33;
typedef glm::mat4 Matrix44;
typedef glm::quat Quaternion;

//math function defines, so it's easier to change math class
//will I change the math class? and it's easier?
#define MFD_Dot glm::dot
#define MFD_Cross glm::cross
#define MFD_Clamp glm::clamp
#define MFD_Distance glm::distance
#define MFD_Max glm::max
#define MFD_Min glm::min
#define MFD_Inverse	glm::inverse
#define MFD_Length glm::length
#define MFD_Length2 glm::length2

//this one will crash, if it's a zero vector
#define MFD_Normalize glm::normalize

#endif

#endif