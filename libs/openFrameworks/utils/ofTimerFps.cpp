#include "ofTimerFps.h"

using namespace std::chrono;
using namespace std::chrono_literals;

void ofTimerFps::reset() {
   wakeTime = steady_clock::now();
}

ofTimerFps::ofTimerFps() {
	reset();
	interval = duration_cast<microseconds>(1s) / currentFPS;
};

void ofTimerFps::setFps(int fps) {
//	interval = std::ratio<1s, fps>;
	if (fps <= 0) {
		fps = 60; // fallback
	}
	currentFPS = fps;

	interval = duration_cast<microseconds>(1s) / currentFPS;
}

void ofTimerFps::waitNext() {
   // Lazy wakeup: sleep until 1 ms before the frame is due, spin only the last millisecond.
   // (chroma_rt patch 2026-10-14: upstream sleeps until wakeTime - 36ms - longer than a whole frame at any rate
   // above ~28 fps, so the sleep never happened and every capped loop spun on yield() for the full frame: one
   // core per window, measured 93 % on chroma_rt's 60 fps control window. Linux wakes a sleeping thread within
   // ~0.1 ms, so 1 ms of spin keeps the timing exact.)
   std::this_thread::sleep_until(wakeTime - 1ms);

   // Processor Coffee
   while(steady_clock::now() < (wakeTime)) { // 0.05ms 0.5us // - 0.5us  - 1ns
	   std::this_thread::yield();
   }

   lastWakeTime = wakeTime;
   wakeTime += interval;
}
