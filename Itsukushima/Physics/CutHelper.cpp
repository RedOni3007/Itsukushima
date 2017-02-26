#include "CutHelper.h"
#include <Physics/Collider.h>
#include <Resource/Mesh.h>
#include <Resource/CollisionMesh.h>
#include <Physics/RigidBody.h>
#include <Resource/Model.h>
#include <Game/GameObject.h>
#include <Physics/CollisionManager.h>
#include <Math/CollisionHelper.h>
#include <Math/MathHelper.h>

#include <glm/ext.hpp>

uint32 CutHelper::s_uPieceCount = 0;
uint32 CutHelper::s_uColliderPieceCount = 0;

void
CutHelper::CalculateSeparatedObjects(std::list<Face*> &searchFaces, std::vector<Vector3> &vertexData, std::list<CutHelper::TempObject*> &objectCollection)
{
	CutHelper::TempObject* pCurrentObject = nullptr;
	Face* pCurrentFace = nullptr;
	std::list<Face*>::iterator it1, it2;
	bool bConnectFound = false;
	bool bFoundNothing = true;
	while(searchFaces.size() > 0)
	{
		pCurrentFace = searchFaces.front();
		searchFaces.pop_front();
		pCurrentObject = new CutHelper::TempObject();
		pCurrentObject->faces.push_back(pCurrentFace);
		objectCollection.push_back(pCurrentObject);
		
		while(true)
		{
			bFoundNothing = true;
			for(it1 = searchFaces.begin(); it1 != searchFaces.end();)
			{
				for(it2 = pCurrentObject->faces.begin(); it2 != pCurrentObject->faces.end(); ++it2)
				{

					bConnectFound = false;
					for(uint32 i = 0; i < 3; ++i)
					{
						for(uint32 j = 0; j < 3; ++j)
						{
							if(MathHelper::AlmostEqual(vertexData[(*it1)->v[i]],vertexData[(*it2)->v[j]]))
							{
								bConnectFound= true;
								break;
							}
						}//j
						if(bConnectFound)
							break;
					}//i
					if(bConnectFound)
						break;
				}//it2

				if(bConnectFound)
				{
					bFoundNothing = false;
					pCurrentObject->faces.push_back(*it1);
					it1 = searchFaces.erase(it1);
				}
				else 
				{
					++it1;
				}
			}//it1
			
			if(bFoundNothing)
				break;
		}//while(true)
	}//while(searchFaces.size() > 0)
}

/*
void 
CutHelper::BuildAdjLinks(std::vector<Face*> &searchFaces, std::vector<Vector3>& verticesData)
{
	//super expensive T_T
	Face* pCurrentFace = nullptr;
	Face* pTargetFace = nullptr;

	//test position, I prefer only the position
	Vector3* pCurrentVertex = nullptr;
	Vector3* pTargetVertex = nullptr;

	uint32 uHitCount = 0;
	uint32 uFaceCount = searchFaces.size();
	for(uint32 i = 0; i < uFaceCount; ++i)
	{
		pCurrentFace = searchFaces[i];
		for(uint32 j = (i + 1); j < uFaceCount; ++j)
		{
			pTargetFace = searchFaces[j];
			if(pCurrentFace == pTargetFace)
				continue;

			uHitCount = 0;
			for(int k = 0; k < 3; ++k)
			{
				pCurrentVertex = &verticesData[pCurrentFace->v[k]];
				for(int l = 0; l < 3; ++l)
				{
					pTargetVertex =  &verticesData[ pTargetFace->v[l]];
					if((*pCurrentVertex) == (*pTargetVertex))
					{
						++uHitCount;
						break;
					}
				}

				if(uHitCount == 2)
					break;
			}

			if(uHitCount == 2)
			{
				pCurrentFace->adjFaces.push_back(pTargetFace->index);
				pTargetFace->adjFaces.push_back(pCurrentFace->index);
			}
		}
	}
}
*/

 bool 
CutHelper::AttachToObject(std::list<TempObject*> &objects, std::vector<Vector3>& verticesData, std::list<Face*> &faces)
 {
	 std::list<TempObject*>::iterator objectIT;
	 std::list<Face*>::iterator faceIT1,faceIT2;

	 TempObject* pObject = nullptr;
	 Face* pFace1 = nullptr;
	 Face* pFace2 = nullptr;

	 bool bFoundOnce = false;
	 bool bConnectFound = false;
	 for(faceIT1 = faces.begin(); faceIT1 != faces.end();)
	 {
		bConnectFound = false;
		pFace1 = *faceIT1;
		for(objectIT = objects.begin(); objectIT != objects.end(); ++objectIT)
		{
			pObject = *objectIT;
			for(faceIT2 = pObject->faces.begin(); faceIT2 != pObject->faces.end(); ++faceIT2)
			{
				pFace2 = *faceIT2;
				for(uint32 i = 0; i < 3; ++i)
				{
					for(uint32 j = 0; j < 3; ++j)
					{
						if(MathHelper::AlmostEqual(verticesData[pFace1->v[i]],verticesData[pFace2->v[j]]))
						{
							bConnectFound= true;
							break;
						}
					}//j
					if(bConnectFound)
						break;
				}//i
				if(bConnectFound)
					break;
			}//faceIT
			if(bConnectFound)
			{
				pObject->faces.push_back(pFace1);
				break;
			}
		}//objectIT
		if(bConnectFound)
		{
			faceIT1 = faces.erase(faceIT1);
			bFoundOnce = true;
		}
		else
		{
			++faceIT1;
		}
	 }//i

	 return bFoundOnce;
 }

