#ifndef __SP_MAN_QUICK_FIT_HPP__

#include "ep_base.hpp"
#include "space_manager.hpp"

#include <list>
#include <map>

namespace SpaceManagers
{
class QuickFit : public SpaceManager
{
  public:
    QuickFit(EP &ep);
    ~QuickFit();

    void init();

  private:
    const int n_sizes = 3;

    // Structure that stores free blocks of memory
    struct mem_block
    {
        // Start of memory block
        int init;
        // Size of memory block
        int size;
        // Pointer to next block
        mem_block *next;
        bool operator==(mem_block &a)
        {
            return a.init;
        }
        bool operator!=(mem_block &a)
        {
            return a.init;
        }
    };

    // Anchors for start and end of free spaces
    mem_block s_anchor, e_anchor;

    /*
     * Struct that stores an allocation size and how many processes required
     * this ammount. Only used init pre-processing.
     */
    struct alloc_size
    {
        // Size of allocation
        int size;

        // How many times it appears in input file
        int rep;

        inline bool operator==(alloc_size &a)
        {
            return a.size;
        }
    };

    /*
     * Map of popular sizes as keys with lists of memory blocks of that popular
     * sizes as values
     */
    std::map<int, std::list<mem_block>> pop_sizes;

    /*
     * Pre calculation function. Reads input file and saves most common space
     * requisitions
     */
    void pre_calculation();

    /*
     * Resdistributes the current memory blocks into smaller blocks of the popular sizes
     * and adds them to the respective lists
     */
    void redist_mem();

    void free(int pos);

    int allocate(int size, int uid);
};
}

#endif
