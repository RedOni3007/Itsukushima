/*
* To map the glfw key to my key todo:think a name for my engine
*
* @author: Kai Yang
*/

#ifndef INPUT_MAPPER_GLFW_H
#define INPUT_MAPPER_GLFW_H

#include <Core/CoreHeaders.h>

class InputMapper
{
public:
	static uint32 MapKeyToButton(int32 nKey);
	static uint32 MapMouseButtonToButton(int32 nKey);
};

#endif