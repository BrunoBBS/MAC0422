#include "page_replacers/lru_4.hpp"
#include <bitset>

PageReplacers::Lru4::Lru4(EP& ep)
    : PageReplacer(ep, "Least Recently Used v4")
{
}

bool PageReplacers::Lru4::write(int pos, byte val)
{
    ep.mem_handler()->access(translate_addr(pos), PHYS, val);
    return true;
}

void PageReplacers::Lru4::init()
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
    for (auto& curr_page : page_table) {
        curr_page.R = curr_page.M = curr_page.presence_bit = 0;
        curr_page.virt_addr_index = i++;
    }
}

int PageReplacers::Lru4::translate_addr(int virtual_addr)
{
    // Calculates which page the address refers to
    int page_index = virtual_addr / page_size;
    if (page_index > n_pages)
        return -1;
    // Calculates page relative address
    int offset = virtual_addr % page_size;
    Page page = page_table[page_index];
    // Checks page presence in physical memory
    if (!page.presence_bit) {
        // Page Fault!
        page_fault_cnt++;
        place_page(&page);
    }
    std::cout << "TRANSLATED ADDRESSS : " << page.phys_addr + offset << "\n";
    return page.phys_addr + offset;
}

void PageReplacers::Lru4::place_page(Page *page)
{
    int frame_index;
    // If there is no free frame in physical memory, remove aq page
    if (free_frames.size() == 0) {
        remove_page(select_page());
    }

    // Find first position free in physical memory
    frame_index = free_frames.back();
    free_frames.pop_back();
    std::cout << "copy to : " << frame_index * page_size<< " until : " << frame_index * page_size + page_size << std::endl;
    ep.mem_handler()->copy(page->virt_addr_index * page_size,
        frame_index * page_size, VIRT, PHYS, page_size);
    page->phys_addr = frame_index * page_size;
    std::cout << "PHYSICAL ADDDDRESSS : " << page->phys_addr << "\n";
    page->presence_bit = 1;
    page->counter = 0;
    page->R = 1;
    page->M = 0;
    page_queue.push_back(*page);
}

void PageReplacers::Lru4::remove_page(Page *page)
{
    // Copies the page frame content to virtual memory if needed
    if (page->M)
        ep.mem_handler()->copy(page->phys_addr, page->virt_addr_index * page_size,
            PHYS, VIRT, page_size);
    std::cout << "wipe : " << page->phys_addr << std::endl;
    ep.mem_handler()->wipe(page->phys_addr, PHYS, page_size);
    free_frames.push_back(page->phys_addr / page_size);
    page->phys_addr = 0;
    page->presence_bit = 0;
    page->R = page->M = 0;
}

PageReplacers::Lru4::Page *PageReplacers::Lru4::select_page()
{
    Page* smallest = &page_queue.front();
    for (auto& page : page_queue) {
        if (page.counter < smallest->counter)
            smallest = &page;
    }
    page_queue.remove(*smallest);
    return smallest;
}

void PageReplacers::Lru4::clock()
{
    int add = 0;
    for (auto& page : page_queue) {
        add = (page.R ? 1 : 0) << sizeof(char) * 7;
        page.counter >>= 1;
        page.counter += add;
        page.R = false;
    }
}
