#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP


#include "ConsoleLogger.hpp"

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

template <typename I, typename O>
class ThreadPool {
public:
	ThreadPool(std::function<O(I)> workerFunction);
	ThreadPool(std::function<O(I)> workerFunction, int threads);

	void run();
	void stop();
	void wait();

	void addInput(const I& inputData);
	std::vector<O> getResults();

	int getInputCount() const;
	int getResultCount() const;

private:
	ConsoleLogger* logger_;

	bool running_ = false;
	bool waiting_ = false;
	int threadCount_;
	std::vector<std::thread> threads_;
	std::queue<I> input_;
	std::vector<O> results_;
	std::function<O(I)> workerFunction_;

	std::mutex mInput_;
	std::condition_variable cvInput_;
	std::mutex mOutput_;

	std::atomic<int> inputCounter_ = 0;
	std::atomic<int> resultCounter_ = 0;

	void process();

protected:
	void logFinish(const std::string& message);

};


#endif
