#include <libvmem.h>
#include <libpmem.h>
#include <string.h>
#include <iostream>
#include <cstdio>
#include <bits/stdc++.h>
using namespace std;
#define VMEM_POOL 1024*1024*1024*1L // POOL size
VMEM *vmp;
void pm_pool_init(){
    if ((vmp = vmem_create("/pmem6/btree_pool",
					VMEM_POOL)) == NULL) {
		perror("vmem_create");
		exit(1); 
	}
}

#define SIZE 150000

int main(){
    pm_pool_init();
    cout << "pm init " << endl;
    char * dram_buffer = (char*)malloc(sizeof(char)*SIZE);
    char * pm_buffer = (char*)vmem_malloc(vmp, sizeof(char)*SIZE); // alloc space from PM

    struct timespec start, finish;
    double time1, time2;

    clock_gettime(CLOCK_MONOTONIC, &start);
    memset(dram_buffer, 'x', SIZE);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    time1 = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("DRAM write: %.4f", time1);

    char xx;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < SIZE; i++) {
        xx = *(dram_buffer[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    time1 = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("DRAM read: %.4f", time1);

    // cout << "alloc buffer from pm" << endl;
    clock_gettime(CLOCK_MONOTONIC, &start);
    pmem_memcpy_persist(pm_buffer, dram_buffer, SIZE); // write to PM, and persist
    clock_gettime(CLOCK_MONOTONIC, &finish);
    time2 = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("PM write: %.4f", time2);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < SIZE; i++) {
        xx = *(pm_buffer[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    time2 = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("PM read: %.4f", time2);

    // cout << "copy data to pm buffer, which contains 1024 " << pm_buffer[0] << endl;
    vmem_free(vmp, pm_buffer);
    cout << "free buffer from pm " << endl;
    /*other PM func
     *  man libpmem, man libvmem
     *  man pmem_memcpy, pmem_memmove
     */
    return 0;
}
