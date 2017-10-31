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
        bool write(int pos, Process &proc, char val);

        // Start process
        bool start_process(Process &process);
        
        // End process
        bool end_process(Process &process);

        // Set page replacer
        bool set_page_replacer(std::shared_ptr<PageReplacer> replacer);
        
        // Sets up the space manager
        virtual void init() {}

        // Destructs space manager
        virtual void end() {}

    protected:
        // EP base object
        EP &ep;        

        // Returns position of the start of the process
        virtual int allocate(int size) = 0;
        
        // Free process allocated in pos
        virtual void free(int pos) = 0;
        
        // Debug printing
        virtual void dprint() {}
        
        // Page replacer handle
        std::shared_ptr<PageReplacer> page_replacer;
    
    private:
        // Number of processes
        uint proc_cnt = 0;

        // Space manager name
        std::string name;
};

#endif
