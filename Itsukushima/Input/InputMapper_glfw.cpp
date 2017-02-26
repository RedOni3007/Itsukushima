#include <Input/InputMapper_glfw.h>
#include <Input/InputManager.h>
#include <Core/GLEW_GLFW.h>



uint32 
InputMapper::MapKeyToButton(int32 nKey)
{
	//just for lazy test
	switch(nKey)
	{
		case GLFW_KEY_UNKNOWN:	return VPAD_BUTTON_UNKNOWN;break;
		case GLFW_KEY_ENTER:	return VPAD_BUTTON_START; break;
		case GLFW_KEY_SPACE:	return VPAD_BUTTON_SELECT; break;
		case GLFW_KEY_W:		return VPAD_BUTTON_UP;break;
		case GLFW_KEY_A:		return VPAD_BUTTON_LEFT; break;
		case GLFW_KEY_S:		return VPAD_BUTTON_DOWN; break;
		case GLFW_KEY_D:		return VPAD_BUTTON_RIGHT;break;
		case GLFW_KEY_J:		return VPAD_BUTTON_X; break;
		case GLFW_KEY_K:		return VPAD_BUTTON_A; break;
		case GLFW_KEY_L:		return VPAD_BUTTON_B;break;
		case GLFW_KEY_I:		return VPAD_BUTTON_Y; break;
		case GLFW_KEY_Q:		return VPAD_BUTTON_L1; break;
		case GLFW_KEY_E:		return VPAD_BUTTON_L2;break;
		case GLFW_KEY_U:		return VPAD_BUTTON_R1; break;
		case GLFW_KEY_O:		return VPAD_BUTTON_R2; break;

		case GLFW_KEY_LEFT_BRACKET: return VPAD_BUTTON_10; break;
		case GLFW_KEY_RIGHT_BRACKET: return VPAD_BUTTON_11; break;

		case GLFW_KEY_PAUSE: return VPAD_BUTTON_12; break;
		case GLFW_KEY_TAB: return VPAD_BUTTON_13; break;

		case GLFW_KEY_C: return VPAD_BUTTON_14; break;
		case GLFW_KEY_F: return VPAD_BUTTON_15; break;
		case GLFW_KEY_B: return VPAD_BUTTON_16; break;

		default:
			return VPAD_BUTTON_UNKNOWN;
			break;
	}
}

uint32
InputMapper::MapMouseButtonToButton(int32 nButton)
{
	//just for lazy test
	switch(nButton)
	{
		case GLFW_MOUSE_BUTTON_1:	return VPAD_BUTTON_MOUSE1;break;
		case GLFW_MOUSE_BUTTON_2:	return VPAD_BUTTON_MOUSE2; break;
		case GLFW_MOUSE_BUTTON_3:	return VPAD_BUTTON_MOUSE3; break;
		case GLFW_MOUSE_BUTTON_4:	return VPAD_BUTTON_MOUSE4;break;
		case GLFW_MOUSE_BUTTON_5:	return VPAD_BUTTON_MOUSE5; break;
		case GLFW_MOUSE_BUTTON_6:	return VPAD_BUTTON_MOUSE6; break;
		case GLFW_MOUSE_BUTTON_7:	return VPAD_BUTTON_MOUSE7;break;
		case GLFW_MOUSE_BUTTON_8:	return VPAD_BUTTON_MOUSE8; break;

		default:
			return VPAD_BUTTON_UNKNOWN;
			break;
	}
}