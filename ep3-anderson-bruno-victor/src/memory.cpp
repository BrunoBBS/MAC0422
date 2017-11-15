#include "memory.hpp"

Memory::Memory(int p_qty, int v_qty,
        std::string p_mem_filename,
        std::string v_mem_filename) :
    p_mem_size(p_qty),
    v_mem_size(v_qty),
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
    
    if (!v_mem_file.good())
    {
        std::cerr << "Error opening virtual memory file '" <<
            v_mem_filename << "'\n";
        is_ok = false;
        return;
    }

    for (int i = 0; i < p_qty; i++)
        p_mem_file << (char) -1;

    for (int i = 0; i < v_qty; i++)
        v_mem_file << (char) -1;

    p_mem_file.flush();
    v_mem_file.flush();
}

Memory::~Memory()
{
    if (p_mem_file.is_open())
        p_mem_file.close();

    if (v_mem_file.is_open())
        v_mem_file.close();
}

// Write memory position
bool Memory::access(int pos, mem_t mem_type, byte val)
{
    if (!is_ok) return false;
    if (!val_pos(pos, mem_type))
    {
        std::cerr << "Can't access pos " << pos << " from " <<
            ((mem_type == PHYS) ? "Physical" : "Virtual") << " memory\n";
        std::cerr << "Out of range!";
        notify_oor(pos, mem_type);
        return false;
    }

    std::fstream *file_p;

    if (mem_type == PHYS)
        file_p = &p_mem_file;
    else
        file_p = &v_mem_file;

    std::fstream &file = *file_p;
    file.seekg(pos);

    file << val;
    file.flush();

    return true;
}

bool Memory::wipe(int pos, mem_t mem_type, int size)
{
    if (!is_ok) return false;
    if (!val_pos(pos, mem_type))
    {
        std::cerr << "Can't access pos " << pos << " from " <<
            ((mem_type == PHYS) ? "Physical" : "Virtual") << " memory\n";
        std::cerr << "Out of range!";
        notify_oor(pos, mem_type);
        return false;
    }

    int finish_pos = pos + size;

    std::fstream *file_p;

    if (mem_type == PHYS)
        file_p = &p_mem_file;
    else
        file_p = &v_mem_file;

    std::fstream &file = *file_p;
    file.seekg(pos);

    for (; pos < finish_pos; pos++)
    {
        file << -1;
    }
    file.flush();
    return true;
}

// Copy memory area
bool Memory::copy(int src, int dst, mem_t src_t, mem_t dst_t, int size)
{
    if (!is_ok) return false;
    int src_s = src;
    int src_e = src + size;
    int dst_s = dst;
    int dst_e = dst + size;

    if (!val_pos(src_s, src_t) || !val_pos(src_e, src_t))
    {
        std::cerr << "invalid source range [" << src_s << ", " << src_e <<
            "] for " << ((src_t == PHYS) ? "physical" : "virtual") <<
            " memory\n";
        std::cerr << "out of range!";
        return false;
    }
    
    if (!val_pos(dst_s, dst_t) || !val_pos(dst_e, dst_t))
    {
        std::cerr << "invalid destination range [" << dst_s << ", " << dst_e <<
            "] for " << ((dst_t == PHYS) ? "physical" : "virtual") <<
            " memory\n";
        std::cerr << "out of range!";
        return false;
    }

    std::fstream *src_file_p, *dst_file_p;

    src_file_p = (src_t == PHYS) ? &p_mem_file : &v_mem_file;
    dst_file_p = (dst_t == PHYS) ? &p_mem_file : &v_mem_file;

    std::fstream &src_file = *src_file_p;
    std::fstream &dst_file = *dst_file_p;

    for (; src_s < src_e; src_s++, dst_s++)
    {
        src_file.seekg(src_s);
        byte val = src_file.peek();
        dst_file.seekg(dst_s);
        dst_file << val;
    }

    src_file.flush();
    dst_file.flush();

    return true;
}

// Validade memory position
bool Memory::val_pos(int pos, mem_t mem_type)
{
    if (!is_ok) return false;
    return !(pos < 0 || pos >= ((mem_type == PHYS) ? p_mem_size : v_mem_size));
} 

void Memory::notify_oor(int pos, mem_t mem_type)
{
}
