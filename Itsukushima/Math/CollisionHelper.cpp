/****************************************
Most functions are based on "Real Time Collision Detection"
****************************************/

#include <Math/CollisionHelper.h>

#include <glm/ext.hpp>

/****************************************
*           Gobal		                *
****************************************/
template<class T> bool SameSign(T t1, T t2)
 {
	 return (t1 < 0) == (t2 < 0);
 }

//#define EPSILON glm::epsilon<float32>()
float32 EPSILON = glm::epsilon<float32>();

float32 ScalarTriple(const Vector3& a, const Vector3& b, const Vector3& c)
{
	return MFD_Dot(a, MFD_Cross(b,c));
}


float32 TriArea2D(float32 x1, float32 y1, float32 x2, float32 y2, float32 x3, float32 y3)
{
    return (x1-x2)*(y2-y3) - (x2-x3)*(y1-y2);
}

float32 Signed2DTriArea(const Vector3& a, const Vector3& b, const Vector3& c)
{
    return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}

bool Test2DSegmentSegment(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, float &t, Vector3 &p)
{
    // Sign of areas correspond to which side of ab points c and d are
    float a1 = Signed2DTriArea(a, b, d); // Compute winding of abd (+ or -)
    float a2 = Signed2DTriArea(a, b, c); // To intersect, must have sign opposite of a1

    // If c and d are on different sides of ab, areas have different signs
    if (a1 * a2 < 0.0f) 
	{
        // Compute signs for a and b with respect to segment cd
        float a3 = Signed2DTriArea(c, d, a); // Compute winding of cda (+ or -)
        // Since area is constant a1-a2 = a3-a4, or a4=a3+a2-a1
//      float a4 = Signed2DTriArea(c, d, b); // Must have opposite sign of a3
        float a4 = a3 + a2 - a1;
        // Points a and b on different sides of cd if areas have different signs
        if (a3 * a4 < 0.0f) 
		{
            // Segments intersect. Find intersection point along L(t)=a+t*(b-a).
            // Given height h1 of a over cd and height h2 of b over cd,
            // t = h1 / (h1 - h2) = (b*h1/2) / (b*h1/2 - b*h2/2) = a3 / (a3 - a4),
            // where b (the base of the triangles cda and cdb, i.e., the length
            // of cd) cancels out.
            t = a3 / (a3 - a4);
            p = a + t * (b - a);
            return true;
        }
    }

    // Segments not intersecting (or collinear)
    return false;
}


/****************************************
*           CollisionHelper             *
****************************************/
CollisionHelper::CollisionHelper(void)
{
}


CollisionHelper::~CollisionHelper(void)
{
}


//result(x,y,z) == result(u,v,w)
void CollisionHelper::Barycentric(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& p, glm::vec3& result)
{
  // Unnormalized triangle normal
    Vector3 m = MFD_Cross(b - a, c - a);
    // Nominators and one-over-denominator for u and v ratios
    float32 nu, nv, ood;
    // Absolute components for determining projection plane
	float32 x = abs(m.x), y = abs(m.y), z = abs(m.z);

    // Compute areas in plane of largest projection
    if (x >= y && x >= z) 
	{
        // x is largest, project to the yz plane
        nu = TriArea2D(p.y, p.z, b.y, b.z, c.y, c.z); // Area of PBC in yz plane
        nv = TriArea2D(p.y, p.z, c.y, c.z, a.y, a.z); // Area of PCA in yz plane
        ood = 1.0f / m.x;                             // 1/(2*area of ABC in yz plane)
    } 
	else if (y >= x && y >= z)
	{
        // y is largest, project to the xz plane
        nu = TriArea2D(p.x, p.z, b.x, b.z, c.x, c.z);
        nv = TriArea2D(p.x, p.z, c.x, c.z, a.x, a.z);
        ood = 1.0f / -m.y;
    } 
	else 
	{
        // z is largest, project to the xy plane
        nu = TriArea2D(p.x, p.y, b.x, b.y, c.x, c.y);
        nv = TriArea2D(p.x, p.y, c.x, c.y, a.x, a.y);
        ood = 1.0f / m.z;
    }

    result.x = nu * ood;
	result.y = nv * ood;
    result.z = 1.0f - result.x - result.y;
} 


bool CollisionHelper::TestPointInTriangle(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
    Vector3 uvw;
    CollisionHelper::Barycentric(a, b, c, p,uvw);
    return uvw.x >= 0.0f && uvw.y >= 0.0f && uvw.x <= 1.0f && uvw.y <= 1.0f && (uvw.y + uvw.z) <= 1.0f;
}

