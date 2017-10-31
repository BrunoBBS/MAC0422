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

bool SpaceManager::write(int pos, Process &proc, char val)
{
    // If space manager is not set
    if (!page_replacer)
        return false;

    // Translate address to virtual memory
    int new_address = pos + proc.get_offset();

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

    if (globals::e)
        dprint();

    // Sets memory offset
    process.set_offset(start_loc);

    proc_cnt++;

    return true;
}

bool SpaceManager::end_process(Process &process)
{
    // If space manager is not set
    if (!page_replacer)
        return false;

    // Free up memory
    free(process.get_offset());

    if (globals::e)
        dprint();

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
