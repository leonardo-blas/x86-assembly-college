/*
CMPE 102 Project 2.
Leonardo Blas.
015089646
Ubuntu 20.01.1 LTS.
64 bit.
GNU Compiler Collection (GCC).

sorter() implements the selection sort in assembly.
The program works as expected. See sample output in the comments.
The most challenging part was dealing with several labels and counters, but
this situation was resolved through a careful management of labels and the
registers used to track our loops' progress.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Selection sort in assembly.
@param list A pointer to the list to be sorted.
@param count The number of items in the list.
@param opcode 1 for ascending or 2 for descending sorting.
@return void.
*/
void sorter(long* list, long count, long opcode) {
	asm (
		// Store array pointer into rax.
		"movq %0, %%rax;"
		// Store opcode into rbx.
		"movq %1, %%rbx;"
		// Set outer loop's limit to count.
		"movq %2, %%rsi;"
		// Set inner loop's limit to count.
		"movq %2, %%rdi;"
		// Set inner loop's limit to (count + 1).
		"inc %%rdi;"
		// Set outer loop's counter, x, to -1.
		"movq $-1, %%rcx;"
		// Outer loop's scope beginning.
		"outer_loop:"
			// Increase x.
			"inc %%rcx;"
			// Compare x to count.
			"cmpq %%rcx, %%rsi;"
			// Exit outer loop if x == count.
			"je done;"	
			// Set inner loop's counter, y, to -1.
			"mov $-1, %%rdx;"
			// Inner loop's scope beggining.
			"inner_loop:"
				// Increase y.
				"inc %%rdx;"
				// Compare y to count.
				"cmpq %%rdx, %%rdi;"
				// Exit inner loop if y == count + 1.
				"je outer_loop;"
				// Move list[x] into r10.
				"movq (%%rax, %%rcx, 8), %%r10;"
				// Move list[y] into r11.
				"movq (%%rax, %%rdx, 8), %%r11;"
				// Check if opcode is ASC.
				"cmpq $1, %%rbx;"
				// If ASC, jump to asc label.
				"je asc;"
				// Check if opcode is DESC.
				"cmpq $2, %%rbx;"
				// If DESC, jump to desc label.
				"je desc;"
				// asc label.
				"asc:"
					// Compare list[x] with list[y].
					"cmpq %%r10, %%r11;"
					// Break if list[x] < list[y].
					"jl inner_loop;"
					// Else, jump to inner_continued and skip desc label.
					"jmp inner_continued;"
				// desc label.
				"desc:"
					// Compare list[x] with list[y].
					"cmpq %%r10, %%r11;"
					// Break if list[x] > list[y].
					"jg inner_loop;"
				// Label to skip desc operations if opcode is asc.
				"inner_continued:"
					// Exchange list[x] with list[y].
					"xchgq %%r10, (%%rax, %%rdx, 8);"
					// Exchange list[y] with list[x].
					"xchgq %%r11, (%%rax, %%rcx, 8);"
				// Inner loop's scope end.
				"jmp inner_loop;"
			// Outer loop's scope end.
			"jmp outer_loop;"
		// done scope.
		"done:"
		:
		: "m" (list), "m" (opcode), "m" (count)
		:
	);
}

int main(int argc, char** argv) {
	long numlist[1000], asc;
	FILE *fptr;

	long i = 0;
	
	if (argc != 3) {
		printf("Usage: %s filename asc_des\n", argv[0]);
		return 1;
	}

	asc = atol (argv[2]);
	asc = (asc == 1) ? 1 : 2;

	printf("\n");

	fptr = fopen((argv[1]), "rtc");
	if (fptr == NULL) {
		printf( "File %s was not opened\n",argv[1] );
	}
	else {
		/* Set pointer to beginning of file: */
		fseek( fptr, 0L, SEEK_SET );

		/* Read data from file: */
		while ( fscanf(fptr, "%ld", &numlist[i]) != EOF ) {
		printf( "%ld ", numlist[i] );
		i++;
		}

		printf( "\n\nNumber of integer = %ld\n", i );
		printf( "Ascend_or_Descend = %ld\n\n", asc );
		fclose( fptr );
	}

	sorter( numlist, i, asc);

	for (int j = 0; j < i; j++) {
		printf( "%ld ", numlist[j] );
	}

	printf("\n");

	return 0;
}

/*
[10/21/21]seed@VM:~/Desktop$ ./out p2.data1 1

10 -20 5 12 30 -5 -22 55 52 0 

Number of integer = 10
Ascend_or_Descend = 1

-22 -20 -5 0 5 10 12 30 52 55 
[10/21/21]seed@VM:~/Desktop$ ./out p2.data1 2

10 -20 5 12 30 -5 -22 55 52 0 

Number of integer = 10
Ascend_or_Descend = 2

55 52 30 12 10 5 0 -5 -20 -22 
[10/21/21]seed@VM:~/Desktop$ ./out p2.data2 1

48 -24 48 -24 10 100 -10 60 -256 10 -10 4096 -1024 60 10 -10 

Number of integer = 16
Ascend_or_Descend = 1

-1024 -256 -24 -24 -10 -10 -10 10 10 10 48 48 60 60 100 4096 
[10/21/21]seed@VM:~/Desktop$ ./out p2.data2 2

48 -24 48 -24 10 100 -10 60 -256 10 -10 4096 -1024 60 10 -10 

Number of integer = 16
Ascend_or_Descend = 2

4096 100 60 60 48 48 10 10 10 -10 -10 -10 -24 -24 -256 -1024 
*/

	


