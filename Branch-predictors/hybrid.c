/***************************************************************************************************************
Filename: hybrid.c
Description: Combines two level gshare and two level local branch prediction schemes with branch history table 
size 262144 and variable counter size.
How to run this program:
To compile: gcc -o hybrid hybrid.c -lm
To run: ./hybrid <trace_file_name> <counter_size_in_bits>
For 2 bit counter, counter_size_in_bits will be 2.
****************************************************************************************************************/

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
	unsigned int o_bht[262144], l_bht[262144];
	unsigned int lhisreg[262144];
	unsigned int meta[262144];


	for(unsigned int i =0; i<262144; i++)
	{
		o_bht[i] = 0;
		l_bht[i] = 0;
	}
	for(unsigned int j =0; j<262144; j++)
	{
		lhisreg[j] = 0;
	}
	for(unsigned int j =0; j<262144; j++)
	{
		meta[j] = 0;
	}
	

	char line_of_trace[20];
   	FILE *pointer_to_line_in_trace;
	int operation_in_trace;
	unsigned int b_address;
	unsigned int lower;
	unsigned int mispred = 0;
	unsigned int branches = 0;
	unsigned int pred = 0;
	unsigned int o_p, l_p, f_p;
	unsigned int bit;
	unsigned int countersize = *argv[2] - 48;
	unsigned int half = (pow(2, countersize))/2;
	unsigned int o_pred_result;
	unsigned int l_pred_result;
	unsigned int ghisreg = 0;
	unsigned int xorres;

   	pointer_to_line_in_trace = fopen(tracefile , "r");
   	
	while((fgets(line_of_trace, 20, pointer_to_line_in_trace)!= NULL))
	{
		branches++;
		operation_in_trace = findoperation(line_of_trace);
		b_address = getaddress(line_of_trace);


		lower = b_address & 0x3FFFF;



		if(branches == 1)
		{
			bit = 0;
			xorres = lower^ghisreg;

			o_bht[xorres] = 0;

			if(operation_in_trace == 78)
			{
				ghisreg = (ghisreg | bit) & 0x3FFFF;
					o_pred_result = 1;
			}
			else
			{
				bit = 1;
				o_bht[xorres] = 1;
				ghisreg = (ghisreg | bit) & 0x3FFFF;
					o_pred_result = 0;
			}


		}

		else
		{

			xorres = lower^ghisreg;

			if(o_bht[xorres] < half)
			{
				o_p = 0;

				if(operation_in_trace == 78)
				{
					bit = 0;
					if(o_bht[xorres] > 0)
					o_bht[xorres] = o_bht[xorres] - 1;
					else
					o_bht[xorres] = o_p;					
					ghisreg = ((ghisreg << 1) | bit) & 0x3FFFF;
					o_pred_result = 1;
				}
				else
				{
					bit = 1;
					o_bht[xorres] = o_bht[xorres] + 1;
					ghisreg = ((ghisreg << 1) | bit) & 0x3FFFF;
					o_pred_result = 0;
				}


			}

			else
			{
				
				o_p = (pow(2, countersize)) - 1;;
				if(operation_in_trace == 84)
				{					
					bit = 1;
					if(o_bht[xorres] < o_p)
					o_bht[xorres] = o_bht[xorres] + 1;
					else
					o_bht[xorres] = o_p;
					ghisreg = ((ghisreg << 1) | bit) & 0x3FFFF;
					o_pred_result = 1;
				}
				else
				{
					bit = 0;
					o_bht[xorres] = o_bht[xorres] - 1;
					ghisreg = ((ghisreg << 1) | bit) & 0x3FFFF;
					o_pred_result = 0;
				}


			}

						
		}

		if(branches == 1)
		{
			bit = 0;

			lhisreg[lower] = 0;
			l_bht[lhisreg[lower]] = 0;

			if(operation_in_trace == 78)
			{
				lhisreg[lower] = (lhisreg[lower] | bit) & 0x3FFFF;
					l_pred_result = 1;
			}
			else
			{
				bit = 1;
				l_bht[lhisreg[lower]] = 1;
				lhisreg[lower] = (lhisreg[lower] | bit) & 0x3FFFF;
					l_pred_result = 0;
			}


		}
		else
		{


			if(l_bht[lhisreg[lower]] < half)
			{
				
				l_p = 0;

				if(operation_in_trace == 78)
				{
					bit = 0;
					if(l_bht[lhisreg[lower]] > 0)
					l_bht[lhisreg[lower]] = l_bht[lhisreg[lower]] - 1;
					else
					l_bht[lhisreg[lower]] = l_p;
					lhisreg[lower] = ((lhisreg[lower] << 1) | bit) & 0x3FFFF;
					l_pred_result = 1;

				}
				else
				{
					bit = 1;
					l_bht[lhisreg[lower]] = l_bht[lhisreg[lower]] + 1;
					lhisreg[lower] = ((lhisreg[lower] << 1) | bit) & 0x3FFFF;
					l_pred_result = 0;

				}


			}
			else
			{
				l_p = (pow(2, countersize)) - 1;
				

				if(operation_in_trace == 84)
				{
					bit = 1;
					if(l_bht[lhisreg[lower]] < l_p)
					l_bht[lhisreg[lower]] = l_bht[lhisreg[lower]] + 1;
					else
					l_bht[lhisreg[lower]] = l_p;
					lhisreg[lower] = ((lhisreg[lower] << 1) | bit) & 0x3FFFF;
					l_pred_result = 1;

				}
				else
				{
					bit = 0;
					l_bht[lhisreg[lower]] = l_bht[lhisreg[lower]] - 1;
					lhisreg[lower] = ((lhisreg[lower] << 1) | bit) & 0x3FFFF;
					l_pred_result = 0;

				}


			}
			
		}

		if(meta[lower] == 0)
		{
			f_p = o_p;
			if(o_pred_result == 0)
			mispred++;
			else
			pred++;
		}
		else
		{
			f_p = l_p;
			if(l_pred_result == 0)
			mispred++;
			else
			pred++;

		}

		if(o_pred_result == 0 && l_pred_result == 1)
		{	
			meta[lower] = 1;
		}
		else
		{
			meta[lower] = 0;
		}

	}

	printf("number of branches = %d\n", branches);
	printf("number of mispredictions = %d\n", mispred);
	printf("number of correct predictions = %d\n", pred);
	printf("Misprediction Percentage = %2.2f\n", (double)mispred/branches*100);
}


