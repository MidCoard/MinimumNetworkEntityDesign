#ifndef NETWORKDESIGN_TIMER_H
#define NETWORKDESIGN_TIMER_H


#include <iostream>
#include <thread>
#include <chrono>

class Timer {
	std::thread th;
	bool running = false;

public:
	typedef std::chrono::milliseconds Interval;
	typedef std::function<void(void)> Timeout;

	void start(const Interval &interval,
	           const Timeout &timeout) {
		running = true;

		th = std::thread([=]() {
			while (running == true) {
				std::this_thread::sleep_for(interval);
				timeout();
			}
		});

// [*]
		th.join();
	}

	void stop() {
		running = false;
	}
};

#endif //NETWORKDESIGN_TIMER_H
