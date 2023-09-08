/*
CMPE 102 Project 1.
Leonardo Blas.
015089646
Ubuntu 20.01.1 LTS.
64 bit.
GNU Compiler Collection (GCC).

mult() uses a loop to multiply two numbers.
If an overflow is detected, it returns the overflowed values.
XOR() uses a mathematical approach to XOR two numbers: 
x xor y = x + y - (2 * (x & y))
rotate() uses the rcl and rcr instructions along with jump directives.
factorial() uses a loop and the imul instruction to obtain the factorial of a number.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Addition. If overflow, return overflowed values in RAX.
long add (long op1, long op2) {
	long output = 0;
	asm (
		"movq %1, %%rax;" 		
		"addq %2, %%rax;"
		"jo overflowDetected;"
		"overflowDetected:"
		: "=a" (output)
		: "m" (op1), "m" (op2)
	 );
	 return output;
}

long mult (long op1, long op2) {
	long output = 0;
	asm (
		"movq %1, %%rcx;"
		"movq $0, %%rax;"
		"multLoop:"
			"addq %2, %%rax;"
			"loop multLoop"
		: "=a" (output)
		: "m" (op1), "m" (op2)
	 );
	 return output;
}

// Using a mathematical approach.
long XOR (long op1, long op2) {
	long output = 0;
	asm (
		"movq %1, %%rax;"
		"and %2, %%rax;"
		"imulq $2, %%rax;"
		"neg %%rax;"
		"addq %2, %%rax;"
		"addq %1, %%rax;" 
		: "=a" (output)
		: "m" (op1), "m" (op2)
	);
	return output;
}

long mod (long op1, long op2) {
	long output = 0;
	long local_op2 = op2;
	asm (
		"movq %1, %%rax;"
		"movq %2, %%rbx;"
		"cdq;"
		"idivq %%rbx;"
		"movq %%rdx, %%rax;"
		: "=a" (output)
		: "m" (op1), "m" (local_op2)
	 ); 
	 return output;
}

long shift (long op1, long direction, long number_of_bits) {
	long output = 0;
	long is_left = direction == 0 ? 1 : 0;
	asm (
		"movq %1, %%rax;"           
		"movq %2, %%rbx;" 	
		"movq %3, %%rcx;" 
		"cmpq $1, %%rbx;"
		"jz Shift_Left;"
		"sarq %%cl, %%rax;"		
		"jmp shift_done;"
		"Shift_Left:"
		"salq %%cl, %%rax;"
		"shift_done:"
	 	: "=a" (output)
		: "m" (op1), "m" (is_left), "m" (number_of_bits)
	 ); 
	 return output;
}

long rotate (long op1, long direction, long number_of_bits) {
	long output = 0;
	long is_left = direction == 0 ? 1 : 0;
	asm (
		"movq %1, %%rax;"           
		"movq %2, %%rbx;" 	
		"movq %3, %%rcx;" 
		"cmpq $1, %%rbx;"
		"jz Rotate_Left;"
		"rcrq %%cl, %%rax;"		
		"jmp rotate_done;"
		"Rotate_Left:"
		"rclq %%cl, %%rax;"
		"rotate_done:"
	 	: "=a" (output)
		: "m" (op1), "m" (is_left), "m" (number_of_bits)
	);
	return output;
}

long factorial (long n) {
	long output = 0;
	asm (
		"movq %1, %%rcx;"
		"subq $1, %%rcx;"
		"movq %1, %%rax;"
		"factorialLoop:"
			"imulq %%rcx, %%rax;"
			"loop factorialLoop"
		: "=a" (output)
		: "m" (n)
	);
	return output;
}

int main(int argc, char** argv) {
	long op1, op2, result;

	op1 = op2 = result = 0;
	
	if (argc != 3) {
		printf("Usage: %s op1 op2 (two integers)\n", argv[0]);
		return 1;
	}

	op1 = atol(argv[1]);
	op2 = atol(argv[2]);

    printf("Operand 1 = %ld x%016lx  Operand 2 = %ld x%016lx\n", op1,op1,op2,op2);
	result = add(op1, op2);
	printf("Add():\t%10ld   x%016lx\n", result, result);
	result = XOR(op1, op2);
	printf("XOR():\t%10ld   x%016lx\n", result, result);
	result = mult(op1, op2);
	printf("Mult():\t%10ld   x%016lx\n", result, result);
	if (op2 == 0) {
	  printf("Mod Error: Divide by 0\n");
	  result = 0;
	}
	else {
  	  result = mod(op1, op2);
	  printf("Mod():\t%10ld   x%016lx\n", result, result);
	}
	if (op2 < 0) {
	  printf("Error: Shift count must be >= 0\n");
	  result = 0;
	}
	else {
  	  result = shift(op1, 0, op2);
	  printf("ShiftL:\t%10ld   x%016lx\n", result, result);
	  result = shift(op1, 1, op2);
	  printf("ShiftR:\t%10ld   x%016lx\n", result, result);
        }
	if (op2 < 0)
	{
	  printf("Error: Rotate count must be >= 0\n");
	  result = 0;
	}
	else
	{
  	  result = rotate(op1, 0, op2);
	  printf("RotateL:%10ld   x%016lx\n", result, result);
	  result = rotate(op1, 1, op2);
	  printf("RotateR:%10ld   x%016lx\n", result, result);
    }
	if (op1 <= 0) {
	  printf("Error: Factorial input must be a positive integer >=1\n");
	  result = 0;
	}
	else {
          result = factorial(op1);
	  printf("Fact():\t%10ld   x%016lx\n\n", result, result);
	}
	return 0;
}

/*
[10/02/21]seed@VM:~/Desktop$ touch readme.p1
[10/02/21]seed@VM:~/Desktop$ gcc task2.c -o task2
[10/02/21]seed@VM:~/Desktop$ ./out 10 5
Operand 1 = 10 x000000000000000a  Operand 2 = 5 x0000000000000005
Add():	        15   x000000000000000f
XOR():	        15   x000000000000000f
Mult():	        50   x0000000000000032
Mod():	         0   x0000000000000000
ShiftL:	       320   x0000000000000140
ShiftR:	         0   x0000000000000000
RotateL:       320   x0000000000000140
RotateR:-6341068275337658368   xa800000000000000
Fact():	   3628800   x0000000000375f00

*/
