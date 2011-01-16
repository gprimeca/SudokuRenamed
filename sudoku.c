#include <math.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>

#define maxsize 100
typedef int vector[maxsize];
typedef vector matrix[maxsize];

// declare these up here so all functions have access to them.
// n ia the number of rows and colums of the incedence matrix
int n;

// type is A=All Solutions or L=Log
char type;

// the number of sudoku solutions and unique solutions after isom_reject
int num_unique_sudoku;

int counter;

// holds the sudoku solution
matrix A;

// these three are used to make sure each row, column, and block
// have the numbers 1 to n
matrix rows;
matrix cols;
matrix blocks;

struct timeb time_before,time_after;
double total_time_taken;

// define functions used, function headers are below
void backtrack(int, int);
void printMatrix();
void printLevelCounter();
float getTestCaseTime();
void printArray(matrix);

int getBlockNumber(int, int);
int canAddNumber(int, int, int);

int main (int argc, char * const argv[]) 
{	
	if(argc == 1)
	{			
		printf("Enter the size of the sudoku game (4, 6, 8, or 9): ");
		scanf("%u", &n);
		
		printf("Would you like to print all or a log of solutions: (A or L): ");
		scanf("%s", &type);
		
		// start the timer
		ftime(&time_before);
		
		// make sure the file opened(if not the path or permissions could be wrong)
		if ((n == 4 || n == 6 || n == 8 || n == 9) && (type == 'A' || type == 'L'))
		{
			// used for for loop
			int i;
			int j;
						
			// reset num_k_regular
			num_unique_sudoku = 0;
			counter = 0;
			
			printf("n=%u, ", n);
			if(type == 'L')
			{
				printf("Only Solutions whose rank is a power of 2\n");
			}
			else 
			{
				printf("All Solutions\n");
			}
			
			
			// init array to all 2 and 0 on the diagonal
			for(i = 0; i < n; ++i)
			{				
				for(j = 0; j < n; ++j)
				{
					// these arrays are used to keep track of the numbers in each
					// row col and blocks. Sudoku must have one of each (every number)
					// so a complete game would have all three arrays all 1 values.
					rows[i][j] = 0;
					cols[i][j] = 0;
					blocks[i][j] = 0;
					
					// the game board for sudoku
					A[i][j] = 0;
				}			
			}
			
			backtrack(0, -1);
			
			
			printf("\n");
			printf("Total number of unique sudoku solutions found: %u\n", num_unique_sudoku);
			
			printf("-----------\n");
		}
		else
		{
			printf("You entered invalid parameters: use only 4, 6, 8, 9 and A or L \n");
		}
		
		ftime(&time_after);
		total_time_taken = (time_after.time  - time_before.time);
		printf("\nTotal time taken: %f\n", total_time_taken);
	}
	
	
    return 0;
}


/**
 * @brief This function is a recursive function that 
 * will generate all unique solutions to sudoku of size n
 *
 * @param row is the current row
 * @param col is the current col
 * @return nothing
 */
void backtrack(int row, int col)
{			
	// i and j used for looping
	int i;
	int j;
	int k;
	
	int index = row * n + col;
	
	++counter;
	
	// is A a solution?
	// we know it is not a solution is the matrix is not filled out completely.
	// this is the base case of the recursive call.
	if(index == (n * n) - 1)
	{		
		++num_unique_sudoku;
		
		if(type == 'A' || 	
		   (!(num_unique_sudoku == 0) && 
			!(num_unique_sudoku & (num_unique_sudoku - 1)))) // test for power of 2
		{
			printf("\nsolution %u: \n", num_unique_sudoku);
			printMatrix();		
			
			printf("\n");
		}
				
		// the matrix is full, we cannot continue		
		return;
	}	
	
	if(col == n - 1)
	{
		++row;
		col = 0;
	}
	else 
	{
		++col;
		// row is the same
	}
	int orig_row = row;
	int orig_col = col;
	
	index = row * n + col;
	
	if(index < (n * n))
	{
		// FOR LOOP HERE FOR GOING THROUGH ALL POSSIBLE VALUES OF SUDOKU
		for (i = 1; i <= n; ++i)
		{				
			// can we add this value to the row and col?
			if(canAddNumber(row, col, i))
			{
				A[row][col] = i;
				rows[row][i - 1] = 1;
				cols[col][i - 1] = 1;
				blocks[getBlockNumber(row, col)][i - 1] = 1;
				
				backtrack(row, col);
				
				
				// lets put the array back to what it was before we made 
				// the call to backtrack
				for(j = orig_row; j < n; ++j)
				{
					for(k = orig_col; k < n; ++k)
					{								
						if(A[j][k] > 0)
						{
							// reset the rows, cols and blocks matrices
							rows[j][(A[j][k] - 1)] = 0;
							cols[k][(A[j][k] - 1)] = 0;
							blocks[getBlockNumber(j, k)][(A[j][k] - 1)] = 0;					
							A[j][k] = 0;
						}
					}
				}
				
			}
		}			
	}
	
	return;
}

