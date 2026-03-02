#pragma once

#include <thread>
#include <pthread.h>
#include <sched.h>
#include <sys/resource.h>
#include <vector>

void setThreadPriority(std::thread& t, int priority);
void setThreadAffinity(std::thread& t, std::vector<int> cores);