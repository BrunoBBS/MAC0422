#ifndef __PROCESS_HPP__
#define __PROCESS_HPP__

#include "typedef.hpp"

#include <string>
#include <vector>

class Process
{
    public:
        Process (int t0, int tf, int b, uint uid, std::string name);

        /* * * * * *
         * Getters *
         * * * * * */
        // Get start time
        int get_start_time();
        // Get ending time
        int get_end_time();
        // Get used memory
        int get_used_mem();
        // Get process name
        std::string get_proc_name();

        // Resets state of process
        void reset();

        // Set pid
        inline void set_pid(uint pid) { this->pid = pid; }

        // Set pid
        inline uint get_pid() { return pid; }

        // Set uid
        inline uint get_uid() { return uid; }

    private:
        /* * * * * * * * * * *
         * Fixed properties  *
         * * * * * * * * * * */
        // Unique process identifier
        byte uid;
        // Process initialization time
        int init_time;
        // Process finalization time
        int final_time;
        // Memory used by the process
        int mem_amount;
        // Process name
        std::string name;

        /* * * * * * * * * * * *
         * Dynamic properties  *
         * * * * * * * * * * * */
        // Process id in the system
        uint pid;

        /* * * * * * * * * * *
         * Static properties *
         * * * * * * * * * * */
        static uint next_uid;
};

#endif
