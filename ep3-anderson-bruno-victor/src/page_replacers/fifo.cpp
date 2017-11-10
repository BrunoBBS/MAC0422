#include "page_replacers/fifo.hpp"

PageReplacers::Fifo::Fifo(EP &ep) : PageReplacer(ep, "FIFO")
{
}

bool PageReplacers::Fifo::write(int pos, byte val)
{
    memory->access(translate_addr(pos), PHYS, val);
    return true;
}

void PageReplacers::Fifo::init()
{
    this->n_pages = ep.virt_size() / ep.get_page_size();
    this->page_size = this->n_pages;
    this->page_table.resize(this->n_pages);
    this->free_frames.resize(ep.phys_size() / this->page_size);
    // Initializes every page
    int i = 0;
    for (auto curr_page : page_table)
    {
        curr_page.R = curr_page.M = curr_page.presence_bit = 0;
        curr_page.virt_addr_index = i++;
    }
}

int PageReplacers::Fifo::translate_addr(int virtual_addr)
{
    // Calculates which page the address refers to
    int page_index = virtual_addr / page_size;
    // Calculates page relative address
    int offset = virtual_addr % page_size;
    Page page = page_table[page_index];
    // Checks page presence in physical memory
    if (!page.presence_bit)
    {
        // Page Fault!
        page_fault_cnt++;
        place_page(page);
    }
    return page.page_frame + offset;
}

void PageReplacers::Fifo::place_page(Page page)
{
    int frame_index;
    // If there is no free frame in physical memory, remove aq page
    if (free_frames.size() == 0)
        remove_page(page_queue.back());

    // Find first position free in physical memory
    frame_index = free_frames.back();
    free_frames.pop_back();
    memory->copy(page.virt_addr_index * page_size, frame_index * page_size,
                 VIRT, PHYS, page_size);
    page.page_frame = frame_index;
    page.presence_bit = 1;
    page.R = 1;
    page.M = 0;
}

void PageReplacers::Fifo::remove_page(Page page)
{
    // Copies the page frame content to virtual memory if needed
    if (page.M)
        memory->copy(page.page_frame * page_size,
                     page.virt_addr_index * page_size, PHYS, VIRT, page_size);
    // TODO: colocar -1  na memoria (fazer uma função no memory.hpp)
    free_frames.push_back(page.page_frame);
    page.page_frame = 0;
    page.presence_bit = 0;
    page.R = page.M = 0;
}
