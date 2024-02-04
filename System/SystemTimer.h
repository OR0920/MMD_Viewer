#ifndef _SYSTEM_TIMER_H_
#define _SYSTEM_TIMER_H_

namespace System
{
	class Timer
	{
	public:
		Timer(); ~Timer();

		void Update();

		unsigned int GetFrameTime() const;
		unsigned int GetFrameRate() const;
	private:
		unsigned int mInitTime;
		unsigned int mPrevTime;
		unsigned int mCurrentTime;
		unsigned int mFrameTime;
		unsigned int mFrameRate;
	};
}

#endif // !_SYSTEM_TIMER_H_


