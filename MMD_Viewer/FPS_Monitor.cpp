#include"FPS_Monitor.h"

FPS_Monitor::FPS_Monitor(const int interval_ms)
	:
	timer(),
	timeCounter(0),
	frameCounter(0),
	waitTime(interval_ms),
	averageFPS(0.f)
{

}

FPS_Monitor::~FPS_Monitor()
{

}

void FPS_Monitor::Update()
{
	timer.Update();
	timeCounter += timer.GetFrameTime();
	frameCounter++;
	if (timeCounter > waitTime)
	{
		averageFPS =
			static_cast<float>(frameCounter) / static_cast<float>(timeCounter / 1000);

		DebugOutParam(averageFPS);

		frameCounter = 0;
		timeCounter %= waitTime;
	}
}

float FPS_Monitor::GetFrameTime() const
{
	return timer.GetFrameTime();
}


