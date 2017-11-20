#ifndef __PG_REP_LRU_2_HPP__

#include "ep_base.hpp"
#include "page_replacer.hpp"

#include <stdlib.h> 
#include <list>

namespace PageReplacers
{
    class Lru2 : public PageReplacer
    {
        public:
            Lru2(EP &ep);

            // Calls the lowest level memory write
            bool write(int pos, byte val);

		    // Initializes page replacer
		    void init();

		    /*
		     * This function is called every clock pulse of simulation system and
		     * updates pages' information about acess and age according to LRU 4th
		     * version system
		     */
		    void clock();

		    int **main_matrix;
		    int *main_values;
		    int m_lines, m_columns;

        private:
		    // The number of pages the virtual memory is devided in
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
		        // Page's starting address in physical memory
		        int phys_addr;
		        // Page is in physical memory flag
		        bool presence_bit;
		        // Counter to decide which page is removed from physical memory
		        unsigned char counter;
		        // Read and modified flags
		        bool R, M;

		        // Operator overload for comparison in list
		        inline bool operator==(const page &a)
		        {
		            return this->virt_addr_index == a.virt_addr_index;
		        }
		        inline bool operator>(const page &a)
		        {
		            return this->virt_addr_index > a.virt_addr_index;
		        }
		        inline bool operator<(const page &a)
		        {
		            return this->virt_addr_index < a.virt_addr_index;
		        }
		    };

		    typedef struct page Page;

		    // Table that stores all pages;
		    std::vector<Page> page_table;

		    // Physical memory tracking stack. Useded to know what page_frames are free
		    // to put new pages.
		    std::vector<int> free_frames;

		    // List of pages in physical memory
		    std::list<Page> page_queue;

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
		    void place_page(Page *page);

		    /*
		     * Removes a given page from pysical and writes to virtual memory if needed
		     * and updates page's info bits. Adds page frame from removed page to
		     * free_frames stack.
		     *
		     * Receives: a page object
		     */
		    void remove_page(Page *page);

		    /*
		     * Selects a page for removal from physical memory accordingo to LRU v4
		     * rules and algotihm
		     *
		     * Returns: The page object to be removed
		     */
		    Page *select_page();


		    // Allocate matrix
		    int **allocate_matrix(int m, int n);

		    /* Replace all elements :
		     * matrix[i][line] = 0;
		     * matrix[line][i] = 1;
			 */
		    void interaction_matrix(int line);

		    /* Calculating the value of all lines usign 
		     * the binary matrix and put it in vector[]
		     */
		    void take_values();

		    // Select the biggest valor of the vector
		    int biggest_value();

		    // Print the matrix
		    void print_matrix();

		    // Destroy all pointers
			void destroy();
    };
}

#endif
