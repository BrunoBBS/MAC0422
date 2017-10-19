#ifndef __EP_BASE_HPP__
#define __EP_BASE_HPP__

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

class SpaceManager;
class PageReplacer;

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

        // Insert space manager option
        bool add_space_manager(int option_number, SpaceManager *manager);

        // Insert page replacer option
        bool add_page_replacer(int option_number, PageReplacer *replacer);

    private:

        // Allowed managers
        std::map<int, std::unique_ptr<SpaceManager> > space_managers;
        std::map<int, std::unique_ptr<PageReplacer> > page_replacers;

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
