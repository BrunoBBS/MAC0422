#ifndef __PG_REP_OPTIMAL_HPP__

#include "ep_base.hpp"
#include "page_replacer.hpp"

namespace PageReplacers
{
    class Optimal : public PageReplacer
    {
        public:
            Optimal(EP &ep);
            
            bool write(int pos, byte val);

            void init();

        private:
            // Basic variables
            int page_size;
            int phys_page_cnt;
            int virt_page_cnt;

            // Future events
            std::map<uint, int> instant_references;
            std::vector<Event> event_list;

            // Page table (physical_page = page_table[virtual_page])
            std::vector<int> page_table;
            
            // Reverse page table (virtual_page = r_page_table[physical_page])
            //  - For more efficient page switching
            std::vector<int> r_page_table;

            // Next free page (>= 0 if there is any unassigned physical page)
            int next_free_page;

            /*
             * Private methods
             */

            // Checks if a given virtual page is loaded in the physical memory
            bool loaded(int page);
            
            // Load given virtual page into the physical memory
            void load_page(int page);

            // Translates address from virtual to physical, swapping pages if
            // necesssary
            int translate_address(int virt);
    };
}

#endif