void 
CutHelper::CutFaces(GameObject* pObject, PlaneHitInfo* pHitInfo, std::list<Face*>* pPosFaces, std::list<Face*>* pNegFaces)
 {
	Model* pModel = pObject->GetModelComponent();
	Mesh* pMesh = pModel->GetMesh();
	std::vector<Vector3> &vertices = *pModel->GetVerticesCache();
	std::vector<Face> &faces = *pMesh->GetFaces();
	std::vector<Vector3> &normals = *pMesh->GetNormals();
	std::vector<Vector2> &uvs = *pMesh->GetUVs();

	Face* pFaceToCut = nullptr;
	uint32 uSingleSideIndex = 0;
	Face newFace1,newFace2,newFace3;
	Vector3* pSinglePoint = nullptr;
	uint32 newVertexIndex1 = 0;
	uint32 newVertexIndex2 = 0;
	uint32 newFaceIndex = 0;
	Vector3 vPos1,vPos2;
	Vector3 vNormal1,vNormal2;
	Vector2 vUV1,vUV2;
	uint32 uIndex1,uIndex2;
	float32 t1,t2;
	float32 t1_inv,t2_inv;
	bool bPositiveSingleSide = false;;
	for(uint32 i = 0; i < pHitInfo->uObjectFacesCount; ++i)
	{
		uIndex1 = i*2;
		uIndex2 = uIndex1 + 1;
		//create new faces
		pFaceToCut = &faces[pHitInfo->ObjectFaces[i]];
		uSingleSideIndex = pHitInfo->ObjectFaceSingleSides[i];
		newVertexIndex1 = vertices.size();
		newVertexIndex2 = newVertexIndex1 + 1;
		vertices.push_back(pHitInfo->touchedPostions[uIndex1]);
		vertices.push_back(pHitInfo->touchedPostions[uIndex2]);
		pSinglePoint = &vertices[pFaceToCut->v[uSingleSideIndex]];//record where is the start
		bPositiveSingleSide = (CollisionHelper::DistPointPlane(*pSinglePoint, pHitInfo->sPlane) > 0);

		t1 = 1.0f - pHitInfo->touchedPostionsT[uIndex1];
		t2 = 1.0f - pHitInfo->touchedPostionsT[uIndex2];
		t1_inv = 1.0f - t1;
		t2_inv = 1.0f - t2;
		//calculate normal and uv, and new faces, newface1 always be the single side
		if(uSingleSideIndex == 0)
		{
			vNormal1 = normals[pFaceToCut->v[0]] * t1 + normals[pFaceToCut->v[1]] * t1_inv;
			vNormal2 = normals[pFaceToCut->v[0]] * t2 + normals[pFaceToCut->v[2]] * t2_inv;
			vUV1 = uvs[pFaceToCut->v[0]] * t1 + uvs[pFaceToCut->v[1]] * t1_inv;
			vUV2 = uvs[pFaceToCut->v[0]] * t2 + uvs[pFaceToCut->v[2]] * t2_inv;

			newFace1.v[0] = pFaceToCut->v[0];
			newFace1.v[1] = newVertexIndex1;
			newFace1.v[2] = newVertexIndex2;

			newFace2.v[0] = pFaceToCut->v[1];
			newFace2.v[1] = newVertexIndex2;
			newFace2.v[2] = newVertexIndex1;

			newFace3.v[0] = pFaceToCut->v[2];
			newFace3.v[1] = newVertexIndex2;
			newFace3.v[2] = pFaceToCut->v[1];
		}
		else if(uSingleSideIndex == 1)
		{
			vNormal1 = normals[pFaceToCut->v[0]] * t1 + normals[pFaceToCut->v[1]] * t1_inv;
			vNormal2 = normals[pFaceToCut->v[1]] * t2 + normals[pFaceToCut->v[2]] * t2_inv;
			vUV1 = uvs[pFaceToCut->v[0]] * t1 + uvs[pFaceToCut->v[1]] * t1_inv;
			vUV2 = uvs[pFaceToCut->v[1]] * t2 + uvs[pFaceToCut->v[2]] * t2_inv;

			newFace1.v[0] = pFaceToCut->v[1];
			newFace1.v[1] = newVertexIndex2;
			newFace1.v[2] = newVertexIndex1;

			newFace2.v[0] = pFaceToCut->v[2];
			newFace2.v[1] = newVertexIndex1;
			newFace2.v[2] = newVertexIndex2;

			newFace3.v[0] = pFaceToCut->v[0];
			newFace3.v[1] = newVertexIndex1;
			newFace3.v[2] = pFaceToCut->v[2];
		}
		else if(uSingleSideIndex == 2)
		{
			vNormal1 = normals[pFaceToCut->v[0]] * t1 + normals[pFaceToCut->v[2]] * t1_inv;
			vNormal2 = normals[pFaceToCut->v[1]] * t2 + normals[pFaceToCut->v[2]] * t2_inv;
			vUV1 = uvs[pFaceToCut->v[0]] * t1 + uvs[pFaceToCut->v[2]] * t1_inv;
			vUV2 = uvs[pFaceToCut->v[1]] * t2 + uvs[pFaceToCut->v[2]] * t2_inv;

			newFace1.v[0] = pFaceToCut->v[2];
			newFace1.v[1] = newVertexIndex1;
			newFace1.v[2] = newVertexIndex2;

			newFace2.v[0] = pFaceToCut->v[0];
			newFace2.v[1] = newVertexIndex2;
			newFace2.v[2] = newVertexIndex1;

			newFace3.v[0] = pFaceToCut->v[1];
			newFace3.v[1] = newVertexIndex2;
			newFace3.v[2] = pFaceToCut->v[0];
		}
		normals.push_back(vNormal1);
		normals.push_back(vNormal2);
		uvs.push_back(vUV1);
		uvs.push_back(vUV2);

		newFaceIndex = faces.size();
		faces.push_back(newFace1);
		faces.push_back(newFace2);
		faces.push_back(newFace3);

		if(bPositiveSingleSide)
		{
			pPosFaces->push_back(&faces[newFaceIndex]);
			pNegFaces->push_back(&faces[newFaceIndex+1]);
			pNegFaces->push_back(&faces[newFaceIndex+2]);
		}
		else
		{
			pNegFaces->push_back(&faces[newFaceIndex]);
			pPosFaces->push_back(&faces[newFaceIndex+1]);
			pPosFaces->push_back(&faces[newFaceIndex+2]);
		}		
	}
 }

 void 
CutHelper::CutFaces_Collider(GameObject* pObject, PlaneHitInfo* pHitInfo, std::list<Face*>* pPosFaces, std::list<Face*>* pNegFaces)
 {
	Collider* pColldier = pObject->GetColliderComponent();
	CollisionMesh* pCollisionMesh = pColldier->GetCollisionMesh();
	std::vector<Vector3> &vertices =  *pColldier->GetVerticesCache();
	std::vector<Face> &faces = *pCollisionMesh->GetFaces();

	Face* pFaceToCut = nullptr;
	uint32 uSingleSideIndex = 0;
	Face newFace1,newFace2,newFace3;
	Vector3* pSinglePoint = nullptr;
	uint32 newVertexIndex1 = 0;
	uint32 newVertexIndex2 = 0;
	uint32 newFaceIndex = 0;
	Vector3 vPos1,vPos2;
	uint32 uIndex1,uIndex2;
	bool bPositiveSingleSide = false;;
	for(uint32 i = 0; i < pHitInfo->uObjectFacesCount; ++i)
	{
		uIndex1 = i*2;
		uIndex2 = uIndex1 + 1;
		//create new faces
		pFaceToCut = &faces[pHitInfo->ObjectFaces[i]];
		uSingleSideIndex = pHitInfo->ObjectFaceSingleSides[i];
		newVertexIndex1 = vertices.size();
		newVertexIndex2 = newVertexIndex1 + 1;
		vertices.push_back(pHitInfo->touchedPostions[uIndex1]);
		vertices.push_back(pHitInfo->touchedPostions[uIndex2]);
		pSinglePoint = &vertices[pFaceToCut->v[uSingleSideIndex]];//record where is the start
		bPositiveSingleSide = (CollisionHelper::DistPointPlane(*pSinglePoint, pHitInfo->sPlane) > 0);

		//calculate normal and uv, and new faces, newface1 always be the single side
		if(uSingleSideIndex == 0)
		{
			newFace1.v[0] = pFaceToCut->v[0];
			newFace1.v[1] = newVertexIndex1;
			newFace1.v[2] = newVertexIndex2;

			newFace2.v[0] = pFaceToCut->v[1];
			newFace2.v[1] = newVertexIndex2;
			newFace2.v[2] = newVertexIndex1;

			newFace3.v[0] = pFaceToCut->v[2];
			newFace3.v[1] = newVertexIndex2;
			newFace3.v[2] = pFaceToCut->v[1];
		}
		else if(uSingleSideIndex == 1)
		{
			newFace1.v[0] = pFaceToCut->v[1];
			newFace1.v[1] = newVertexIndex2;
			newFace1.v[2] = newVertexIndex1;

			newFace2.v[0] = pFaceToCut->v[2];
			newFace2.v[1] = newVertexIndex1;
			newFace2.v[2] = newVertexIndex2;

			newFace3.v[0] = pFaceToCut->v[0];
			newFace3.v[1] = newVertexIndex1;
			newFace3.v[2] = pFaceToCut->v[2];
		}
		else if(uSingleSideIndex == 2)
		{
			newFace1.v[0] = pFaceToCut->v[2];
			newFace1.v[1] = newVertexIndex1;
			newFace1.v[2] = newVertexIndex2;

			newFace2.v[0] = pFaceToCut->v[0];
			newFace2.v[1] = newVertexIndex2;
			newFace2.v[2] = newVertexIndex1;

			newFace3.v[0] = pFaceToCut->v[1];
			newFace3.v[1] = newVertexIndex2;
			newFace3.v[2] = pFaceToCut->v[0];
		}

		newFaceIndex = faces.size();
		faces.push_back(newFace1);
		faces.push_back(newFace2);
		faces.push_back(newFace3);

		if(bPositiveSingleSide)
		{
			pPosFaces->push_back(&faces[newFaceIndex]);
			pNegFaces->push_back(&faces[newFaceIndex+1]);
			pNegFaces->push_back(&faces[newFaceIndex+2]);
		}
		else
		{
			pNegFaces->push_back(&faces[newFaceIndex]);
			pPosFaces->push_back(&faces[newFaceIndex+1]);
			pPosFaces->push_back(&faces[newFaceIndex+2]);
		}		
	}
 }

