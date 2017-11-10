#ifndef __PAGE_REPLACER_HPP__
#define __PAGE_REPLACER_HPP__

#include "typedef.hpp"
#include "ep_base.hpp"

#include <memory>

/* * * * * * * * * * * * * * * * * * * * * * *
 * Page replacer base class                  *
 *                                           *
 * Page replacers deal with managing the     *
 * pages in physical and virtual memory,     *
 * saving or swapping pages between the two  *
 * * * * * * * * * * * * * * * * * * * * * * */

class PageReplacer
{
    public:
        PageReplacer(EP &ep, std::string name);

        // Gets space manager name
        std::string get_name();

        // Sets up the space manager
        virtual void init() {}

        // Writes value to address in virtual memory
        virtual bool write(int pos, char val) = 0;

        // Notifies that a clock cycle has passed
        virtual void clock() {}

        // Destructs space manager
        virtual void end() {}

        // Gets page fault count
        inline int get_page_fault_cnt() { return page_fault_cnt; }

    protected:
        // EP base object
        EP &ep;

        // Memory handler
        std::shared_ptr<Memory> memory;

        // Page fault counter
        int page_fault_cnt;

    private:
        // Page replacer name
        std::string name;
};

#endif