Plane CollisionHelper::ComputePlane(const Vector3& a, const Vector3& b, const Vector3& c)
{
    Plane p;
    p.n = MFD_Normalize(MFD_Cross(b - a, c - a));
    p.d = MFD_Dot(p.n, a);
    return p;
}

bool CollisionHelper::SphereOnSphere(const Sphere& a, const Sphere& b)
{
    Vector3 d = a.c - b.c;
    float dist2 = MFD_Dot(d, d);
    float radiusSum = a.r + b.r;

    return dist2 <= radiusSum * radiusSum;
}

bool CollisionHelper::SphereOnCapsule(const Sphere& s, const Capsule& capsule)
{
    // Compute (squared) distance between sphere center and capsule line segment
    float dist2 = SqDistPointSegment(capsule.a, capsule.b, s.c);

    // If (squared) distance smaller than (squared) sum of radii, they collide
    float radius = s.r + capsule.r;
    return dist2 <= radius * radius;
}

bool CollisionHelper::CapsuleOnCapsule(const Capsule& capsule1, const Capsule& capsule2)
{
    // Compute (squared) distance between the inner structures of the capsules
    float s, t;
	Vector3 c1, c2;
    float dist2 = CollisionHelper::ClosestPtSegmentSegment(capsule1.a, capsule1.b, capsule2.a, capsule2.b, s, t, c1, c2);

    // If (squared) distance smaller than (squared) sum of radii, they collide
    float radius = capsule1.r + capsule2.r;
    return dist2 <= radius * radius;
}

Vector3 CollisionHelper::ClosestPtPointPlane(const Vector3& q, const Plane& p)
{
   float t = MFD_Dot(p.n, q) - p.d;
    return q  -t * p.n;
}

float32 CollisionHelper::DistPointPlane(const Vector3& q, const Plane& p)
{
    return (MFD_Dot(p.n, q) - p.d) / MFD_Dot(p.n, p.n);
}

// Given segment ab and point c, computes closest point d on ab.
// Also returns t for the parametric position of d, d(t) = a + t*(b - a)
void CollisionHelper::ClosestPtPointOnSegment(const Vector3& c, const Vector3& a, const Vector3& b, float32 &outT, Vector3 &outD)
{
    Vector3 ab = b - a;
    // Project c onto ab, but deferring divide by MFD_Dot(ab, ab)
    outT = MFD_Dot(c - a, ab);
    if (outT <= 0.0f) 
	{
        // c projects outside the [a,b] interval, on the a side; clamp to a
        outT = 0.0f;
        outD = a;
    } 
	else 
	{
        float denom = MFD_Dot(ab, ab); // Always nonnegative since denom = ||ab||^2
        if (outT >= denom) 
		{
            // c projects outside the [a,b] interval, on the b side; clamp to b
            outT = 1.0f;
            outD = b;
        } 
		else 
		{
            // c projects inside the [a,b] interval; must do deferred divide now
            outT = outT / denom;
            outD = a + outT * ab;
        }
    }
}


// Returns the squared distance between point c and segment ab
float32 CollisionHelper::SqDistPointSegment(const Vector3& a, const Vector3& b, const Vector3& c)
{
    Vector3 ab = b - a, ac = c  -a, bc = c - b;
    float32 e = MFD_Dot(ac, ab);
    // Handle cases where c projects outside ab
    if (e <= 0.0f) 
		return MFD_Dot(ac, ac);
    
	float32 f = MFD_Dot(ab, ab);
    if (e >= f) 
		return MFD_Dot(bc, bc);
    // Handle case where c projects onto ab
    return MFD_Dot(ac, ac) - e * e / f;
}