void 
CutHelper::CreateCrossSections(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPosFaces, std::list<Face*>* pNegFaces, float32 fMaxUV, float32 fUVScaleFactor)
{
	Model* pModel = pObject->GetModelComponent();
	Mesh* pMesh = pModel->GetMesh();
	std::vector<Vector3> &vertices = *pModel->GetVerticesCache();
	std::vector<Face> &faces = *pMesh->GetFaces();
	std::vector<Vector3> &normals = *pMesh->GetNormals();
	std::vector<Vector2> &uvs = *pMesh->GetUVs();

	std::vector<std::list<Vector3*>> crossSections;
	crossSections.reserve((int32)(pHitInfo->uTouchPostionCount * 0.34f));

	//sort touched positions into order first, only record the vertices at the two end of an edge
	CutHelper::SortOrder(pHitInfo, crossSections);

	//the normal of surface is the normal of cut blade, but need to transform into object local spacess
	Matrix44 mWorldMatInverse = glm::inverse(*pObject->GetTranformMat());	
	Vector3 vNormal = MFD_Normalize(Vector3(mWorldMatInverse * Vector4(pHitInfo->sPlane.n,0)));

	//decide to project on which axis plane to calculate UV, let's just do it in world space
	//it's a little better do it in local space, but the section vertices need convert to local space too, a little expensive for a little bit better result 
	Vector3 vNormalWorld = pHitInfo->sPlane.n;
	uint32 projectMasks[2] = {0,1};
	if(fabsf(vNormalWorld.x) >= fabsf(vNormalWorld.y) && fabsf(vNormalWorld.x) >= fabsf(vNormalWorld.z))
	{
		projectMasks[0] = 1;
		projectMasks[1] = 2;
	}
	else if(fabsf(vNormalWorld.y) >= fabsf(vNormalWorld.x) && fabsf(vNormalWorld.y) >= fabsf(vNormalWorld.z))
	{
		projectMasks[0] = 0;
		projectMasks[1] = 2;
	}


	//everything sorted, now create the new cross sections
	uint32 uSectionCount = crossSections.size();
	uint32 uSectionStartIndex = 0;
	uint32 uSectionEndIndex = 0;
	uint32 uSectionVertexCount = 0;
	Face tempFace1,tempFace2;//two in opposite direction
	Vector2 tempUV(0,0);
	std::vector<Vector3*> processedCollection;
	float32 fDot = 0;
	std::list<Vector3*>* pCurrentSection = nullptr;
	std::list<Vector3*>::iterator it1,it2;
	for(uint32 i = 0; i < uSectionCount; ++i)
	{
		pCurrentSection = &crossSections[i];
		uSectionVertexCount = pCurrentSection->size();
		if(uSectionVertexCount < 3)
		{
			//assert(!"at least need 3 points to make a face");
			continue;
		}

		CutHelper::FixCrossSectionPoints(*pCurrentSection);

		//check the order of points
		bool bConvex = CutHelper::FixPointsOrder(*pCurrentSection, pHitInfo->sPlane.n);

		//calculate texture coord range
		Vector2 minXY(FLT_MAX,FLT_MAX);
		Vector2 maxXY(-FLT_MAX,-FLT_MAX);
		Vector2 rangeXY;
		for(it1 = pCurrentSection->begin(); it1 != pCurrentSection->end(); ++it1)
		{
			tempUV.x  = (**it1)[projectMasks[0]];
			tempUV.y  = (**it1)[projectMasks[1]];
			if(tempUV.x < minXY.x)
				 minXY.x = tempUV.x;
			if(tempUV.x > maxXY.x)
				 maxXY.x = tempUV.x;
			if(tempUV.y < minXY.y)
				 minXY.y = tempUV.y;
			if(tempUV.y > maxXY.y)
				 maxXY.y = tempUV.y;
		}
		rangeXY.x = fabsf(minXY.x - maxXY.x) * 0.5f;
		rangeXY.y = fabsf(minXY.y - maxXY.y) * 0.5f;

		Vector2 vMiddlePoint = (minXY + maxXY) * 0.5f;
		Vector2 vOverallUVSCale = rangeXY / fUVScaleFactor;
		if(vOverallUVSCale.x > 1.0f)
			vOverallUVSCale.x  = 1.0f;
		if(vOverallUVSCale.y > 1.0f)
			vOverallUVSCale.y  = 1.0f;

		//triangulation
		processedCollection.clear();
		if(bConvex)
			CutHelper::TriangulationConvex(*pCurrentSection,processedCollection);
		else
			CutHelper::TriangulationEarClipping(*pCurrentSection,processedCollection, pHitInfo->sPlane.n);

		uSectionVertexCount = processedCollection.size();

		if(uSectionVertexCount < 3)
		{
			assert(!"wrong");
			return;
		}

		//check the direction
		bool bFacePlane = false;//if current order is already 
		Vector3 vAB = *processedCollection[1] - *processedCollection[0];
		Vector3 vBC = *processedCollection[2] - *processedCollection[1];
		Vector3 vCross = MFD_Cross(vAB,vBC);
		fDot = MFD_Dot(vCross, pHitInfo->sPlane.n);
		if(fDot < 0)
		{
			bFacePlane = true;
		}

		//create vertices data 
		uint32 pStartVertexIndex = vertices.size();	
		for(uint32 j = 0; j < uSectionVertexCount; ++j)
		{
			vertices.push_back(*processedCollection[j]);
			vertices.push_back(*processedCollection[j]);

			if(bFacePlane)
			{
				normals.push_back(-vNormal);
				normals.push_back(vNormal);
			}
			else
			{
				normals.push_back(vNormal);
				normals.push_back(-vNormal);
			}
				
			tempUV.x  = (*processedCollection[j])[projectMasks[0]];
			tempUV.y  = (*processedCollection[j])[projectMasks[1]];
			tempUV = ((tempUV - vMiddlePoint) / rangeXY * vOverallUVSCale * 0.5f + 0.5f) * fMaxUV;
			uvs.push_back(tempUV);
			uvs.push_back(tempUV);
		}

		//create new faces	
		uSectionVertexCount *= 2;
		uint32 faceIndex = 0;
		for(uint32 j = 0; j < uSectionVertexCount; j += 6)
		{
			tempFace1.v[0] = pStartVertexIndex + j;	
			tempFace1.v[1] = pStartVertexIndex + j + 2;
			tempFace1.v[2] = pStartVertexIndex + j + 4;
			tempFace2.v[0] = tempFace1.v[0] + 1;
			tempFace2.v[1] = tempFace1.v[2] + 1;
			tempFace2.v[2] = tempFace1.v[1] + 1;

			faceIndex = faces.size();
			faces.push_back(tempFace1);
			faces.push_back(tempFace2);

			if(bFacePlane)
			{
				pPosFaces->push_back(&faces[faceIndex]);
				pNegFaces->push_back(&faces[faceIndex+1]);
			}
			else
			{
				pPosFaces->push_back(&faces[faceIndex + 1]);
				pNegFaces->push_back(&faces[faceIndex]);
			}
		}
	}//	for(uint32 i = 0; i < uSectionCount; ++i)
}

void 
CutHelper::CreateCrossSections_Collider(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPosFaces, std::list<Face*>* pNegFaces)
{
	Collider* pColldier = pObject->GetColliderComponent();
	CollisionMesh* pCollisionMesh = pColldier->GetCollisionMesh();
	std::vector<Vector3> &vertices =  *pColldier->GetVerticesCache();
	std::vector<Face> &faces = *pCollisionMesh->GetFaces();

	std::vector<std::list<Vector3*>> crossSections;
	crossSections.reserve((int32)(pHitInfo->uTouchPostionCount * 0.34f));

	//sort touched positions into order first, only record the vertices at the two end of an edge
	CutHelper::SortOrder(pHitInfo, crossSections);


	//everything sorted, now create the new cross sections
	uint32 uSectionCount = crossSections.size();
	uint32 uSectionStartIndex = 0;
	uint32 uSectionEndIndex = 0;
	uint32 uSectionVertexCount = 0;
	Face tempFace1,tempFace2;//two in opposite direction
	Vector2 tempUV(0,0);
	std::vector<Vector3*> processedCollection;
	float32 fDot = 0;
	std::list<Vector3*>* pCurrentSection = nullptr;
	std::list<Vector3*>::iterator it1,it2;
	for(uint32 i = 0; i < uSectionCount; ++i)
	{
		pCurrentSection = &crossSections[i];
		uSectionVertexCount = pCurrentSection->size();
		if(uSectionVertexCount < 3)
		{
			//assert(!"at least need 3 points to make a face");
			continue;
		}

		CutHelper::FixCrossSectionPoints(*pCurrentSection);

		//check the order of points
		bool bConvex = CutHelper::FixPointsOrder(*pCurrentSection, pHitInfo->sPlane.n);

		//triangulation
		processedCollection.clear();
		if(bConvex)
			CutHelper::TriangulationConvex(*pCurrentSection,processedCollection);
		else
			CutHelper::TriangulationEarClipping(*pCurrentSection,processedCollection, pHitInfo->sPlane.n);

		uSectionVertexCount = processedCollection.size();
		if(uSectionVertexCount < 3)
		{
			assert(!"wrong");
			return;
		}

		//create vertices data 
		uint32 pStartVertexIndex = vertices.size();	
		for(uint32 j = 0; j < uSectionVertexCount; ++j)
		{
			vertices.push_back(*processedCollection[j]);
			vertices.push_back(*processedCollection[j]);
		}

		//check the direction
		bool bFacePlane = false;//if current order is already 
		Vector3 vAB = *processedCollection[1] - *processedCollection[0];
		Vector3 vBC = *processedCollection[2] - *processedCollection[1];
		Vector3 vCross = MFD_Cross(vAB,vBC);
		fDot = MFD_Dot(vCross, pHitInfo->sPlane.n);
		if(fDot < 0)
		{
			bFacePlane = true;
		}

		//create new faces	
		uSectionVertexCount *= 2;
		uint32 faceIndex = 0;
		for(uint32 j = 0; j < uSectionVertexCount; j += 6)
		{
			tempFace1.v[0] = pStartVertexIndex + j;	
			tempFace1.v[1] = pStartVertexIndex + j + 2;
			tempFace1.v[2] = pStartVertexIndex + j + 4;
			tempFace2.v[0] = tempFace1.v[0] + 1;
			tempFace2.v[1] = tempFace1.v[2] + 1;
			tempFace2.v[2] = tempFace1.v[1] + 1;

			faceIndex = faces.size();
			faces.push_back(tempFace1);
			faces.push_back(tempFace2);

			if(bFacePlane)
			{
				pPosFaces->push_back(&faces[faceIndex]);
				pNegFaces->push_back(&faces[faceIndex+1]);
			}
			else
			{
				pPosFaces->push_back(&faces[faceIndex + 1]);
				pNegFaces->push_back(&faces[faceIndex]);
			}
		}
	}//	for(uint32 i = 0; i < uSectionCount; ++i)
}

