#include "Executor.h"

#include <utility>

Executor::Executor(int size) {
	for (int i = 0; i < size; i++)
		threads.emplace_back(&Executor::run, this);
}

void Executor::run() {
	while (true) {
		Task *task;
		code_machina::BlockingCollectionStatus status = queue.try_take(task);
		if (status == code_machina::BlockingCollectionStatus::Ok)
			task->run();
		if (shouldStop)
			break;
	}
}

void Executor::stop() {
	shouldStop = true;
	for (auto &thread: threads)
		thread.join();
}

Executor::~Executor() {
	this->stop();
}


void Task::run() {
	func();
}

Task::Task(std::function<void()> func) : func(std::move(func)) {}
