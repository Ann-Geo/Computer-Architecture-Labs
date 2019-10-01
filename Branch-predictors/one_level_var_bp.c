/**********************************************************************************************************
Filename: one_level_var_bp
Description: Simulates one level branch prediction scheme with branch history table size 1K and variable
counter size.
How to run this program:
To compile: gcc -o one_level_var_bp one_level_var_bp.c -lm
To run: ./one_level_var_bp <trace_file_name>
***********************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

/**********************************************************************************************************
Function name: findoperation
Description: Finds the the branch is taken or not taken from the trace file.
***********************************************************************************************************/

int findoperation(char oper_from_address[]) 
{
	int length = strlen(oper_from_address);
	int operation_in_trace = oper_from_address[length - 2];
	return operation_in_trace;
}

/**********************************************************************************************************
Function name: getaddress
Description: Gets the brach address from the trace file.
***********************************************************************************************************/

unsigned int getaddress(char dec_address[])
{
	int length = strlen(dec_address);
	unsigned int bit_level_rep = 0;
	unsigned int digit;
	unsigned int power;
	unsigned int product;
	for(int i = 0; i<length-3; i++)
	{
		digit = dec_address[i] - 48;
		power = pow(10, ((length-4)-i));
		product = (unsigned int)(digit*power);
		bit_level_rep = bit_level_rep + product;
		
   	}
	return bit_level_rep;
}

/**********************************************************************************************************
Function name: main
Description: Implements variable size state counter for each entry in the branch history table.
***********************************************************************************************************/

int main(int argc, char *argv[]) 
{		
	char* tracefile = argv[1];
	int bht[1024];
	


	for(int i =0; i<1024; i++)
	{
		bht[i] = 0;
	}


	char line_of_trace[20];
   	FILE *pointer_to_line_in_trace;
	int operation_in_trace;
	unsigned int b_address;
	unsigned int lower;
	unsigned int mispred = 0;
	unsigned int branches = 0;
	unsigned int pred = 0;
	unsigned int p;
	unsigned int countersize = *argv[2] - 48;
	unsigned int half = (pow(2, countersize))/2;

   	pointer_to_line_in_trace = fopen(tracefile , "r");
   	
	while((fgets(line_of_trace, 20, pointer_to_line_in_trace)!= NULL))
	{
		branches++;
		operation_in_trace = findoperation(line_of_trace);
		b_address = getaddress(line_of_trace);

		lower = b_address & 0x3FF;

			if(bht[lower] < half)
			{
				p = 0;
				if(operation_in_trace == 78)
				{
					pred++;
					if(bht[lower] > 0)
					bht[lower] = bht[lower] - 1;
					else
					bht[lower] = p;
				}
				else
				{
					bht[lower] = bht[lower] + 1;
					mispred++;

				}

			}
			else
			{
				p = (pow(2, countersize)) - 1;
				if(operation_in_trace == 84)
				{
					pred++;
					if(bht[lower] < p)
					bht[lower] = bht[lower] + 1;
					else
					bht[lower] = p;
				}
				else
				{
					bht[lower] = bht[lower] - 1;
					mispred++;

				}
					
			}

	}

	printf("number of branches = %d\n", branches);
	printf("number of mispredictions = %d\n", mispred);
	printf("number of correct predictions = %d\n", pred);
	printf("Misprediction Percentage = %2.2f\n", (double)mispred/branches*100);

}


































