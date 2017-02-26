#include <App/Timer.h>


Timer::Timer(void)
{
	m_fDeltaTime = 0;
	m_fTime = 0;
	m_fMaxDeltaTime = 0.1f;
	m_uFPS = 0;
}


Timer::~Timer(void)
{
}

Timer* 
Timer::Instance()
{
	static Timer me;
	return &me;
}

void 
Timer::SetTime(float32 fTime)
{
	m_fTime = fTime;
}

void 
Timer::SetDeltaTime(float32 fDelta)
{
	m_fDeltaTime = fDelta;
}

float32 
Timer::GetDeltaTime()
{
	return m_fDeltaTime;
}

float32 
Timer::GetTime()
{
	return m_fTime;
}

void	
Timer::SetMaxDeltaTime(float32 fDelta)
{
	m_fMaxDeltaTime = fDelta;
}

float32 
Timer::GetMaxDeltaTime()
{
	return m_fMaxDeltaTime;
}

void	
Timer::SetFPS(uint16 uFPS)
{
	m_uFPS = uFPS;
}

uint16	
Timer::GetFPS()
{
	return m_uFPS;
}