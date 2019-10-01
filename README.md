# Computer-Architecture-Labs
Programming assignments for Computer Architecture course - Branch predictors, Cache simulator


Cache simulator:
A cache simulator implementation in C, that is able to read the provided memory access trace file and simulate the behavior of cache. 
The simulation scenarios assumed in this program are,
- LRU as the replacement policy
- An instruction cache and a data cache with a combined total cache space of 32KB 
- Split cache - 16KB instruction cache and 16KB data cache
- The block size is varied - 8B, 32B and 128B
- The associativity should is varied - direct mapped and 4-way
- Both split (separate instruction and data caches) and shared (all accesses go to a single cache that holds both instructions and data) caches are modeled