/**
 * @brief This function will print out the matrix
 *
 * @return nothing
 */
void printMatrix()
{	
	//output matrix
	int i;
	int j;
	for(i = 0; i < n; ++i)
	{
		for(j = 0; j < n; ++j)
		{
			printf("%u ", A[i][j]);
		}
		printf("\n");
	}
}

/**
 * Determines if we can add num into current row and col.
 * In each row col and bloack you can have the numbers 1 to n only once.
 * This function checks this.
 *
 * @param row is the current row number
 * @param col is the current col number
 * @param num is the number we want to place in A[row][col]
 */
int canAddNumber(int row, int col, int num)
{
	// if it is not currently in use in this row, col, and block we can add it
	if(rows[row][num - 1] == 0 &&
	   cols[col][num - 1] == 0 &&
	   blocks[getBlockNumber(row, col)][num - 1] == 0)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
	
}

/**
 * Figure out the block number from the row and col parameters.
 * If is hardcoded for 4, 6, 8 and 9 sized games.
 */
int getBlockNumber(int row, int col)
{
	if(n == 4)
	{
		if(row < 2)
		{
			if(col < 2)
				return 0;
			else 
				return 1;
		}
		else 
		{
			if(col < 2)
				return 2;
			else 
				return 3;
		}
	}
	else if(n == 6)
	{
		if(row < 2)
		{
			if(col < 3)
				return 0;
			else 
				return 1;
		}
		else if(row < 4)
		{
			if(col < 3)
				return 2;
			else 
				return 3;
		}
		else 
		{
			if(col < 3)
				return 4;
			else 
				return 5;
		}
	}
	else if(n == 8)
	{
		if(row < 2)
		{
			if(col < 4)
				return 0;
			else 
				return 1;
		}
		else if(row < 4)
		{
			if(col < 4)
				return 2;
			else 
				return 3;
		}
		else if(row < 6)
		{
			if(col < 4)
				return 4;
			else 
				return 5;
		}
		else 
		{
			if(col < 4)
				return 6;
			else 
				return 7;
		}
		
	}
	else
	{
		// we assume 9 is not larger than 9
		if (row < 3)
		{
			if (col < 3)
			{
				return 0;
			}
			else if(col >= 3 && col < 6)
			{
				return 1;
			}
			else
			{
				return 2;
			}
		}
		else if(row >= 3 && row < 6)
		{
			if (col < 3)
			{
				return 3;
			}
			else if(col >= 3 && col < 6)
			{
				return 4;
			}
			else 
			{
				return 5;
			}
		}
		else
		{
			if (col < 3)
			{
				return 6;
			}
			else if(col >= 3 && col < 6)
			{
				return 7;
			}
			else
			{
				return 8;
			}
		}
	}
}


/**
 * Helper function to print out matrix parameter
 */
void printArray(matrix m)
{
	int i;
	int j;
	for(i = 0; i < n; ++i)
	{
		for(j = 0; j < n; ++j)
		{
			printf("%u ", m[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
