#include "page_replacers/lru_4.hpp"

PageReplacers::Lru4::Lru4(EP &ep) : PageReplacer(ep, "Least Recently Used v4")
{
}

bool PageReplacers::Lru4::write(int pos, byte val)
{
    ep.mem_handler()->access(translate_addr(pos), PHYS, val);
    return true;
}

void PageReplacers::Lru4::init()
{
    page_size = ep.get_page_size();
    n_pages = ep.virt_size() / page_size;
    page_table.resize(n_pages);
    free_frames.resize(ep.phys_size() / page_size);
    // Initializes every page
    int i = 0;
    for (auto curr_page : page_table)
    {
        curr_page.R = curr_page.M = curr_page.presence_bit = 0;
        curr_page.virt_addr_index = i++;
    }
}

int PageReplacers::Lru4::translate_addr(int virtual_addr)
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

void PageReplacers::Lru4::place_page(Page page)
{
    int frame_index;
    // If there is no free frame in physical memory, remove aq page
    if (free_frames.size() == 0)
        // remove_page(select_page());

    // Find first position free in physical memory
    frame_index = free_frames.back();
    free_frames.pop_back();
    ep.mem_handler()->copy(page.virt_addr_index * page_size, frame_index * page_size,
                           VIRT, PHYS, page_size);
    page.page_frame = frame_index;
    page.presence_bit = 1;
    page.R = 1;
    page.M = 0;
}

void PageReplacers::Lru4::remove_page(Page page)
{
    // Copies the page frame content to virtual memory if needed
    if (page.M)
        ep.mem_handler()->copy(page.page_frame * page_size,
                               page.virt_addr_index * page_size, PHYS, VIRT, page_size);
}

/*
 * Page PageReplacers::Lru4::select_page()
 * {
 * }
 */

void PageReplacers::Lru4::clock()
{
}
