#ifndef __SPACE_MANAGER_HPP__
#define __SPACE_MANAGER_HPP__

#include "typedef.hpp"
#include "ep_base.hpp"
#include "page_replacer.hpp"

#include <map>
#include <memory>

/* * * * * * * * * * * * * * * * * * * * * * *
 * Space Manager base class                  *
 *                                           *
 * Space Managers do 2 things:               *
 * - Manages free memory for allocation      *
 *   (Implementation specific)               *
 * - Translates addresses from process space *
 *   to global virtual memory address        *
 * * * * * * * * * * * * * * * * * * * * * * */

class SpaceManager
{
    public:
        SpaceManager(EP &ep, std::string name);

        // Gets Space Manager name
        std::string get_name();

        // Access memory
        bool write(int pos, uint pid, char val);

        // Start process
        bool start_process(Process &process);
        
        // End process
        bool end_process(Process &process);

        // Set page replacer
        bool set_page_replacer(std::shared_ptr<PageReplacer> replacer);

    protected:
        // EP base object
        EP &ep;

        // Returns position of the start of the process
        virtual int allocate(int size) = 0;
        
        // Free process allocated in pos
        virtual void free(int pos) = 0;

        // Page replacer handle
        std::shared_ptr<PageReplacer> page_replacer;
    
    private:
        // Get new process id
        uint new_id();

        // Next id
        uint next_id = 1;

        // Number of processes
        uint proc_cnt = 0;

        // Processes table
        std::map<int, int> pid_offset_table; 

        // Space manager name
        std::string name;
};

#endif