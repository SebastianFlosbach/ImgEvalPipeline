#include "ThreadPool.hpp"

#include "FeatureMatching/MatchData.hpp"
#include "FeatureMatching/MatchingPair.hpp"
#include "ImageData.hpp"
#include "FeatureDetection/FeatureContainer.hpp"

template <>
ThreadPool<MatchingPair, MatchData>::ThreadPool(std::function<MatchData(MatchingPair)> workerFunction) : ThreadPool(workerFunction, std::thread::hardware_concurrency())
{
}

template <>
ThreadPool<MatchingPair, MatchData>::ThreadPool(std::function<MatchData(MatchingPair)> workerFunction, int threads) :
	workerFunction_(workerFunction),
	threadCount_(threads),
	logger_(ConsoleLogger::instance())
{
}

template <>
void ThreadPool<MatchingPair, MatchData>::run() {
	if (running_) {
		return;
	}

	running_ = true;
	for (int i = 0; i < threadCount_; i++)
	{
		threads_.push_back(std::move(std::thread(&ThreadPool::process, this)));
	}
}

template <>
void ThreadPool<MatchingPair, MatchData>::stop() {
	running_ = false;
	wait();
}

template <>
void ThreadPool<MatchingPair, MatchData>::wait() {
	if (!running_) {
		return;
	}

	waiting_ = true;
	for (auto& thread : threads_) {
		thread.join();
	}
	running_ = false;
}

template <>
void ThreadPool<MatchingPair, MatchData>::addInput(const MatchingPair& inputData) {
	std::unique_lock<std::mutex> lock(mInput_);
	input_.push(inputData);
	inputCounter_++;
	lock.unlock();
	cvInput_.notify_one();
}

template <>
std::vector<MatchData> ThreadPool<MatchingPair, MatchData>::getResults() {
	std::lock_guard<std::mutex> lock(mOutput_);
	std::vector<MatchData> results = std::move(results_);
	return results;
}

template <>
int ThreadPool<MatchingPair, MatchData>::getInputCount() const {
	return inputCounter_;
}

template <>
int ThreadPool<MatchingPair, MatchData>::getResultCount() const {
	return resultCounter_;
}

template <>
void ThreadPool<MatchingPair, MatchData>::process() {
	while (running_) {
		std::unique_lock<std::mutex> inputLock(mInput_);
		if (input_.empty()) {
			if (waiting_) {
				return;
			}
			cvInput_.wait(inputLock);
		}

		MatchingPair input = input_.front();
		input_.pop();

		inputLock.unlock();

		MatchData result = workerFunction_(input);

		std::unique_lock<std::mutex> outputLock(mOutput_);
		results_.push_back(std::move(result));
	}
}

template <>
void ThreadPool<MatchingPair, MatchData>::logFinish(const std::string& message) {
	std::unique_lock<std::mutex> outputLock(mOutput_);
	resultCounter_++;
	logger_->log(std::to_string(getResultCount()) + "/" + std::to_string(getInputCount()) + ":\n" + message);
}

template <>
ThreadPool<ImageData, FeatureContainer>::ThreadPool(std::function<FeatureContainer(ImageData)> workerFunction) : ThreadPool(workerFunction, std::thread::hardware_concurrency())
{
}

template <>
ThreadPool<ImageData, FeatureContainer>::ThreadPool(std::function<FeatureContainer(ImageData)> workerFunction, int threads) :
	workerFunction_(workerFunction),
	threadCount_(threads),
	logger_(ConsoleLogger::instance())
{
}

template <>
void ThreadPool<ImageData, FeatureContainer>::run() {
	if (running_) {
		return;
	}

	running_ = true;
	for (int i = 0; i < threadCount_; i++)
	{
		threads_.push_back(std::move(std::thread(&ThreadPool::process, this)));
	}
}

template <>
void ThreadPool<ImageData, FeatureContainer>::stop() {
	running_ = false;
	wait();
}

template <>
void ThreadPool<ImageData, FeatureContainer>::wait() {
	if (!running_) {
		return;
	}

	waiting_ = true;
	for (auto& thread : threads_) {
		thread.join();
	}
	running_ = false;
}

template <>
void ThreadPool<ImageData, FeatureContainer>::addInput(const ImageData& inputData) {
	std::unique_lock<std::mutex> lock(mInput_);
	input_.push(inputData);
	inputCounter_++;
	lock.unlock();
	cvInput_.notify_one();
}

template <>
std::vector<FeatureContainer> ThreadPool<ImageData, FeatureContainer>::getResults() {
	std::lock_guard<std::mutex> lock(mOutput_);
	std::vector<FeatureContainer> results = std::move(results_);
	return results;
}

template <>
int ThreadPool<ImageData, FeatureContainer>::getInputCount() const {
	return inputCounter_;
}

template <>
int ThreadPool<ImageData, FeatureContainer>::getResultCount() const {
	return resultCounter_;
}

template <>
void ThreadPool<ImageData, FeatureContainer>::process() {
	while (running_) {
		std::unique_lock<std::mutex> inputLock(mInput_);
		if (input_.empty()) {
			if (waiting_) {
				return;
			}
			cvInput_.wait(inputLock);
		}

		ImageData input = input_.front();
		input_.pop();

		inputLock.unlock();

		FeatureContainer result = workerFunction_(input);

		std::unique_lock<std::mutex> outputLock(mOutput_);
		results_.push_back(std::move(result));
	}
}

template <>
void ThreadPool<ImageData, FeatureContainer>::logFinish(const std::string& message) {
	std::unique_lock<std::mutex> outputLock(mOutput_);
	resultCounter_++;
	logger_->log(std::to_string(getResultCount()) + "/" + std::to_string(getInputCount()) + ":\n" + message);
}