GameObject*
CutHelper::CreateNewObjectFromPieces(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPieceFaces)
{
	Model* pModel = pObject->GetModelComponent();
	Mesh* pMesh = pModel->GetMesh();
	std::vector<Vector3> &vertices = *pModel->GetVerticesCache();
	std::vector<Face> &faces = *pMesh->GetFaces();
	std::vector<Vector3> &normals = *pMesh->GetNormals();
	std::vector<Vector2> &uvs = *pMesh->GetUVs();

	std::vector<Vector3> *newVertices = nullptr;
	std::vector<Vector3> *newNormals = nullptr;
	std::vector<Vector2> *newUVs = nullptr;
	std::vector<VertexIndex> *newIndices= nullptr;
	RefCountPtr<Mesh> pNewMesh = nullptr;
	Model* pNewModel = nullptr;
	uint32 uNewFaceCount = 0;
	uint32 uNewVertexCount = 0;
	Vector3 vMiddlePoint;
	char pszName[NAME_CBUFFER_SIZE];

	uNewFaceCount = pPieceFaces->size();
	uNewVertexCount = uNewFaceCount * 3;
	pNewMesh = new Mesh();
	newVertices = pNewMesh->GetVertices();
	newNormals = pNewMesh->GetNormals();
	newUVs = pNewMesh->GetUVs();
	newIndices = pNewMesh->GetIndices();
	newVertices->reserve(uNewVertexCount);
	newNormals->reserve(uNewVertexCount);
	newUVs->reserve(uNewVertexCount);
	newIndices->reserve(uNewVertexCount);

	Matrix44 mWorldMat = *pObject->GetTranformMat();
	Vector3 vNormal;
	uint32 uNormalIndex = 0;
	Face* pFace = nullptr;

	uint32 index = 0;
	std::list<Face*>::iterator it;
	for(it = pPieceFaces->begin(); it != pPieceFaces->end(); ++it)
	{
		pFace = *it;
		newVertices->push_back(vertices[pFace->v[0]]);
		newVertices->push_back(vertices[pFace->v[1]]);
		newVertices->push_back(vertices[pFace->v[2]]);

		//normal need to be in world space
		vNormal = MFD_Normalize(Vector3(mWorldMat * Vector4(normals[pFace->v[0]],0.0f)));
		newNormals->push_back(vNormal);

		vNormal = MFD_Normalize(Vector3(mWorldMat * Vector4(normals[pFace->v[1]],0.0f)));
		newNormals->push_back(vNormal);

		vNormal = MFD_Normalize(Vector3(mWorldMat * Vector4(normals[pFace->v[2]],0.0f)));
		newNormals->push_back(vNormal);


		newUVs->push_back(uvs[pFace->v[0]]);
		newUVs->push_back(uvs[pFace->v[1]]);
		newUVs->push_back(uvs[pFace->v[2]]);

		newIndices->push_back(index*3);
		newIndices->push_back(index*3 + 1);
		newIndices->push_back(index*3 + 2);
		++index;
	}
	pNewMesh->CreatePhysicsGeometryData();
	vMiddlePoint = pNewMesh->ResetVerticesOnMiddlePoint();
	pNewMesh->CreateBuffers();

	sprintf_s(&pszName[0],NAME_CBUFFER_SIZE,"CutPiece_%d",CutHelper::s_uPieceCount);
	GameObject* pNewObject = new GameObject(pszName);
	pNewObject->SetPos(vMiddlePoint);
	pNewObject->RefreshTranformMat();
	pNewModel = pNewObject->CreateModelComponent(nullptr);
	pNewModel->SetMesh(pNewMesh);
	pNewModel->SetMaterialIndex(pModel->GetMaterialIndex());

	++CutHelper::s_uPieceCount;

	return pNewObject;
}

void
CutHelper::CreateNewObjectFromPieces_Collider(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPieceFaces, GameObject* pObjToAttach)
{
	Collider* pColldier = pObject->GetColliderComponent();
	CollisionMesh* pCollisionMesh = pColldier->GetCollisionMesh();
	std::vector<Vector3> &vertices =  *pColldier->GetVerticesCache();

	std::vector<Vector3> *newVertices = nullptr;
	std::vector<VertexIndex> *newIndices= nullptr;
	RefCountPtr<CollisionMesh> pNewMesh = nullptr;
	Collider* pNewCollider = nullptr;
	uint32 uNewFaceCount = 0;
	uint32 uNewVertexCount = 0;
	Vector3 vMiddlePoint;

	uNewFaceCount = pPieceFaces->size();
	uNewVertexCount = uNewFaceCount * 3;
	pNewMesh = new CollisionMesh();
	newVertices = pNewMesh->GetVertices();
	newIndices = pNewMesh->GetIndices();
	newVertices->reserve(uNewVertexCount);
	newIndices->reserve(uNewVertexCount);

	Face* pFace = nullptr;
	uint32 uFaceIndex = 0;
	std::list<Face*>::iterator it;
	for(it = pPieceFaces->begin(); it != pPieceFaces->end(); ++it)
	{
		pFace = (*it);
		
		//not put anything is a line into collision mesh
		//todo:why the line shape triangle appear
		if(vertices[pFace->v[0]] != vertices[pFace->v[1]]
			&& vertices[pFace->v[0]] != vertices[pFace->v[1]]
			&& vertices[pFace->v[0]] != vertices[pFace->v[1]])
		{
			uFaceIndex = newVertices->size();
			newVertices->push_back(vertices[pFace->v[0]]);
			newVertices->push_back(vertices[pFace->v[1]]);
			newVertices->push_back(vertices[pFace->v[2]]);

			newIndices->push_back(uFaceIndex);
			newIndices->push_back(uFaceIndex + 1);
			newIndices->push_back(uFaceIndex + 2);
		}
	}
	pNewMesh->CreatePhysicsGeometryData();
	vMiddlePoint = pNewMesh->ResetVerticesOnMiddlePoint();

	pNewCollider = pObjToAttach->CreateColliderComponent(nullptr);
	pNewCollider->SetMesh(pNewMesh);
	

	++CutHelper::s_uColliderPieceCount;
}

 void 
