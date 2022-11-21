#ifndef _AUTO_TIMER_H_
#define _AUTO_TIMER_H_

#if _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <cstring>
#include <string>

static inline double getCurrentTime()
{
#ifdef _WIN32
    LARGE_INTEGER freq;
    LARGE_INTEGER pc;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&pc);
    return pc.QuadPart * 1000.0 / freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
#endif // _WIN32
}

class AutoTimer{
public:
    explicit AutoTimer(const std::string& name = "CustomTask"){
        m_task_name = name;
        m_start_time = getCurrentTime();
    }

    /**
     * auto delete after leaving the region
    */
    ~AutoTimer(){
        report();
    }

    /**
     * get Elapsed Time
    */
    double get_elapsed_time() const {
        return getCurrentTime() - m_start_time;
    }

    void report(){
        double total_time = get_elapsed_time();
        printf("%8s: take %7.3lf ms\n",m_task_name.c_str(),total_time);
    }

private:
    /**
     * copy and operator banned !!!
    */
    AutoTimer(const AutoTimer&);
    AutoTimer& operator=(const AutoTimer&);

private:
    std::string m_task_name;
    double      m_start_time;
    double      m_stop_time;
};

/*
int main()
{
    {
        AutoTimer timer("task1");
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    printf("==============DONE==============\n");
    return 0;
}
*/
#endif //_AUTO_TIMER_H_