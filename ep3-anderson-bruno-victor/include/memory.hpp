#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include <fstream>
#include <iostream>

class Memory
{
    public:
        Memory(int p_qty, int v_qty,
                std::string p_mem_filename = "/tmp/ep3.mem",
                std::string v_mem_filename = "/tmp/ep3.vir");
        
        ~Memory();

        // Loaded successfuly?
        inline bool good() { return is_ok; }

    private:
        // Memory file handlers
        std::fstream p_mem_file;
        std::fstream v_mem_file;

        // Was it loaded successfuly?
        bool is_ok;
};

#endif
