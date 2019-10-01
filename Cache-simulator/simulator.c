/***************************************************************************************************************** 
File name: simulator.c
Descrption: This program simulates the behaviour of a cache. The functionalities included are listed nelow.
Functionalities:
1. Supports is_it_direct mapped and 4 way set associative
2. Supports block sizes 8B, 32B and 128B
3. Supports LRU policy
4. Supports shared and split cache
How to run this program: First of all chosse between shared/split and is_it_direct/4-way. For example if a split 4-way
cache behaviour to be simulated then edit the #defined parameters under the Four way split cache parameters section.
The parameters to be entered are sets in instruction four way cache, sets in data four way cache, number of index
bits required to address these sets, mask required to extract theses index bits, block size of the cache, number of 
offset bits, number of tag shift bits required to extract tag bits from address.
To run this program: To run this program after compiling it give the following command.
./simulator <cache_type> <tracefile_name> <cache_organization>
cache type can be: fourway_cache or dir_cache
cache organization can be: 0 for shared cahe or 1 for split cache
trace file: file format should be plain text
For example a split 4-way cache command will look like: ./simulator fourway_cache trace 1

******************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

//is_it_direct mapped shared cache parameters
#define BLOCKS_IN_CACHE 256//Enter the blocks in is_it_direct mapped cache(total cache size/block size)
#define DIR_INDEX 8//Enter the index bits required to address these blocks(2^(index bits) = no of blocks in cache)
#define DIR_MASK 0xff//Enter the mask to find Index bits(mask of 1's of length of index bits)
#define DIR_BLOCK_SIZE 128//Enter block size(in bytes)
#define DIR_OFFSET 7//Enter offset bits(2^(offset) = block size)
#define DIR_TAG_SHIFT 15//Enter tag shift required to find tag(DIR_INDEX+DIR_OFFSET)

//Four way shared cache parameters
#define SETS_IN_FOUR_WAY 64//Enter sets in a 4 way cache(total cache size/block size = no of blocks, sets = no of blocks/4)
#define SET_INDEX 6//Enter the index bits required to address these sets(2^(index bits) = sets in cache)
#define SET_MASK 0x3f//Enter the mask to find Index bits(mask of 1's of length of index bits)
#define SET_BLOCK_SIZE 128//Enter block size(in bytes)
#define SET_OFFSET 7//Enter offset bits(2^(offset) = block size)
#define SET_TAG_SHIFT 13//Enter tag shift required to find tag(SET_INDEX+SET_OFFSET)

//is_it_direct mapped split cache parametrs
#define BLOCKS_IN_INST_CACHE 128//Enter the blocks in is_it_direct mapped instruction cache(total inst cache size/block size = blocks in inst cache)
#define BLOCKS_IN_DATA_CACHE 128//Enter the blocks in is_it_direct mapped data cache(total data cache size/block size = blocks in data cache)
#define SPLIT_DIR_INDEX 7//Enter the index bits required to address these blocks(2^(index bits) = no of blocks in cache)
#define SPLIT_DIR_MASK 0x7f//Enter the mask to find Index bits(mask of 1's of length of index bits)
#define SPLIT_DIR_BLOCK_SIZE 128//Enter block size(in bytes)
#define SPLIT_DIR_OFFSET 7//Enter offset bits(2^(offset) = block size)
#define SPLIT_DIR_TAG_SHIFT 14//Enter tag shift required to find tag(SPLIT_DIR_INDEX+SPLIT_DIR_OFFSET)

//Four way split cache parameters
#define SETS_IN_INST_FOUR_WAY 32//Enter sets in a 4 way split instruction cache(total inst cache size/block size = no of blocks in inst cache, sets = no of blocks/4)
#define SETS_IN_DATA_FOUR_WAY 32//Enter sets in a 4 way split instruction cache(total data cache size/block size = no of blocks in data cache, sets = no of blocks/4)
#define SPLIT_SET_INDEX 5//Enter the index bits required to address these blocks(2^(index bits) = sets in cache)
#define SPLIT_SET_MASK 0x1f//Enter the mask to find Index bits(mask of 1's of length of index bits)
#define SPLIT_SET_BLOCK_SIZE 128//Enter block size(in bytes)
#define SPLIT_SET_OFFSET 7//Enter offset bits(2^(offset) = block size)
#define SPLIT_SET_TAG_SHIFT 12////Enter tag shift required to find tag(SPLIT_SET_INDEX+SPLIT_SET_OFFSET)


//direct mapped cache structure
struct dir_cache{ 
	uint64_t blocks[BLOCKS_IN_CACHE]; 
	int no_of_hits;
	int no_of_misses;
};

//direct mapped instruction cache structure
struct dir_inst_cache{ 
	uint64_t blocks[BLOCKS_IN_INST_CACHE];
	int no_of_hits;
	int no_of_misses;
};

//direct mapped data cache structure
struct dir_data_cache{ 
	uint64_t blocks[BLOCKS_IN_DATA_CACHE];
	int no_of_hits;
	int no_of_misses;
};

//4-way associative cache structure
struct fourway_cache{
	uint64_t sets[SETS_IN_FOUR_WAY][4];
	uint64_t no_of_access[SETS_IN_FOUR_WAY][4];
	int no_of_hits;
	int no_of_misses;
};

//4-way associative instruction cache structure
struct fourway_inst_cache{
	uint64_t sets[SETS_IN_INST_FOUR_WAY][4];
	uint64_t no_of_access[SETS_IN_INST_FOUR_WAY][4];
	int no_of_hits;
	int no_of_misses;
};

//4-way associative data cache structure
struct fourway_data_cache{
	uint64_t sets[SETS_IN_DATA_FOUR_WAY][4];
	uint64_t no_of_access[SETS_IN_DATA_FOUR_WAY][4];
	int no_of_hits;
	int no_of_misses;
};


uint64_t hexaddresstobits(char hex_address[]);
int findoperation(char hex_address[]);
void read_from_dir_cache(struct dir_cache *cache, uint64_t address_in_trace);
void read_from_four_way_cache(struct fourway_cache *cache, uint64_t address_in_trace);
void write_to_shared_on_a_miss(struct fourway_cache *cache, uint64_t set, uint64_t tag);
void write_to_dir_cache(struct dir_cache *cache, uint64_t address_in_trace);
void write_to_four_way_cache(struct fourway_cache *cache, uint64_t address_in_trace);

void read_from_dir_inst_cache(struct dir_inst_cache *cache, uint64_t address_in_trace);
void read_from_four_way_inst_cache(struct fourway_inst_cache *cache, uint64_t address_in_trace);
void read_from_dir_data_cache(struct dir_data_cache *cache, uint64_t address_in_trace);
void read_from_four_way_data_cache(struct fourway_data_cache *cache, uint64_t address_in_trace);
void write_to_dir_data_cache(struct dir_data_cache *cache, uint64_t address_in_trace);
void write_to_four_way_data_cache(struct fourway_data_cache *cache, uint64_t address_in_trace);
void write_to_split_data_cache_on_a_miss(struct fourway_data_cache *cache, uint64_t set, uint64_t tag);
void write_to_split_inst_cache_on_a_miss(struct fourway_inst_cache *cache, uint64_t set, uint64_t tag);


//no of cache reads
int reads = 0;

//no of cache writes
int writes = 0;

/*****************************************************************************************************************
Function: main()
Description: 
1. Initializes cache blocks
2. Reads the trace file
3. After determining the operation it reads/writes the cache with respect to operation
*******************************************************************************************************************/

