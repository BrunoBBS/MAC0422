#include "space_managers/best_fit.hpp"

SpaceManagers::BestFit::BestFit(EP &ep) :
    SpaceManager(ep, "Best Fit"),
    free_cnt(1)
{}

SpaceManagers::BestFit::~BestFit()
{}

void SpaceManagers::BestFit::init()
{
    // Start anchor starts and ends at block -1
    s_anchor.init = 0;
    s_anchor.size = 0;
    s_anchor.next = &e_anchor;

    // End anchor starts and ends at block next to last usable block
    e_anchor.init = ep.virt_size();
    e_anchor.size = 0;
    e_anchor.next = nullptr;
}

void SpaceManagers::BestFit::end()
{
    // Free processes that were not freed properly
    while (free_cnt > 1)
        free(s_anchor.next->init);
}

void SpaceManagers::BestFit::compress()
{
    // Do for each memory block
    mem_block *current_block = &s_anchor;
    while (current_block->next)
    {
        // If is not an anchor
        if (current_block->next->size)
        {
            // Calculate minimum possible address for next block
            int min_next = current_block->init + current_block->size;

            // Align to page and allocation unit
            int rest;
            if ((rest = (min_next % ep.get_page_size())))
                min_next += ep.get_page_size() - rest;
            
            if ((rest = (min_next % ep.get_alloc_size())))
                min_next += ep.get_alloc_size() - rest;

            // If address has changed
            if (min_next < current_block->next->init)
            {
                // Move process forward
                Process &process = ep.get_process(current_block->next->uid);
                int prev_offset = process.get_offset();
                int prev_end = process.get_offset() + process.get_used_mem();
                process.set_offset(min_next);
                int next_offset = process.get_offset();
                int next_end = process.get_offset() + process.get_used_mem();

                ep.mem_handler()->copy(prev_offset, next_offset,
                        VIRT, VIRT, process.get_used_mem());

                int wipe_from =
                    next_end > prev_offset ? next_end : prev_offset;
                int wipe_to = prev_end;

                ep.mem_handler()->wipe(wipe_from, VIRT, wipe_to - wipe_from);
            }
        }

        // Get next block
        current_block = current_block->next;
    }
}

int SpaceManagers::BestFit::allocate(int size, int uid)
{
    // Can't allocate 0 or less bytes
    if (size <= 0)
        return -1;

    // Minimum size for the process
    int min_req = ((size / ep.get_alloc_size()) +
        (size % ep.get_alloc_size() ? 1 : 0)) * ep.get_alloc_size();

    // Size of biggest free space block available (in allocation unit number)
    int max_size = 10000;

    // Start address of biggest free space block available
    int max_begin = -1;

    // Pointer to the item whose block after it is the biggest
    mem_block *max_prev = nullptr;

    // For each inter-process space
    mem_block *current = &s_anchor;
    for (int space = 0; space < free_cnt; space++, current = current->next)
    //Maybe change the space < free_cnt to >= 
    {
        // Calculate beginning of free space (Aligning to memory pages and
        // allocation units)
        int beginning = current->init + current->size;
        int rest;
        if ((rest = (beginning % ep.get_page_size())))
            beginning += ep.get_page_size() - rest;
        
        if ((rest = (beginning % ep.get_alloc_size())))
            beginning += ep.get_alloc_size() - rest;

        // Calculate end of free space (Aligning to memory pages and allocation
        // units)
        int end = current->next->init;
        if ((rest = (end % ep.get_page_size())))
            end -= rest;
        
        if ((rest = (end % ep.get_alloc_size())))
            end -= rest;

        // Calculate current free space size
        int curr_sz = end - beginning;

        // If process fits and is space is bigger than previous maximum
        if (curr_sz >= min_req && curr_sz < max_size)
        {
            // Update best alternative
            max_size = curr_sz;
            max_prev = current;
            max_begin = beginning; 
        }
    }

    // If no blocks were big enough, return error
    if (!max_prev)
        return -1;

    // Create new block
    mem_block *new_block = new mem_block;
    new_block->init = max_begin;
    new_block->size = min_req;
    new_block->uid = uid;
    
    // Add new block to linked list
    new_block->next = max_prev->next;
    max_prev->next = new_block;

    // We have now one more possible free space
    free_cnt++;

    // Return block memory address
    return new_block->init;
}


void SpaceManagers::BestFit::free(int pos)
{
    // Block starting address
    int start_addr = pos;
    
    // Fail if address is invalid
    if (start_addr < 0 || start_addr >= ep.virt_size() ||
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
        if (current->next->init == start_addr)
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

void SpaceManagers::BestFit::dprint()
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
            desc << "i: " << current->init <<
                " sz: " << current->size <<
                " uid: " << current->uid;
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
