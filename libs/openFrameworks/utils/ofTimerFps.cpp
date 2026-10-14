#include "ofTimerFps.h"
#include <thread>

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
   // Lazy wakeup: sleep until 1 ms before the frame is due, nap only the last millisecond.
   // (chroma_rt patch: upstream sleeps until wakeTime - 8ms, so a 60 fps loop naps 5 us at a time for half of
   // every frame. Linux wakes a sleeping thread within ~0.1 ms, so 1 ms keeps the timing exact.)
   std::this_thread::sleep_until(wakeTime - 1ms);

   // Processor Coffee
   while(steady_clock::now() < (wakeTime)) { // 0.05ms 0.5us // - 0.5us  - 1ns
//	   std::this_thread::yield();
	   std::this_thread::sleep_for(5us);
   }

   lastWakeTime = wakeTime;
   wakeTime += interval;
}
