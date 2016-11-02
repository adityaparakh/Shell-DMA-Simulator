#include <stdio.h>
#include <stdlib.h>



//defining structues that will be useful
//line symbolizes a line of cache, it will have a line a time stamp that will be used for LRU and status part to differentiate between mode
struct line_t{
	int tag;

	int tmstmp;
	int status;

};
typedef struct line_t line;

//set is just a pointer to lines
struct set_t{
	line * lineArr;
};
typedef struct set_t set;


//the actual cache struct
struct cache_t{
	int num_sets; //set = number of lines
	int assc;    //associativity == number of linees per sets
	int block_size; // number bytes per bloct 
	set * sets; //will be typedeffed later
};
typedef struct cache_t cache;
//headers
cache * make(int lineArr, int lps, int blocksize);
int search(long address, cache *cache);


int main (int argc, char *argv[]) {
    int total = 0;
    int hits = 0;
    int num_lines = atoi(argv[1]),
        lines_per_set = atoi(argv[2]),
        bytes_per_block = atoi(argv[3]);
    
    cache * cache = make(num_lines,lines_per_set,bytes_per_block);

    char line[80];

    long addr_req;
   
    printf("Simulating cache with:\n");
   printf(" - Total lines   = %d\n", num_lines);
    printf(" - Lines per set = %d\n", lines_per_set);
    printf(" - Block size    = %d bytes\n", bytes_per_block);

//    int n = 0;
    while (fgets(line, 80, stdin)) {
        addr_req = strtol(line, NULL, 0);
      //  n++;
  	     
       /* simulate cache fetch with address `addr_req` */
//        printf("Processing request: 0x%lX\n", addr_req);
	int hit = search(addr_req,cache);
	if(hit)
		++hits;
	++total;
    }
    printf("Hit rate: %f percent | Miss rate: %f percent \n",((double)hits/total)*100, ((total - (double)hits)/total)*100);	
    return 0;
}



//helper functions
//there will be two helper functions, one that makes a cache adn one that looks up the cache and registers hits and misses

//MAKE FUNCTION
cache * make (int lineArr, int lps,int blocksize){
	//allocate memory
	cache * c = malloc(sizeof(cache));
	//assign the variables
	c->num_sets = lineArr/lps; //lines over lines per set
	c->assc = lps;
	c->block_size= blocksize;
	//allocate memm for sets, sets = lines, so allocate memm enough for how many lines we have 
	c->sets = malloc(sizeof(set)*lineArr);
	//allocate and assign the line
	int i;
	for(i=0; i<(c->num_sets);++i){
		(c->sets+i)->lineArr = malloc(sizeof(line)*lps);
		int j;
		for(j=0; j<lps; ++j){
			((c->sets+i)->lineArr[j]).status = 0;		
		}	
	}
	return c;	
}

//search method will iterate through the cache and see if the thing that we want exists, if not will return a miss
int search(long address, cache *cache){
	//timestamp variable
	static unsigned int tmstmp = 0;
	//getting correct sum
	int set_idx = (address/cache->block_size)%cache->num_sets;
	//setting tag to the int division
	int tag = (address/cache->block_size)/cache->num_sets;

	int i,hit =0;
	// assing the set with with set i
	set *set= cache->sets + set_idx;
	//iterate through the set
	for (i=0; i<(cache->assc);i++){
		if (set->lineArr[i].status && set->lineArr[i].tag == tag){
			hit =1;
			break;
		}
	}
	
	//if there is a hit	
	if (hit) {
		set->lineArr[i].tmstmp = ++tmstmp;
		return 1;
	}

	else{
		int m = 0;
		int minimum = set->lineArr[0].tmstmp;
		
		for (i = 0; i<(cache->assc); i++){
			if (set->lineArr[i].tmstmp < minimum) {
				minimum = set->lineArr[i].tmstmp;
				m = i;
			}	
			if(!(set->lineArr[i].status)){
				//evict
				set->lineArr[i].tmstmp = ++tmstmp;
				set->lineArr[i].tag = tag;
				set->lineArr[i].status = 1;
				return 0;
			}		
		}
		//evict again	
		set->lineArr[m].tmstmp = ++tmstmp;
		set->lineArr[m].tag = tag;
		set->lineArr[m].status = 1;
		return  0;
	}
}





