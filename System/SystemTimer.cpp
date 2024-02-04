#include"SystemTimer.h"
#include"DebugMessage.h"

#include<windows.h>
#pragma comment(lib, "winmm.lib")

namespace System
{
	Timer::Timer()
		:
		mInitTime((0xffffffff - timeGetTime()) - 6000),
		mPrevTime(0),
		mCurrentTime(0),
		mFrameTime(0),
		mFrameRate(0)
	{

	}

	Timer::~Timer()
	{

	}

	unsigned int Timer::GetFrameTime() const 
	{
		return mFrameTime;
	}
	unsigned int Timer::GetFrameRate() const
	{
		return mFrameRate;
	}
	void Timer::Update()
	{
		mCurrentTime = timeGetTime() + mInitTime;
		mFrameTime = mCurrentTime - mPrevTime;
		if (mFrameTime < 1)
		{
			DebugMessageWarning("The frame time is less than 1[ms]. The" << ToString(Timer::Update()) << " might have been Called more than twice at frame.");
		}

		mFrameRate = 1000.f / static_cast<float>(mFrameTime);

		mPrevTime = mCurrentTime;
	}


}
