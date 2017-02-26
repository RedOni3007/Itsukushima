/*
*  Physics Contants class
*  1.0f in World Position = 1 meter
*  1.0f in Velocity = 1 meter per second
*  1.0f in Mass = 1 kg
*  1.0f in Force = 1 N
*  todo:make a unit class for easy changing, and make them really work
*
*  @author: Kai Yang
*/

#ifndef PHYSICS_CONSTANTS_H
#define PHYSICS_CONSTANTS_H

#include <Core/CoreHeaders.h>

class PhysicsContants
{
private:
	PhysicsContants();
	~PhysicsContants();

public:
	const static Vector3 Gravity;
};


#endif