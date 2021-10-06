#ifndef CONSOLE_LOGGER_HPP
#define CONSOLE_LOGGER_HPP


#include <iostream>
#include <mutex>
#include <string>

class ConsoleLogger {
public:
	static ConsoleLogger* instance() {
		if (instance_ == nullptr) {
			instance_ = new ConsoleLogger();
		}

		return instance_;
	}

	void log(const std::string& message) {
		std::lock_guard<std::mutex> lock(mLock_);
		std::cout << message << std::endl;
	}

private:
	std::mutex mLock_;
	static ConsoleLogger* instance_;

	ConsoleLogger() {}

};


#endif
