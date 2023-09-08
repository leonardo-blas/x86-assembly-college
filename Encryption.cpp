/*
CMPE 102 Project 4.
Leonardo Blas.
015089646
Ubuntu 20.01.1 LTS.
64 bit.
GNU Compiler Collection (GCC).

s_encrypt() encrypts or decrypts A-Z and a-z characters in strings, by doing character shifting.
It may be provided with a direction and number of characters to shift.
The program works as expected. See sample output in the comments.
The most challenging part was hadling the encryption and decryption routine.
Particularly, there was a time I was confident my routine should be working, but it didn't.
Upon closer examination, I noticed that I was trying to count over 127 on a byte register.
After correcting this, my program worked as intended.
Nevertheless, it was a situation that involved a hefty amount of time for debugging.
Additionally, note that this program can be shorter and more optimized.
But it was extended for debugging purposes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "key.h"

__declspec() long s_encrypt(char* src, char* dest, long en_flag);
__declspec() void get_key(long* dir, long* count);
__declspec() void set_key(long dir, long count);

__declspec() void get_key(long* dir, long* count) {
   *dir = direction;
   *count = shiftcount;
}

__declspec() void set_key(long dir, long count) {
   direction = dir;
   shiftcount = count;
}

/*
Returns the # of changed characters in the string. If unchanged, returns zero.
If decrypting, the direction is reversed.
@param src The original string.
@param dest The output string.
@param en_flag 0 for encryption. 1 for decryption.
@return Number of changed characters in the string.
*/
__declspec() long s_encrypt(char* src, char* dest, long en_flag) {
   long characters_changed = 0;
   long len = strlen(src);
   long count = shiftcount % 26;     /* adjust shiftcount if > 26 */
   asm(
      // Assigning the input string to RDI.
      "movq %1, %%rdi;"
      // Assigning the output string to RSI.
      "movq %2, %%rsi;"
      // Assigning count to RCX.
      "movq %5, %%rcx;"
      // Zeroing the return.
      "xor %0, %0;"
      // Checking if encyrpting.
      "cmp $0, %3;"
      // If encrypting, shift characters according to the direction.
      "je loop_input_string;"
      // If decrypting, check the direction flag.
      "cmp $0, %6;"
      // If decrypting and forward, make it backward.
      "je reverse_forward;"
      // Else, if decrypting and backward, make it forward.
      "movq $0, %6;"
      // And continue to iterate over the input string.
      "jmp loop_input_string;"
      // If decrypting and forward, make it backward.
      "reverse_forward:"
         // Make it backward.
         "movq $1, %6;"
      // Iterate over the input string.
      "loop_input_string:"
         // Checking for EOF character.
         "cmpb $0, (%%rdi);"
         // If so, return.
         "je done;"
         // Check for A.
         "cmpb $65, (%%rdi);"
         // If less than A, append in out_string but don't modify.
         "jl append_unchanged_char;"
         // Check for z.
         "cmpb $122, (%%rdi);"
         // If greater than z, append in out_string but don't modify.
         "jg append_unchanged_char;"
         // Check for characters between A and z.
         "cmpb $91, (%%rdi);"
         // If found, append in out_string but don't modify.
         "je append_unchanged_char;"
         // Check for characters between A and z.
         "cmpb $92, (%%rdi);"
         // If found, append in out_string but don't modify.
         "je append_unchanged_char;"
         // Check for characters between A and z.
         "cmpb $93, (%%rdi);"
         // If found, append in out_string but don't modify.
         "je append_unchanged_char;"
         // Check for characters between A and z.
         "cmpb $94, (%%rdi);"
         // If found, append in out_string but don't modify.
         "je append_unchanged_char;"
         // Check for characters between A and z.
         "cmpb $95, (%%rdi);"
         // If found, append in out_string but don't modify.
         "je append_unchanged_char;"
         // Check for characters between A and z.
         "cmpb $96, (%%rdi);"
         // If found, append in out_string but don't modify.
         "je append_unchanged_char;"
         // Increasing the count of changed characters.
         "inc %0;"
         // Moving a character into a temporary register.   
         "movb (%%rdi), %%r10b;"
         // Moving a character into the output string.
         "movb %%r10b, (%%rsi);"
         // Shifting characters on input string.
         "shifting:"
            // Checking if a character is upper cased.
            "cmpb $90, (%%rsi);"
            // If lower cased, shift lower characters.
            "jg lower_case_shifting;"
            // Checking for left direction flag.
            "cmp $1, %6;"
            // If left direction flag, do a upper case left shifting.
            "je upper_case_left_shifting;"
            // Calculating the upper case right shift offset.
            "addb %%cl, %%r10b;"
            // Check if the upper right shift extends beyond Z.
            "cmp $90, %%r10b;"
            // In such case, perform an upper right cycling.
            "jg upper_case_right_cycling;"
            // Else, shift without cycling.
            "addb %%cl, (%%rsi);"
            // Changing a character's case.
            "xor $32, (%%rsi);"
            // Iterate to next character.
            "jmp next_char;"
         // Cycling if the upper right shifting extends beyond Z.
         "upper_case_right_cycling:"
            // Moving the offset into a helper register.
            "movb %%cl, %%r10b;"
            // Calculating total offset.
            "addb (%%rsi), %%r10b;"
            // Calculating offset from A.
            "subb $90, %%r10b;"
            // Adding new offset to one character before A.
            "addb $64, %%r10b;"
            // Adding cycled upper case character into out_string.
            "movb %%r10b, (%%rsi);"
            // Changing a character's case.
            "xor $32, (%%rsi);"
            // Iterate to next character.
            "jmp next_char;"
         // Upper case characters left shifting.
         "upper_case_left_shifting:"
            // Moving an out_string character into a helper register.   
            "movb (%%rsi), %%r10b;"
            // Calculating the upper case left shift offset.
            "subb %%cl, %%r10b;"
            // Check if the upper left shift extends beneath A.
            "cmp $65, %%r10b;"
            // In such case, perform a upper left cycling.
            "jl upper_case_left_cycling;"
            // Else, shift without cycling.
            "subb %%cl, (%%rsi);"
            // Changing a character's case.
            "xor $32, (%%rsi);"
            // Iterate to next character.
            "jmp next_char;"
         // Cycling if the upper left shifting extends beyond Z.
         "upper_case_left_cycling:"
            // Moving a character into a helper register.
            "movb (%%rsi), %%r10b;"
            // Calculating total offset.
            "subb %%cl, %%r10b;"
            // Calculating offset distance from A.
            "movb $65, %%r11b;"
            // Calculating offset from Z.
            "subb %%r10b, %%r11b;"
            // Adding new offset to one character after Z.
            "movb $91, %%r10b;"
            // Calculating new position relative to Z.
            "subb %%r11b, %%r10b;"
            // Adding cycled upper case character into out_string.
            "movb %%r10b, (%%rsi);"
            // Changing a character's case.
            "xor $32, (%%rsi);"
            // Iterate to next character.
            "jmp next_char;"
         // Shifting lower case characters.
         "lower_case_shifting:"
            // Checking for left direction flag.
            "cmp $1, %6;"
            // If left direction flag, do a lower case left shifting.
            "je lower_case_left_shifting;"
            // Calculating the lower case right shift offset.
            "addb %%cl, %%r10b;"
            // Check if the upper right shift extends beyond z.
            "cmp $122, %%r10;"
            // In such case, perform a lower right cycling.
            "jg lower_case_right_cycling;"
            // Else, shift without cycling.
            "addb %%cl, (%%rsi);"
            // Changing a character's case.
            "xor $32, (%%rsi);"
            // Iterate to next character.
            "jmp next_char;"
         // Cycling if the upper lower shifting extends beyond z.
         "lower_case_right_cycling:"
            // Moving the offset into a helper register.
            "movb %%cl, %%r10b;"
            // Calculating total offset.
            "addb (%%rsi), %%r10b;"
            // Calculating offset from a.
            "subb $122, %%r10b;"
            // Adding new offset to one character before a.
            "addb $96, %%r10b;"
            // Adding cycled lower case character into out_string.
            "movb %%r10b, (%%rsi);"
            // Changing a character's case.
            "xor $32, (%%rsi);"
            // Iterate to next character.
            "jmp next_char;"
         // Lower case characters left shifting.
         "lower_case_left_shifting:"
            // Moving an out_string character into a helper register.   
            "movb (%%rsi), %%r10b;"
            // Calculating the lower case left shift offset.
            "subb %%cl, %%r10b;"
            // Check if the lower left shift extends beneath a.
            "cmp $97, %%r10b;"
            // In such case, perform a lower left cycling.
            "jl lower_case_left_cycling;"
            // Else, shift without cycling.
            "subb %%cl, (%%rsi);"
            // Changing a character's case.
            "xor $32, (%%rsi);"
            // Iterate to next character.
            "jmp next_char;"
         // Cycling if the lower left shifting extends beyond a.
         "lower_case_left_cycling:"
            // Moving a character into a helper register.
            "movb (%%rsi), %%r10b;"
            // Calculating total offset.
            "subb %%cl, %%r10b;"
            // Calculating offset distance from a.
            "movb $97, %%r11b;"
            // Calculating offset from z.
            "subb %%r10b, %%r11b;"
            // Adding new offset to one character after z.
            "movb $123, %%r10b;"
            // Calculating new position relative to Z.
            "subb %%r11b, %%r10b;"
            // Adding cycled lower case character into out_string.
            "movb %%r10b, (%%rsi);"
            // Changing a character's case.
            "xor $32, (%%rsi);"
            // Iterate to next character.
            "jmp next_char;"
      // Appending chars that are not A-Z or a-z to out_string.
      "append_unchanged_char:"
         // Moving a character into a temporary register.   
         "movb (%%rdi), %%r10b;"
         // Moving a character into the output string.
         "movb %%r10b, (%%rsi);"
      // Moving onto the next characters.
      "next_char:"
         // Moving onto the next character on in_string.
         "inc %%rdi;"
         // Moving onto the next character on out_string.
         "inc %%rsi;"
      // Next iteration.
      "jmp loop_input_string;"
      // Done scope.
      "done:"
         : "=a" (characters_changed)
         : "r" (src), "m" (dest), "m" (en_flag), "m" (len), "m" (count), "m" (direction)
         :
   );
   return characters_changed;
}

