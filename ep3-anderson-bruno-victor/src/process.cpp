#include "process.hpp"

uint Process::next_uid = 0;

Process::Process (int t0, int tf, int b, uint uid, std::string name) :
    uid(uid), init_time(t0), final_time(tf), mem_amount(b), name(name)
{
}

int Process::get_start_time()
{
    return init_time;
}

int Process::get_end_time()
{
    return final_time;
}

int Process::get_used_mem()
{
    return mem_amount;
}

std::string Process::get_proc_name()
{
    return name;
}
