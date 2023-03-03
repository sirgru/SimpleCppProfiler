#include "Timer.h"

#include <chrono>

using namespace Timing;

Timer::Timer()
{
	auto startTime = std::chrono::high_resolution_clock::now();
	m_start = std::chrono::time_point_cast<std::chrono::microseconds>(startTime).time_since_epoch().count();
}

long long Timer::Stop() {
	if (m_isStopped) return m_elapsed;
	m_isStopped = true;

	m_elapsed = CalculateElapsed();
	return m_elapsed;
}

long long Timer::Elapsed() {
	if (!m_isStopped) return CalculateElapsed();
	else return m_elapsed;
}

bool Timer::IsStopped() {
	return m_isStopped;
}

long long Timer::CalculateElapsed() {
	auto endTime = std::chrono::high_resolution_clock::now();
	long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();
	return end - m_start;
}

long long Stopwatch::Pause() {
	m_totalElapsed += m_timer.Stop();
	return m_totalElapsed;
}

bool Stopwatch::IsPaused() {
	return m_timer.IsStopped();
}

void Stopwatch::Resume() {
	if (!m_timer.IsStopped()) return;
	m_timer = Timer();
}

long long Stopwatch::Elapsed() {
	if (m_timer.IsStopped()) return m_totalElapsed;
	else {
		Pause();
		Resume();
		return m_totalElapsed;
	}
}

void Stopwatch::Reset() {
	m_totalElapsed = 0;
	m_timer = Timer();
}

size_t Instrumentor::GetThreadID() {
	return std::hash<std::thread::id>{}(std::this_thread::get_id());
}

void Instrumentor::Print(const std::string& outputFileName) {
	std::ofstream outputStream;
	outputStream.open(outputFileName);
	outputStream << "{\"otherData\": {},\"traceEvents\":[";
	for (auto data : m_data) {
		InstrumentationData::Print(data, outputStream, m_threadID);
	}
	outputStream << "]}";
	outputStream.flush();
	outputStream.close();
	m_data.clear();
}

void InstrumentationData::Print(InstrumentationData& data, std::ofstream& outputStream, size_t threadID) {
	outputStream << "{";
	outputStream << "\"cat\":\"function\",";
	outputStream << "\"dur\":" << data.m_timer.Elapsed() << ',';
	outputStream << "\"name\":\"" << data.m_name << "\",";
	outputStream << "\"ph\":\"X\",";
	outputStream << "\"pid\":0,";
	outputStream << "\"tid\":" << threadID << ",";
	outputStream << "\"ts\":" << data.m_timer.Start();
	outputStream << "}";
}