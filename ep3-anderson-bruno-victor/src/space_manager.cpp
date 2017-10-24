#include "space_manager.hpp"

SpaceManager::SpaceManager (EP &ep, std::string name) :
    ep(ep),
    name(name)
{
}

std::string SpaceManager::get_name()
{
    return name;
}

bool SpaceManager::write(int pos, uint pid, char val)
{
    // If space manager is not set
    if (!page_replacer)
        return false;

    // Find where the process is at the memory
    std::map<int, int>::iterator location = pid_offset_table.find(pid);

    // If the process is not in the table, just exit
    if (location == pid_offset_table.end())
        return false;

    // Translate address to virtual memory
    int new_address = pos + location->second;

    // Send write request to page replacer
    page_replacer->write(new_address, val);

    return true;
}

bool SpaceManager::start_process(Process &process)
{
    // If space manager is not set
    if (!page_replacer)
        return false;

    // Tries to allocate memory for process
    int start_loc = allocate(process.get_used_mem());

    // If process does not fit memory, failed to insert
    if (start_loc == -1)
        return false;
    
    // Generate new process id and set process id
    uint pid = new_id();
    process.set_pid(pid);

    // Add entry to pid table
    pid_offset_table.insert({pid, start_loc});

    proc_cnt++;

    return true;
}

bool SpaceManager::end_process(Process &process)
{
    // If space manager is not set
    if (!page_replacer)
        return false;

    // Get process PID
    uint pid = process.get_pid();

    // Find where the process is at the memory
    std::map<int, int>::iterator location = pid_offset_table.find(pid);

    // If the process is not in the table, just exit
    if (location == pid_offset_table.end())
        return false;

    // Free up memory
    this->free(process.get_used_mem());

    // And erase entry in pid table
    pid_offset_table.erase(location);

    proc_cnt--;

    return true;
}

bool SpaceManager::set_page_replacer(std::shared_ptr<PageReplacer> replacer)
{
    // If there are active processes, do not change the manager
    if (proc_cnt)
        return false;

    // Set page replacer
    page_replacer = replacer;

    return true;
}

uint SpaceManager::new_id()
{
    // Find next unused id
    while (pid_offset_table.find(next_id) != pid_offset_table.end())
        next_id++;

    // Returns new id and begins search on the next one
    return next_id++;
}
