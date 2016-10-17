#ifndef __SYSTEMTIMER_INCLUDED__
#define __SYSTEMTIMER_INCLUDED__
class CSystemTimer
{
public:
	CSystemTimer() {};
	~CSystemTimer(){};
	bool Initialize(const long double *systemClock = NULL)
	{
		if (systemClock==NULL)
			return false;
		m_clock=systemClock;
		m_lastTime=(*m_clock);
		m_fps=0.0f;
		return true;
	};
	float GetElapsedSeconds(unsigned long elapsedFrames=1)
	{
		if (m_clock==NULL)
			return 0.0f;
		long double currentTime = (*m_clock);
		float timeElapsed = (float)(currentTime-m_lastTime);
		m_lastTime=currentTime;
		m_fps=(float)elapsedFrames/timeElapsed;
		return timeElapsed;
	};
	float GetFPS()
	{
		if (m_clock==NULL)
			return 0.0f;
		return m_fps;
	};
	long double GetStartTime()
	{
		return (*m_clock);
	};
	float GetTimeElapsedSince(long double time)
	{
		long double currentTime = (*m_clock);
		return (float)(currentTime-time);
	};		
private:
	long double const * m_clock;
	long double m_lastTime;
	float m_fps;
};
#endif

