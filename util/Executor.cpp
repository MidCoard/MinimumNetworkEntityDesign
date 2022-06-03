#include "Executor.h"

#include <utility>

Executor::Executor(int size) {
	for (int i = 0; i < size; i++)
		threads.emplace_back(&Executor::run, this);
}

void Executor::run() {
	while (true) {
		Task *task = nullptr;
		code_machina::BlockingCollectionStatus status = queue.try_take(task, std::chrono::milliseconds(1));
		if (status == code_machina::BlockingCollectionStatus::Ok) {
			task->run();
			delete task;
		}
		if (shouldStop)
			break;
	}
}

void Executor::stop() {
	shouldStop = true;
	for (auto &thread: threads)
		thread.join();
}

void Executor::submit(std::function<void()> func, std::chrono::milliseconds delay) {
	queue.emplace(new Task(std::move(func), delay));
}


void Task::run() {
	std::this_thread::sleep_for(this->delay);
	func();
}

Task::Task(std::function<void()> func, std::chrono::milliseconds delay) : func(std::move(func)), delay(delay) {}
