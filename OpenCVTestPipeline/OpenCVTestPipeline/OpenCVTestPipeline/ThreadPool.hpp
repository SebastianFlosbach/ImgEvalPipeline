#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP


#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

template <typename I, typename O>
class ThreadPool {
public:
	ThreadPool(std::function<O(I)> workerFunction) : ThreadPool(workerFunction, std::thread::hardware_concurrency())
	{
	}

	ThreadPool(std::function<O(I)> workerFunction, int threads) : workerFunction_(workerFunction), threadCount_(threads)
	{
	}

	void run() {
		if (running_) {
			return;
		}

		running_ = true;
		for (int i = 0; i < threadCount_; i++)
		{
			threads_.push_back(std::move(std::thread(&ThreadPool::process, this)));
		}
	}

	void stop() {
		running_ = false;
		wait();
	}

	void wait() {
		if (!running_) {
			return;
		}

		waiting_ = true;
		for (auto& thread : threads_) {
			thread.join();
		}
		running_ = false;
	}

	void addInput(const I& inputData) {
		std::unique_lock<std::mutex> lock(mInput_);
		input_.push(inputData);
		inputCounter_++;
		lock.unlock();
		cvInput_.notify_one();
	}

	std::vector<O> getResults() {
		std::lock_guard<std::mutex> lock(mOutput_);
		std::vector<O> results = std::move(results_);
		results_ = std::vector<O>();
		return results;
	}

	int getInputCount() const {
		return inputCounter_;
	}

	int getResultCount() const {
		return resultCounter_;
	}

private:
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

	void process() {
		while (running_) {
			std::unique_lock<std::mutex> inputLock(mInput_);
			if (input_.empty()) {
				if (waiting_) {
					return;
				}
				cvInput_.wait(inputLock);
			}

			I input = input_.front();
			input_.pop();

			inputLock.unlock();

			O result = workerFunction_(input);

			std::unique_lock<std::mutex> outputLock(mOutput_);
			results_.push_back(std::move(result));

			resultCounter_++;
		}
	}

};


#endif
