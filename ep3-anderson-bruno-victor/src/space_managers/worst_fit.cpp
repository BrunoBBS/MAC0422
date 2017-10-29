#include "space_managers/worst_fit.hpp"

SpaceManagers::WorstFit::WorstFit(EP &ep) :
    SpaceManager(ep, "Worst Fit"),
    free_cnt(1)
{}

SpaceManagers::WorstFit::~WorstFit()
{}

void SpaceManagers::WorstFit::init()
{
    // Free processes that were not freed properly
    while (free_cnt > 1)
        free(s_anchor.next->init);

    // Start anchor starts and ends at block -1
    s_anchor.init = 0;
    s_anchor.size = 0;
    s_anchor.next = &e_anchor;

    // End anchor starts and ends at block next to last usable block
    e_anchor.init = ep.virt_size() / ep.get_alloc_size();
    e_anchor.size = 0;
    e_anchor.next = nullptr;
}

int SpaceManagers::WorstFit::allocate(int size)
{
    // Can't allocate 0 or less bytes
    if (size <= 0)
        return -1;

    // Minimum allocation units required for specified size
    int min_req = (size / ep.get_alloc_size()) +
        (size % ep.get_alloc_size() ? 1 : 0);

    // Size of biggest free space block available (in allocation unit number)
    int max_size = -1;

    // Pointer to the item whose block after it is the biggest
    mem_block *max_prev = nullptr;

    // For each inter-process space
    mem_block *current = &s_anchor;
    for (int space = 0; space < free_cnt; space++, current = current->next)
    {
        // Calculate current free space
        int curr_sz = current->next->init - (current->init + current->size);

        // If process fits and is space is bigger than previous maximum
        if (curr_sz >= min_req && curr_sz > max_size)
        {
            // Update best alternative
            max_size = curr_sz;
            max_prev = current;
        }
    }

    // If no blocks were big enough, return error
    if (!max_prev)
        return -1;

    // Create new block
    mem_block *new_block = new mem_block;
    new_block->init = max_prev->init + max_prev->size;
    new_block->size = min_req;
    
    // Add new block to linked list
    new_block->next = max_prev->next;
    max_prev->next = new_block;

    // We have now one more possible free space
    free_cnt++;

    // Return converted memory address
    return new_block->init * ep.get_alloc_size();
}

void SpaceManagers::WorstFit::free(int pos)
{
    // Calculate start block
    int start_b = pos / ep.get_alloc_size();
    
    // Fail if address is invalid
    if (start_b < 0 || start_b >= ep.virt_size() / ep.get_alloc_size() ||
            pos % ep.get_alloc_size())
    {
        std::cerr << "Invalid address " << pos << "!\n";
        return;
    }

    // Search for block preceding the one we want to free
    mem_block *prec_free = nullptr;
    mem_block *current = &s_anchor;
    for (int space = 0; space < free_cnt; space++, current = current->next)
    {
        if (current->next->init == start_b)
        {
            prec_free = current;
            break;
        }
    }

    // If block was not found, fail
    if (!prec_free)
    {
        std::cerr << "No memory allocated at " << pos << "!\n";
        return;
    }

    // If it was, remove it from linked list
    mem_block *tmp = prec_free->next;
    prec_free->next = prec_free->next->next;

    // And free its memory
    tmp->init = tmp->size = 0;
    tmp->next = nullptr;
    delete tmp;

    // Finally, we have one less free space
    free_cnt--;
}

void SpaceManagers::WorstFit::dprint()
{
    std::vector<std::string> mem_block_desc;

    mem_block *current = &s_anchor;
    for (int space = 0; space <= free_cnt; space++, current = current->next)
    {
        if (!space || space == free_cnt)
            mem_block_desc.push_back("Anchor");
        else
        {
            std::ostringstream desc;
            desc << "i: " << current->init << " sz: " << current->size;
            mem_block_desc.push_back(desc.str());
        }
    }

    for (uint block = 0; block < mem_block_desc.size(); block++)
    {
        int len = mem_block_desc[block].size();
        for (int c = 0; c < len + 4; c++)
            std::cout << "*";
        std::cout << "   ";
    }
    std::cout << "\n";

    for (uint block = 0; block < mem_block_desc.size(); block++)
    {
        std::cout << "* " << mem_block_desc[block] << " *";
        if (block != mem_block_desc.size() - 1)
            std::cout << "-->";
    }
    std::cout << "\n";

    for (uint block = 0; block < mem_block_desc.size(); block++)
    {
        int len = mem_block_desc[block].size();
        for (int c = 0; c < len + 4; c++)
            std::cout << "*";
        std::cout << "   ";
    }
    std::cout << "\n";
}