CutHelper::CreateNewObjectFromPieces_Collider_complex(GameObject* pObject, PlaneHitInfo* pHitInfo,std::list<Face*>* pPieceFaces, std::list<GameObject*>* pObjects)
 {
	Collider* pColldier = pObject->GetColliderComponent();
	CollisionMesh* pCollisionMesh = pColldier->GetCollisionMesh();
	std::vector<Vector3> &vertices =  *pColldier->GetVerticesCache();

	std::vector<Vector3> *newVertices = nullptr;
	std::vector<VertexIndex> *newIndices= nullptr;
	RefCountPtr<CollisionMesh> pNewMesh = nullptr;
	Collider* pNewCollider = nullptr;
	uint32 uNewFaceCount = 0;
	uint32 uNewVertexCount = 0;
	Vector3 vMiddlePoint;

	uNewFaceCount = pPieceFaces->size();
	uNewVertexCount = uNewFaceCount * 3;
	pNewMesh = new CollisionMesh();
	newVertices = pNewMesh->GetVertices();
	newIndices = pNewMesh->GetIndices();
	newVertices->reserve(uNewVertexCount);
	newIndices->reserve(uNewVertexCount);

	Face* pFace = nullptr;
	uint32 uFaceIndex = 0;
	std::list<Face*>::iterator it;
	for(it = pPieceFaces->begin(); it != pPieceFaces->end(); ++it)
	{
		pFace = (*it);
		
		//not put anything is a line into collision mesh
		//todo:why the line shape triangle appear
		if(vertices[pFace->v[0]] != vertices[pFace->v[1]]
			&& vertices[pFace->v[0]] != vertices[pFace->v[1]]
			&& vertices[pFace->v[0]] != vertices[pFace->v[1]])
		{
			uFaceIndex = newVertices->size();
			newVertices->push_back(vertices[pFace->v[0]]);
			newVertices->push_back(vertices[pFace->v[1]]);
			newVertices->push_back(vertices[pFace->v[2]]);

			newIndices->push_back(uFaceIndex);
			newIndices->push_back(uFaceIndex + 1);
			newIndices->push_back(uFaceIndex + 2);
		}
	}
	pNewMesh->CreatePhysicsGeometryData();
	vMiddlePoint = pNewMesh->ResetVerticesOnMiddlePoint();

	//find the closest object to attach
	GameObject* pObjToAttach = nullptr;
	float32 fMinDistance = FLT_MAX;
	float32 fDistance2 = 0;
	std::list<GameObject*>::iterator objIT;
	for(objIT = pObjects->begin(); objIT != pObjects->end(); ++objIT)
	{
		if((*objIT)->GetColliderComponent() != nullptr)
			continue;

		fDistance2 = MFD_Length2(vMiddlePoint - (*objIT)->GetWorldPos());
		if(fDistance2 < fMinDistance)
		{
			fMinDistance = fDistance2;
			pObjToAttach = *objIT;
		}
	}

	if(pObjToAttach != nullptr)
	{
		pNewCollider = pObjToAttach->CreateColliderComponent(nullptr);
		pNewCollider->SetMesh(pNewMesh);
	}
	else
	{
		pNewMesh = nullptr; 
	}
	

	++CutHelper::s_uColliderPieceCount;
 }

void 
CutHelper::CutObject(GameObject* pObject, PlaneHitInfo* pHitInfo, std::list<GameObject*> &posSideObjects, std::list<GameObject*> &negSideObjects)
{
	Model* pModel = pObject->GetModelComponent();
	Mesh* pMesh = pModel->GetMesh();
	Collider* pCollider = pObject->GetColliderComponent();
	std::vector<Vector3> &vertices = *pModel->GetVerticesCache();
	std::vector<Face> &faces = *pMesh->GetFaces();
	std::vector<Vector3> &normals = *pMesh->GetNormals();
	std::vector<Vector2> &uvs = *pMesh->GetUVs();

	uint32 uOriginFaceCount = faces.size();
	uint32 uVerticesReserveNum = vertices.size() + pHitInfo->uTouchPostionCount*3;
	//reserve the memory, so the memory address won't change later
	vertices.reserve(uVerticesReserveNum);
	normals.reserve(uVerticesReserveNum);
	uvs.reserve(uVerticesReserveNum);
	faces.reserve(faces.size() + pHitInfo->uObjectFacesCount*3 +  pHitInfo->uTouchPostionCount);

	std::list<Face*> posFaces;//temp collection of face
	std::list<Face*> negFaces;//temp collection of face

	//process untouched faces
	Face* pFace = nullptr;
	Vector3* pSamplePos = nullptr;
	for(uint32 i = 0; i < uOriginFaceCount; ++i)
	{
		pFace = &faces[i];
		if(pFace->bTouched == false)
		{
			if(CollisionHelper::DistPointPlane(vertices[pFace->v[0]], pHitInfo->sPlane) > 0)
			{
				posFaces.push_back(pFace);
			}
			else
			{
				negFaces.push_back(pFace);
			}
		}
	}

	//process touched faces(to cut)
	CutFaces(pObject, pHitInfo, &posFaces, &negFaces);

	//to create the cross section
	CreateCrossSections(pObject, pHitInfo, &posFaces, &negFaces, 0.25f, pCollider->GetRange());

	//create new objects
	GameObject* pOut1 = CreateNewObjectFromPieces(pObject, pHitInfo, &posFaces);
	GameObject* pOut2 = CreateNewObjectFromPieces(pObject, pHitInfo, &negFaces);

	//now cut collider mesh, the hit infomation will be overwritten
	if(CollisionManager::Instance()->IsPlaneColliderCollided(pObject, *pHitInfo))
	{
		CutCollider(pObject,pHitInfo,pOut1,pOut2);
	}
	
	RigidBody* pRigidBody = pObject->GetRigidBodyComponent();
	float32 fOriginMass = pRigidBody->GetMass();
	if(pRigidBody != nullptr)
	{
		RigidBody* pNewBody1 = pRigidBody->CreateCharacteristicClone();
		RigidBody* pNewBody2 = pRigidBody->CreateCharacteristicClone();

		pOut1->SetRigidBodyComponent(pNewBody1);
		pOut2->SetRigidBodyComponent(pNewBody2);

		pNewBody1->SetMomentInertiaType(InertiaType::Box);
		pNewBody2->SetMomentInertiaType(InertiaType::Box);

		float32 fRoughVolume1 =  pOut1->GetColliderComponent()->GetCollisionMesh()->GetRoughVolume();
		float32 fRoughVolume2 =  pOut2->GetColliderComponent()->GetCollisionMesh()->GetRoughVolume();
		pNewBody1->SetMass( fRoughVolume1 / (fRoughVolume1 + fRoughVolume2) * fOriginMass);
		pNewBody2->SetMass( fRoughVolume2 / (fRoughVolume1 + fRoughVolume2) * fOriginMass);
	}

	posSideObjects.push_back(pOut1);
	negSideObjects.push_back(pOut2);
}

 void 
CutHelper::CutCollider(GameObject* pObject, PlaneHitInfo* pHitInfo, GameObject* pNewObj1, GameObject* pNewObj2)
 {
	Collider* pColldier = pObject->GetColliderComponent();
	CollisionMesh* pCollisionMesh = pColldier->GetCollisionMesh();
	std::vector<Vector3> &vertices =  *pColldier->GetVerticesCache();
	std::vector<Face> &faces = *pCollisionMesh->GetFaces();
			
	uint32 uOriginFaceCount = faces.size();
	uint32 uVerticesReserveNum = vertices.size() + pHitInfo->uTouchPostionCount*3;

	//reserve the memory, so the memory address won't change
	vertices.reserve(uVerticesReserveNum);
	faces.reserve(faces.size() + pHitInfo->uObjectFacesCount*3 +  pHitInfo->uTouchPostionCount);
	
	std::list<Face*> posFaces;//temp collection of face
	std::list<Face*> negFaces;//temp collection of face
	
	//process untouched faces
	Face* pFace = nullptr;
	Vector3* pSamplePos = nullptr;
	for(uint32 i = 0; i < uOriginFaceCount; ++i)
	{
		pFace = &faces[i];
		if(pFace->bTouched == false)
		{
			if(CollisionHelper::DistPointPlane(vertices[pFace->v[0]], pHitInfo->sPlane) > 0)
			{
				posFaces.push_back(pFace);
			}
			else
			{
				negFaces.push_back(pFace);
			}
		}
	}

	//process touched faces(to cut)
	CutFaces_Collider(pObject, pHitInfo, &posFaces, &negFaces);

	//to create the cross section
	CreateCrossSections_Collider(pObject, pHitInfo, &posFaces, &negFaces);

	CreateNewObjectFromPieces_Collider(pObject, pHitInfo, &posFaces, pNewObj1);
	CreateNewObjectFromPieces_Collider(pObject, pHitInfo, &negFaces, pNewObj2);
 }

 void 
