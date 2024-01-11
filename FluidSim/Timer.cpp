#include "Timer.h"
#include "raylib.h"
void Timer::startTimer(Timer* timer, float lifeTime) {
	if (timer != nullptr) {
		timer->lifeTime = lifeTime;
		timer->hasEnded = false;
	}
}

void Timer::endTimer(Timer* timer) {
	if (timer != nullptr)
		timer->hasEnded = true;
}

void Timer::updateTimer(Timer* timer) {
	if(timer != nullptr)
		timer->lifeTime -= GetFrameTime();
}

bool Timer::timerDone(Timer* timer) {
	if(timer != nullptr)
		return (timer->lifeTime <= 0) && (!timer->hasEnded);
}