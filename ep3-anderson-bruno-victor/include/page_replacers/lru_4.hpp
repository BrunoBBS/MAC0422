#ifndef __PG_REP_LRU_4_HPP__

#include "ep_base.hpp"
#include "page_replacer.hpp"

namespace PageReplacers
{
class Lru4 : public PageReplacer
{
  public:
    Lru4(EP &ep);

    bool write(int pos, byte val);

    // Initializes page replacer
    void init();

    /*
     * This function is called every clock pulse of simulation system and
     * updates pages' information about acess and age according to LRU 4th
     * version system
     */
    void clock();

  private:
    // The number of pages the virtual memory is devided in
    int n_pages;
    // Local copy of page_size
    int page_size;

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

    // Table that stores all pages;
    std::vector<Page> page_table;

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
     * a page according to LRU4 rules and updates page's infos.
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

    /*
     * Selects a page for removal from physical memory accordingo to LRU v4 rules and algotihm
     *
     * Returns: The page object to be removed
     */
    Page select_page();
};
}

#endif