CutHelper::FixCrossSectionPoints(std::list<Vector3*> &points)
 {
	//remove repeat points
	std::list<Vector3*>::iterator it1 = points.begin();
	std::list<Vector3*>::iterator it2 = points.begin();
	std::advance(it2,1);
	for(; it2 != points.end();)
	{
		if((**it1) == (**it2))
		{
			it2 = points.erase(it2);
		}
		else
		{
			++it1;
			++it2;
		}
	}
	it2 = points.begin();
	if((**it1) == (**it2))
	{
		//now the it1 should be the last point
		points.erase(it1);
	}

	//remove middle points of a line
	uint32 uSectionVertexCount = points.size();
	it1 = points.begin();
	it2 = points.begin();
	std::advance(it2,1);
	Vector3 vCurrentDir; 
	Vector3 vLastDir = MFD_Normalize(*points.front() - *points.back());//now this won't have a chance to crash
	float32 fDirDot = 0;
	for(; it2 != points.end();)
	{
		vCurrentDir = MFD_Normalize(**it2 - **it1);
		fDirDot = MFD_Dot(vLastDir,vCurrentDir);
		if(fDirDot > 0.999f)
		{
			it1 = points.erase(it1);
			++it2;
		}
		else
		{
			++it1;
			++it2;
		}
		vLastDir = vCurrentDir;
	}
	vCurrentDir = MFD_Normalize(*points.front() - *points.back());
	fDirDot = MFD_Dot(vLastDir,vCurrentDir);
	if(fDirDot > 0.999f)
	{
		//now the it1 should be the last point
		points.erase(it1);
	}
 }

 bool  
CutHelper::FixPointsOrder(std::list<Vector3*> &points, const Vector3& vRefNormal)
 {
	uint32 uSectionVertexCount = points.size();
	std::list<Vector3*>::iterator it1 = points.begin();
	std::list<Vector3*>::iterator it2 = points.begin();
	std::advance(it2,1);
	Vector3 vCurrentDir; 
	Vector3 vLastDir = MFD_Normalize(*points.front() - *points.back());
	float32 fDirDot = 0;
	Vector3 vDirCross;
	uint32 uPosDirCount = 0;
	uint32 uNegDirCount = 0;
	float32 fSum = 0;
	for(uint32 j = 0; j < uSectionVertexCount - 1; ++j)
	{
		if(j != 0)
		{
			std::advance(it1,1);
			std::advance(it2,1);
		}

		vCurrentDir = MFD_Normalize(**it2 - **it1);
		vDirCross = MFD_Cross(vLastDir,vCurrentDir);
		fDirDot = MFD_Dot(vRefNormal, vDirCross);
		vLastDir = vCurrentDir;

		if(fDirDot > 0)
			++uPosDirCount;
		else if(fDirDot < 0)
			++uNegDirCount;

		//Debug::LogValue("cross",vDirCross);
		//Debug::LogValue("dot",fDirDot);
		fSum += fDirDot;

		if(j == uSectionVertexCount - 2)
		{
			it1 = points.begin();
			vCurrentDir = MFD_Normalize( **it1 - **it2);
			vDirCross = MFD_Cross(vLastDir,vCurrentDir);
			fDirDot = MFD_Dot(vRefNormal, vDirCross);
			vLastDir = vCurrentDir;

			if(fDirDot > 0)
				++uPosDirCount;
			else if(fDirDot < 0)
				++uNegDirCount;

			//Debug::LogValue("cross",vDirCross);
			//Debug::LogValue("dot",fDirDot);
			fSum += fDirDot;
		}
	}
	//Debug::LogValue("pos count",uPosDirCount);
	//Debug::LogValue("neg count",uNegDirCount);
	//Debug::LogValue("sum",fSum);
	//Debug::Log("-------------------\n");

	if(uPosDirCount == uNegDirCount)
	{
		if(fSum > 0)//not really sure is this correct or not, but it seems correct
		{
			points.reverse();
		}
	}
	else if(uPosDirCount < uNegDirCount)
	{
		//points not in anti-clock order from ref normal direction, so reverse them
		points.reverse();
	}

	return (uPosDirCount == 0 || uNegDirCount == 0);
 }

 void 
 CutHelper::TriangulationConvex(std::list<Vector3*> &inCollection, std::vector<Vector3*> &outCollection)
 {
	 outCollection.reserve((inCollection.size() - 2) * 3);

	 Vector3* pFixedVertex = inCollection.front();

	 std::list<Vector3*>::iterator it1 = inCollection.begin();
	 std::list<Vector3*>::iterator it2 = inCollection.begin();
	 std::advance(it2,1);
	 uint32 uVertexCount = inCollection.size() - 1;
	 for(uint32 i = 1; i < uVertexCount; ++i)
	 {
		 ++it1;
		 ++it2;
		 outCollection.push_back(pFixedVertex);
		 outCollection.push_back(*it1);
		 outCollection.push_back(*it2);
	 }
 }

 void 
CutHelper::TriangulationEarClipping(std::list<Vector3*> &inCollection, std::vector<Vector3*> &outCollection, const Vector3& vRefNormal)
 {
	 outCollection.reserve((inCollection.size() - 2) * 3);
	 std::list<Vector3*> clone(inCollection.begin(),inCollection.end());

	 std::list<Vector3*>::iterator it,it1,it2,it3;
	 Vector3 v12, v23, vCross, uvw;
	 float32 fDot = 0;
	 uint32 uVertexCount = 0;
	 uint32 uMainIndex = 0;
	 uint32 uEarIndex = 0;
	 uint32 uAdvance = 0;
	 uint32 i = 0;
	 bool bHit = false;
	 bool bLastCheck = false;
	 uint32 uMaxLoopCount = inCollection.size() * 5;
	 uint32 uCurrentCount = 0;
	 while(true)
	 {
		if(++uCurrentCount > uMaxLoopCount)
			break;

		it1 = inCollection.begin();
		std::advance(it1,uMainIndex);
		while (true)
		{
			uVertexCount = inCollection.size();
			if(uVertexCount == 3)
				bLastCheck = true;

			//the list will change, so need to recalculate the iterator position every time
			it2 = inCollection.begin();
			uAdvance = uMainIndex + 1;
			if(uAdvance >= uVertexCount)
				uAdvance = uAdvance % uVertexCount;
			std::advance(it2,uAdvance);
			uEarIndex = uAdvance;

			it3 = inCollection.begin();
			uAdvance = uMainIndex + 2;
			if(uAdvance >= uVertexCount)
				uAdvance = uAdvance % uVertexCount;
			std::advance(it3,uAdvance);

			if(it2 == it1 || it3 == it1)
			{
				assert(!"not really think this will happen, but just in case");
				break;
			}

			v12 = MFD_Normalize(**it2 - **it1);
			v23 = MFD_Normalize(**it3 - **it2);
			vCross = MFD_Cross(v12,v23);

			if(vCross.x == 0 && vCross.y == 0 && vCross.z == 0)//in a stright line
				break;

			fDot = MFD_Dot(vRefNormal, vCross);

			if(fDot < 0)//face wrong direction
				break;

			bHit = false;
			//for(it = inCollection.begin(); it != inCollection.end(); ++it)//have not seen anything wrong, but don't feel this is correct, should check all origin points
			for(it = clone.begin(); it != clone.end(); ++it)
			{
				if(*it == *it1 || *it == *it2 || *it == *it3)
					continue;

				//check if the point is inside the triangle, by check barycentric coord value
				CollisionHelper::Barycentric(**it1, **it2, **it3, **it, uvw);
				if(uvw.x >= 0.0f && uvw.y >= 0.0f && uvw.z >= 0.0f
					&& uvw.x <= 1.0f && uvw.y <= 1.0f && uvw.z <= 1.0f)
				{
					bHit = true;
					break;
				}
			}

			if(bHit)
				break;
			   
			outCollection.push_back(*it1);
			outCollection.push_back(*it2);
			outCollection.push_back(*it3);
			inCollection.erase(it2);

			if(uEarIndex < uMainIndex)
			{
				--uMainIndex;
			}

			if(bLastCheck)
				break;
		 }//while2

		 if(bLastCheck)
			break;

		 ++uMainIndex;
		 if(uMainIndex >= inCollection.size())
			 uMainIndex = 0;
	 }//while1

	 //Debug::LogValue("Remaining points",(uint32)inCollection.size());
 }

 void 
CutHelper::RemoveRepeatPairs(std::vector<Vector3>& points,uint32& uPointCount)
 {
	 if(uPointCount < 5)
	 {
		 return;
	 }

	 uint32 uPointToCheck1 = uPointCount - 3;
	 uint32 uPointToCheck2 = uPointCount - 1;
	 size_t uTypeSize = sizeof(Vector3);
	 for(uint32 i = 0; i < uPointToCheck1; i+=2)
	 {
		  for(uint32 j = i+2; j < uPointToCheck2; j+=2)
		  {
			  if((points[i] == points[j + 1] && points[i+1] == points[j])
				|| (points[i] == points[j] && points[i+1] == points[j+1]))
				{
					if(j < uPointCount - 2)
					{
						memcpy(&points[j], &points[j + 2], uTypeSize * (uPointCount - j - 2));
					}
					j-=2;
					uPointCount -= 2;
					uPointToCheck1 -= 2;
					uPointToCheck2 -= 2;
				}
		  }
	 }
 }

