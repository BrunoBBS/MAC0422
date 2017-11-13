#include "page_replacers/lru_2.hpp"

PageReplacers::Lru2::Lru2(EP &ep) :
    PageReplacer(ep, "Least Recently Used v2")
{

}

bool PageReplacers::Lru2::write(int pos, byte val)
{
    return true;
}

void PageReplacers::Lru2::init() {
	return ;
}

int **PageReplacers::Lru2::allocate_matrix(int m, int n) {
	// Tries to allocate memory for matrix
	int **matrix = (int**) malloc(m * sizeof(int));
	for (int i = 0; i <m; i++)
		matrix[i] = (int*) malloc(n * sizeof(int));
	return matrix;
}

void PageReplacers::Lru2::interaction_matrix(int line){
	for (int i = 0; i <m_columns; i++)
		MATRIX[i][line] = 1;

	for (int i = 0; i <m_lines; i++)
		MATRIX[line][i] = 0;
}

int *PageReplacers::Lru2::take_values(){
	int pot = n_pages - 1;
	int *values = (int*) malloc(m_columns*sizeof(int));
	for (int i = 0; i <m_columns; i++)
		values[i] = 0;

	//Calculating the numbers using the binarys
	for (int i = 0; i <m_columns; i++) {
		for (int j = 0; j <m_lines; j++) {
			if (MATRIX[i][j] == 1) {
				values[i] += (2^pot);
			}
		}
		pot-=1;
	}
	return values;
}

int PageReplacers::Lru2::biggest_value(){
	int max = 0;
	for (int i = 1; i <m_lines; i++) {
		if(VALUES[i] > VALUES[max])
			max = i;
	}
	return max;
}

void PageReplacers::Lru2::destroy(){
	for (int i = 0; i <m_columns; i++)
		free(MATRIX[i]);
	free(MATRIX);
	free(VALUES);
}