int main(int argc, char *argv[]) 
{	

printf("Reminder to read readme file and change the cache parameters before compiling\n");

if(*argv[3] == '0')
{	
	char* tracefile = argv[2];
	char is_it_fourway[20];
	char is_it_direct[10];
	strcpy(is_it_fourway, "fourway_cache");
	strcpy(is_it_direct, "dir_cache");
	struct fourway_cache assoccache;
	struct dir_cache dirmapcache;
	assoccache.no_of_hits = 0;
	assoccache.no_of_misses = 0;
	dirmapcache.no_of_hits = 0;
	dirmapcache.no_of_misses = 0;

	for(int i =0; i<BLOCKS_IN_CACHE; i++){
		dirmapcache.blocks[i] = 0;
	}
	for(int x =0; x<SETS_IN_FOUR_WAY; x++){
		for(int y = 0; y<4; y++){
			assoccache.sets[x][y] = 0;
			assoccache.no_of_access[x][y] = 0;
		}
	}

	char line_of_trace[20];
   	FILE *pointer_to_line_in_trace;
	int operation_in_trace;

   	pointer_to_line_in_trace = fopen(tracefile , "r");
   	
	while((fgets(line_of_trace, 20, pointer_to_line_in_trace)!= NULL))
	{

		operation_in_trace = findoperation(line_of_trace);

		if(operation_in_trace == 49)
		{

			uint64_t address = hexaddresstobits(line_of_trace);
			if(strncmp(argv[1], is_it_fourway, 24)==0){
				write_to_four_way_cache(&assoccache, address);
			}
			if(strncmp(argv[1], is_it_direct, 19)==0){
				write_to_dir_cache(&dirmapcache, address);
			}
		}
		else
		{

			uint64_t address = hexaddresstobits(line_of_trace);
			if(strncmp(argv[1], is_it_fourway, 24)==0){
				read_from_four_way_cache(&assoccache, address);
			}
			if(strncmp(argv[1], is_it_direct, 19)==0){
				read_from_dir_cache(&dirmapcache, address);
			}
		}
	}

	fclose(pointer_to_line_in_trace);


	if(strncmp(argv[1], is_it_fourway, 24)==0){

		printf("You selected a shared cache with 4-way associativity\n");

		printf("No of cache Hits: %d\n", assoccache.no_of_hits);
		printf("No of cache Misses: %d\n", assoccache.no_of_misses);

		printf("Cache hit rate: %2.4f\n", (double)assoccache.no_of_hits/(assoccache.no_of_hits+assoccache.no_of_misses));
		printf("Cache miss rate: %2.4f\n", (double)assoccache.no_of_misses/(assoccache.no_of_hits+assoccache.no_of_misses));

		printf("cache reads : %d\n", reads);
		printf("cache writes : %d\n", writes);
	}
	if(strncmp(argv[1], is_it_direct, 19)==0){

		printf("You selected a shared cache with direct mapping\n");

		printf("No of cache Hits: %d\n", dirmapcache.no_of_hits);
		printf("No of cache Misses: %d\n", dirmapcache.no_of_misses);

		printf("Cache hit rate: %2.4f\n", (double)dirmapcache.no_of_hits/(dirmapcache.no_of_hits+dirmapcache.no_of_misses));
		printf("Cache miss rate: %2.4f\n", (double)dirmapcache.no_of_misses/(dirmapcache.no_of_hits+dirmapcache.no_of_misses));

		printf("cache reads : %d\n", reads);
		printf("cache writes : %d\n", writes);
	}


}

else
{
	

	char* tracefile = argv[2];
	char is_it_fourway[20];
	char is_it_direct[10];
	strcpy(is_it_fourway, "fourway_cache");
	strcpy(is_it_direct, "dir_cache");
	struct fourway_inst_cache instassoccache;
	struct fourway_data_cache dataassoccache;
	struct dir_inst_cache instdirmapcache;
	struct dir_data_cache datadirmapcache;
	instassoccache.no_of_hits = 0;
	dataassoccache.no_of_misses = 0;
	instdirmapcache.no_of_hits = 0;
	datadirmapcache.no_of_misses = 0;

	for(int i =0; i<BLOCKS_IN_DATA_CACHE; i++){
		instdirmapcache.blocks[i] = 0;
		datadirmapcache.blocks[i] = 0;
	}
	for(int x =0; x<SETS_IN_DATA_FOUR_WAY; x++){
		for(int y = 0; y<8; y++){
			instassoccache.sets[x][y] = 0;
			instassoccache.no_of_access[x][y] = 0;
			dataassoccache.sets[x][y] = 0;
			dataassoccache.no_of_access[x][y] = 0;
		}
	}

	char line_of_trace[20];
	
   	FILE *pointer_to_line_in_trace;

	int operation_in_trace;


   	pointer_to_line_in_trace = fopen(tracefile , "r");
   	
	while((fgets(line_of_trace, 20, pointer_to_line_in_trace)!= NULL))
	{

		operation_in_trace = findoperation(line_of_trace);

		if(operation_in_trace == 49)
		{

			uint64_t address = hexaddresstobits(line_of_trace);
			if(strncmp(argv[1], is_it_fourway, 24)==0){
				write_to_four_way_data_cache(&dataassoccache, address);
			}
			if(strncmp(argv[1], is_it_direct, 19)==0){
				write_to_dir_data_cache(&datadirmapcache, address);
			}
		}	
		else if(operation_in_trace == 50)
		{

			uint64_t address = hexaddresstobits(line_of_trace);
			if(strncmp(argv[1], is_it_fourway, 24)==0){
				read_from_four_way_inst_cache(&instassoccache, address);
			}
			if(strncmp(argv[1], is_it_direct, 19)==0){
				read_from_dir_inst_cache(&instdirmapcache, address);
			}
		}
		else
		{

			uint64_t address = hexaddresstobits(line_of_trace);
			if(strncmp(argv[1], is_it_fourway, 24)==0){
				read_from_four_way_data_cache(&dataassoccache, address);
			}
			if(strncmp(argv[1], is_it_direct, 19)==0){
				read_from_dir_data_cache(&datadirmapcache, address);
			}
		}
	}

	fclose(pointer_to_line_in_trace);


	if(strncmp(argv[1], is_it_fourway, 24)==0){

		printf("You selected a split cache with 4-way associativity\n");

		printf("No of Inst Cache Hits: %d\n", instassoccache.no_of_hits);
		printf("No of Inst Cache Misses: %d\n", instassoccache.no_of_misses);

		printf("Inst Cache Hit rate: %2.4f\n", (double)instassoccache.no_of_hits/(instassoccache.no_of_hits+instassoccache.no_of_misses));
		printf("Inst Cache Miss rate: %2.4f\n", (double)instassoccache.no_of_misses/(instassoccache.no_of_hits+instassoccache.no_of_misses));

		printf("No of Data Cache Hits: %d\n", dataassoccache.no_of_hits);
		printf("No of Data Cache Misses: %d\n", dataassoccache.no_of_misses);

		printf("Data Cache Hit rate: %2.4f\n", (double)dataassoccache.no_of_hits/(dataassoccache.no_of_hits+dataassoccache.no_of_misses));
		printf("Data Cache Miss rate: %2.4f\n", (double)dataassoccache.no_of_misses/(dataassoccache.no_of_hits+dataassoccache.no_of_misses));

		printf("cache reads : %d\n", reads);
		printf("cache writes : %d\n", writes);
	}
	if(strncmp(argv[1], is_it_direct, 19)==0){
		
		printf("You selected a split cache with direct mapping\n");
		
		printf("No of Inst Cache Hits: %d\n", instdirmapcache.no_of_hits);
		printf("No of Inst Cache Misses: %d\n", instdirmapcache.no_of_misses);

		printf("Inst Cache Hit rate: %2.4f\n", (double)instdirmapcache.no_of_hits/(instdirmapcache.no_of_hits+instdirmapcache.no_of_misses));
		printf("Inst Cache Miss rate: %2.4f\n", (double)instdirmapcache.no_of_misses/(instdirmapcache.no_of_hits+instdirmapcache.no_of_misses));

		printf("No of Data Cache Hits: %d\n", datadirmapcache.no_of_hits);
		printf("No of Data Cache Misses: %d\n", datadirmapcache.no_of_misses);

		printf("Data Cache Hit rate: %2.4f\n", (double)datadirmapcache.no_of_hits/(datadirmapcache.no_of_hits+datadirmapcache.no_of_misses));
		printf("Data Cache Miss rate: %2.4f\n", (double)datadirmapcache.no_of_misses/(datadirmapcache.no_of_hits+datadirmapcache.no_of_misses));

		printf("cache reads : %d\n", reads);
		printf("cache writes : %d\n", writes);
	}



}

}

