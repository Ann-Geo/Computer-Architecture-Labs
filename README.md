# Computer-Architecture-Labs
Programming assignments for Computer Architecture course - Branch predictors, Cache simulator

Branch Predictors:\
Written programs that read in trace file and simulate different branch prediction schemes and different predictor sizes.\
Assumptions taken in this assignment are,
- 1K (1024) entry for Pattern History Table (PHT) with 2-bit Counter per entry (predictor index size of 2 bits)\
Following BPs are implemented,
- One level branch Prediction
- Two Level Global Branch Prediction
- Two Level Gshare Branch Predictor
- Two Level Local Branch Prediction (assume 128 entry for Local History Register Table)
- Also implemented a hybrid BP that combines the benefits of Gshare and Two Level Local BP.



Cache simulator:\
A cache simulator implementation in C, that is able to read the provided memory access trace file and simulate the behavior of cache. 
The simulation scenarios assumed in this assignment are,
- LRU as the replacement policy
- An instruction cache and a data cache with a combined total cache space of 32KB 
- Split cache - 16KB instruction cache and 16KB data cache
- The block size is varied - 8B, 32B and 128B
- The associativity is varied - direct mapped and 4-way
- Both split (separate instruction and data caches) and shared (all accesses go to a single cache that holds both instructions and data) caches are modeled