void 
CutHelper::SortOrder(PlaneHitInfo* pHitInfo,std::vector<std::list<Vector3*>> &outResult)
{
	CutHelper::RemoveRepeatPairs(pHitInfo->touchedPostions, pHitInfo->uTouchPostionCount);

	std::vector<bool> checkFlags;
	checkFlags.reserve(pHitInfo->uTouchPostionCount);
	for(uint32 i = 0; i < pHitInfo->uTouchPostionCount; ++i)
	{
		checkFlags.push_back(false);
	}

	//sort touched positions into order first, only record the vertices at the two end of an edge
	Vector3* pPos1 = nullptr;
	Vector3* pPos2 = nullptr;
	Vector3* pStartPos = nullptr;
	Vector3* pLastEnd = nullptr;
	Vector3 vCurrentDir,vTargetDir;
	int32 uLastUncheckIndex = -1;
	bool bConnectFound = false;
	float32 fDot = 0;
	uint32 uLastIndex_turn = 0;
	std::list<Vector3*>* pCurrentSection = nullptr;
	std::list<Vector3*>::iterator it1,it2;
	for(uint32 i = 0; i < pHitInfo->uTouchPostionCount; i+=2)
	{
		if(checkFlags[i])
			continue;

		//create the seed of new section
		outResult.push_back(std::list<Vector3*>());
		pCurrentSection = &outResult[outResult.size() - 1];

		pStartPos = &pHitInfo->touchedPostions[i];
		pLastEnd = &pHitInfo->touchedPostions[i+1];
		checkFlags[i] = checkFlags[i+1] = true;
		vCurrentDir = MFD_Normalize(*pLastEnd - *pStartPos);

		pCurrentSection->push_back(pStartPos);
		pCurrentSection->push_back(pLastEnd);
			
		uLastUncheckIndex = -1;
		for(uint32 j = i + 2; j < pHitInfo->uTouchPostionCount; j += 2)
		{
			if(checkFlags[j])
				continue;
			else if(uLastUncheckIndex < 0)
				uLastUncheckIndex = j;

			pPos1 = &pHitInfo->touchedPostions[j];
			pPos2 = &pHitInfo->touchedPostions[j+1];

			bConnectFound = false;
			if(MathHelper::AlmostEqual(*pPos1,*pLastEnd))
			{
				bConnectFound = true;
			}
			else if(MathHelper::AlmostEqual(*pPos2, *pLastEnd))
			{
				pPos1 = pPos2;
				pPos2 = &pHitInfo->touchedPostions[j];
				bConnectFound = true;
			}

			if(bConnectFound)
			{
				pLastEnd = pPos2;
				checkFlags[j] = checkFlags[j+1] = true;
				j = uLastUncheckIndex - 2;
				uLastUncheckIndex = -1;

				vTargetDir = MFD_Normalize(*pPos2 - *pPos1);
				fDot = MFD_Dot(vCurrentDir, vTargetDir);

				if(fDot > 0.999f)
				{
					if(MathHelper::AlmostEqual(*pLastEnd,*pStartPos))
					{
						//reach the end, so jump out
						uint32 uStartIndexToDelete = uLastIndex_turn;
						uint32 uEndIndexToDelete = pCurrentSection->size() - 1;
						it1 = pCurrentSection->begin();
						it2 = pCurrentSection->begin();
						std::advance(it1,uStartIndexToDelete);
						std::advance(it2,uEndIndexToDelete);

						if(uStartIndexToDelete == uEndIndexToDelete)
							pCurrentSection->erase(it1);
						else
							pCurrentSection->erase(it1, it2);

						break;
					}
					else
					{
						//in same direction, so just change the end pos
						pCurrentSection->pop_back();
						pCurrentSection->push_back(pLastEnd);
					}

				}
				else
				{
					if(MathHelper::AlmostEqual(*pLastEnd, *pStartPos))
					{
						//reach the end, so jump out
						break;
					}

					vCurrentDir = vTargetDir;
					uLastIndex_turn = pCurrentSection->size();
					pCurrentSection->push_back(pPos2);
				}
			}//bConnectFound
		}//j			
	}//i
}

void 
CutHelper::CutObject_complex(GameObject* pObject, PlaneHitInfo* pHitInfo, std::list<GameObject*> &posSideObjects, std::list<GameObject*> &negSideObjects)
{
	Model* pModel = pObject->GetModelComponent();
	Mesh* pMesh = pModel->GetMesh();
	Collider* pCollider = pObject->GetColliderComponent();
	std::vector<Vector3> &vertices = *pModel->GetVerticesCache();
	std::vector<Face> &faces = *pMesh->GetFaces();
	std::vector<Vector3> &normals = *pMesh->GetNormals();
	std::vector<Vector2> &uvs = *pMesh->GetUVs();

	uint32 uOriginFaceCount = faces.size();
	uint32 uVerticesReserveNum = vertices.size() + pHitInfo->uTouchPostionCount*3;
	//reserve the memory, so the memory address won't change later
	vertices.reserve(uVerticesReserveNum);
	normals.reserve(uVerticesReserveNum);
	uvs.reserve(uVerticesReserveNum);
	faces.reserve(faces.size() + pHitInfo->uObjectFacesCount*3 +  pHitInfo->uTouchPostionCount);

	//process untouched faces
	std::list<Face*> untouchedPosFaces;
	std::list<Face*> untouchedNegFaces;
	Face* pFace = nullptr;
	Vector3* pSamplePos = nullptr;
	float32 fDistance = 0;
	for(uint32 i = 0; i < uOriginFaceCount; ++i)
	{
		pFace = &faces[i];
		if(pFace->bTouched == false)
		{
			fDistance = CollisionHelper::DistPointPlane(vertices[pFace->v[0]], pHitInfo->sPlane);
			if(fDistance == 0)
			{
				fDistance = CollisionHelper::DistPointPlane(vertices[pFace->v[1]], pHitInfo->sPlane);
				if(fDistance == 0)
				{
					fDistance = CollisionHelper::DistPointPlane(vertices[pFace->v[2]], pHitInfo->sPlane);
					if(fDistance == 0)
					{
						assert(!"this triangle should be touched");
					}
				}
			}

			if(fDistance > 0)
			{
				untouchedPosFaces.push_back(pFace);
			}
			else
			{
				untouchedNegFaces.push_back(pFace);
			}
		}
	}

	//calculate the separated objects on each side
	std::list<TempObject*> posObjects;
	std::list<TempObject*> negObjects;
	CutHelper::CalculateSeparatedObjects(untouchedPosFaces, vertices, posObjects);
	CutHelper::CalculateSeparatedObjects(untouchedNegFaces, vertices, negObjects);

	//cut faces
	uint32 uMaxFaceCount = pHitInfo->uObjectFacesCount * 3;//how much faces the temp collection might need to hold
	std::list<Face*> posFaces;//temp collection of face
	std::list<Face*> negFaces;//temp collection of face
	CutHelper::CutFaces(pObject, pHitInfo, &posFaces, &negFaces);

	//attach new faces to existing objects
	//some new faces might connect to other new face, rather than the seed object's faces, so multiple tests are needed
	while(CutHelper::AttachToObject(posObjects, vertices, posFaces))
	{}
	while(CutHelper::AttachToObject(negObjects, vertices, negFaces))
	{}

	//if there are some remaining faces, group them into new temp objects
	if(posFaces.size() > 0)
	{
		CutHelper::CalculateSeparatedObjects(posFaces, vertices, posObjects);
	}
	if(negFaces.size() > 0)
	{
		CutHelper::CalculateSeparatedObjects(negFaces, vertices, negObjects);
	}

	//create cross section faces
	CreateCrossSections(pObject, pHitInfo, &posFaces, &negFaces, 0.25f, pCollider->GetRange());

	//attach new faces to existing objects
	CutHelper::AttachToObject(posObjects, vertices, posFaces);
	CutHelper::AttachToObject(negObjects, vertices, negFaces);

	//create gameobject from temp object's data
	GameObject* pNewObject = nullptr;
	std::list<TempObject*>::iterator it;
	for(it = posObjects.begin(); it != posObjects.end(); ++it)
	{
		pNewObject = CreateNewObjectFromPieces(pObject, pHitInfo, &(*it)->faces);
		posSideObjects.push_back(pNewObject);
	}

	for(it = negObjects.begin(); it != negObjects.end(); ++it)
	{
		pNewObject = CreateNewObjectFromPieces(pObject, pHitInfo, &(*it)->faces);
		negSideObjects.push_back(pNewObject);
	}

	//now cut collider mesh, the hit infomation will be overwritten
	if(CollisionManager::Instance()->IsPlaneColliderCollided(pObject, *pHitInfo))
	{
		CutCollider_complex(pObject,pHitInfo,posSideObjects,negSideObjects);
	}

	//recreate rigid body
	RigidBody* pRigidBody = pObject->GetRigidBodyComponent();
	float32 fOriginMass = pRigidBody->GetMass();
	float32 fTotalVolume = 0;
	std::list<GameObject*>::iterator objIT;
	for(objIT = posSideObjects.begin(); objIT != posSideObjects.end(); ++objIT)
	{
		fTotalVolume += (*objIT)->GetColliderComponent()->GetCollisionMesh()->GetRoughVolume();
		(*objIT)->SetRigidBodyComponent(pRigidBody->CreateCharacteristicClone());
		(*objIT)->GetRigidBodyComponent()->SetMomentInertiaType(InertiaType::Box);
	}
	for(objIT = negSideObjects.begin(); objIT != negSideObjects.end(); ++objIT)
	{
		fTotalVolume += (*objIT)->GetColliderComponent()->GetCollisionMesh()->GetRoughVolume();
		(*objIT)->SetRigidBodyComponent(pRigidBody->CreateCharacteristicClone());
		(*objIT)->GetRigidBodyComponent()->SetMomentInertiaType(InertiaType::Box);
	}

	float32 fRoughVolume = 0;
	for(objIT = posSideObjects.begin(); objIT != posSideObjects.end(); ++objIT)
	{
		fRoughVolume = (*objIT)->GetColliderComponent()->GetCollisionMesh()->GetRoughVolume();
		(*objIT)->GetRigidBodyComponent()->SetMass(fRoughVolume / fTotalVolume * fOriginMass);
	}
	for(objIT = negSideObjects.begin(); objIT != negSideObjects.end(); ++objIT)
	{
		fRoughVolume = (*objIT)->GetColliderComponent()->GetCollisionMesh()->GetRoughVolume();
		(*objIT)->GetRigidBodyComponent()->SetMass(fRoughVolume / fTotalVolume * fOriginMass);
	}
}

