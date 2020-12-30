#include "btree.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include<bits/stdc++.h>
typedef struct thread_queue{
    entry_key_t key;
    uint8_t operation;                    // 0: read, 1: insert;
} thread_queue;

typedef struct sub_thread{
    pthread_t thread;
    uint32_t id;
    uint64_t inserted;
    uint64_t update;
    uint64_t read;
    uint64_t op_num;
    uint32_t thread_num;
    uint8_t flag;
    btree* root;
    thread_queue* run_queue;
} sub_thread;

void* ycsb_thread_run(void* arg){
    sub_thread* subthread = (sub_thread*)arg;
    char* new_value = (char*)malloc(sizeof(char)*8);
    memset(new_value, 'x', 8);
    int i = 0;
    for(; i < subthread->op_num/subthread->thread_num; i++){
        if( subthread->run_queue[i].operation == 1){
            // insert
            subthread->root->btree_insert(subthread->run_queue[i].key, (char*)new_value);
            subthread->inserted ++;
        }else if(subthread->run_queue[i].operation == 0){
            // look up
            char* t=subthread->root->btree_search(subthread->run_queue[i].key);
            if(t!=NULL)
            {
                subthread->read ++; 
            }                
        }else{
            // TODO update  
            // subthread->root->btree_update(subthread->run_queue[i].key,value_addr);
            // subthread->update ++;
        }
    }
    pthread_exit(NULL);
}

void clear_cache() {
  // Remove cache
  int size = 24*1024*1024;
  char *garbage = new char[size];
  for(int i=0;i<size;++i)
    garbage[i] = i;
  for(int i=100;i<size;++i)
    garbage[i] += garbage[i-100];
  delete[] garbage;
}

int main(int argc, char* argv[])
{    
    // init PM memory pool
	if ((vmp = vmem_create("/pmem6/btree_pool",
					VMEM_MIN_POOL1)) == NULL) {
		perror("vmem_create");
		exit(1); 
	}

    int thread_num = atoi(argv[1]);             // INPUT: the number of threads
    int all_num = atoi(argv[2]);
    btree *bt;
    bt = new btree();
    uint64_t inserted = 0, queried = 0, t = 0;
    uint8_t key[8];

	FILE *ycsb, *ycsb_read;
	char *buf = NULL;
	size_t len = 0;
    struct timespec start, finish;
    double single_time;

    if((ycsb = fopen("/home/YCSB_DATA2/099/c_6M_16_load","r")) == NULL)
    {
        perror("fail to read");
    }

    vector<entry_key_t> keys;
    int i;
    for(i=0;i<all_num;i++)
    {
        getline(&buf,&len,ycsb);
        if(strncmp(buf, "insert", 6) == 0){
            memcpy(key, buf+19, 8);
            keys.push_back(atol((char*)key));
        }
    }
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(keys.begin(), keys.end(), rng);

    char* value = (char*)malloc(sizeof(char)*8);
    memset(value, 'x', 8);

    for(i=0;i<all_num;i++)
    {
        bt->btree_insert(keys[i], value);
        inserted++;
    }
    
	fclose(ycsb);
    /*print B+tree information */
    bt->printAll();

    if((ycsb_read = fopen("/home/YCSB_DATA2/099/c_6M_16_run","r")) == NULL)
    {
        perror("fail to read");
    }

    thread_queue* run_queue[thread_num];
    int move[thread_num];
    for(t = 0; t < thread_num; t ++){
        run_queue[t] = (thread_queue*)calloc((all_num/thread_num)+1, sizeof(thread_queue));
        move[t] = 0;
    }

	int operation_num = 0;
  	for(i=0;i<all_num;i++)
    {	
        getline(&buf,&len,ycsb_read);
        if(strncmp(buf, "insert", 6) == 0){
            memcpy(key, buf+19, 8);
            run_queue[operation_num%thread_num][move[operation_num%thread_num]].key = atol((char*)key);
            run_queue[operation_num%thread_num][move[operation_num%thread_num]].operation = 1;
            move[operation_num%thread_num] ++;
        }
        else if(strncmp(buf, "read", 4) == 0){
            memcpy(key, buf+17, 8);
            run_queue[operation_num%thread_num][move[operation_num%thread_num]].key = atol((char*)key);
            run_queue[operation_num%thread_num][move[operation_num%thread_num]].operation = 0;
            move[operation_num%thread_num] ++;
        }else if(strncmp(buf,"update",6)==0){
            memcpy(key, buf+19, 8);
            run_queue[operation_num%thread_num][move[operation_num%thread_num]].key = atol((char*)key);
            run_queue[operation_num%thread_num][move[operation_num%thread_num]].operation = 2;
            move[operation_num%thread_num] ++;
        }
        operation_num ++;
    }
	fclose(ycsb_read);
    
	sub_thread* THREADS = (sub_thread*)malloc(sizeof(sub_thread)*thread_num);
    inserted = 0;
	
    clock_gettime(CLOCK_MONOTONIC, &start);	
    for(t = 0; t < thread_num; t++){
        THREADS[t].id = t;
        THREADS[t].root = bt;
        THREADS[t].inserted = 0;
        THREADS[t].read = 0;
        THREADS[t].update = 0;
        THREADS[t].op_num = all_num;
        THREADS[t].run_queue = run_queue[t];
        THREADS[t].thread_num = thread_num;
        pthread_create(&THREADS[t].thread, NULL, ycsb_thread_run, (void*)&THREADS[t]);
    }

    for(t = 0; t < thread_num; t++){
        pthread_join(THREADS[t].thread, NULL);
    }

	clock_gettime(CLOCK_MONOTONIC, &finish);
	single_time = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    for(t = 0; t < thread_num; ++t){
        inserted +=  THREADS[t].inserted;
    }
    int readed=0;int updated=0;
    for(t = 0; t < thread_num; ++t){
        readed +=  THREADS[t].read;
    }
    for(t = 0; t < thread_num; ++t){
        updated +=  THREADS[t].update;
    }
    //printf("Run phase finishes: %d/%d/%d items are inserted/searched/update\n", inserted, readed ,updated );
    printf("Run phase throughput: %f operations per second \n", all_num/single_time);	
    printf("Run Latency is %f ns \n", (single_time*1000000000.0)/all_num);
    return 0;
}