Vector3 CollisionHelper::ClosestPtPointOnTriangle(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
    // Check if P in vertex region outside A
    Vector3 ab = b - a;
    Vector3 ac = c - a;
    Vector3 ap = p - a;
    float32 d1 = MFD_Dot(ab, ap);
    float32 d2 = MFD_Dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) 
		return a; // barycentric coordinates (1,0,0)

    // Check if P in vertex region outside B
    Vector3 bp = p - b;
    float32 d3 = MFD_Dot(ab, bp);
    float32 d4 = MFD_Dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) 
		return b; // barycentric coordinates (0,1,0)

    // Check if P in edge region of AB, if so return projection of P onto AB
    float32 vc = d1*d4 - d3*d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) 
	{
        float v = d1 / (d1 - d3);
        return a + v * ab; // barycentric coordinates (1-v,v,0)
    }

    // Check if P in vertex region outside C
    Vector3 cp = p - c;
    float32 d5 = MFD_Dot(ab, cp);
    float32 d6 = MFD_Dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) 
		return c; // barycentric coordinates (0,0,1)

    // Check if P in edge region of AC, if so return projection of P onto AC
    float32 vb = d5*d2 - d1*d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) 
	{
        float32 w = d2 / (d2 - d6);
        return a + w * ac; // barycentric coordinates (1-w,0,w)
    }

    // Check if P in edge region of BC, if so return projection of P onto BC
    float32 va = d3*d6 - d5*d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) 
	{
        float32 w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + w * (c - b); // barycentric coordinates (0,1-w,w)
    }

    // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
    float32 denom = 1.0f / (va + vb + vc);
    float32 v = vb * denom;
    float32 w = vc * denom;
    return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}

bool CollisionHelper::PointOutsidePlane(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
    return MFD_Dot(p -a, MFD_Cross(b -a, c -a)) >= 0.0f; // [AP AB AC] >= 0
}

int CollisionHelper::PointSideOfPlane(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
	if(MFD_Dot(p -a, MFD_Cross(b -a, c -a)) >= 0.0f)
		return 1;
	else
		return -1;
}

