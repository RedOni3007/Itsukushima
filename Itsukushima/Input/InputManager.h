/*
* Input Manager
* todo: input mess queue
*
* @author: Kai Yang
*/

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <Core/CoreHeaders.h>
#include <Core/GLEW_GLFW.h>
#include "InputDefine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#define MAX_PAD 8
#define VPAD_BUTTON_COUNT 256
#define VPAD_AXIS_COUNT 16
#define VPAD_INT_VALUE_COUNT 8

//per frame
struct VPadData
{
	bool Button[VPAD_BUTTON_COUNT];
	float32 Axis[VPAD_AXIS_COUNT];
	int32 IntValue[VPAD_INT_VALUE_COUNT];
};

class VPad
{
public:
	VPad();
	~VPad();

	void Clear();

	void Update();
	void ButtonDown(uint32 uButton);
	void ButtonUp(uint32 uButton);
	void SetAxis(uint32 uAxis, float32 fValue);
	void SetIntValue(uint32 uIndex, int32 nValue);

	bool	IsButtonDown(int32 uKey);//current down
	bool	IsButtonPressed(int32 uKey);//current up, last down
	float32 GetAxis(uint32 uAxis);
	int32	GetIntValue(uint32 uIndex);

private:
	VPadData m_sCurrentData;
	VPadData m_sLastData;
};

class InputManager
{
private:
	InputManager();
	~InputManager();

public:
	static InputManager* Instance();

	void Init();
	void Update();
	void PostUpdate();

	void NewMousePos(float32 fx, float32 fy);
	void MouseMovement(float32 fx, float32 fy);
	glm::vec2 GetMousePos();
	glm::vec2 GetMouseMovement();

	void ButtonDown(uint32 uPadIndex, uint32 uButtonIndex);
	void ButtonUp(uint32 uPadIndex, uint32 uButtonIndex);
	void NewAxisValue(uint32 uPadIndex, uint32 uAxisIndex, float32 fValue);
	void NewIntValue(uint32 uPadIndex, uint32 uIntIndex, int32 nValue);

	VPad& GetPad(uint32 uIndex);

private:
	VPad* m_pPadList;
	float32 m_fMouseX, m_fMouseY;
	float32 m_fLastMouseX, m_fLastMouseY;
	float32 m_fMouseMovementX, m_fMouseMovementY;

};

#endif