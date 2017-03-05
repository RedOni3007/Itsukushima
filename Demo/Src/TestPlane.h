/*
* To modifed verices positon in realtime and push to GPU, just for testing
*
* @author: Kai Yang
*/

#ifndef TEST_PLANE_H
#define TEST_PLANE_H

class TestPlane : public GameObjectComponent
{
public:
	TestPlane();
	virtual ~TestPlane();

public:
	virtual void LogicUpdate();

	virtual const char* GetName();

	static const char* ClassName();
};

#endif