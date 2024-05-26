#include "memory_management.h"

MemoryBlock memory_blocks[MAX_MEM_BLOCKS]; // 메모리 분할 배열
int next_id = 1; //random으로?

FreeSpace free_spaces[MAX_MEM_BLOCKS];
int n_free_spaces = 0;

void init_partitions() {
    for (int i = 0; i < MAX_MEM_BLOCKS; i++) {
        memory_blocks[i].addr = NULL;
        memory_blocks[i].block_id = 0;
        memory_blocks[i].in_use_flag = 0;
    }
}

int fixedsize_memory_management() {
    init_partitions();
    
    printf("----- Print partitions after init ----- \n");
    print_memory_blocks();

    //메모리 할당 시도
    void *mem1 = fixed_allocate_memory();
    if (mem1 != NULL) {
        printf("Allocated memory at %p\n", mem1);
    }

    printf("----- Print partitions after allocation ----- \n");
    print_memory_blocks();

    //메모리 해제
    fixed_free_memory(mem1);
    printf("Memory at %p freed\n", mem1);

    printf("----- Print partitions after free ----- \n");
    print_memory_blocks();

    return 0;
}

void* fixed_allocate_memory() {
    for(int i = 0; i < MAX_MEM_BLOCKS; i++) {
        if(!memory_blocks[i].in_use_flag) {
            memory_blocks[i].addr = malloc(PARTITION_SIZE);
            fprintf(stderr, "Try to allocate memory for partitions[%d]\n", i);
            if (!memory_blocks[i].addr) { //할당 실패
               fprintf(stderr, "Failed to allocate memory for partition[%d]\n", i);
               return NULL;
            }
            memset(memory_blocks[i].addr, 0, PARTITION_SIZE); //memory_blocks[i]에 0으로 세팅. 메모리 크기 한 조각의 크기
            return memory_blocks[i].addr;
        }
    }
    //여기 까지오면 에러. 잘 실행된거면 for문에서 리턴 됐어야함.
    fprintf(stderr, "No available memory partitions\n");
    return NULL;
}

void fixed_free_memory(void *memory) {
    for(int i = 0; i < MAX_MEM_BLOCKS; i++) {
        if (memory_blocks[i].addr == memory) {
            free(memory_blocks[i].addr);
            memory_blocks[i].addr = NULL;
            return;
        } 
    }
    fprintf(stderr, "Attempted to free a non-allocated partition\n");
    return;
}

//사이즈를 받고 할당하고 id 리턴
int dyna_alloc(size_t size,  Process process) {
    int n_blocks = size / PARTITION_SIZE;
    size_t last_block_size = size % PARTITION_SIZE;
    if (last_block_size != 0){
        n_blocks++;
    } else {
        last_block_size = PARTITION_SIZE; //남은게 없으면 마지막 블록의 크기는 전체 크기로
    }

    update_free_spaces();
    int best_idx = -1;
    int min_diff = INT_MAX;

    for(int i = 0; i < n_free_spaces; i++){
        if(free_spaces[i].free_size >= n_blocks) {
            int diff = free_spaces[i].free_size - n_blocks;
            if(min_diff > diff) {
                min_diff = diff;
                best_idx = free_spaces[i].start_idx;
            }
        }
    }
    printf("\n");
    printf("Best index: %d\n", best_idx);
    if(best_idx == -1) {
        return -1;
    }
    
    int assigned_id = next_id++;
    for(int k = 0; k < n_blocks; k++) {
        size_t alloc_size;
        if(k != n_blocks - 1) { //마지막 블록이 아닐 때
            alloc_size = PARTITION_SIZE;
        } else {
            alloc_size = last_block_size;
        }
        memory_blocks[best_idx + k].addr = malloc(alloc_size);
        if(!memory_blocks[best_idx + k].addr) { 
            //할당 실패시 이전 반복에서 할당한 메모리 해제
            for(int l = 0; l < k; l++){
                free(memory_blocks[best_idx + l].addr);
                // memory_blocks[best_idx + l].process = NULL; 할당 해제시 process를 빼야하는데 NULL이 안됨.
                memory_blocks[best_idx + l].block_id = 0;
                memory_blocks[best_idx + l].addr = NULL;
                memory_blocks[best_idx + l].in_use_flag = 0;
                memory_blocks[best_idx + l].size = 0;
            }
            return -1;
        }
        memory_blocks[best_idx + k].process = process;
        memory_blocks[best_idx + k].block_id = assigned_id;
        memory_blocks[best_idx + k].in_use_flag = 1;
        memory_blocks[best_idx + k].size = alloc_size;
        memset(memory_blocks[best_idx + k].addr, 0, alloc_size);
    }
    return assigned_id;
}

//id받아서 할당
void dyna_free(int free_id) {
    // int found = 0;

    for(int i = 0; i < MAX_MEM_BLOCKS; i++) {
        if(memory_blocks[i].in_use_flag && memory_blocks[i].block_id == free_id) {
            free(memory_blocks[i].addr);
            memory_blocks[i].addr = NULL;
            memory_blocks[i].block_id = 0;
            memory_blocks[i].in_use_flag = 0;
            memory_blocks[i].size = 0;
            // found = 1;
        }
    }
    // if(found) {
    //     fprintf(stderr, "Freed allocated partition\n");
    //     print_memory_blocks();
    // }
    // else {
    //     fprintf(stderr, "Attempted to free a non-allocated partition\n");
    // }
}

void update_free_spaces() {
    n_free_spaces = 0;
    int i = 0;
    while(i < MAX_MEM_BLOCKS) {
        if(!memory_blocks[i].in_use_flag) {
            int start = i;
            while(i < MAX_MEM_BLOCKS && !memory_blocks[i].in_use_flag) {
                i++;
            }
            free_spaces[n_free_spaces].start_idx = start;
            free_spaces[n_free_spaces].free_size = i - start;
            n_free_spaces++;
        } else{
            i++;
        }
    }
}

void print_memory_blocks() {
    printf("Memory Blocks Status:\n");
    for (int i = MAX_MEM_BLOCKS - 1; i >= 0 ; i--) {
        if (memory_blocks[i].in_use_flag) {  // 메모리 블록이 사용 중인지 확인
            printf("Block %d: Process_ID = %d, Block_ID=%d, Size=%zu, Address=%p\n", 
            i, memory_blocks[i].process.pid, memory_blocks[i].block_id, memory_blocks[i].size, memory_blocks[i].addr);
        } else {
            printf("Block %d: UNUSED\n", i);  // 사용되지 않는 블록 표시
        }
    }
}