/************************************************************************************************************************************
Function: hexaddresstobits(char)
Input: Hex address
Output: binary representation
Description: Coverts hexadecimal address to its binary representation
*************************************************************************************************************************************/

uint64_t hexaddresstobits(char hex_address[]) 
{
	uint64_t bit_level_rep = 0;
	int i = 2;
	while(hex_address[i]!= '\n'){
   		if (hex_address[i]<= '9' && hex_address[i]>='0'){
   			bit_level_rep = (bit_level_rep*16) + (hex_address[i] - '0');
   		}else{
   			if(hex_address[i] == 'a'){
   		 		bit_level_rep = (bit_level_rep*16) + 10;
   		 	}
   		 	if(hex_address[i] == 'b'){
   		 		bit_level_rep = (bit_level_rep*16) + 11;
   		 	}
   		 	if(hex_address[i] == 'c'){
   		 		bit_level_rep = (bit_level_rep*16) + 12;
   		 	}
   		 	if(hex_address[i] == 'd'){
   		 		bit_level_rep = (bit_level_rep*16) + 13;
   		 	}
   		 	if(hex_address[i] == 'e'){
   		 		bit_level_rep = (bit_level_rep*16) + 14;
   		 	}
   		 	if(hex_address[i] == 'f'){
   		 		bit_level_rep = (bit_level_rep*16) + 15;
   		 	}
   		}
   	    i++;
   	}

    return bit_level_rep;
}

