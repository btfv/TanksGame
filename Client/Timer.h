#pragma once
#include <iostream>
#include <chrono>

class Timer {
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	unsigned long milliseconds;
public:
	Timer(unsigned long nanoseconds) {
		this->milliseconds = nanoseconds;
		this->startTime = std::chrono::high_resolution_clock::now();
	}
	bool isExpired() {
		auto currTime = std::chrono::high_resolution_clock::now();
		return (std::chrono::duration_cast<std::chrono::milliseconds>(currTime - startTime).count() >= milliseconds);
	}
	unsigned long timePassed() {
		std::chrono::time_point<std::chrono::high_resolution_clock> currTime = std::chrono::high_resolution_clock::now();
		return static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(currTime - startTime).count());
	}
};