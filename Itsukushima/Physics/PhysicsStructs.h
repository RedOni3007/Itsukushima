/**
* physics struct defines
* should only have very basic geometric properties
*
* @author: Kai Yang
**/

#ifndef PHYSICS_STRUCTS_H
#define PHYSICS_STRUCTS_H

#include <Core/CoreHeaders.h>

struct Plane 
{
    Vector3 n;  // normalized normal
    float32 d;  // d = dot(n,p) for a given point p on the plane
};

struct Sphere 
{
    Vector3 c; //center
    float	r; //radius
};

struct Capsule//not in use, but might need in the future
{
    Vector3 a;    // Medial line segment start point
    Vector3 b;    // Medial line segment end point
    float32 r;    // Radius
};


#endif