/************************************************************************************************************************************
Function: read_from_dir_cache(struct*, uint64_t)
Input: Structure pointer to cache and address read in trace file
Description: Performs cache read operation from direct mapped shared cache. If the tag is present in cache block, it increments hits 
else it increments cache misses. And updates the current tag to the cache block
*************************************************************************************************************************************/

void read_from_dir_cache(struct dir_cache *cache, uint64_t address_in_trace)
{
	uint64_t mask = DIR_MASK;
	uint64_t cache_block = (address_in_trace >> DIR_OFFSET) & mask;
	uint64_t tag = address_in_trace >> DIR_TAG_SHIFT;

	if (cache->blocks[cache_block] == tag){
		cache->no_of_hits +=1;
		reads++;
	}
	else{
		cache->no_of_misses += 1;
		cache->blocks[cache_block] = tag;
		writes++;
	}
}

/*****************************************************************************************************************************************
Function: read_from_four_way_cache(struct*, uint64_t)
Input: Structure pointer to cache and address read in trace file
Description: Performs cache read operation from four way set associative shared cache. If the tag is present in cache block, it increments 
hits else it increments cache misses. And updates the current tag to the cache block
******************************************************************************************************************************************/

void read_from_four_way_cache(struct fourway_cache *cache, uint64_t address_in_trace)
{
	uint64_t mask = SET_MASK;
	uint64_t identify_set = (address_in_trace >> SET_OFFSET) & mask;
	uint64_t tag = address_in_trace >> SET_TAG_SHIFT;

	for(int i = 0; i <4; i++){
		if(cache->sets[identify_set][i] == tag){
			cache->no_of_hits += 1;
			cache->no_of_access[identify_set][i] += 1;
			reads++;
			return;
		}	
	}
	writes++;
	cache->no_of_misses += 1;
	write_to_shared_on_a_miss(cache, identify_set, tag);

}

