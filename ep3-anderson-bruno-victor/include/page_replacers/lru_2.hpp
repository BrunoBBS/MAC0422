#ifndef __PG_REP_LRU_2_HPP__

#include "ep_base.hpp"
#include "page_replacer.hpp"
#include <stdlib.h> 

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

		    int **MATRIX;
		    int *VALUES;
		    int m_lines, m_columns;

        private:
        	// The number of pages the virtual memory is devided in
		    int n_pages;
		    // Local copy of page_size
		    int page_size;

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
		    int *take_values();

		    // Select the biggest valor of the vector
		    int biggest_value();

		    // Destroy all pointers
			void destroy();
    };
}

#endif
