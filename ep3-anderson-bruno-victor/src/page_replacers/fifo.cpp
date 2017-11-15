#include "page_replacers/fifo.hpp"

PageReplacers::Fifo::Fifo(EP& ep)
    : PageReplacer(ep, "FIFO")
{
}

bool PageReplacers::Fifo::write(int pos, byte val)
{
    ep.mem_handler()->access(translate_addr(pos), PHYS, val);
    return true;
}

void PageReplacers::Fifo::init()
{
    this->n_pages = ep.virt_size() / ep.get_page_size();
    this->page_size = ep.get_page_size();
    this->n_frames = (ep.phys_size() / page_size);
    this->page_table.resize(this->n_pages);
    this->free_frames.resize(n_frames);
    for (int i = 0; i < n_frames; i++) {
        free_frames[i] = i;
    }
    // Initializes every page
    int i = 0;
    for (auto curr_page : page_table) {
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
    if (!page.presence_bit) {
        // Page Fault!
        std::cerr << "Page Fault!" << std::endl;
        page_fault_cnt++;
        place_page(page);
    }
    return page.phys_addr + offset;
}

void PageReplacers::Fifo::place_page(Page page)
{
    int frame_index;
    // If there is no free frame in physical memory, remove aq page
    if (page_queue.size() == n_frames) {
        remove_page(page_queue.front());
        page_queue.pop();
    }

    // Find first position free in physical memory
    frame_index = free_frames.back();
    free_frames.pop_back();
    std::cerr << "Adding page to frame : " << frame_index << std::endl;
    ep.mem_handler()->copy(page.virt_addr_index * page_size,
        frame_index * page_size, VIRT, PHYS, page_size - 1);
    page.phys_addr = frame_index * page_size;
    page.presence_bit = 1;
    page.R = 1;
    page.M = 0;
    page_queue.push(page);
}

void PageReplacers::Fifo::remove_page(Page page)
{
    std::cerr << "Removing Page from PF : " << page.phys_addr / page_size
              << std::endl;
    // Copies the page frame content to virtual memory if needed
    if (page.M)
        ep.mem_handler()->copy(page.phys_addr, page.virt_addr_index * page_size,
            PHYS, VIRT, page_size - 1);
    ep.mem_handler()->wipe(page.phys_addr, PHYS, page_size - 1);
    free_frames.push_back(page.phys_addr / page_size);
    page.phys_addr = 0;
    page.presence_bit = 0;
    page.R = page.M = 0;
}
