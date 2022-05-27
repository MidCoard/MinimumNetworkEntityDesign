//
// Created by 周蜀杰 on 2022/5/27.
//

#include "Executor.h"

#include <utility>

Executor::Executor(int size) {
	for (int i = 0; i < size; i++)
		threads[i] = std::thread(&Executor::run, this);
}

void Executor::run() {
	while (true) {
		Task* task;
		code_machina::BlockingCollectionStatus status = queue.try_take(task);
		if (status == code_machina::BlockingCollectionStatus::Ok)
			task->run();
		if (shouldStop)
			break;
	}
}

void Executor::stop() {
	shouldStop = true;
	for (auto & thread : threads)
		thread.join();
}


void Task::run() {
	func();
}

Task::Task(std::function<void()>  func) : func(std::move(func)) {}
