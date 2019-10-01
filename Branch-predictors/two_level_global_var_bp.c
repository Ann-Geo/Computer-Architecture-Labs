/**********************************************************************************************************
Filename: two_level_global_var_bp
Description: Simulates two level global branch prediction scheme with branch history table size 1K and 
variable counter size.
How to run this program:
To compile: gcc -o two_level_global_var_bp two_level_global_var_bp.c -lm
To run: ./two_level_global_var_bp <trace_file_name> <counter_size_in_bits>
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


	unsigned int mispred = 0;
	unsigned int branches = 0;
	unsigned int pred = 0;
	unsigned int p;
	unsigned int countersize = *argv[2] - 48;
	unsigned int half = (pow(2, countersize))/2;


	unsigned int bit;

   	pointer_to_line_in_trace = fopen(tracefile , "r");
   	
	while((fgets(line_of_trace, 20, pointer_to_line_in_trace)!= NULL))
	{
		branches++;
		operation_in_trace = findoperation(line_of_trace);

		if(branches == 1)
		{
			bit = 0;
			bht[ghisreg] = 0;

			if(operation_in_trace == 78)
			{
				pred++;
				ghisreg = (ghisreg | bit) & 0x3FF;

			}
			else
			{
				bit = 1;
				bht[ghisreg] = 1;
				mispred++;
				ghisreg = (ghisreg | bit) & 0x3FF;
			}


		}

		else
		{
			if(bht[ghisreg] < half)
			{
				
				p = 0;

				if(operation_in_trace == 78)
				{
					pred++;
					bit = 0;
					if(bht[ghisreg] > 0)
					bht[ghisreg] = bht[ghisreg] - 1;
					else
					bht[ghisreg] = p;
					ghisreg = ((ghisreg << 1) | bit) & 0x3FF;

				}
				else
				{
					bit = 1;
					bht[ghisreg] = bht[ghisreg] + 1;
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
					if(bht[ghisreg] < p)
					bht[ghisreg] = bht[ghisreg] + 1;
					else
					bht[ghisreg] = p;
					ghisreg = ((ghisreg << 1) | bit) & 0x3FF;

				}
				else
				{
					bit = 0;
					bht[ghisreg] = bht[ghisreg] - 1;
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
		



