int main(int argc, char** argv)
{
	char *in_string, out_string[128];
	long enc_flag;
	long rc = 0;
	long dir = 0, count = 0;
	
	if (argc < 3)
	{
		printf("Usage #1: %s input_string encrypt_flag\n", argv[0]);		
		printf("Usage #2: %s input_string encrypt_flag direction shiftcount\n", argv[0]);	
		return 1;
	}

	in_string = argv[1];
	enc_flag = atol(argv[2]);
	enc_flag = (enc_flag == 0) ? 0 : 1;

	if (argc == 5)
	{
		dir = atol(argv[3]);
		count = atol(argv[4]);

    	if ((dir < 0 || dir > 1) || (count < 0))
    	{
        	printf("Invalid direction or shiftcount\n");
			return 1;
    	}
		set_key(dir, count);
	}

	get_key(&dir, &count);
    printf("Current keys: Direction=%ld, Shiftcount=%ld\n", dir, count);

    rc = s_encrypt( in_string, out_string, enc_flag);

	printf( "Source string = %s\n", in_string);
	printf( "Target string = %s\n", out_string);
 	printf( "Number of characters converted = %ld\n\n", rc );
 
  return rc;
}

/*
C:\Users\wapen\Desktop>p4 "FRANK" 0 0 190
Current keys: Direction=0, Shiftcount=190
Source string = FRANK
Target string = nzivs
Number of characters converted = 5


C:\Users\wapen\Desktop>p4 "nzivs" 1 0 190
Current keys: Direction=0, Shiftcount=190
Source string = nzivs
Target string = FRANK
Number of characters converted = 5


C:\Users\wapen\Desktop>p4 "123SpOnGe!@#" 0 0 72
Current keys: Direction=0, Shiftcount=72
Source string = 123SpOnGe!@#
Target string = 123mJiHaY!@#
Number of characters converted = 6


C:\Users\wapen\Desktop>p4 "123mJiHaY!@#" 1 0 72
Current keys: Direction=0, Shiftcount=72
Source string = 123mJiHaY!@#
Target string = 123SpOnGe!@#
Number of characters converted = 6


C:\Users\wapen\Desktop>p4 "squidWARD$^&%^" 0 1 72
Current keys: Direction=1, Shiftcount=72
Source string = squidWARD$^&%^
Target string = YWAOJcgxj$^&%^
Number of characters converted = 9


C:\Users\wapen\Desktop>p4 "YWAOJcgxj$^&%^" 1 1 72
Current keys: Direction=1, Shiftcount=72
Source string = YWAOJcgxj$^&%^
Target string = squidWARD$^&%^
Number of characters converted = 9
*/