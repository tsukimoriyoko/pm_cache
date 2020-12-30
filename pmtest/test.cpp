#include <libvmem.h>
#include <libpmem.h>
#include <string.h>
#include <iostream>
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
int main(){
    pm_pool_init();
    cout << "pm init " << endl;
    char* dram_buffer = (char*)malloc(sizeof(char)*1024);
    char* pm_buffer = (char*)vmem_malloc(vmp, sizeof(char)*1024); // alloc space from PM
    memset(dram_buffer, 'x', 1024);
    cout << "alloc buffer from pm " << endl;
    pmem_memcpy_persist(pm_buffer, dram_buffer, 1024); // write to PM, and persist
    cout << "copy data to pm buffer, which contains 1024 " << pm_buffer[0] << endl;
    vmem_free(vmp, pm_buffer);
    cout << "free buffer from pm " << endl;
    /*other PM func
     *  man libpmem, man libvmem
     *  man pmem_memcpy, pmem_memmove
     */
    return 0;
}
