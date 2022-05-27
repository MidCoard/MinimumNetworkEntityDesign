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
	explicit Task(std::function<void()>  func);
	void run();
private:
	std::function<void()> func;
};

class Executor {
public:
	explicit Executor(int size);

	void run();

	void stop();
private:
	std::vector<std::thread> threads;
	code_machina::BlockingQueue<Task*> queue;
	bool shouldStop = false;
};


#endif //NETWORKDESIGN_EXECUTOR_H
