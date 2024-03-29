#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include "typedef.hpp"

#include <fstream>
#include <iostream>

/*
 * Used to define if access will be to physical memory or virtual memory
 */
enum mem_t
{
    PHYS,
    VIRT
};

class Memory
{
    public:
        Memory(int p_qty, int v_qty,
                std::string p_mem_filename = "/tmp/ep3.mem",
                std::string v_mem_filename = "/tmp/ep3.vir");
        
        ~Memory();

        // Loaded successfuly?
        inline bool good() { return is_ok; }

        // Write memory position
        bool access(int pos, mem_t mem_type, byte val, int size = 1);

        // Copy memory area
        bool copy(int src, int dst, mem_t src_t, mem_t dst_t, int size); 

        // Effectively deletes a process from memory by writing -1 on every byte
        // in that memory area
        bool wipe(int pos, mem_t mem_type, int size);

        // Prints state of the memory
        void print(int row_size = 16);

    private:

        // Validate memory position
        bool val_pos(int pos, mem_t mem_type);

        // Notify out of range
        void notify_oor(int post, mem_t mem_type);

        // Memory file handlers
        std::fstream p_mem_file;
        std::fstream v_mem_file;

        // Memory sizes
        int p_mem_size;
        int v_mem_size;

        // Was it loaded successfuly?
        bool is_ok;
};

#endif
