#ifndef _FPS_MONITOR_H_
#define _FPS_MONITOR_H_

#include"System.h"

class FPS_Monitor
{
public:
	FPS_Monitor(const int interval_ms); ~FPS_Monitor();

	void Update();
	float GetFrameTime() const;
private:
	System::Timer timer;
	unsigned int timeCounter;
	unsigned int frameCounter;
	const unsigned int waitTime;
	float averageFPS;
};

#endif // !_FPS_MONITOR_H_


