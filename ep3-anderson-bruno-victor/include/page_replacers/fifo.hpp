#ifndef __PG_REP_FIFO_HPP__

#include "ep_base.hpp"
#include "page_replacer.hpp"

#include <queue>

namespace PageReplacers
{
class Fifo : public PageReplacer
{
  public:
    Fifo(EP &ep);

    // Calls the lowest level memory write
    bool write(int pos, byte val);

    // Initializes page replacer
    void init();

  private:
    // The number of pages the virtual memory is devided
    int n_pages;
    // Local copy of page_size
    int page_size;
    // The number of page frames the physical memory is divided
    int n_frames;

    struct page
    {
        // Page's virtual address index (is its index in page_table)
        // so the starting virtual address of the page is index * page_size
        int virt_addr_index;
        // Page's address in physical memory
        int page_frame;
        // Page is in physical memory flag
        bool presence_bit;
        // Read and modified flags
        bool R, M;
    };

    typedef struct page Page;
    typedef struct page_queue *Page_queue;

    // Table that stores all pages;
    std::vector<Page> page_table;

    // Page queue for page replacement
    std::queue<Page> page_queue;

    // Physical memory tracking stack. Useded to know what page_frames are free to
    // put new pages.
    std::vector<int> free_frames;

    /*
     * Translates a virtual memory address to a physical one.
     *
     * Receives:   virtual memory address
     * Returns:    physical memory address
     */
    int translate_addr(int virtual_addr);

    /*
     * Page faut time!!
     * Puts a given page in physical memory if there is free space, or replaces
     * a page according to FIFO rules and updates page's infos.
     *
     * Receives: a page object
     */
    void place_page(Page page);

    /*
     * Removes a given page from pysical and writes to virtual memory if needed
     * and updates page's info bits. Adds page frame from removed page to
     * free_frames stack.
     *
     * Receives: a page object
     */
    void remove_page(Page page);
};
}

#endif
