#include "multiProcessHandler.hpp"

#include <unistd.h>
#include <vector>
#include "logger.hpp"

struct child_process{
    bool active;
    int worker_thread_count;
    pid_t PID;
};

std::vector<child_process> g_child_processes;

int init_MP(){
    
}

int init_child_process(int worker_thread_count){
    pid_t child_pid = fork();

    if(child_pid == 0){
        //child process
        while (true)
        {
            sleep(3);
        }
        
        _exit(0); 
    }else if(child_pid<0){
        log_msg(LOG_ERROR,"Failed to start the child process\n");
        return -1;
    }

    //parent process
    log_msg(LOG_INFO, "fork() returned %d\n", child_pid);

    child_process child{};
    child.PID = child_pid;
    child.active = true;
    child.worker_thread_count = worker_thread_count;

    g_child_processes.push_back(child);

    log_msg(LOG_INFO, "Started child process PID=%d\n", child_pid);

    return 0;
}