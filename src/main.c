#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "splay-tree.h"

static inline uint64_t __attribute__((always_inline)) rdtsc (void)
{
	uint32_t lo, hi;
	asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
	return lo | ((uint64_t)(hi) << 32);
}

int main(int argc, char* argv[]){

    //sanity check, and generate workload input file
    if(argc < 5){
        printf("usage: zipf_gen.pl range_max num_keys num_lookups unif/zipf\n");
        return 1;
    }

    printf("The user output is %s, %s, %s\n",argv[1],argv[2],argv[3]);

    char buffer[100];
    int cx;
    if(!strcmp(argv[4],"unif")){
        printf("with uniform distribution\n");
        cx = snprintf(buffer,100,"./unif_gen.pl %s %s %s > workload.txt", argv[1],argv[2],argv[3]);
    }
    else{
        printf("with zip distribution\n");
        cx = snprintf(buffer,100,"./zipf_gen.pl %s %s %s > workload.txt", argv[1],argv[2],argv[3]);
    }

    if(cx >=0 && cx <100){
        system(buffer);
    }

    else{
        printf("input string too long, should be less than 100");
        return 1;
    }

    // import workload
    FILE *fptr;
    fptr =  fopen("workload.txt","r");

    //construct the splay tree
    splay_tree_t *sp = (splay_tree_t *) malloc(sizeof(splay_tree_t));

    int lookup[atoi(argv[3])];
    int counter = 0;

    if(fptr){
        printf("Reading from file\n");

        char line[100];
        while(fgets(line,sizeof(line),fptr)){
            char* token = strtok(line," ");

            if(!strncmp(token,"insert",sizeof("insert"))){
                token = strtok(NULL, " ");
                splay_tree_node_t *node = (splay_tree_node_t *) malloc(sizeof(splay_tree_node_t));
                node->key=atoi(token);
                splay_tree_insert(sp, node);
                // printf(" node %d inserted\n",atoi(token));
            }

            else
            {
                token = strtok(NULL, " ");
                lookup[counter++]= atoi(token);
            }
        }

        fclose(fptr);
    }

    else{
        printf("Error opening the file\n");
        return 1;
    }


    //start lookup nodes:

    uint64_t start_reading = rdtsc();

    for(int i =0 ;i<atoi(argv[3]);i++){

        // printf("lookup for node: %d\n",lookup[i]);
        int res = splay_tree_lookup(sp,lookup[i]);

        // if(res==-1){
        //     printf("Something went wrong, the lookup failed...\n");
        //     return -1;
        // }
        // printf("lookup succeeded\n");
    }

    uint64_t end_reading = rdtsc();

    printf("reading here are %llu\n",(end_reading - start_reading)/atoi(argv[3]));
    
    return 0;
}