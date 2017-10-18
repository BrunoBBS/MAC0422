#include "process.hpp"

Process::Process (int t0, int tf, int b, std::string name,
        std::vector<MemoryAccess> accesses) :
    init_time(t0), final_time(tf), mem_amount(b), name(name)
{
    mem_accesses = accesses;
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

std::vector<MemoryAccess>::iterator Process::accesses_iterator()
{
    return mem_accesses.begin();
}
