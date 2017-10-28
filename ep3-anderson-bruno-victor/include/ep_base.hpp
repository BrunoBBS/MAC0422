#ifndef __EP_BASE_HPP__
#define __EP_BASE_HPP__

#include "typedef.hpp"

#include "memory.hpp"
#include "process.hpp"

#include "space_manager.hpp"
#include "page_replacer.hpp"

#include <iostream>
#include <fstream>

#include <string>

#include <memory>
#include <algorithm>
#include <vector>
#include <map>
#include <set>

class EP
{
    public:
        EP(std::string p_mem_filename = "/tmp/ep3.mem",
                std::string v_mem_filename = "/tmp/ep3.vir");

        // Loads input file
        void load_file(std::string filename);

        // Selects free space manager
        void select_free_space_manager(std::string manager_s);

        // Selects page replace manager
        void select_page_replace_manager(std::string manager_s);

        // Runs simulator
        void run(std::string interval_s);

        // Insert space manager option
        bool add_space_manager(int option_number, SpaceManager *manager);

        // Insert page replacer option
        bool add_page_replacer(int option_number, PageReplacer *replacer);

        // Get memory handler
        inline std::shared_ptr<Memory> mem_handler() { return memory; }
        
        // Get virtual memory size
        inline int virt_size() { return virt_mem; }

        // Get physical memory size
        inline int phys_size() { return phys_mem; }

        // Get allocation unit size
        inline int get_alloc_size() { return alloc_size; }
        
        // Get page size
        inline int get_page_size() { return page_size; }

    private:
        // Memory object
        std::string p_mem_filename;
        std::string v_mem_filename;
        std::shared_ptr<Memory> memory;

        // Allowed managers
        std::map<int, std::shared_ptr<SpaceManager> > space_managers;
        std::map<int, std::shared_ptr<PageReplacer> > page_replacers;

        // Currently selected managers
        int free_space_manager;
        int page_replace_manager;
        
        // Is any file loaded?
        bool file_loaded = false;

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

        // Compress memory events
        std::vector<uint> compress_evn;
        
        // Compress memory events
        std::map<uint, std::vector<std::pair<uint, int> > > mem_accesses;

        // List of processes
        std::vector<Process> process_list;
};

#endif
