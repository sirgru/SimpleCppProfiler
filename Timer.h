#pragma once

#include <vector>
#include <fstream>

// We use this alias to signal that only string literals embedded in the executable are safe to use.
using StaticString = const char*;

namespace Timing {
	/// <summary>
	/// Measures time from creation until stopping in microseconds.
	/// </summary>
	class Timer
	{
	private:
		// Storing the calculated time imposes an overhead which is usually not measurable.
		// However, not storing chrono allows us to not include <chrono>, which improves compile times.
		long long m_start;
		long long m_elapsed = 0;
		bool m_isStopped = false;

	public:
		/// <summary>
		/// Creates and starts the timer.
		/// </summary>
		Timer();

		/// <summary>
		/// Stops the timer and returns the elapsed time in microseconds.
		/// </summary>
		long long Stop();

		/// <summary>
		/// If the timer is stopped, returns the elapsed time in microseconds, from creation to the stop time.
		/// If the timer is not stopped, calculates the elapsed time without stopping.
		/// </summary>
		long long Elapsed();

		/// <summary>
		/// Returns whether the timer is stopped.
		/// </summary>
		bool IsStopped();

		/// <summary>
		/// Returns the representation of a timestamp when the timer was started.
		/// </summary>
		long long Start() {
			return m_start;
		}

	private:
		long long CalculateElapsed();
	};

	/// <summary>
	/// A Timer with the ability to be paused and reset.
	/// </summary>
	class Stopwatch {
	private:
		Timer m_timer;
		long long m_totalElapsed = 0;

	public:
		/// <summary>
		/// Pauses the Stopwatch.
		/// </summary>
		long long Pause();

		/// <summary>
		/// Queries whether the stopwatch is currently paused.
		/// </summary>
		bool IsPaused();

		/// <summary>
		/// Resumes the stopwatch if it is paused.
		/// </summary>
		void Resume();

		/// <summary>
		/// Returns the cumulative time duration of all time intervals from starting or resuming to pausing.
		/// If the stopwatch is not paused, calculates the elapsed time from the last start or resume time without pausing.
		/// </summary>
		long long Elapsed();

		/// <summary>
		/// Resets the stopwatch. Elapsed time is set to 0 and timer is restarted.
		/// </summary>
		void Reset();

	};

	class InstrumentationData;

	/// <summary>
	/// A Singleton type in charge of collecting InstrumentationData.
	/// </summary>
	class Instrumentor 
	{
	friend class InstrumentationData;

	private:
		std::vector<InstrumentationData> m_data;
		size_t m_threadID;
		bool m_isEnabled = false;

		void Add(const InstrumentationData& data) {
			if (!m_isEnabled) return;
			m_data.push_back(data);
		}

	public:
		/// <summary>
		/// Returns the singleton instance of this class.
		/// </summary>
		static Instrumentor& Instance() {
			static Instrumentor instance;
			return instance;
		}

		/// <summary>
		/// Enables receiving Instrumentation Data
		/// </summary>
		void Enable() {
			m_isEnabled = true;
		}

		/// <summary>
		/// Disables receiving Instrumentation Data
		/// </summary>
		void Disable() {
			m_isEnabled = false;
		}

		/// <summary>
		/// Prints the accumulated InstrumentationData to the file with the specified path.
		/// </summary>
		void Print(const std::string& outputFileName);

	private:
		Instrumentor() {
			m_data.reserve(64);
			m_threadID = GetThreadID();
		}

		static size_t GetThreadID();
	};

	/// <summary>
	/// A single data point of instrumentation.
	/// </summary>
	class InstrumentationData 
	{
	friend class Instrumentor;

	private:
		Timer m_timer;
		StaticString m_name;
		
		static void Print(InstrumentationData& data, std::ofstream& outputStream, size_t threadID);

	public:
		explicit InstrumentationData(StaticString name) : m_name(name), m_timer(Timer()) {}

		~InstrumentationData() {
			m_timer.Stop();
			Instrumentor::Instance().Add(*this);
		}
	};
	
}