/************************************************************************************************************************************
Function: write_to_shared_on_a_miss(struct*, uint64_t, uint64_t)
Input: Structure pointer to cache, set to which write to be done and tag to be updated to the block
Description: Performs cache write operation to four way set associative shared cache on a miss. Finds the least recently used cache 
block from the set and updates the tag to that cache block.
*************************************************************************************************************************************/

void write_to_shared_on_a_miss(struct fourway_cache *cache, uint64_t set, uint64_t tag)
{
	int least_recently_used = cache->no_of_access[set][0];
	for(int x = 0; x<4; x++){
		if(cache->no_of_access[set][x] < least_recently_used){
			least_recently_used = cache->no_of_access[set][x];
		}
	}
	
	for(int x = 0; x<4; x++){
		if(cache->no_of_access[set][x] == least_recently_used){
			cache->sets[set][x] = tag;
			cache->no_of_access[set][x] = 0;
			return;
		}
	}
}

/************************************************************************************************************************************
Function: write_to_split_data_cache_on_a_miss(struct*, uint64_t, uint64_t)
Input: Structure pointer to data cache, set to which write to be done and tag to be updated to the block
Description: Performs cache write operation to four way set associative data cache on a miss. Finds the least recently used cache block
from the set and updates the tag to that cache block.
*************************************************************************************************************************************/

void write_to_split_data_cache_on_a_miss(struct fourway_data_cache *cache, uint64_t set, uint64_t tag)
{
	int least_recently_used = cache->no_of_access[set][0];
	for(int x = 0; x<4; x++){
		if(cache->no_of_access[set][x] < least_recently_used){
			least_recently_used = cache->no_of_access[set][x];
		}
	}
	
	for(int x = 0; x<4; x++){
		if(cache->no_of_access[set][x] == least_recently_used){
			cache->sets[set][x] = tag;
			cache->no_of_access[set][x] = 0;
			return;
		}
	}
}

