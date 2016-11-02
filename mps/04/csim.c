#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include "cachelab.h"



struct line_t{
	int tag;

	int tmstmp;
	int status;

};
typedef struct line_t line;

struct set_t{
	line * lineArr;
};
typedef struct set_t set;

struct cache_t{
	int num_sets;
	int assc;
	int block_size;
	set * sets;
};

typedef struct cache_t cache;

cache * make(int lineArr, int lps, int blocksize);
void search(long address, cache *cache);
void store(long address, cache *cache);
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]);

static int hits = 0, misses = 0, evictions = 0;

long long bit_pow(int power) {
    long long result = 1;
    result = result << power;
    return result;
}

int main (int argc, char *argv[]) {
    
    int num_sets;
    int block_size;
    
    FILE *read_trace;
    
    char cmd; /* cmd will take in the operation address I, L, S, or M */
    long address;
    int size;
    
    int s, E, v, b;
    
    char *trace_file;
    char c;
    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c){
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                v = 1;
                break;
            case 'h':
                exit(0);
            default:
                exit(1);
        }
    }

    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        exit(1);
    }
    
    
    num_sets = pow(2.0, s);
    block_size = bit_pow(b);
    
    cache * cache = make(num_sets*E, E, block_size);
    
    read_trace = fopen(trace_file, "r");
        
   	if (read_trace != NULL) {
        
        while (fscanf(read_trace, " %c %lx,%d", &cmd, &address, &size) == 3) {
            switch(cmd) {
                case 'I':
                    break;
                case 'L':
                    search(address, cache);
                    break;
                case 'S':
                    store(address, cache);
                    break;
                case 'M':
                    search(address, cache);
                    store(address, cache);
                    break;
                default:
                    break;
            }
        }
    }
    
    printSummary(hits, misses, evictions);
    fclose(read_trace);
    return 0;
}


cache * make (int lineArr, int lps, int blocksize){
	cache * c = malloc(sizeof(cache));
	c->num_sets = lineArr/lps;
	c->assc = lps;
	c->block_size= blocksize;
	c->sets = malloc(sizeof(set)*lineArr);
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

void search(long address, cache *cache)  {
    
	static unsigned int tmstmp = 0;
	int set_idx = (address/cache->block_size)%cache->num_sets;
	int tag = (address/cache->block_size)/cache->num_sets;

	int i, hit = 0;
	set *set= cache->sets + set_idx;
	for (i=0; i<=(cache->assc);i++){
		if (set->lineArr[i].status && set->lineArr[i].tag == tag){
			hit =1;
			break;
		}
	}
	
	if (hit) {
		set->lineArr[i].tmstmp = ++tmstmp;
        hits++;
		return;
	}
	else {
        
        misses++;
		int m = 0;
		int minimum = set->lineArr[0].tmstmp;
		
		for (i = 0; i<(cache->assc); i++){
			if (set->lineArr[i].tmstmp < minimum) {
				minimum = set->lineArr[i].tmstmp;
				m = i;
			}	
			if(!(set->lineArr[i].status)){
				set->lineArr[i].tmstmp = ++tmstmp;
				set->lineArr[i].tag = tag;
				set->lineArr[i].status = 1;
				return;
			}		
		}
		set->lineArr[m].tmstmp = ++tmstmp;
		set->lineArr[m].tag = tag;
		set->lineArr[m].status = 1;
        evictions++;
		return;
	}
    
    exit(0);
}

void store(long address, cache *cache)  {
    
    static unsigned int tmstmp = 0;
    int set_idx = (address/cache->block_size)%cache->num_sets;
    int tag = (address/cache->block_size)/cache->num_sets;
    
    int i, hit = 0;
    set *set= cache->sets + set_idx;
    for (i=0; i<=(cache->assc); i++){
        if (set->lineArr[i].status && set->lineArr[i].tag == tag){
            hit = 1;
            break;
        }
    }
    
    if (hit) {
        set->lineArr[i].tmstmp = ++tmstmp;
        hits++;
        return;
    }
    else {
        
        misses++;
        int m = 0;
        int minimum = set->lineArr[0].tmstmp;
        
        for (i = 0; i<(cache->assc); i++){
            if (set->lineArr[i].tmstmp < minimum) {
                minimum = set->lineArr[i].tmstmp;
                m = i;
            }
            if(!(set->lineArr[i].status)){
                set->lineArr[i].tmstmp = ++tmstmp;
                set->lineArr[i].tag = tag;
                set->lineArr[i].status = 1;
                return;
            }		
        }
        set->lineArr[m].tmstmp = ++tmstmp;
        set->lineArr[m].tag = tag;
        set->lineArr[m].status = 1;
        evictions++;
        return;
    }
    
    exit(0);
}





