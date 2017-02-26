#include <Input/InputManager.h>
#include <stdio.h>
#include <string.h>


InputManager::InputManager(void)
{
	m_pPadList = nullptr;
	m_fMouseX = m_fMouseY = 0;
	m_fLastMouseX = m_fLastMouseY = 0;
	m_fMouseMovementX = m_fMouseMovementY = 0;
}


InputManager::~InputManager(void)
{
	if(m_pPadList != nullptr)
	{
		delete[] m_pPadList;
		m_pPadList = 0;
	}
}

InputManager* 
InputManager::Instance()
{
	static InputManager me;
	return &me;
}

void 
InputManager::Init()
{
	m_pPadList = new VPad[MAX_PAD];
	for(int i = 0; i < MAX_PAD; ++i)
	{
		m_pPadList[i].Clear();
	}
}

void 
InputManager::Update()
{
	m_fMouseMovementX = m_fMouseX - m_fLastMouseX;
	m_fMouseMovementY = m_fMouseY - m_fLastMouseY;
	m_fLastMouseX = m_fMouseX;
	m_fLastMouseY = m_fMouseY;
}

void 
InputManager::PostUpdate()
{
	for(int i = 0; i < MAX_PAD; ++i)
	{
		m_pPadList[i].Update();
	}
}

void
InputManager::NewMousePos(float32 fx, float32 fy)
{
	m_fMouseX = fx;
	m_fMouseY = fy;
}

void
InputManager::MouseMovement(float32 fx, float32 fy)
{
	m_fMouseX += fx;
	m_fMouseY += fy;
}

glm::vec2 
InputManager::GetMousePos()
{
	return glm::vec2(m_fMouseX,m_fMouseY);
}

glm::vec2 
InputManager::GetMouseMovement()
{
	return glm::vec2(m_fMouseMovementX, m_fMouseMovementY);
}

void InputManager::ButtonDown(uint32 uPadIndex, uint32 uButtonIndex)
{
	assert(uPadIndex < MAX_PAD);
	if(uPadIndex >= MAX_PAD)
		return;

	m_pPadList[uPadIndex].ButtonDown(uButtonIndex);
}

void InputManager::ButtonUp(uint32 uPadIndex, uint32 uButtonIndex)
{
	assert(uPadIndex < MAX_PAD);
	if(uPadIndex >= MAX_PAD)
		return;

	m_pPadList[uPadIndex].ButtonUp(uButtonIndex);
}

void InputManager::NewAxisValue(uint32 uPadIndex, uint32 uAxisIndex, float32 fValue)
{
	assert(uPadIndex < MAX_PAD);
	if(uPadIndex >= MAX_PAD)
		return;

	m_pPadList[uPadIndex].SetAxis(uAxisIndex,fValue);
}

void InputManager::NewIntValue(uint32 uPadIndex, uint32 uIntIndex, int32 nValue)
{
	assert(uPadIndex < MAX_PAD);
	if(uPadIndex >= MAX_PAD)
		return;

	m_pPadList[uPadIndex].SetIntValue(uIntIndex,nValue);
}

VPad& 
InputManager::GetPad(uint32 uIndex)
{
	assert(uIndex < MAX_PAD);
	if(uIndex >= MAX_PAD)
		uIndex = 0;

	return m_pPadList[uIndex];
}

VPad::VPad()
{
	Clear();
}
	
VPad::~VPad()
{

}

void VPad::Clear()
{
	memset(&m_sCurrentData,0,sizeof(VPadData));
	memset(&m_sLastData,0,sizeof(VPadData));
}

void VPad::Update()
{
	memcpy(&m_sLastData,&m_sCurrentData,sizeof(VPadData));
}

void VPad::ButtonDown(uint32 uButton)
{
	assert(uButton < VPAD_BUTTON_COUNT);
	if(uButton >= VPAD_BUTTON_COUNT)
		return;

	m_sCurrentData.Button[uButton] = true;
}

void VPad::ButtonUp(uint32 uButton)
{
	assert(uButton < VPAD_BUTTON_COUNT);
	if(uButton >= VPAD_BUTTON_COUNT)
		return;

	m_sCurrentData.Button[uButton] = false;
}


void VPad::SetAxis(uint32 uAxis, float32 fValue)
{
	assert(uAxis < VPAD_AXIS_COUNT);
	if(uAxis >= VPAD_AXIS_COUNT)
		return;

	m_sCurrentData.Axis[uAxis] = fValue;
}

void VPad::SetIntValue(uint32 uIndex, int32 nValue)
{
	assert(uIndex < VPAD_INT_VALUE_COUNT);
	if(uIndex >= VPAD_INT_VALUE_COUNT)
		return;

	m_sCurrentData.IntValue[uIndex] = nValue;
}

bool VPad::IsButtonDown(int32 uButton)
{
	assert(uButton < VPAD_BUTTON_COUNT);
	if(uButton >= VPAD_BUTTON_COUNT)
		return false;

	return m_sCurrentData.Button[uButton];
}

bool VPad::IsButtonPressed(int32 uButton)
{
	assert(uButton < VPAD_BUTTON_COUNT);
	if(uButton >= VPAD_BUTTON_COUNT)
		return false;

	return (!m_sCurrentData.Button[uButton] && m_sLastData.Button[uButton]);
}

float32 VPad::GetAxis(uint32 uAxis)
{
	assert(uAxis < VPAD_AXIS_COUNT);
	if(uAxis >= VPAD_AXIS_COUNT)
		return 0;

	return m_sCurrentData.Axis[uAxis];
}

int32	VPad::GetIntValue(uint32 uIndex)
{
	assert(uIndex < VPAD_INT_VALUE_COUNT);
	if(uIndex >= VPAD_INT_VALUE_COUNT)
		return 0;

	return m_sCurrentData.IntValue[uIndex];
}