/************************************************************************************************************************************
Function: write_to_split_inst_cache_on_a_miss(struct*, uint64_t, uint64_t)
Input: Structure pointer to inst cache, set to which write to be done and tag to be updated to the block
Description: Performs cache write operation to four way set associative data cache on a miss. Finds the least recently used cache 
block from the set and updates the tag to that cache block.
*************************************************************************************************************************************/

void write_to_split_inst_cache_on_a_miss(struct fourway_inst_cache *cache, uint64_t set, uint64_t tag)
{
	int least_recently_used = cache->no_of_access[set][0];
	for(int x = 0; x<4; x++){
		if(cache->no_of_access[set][x] < least_recently_used){
			least_recently_used = cache->no_of_access[set][x];
		}
	}
	
	for(int x = 0; x<4; x++){
		if(cache->no_of_access[set][x] == least_recently_used){
			cache->sets[set][x] = tag;
			cache->no_of_access[set][x] = 0;
			return;
		}
	}
}

/************************************************************************************************************************************
Function: write_to_dir_cache(struct*, uint64_t)
Input: Structure pointer to cache and address read in trace file
Description: Performs cache write operation to direct mapped shared cache. Attach the tag to the cache block, finds the cache block by
extracting index bits from the binary of adress
*************************************************************************************************************************************/

void write_to_dir_cache(struct dir_cache *cache, uint64_t address_in_trace)
{
	uint64_t mask = DIR_MASK;
	uint64_t cache_block = (address_in_trace >> DIR_OFFSET) & mask;
	uint64_t tag = address_in_trace >> DIR_TAG_SHIFT;

		cache->blocks[cache_block] = tag;
		writes++;
}

/************************************************************************************************************************************
Function: write_to_four_way_cache(struct*, uint64_t)
Input: Structure pointer to cache and address read in trace file
Description: Performs cache write operation to four way set associative shared cache. Calls the function write_to_shared_on_a_miss to
find the least recently used cache block and updates it with tag
*************************************************************************************************************************************/

void write_to_four_way_cache(struct fourway_cache *cache, uint64_t address_in_trace)
{
	

	uint64_t mask = SET_MASK;
	uint64_t identify_set = (address_in_trace >> SET_OFFSET) & mask;
	uint64_t tag = address_in_trace >> SET_TAG_SHIFT;
	writes++;
	write_to_shared_on_a_miss(cache, identify_set, tag);

}

/************************************************************************************************************************************
Function: findoperation(char hex_address[]) 
Input: Pointer to the hex address array
Description: Finds the opration to be performed on an address. Possible operations in the trace file are data read, data write and 
instruction fetch
*************************************************************************************************************************************/

int findoperation(char hex_address[]) 
{
	int operation_in_trace = hex_address[0];
	return operation_in_trace;
}

/************************************************************************************************************************************
Function: write_to_dir_data_cache(struct*, uint64_t)
Input: Structure pointer to cache and address read in trace file
Description: Performs cache write operation to direct mapped data cache. Attach the tag to the cache block, finds the cache block by
extracting index bits from the binary of adress
*************************************************************************************************************************************/

void write_to_dir_data_cache(struct dir_data_cache *cache, uint64_t address_in_trace)
{
	uint64_t mask = SPLIT_DIR_MASK;
	uint64_t cache_block = (address_in_trace >> SPLIT_DIR_OFFSET) & mask;
	uint64_t tag = address_in_trace >> SPLIT_DIR_TAG_SHIFT;
	cache->blocks[cache_block] = tag;
	writes++;
}

/************************************************************************************************************************************
Function: write_to_four_way_data_cache(struct*, uint64_t)
Input: Structure pointer to cache and address read in trace file
Description: Performs cache write operation to four way set associative data cache. Calls the function write_to_split_data_on_a_miss to
find the least recently used cache block and updates it with tag
*************************************************************************************************************************************/

