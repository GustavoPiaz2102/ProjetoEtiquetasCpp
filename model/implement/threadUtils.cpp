#include "../model/heaters/threadUtils.hpp"

void setThreadPriority(std::thread& t, int priority) {
	sched_param param;
	param.sched_priority = priority; // 1-99 para SCHED_FIFO/RR

	pthread_setschedparam(t.native_handle(), SCHED_FIFO, &param);
}

void setThreadAffinity(std::thread& t, std::vector<int> cores) {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);

	for (int core : cores)
		CPU_SET(core, &cpuset);

	pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset);
}