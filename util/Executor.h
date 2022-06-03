//
// Created by 周蜀杰 on 2022/5/27.
//

#ifndef NETWORKDESIGN_EXECUTOR_H
#define NETWORKDESIGN_EXECUTOR_H

#include "BlockingCollection.h"
#include "vector"
#include "thread"

class Task {

public:
	Task(std::function<void()> func, std::chrono::milliseconds delay);

	void run();

private:
	std::function<void()> func;
	std::chrono::milliseconds delay;
};

class Executor {
public:
	explicit Executor(int size);

	void run();

	void stop();

	void submit(std::function<void()> func, std::chrono::milliseconds delay);

private:
	std::vector<std::thread> threads;
	code_machina::BlockingQueue<Task *> queue;
	bool shouldStop = false;
};


#endif //NETWORKDESIGN_EXECUTOR_H
