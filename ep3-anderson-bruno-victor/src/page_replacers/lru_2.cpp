#include "page_replacers/lru_2.hpp"
#include <bitset>
#include <climits>
#include <math.h>

PageReplacers::Lru2::Lru2(EP &ep) :
    PageReplacer(ep, "Least Recently Used v2")
{

}

bool PageReplacers::Lru2::write(int pos, byte val)
{
    ep.mem_handler()->access(translate_addr(pos), PHYS, val);
    return true;
}

void PageReplacers::Lru2::init() 
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
    // Initializes the main matrix
    main_matrix = allocate_matrix(n_pages, n_pages);
    m_lines = m_columns = n_pages;
    for (int i = 0; i < n_pages; i++) {
    	for (int j = 0; j < n_pages; j++) 
    		main_matrix[i][j] = 0;
    }
    main_values = new int[n_pages];
    for (int i = 0; i < n_pages; i++)
        main_values[i] = 0;
}

int PageReplacers::Lru2::translate_addr(int virtual_addr)
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

void PageReplacers::Lru2::place_page(Page *page)
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

void PageReplacers::Lru2::remove_page(Page *page)
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

PageReplacers::Lru2::Page *PageReplacers::Lru2::select_page()
{
    int index, count = 0;
    std::cout << "Selecting page with smallest counter\n";
    take_values();
    index = biggest_value();
    Page* smallest = &page_queue.front();
    for (auto& page : page_queue ) {
	    if (count < index) {
            smallest = &page;
	        count++;
	    }
	    else break;
    }
    page_queue.remove(*smallest);
    return smallest;
}

void PageReplacers::Lru2::clock()
{
    int add = 0;
    for (auto& page : page_queue) {
        // If the page was used, change the main matrix 
        if (page.R == 1) {
        	interaction_matrix(page.virt_addr_index);
        }
        add = page.R ? 1 : 0 << sizeof(char) * 7;
        page.counter >>= 1;
        page.counter += add;
        page.R = false;
    }
}

int **PageReplacers::Lru2::allocate_matrix(int m, int n) {
	// Tries to allocate memory for matrix
	main_matrix = new int*[m];    
	if(main_matrix == NULL) {
		return NULL;
	}
	for (int i = 0; i <m; i++) {
		main_matrix[i] = new int[n];
		if (main_matrix[i] == NULL){
			return NULL;
		}
	}
	return main_matrix;
}

void PageReplacers::Lru2::interaction_matrix(int line){
	for (int i = 0; i <m_lines; i++)
		main_matrix[line][i] = 1;
	for (int i = 0; i <m_columns; i++)
		main_matrix[i][line] = 0;
	return;
}

void PageReplacers::Lru2::take_values(){
	int pot = n_pages;
    int i, j;
	//Calculating the numbers using the binarys
	for (i = 0; i < n_pages; i++) {
		for (j = 0; j < n_pages; j++) {
            if (main_matrix[i][j] == 1) {
				main_values[i] += 1<<pot;
			}
            pot-=1;
		}
        if (main_matrix[i][j-1] == 1 && j == n_pages) {
            main_values[i] += 1;
		}
	}
}

void PageReplacers::Lru2::print_matrix() {
	for (int i = 0; i < n_pages; i++) {
		for (int j = 0; j < n_pages; j++) {
			printf("%d ", main_matrix[i][j]);
		}
		printf("\n");
	}
}

int PageReplacers::Lru2::biggest_value(){
	int index = 0, max = INT_MAX;
    for (auto page : page_queue) {
        // If the page was used, change the main matrix 
        if (page.R == 0 ) {
            if (main_values[page.virt_addr_index] < max) {
        	    index = page.virt_addr_index;
                max = main_values[index];
            }
        }
    }
    return index;
}
void PageReplacers::Lru2::destroy(){
	delete main_matrix;
	delete main_values;
}