/*
* collision detection helper functions
* most of these are based on "real-time collision detection"(book)
*
* @author: Kai Yang
*/

#ifndef COLLISION_HELPER_H
#define COLLISION_HELPER_H

#include <Core/CoreHeaders.h>
#include <Physics/PhysicsStructs.h>

//prevent custom class/struct except in PhysicsStructs, so this helper will be more flexible
class CollisionHelper
{
private:
	CollisionHelper(void);
	~CollisionHelper(void);

public:
	static void Barycentric(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& p, Vector3& result);

	static bool TestPointInTriangle(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c);

	static Plane ComputePlane(const Vector3& a, const Vector3& b, const Vector3& c);

	static bool SphereOnSphere(const Sphere& a, const Sphere& b);
	static bool SphereOnCapsule(const Sphere& s, const Capsule& capsule);
	static bool SphereOnPlane(const Sphere& s, const Plane& p);
	static bool SphereInsidePlane(const Sphere& s, const Plane& p);
	static bool SphereOnTriangle(const Sphere& s, const Vector3& a, const Vector3& b, const Vector3& c, Vector3 &outP);

	static bool RayOnSphere(const Vector3& p, const Vector3& d, const Sphere& s, float32 &outT, Vector3 &outP);
	static bool RayOnSphere(const Vector3& p, const Vector3& d, const Sphere& s);

	static bool CapsuleOnCapsule(const Capsule& capsule1, const Capsule& capsule2);

	static bool LineOnTriangle(const Vector3& p, const Vector3& q, const Vector3& a, const Vector3& b, const Vector3& c, Vector3 &outP);
	static bool SegmentOnTriangle(const Vector3& p, const Vector3& q, const Vector3& a, const Vector3& b, const Vector3& c, Vector3 &outP, float32 &outT);
	static bool SegmentOnTriangle(const Vector3& p, const Vector3& q, const Vector3& a, const Vector3& b, const Vector3& c);

	static bool TriangleOnPlane(const Vector3& a, const Vector3& b, const Vector3& c, const Plane& p, Vector3 &outP1, Vector3 &outP2, float32 &outT1, float32 &outT2,uint32 &uSingleSideIndex);
	static bool SegmentOnPlane(const Vector3& a, const Vector3& b, const Plane& p, Vector3 &outP, float32 &outT);

	static bool TriangleOnTriangle(const Vector3& v1a, const Vector3& v1b, const Vector3& v1c,const Vector3& v2a, const Vector3& v2b, const Vector3& v2c);
	static bool TriangleOnTriangle(const Vector3& v1a, const Vector3& v1b, const Vector3& v1c,const Vector3& v2a, const Vector3& v2b, const Vector3& v2c, Vector3 &outP1, Vector3 &outP2);

	static Vector3 ClosestPtPointPlane(const Vector3& q, const Plane& p);

	static float32 DistPointPlane(const Vector3& q, const Plane& p);

	static void ClosestPtPointOnSegment(const Vector3& c, const Vector3& a, const Vector3& b, float32 &outT, Vector3 &outD);

	static float32 SqDistPointSegment(const Vector3& a, const Vector3& b, const Vector3& c);

	static Vector3 ClosestPtPointOnTriangle(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c);

	static bool PointOutsidePlane(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c);

	static int PointSideOfPlane(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c);

	static float32 ClosestPtSegmentSegment(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2, float32 &s, float32 &t, Vector3& c1, Vector3& c2);

};

#endif