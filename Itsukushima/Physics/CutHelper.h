/*
* All the "actual" cutting codes are here
* todo:more QA and fix all the bugs
* todo:improve performance
*
* @author: Kai Yang
*/

#ifndef CUT_HELPER_H
#define CUT_HELPER_H

#include <Core/CoreHeaders.h>
#include <Physics/PhysicsStructs.h>

#include <vector>
#include <list>

class GameObject;
class Collider;

struct PlaneHitInfo;
struct Face;

class CutHelper
{
private:
	struct TempObject
	{
		std::list<Face*> faces;
		Vector3 vMiddlePoint;
	};

public:
	static void CutObject(GameObject* pObject, PlaneHitInfo* pHitInfo, std::list<GameObject*> &posSideObjects, std::list<GameObject*> &negSideObjects);
	static void CutObject_complex(GameObject* pObject, PlaneHitInfo* pHitInfo, std::list<GameObject*> &posSideObjects, std::list<GameObject*> &negSideObjects);

private:
	static void CalculateSeparatedObjects(std::list<Face*> &searchFaces, std::vector<Vector3> &vertexData, std::list<CutHelper::TempObject*> &objectCollection);

	//return true, if any face is attached to a object
	static bool AttachToObject(std::list<TempObject*> &objects, std::vector<Vector3>& verticesData, std::list<Face*> &faces);

	//static void BuildAdjLinks(std::vector<Face*> &searchFaces, std::vector<Vector3>& verticesData);

	static void CutFaces(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPosFaces, std::list<Face*>* pNegFaces);
	static void CutFaces_Collider(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPosFaces, std::list<Face*>* pNegFaces);

	static void CreateCrossSections(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPosFaces, std::list<Face*>* pNegFaces, float32 fMaxUV, float32 fUVScaleFactor);
	static void CreateCrossSections_Collider(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPosFaces, std::list<Face*>* pNegFaces);

	static GameObject* CreateNewObjectFromPieces(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPieceFaces);
	static void CreateNewObjectFromPieces_Collider(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPieceFaces, GameObject* pObjToAttach);

	//collider will be added to the closest object
	static void CreateNewObjectFromPieces_Collider_complex(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPieceFaces, std::list<GameObject*>* pObjects);

	static void CutCollider(GameObject* pObject, PlaneHitInfo* pHitInfo, GameObject* pNewObj1, GameObject* pNewObj2);
	static void CutCollider_complex(GameObject* pObject, PlaneHitInfo* pHitInfo, std::list<GameObject*> &posSideObjects, std::list<GameObject*> &negSideObjects);

	static void SortOrder(PlaneHitInfo* pHitInfo,std::vector<std::list<Vector3*>> &outResult);

	//return true if the shape of points is convex
	static bool FixPointsOrder(std::list<Vector3*> &points, const Vector3& vRefNormal);

	static void TriangulationConvex(std::list<Vector3*> &inCollection, std::vector<Vector3*> &outCollection);
	static void TriangulationEarClipping(std::list<Vector3*> &inCollection, std::vector<Vector3*> &outCollection, const Vector3& vRefNormal);

	static Vector3 CalculateMiddlePoint(std::vector<Vector3>& verticesData, std::list<Face*> &faces);

	//remove sequence repeat points, and any points in a middle of a line
	static void FixCrossSectionPoints(std::list<Vector3*> &points);

	//every two points a part, remove any repeat pairs (no matter the order) 
	static void RemoveRepeatPairs(std::vector<Vector3>& points, uint32& uPointCount);

public:
	static uint32 s_uPieceCount;
	static uint32 s_uColliderPieceCount;
};

#endif
