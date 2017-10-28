#ifndef __SP_MAN_WORST_FIT_HPP__

#include "ep_base.hpp"
#include "space_manager.hpp"

#include <vector>

namespace SpaceManagers
{
    class WorstFit : public SpaceManager 
    {
        public:
            WorstFit(EP &ep);
            ~WorstFit();
            
            void init();

        private:
            int allocate(int size);
            void free(int pos);

            struct mem_block {
                // Start of memory block
                int init;
                // Size of memory block
                int size;
                // Pointer to next block
                mem_block *next;
            };
            
            // Anchors marking the start and end of the linked list
            mem_block s_anchor, e_anchor;

            // Number of inter-process spaces
            int free_cnt;
    };
}

#endif