void 
CutHelper::CutCollider_complex(GameObject* pObject, PlaneHitInfo* pHitInfo, std::list<GameObject*> &posSideObjects, std::list<GameObject*> &negSideObjects)
{
	Collider* pColldier = pObject->GetColliderComponent();
	CollisionMesh* pCollisionMesh = pColldier->GetCollisionMesh();
	std::vector<Vector3> &vertices =  *pColldier->GetVerticesCache();
	std::vector<Face> &faces = *pCollisionMesh->GetFaces();
			
	uint32 uOriginFaceCount = faces.size();
	uint32 uVerticesReserveNum = vertices.size() + pHitInfo->uTouchPostionCount*3;

	//reserve the memory, so the memory address won't change
	vertices.reserve(uVerticesReserveNum);
	faces.reserve(faces.size() + pHitInfo->uObjectFacesCount*3 +  pHitInfo->uTouchPostionCount);

	//process untouched faces
	std::list<Face*> untouchedPosFaces;
	std::list<Face*> untouchedNegFaces;
	Face* pFace = nullptr;
	Vector3* pSamplePos = nullptr;
	for(uint32 i = 0; i < uOriginFaceCount; ++i)
	{
		pFace = &faces[i];
		if(pFace->bTouched == false)
		{
			if(CollisionHelper::DistPointPlane(vertices[pFace->v[0]], pHitInfo->sPlane) > 0)
			{
				untouchedPosFaces.push_back(pFace);
			}
			else
			{
				untouchedNegFaces.push_back(pFace);
			}
		}
	}

	//calculate the separated objects on each side
	std::list<TempObject*> posObjects;
	std::list<TempObject*> negObjects;
	CutHelper::CalculateSeparatedObjects(untouchedPosFaces, vertices, posObjects);
	CutHelper::CalculateSeparatedObjects(untouchedNegFaces, vertices, negObjects);

	//cut faces
	uint32 uMaxFaceCount = pHitInfo->uObjectFacesCount * 3;//how much faces the temp collection might need to hold
	std::list<Face*> posFaces;//temp collection of face
	std::list<Face*> negFaces;//temp collection of face
	CutHelper::CutFaces_Collider(pObject, pHitInfo, &posFaces, &negFaces);

	//attach new faces to existing objects
	//some new faces might connect to other new face, rather than the seed object's faces, so multiple tests are needed
	while(CutHelper::AttachToObject(posObjects, vertices, posFaces))
	{}
	while(CutHelper::AttachToObject(negObjects, vertices, negFaces))
	{}

	//if there are some remaining faces, group them into new temp objects
	if(posFaces.size() > 0)
	{
		CutHelper::CalculateSeparatedObjects(posFaces, vertices, posObjects);
	}
	if(negFaces.size() > 0)
	{
		CutHelper::CalculateSeparatedObjects(negFaces, vertices, negObjects);
	}

	//create cross section faces
	CreateCrossSections_Collider(pObject, pHitInfo, &posFaces, &negFaces);

	//attach new faces to existing objects
	CutHelper::AttachToObject(posObjects, vertices, posFaces);
	CutHelper::AttachToObject(negObjects, vertices, negFaces);

	std::list<TempObject*>::iterator it;
	if(posObjects.size() == posSideObjects.size())
	{
		//new colliders count == new objects count, so just attach collider to closest object
		for(it = posObjects.begin(); it != posObjects.end(); ++ it)
		{
			CreateNewObjectFromPieces_Collider_complex(pObject,pHitInfo, &(*it)->faces, &posSideObjects);
		}
	}
	else
	{
		//new colliders count != new objects count, so attach the object to closest collider
		//the remaining object will using its mesh data to create collider
		//this do not work very well, because the count is not equal means the collider mesh do not fit mesh very well
		//so, just use object's mesh data to create collider, although some new mesh-collider could still fit perfectly
		std::list<GameObject*>::iterator objIT;
		RefCountPtr<CollisionMesh> pNewMesh = nullptr;
		for(objIT = posSideObjects.begin(); objIT != posSideObjects.end(); ++ objIT)
		{
			pNewMesh = (*objIT)->GetModelComponent()->GetMesh()->CreateCollisionMesh();
			(*objIT)->CreateColliderComponent(nullptr)->SetMesh(pNewMesh);
		}
	}

	if(negObjects.size() == negSideObjects.size())
	{
		//new colliders count == new objects count, so just attach to closest object
		for(it = negObjects.begin(); it != negObjects.end(); ++ it)
		{
			CreateNewObjectFromPieces_Collider_complex(pObject,pHitInfo, &(*it)->faces, &negSideObjects);
		}
	}
	else
	{
		std::list<GameObject*>::iterator objIT;
		RefCountPtr<CollisionMesh> pNewMesh = nullptr;
		for(objIT = negSideObjects.begin(); objIT != negSideObjects.end(); ++ objIT)
		{
			pNewMesh = (*objIT)->GetModelComponent()->GetMesh()->CreateCollisionMesh();
			(*objIT)->CreateColliderComponent(nullptr)->SetMesh(pNewMesh);
		}
		
	}
	
}

Vector3 
CutHelper::CalculateMiddlePoint(std::vector<Vector3>& verticesData, std::list<Face*> &faces)
{
	Vector3 vMin,vMax;
	vMin.x = vMin.y = vMin.z = FLT_MAX;
	vMax.x = vMax.y = vMax.z = -FLT_MAX;

	std::list<Face*>::iterator it;
	Face* pFace = nullptr;
	Vector3* pVertex = nullptr;
	uint32 i = 0;
	for(it = faces.begin(); it != faces.end(); ++it)
	{
		pFace = *it;
		for(i = 0; i < 3; ++i)
		{
			pVertex = &verticesData[pFace->v[i]];

			if(pVertex->x < vMin.x)
				vMin.x = pVertex->x;

			if(pVertex->y < vMin.y)
				vMin.y = pVertex->y;

			if(pVertex->z < vMin.z)
				vMin.z = pVertex->z;

			if(pVertex->x > vMax.x)
				vMax.x = pVertex->x;

			if(pVertex->y > vMax.y)
				vMax.y = pVertex->y;

			if(pVertex->z > vMax.z)
				vMax.z = pVertex->z;
		}
	}

	return (vMin + vMax) * 0.5f;
}