/**********************************************************************************************************
Filename: two_level_gshare_var_bp
Description: Simulates two level gshare branch prediction scheme with branch history table size 1K and 
variable counter size.
How to run this program:
To compile: gcc -o two_level_gshare_var_bp two_level_gshare_var_bp.c -lm
To run: ./two_level_gshare_var_bp <trace_file_name> <counter_size_in_bits>
For 2 bit counter, counter_size_in_bits will be 2.
***********************************************************************************************************/

#include <stdio.h>
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

	unsigned int ghisreg = 0;
	unsigned int b_address;
	unsigned int lower;

	unsigned int mispred = 0;
	unsigned int branches = 0;
	unsigned int pred = 0;

	unsigned int bit;
	unsigned int xorres;


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


		if(branches == 1)
		{
			bit = 0;
			xorres = lower^ghisreg;

			bht[xorres] = 0;

			if(operation_in_trace == 78)
			{
				pred++;
				ghisreg = (ghisreg | bit) & 0x3FF;
			}
			else
			{
				bit = 1;
				bht[xorres] = 1;
				mispred++;
				ghisreg = (ghisreg | bit) & 0x3FF;
			}


		}

		else
		{

			xorres = lower^ghisreg;

			if(bht[xorres] < half)
			{
				p = 0;

				if(operation_in_trace == 78)
				{
					bit = 0;
					pred++;
					if(bht[xorres] > 0)
					bht[xorres] = bht[xorres] - 1;
					else
					bht[xorres] = p;					
					ghisreg = ((ghisreg << 1) | bit) & 0x3FF;
				}
				else
				{
					bit = 1;
					bht[xorres] = bht[xorres] + 1;
					mispred++;
					ghisreg = ((ghisreg << 1) | bit) & 0x3FF;
				}


			}

			else
			{
				
				p = (pow(2, countersize)) - 1;
				if(operation_in_trace == 84)
				{
					pred++;					
					bit = 1;
					if(bht[xorres] < p)
					bht[xorres] = bht[xorres] + 1;
					else
					bht[xorres] = p;
					ghisreg = ((ghisreg << 1) | bit) & 0x3FF;
				}
				else
				{
					bit = 0;
					bht[xorres] = bht[xorres] - 1;
					mispred++;
					ghisreg = ((ghisreg << 1) | bit) & 0x3FF;
				}


			}

						
		}
	}

	printf("number of branches = %d\n", branches);
	printf("number of mispredictions = %d\n", mispred);
	printf("number of correct predictions = %d\n", pred);
	printf("Misprediction Percentage = %2.2f\n", (double)mispred/branches*100);
}
		



















