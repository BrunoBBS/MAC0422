#include "memory.hpp"

Memory::Memory(int p_qty, int v_qty,
        std::string p_mem_filename,
        std::string v_mem_filename) :
    is_ok(true)
{
    std::ios_base::openmode mode
        = std::ios_base::in | std::ios_base::out |
        std::ios_base::binary | std::ios_base::trunc;

    p_mem_file.open(p_mem_filename, mode);

    if (!p_mem_file.good())
    {
        std::cerr << "Error opening physical memory file '" <<
            p_mem_filename << "'\n";
        is_ok = false;
        return;
    }

    v_mem_file.open(v_mem_filename, mode);
    
    if (!p_mem_file.good())
    {
        std::cerr << "Error opening virtual memory file '" <<
            p_mem_filename << "'\n";
        is_ok = false;
        return;
    }

    for (int i = 0; i < p_qty; i++)
        p_mem_file << (char) -1;

    for (int i = 0; i < v_qty; i++)
        v_mem_file << (char) -1;
}

Memory::~Memory()
{
    if (p_mem_file.is_open())
        p_mem_file.close();

    if (v_mem_file.is_open())
        v_mem_file.close();
}