float32 CollisionHelper::ClosestPtSegmentSegment(const Vector3& p1, const Vector3& q1, const Vector3& p2, const Vector3& q2,
                              float32 &s, float32 &t, Vector3& c1, Vector3& c2)
{
    Vector3 d1 = q1 - p1; // Direction vector of segment S1
    Vector3 d2 = q2 - p2; // Direction vector of segment S2
    Vector3 r = p1 - p2;
    float32 a = MFD_Dot(d1, d1); // Squared length of segment S1, always nonnegative
    float32 e = MFD_Dot(d2, d2); // Squared length of segment S2, always nonnegative
    float32 f = MFD_Dot(d2, r);

    // Check if either or both segments degenerate into points
    if (a <= EPSILON && e <= EPSILON) 
	{
        // Both segments degenerate into points
        s = t = 0.0f;
        c1 = p1;
        c2 = p2;
        return MFD_Dot(c1 - c2, c1 - c2);
    }

    if (a <= EPSILON) 
	{
        // First segment degenerates into a point
        s = 0.0f;
        t = f / e; // s = 0 => t = (b*s + f) / e = f / e
        t = MFD_Clamp(t, 0.0f, 1.0f);
    } 
	else 
	{
        float32 c = MFD_Dot(d1, r);
        if (e <= EPSILON) 
		{
            // Second segment degenerates into a point
            t = 0.0f;
            s = MFD_Clamp(-c / a, 0.0f, 1.0f); // t = 0 => s = (b*t - c) / a = -c / a
        } 
		else 
		{
            // The general nondegenerate case starts here
            float32 b = MFD_Dot(d1, d2);
            float32 denom = a*e-b*b; // Always nonnegative

            // If segments not parallel, compute closest point on L1 to L2, and
            // clamp to segment S1. Else pick arbitrary s (here 0)
            if (denom != 0.0f) 
			{
                s = MFD_Clamp((b*f - c*e) / denom, 0.0f, 1.0f);
            } 
			else
			{
				s = 0.0f;
			}

            // Compute point on L2 closest to S1(s) using
            // t = MFD_Dot((P1+D1*s)-P2,D2) / MFD_Dot(D2,D2) = (b*s + f) / e
            t = (b*s + f) / e;

            // If t in [0,1] done. Else clamp t, recompute s for the new value
            // of t using s = MFD_Dot((P2+D2*t)-P1,D1) / MFD_Dot(D1,D1)= (t*b - c) / a
            // and clamp s to [0, 1]
            if (t < 0.0f) 
			{
                t = 0.0f;
                s = MFD_Clamp(-c / a, 0.0f, 1.0f);
            } 
			else if (t > 1.0f) 
			{
                t = 1.0f;
                s = MFD_Clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
    return MFD_Dot(c1 - c2, c1 - c2);
}

bool CollisionHelper::SphereOnPlane(const Sphere& s, const Plane& p)
{
    // For a normalized plane (|p.n| = 1), evaluating the plane equation
    // for a point gives the signed distance of the point to the plane
    float32 dist = MFD_Dot(s.c, p.n) - p.d;
    // If sphere center within +/-radius from plane, plane intersects sphere
    return abs(dist) <= s.r;
}

// Determine whether sphere s fully behind (inside negative halfspace of) plane p
bool CollisionHelper::SphereInsidePlane(const Sphere& s, const Plane& p)
{
    float32 dist = MFD_Dot(s.c, p.n) - p.d;
    return dist < -s.r;
}


bool CollisionHelper::SphereOnTriangle(const Sphere& s, const Vector3& a, const Vector3& b, const Vector3& c, Vector3 &outP)
{
    outP = CollisionHelper::ClosestPtPointOnTriangle(s.c, a, b, c);

    Vector3 v = outP - s.c;
    return MFD_Dot(v, v) <= s.r * s.r;
}

bool CollisionHelper::RayOnSphere(const Vector3& p, const Vector3& d, const Sphere& s, float32 &outT, Vector3 &outP)
{
    Vector3 m = p - s.c;
    float32 b = MFD_Dot(m, d);
    float32 c = MFD_Dot(m, m) - s.r * s.r;
    
	// Exit if r's origin outside s (c > 0)and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f) 
		return false;
    
	float32 discr = b * b - c;

    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f) 
		return false;

    // Ray now found to intersect sphere, compute smallest t value of intersection
	outT = -b - sqrt(discr);
    // If t is negative, ray started inside sphere so clamp t to zero
    if (outT < 0.0f) 
		outT = 0.0f;
    outP = p + outT * d;

    return true;
}

bool CollisionHelper::RayOnSphere(const Vector3& p, const Vector3& d, const Sphere& s)
{
    Vector3 m = p - s.c;
    float32 c = MFD_Dot(m, m) - s.r * s.r;
    // If there is definitely at least one real root, there must be an intersection
    if (c <= 0.0f) 
		return true;

    float32 b = MFD_Dot(m, d);
    // Early exit if ray origin outside sphere and ray pointing away from sphere
    if (b > 0.0f) 
		return false;

    float32 disc = b*b - c;
    // A negative discriminant corresponds to ray missing sphere
    if (disc < 0.0f) 
		return false;

    // Now ray must hit sphere
    return true;
}

// Given line pq and ccw triangle abc, return whether line pierces triangle. If
// so, also return the barycentric coordinates uvw(xyz) in outP of the intersection point
bool CollisionHelper::LineOnTriangle(const Vector3& p, const Vector3& q, const Vector3& a, const Vector3& b, const Vector3& c, Vector3 &outP)
{
    Vector3 pq = q - p;
    Vector3 pa = a - p;
    Vector3 pb = b - p;
    Vector3 pc = c - p;
	Vector3 m = MFD_Cross(pq, pc);

    // Test if pq is inside the edges bc, ca and ab. Done by testing
    // that the signed tetrahedral volumes, computed using scalar triple
    // products, are all positive
	outP.x = MFD_Dot(pb, m);
	outP.y = -MFD_Dot(pa, m);
	if (SameSign(outP.x, outP.y) == false) 
		return false;

	outP.z = ScalarTriple(pq, pb, pa);
	if (SameSign(outP.x, outP.z) == false) 
		return false;	

    // Compute the barycentric coordinates (u, v, w) determining the
    // intersection point r, r = u*a + v*b + w*c
    float32 denom = 1.0f / (outP.x + outP.y + outP.z);
    outP.x *= denom;
    outP.y *= denom;
    outP.z *= denom; // w = 1.0f - u - v;
    return true;
}

bool CollisionHelper::SegmentOnTriangle(const Vector3& p, const Vector3& q, const Vector3& a, const Vector3& b, const Vector3& c, Vector3 &outP, float32 &outT)
{
    Vector3 ab = b - a;
    Vector3 ac = c - a;
    Vector3 qp = p - q;

    // Compute triangle normal. Can be precalculated or cached if
    // intersecting multiple segments against the same triangle
    Vector3 n = MFD_Cross(ab, ac);

    // Compute denominator d. If d == 0, segment is parallel to or points
    // away from triangle, so exit early
    float32 d = MFD_Dot(qp, n);
	if (d == 0.0f) 
		return false;

    // Compute intersection t value of pq with plane of triangle. A ray
    // intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
    // dividing by d until intersection has been found to pierce triangle
    Vector3 ap = p - a;
    outT = MFD_Dot(ap, n);
    if (outT < 0.0f)
	{
		//flip direction of qp
		ap = q - a;
		outT = MFD_Dot(ap, n);

		if (outT < 0.0f)
			return false;

		qp = -qp;
		d = MFD_Dot(qp, n);
	}

    if (outT > d) 
		return false; // For segment; exclude this code line for a ray test

    // Compute barycentric coordinate components and test if within bounds
    Vector3 e = MFD_Cross(qp, ap);
    outP.y = MFD_Dot(ac, e);
    if (outP.y < 0.0f || outP.y > d) 
		return false;

    outP.z = -MFD_Dot(ab, e);
    if (outP.z  < 0.0f || outP.y + outP.z  > d) 
		return false;

    // Segment/ray intersects triangle. Perform delayed division and
    // compute the last barycentric coordinate component
    float ood = 1.0f / d;
    outT *= ood;
    outP.y *= ood;
    outP.z *= ood;
    outP.x = 1.0f - outP.y - outP.z;

	outP = a * outP.x + b * outP.y + c * outP.z;

    return true;
}

bool CollisionHelper::SegmentOnTriangle(const Vector3& p, const Vector3& q, const Vector3& a, const Vector3& b, const Vector3& c)
{
    Vector3 ab = b - a;
    Vector3 ac = c - a;
    Vector3 qp = p - q;

    // Compute triangle normal. Can be precalculated or cached if
    // intersecting multiple segments against the same triangle
    Vector3 n = MFD_Cross(ab, ac);

    // Compute denominator d. If d <= 0, segment is parallel to or points
    // away from triangle, so exit early
    float32 d = MFD_Dot(qp, n);
    if (d == 0.0f) 
		return false;

    // Compute intersection t value of pq with plane of triangle. A ray
    // intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
    // dividing by d until intersection has been found to pierce triangle
    Vector3 ap = p - a;
    float32 t = MFD_Dot(ap, n);
 	if (t < 0.0f)
	{
		//flip direction of qp
		ap = q - a;
		t = MFD_Dot(ap, n);

		if (t < 0.0f)
			return false;

		qp = -qp;
		d = MFD_Dot(qp, n);
	}

    if (t > d) 
		return false; // For segment; exclude this code line for a ray test

    // Compute barycentric coordinate components and test if within bounds
    Vector3 e = MFD_Cross(qp, ap);
    float32 v = MFD_Dot(ac, e);
    if (v < 0.0f || v > d) 
		return false;

    float32 w = -MFD_Dot(ab, e);
    if (w  < 0.0f || v + w > d) 
		return false;

    return true;
}

bool CollisionHelper::SegmentOnPlane(const Vector3& a, const Vector3& b, const Plane& p, Vector3 &outP, float32 &outT)
{

	// Compute the t value for the directed line ab intersecting the plane
	Vector3 ab = b - a;
	float32 t = (p.d - MFD_Dot(p.n, a)) / MFD_Dot(p.n, ab);
	// If t in [0..1] compute and return intersection point
	// and because it will be very naughty when t is 0 or 1, then I clamp the range to between 0 and 1(not including 0 and 1)
	if (t >= 0.0f && t <= 1.0f) 
	{
		outT = t;
		outP = a + outT * ab;
		return true;
	}

	return false;
}

bool 
CollisionHelper::TriangleOnPlane(const Vector3& a, const Vector3& b, const Vector3& c, const Plane& p, Vector3 &outP1, Vector3 &outP2, float32 &outT1, float32 &outT2,uint32 &uSingleSideIndex)
{
	int32 hit = 0;
	Vector3* currentP = &outP1;
	float32* currentT = &outT1;
	bool bABTouched = false;
	if(CollisionHelper::SegmentOnPlane(a,b,p,*currentP,*currentT))
	{
		++hit;
		currentP = &outP2;
		currentT = &outT2;
		bABTouched = true;
	}

	if(CollisionHelper::SegmentOnPlane(a,c,p,*currentP,*currentT))
	{
		++hit;
		currentP = &outP2;
		currentT = &outT2;
	}

	if(hit == 2)
	{
		if(outP1 != outP2)
		{
			uSingleSideIndex = 0;
			return true;
		}
		else
		{
			//two point at same position,  not to consider single vertex hit is a hit, then check the third edge
			--hit;
		}
	}
	else if(hit == 0)
	{
		return false;
	}

	if(CollisionHelper::SegmentOnPlane(b,c,p,*currentP,*currentT))
	{
		++hit;
	}

	if(hit == 2)
	{
		if(outP1 == outP2)
		{
			//it's a single vertex hit, not consider as a hit
			return false;

			//or
			//it's a single vertex hit, be careful, not sure if this will happen in "real game"
			//offset one of the position a little little little bit
			//outP2 += MFD_Normalize(b-c) * 0.0001f;
		}
		else
		{
			if(bABTouched)
			{
				uSingleSideIndex = 1;
			}
			else
			{
				uSingleSideIndex = 2;
			}
			return true;
		}
	}
	else if(hit == 0)
	{
		return false;
	}
	else
	{
		//it's a single vertex hit, not consider as a hit
		return false;
	}
}

//6 edge-triangle tests
bool CollisionHelper::TriangleOnTriangle(const Vector3& v1a, const Vector3& v1b, const Vector3& v1c,const Vector3& v2a, const Vector3& v2b, const Vector3& v2c)
{
	//if all three points on one side, exit. Oh Yeah
	bool t1 = PointOutsidePlane(v1a, v2a, v2b, v2c);
	if(t1 == PointOutsidePlane(v1b, v2a, v2b, v2c)
		&& t1 == PointOutsidePlane(v1c, v2a, v2b, v2c))
		return false;

	t1 = PointOutsidePlane(v2a, v1a, v1b, v1c);
	if(t1 == PointOutsidePlane(v2b, v1a, v1b, v1c)
		&& t1 == PointOutsidePlane(v2c, v1a, v1b, v1c))
		return false;


	if(CollisionHelper::SegmentOnTriangle(v1a, v1b, v2a, v2b, v2c))
		return true;

	if(CollisionHelper::SegmentOnTriangle(v1b, v1c, v2a, v2b, v2c))
		return true;

	if(CollisionHelper::SegmentOnTriangle(v1c, v1a, v2a, v2b, v2c))
		return true;

	if(CollisionHelper::SegmentOnTriangle(v2a, v2b, v1a, v1b, v1c))
		return true;

	if(CollisionHelper::SegmentOnTriangle(v2b, v2c, v1a, v1b, v1c))
		return true;

	if(CollisionHelper::SegmentOnTriangle(v2c, v2a, v1a, v1b, v1c))
		return true;

	return false;
}

bool CollisionHelper::TriangleOnTriangle(const Vector3& v1a, const Vector3& v1b, const Vector3& v1c,const Vector3& v2a, const Vector3& v2b, const Vector3& v2c, Vector3 &outP1, Vector3 &outP2)
{
	//if all three points on one side, exit. Oh Yeah
	bool t1 = PointOutsidePlane(v1a, v2a, v2b, v2c);
	if(t1 == PointOutsidePlane(v1b, v2a, v2b, v2c)
		&& t1 == PointOutsidePlane(v1c, v2a, v2b, v2c))
		return false;

	t1 = PointOutsidePlane(v2a, v1a, v1b, v1c);
	if(t1 == PointOutsidePlane(v2b, v1a, v1b, v1c)
		&& t1 == PointOutsidePlane(v2c, v1a, v1b, v1c))
		return false;

	int32 hit = 0;
	float32 t = 0;
	Vector3* currentP = &outP1;

	if(CollisionHelper::SegmentOnTriangle(v1a, v1b, v2a, v2b, v2c, *currentP, t))
	{
		++hit;
		currentP = &outP2;
	}

	if(CollisionHelper::SegmentOnTriangle(v1b, v1c, v2a, v2b, v2c, *currentP, t))
	{
		++hit;
		currentP = &outP2;
		if(hit == 2)
			return true;
	}

	if(CollisionHelper::SegmentOnTriangle(v1c, v1a, v2a, v2b, v2c, *currentP, t))
	{
		++hit;
		currentP = &outP2;
		if(hit == 2)
			return true;
	}

	if(CollisionHelper::SegmentOnTriangle(v2a, v2b, v1a, v1b, v1c, *currentP,t))
	{
		++hit;
		currentP = &outP2;
		if(hit == 2)
			return true;
	}

	if(CollisionHelper::SegmentOnTriangle(v2b, v2c, v1a, v1b, v1c, *currentP,t))
	{
		++hit;
		currentP = &outP2;
		if(hit == 2)
			return true;
	}

	if(hit == 0)
		return false;

	if(CollisionHelper::SegmentOnTriangle(v2c, v2a, v1a, v1b, v1c, *currentP,t))
	{
		++hit;
		currentP = &outP2;
		if(hit == 2)
			return true;
	}

	//assert(hit == 0);

	return false;
}