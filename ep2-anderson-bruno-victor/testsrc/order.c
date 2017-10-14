#include "velodrome.h"
#include <stdio.h>

int main() {

	int x[] = {10, 2 ,3, 15};

	qsort (x, sizeof(x)/sizeof(*x), sizeof(*x), compare_scores);

    for (int i = 0 ; i < 4 ; i++)
        printf ("%d ", x[i]);

    return 0;
}