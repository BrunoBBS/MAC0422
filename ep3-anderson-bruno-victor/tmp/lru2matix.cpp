
// Tries to allocate memory for matrix
matrix = malloc(n*sizeof(int));
for (int i = 0; i < n; i++)
	for (int j = 0; j < m; j++)
		matrix[i][j] = malloc(m*sizeof(int));

//Suposed line_process is the line selected to change.
k = line_process;

for (int i = 0; i < n; i++)
	matrix[i][k] = 1;

for (int i = 0; i < m; i++)
	matrix[k][i] = 0;

//-------------------------------------------------
//Selecting the biggest number
//-------------------------------------------------
int pot = n-1; //Size of binary
int sizes = malloc(n *sizeof(int));
for (int i = 0; i < n; i++)
	sizes[i] = 0;

//Calculating the numbers using the binarys
for (int i = 0; i < n; i++) {
	for (int j = 0; j < m; j++) {
		if (matrix[i][j] == 1) {
			size[i] += (2^pot);
		}
	}
	pot-=1;
}

//Selecting the biggest numbers using the sizes[]
int max = 0;
for (int i = 1; i < n; i++) {
	if(sizes[i] > sizes[max])
		max = i;
}

//Destroying the matrix
for (int i = 0; i < n; i++)
	for (int j = 0; j < m; j++)
		free(matrix[i][j]);
free(matrix);
free(sizes);

