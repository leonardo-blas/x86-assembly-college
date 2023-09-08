/*
CMPE 102 Project 3.
Leonardo Blas.
015089646
Ubuntu 20.01.1 LTS.
64 bit.
GNU Compiler Collection (GCC).

search_by_name() searches for a student's name and returns their ID.
Linear search and case insensitive function.
The program works as expected. See sample output in the comments.
The most challenging part was handling characters through registers.
Initially, handling characters was attempted through registers AL and BL.
The idea was to iterate and perform operations on individual characters.
Nevertheless, programs using AL and BL did not work as expected.
Sometimes, they even produced different outputs.
Many programs were written and many factors were changed, except the usage of AL and BL.
After researching, it was decided to use RSI, R10B, and R11B to handle characters.
Using these registers, the program was finished in just a couple of hours.
Designing the program's logic and architecture was not too hard.
Nevertheless, using the appropriate registers to handle characters was challenging.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student {
  long ID;
  char name[24];
};

/* This function returns the char* to the name string if the input sid has a match on the student ID
from the array of student structures */
long search_by_id(char* list, long count, long sid) {
	long index = 0;
	asm(
		"mov %1, %%rax;"
		"mov %2, %%rbx;"
		"mov %3, %%rcx;"
		"xor %%rdx, %%rdx;"
		"loop_start:"
			"cmp %%rdx, %%rcx;"
			"je not_found;"
			"mov (%%rax), %%rsi;"
			"cmp %%rsi, %%rbx;"
			"je found;"
			"add $32, %%rax;"
			"inc %%rdx;"	
			"jmp loop_start;"
		"not_found:"
			"xor %%rax, %%rax;"
			"jmp done;"
		"found:"
			"add $8, %%rax;"
		"done:"
		 	: "=a" (index)
			: "m" (list), "m" (sid), "m" (count)
			:
	);
	 return index;
}

/*
Searches for a student by name and returns their ID.
Else, returns 0.
@param list Starting address of the list of structs to search on.
@param count Total number of names in the list.
@param token Name to search.
@return The ID if the name is found. Else, 0.
*/
long search_by_name(char* list, long count, char* token) {
	long index = 0;
	asm(
		// Assigning list to RAX.
		"mov %1, %%rax;"
		// Assigning count to RCX.
		"mov %3, %%rcx;"
		// Handling the first name through RAX.
		"add $8, %%rax;"
		// Zeroing our counter, RDX.
		"xor %%rdx, %%rdx;"
		// Iterating over the list.
		"loop_list:"
			// Checking counter against count.
			"cmp %%rcx, %%rdx;"
			// If counter reaches count, name not in list.
			"je name_not_found;"
			// Assigning the current name's handler to RSI.
			"mov %%rax, %%rsi;"
			// Assign the token to RBX.
			"mov %2, %%rbx;"
			// Iterating over a name and the token.
			"loop_name_token:"
				// Checking for a null char in a name.
				"cmpb $0, (%%rsi);"
				// If not null, jumping to name_char_not_null.
				"jne name_char_not_null;"
				// Checking for a null char in token.
				"cmpb $0, (%%rbx);"
				// If simultaneous null, name found.
				"je name_found;"
				// If not simultaneous null, comparing chars.
				"name_char_not_null:"
					// Assigning a name's char to R10B.
					"mov (%%rsi), %%r10b;"
					// Assigning a token's char to R11B.
					"mov (%%rbx), %%r11b;"
					// Lowering a name's char.
					"or $32, %%r10b;"
					// Lowering a token's char.
					"or $32, %%r11b;"
					// Comparing both chars.
					"cmpb %%r10b, %%r11b;"
					// If not equal, iterating to next student.
					"jne next_student;"
					// Iterating to a name's next char.
					"inc %%rsi;"
					// Iterating to token's next char.
					"inc %%rbx;"
					// Iterating over a name and the token again.
					"jmp loop_name_token;"
			// Iterating to the next student in list.
			"next_student:"
				// Increasing the counter.
				"inc %%rdx;"
				// Handling the next name through RAX.
				"add $32, %%rax;"
				// Iterating over the list again.
				"jmp loop_list;"
		// Managing the case token is not found.
		"name_not_found:"
			// Zeroing RAX.
			"xor %%rax, %%rax;"
			// Jumping to done scope.
			"jmp name_done;"
		// Managing the case token is found.
		"name_found:"
			// Handling the list's first ID through RSI.
			"mov %1, %%rsi;"
			// Calculating RSI's offset through our counter.
			"imul $32, %%rdx;"
			// Adding the offset on RSI.
			"add %%rdx, %%rsi;"
			// Returning the found student's ID.
			"mov (%%rsi), %0;"
		// Done scope.
		"name_done:"
 	 		: "=a" (index)
	 		: "m" (list), "m" (token), "m" (count)
	 		:
	);
	return index;
}

