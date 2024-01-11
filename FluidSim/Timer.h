#pragma once
class Timer
{

private:
	float lifeTime;
	bool hasEnded;

public:
	static void startTimer(Timer* timer, float lifeTime);
	static void endTimer(Timer* timer);
	static void updateTimer(Timer* timer);
	static bool timerDone(Timer* timer);
};

