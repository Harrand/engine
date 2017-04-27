#ifndef TIMEKEEPER_HPP
#define TIMEKEEPER_HPP
#include <chrono>
#include <vector>
#include <thread>
#include <functional>

class TimeKeeper
{
public:
	TimeKeeper();
	TimeKeeper(const TimeKeeper& copy) = default;
	TimeKeeper(TimeKeeper&& move) = default;
	TimeKeeper& operator=(const TimeKeeper& rhs) = default;
	
	void update();
	void reload();
	float getRange() const;
	bool millisPassed(float millis) const;
private:
	unsigned long before, after;
	time_t previous, now;
};

class TimeProfiler
{
public:
	TimeProfiler();
	TimeProfiler(const TimeProfiler& copy) = default;
	TimeProfiler(TimeProfiler&& move) = default;
	TimeProfiler& operator=(const TimeProfiler& rhs) = default;
	
	void beginFrame();
	void endFrame();
	void reset();
	float getDeltaAverage() const;
	unsigned int getFPS() const;
private:
	std::vector<float> deltas;
	TimeKeeper tk;
};

namespace Scheduler
{
template<class ReturnType>
inline void syncDelayedTask(unsigned int millisDelay, std::function<ReturnType> f)
{
	std::this_thread::sleep_for(std::chrono::duration<unsigned int, std::milli>(millisDelay));
	f();
}

template<class ReturnType>
inline void asyncDelayedTask(unsigned int millisDelay, std::function<ReturnType> f)
{
	std::thread(Scheduler::syncDelayedTask<ReturnType>, millisDelay, f).detach();
}
}

#endif // TIMEKEEPER_HPP