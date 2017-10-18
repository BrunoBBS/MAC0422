#ifndef __EP_BASE_HPP__
#define __EP_BASE_HPP__

#include "process.hpp"

#include <iostream>
#include <fstream>

#include <string>

#include <algorithm>
#include <vector>

class EP
{
    public:
        EP();

        // Loads input file
        void load_file(std::string filename);

        // Selects free space manager
        void select_free_space_manager(std::string manager);

        // Selects page replace manager
        void select_page_replace_manager(std::string manager);

        // Runs simulator
        void run(std::string interval);

    private:

        // Currently selected managers
        int free_space_manager;
        int page_replace_manager;
        
        /* * * * * * * * * * *
         * Loaded from input *
         * * * * * * * * * * */
        // Physical memory amount
        int phys_mem;
        // Virtual memory amount
        int virt_mem;
        // Allocation unit size
        int alloc_size;
        // Page size
        int page_size;

        // List of processes
        std::vector<Process> process_list;
};

#endif
