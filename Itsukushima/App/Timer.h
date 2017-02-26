/**
* My Timer
*
* @author: Kai Yang
**/

#ifndef TIMER_H
#define TIMER_H

#include <Core/CoreHeaders.h>

class Timer
{
private:
	Timer(void);
	~Timer(void);

	float32 m_fDeltaTime;
	float32 m_fTime;
	float32 m_fMaxDeltaTime;

	uint16	m_uFPS;

public:
	static Timer* Instance();

	void	SetTime(float32 fTime);
	float32 GetTime();

	void	SetDeltaTime(float32 fDelta);
	float32 GetDeltaTime();


	void	SetMaxDeltaTime(float32 fDelta);
	float32 GetMaxDeltaTime();

	void	SetFPS(uint16 uFPS);
	uint16	GetFPS();
};

#endif