void write_to_four_way_data_cache(struct fourway_data_cache *cache, uint64_t address_in_trace)
{
	

	uint64_t mask = SPLIT_SET_MASK;
	uint64_t identify_set = (address_in_trace >> SPLIT_SET_OFFSET) & mask;
	uint64_t tag = address_in_trace >> SPLIT_SET_TAG_SHIFT;
	writes++;
	write_to_split_data_cache_on_a_miss(cache, identify_set, tag);

}

/************************************************************************************************************************************
Function: read_from_dir_inst_cache(struct*, uint64_t)
Input: Structure pointer to cache and address read in trace file
Description: Performs cache read operation from direct mapped inst cache. If the tag is present in cache block, it increments hits 
else it increments cache misses. And updates the current tag to the cache block
*************************************************************************************************************************************/

void read_from_dir_inst_cache(struct dir_inst_cache *cache, uint64_t address_in_trace)
{
	uint64_t mask = SPLIT_DIR_MASK;
	uint64_t cache_block = (address_in_trace >> SPLIT_DIR_OFFSET) & mask;
	uint64_t tag = address_in_trace >> SPLIT_DIR_TAG_SHIFT;

	if (cache->blocks[cache_block] == tag){
		cache->no_of_hits +=1;
		reads++;
	}
	else{
		cache->no_of_misses += 1;
		cache->blocks[cache_block] = tag;
		writes++;
	}
}

/*****************************************************************************************************************************************
Function: read_from_four_way_inst_cache(struct*, uint64_t)
Input: Structure pointer to cache and address read in trace file
Description: Performs cache read operation from four way set associative inst cache. If the tag is present in cache block, it increments 
hits else it increments cache misses. And updates the current tag to the cache block
******************************************************************************************************************************************/

void read_from_four_way_inst_cache(struct fourway_inst_cache *cache, uint64_t address_in_trace)
{
	uint64_t mask = SPLIT_SET_MASK;
	uint64_t identify_set = (address_in_trace >> SPLIT_SET_OFFSET) & mask;
	uint64_t tag = address_in_trace >> SPLIT_SET_TAG_SHIFT;

	for(int i = 0; i <8; i++){
		if(cache->sets[identify_set][i] == tag){
			cache->no_of_hits += 1;
			cache->no_of_access[identify_set][i] += 1;
			reads++;
			return;
		}	
	}
	writes++;
	cache->no_of_misses += 1;
	write_to_split_inst_cache_on_a_miss(cache, identify_set, tag);

}

/************************************************************************************************************************************
Function: read_from_dir_data_cache(struct*, uint64_t)
Input: Structure pointer to cache and address read in trace file
Description: Performs cache read operation from direct mapped data cache. If the tag is present in cache block, it increments hits 
else it increments cache misses. And updates the current tag to the cache block
*************************************************************************************************************************************/

void read_from_dir_data_cache(struct dir_data_cache *cache, uint64_t address_in_trace)
{
	uint64_t mask = SPLIT_DIR_MASK;
	uint64_t cache_block = (address_in_trace >> SPLIT_DIR_OFFSET) & mask;
	uint64_t tag = address_in_trace >> SPLIT_DIR_TAG_SHIFT;

	if (cache->blocks[cache_block] == tag){
		cache->no_of_hits +=1;
		reads++;
	}
	else{
		cache->no_of_misses += 1;
		cache->blocks[cache_block] = tag;
		writes++;
	}
}

/*****************************************************************************************************************************************
Function: read_from_four_way_data_cache(struct*, uint64_t)
Input: Structure pointer to cache and address read in trace file
Description: Performs cache read operation from four way set associative data cache. If the tag is present in cache block, it increments 
hits else it increments cache misses. And updates the current tag to the cache block
******************************************************************************************************************************************/

void read_from_four_way_data_cache(struct fourway_data_cache *cache, uint64_t address_in_trace)
{
	uint64_t mask = SPLIT_SET_MASK;
	uint64_t identify_set = (address_in_trace >> SPLIT_SET_OFFSET) & mask;
	uint64_t tag = address_in_trace >> SPLIT_SET_TAG_SHIFT;

	for(int i = 0; i <8; i++){
		if(cache->sets[identify_set][i] == tag){
			cache->no_of_hits += 1;
			cache->no_of_access[identify_set][i] += 1;
			reads++;
			return;
		}	
	}
	writes++;
	cache->no_of_misses += 1;
	write_to_split_data_cache_on_a_miss(cache, identify_set, tag);

}