int main(int argc, char** argv) {
	char *token;
	char *sname=NULL;
	FILE *fptr;
	long id = 0;
	long sid = 0;
	long i = 0;
	struct student list[100];	/* array of structure */
	
	if (argc != 4)
	{
		printf("Usage: %s filename token student_id\n", argv[0]);
		return 1;
	}

	token = argv[2];

	sid = atol(argv[3]);

	printf("\n");

	fptr = fopen((argv[1]), "rtc");
	if (fptr == NULL)
      printf( "File %s was not opened\n",argv[1] );
	else
   {
      /* Set pointer to beginning of file: */
      fseek( fptr, 0L, SEEK_SET );

      /* Read data from file: */
	  while ( fscanf(fptr, "%s", (char*) &list[i].name) != EOF )
	  {
	  list[i].ID = i+1001;
          printf( "%s ", list[i].name );
		  i++;
	  }

      printf( "\n\nNumber of names = %ld\n", i );
      printf( "Search Token = %s\n", token );
      fclose( fptr );
   }

	printf( "\nStudent Id = %ld; Name = %s.\n\n", sid, (char*)search_by_id((char*)list, i, sid));

   id = search_by_name ((char*)list, i, token);

   if (id)
	   printf( "Student Id = %ld; Name = %s\n", id, list[id-(1001)].name );
   else
     printf( "Student is not found.\n");

   return 0;
}

/*
seed@VM:~/Desktop$ gcc p3.cpp -o p3
seed@VM:~/Desktop$ ./p3 p3.data2 BRIan 1002

Brian Bryan chris Christopher Christy DAVE David Johnny Jon Jonathan Tom Tomas Tommy 

Number of names = 13
Search Token = Brian

Student Id = 1002; Name = Bryan.

Student Id = 1001; Name = Brian
seed@VM:~/Desktop$ ./p3 p3.data2 BRYAN 1002

Brian Bryan chris Christopher Christy DAVE David Johnny Jon Jonathan Tom Tomas Tommy 

Number of names = 13
Search Token = BRYAN

Student Id = 1002; Name = Bryan.

Student Id = 1002; Name = Bryan
seed@VM:~/Desktop$ ./p3 p3.data2 ChrIS 1002

Brian Bryan chris Christopher Christy DAVE David Johnny Jon Jonathan Tom Tomas Tommy 

Number of names = 13
Search Token = ChrIS

Student Id = 1002; Name = Bryan.

Student Id = 1003; Name = chris
seed@VM:~/Desktop$ ./p3 p3.data2 CHrist 1002

Brian Bryan chris Christopher Christy DAVE David Johnny Jon Jonathan Tom Tomas Tommy 

Number of names = 13
Search Token = CHrist

Student Id = 1002; Name = Bryan.

Student is not found.
seed@VM:~/Desktop$ ./p3 p3.data2 davE 1002

Brian Bryan chris Christopher Christy DAVE David Johnny Jon Jonathan Tom Tomas Tommy 

Number of names = 13
Search Token = davE

Student Id = 1002; Name = Bryan.

Student Id = 1006; Name = DAVE
seed@VM:~/Desktop$ ./p3 p3.data2 dav 1002

Brian Bryan chris Christopher Christy DAVE David Johnny Jon Jonathan Tom Tomas Tommy 

Number of names = 13
Search Token = dav

Student Id = 1002; Name = Bryan.

Student is not found.
seed@VM:~/Desktop$ ./p3 p3.data2 JoN 1002

Brian Bryan chris Christopher Christy DAVE David Johnny Jon Jonathan Tom Tomas Tommy 

Number of names = 13
Search Token = JoN

Student Id = 1002; Name = Bryan.

Student Id = 1009; Name = Jon
seed@VM:~/Desktop$ ./p3 p3.data2 SPONgeBob 1001

Brian Bryan chris Christopher Christy DAVE David Johnny Jon Jonathan Tom Tomas Tommy 

Number of names = 13
Search Token = SPONgeBob

Student Id = 1001; Name = Brian.

Student is not found.
*/
