#pragma once


namespace bre
{
// 针对POSIX线程的实时优先级设置
#if defined(__APPLE__)
#include <pthread.h>
#include <mach/mach.h>

inline int set_realtime_priority() {
    mach_port_t thread = pthread_mach_thread_np(pthread_self());
    thread_time_constraint_policy_data_t policy;
    policy.period = 0;
    policy.computation = 5000; // 5 ms
    policy.constraint = 10000; // 10 ms
    policy.preemptible = 0; // 不可抢占

    kern_return_t result = thread_policy_set(thread, THREAD_TIME_CONSTRAINT_POLICY, (thread_policy_t)&policy, THREAD_TIME_CONSTRAINT_POLICY_COUNT);
    return result;
    // mach_port_deallocate(mach_task_self(), thread);
}
#elif defined(__linux__)
#include <pthread.h>
#include <mach/mach.h>
inline int set_realtime_priority() {
    struct sched_param param;
    param.sched_priority = 99; // 1~99, 99为最高优先级
    return pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
}
#elif defined(_WIN32)
inline int set_realtime_priority() {
    return 0;
}
#endif


} // namespace bre

